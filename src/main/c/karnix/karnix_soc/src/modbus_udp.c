#include "utils.h"
#include "lwip/inet.h"
#include "lwip/udp.h"
#include "lwip/ip4_frag.h"
#include "lwip/netif.h"
#include "lwip/init.h"
#include "lwip/stats.h"
#include "netif/etharp.h"
#include "soc.h"
#include "gpio.h"
#include "config.h"
#include "crc16.h"
//#include "aes.h"
#include "modbus.h"

//#define MODBUS_DEBUG	1

#define	MODBUS_UDP_PORT	2001
	
struct udp_pcb* modbus_udp_pcb = NULL;


void modbus_udp_recv(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr, u16_t port);
void modbus_udp_send(const ip_addr_t *addr, u16_t port, uint8_t* buf, uint32_t buf_len);

int modbus_udp_init(void) {

	err_t ret;

	/* LWIP_ASSERT_CORE_LOCKED(); is checked by udp_new() */
	modbus_udp_pcb = udp_new_ip_type(IPADDR_TYPE_ANY);

	if(modbus_udp_pcb == NULL) {
		printf("modbus_udp_init() failed to udp_new_ip_type()\r\n");
		return ERR_MEM;
	}


	if((ret = udp_bind(modbus_udp_pcb, IP_ANY_TYPE, MODBUS_UDP_PORT)) != ERR_OK) {
		printf("modbus_udp_init() failed to udp_bind()\r\n");
		udp_remove(modbus_udp_pcb);
		return ret;
	}

	udp_recv(modbus_udp_pcb, modbus_udp_recv, NULL);

	printf("modbus_udp_init() done\r\n");

	return ERR_OK;
}


void modbus_udp_recv(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr, u16_t port) {

	uint8_t *rx_buf = (uint8_t *) p->payload;
	uint32_t rx_len = p->len;
	uint32_t tx_len = 0;
	uint8_t tx_buf[MODBUS_TX_BUF_SIZE];

	#ifdef MODBUS_DEBUG
	printf("modbus_udp_recv() from %s:%d, size = %d\r\n", ipaddr_ntoa(addr), port, rx_len);
	#endif

	LWIP_UNUSED_ARG(arg);
	LWIP_UNUSED_ARG(upcb);


	tx_len = modbus_recv(rx_buf, rx_len, tx_buf);

	pbuf_free(p);

	if(tx_len > 0) {
		#ifdef MODBUS_DEBUG
		printf("modbus_udp_recv() responding with %d bytes\r\n", tx_len);

		for(int i = 0; i < tx_len; i++)
			printf("0x%02X ", tx_buf[i]);
		printf("\r\n");
		#endif

		modbus_udp_send(addr, port, tx_buf, tx_len);
	}


	return;
}



void modbus_udp_send(const ip_addr_t *addr, u16_t port, uint8_t* buf, uint32_t buf_len) {

	struct pbuf *p;
	uint8_t *payload;
	err_t err;

	if((p = pbuf_alloc(PBUF_TRANSPORT, (u16_t)(buf_len + 1), PBUF_RAM)) == NULL) {
		GPIO->OUTPUT &= ~GPIO_OUT_LED2; // Error indicator
		printf("modbus_udp_send() error while in pbuf_alloc()\r\n");
		return;
	}

	if(p->payload == NULL || p->len < buf_len + 1) {
		GPIO->OUTPUT &= ~GPIO_OUT_LED2; // Error indicator
		printf("modbus_udp_send() bogus p->payload = %p, p->len = %d\r\n", p->payload, p->len);
		return;
	}

	payload = (uint8_t*)p->payload;

	MEMCPY(payload, buf, buf_len);

	payload[buf_len] = 0;

	if((err = udp_sendto(modbus_udp_pcb, p, addr, port)) != ERR_OK) {
		GPIO->OUTPUT &= ~GPIO_OUT_LED2; // Error indicator
		printf("modbus_udp_send() error while in udp_sendto() = %d\r\n", err);
	} else {
		#ifdef MODBUS_DEBUG
		printf("modbus_udp_send() sent %d bytes to %s:%d\r\n", p->len, ipaddr_ntoa(addr), port);
		#endif
	}

	pbuf_free(p);

}



