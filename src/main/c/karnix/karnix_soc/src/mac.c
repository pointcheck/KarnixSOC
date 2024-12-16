#include <stdio.h>
#include "soc.h"
#include "riscv.h"
#include "mac.h"
#include "plic.h"
#include "utils.h"
#include "lwip/inet.h"
#include "lwip/sys.h"
#include "lwip/tcp.h"
#include "lwip/ip4_frag.h"
#include "lwip/netif.h"
#include "lwip/init.h"
#include "lwip/stats.h"
#include "lwip/dhcp.h"
#include "netif/etharp.h"
#include "config.h"
#include "utils.h"


//#define	MAC_DEBUG	1

// Below are needed for LWIP stack

uint32_t ts_etharp = 0;
uint32_t ts_tcp = 0;
uint32_t ts_ipreass = 0;
uint32_t ts_dhcp_fine = 0;
uint32_t ts_dhcp_coarse = 0;

struct netif default_netif; // Our network interface structure

err_t mac_tx_packet(struct netif *netif, struct pbuf *p);
void mac_rx_packet(void);

uint32_t sys_now(void) {
	return get_mtime() / 1000; // return current timestamp in ms
}

sys_prot_t sys_arch_protect(void) {
	//csr_clear(mstatus, MSTATUS_MIE); // Disable Machine interrupts
}

void sys_arch_unprotect(sys_prot_t pval) {
	//csr_set(mstatus, MSTATUS_MIE); // Enable Machine interrupts
}


// This function is called from LWIP during netif_add() interface initialization procedure

int mac_init(struct netif* iface) {

	//printf("mac_init() begin\r\n");

	if(iface == NULL) {
		printf("mac_init() iface is NULL !!!\r\n");
		return ERR_ARG;
	}

	memcpy(iface->name, "eth0", 5);

	iface->output = etharp_output;		// We use default ethernet parsing fuction for received packets
	iface->linkoutput = mac_tx_packet;	// We use our own MAC tx function to transmit packets
  

	// Configure hardware interface
	iface->hwaddr_len = ETHARP_HWADDR_LEN;
	memcpy(iface->hwaddr, active_config.mac_addr, iface->hwaddr_len);
	iface->mtu = 1500;
	iface->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP;

	// Init MAC controller
	mac_setCtrl(MAC, MAC_CTRL_TX_RESET | MAC_CTRL_RX_RESET);
	delay_us(10000);
	mac_setCtrl(MAC, 0);

	printf("mac_init() done\r\n");

	return ERR_OK;
}


int mac_lwip_init(void)
{
	//printf("mac_lwip_init() begin\r\n");

	// Convert IP strings into binary
	/*
	if (!inet_aton(DEFAULT_IP_ADDR, &active_config.ip_addr))
		return -1;
	if (!inet_aton(DEFAULT_GATEWAY, &active_config.gw_addr))
		return -1;
	if (!inet_aton(DEFAULT_NETMASK, &active_config.netmask))
		return -1;
	*/

	// Initialize LWIP
	lwip_init();

	// Init timers
	ts_etharp = ts_ipreass = sys_now();


	// Add netif to LWIP (netif_add calls our driver initialization function)
	if (netif_add(&default_netif, &active_config.ip_addr, &active_config.netmask, &active_config.gw_addr, NULL, mac_init, ethernet_input) == NULL) {
		printf("mac_lwip_init() netif_add failed!\r\n");
        	return -1;
	}

	netif_set_default(&default_netif);
	netif_set_up(&default_netif);

	if(active_config.options & CONFIG_OPTION_USE_DHCP)
		dhcp_start(&default_netif);
	
	//printf("mac_lwip_init() done, ARP_TMR_INTERVAL = %d ms, IP_TMR_INTERVAL = %d ms\r\n", ARP_TMR_INTERVAL, IP_TMR_INTERVAL);

	return 0;
}


// Call this once every 1 ms to d LWIP house-keeping
void mac_poll(void)
{
	uint32_t ts_now = sys_now();


	//printf("mac_poll() ts_now = %d, ts_etharp = %d, ts_ipreass = %d\r\n", ts_now, ts_etharp, ts_ipreass);

	if(ts_now <= ts_etharp || ts_now <= ts_ipreass) {
		//printf("mac_poll() timer overlap!\r\n");
		ts_etharp = ts_ipreass = ts_now;
		return;
	}

	// Process lwip network-related timers.

	#if LPFW_ARP
	if(ts_now - ts_etharp >= ARP_TMR_INTERVAL) {
		etharp_tmr();
		ts_etharp = ts_now;
	}
	#endif

	#if IPFW_TCP
	if(ts_now - ts_tcp >= TCP_TMR_INTERVAL) {
		tcp_tmr();
		ts_tcp = ts_now;
	}
	#endif

	#if IP_REASSEMBLY
	if(ts_now - ts_ipreass >= IP_TMR_INTERVAL) {
		ip_reass_tmr();
		ts_ipreass = ts_now;
	}
	#endif

	#if LWIP_DHCP
	if(ts_now - ts_dhcp_fine >= DHCP_FINE_TIMER_MSECS) {
		dhcp_fine_tmr();
		ts_dhcp_fine = ts_now;
	}
	if(ts_now - ts_dhcp_coarse >= DHCP_COARSE_TIMER_SECS*1000) {
		dhcp_coarse_tmr();
		ts_dhcp_coarse = ts_now;
	}
	#endif

	if(PLIC->IRQLINE & 0x04) {
		//printf("mac_poll() stuck RX packet...\r\n");
		mac_rx();
	}

}



void mac_rx(void) {

	struct pbuf *p, *q;
 	u16_t buffer_len;
	uint32_t bits;
	uint32_t words;
	int32_t bytes_left; 
	uint32_t q_left = 0;
	uint32_t frags = 0;
	uint32_t byteId;
	uint32_t word;
	uint8_t *payload;

	//print("mac_rx() begin\r\n");

	while(mac_rxPending(MAC)) {

		bits = mac_getRx(MAC);
		words = (bits+31)/32;
		bytes_left = (bits+7)/8;

		//print("mac_rx() reading %d bytes (%d bits)\r\n", bytes_left, bits);

		if(bytes_left > 2048) {
			printf("mac_rx() RX FIFO error, bytes_left = %d bytes (%d bits)\r\n", bytes_left, bits);
			fflush(stdout);

			// Reset MAC
			mac_setCtrl(MAC, MAC_CTRL_TX_RESET | MAC_CTRL_RX_RESET);
			delay_us(10000);
			mac_setCtrl(MAC, 0);

			return;
		}

		buffer_len = bytes_left;

		#if ETH_PAD_SIZE
		buffer_len += ETH_PAD_SIZE; /* allow room for Ethernet padding */
		#endif

		//printf("mac_rx() pbuf_alloc = %d, bytes_left = %d, bits = %d, words = %d\r\n", buffer_len, bytes_left, bits, words);

		p = pbuf_alloc(PBUF_RAW, buffer_len, PBUF_POOL);
  
		if(p == NULL) {
			GPIO->OUTPUT |= GPIO_OUT_LED2; // Error indicator

			printf("mac_rx() pbuf_alloc(%d) error, bytes_left = %d, bits = %d, words = %d !!!\r\n", buffer_len, bytes_left, bits, words);
			LINK_STATS_INC(link.memerr);
			LINK_STATS_INC(link.drop);

			// Empty FIFO
			while(bytes_left > 0) {
				while(mac_rxPending(MAC) == 0);
				mac_getRx(MAC);
				bytes_left -= 4;
			}

			printf("mac_rx() RX FIFO cleared\r\n");
			return;
		}


		#if ETH_PAD_SIZE
		pbuf_header(p, -ETH_PAD_SIZE); /* drop the padding word */
		#endif

		//printf("mac_rx() bytes_left = %d, p = %p, p->len = %d, p->next = %p\r\n", bytes_left, p, p->len, p->next);

		q = NULL;
		byteId = 0;
		frags = 0;

		while(bytes_left) {

			if(q_left == 0) {
				if(q == NULL) {
					q = p;
				} else {
					q = q->next;

					if(q == NULL) {
						GPIO->OUTPUT |= GPIO_OUT_LED2; // Error indicator

						print("mac_rx() not enough space in pbuf!\r\n");

						LINK_STATS_INC(link.memerr);
						LINK_STATS_INC(link.drop);


						// Empty FIFO
						while(bytes_left > 0) {
							while(mac_rxPending(MAC) == 0);
							mac_getRx(MAC);
							bytes_left -= 4;
						}

						#if ETH_PAD_SIZE
						pbuf_header(p, ETH_PAD_SIZE); /* reclaim the padding word */
						#endif

						goto skip_rx_packet;
					}
				}

				q_left = q->len;

				payload = q->payload;

				frags++;

				//printf("\r\nmac_rx() q_left = %d, payload = %p\r\n", q_left, payload);
			}
		
			if(byteId == 0) {
				while(mac_rxPending(MAC) == 0);
				word = mac_getRx(MAC);
			}

			*payload = (uint8_t) word;
			word = word >> 8;

			//printf("%02X ", (uint8_t)*payload);

			byteId = (byteId + 1) & 0x03;

			bytes_left--;
			payload++;
			q_left--;
		}


		payload = (uint8_t*) p->payload;

		#if MAC_DEBUG
		printf("mac_rx() %02x:%02x:%02x:%02x:%02x:%02x <- %02x:%02x:%02x:%02x:%02x:%02x type: 0x%02x%02x, fragments = %d, bytes_left = %d, q_left = %d, words = %d, packet_len = %d, last word = 0x%08x, byteId = %d\r\n", 
			payload[0], payload[1], payload[2], payload[3], payload[4], payload[5],
			payload[6], payload[7], payload[8], payload[9], payload[10], payload[11],
			payload[12], payload[13],
			frags, bytes_left, q_left, words, bits / 8, word, byteId);
		#endif


						
		#if ETH_PAD_SIZE
		pbuf_header(p, ETH_PAD_SIZE); /* reclaim the padding word */
		#endif

		LINK_STATS_INC(link.recv);

		int rc;

		if((rc = default_netif.input(p, &default_netif)) != ERR_OK) {
			GPIO->OUTPUT |= GPIO_OUT_LED2; // Error indicator
			printf("mac_rx() default_netif.input() error %d\r\n", rc);
		}


		skip_rx_packet:

		pbuf_free(p);
	}

	//printf("mac_rx() done\r\n");
}


err_t mac_tx_packet(struct netif *netif, struct pbuf *p) {
	struct pbuf *q;

	//printf("mac_tx_packet() begin\r\n");
  
	if(netif == NULL || p == NULL) {
		GPIO->OUTPUT |= GPIO_OUT_LED2; // Error indicator
		//printf("mac_tx_packet() wrong args, netif = %p, p = %p\r\n", netif, p);
		return ERR_ARG;
	}

	#if ETH_PAD_SIZE
	pbuf_header(p, -ETH_PAD_SIZE); /* drop the padding word */
	#endif


	// calculate packet len by summing up all fragments
	uint32_t frame_size = 0;
	uint32_t frame_frags = 0;
	for(q = p; q != NULL; q = q->next) {
		frame_size += q->len;
		frame_frags++;
	}

        uint32_t bits = frame_size*8;
        uint32_t words = (bits+31)/32;

	//printf("mac_tx_packet() frame_size = %d, frame_frags = %d\r\n", frame_size, frame_frags);
	
	// wait for MAC controller to get ready to send
        while(!mac_txReady(MAC));

	// wait for space in TX FIFO
	while(mac_writeAvailability(MAC) == 0);

        mac_pushTx(MAC, bits); // first word in FIFO is number of bits in following packet

	// send packet by sending all its fragments
	uint32_t byte_idx = 0;
	uint32_t word = 0;
	uint32_t words_sent = 0;
	uint8_t *payload;
	uint32_t q_left;

	for(q = p; q != NULL; q = q->next) {
		/* Send the data from the pbuf to the interface, one pbuf at a
		time. The size of the data in each pbuf is kept in the ->len
		variable. */
		//send data from(q->payload, q->len);

        	payload = (uint8_t*) q->payload;
		q_left = q->len;

		//printf("mac_tx_packet() q = %p, q_left = %d, payload = %p\r\n", q, q_left, payload);

		while(q_left) {
			

			word |= ((*payload) & 0xff) << (byte_idx * 8);

			if(byte_idx == 3) {
        			while(mac_writeAvailability(MAC) == 0);
				mac_pushTx(MAC, word);
				word = 0;
				words_sent++;
			}

			byte_idx = (byte_idx + 1) & 0x03;

			//printf("%02X ", *payload);

			payload++;
			q_left--;
		}

		//printf("\r\n");
	}

	// Write remaining tail
	if(byte_idx != 0) {
        	while(mac_writeAvailability(MAC) == 0);
		mac_pushTx(MAC, word);

		words_sent++;

		//printf("mac_tx_packet() word = %08X\r\n", word);
	}

	payload = (uint8_t*) p->payload;

	#if MAC_DEBUG
	printf("mac_tx_packet() %02x:%02x:%02x:%02x:%02x:%02x <- %02x:%02x:%02x:%02x:%02x:%02x type: 0x%02x%02x, byte_idx = %d, words sent = %d, frame_size = %d, frame_frags = %d\r\n", 
		payload[0], payload[1], payload[2], payload[3], payload[4], payload[5],
		payload[6], payload[7], payload[8], payload[9], payload[10], payload[11],
		payload[12], payload[13],
		byte_idx, words_sent, frame_size, frame_frags);
	#endif


	#if ETH_PAD_SIZE
	pbuf_header(p, ETH_PAD_SIZE); /* reclaim the padding word */
	#endif
  
	LINK_STATS_INC(link.xmit);

	//printf("mac_tx_packet() done\r\n");

	return ERR_OK;
}


void mac_print_stats(void) {

	#ifdef LINK_STATS
	char str[256];
	snprintf(str, 256, "NETLINK: ip_addr = %s, rx = %d, tx = %d, err = %d, drop = %d, lenerr = %d, chkerr = %d\r\n", 
		inet_ntoa(default_netif.ip_addr),
		lwip_stats.link.recv,
		lwip_stats.link.xmit,
		lwip_stats.link.err,
		lwip_stats.link.drop,
		lwip_stats.link.lenerr,
		lwip_stats.link.chkerr
	);
	print(str);
	#else
	print("mac_print_stats() NO STATs\r\n", 
	#endif


}

//    MDIO
//    uint32_t clkDivider = BSP_CLINT_HZ/(1000000*2)-1;
//
//    Spi_Config spiCfg;
//    spiCfg.cpol = 0;
//    spiCfg.cpha = 0;
//    spiCfg.mode = 1;
//    spiCfg.clkDivider = clkDivider;
//    spiCfg.ssSetup = clkDivider;
//    spiCfg.ssHold = clkDivider;
//    spiCfg.ssDisable = clkDivider;
//    spi_applyConfig(SPI, &spiCfg);
//
//    gpio_setOutputEnable(RESETN_GPIO, RESETN_PIN);
//
//    uint32_t address = 0;
//    while(1){
//        mdio_init(SPI,SPI_CS);
//
//        u16 control = mdio_read(SPI, SPI_CS, 1, 0);
//        u16 status = mdio_read(SPI, SPI_CS, 1, 1);
//        u16 id1 = mdio_read(SPI, SPI_CS, 1, 2);
//        u16 id2 = mdio_read(SPI, SPI_CS, 1, 3);
//        mdio_write(SPI, SPI_CS, 1, 0, control | 0x0200);
//        while(1){
//            control = mdio_read(SPI, SPI_CS, 1, 1);
//            bsp_uDelay(50000);
//        }
//
//        asm("nop");
//    }

