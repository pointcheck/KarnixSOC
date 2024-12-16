
build/lwip_test.elf:     file format elf32-littleriscv


Disassembly of section ._vector:

80000000 <crtStart>:

    .section	.start_jump,"ax",@progbits
crtStart:
  //long jump to allow crtInit to be anywhere
  //do it always in 12 bytes
  lui x2,       %hi(crtInit)
80000000:	80000137          	lui	sp,0x80000
  addi x2, x2,  %lo(crtInit)
80000004:	0b010113          	addi	sp,sp,176 # 800000b0 <_stack_start+0xffff9320>
  jalr x1,x2
80000008:	000100e7          	jalr	sp
  nop
8000000c:	00000013          	nop
	...

80000020 <trap_entry>:
.section .text

.global  trap_entry
.align 5
trap_entry:
  sw x1,  - 1*4(sp)
80000020:	fe112e23          	sw	ra,-4(sp)
  sw x5,  - 2*4(sp)
80000024:	fe512c23          	sw	t0,-8(sp)
  sw x6,  - 3*4(sp)
80000028:	fe612a23          	sw	t1,-12(sp)
  sw x7,  - 4*4(sp)
8000002c:	fe712823          	sw	t2,-16(sp)
  sw x10, - 5*4(sp)
80000030:	fea12623          	sw	a0,-20(sp)
  sw x11, - 6*4(sp)
80000034:	feb12423          	sw	a1,-24(sp)
  sw x12, - 7*4(sp)
80000038:	fec12223          	sw	a2,-28(sp)
  sw x13, - 8*4(sp)
8000003c:	fed12023          	sw	a3,-32(sp)
  sw x14, - 9*4(sp)
80000040:	fce12e23          	sw	a4,-36(sp)
  sw x15, -10*4(sp)
80000044:	fcf12c23          	sw	a5,-40(sp)
  sw x16, -11*4(sp)
80000048:	fd012a23          	sw	a6,-44(sp)
  sw x17, -12*4(sp)
8000004c:	fd112823          	sw	a7,-48(sp)
  sw x28, -13*4(sp)
80000050:	fdc12623          	sw	t3,-52(sp)
  sw x29, -14*4(sp)
80000054:	fdd12423          	sw	t4,-56(sp)
  sw x30, -15*4(sp)
80000058:	fde12223          	sw	t5,-60(sp)
  sw x31, -16*4(sp)
8000005c:	fdf12023          	sw	t6,-64(sp)
  addi sp,sp,-16*4
80000060:	fc010113          	addi	sp,sp,-64
  call irqCallback
80000064:	544000ef          	jal	ra,800005a8 <irqCallback>
  lw x1 , 15*4(sp)
80000068:	03c12083          	lw	ra,60(sp)
  lw x5,  14*4(sp)
8000006c:	03812283          	lw	t0,56(sp)
  lw x6,  13*4(sp)
80000070:	03412303          	lw	t1,52(sp)
  lw x7,  12*4(sp)
80000074:	03012383          	lw	t2,48(sp)
  lw x10, 11*4(sp)
80000078:	02c12503          	lw	a0,44(sp)
  lw x11, 10*4(sp)
8000007c:	02812583          	lw	a1,40(sp)
  lw x12,  9*4(sp)
80000080:	02412603          	lw	a2,36(sp)
  lw x13,  8*4(sp)
80000084:	02012683          	lw	a3,32(sp)
  lw x14,  7*4(sp)
80000088:	01c12703          	lw	a4,28(sp)
  lw x15,  6*4(sp)
8000008c:	01812783          	lw	a5,24(sp)
  lw x16,  5*4(sp)
80000090:	01412803          	lw	a6,20(sp)
  lw x17,  4*4(sp)
80000094:	01012883          	lw	a7,16(sp)
  lw x28,  3*4(sp)
80000098:	00c12e03          	lw	t3,12(sp)
  lw x29,  2*4(sp)
8000009c:	00812e83          	lw	t4,8(sp)
  lw x30,  1*4(sp)
800000a0:	00412f03          	lw	t5,4(sp)
  lw x31,  0*4(sp)
800000a4:	00012f83          	lw	t6,0(sp)
  addi sp,sp,16*4
800000a8:	04010113          	addi	sp,sp,64
  mret
800000ac:	30200073          	mret

800000b0 <crtInit>:


crtInit:
  .option push
  .option norelax
  la gp, __global_pointer$
800000b0:	00001197          	auipc	gp,0x1
800000b4:	a9818193          	addi	gp,gp,-1384 # 80000b48 <__global_pointer$>
  .option pop
  la sp, _stack_start
800000b8:	00007117          	auipc	sp,0x7
800000bc:	cd810113          	addi	sp,sp,-808 # 80006d90 <_stack_start>

800000c0 <bss_init>:

bss_init:
  la a0, _bss_start
800000c0:	85c18513          	addi	a0,gp,-1956 # 800003a4 <mch_myip_addr>
  la a1, _bss_end
800000c4:	a5418593          	addi	a1,gp,-1452 # 8000059c <_bss_end>

800000c8 <bss_loop>:
bss_loop:
  beq a0,a1,bss_done
800000c8:	00b50863          	beq	a0,a1,800000d8 <bss_done>
  sw zero,0(a0)
800000cc:	00052023          	sw	zero,0(a0)
  add a0,a0,4
800000d0:	00450513          	addi	a0,a0,4
  j bss_loop
800000d4:	ff5ff06f          	j	800000c8 <bss_loop>

800000d8 <bss_done>:
bss_done:

ctors_init:
  la a0, _ctors_start
800000d8:	00006517          	auipc	a0,0x6
800000dc:	4b850513          	addi	a0,a0,1208 # 80006590 <_ctors_end>
  addi sp,sp,-4
800000e0:	ffc10113          	addi	sp,sp,-4

800000e4 <ctors_loop>:
ctors_loop:
  la a1, _ctors_end
800000e4:	00006597          	auipc	a1,0x6
800000e8:	4ac58593          	addi	a1,a1,1196 # 80006590 <_ctors_end>
  beq a0,a1,ctors_done
800000ec:	00b50e63          	beq	a0,a1,80000108 <ctors_done>
  lw a3,0(a0)
800000f0:	00052683          	lw	a3,0(a0)
  add a0,a0,4
800000f4:	00450513          	addi	a0,a0,4
  sw a0,0(sp)
800000f8:	00a12023          	sw	a0,0(sp)
  jalr  a3
800000fc:	000680e7          	jalr	a3
  lw a0,0(sp)
80000100:	00012503          	lw	a0,0(sp)
  j ctors_loop
80000104:	fe1ff06f          	j	800000e4 <ctors_loop>

80000108 <ctors_done>:
ctors_done:
  addi sp,sp,4
80000108:	00410113          	addi	sp,sp,4


  li a0, 0x880     //880 enable timer + external interrupts
8000010c:	00001537          	lui	a0,0x1
80000110:	88050513          	addi	a0,a0,-1920 # 880 <_stack_size+0x80>
  csrw mie,a0
80000114:	30451073          	csrw	mie,a0
  li a0, 0x1808     //1808 enable interrupts
80000118:	00002537          	lui	a0,0x2
8000011c:	80850513          	addi	a0,a0,-2040 # 1808 <_stack_size+0x1008>
  csrw mstatus,a0
80000120:	30051073          	csrw	mstatus,a0

  call main
80000124:	040060ef          	jal	ra,80006164 <end>

80000128 <infinitLoop>:
infinitLoop:
  j infinitLoop
80000128:	0000006f          	j	80000128 <infinitLoop>
	...

Disassembly of section .memory:

800005a0 <mchdrv_init>:
static struct netif mchdrv_netif;

// Init function
int mchdrv_init(struct netif *iface) {
	return 1;
}
800005a0:	00100513          	li	a0,1
800005a4:	00008067          	ret

800005a8 <irqCallback>:
}
800005a8:	00008067          	ret

800005ac <mchdrv_poll>:
// Poll for received frames
void mchdrv_poll(struct netif *iface) {
	return;
}
800005ac:	00008067          	ret

800005b0 <sys_now>:

u32_t sys_now(void) {
	return 0; // return current timestamp
}
800005b0:	00000513          	li	a0,0
800005b4:	00008067          	ret

800005b8 <sys_arch_protect>:
800005b8:	00008067          	ret

800005bc <sys_arch_unprotect>:
800005bc:	00008067          	ret

800005c0 <mch_net_init>:
	return 0;
}
*/

int mch_net_init(void)
{
800005c0:	fe010113          	addi	sp,sp,-32
800005c4:	01212823          	sw	s2,16(sp)
    void * mchdrvnet_priv;
    uint8_t mac_addr[6];
    int err = -1;

    // Hard-coded IP for my address, gateway and netmask
    if (inet_aton(MCH_IPADDR_BASE, &mch_myip_addr))
800005c8:	80000537          	lui	a0,0x80000
800005cc:	85c18593          	addi	a1,gp,-1956 # 800003a4 <mch_myip_addr>
800005d0:	13450513          	addi	a0,a0,308 # 80000134 <_stack_start+0xffff93a4>
{
800005d4:	00112e23          	sw	ra,28(sp)
800005d8:	00812c23          	sw	s0,24(sp)
800005dc:	00912a23          	sw	s1,20(sp)
    if (inet_aton(MCH_IPADDR_BASE, &mch_myip_addr))
800005e0:	5fc050ef          	jal	ra,80005bdc <ip4addr_aton>
800005e4:	02050263          	beqz	a0,80000608 <mch_net_init+0x48>
        return -1;
800005e8:	fff00413          	li	s0,-1
    netif_set_default(&mchdrv_netif);
    netif_set_up(&mchdrv_netif);


    return 0;
}
800005ec:	00040513          	mv	a0,s0
800005f0:	01c12083          	lw	ra,28(sp)
800005f4:	01812403          	lw	s0,24(sp)
800005f8:	01412483          	lw	s1,20(sp)
800005fc:	01012903          	lw	s2,16(sp)
80000600:	02010113          	addi	sp,sp,32
80000604:	00008067          	ret
    if (inet_aton(MCH_IPADDR_GW, &gw_addr))
80000608:	80000537          	lui	a0,0x80000
8000060c:	00810593          	addi	a1,sp,8
80000610:	14450513          	addi	a0,a0,324 # 80000144 <_stack_start+0xffff93b4>
80000614:	5c8050ef          	jal	ra,80005bdc <ip4addr_aton>
80000618:	fc0518e3          	bnez	a0,800005e8 <mch_net_init+0x28>
    if (inet_aton(MCH_IPADDR_NETMASK, &netmask))
8000061c:	80000537          	lui	a0,0x80000
80000620:	00c10593          	addi	a1,sp,12
80000624:	15450513          	addi	a0,a0,340 # 80000154 <_stack_start+0xffff93c4>
80000628:	5b4050ef          	jal	ra,80005bdc <ip4addr_aton>
8000062c:	00050413          	mv	s0,a0
80000630:	fa051ce3          	bnez	a0,800005e8 <mch_net_init+0x28>
    lwip_init();
80000634:	28c000ef          	jal	ra,800008c0 <lwip_init>
    if (netif_add(&mchdrv_netif, &mch_myip_addr, &netmask, &gw_addr, mchdrvnet_priv,
80000638:	b4c18813          	addi	a6,gp,-1204 # 80000694 <ethernet_input>
8000063c:	a5818793          	addi	a5,gp,-1448 # 800005a0 <mchdrv_init>
80000640:	00000713          	li	a4,0
80000644:	00810693          	addi	a3,sp,8
80000648:	00c10613          	addi	a2,sp,12
8000064c:	85c18593          	addi	a1,gp,-1956 # 800003a4 <mch_myip_addr>
80000650:	88818513          	addi	a0,gp,-1912 # 800003d0 <mchdrv_netif>
80000654:	4a5000ef          	jal	ra,800012f8 <netif_add>
80000658:	f80508e3          	beqz	a0,800005e8 <mch_net_init+0x28>
    netif_set_default(&mchdrv_netif);
8000065c:	88818513          	addi	a0,gp,-1912 # 800003d0 <mchdrv_netif>
80000660:	5bd000ef          	jal	ra,8000141c <netif_set_default>
    netif_set_up(&mchdrv_netif);
80000664:	88818513          	addi	a0,gp,-1912 # 800003d0 <mchdrv_netif>
80000668:	5bd000ef          	jal	ra,80001424 <netif_set_up>
    return 0;
8000066c:	f81ff06f          	j	800005ec <mch_net_init+0x2c>

80000670 <mch_net_poll>:
80000670:	00008067          	ret

80000674 <mch_net_aton>:
//
// Convert address from string to internal format.
// Return 0 on success; else non-zero
//
int mch_net_aton(char * str_addr, ip4_addr_t * net_addr)
{
80000674:	ff010113          	addi	sp,sp,-16
80000678:	00112623          	sw	ra,12(sp)
    struct in_addr a;
    int i = inet_aton(str_addr, &net_addr->addr);
8000067c:	560050ef          	jal	ra,80005bdc <ip4addr_aton>
    if (!i)
        return -1;
    return 0;
}
80000680:	00c12083          	lw	ra,12(sp)
    if (!i)
80000684:	00153513          	seqz	a0,a0
}
80000688:	40a00533          	neg	a0,a0
8000068c:	01010113          	addi	sp,sp,16
80000690:	00008067          	ret

80000694 <ethernet_input>:
 * @see ETHARP_SUPPORT_VLAN
 * @see LWIP_HOOK_VLAN_CHECK
 */
err_t
ethernet_input(struct pbuf *p, struct netif *netif)
{
80000694:	ff010113          	addi	sp,sp,-16
80000698:	00812423          	sw	s0,8(sp)
8000069c:	00112623          	sw	ra,12(sp)
800006a0:	00912223          	sw	s1,4(sp)
800006a4:	01212023          	sw	s2,0(sp)
  u16_t next_hdr_offset = SIZEOF_ETH_HDR;
#endif /* LWIP_ARP || ETHARP_SUPPORT_VLAN */

  LWIP_ASSERT_CORE_LOCKED();

  if (p->len <= SIZEOF_ETH_HDR) {
800006a8:	00a55703          	lhu	a4,10(a0)
800006ac:	01000793          	li	a5,16
{
800006b0:	00050413          	mv	s0,a0
  if (p->len <= SIZEOF_ETH_HDR) {
800006b4:	00e7ec63          	bltu	a5,a4,800006cc <ethernet_input+0x38>
#ifdef LWIP_HOOK_UNKNOWN_ETH_PROTOCOL
      if (LWIP_HOOK_UNKNOWN_ETH_PROTOCOL(p, netif) == ERR_OK) {
        break;
      }
#endif
      ETHARP_STATS_INC(etharp.proterr);
800006b8:	8dc18793          	addi	a5,gp,-1828 # 80000424 <lwip_stats>
800006bc:	0107d703          	lhu	a4,16(a5)
800006c0:	00170713          	addi	a4,a4,1
800006c4:	00e79823          	sh	a4,16(a5)
800006c8:	0a00006f          	j	80000768 <ethernet_input+0xd4>
  if (p->if_idx == NETIF_NO_INDEX) {
800006cc:	00f54783          	lbu	a5,15(a0)
800006d0:	00058913          	mv	s2,a1
800006d4:	00079863          	bnez	a5,800006e4 <ethernet_input+0x50>
    p->if_idx = netif_get_index(netif);
800006d8:	0345c783          	lbu	a5,52(a1)
800006dc:	00178793          	addi	a5,a5,1
800006e0:	00f507a3          	sb	a5,15(a0)
  ethhdr = (struct eth_hdr *)p->payload;
800006e4:	00442503          	lw	a0,4(s0)
  type = ethhdr->type;
800006e8:	00f54483          	lbu	s1,15(a0)
800006ec:	00e54783          	lbu	a5,14(a0)
800006f0:	00849493          	slli	s1,s1,0x8
800006f4:	00f4e4b3          	or	s1,s1,a5
  if (ethhdr->dest.addr[0] & 1) {
800006f8:	00254783          	lbu	a5,2(a0)
800006fc:	0017f713          	andi	a4,a5,1
80000700:	02070663          	beqz	a4,8000072c <ethernet_input+0x98>
    if (ethhdr->dest.addr[0] == LL_IP4_MULTICAST_ADDR_0) {
80000704:	00100713          	li	a4,1
80000708:	06e79c63          	bne	a5,a4,80000780 <ethernet_input+0xec>
      if ((ethhdr->dest.addr[1] == LL_IP4_MULTICAST_ADDR_1) &&
8000070c:	00354783          	lbu	a5,3(a0)
80000710:	00079e63          	bnez	a5,8000072c <ethernet_input+0x98>
80000714:	00454703          	lbu	a4,4(a0)
80000718:	05e00793          	li	a5,94
8000071c:	00f71863          	bne	a4,a5,8000072c <ethernet_input+0x98>
        p->flags |= PBUF_FLAG_LLMCAST;
80000720:	00d44783          	lbu	a5,13(s0)
80000724:	0107e793          	ori	a5,a5,16
      p->flags |= PBUF_FLAG_LLBCAST;
80000728:	00f406a3          	sb	a5,13(s0)
  switch (type) {
8000072c:	00800793          	li	a5,8
80000730:	06f48863          	beq	s1,a5,800007a0 <ethernet_input+0x10c>
80000734:	60800793          	li	a5,1544
80000738:	f8f490e3          	bne	s1,a5,800006b8 <ethernet_input+0x24>
      if (!(netif->flags & NETIF_FLAG_ETHARP)) {
8000073c:	03194783          	lbu	a5,49(s2)
80000740:	0087f793          	andi	a5,a5,8
80000744:	02078863          	beqz	a5,80000774 <ethernet_input+0xe0>
      if (pbuf_remove_header(p, next_hdr_offset)) {
80000748:	01000593          	li	a1,16
8000074c:	00040513          	mv	a0,s0
80000750:	178010ef          	jal	ra,800018c8 <pbuf_remove_header>
80000754:	08050863          	beqz	a0,800007e4 <ethernet_input+0x150>
        ETHARP_STATS_INC(etharp.lenerr);
80000758:	8dc18793          	addi	a5,gp,-1828 # 80000424 <lwip_stats>
8000075c:	00a7d703          	lhu	a4,10(a5)
80000760:	00170713          	addi	a4,a4,1
80000764:	00e79523          	sh	a4,10(a5)
      ETHARP_STATS_INC(etharp.drop);
80000768:	0067d703          	lhu	a4,6(a5)
8000076c:	00170713          	addi	a4,a4,1
80000770:	00e79323          	sh	a4,6(a5)
  /* This means the pbuf is freed or consumed,
     so the caller doesn't have to free it again */
  return ERR_OK;

free_and_return:
  pbuf_free(p);
80000774:	00040513          	mv	a0,s0
80000778:	1d8010ef          	jal	ra,80001950 <pbuf_free>
  return ERR_OK;
8000077c:	04c0006f          	j	800007c8 <ethernet_input+0x134>
    else if (eth_addr_cmp(&ethhdr->dest, &ethbroadcast)) {
80000780:	00600613          	li	a2,6
80000784:	81018593          	addi	a1,gp,-2032 # 80000358 <ethbroadcast>
80000788:	00250513          	addi	a0,a0,2
8000078c:	201050ef          	jal	ra,8000618c <memcmp>
80000790:	f8051ee3          	bnez	a0,8000072c <ethernet_input+0x98>
      p->flags |= PBUF_FLAG_LLBCAST;
80000794:	00d44783          	lbu	a5,13(s0)
80000798:	0087e793          	ori	a5,a5,8
8000079c:	f8dff06f          	j	80000728 <ethernet_input+0x94>
      if (!(netif->flags & NETIF_FLAG_ETHARP)) {
800007a0:	03194783          	lbu	a5,49(s2)
800007a4:	0087f793          	andi	a5,a5,8
800007a8:	fc0786e3          	beqz	a5,80000774 <ethernet_input+0xe0>
      if (pbuf_remove_header(p, next_hdr_offset)) {
800007ac:	01000593          	li	a1,16
800007b0:	00040513          	mv	a0,s0
800007b4:	114010ef          	jal	ra,800018c8 <pbuf_remove_header>
800007b8:	fa051ee3          	bnez	a0,80000774 <ethernet_input+0xe0>
        ip4_input(p, netif);
800007bc:	00090593          	mv	a1,s2
800007c0:	00040513          	mv	a0,s0
800007c4:	48d040ef          	jal	ra,80005450 <ip4_input>
}
800007c8:	00c12083          	lw	ra,12(sp)
800007cc:	00812403          	lw	s0,8(sp)
800007d0:	00412483          	lw	s1,4(sp)
800007d4:	00012903          	lw	s2,0(sp)
800007d8:	00000513          	li	a0,0
800007dc:	01010113          	addi	sp,sp,16
800007e0:	00008067          	ret
        etharp_input(p, netif);
800007e4:	00090593          	mv	a1,s2
800007e8:	00040513          	mv	a0,s0
800007ec:	248030ef          	jal	ra,80003a34 <etharp_input>
      break;
800007f0:	fd9ff06f          	j	800007c8 <ethernet_input+0x134>

800007f4 <ethernet_output>:
 * @return ERR_OK if the packet was sent, any other err_t on failure
 */
err_t
ethernet_output(struct netif * netif, struct pbuf * p,
                const struct eth_addr * src, const struct eth_addr * dst,
                u16_t eth_type) {
800007f4:	fd010113          	addi	sp,sp,-48
800007f8:	01312e23          	sw	s3,28(sp)
800007fc:	00050993          	mv	s3,a0
  struct eth_hdr *ethhdr;
  u16_t eth_type_be = lwip_htons(eth_type);
80000800:	00070513          	mv	a0,a4
                u16_t eth_type) {
80000804:	02912223          	sw	s1,36(sp)
80000808:	03212023          	sw	s2,32(sp)
8000080c:	01412c23          	sw	s4,24(sp)
80000810:	00058913          	mv	s2,a1
80000814:	02112623          	sw	ra,44(sp)
80000818:	02812423          	sw	s0,40(sp)
8000081c:	00060a13          	mv	s4,a2
80000820:	00d12623          	sw	a3,12(sp)
  u16_t eth_type_be = lwip_htons(eth_type);
80000824:	0c4000ef          	jal	ra,800008e8 <lwip_htons>
80000828:	00050493          	mv	s1,a0

    eth_type_be = PP_HTONS(ETHTYPE_VLAN);
  } else
#endif /* ETHARP_SUPPORT_VLAN && defined(LWIP_HOOK_VLAN_SET) */
  {
    if (pbuf_add_header(p, SIZEOF_ETH_HDR) != 0) {
8000082c:	01000593          	li	a1,16
80000830:	00090513          	mv	a0,s2
80000834:	084010ef          	jal	ra,800018b8 <pbuf_add_header>
80000838:	06051263          	bnez	a0,8000089c <ethernet_output+0xa8>
    }
  }

  LWIP_ASSERT_CORE_LOCKED();

  ethhdr = (struct eth_hdr *)p->payload;
8000083c:	00492403          	lw	s0,4(s2)
  ethhdr->type = eth_type_be;
  SMEMCPY(&ethhdr->dest, dst, ETH_HWADDR_LEN);
80000840:	00c12683          	lw	a3,12(sp)
80000844:	00600613          	li	a2,6
  ethhdr->type = eth_type_be;
80000848:	00940723          	sb	s1,14(s0)
8000084c:	0084d493          	srli	s1,s1,0x8
80000850:	009407a3          	sb	s1,15(s0)
  SMEMCPY(&ethhdr->dest, dst, ETH_HWADDR_LEN);
80000854:	00068593          	mv	a1,a3
80000858:	00240513          	addi	a0,s0,2
8000085c:	161050ef          	jal	ra,800061bc <memcpy>
  SMEMCPY(&ethhdr->src,  src, ETH_HWADDR_LEN);
80000860:	000a0593          	mv	a1,s4
80000864:	00840513          	addi	a0,s0,8
80000868:	00600613          	li	a2,6
8000086c:	151050ef          	jal	ra,800061bc <memcpy>
pbuf_header_failed:
  LWIP_DEBUGF(ETHARP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_LEVEL_SERIOUS,
              ("ethernet_output: could not allocate room for header.\n"));
  LINK_STATS_INC(link.lenerr);
  return ERR_BUF;
}
80000870:	02812403          	lw	s0,40(sp)
  return netif->linkoutput(netif, p);
80000874:	0189a303          	lw	t1,24(s3)
}
80000878:	02c12083          	lw	ra,44(sp)
8000087c:	02412483          	lw	s1,36(sp)
80000880:	01812a03          	lw	s4,24(sp)
  return netif->linkoutput(netif, p);
80000884:	00090593          	mv	a1,s2
80000888:	00098513          	mv	a0,s3
}
8000088c:	02012903          	lw	s2,32(sp)
80000890:	01c12983          	lw	s3,28(sp)
80000894:	03010113          	addi	sp,sp,48
  return netif->linkoutput(netif, p);
80000898:	00030067          	jr	t1
}
8000089c:	02c12083          	lw	ra,44(sp)
800008a0:	02812403          	lw	s0,40(sp)
800008a4:	02412483          	lw	s1,36(sp)
800008a8:	02012903          	lw	s2,32(sp)
800008ac:	01c12983          	lw	s3,28(sp)
800008b0:	01812a03          	lw	s4,24(sp)
800008b4:	ffe00513          	li	a0,-2
800008b8:	03010113          	addi	sp,sp,48
800008bc:	00008067          	ret

800008c0 <lwip_init>:
 * Initialize all modules.
 * Use this in NO_SYS mode. Use tcpip_init() otherwise.
 */
void
lwip_init(void)
{
800008c0:	ff010113          	addi	sp,sp,-16
800008c4:	00112623          	sw	ra,12(sp)
#ifndef LWIP_SKIP_PACKING_CHECK
  LWIP_ASSERT("Struct packing not implemented correctly. Check your lwIP port.", sizeof(struct packed_struct_test) == PACKED_STRUCT_TEST_EXPECTED_SIZE);
#endif

  /* Modules initialization */
  stats_init();
800008c8:	164020ef          	jal	ra,80002a2c <stats_init>
#if !NO_SYS
  sys_init();
#endif /* !NO_SYS */
  mem_init();
800008cc:	6f4000ef          	jal	ra,80000fc0 <mem_init>
  memp_init();
800008d0:	770000ef          	jal	ra,80001040 <memp_init>
  pbuf_init();
  netif_init();
800008d4:	0f1000ef          	jal	ra,800011c4 <netif_init>
#endif /* LWIP_IPV4 */
#if LWIP_RAW
  raw_init();
#endif /* LWIP_RAW */
#if LWIP_UDP
  udp_init();
800008d8:	414020ef          	jal	ra,80002cec <udp_init>
#endif

#if LWIP_TIMERS
  sys_timeouts_init();
#endif /* LWIP_TIMERS */
}
800008dc:	00c12083          	lw	ra,12(sp)
800008e0:	01010113          	addi	sp,sp,16
  sys_timeouts_init();
800008e4:	2600206f          	j	80002b44 <sys_timeouts_init>

800008e8 <lwip_htons>:
 * @return n in network byte order
 */
u16_t
lwip_htons(u16_t n)
{
  return PP_HTONS(n);
800008e8:	00851793          	slli	a5,a0,0x8
800008ec:	00855513          	srli	a0,a0,0x8
800008f0:	00a7e533          	or	a0,a5,a0
}
800008f4:	01051513          	slli	a0,a0,0x10
800008f8:	01055513          	srli	a0,a0,0x10
800008fc:	00008067          	ret

80000900 <lwip_htonl>:
 * @return n in network byte order
 */
u32_t
lwip_htonl(u32_t n)
{
  return PP_HTONL(n);
80000900:	01855713          	srli	a4,a0,0x18
80000904:	01851793          	slli	a5,a0,0x18
80000908:	00e7e7b3          	or	a5,a5,a4
8000090c:	00ff06b7          	lui	a3,0xff0
80000910:	00851713          	slli	a4,a0,0x8
80000914:	00d77733          	and	a4,a4,a3
80000918:	00e7e7b3          	or	a5,a5,a4
8000091c:	00010737          	lui	a4,0x10
80000920:	f0070713          	addi	a4,a4,-256 # ff00 <_stack_size+0xf700>
80000924:	00855513          	srli	a0,a0,0x8
80000928:	00e57533          	and	a0,a0,a4
}
8000092c:	00a7e533          	or	a0,a5,a0
80000930:	00008067          	ret

80000934 <lwip_strnstr>:
 * lwIP default implementation for strnstr() non-standard function.
 * This can be \#defined to strnstr() depending on your platform port.
 */
char *
lwip_strnstr(const char *buffer, const char *token, size_t n)
{
80000934:	fe010113          	addi	sp,sp,-32
80000938:	00812c23          	sw	s0,24(sp)
8000093c:	00050413          	mv	s0,a0
  const char *p;
  size_t tokenlen = strlen(token);
80000940:	00058513          	mv	a0,a1
{
80000944:	00912a23          	sw	s1,20(sp)
80000948:	01312623          	sw	s3,12(sp)
8000094c:	00112e23          	sw	ra,28(sp)
80000950:	01212823          	sw	s2,16(sp)
80000954:	00058993          	mv	s3,a1
80000958:	00060493          	mv	s1,a2
  size_t tokenlen = strlen(token);
8000095c:	195050ef          	jal	ra,800062f0 <strlen>
  if (tokenlen == 0) {
80000960:	02050063          	beqz	a0,80000980 <lwip_strnstr+0x4c>
80000964:	00050913          	mv	s2,a0
    return LWIP_CONST_CAST(char *, buffer);
  }
  for (p = buffer; *p && (p + tokenlen <= buffer + n); p++) {
80000968:	009404b3          	add	s1,s0,s1
8000096c:	00044783          	lbu	a5,0(s0)
80000970:	00078663          	beqz	a5,8000097c <lwip_strnstr+0x48>
80000974:	01240733          	add	a4,s0,s2
80000978:	02e4f463          	bgeu	s1,a4,800009a0 <lwip_strnstr+0x6c>
    if ((*p == *token) && (strncmp(p, token, tokenlen) == 0)) {
      return LWIP_CONST_CAST(char *, p);
    }
  }
  return NULL;
8000097c:	00000413          	li	s0,0
}
80000980:	00040513          	mv	a0,s0
80000984:	01c12083          	lw	ra,28(sp)
80000988:	01812403          	lw	s0,24(sp)
8000098c:	01412483          	lw	s1,20(sp)
80000990:	01012903          	lw	s2,16(sp)
80000994:	00c12983          	lw	s3,12(sp)
80000998:	02010113          	addi	sp,sp,32
8000099c:	00008067          	ret
    if ((*p == *token) && (strncmp(p, token, tokenlen) == 0)) {
800009a0:	0009c703          	lbu	a4,0(s3)
800009a4:	00f71c63          	bne	a4,a5,800009bc <lwip_strnstr+0x88>
800009a8:	00090613          	mv	a2,s2
800009ac:	00098593          	mv	a1,s3
800009b0:	00040513          	mv	a0,s0
800009b4:	159050ef          	jal	ra,8000630c <strncmp>
800009b8:	fc0504e3          	beqz	a0,80000980 <lwip_strnstr+0x4c>
  for (p = buffer; *p && (p + tokenlen <= buffer + n); p++) {
800009bc:	00140413          	addi	s0,s0,1
800009c0:	fadff06f          	j	8000096c <lwip_strnstr+0x38>

800009c4 <lwip_stricmp>:
  do {
    c1 = *str1++;
    c2 = *str2++;
    if (c1 != c2) {
      char c1_upc = c1 | 0x20;
      if ((c1_upc >= 'a') && (c1_upc <= 'z')) {
800009c4:	01900813          	li	a6,25
    c1 = *str1++;
800009c8:	00150513          	addi	a0,a0,1
    c2 = *str2++;
800009cc:	00158593          	addi	a1,a1,1
    c1 = *str1++;
800009d0:	fff54703          	lbu	a4,-1(a0)
    c2 = *str2++;
800009d4:	fff5c783          	lbu	a5,-1(a1)
    if (c1 != c2) {
800009d8:	00f70e63          	beq	a4,a5,800009f4 <lwip_stricmp+0x30>
      char c1_upc = c1 | 0x20;
800009dc:	02076613          	ori	a2,a4,32
      if ((c1_upc >= 'a') && (c1_upc <= 'z')) {
800009e0:	f9f60693          	addi	a3,a2,-97
800009e4:	0ff6f693          	andi	a3,a3,255
800009e8:	00d86c63          	bltu	a6,a3,80000a00 <lwip_stricmp+0x3c>
        /* characters are not equal an one is in the alphabet range:
        downcase both chars and check again */
        char c2_upc = c2 | 0x20;
        if (c1_upc != c2_upc) {
800009ec:	0207e793          	ori	a5,a5,32
800009f0:	00f61863          	bne	a2,a5,80000a00 <lwip_stricmp+0x3c>
      } else {
        /* characters are not equal but none is in the alphabet range */
        return 1;
      }
    }
  } while (c1 != 0);
800009f4:	fc071ae3          	bnez	a4,800009c8 <lwip_stricmp+0x4>
  return 0;
800009f8:	00000513          	li	a0,0
800009fc:	00008067          	ret
        return 1;
80000a00:	00100513          	li	a0,1
}
80000a04:	00008067          	ret

80000a08 <lwip_strnicmp>:
 * lwIP default implementation for strnicmp() non-standard function.
 * This can be \#defined to strnicmp() depending on your platform port.
 */
int
lwip_strnicmp(const char *str1, const char *str2, size_t len)
{
80000a08:	fff60613          	addi	a2,a2,-1
80000a0c:	00000793          	li	a5,0
  do {
    c1 = *str1++;
    c2 = *str2++;
    if (c1 != c2) {
      char c1_upc = c1 | 0x20;
      if ((c1_upc >= 'a') && (c1_upc <= 'z')) {
80000a10:	01900313          	li	t1,25
    c1 = *str1++;
80000a14:	00f50733          	add	a4,a0,a5
80000a18:	00074683          	lbu	a3,0(a4)
    c2 = *str2++;
80000a1c:	00f58733          	add	a4,a1,a5
80000a20:	00074703          	lbu	a4,0(a4)
    if (c1 != c2) {
80000a24:	00e68e63          	beq	a3,a4,80000a40 <lwip_strnicmp+0x38>
      char c1_upc = c1 | 0x20;
80000a28:	0206e893          	ori	a7,a3,32
      if ((c1_upc >= 'a') && (c1_upc <= 'z')) {
80000a2c:	f9f88813          	addi	a6,a7,-97
80000a30:	0ff87813          	andi	a6,a6,255
80000a34:	03036063          	bltu	t1,a6,80000a54 <lwip_strnicmp+0x4c>
        /* characters are not equal an one is in the alphabet range:
        downcase both chars and check again */
        char c2_upc = c2 | 0x20;
        if (c1_upc != c2_upc) {
80000a38:	02076713          	ori	a4,a4,32
80000a3c:	00e89c63          	bne	a7,a4,80000a54 <lwip_strnicmp+0x4c>
        /* characters are not equal but none is in the alphabet range */
        return 1;
      }
    }
    len--;
  } while ((len != 0) && (c1 != 0));
80000a40:	00f60663          	beq	a2,a5,80000a4c <lwip_strnicmp+0x44>
80000a44:	00178793          	addi	a5,a5,1
80000a48:	fc0696e3          	bnez	a3,80000a14 <lwip_strnicmp+0xc>
  return 0;
80000a4c:	00000513          	li	a0,0
}
80000a50:	00008067          	ret
        return 1;
80000a54:	00100513          	li	a0,1
80000a58:	00008067          	ret

80000a5c <lwip_itoa>:
 * lwIP default implementation for itoa() non-standard function.
 * This can be \#defined to itoa() or snprintf(result, bufsize, "%d", number) depending on your platform port.
 */
void
lwip_itoa(char *result, size_t bufsize, int number)
{
80000a5c:	fe010113          	addi	sp,sp,-32
80000a60:	01212823          	sw	s2,16(sp)
80000a64:	01412423          	sw	s4,8(sp)
80000a68:	00112e23          	sw	ra,28(sp)
80000a6c:	00812c23          	sw	s0,24(sp)
80000a70:	00912a23          	sw	s1,20(sp)
80000a74:	01312623          	sw	s3,12(sp)
  char *res = result;
  char *tmp = result + bufsize - 1;
  int n = (number >= 0) ? number : -number;

  /* handle invalid bufsize */
  if (bufsize < 2) {
80000a78:	00100793          	li	a5,1
{
80000a7c:	00050913          	mv	s2,a0
80000a80:	00058a13          	mv	s4,a1
  if (bufsize < 2) {
80000a84:	02b7e663          	bltu	a5,a1,80000ab0 <lwip_itoa+0x54>
    if (bufsize == 1) {
80000a88:	00f59463          	bne	a1,a5,80000a90 <lwip_itoa+0x34>
      *result = 0;
80000a8c:	00090023          	sb	zero,0(s2)
    *res++ = 0;
    return;
  }
  /* move from temporary buffer to output buffer (sign is not moved) */
  memmove(res, tmp, (size_t)((result + bufsize) - tmp));
}
80000a90:	01c12083          	lw	ra,28(sp)
80000a94:	01812403          	lw	s0,24(sp)
80000a98:	01412483          	lw	s1,20(sp)
80000a9c:	01012903          	lw	s2,16(sp)
80000aa0:	00c12983          	lw	s3,12(sp)
80000aa4:	00812a03          	lw	s4,8(sp)
80000aa8:	02010113          	addi	sp,sp,32
80000aac:	00008067          	ret
  if (number < 0) {
80000ab0:	00050993          	mv	s3,a0
80000ab4:	00065863          	bgez	a2,80000ac4 <lwip_itoa+0x68>
    *res++ = '-';
80000ab8:	02d00793          	li	a5,45
80000abc:	00150993          	addi	s3,a0,1
80000ac0:	00f50023          	sb	a5,0(a0)
  int n = (number >= 0) ? number : -number;
80000ac4:	41f65793          	srai	a5,a2,0x1f
  char *tmp = result + bufsize - 1;
80000ac8:	fffa0413          	addi	s0,s4,-1
80000acc:	00890433          	add	s0,s2,s0
  int n = (number >= 0) ? number : -number;
80000ad0:	00c7c4b3          	xor	s1,a5,a2
80000ad4:	40f484b3          	sub	s1,s1,a5
  *tmp = 0;
80000ad8:	00040023          	sb	zero,0(s0)
  while ((n != 0) && (tmp > res)) {
80000adc:	04048263          	beqz	s1,80000b20 <lwip_itoa+0xc4>
80000ae0:	fa89f6e3          	bgeu	s3,s0,80000a8c <lwip_itoa+0x30>
    char val = (char)('0' + (n % 10));
80000ae4:	00a00593          	li	a1,10
80000ae8:	00048513          	mv	a0,s1
80000aec:	648050ef          	jal	ra,80006134 <__modsi3>
80000af0:	03050513          	addi	a0,a0,48
    tmp--;
80000af4:	fff40413          	addi	s0,s0,-1
    *tmp = val;
80000af8:	00a40023          	sb	a0,0(s0)
    n = n / 10;
80000afc:	00a00593          	li	a1,10
80000b00:	00048513          	mv	a0,s1
80000b04:	5ac050ef          	jal	ra,800060b0 <__divsi3>
80000b08:	00050493          	mv	s1,a0
80000b0c:	fd1ff06f          	j	80000adc <lwip_itoa+0x80>
    *res++ = '0';
80000b10:	03000793          	li	a5,48
80000b14:	00f98023          	sb	a5,0(s3)
    *res++ = 0;
80000b18:	000980a3          	sb	zero,1(s3)
    return;
80000b1c:	f75ff06f          	j	80000a90 <lwip_itoa+0x34>
  if (*tmp == 0) {
80000b20:	00044783          	lbu	a5,0(s0)
80000b24:	fe0786e3          	beqz	a5,80000b10 <lwip_itoa+0xb4>
  memmove(res, tmp, (size_t)((result + bufsize) - tmp));
80000b28:	01490633          	add	a2,s2,s4
80000b2c:	40860633          	sub	a2,a2,s0
80000b30:	00040593          	mv	a1,s0
}
80000b34:	01812403          	lw	s0,24(sp)
80000b38:	01c12083          	lw	ra,28(sp)
80000b3c:	01412483          	lw	s1,20(sp)
80000b40:	01012903          	lw	s2,16(sp)
80000b44:	00812a03          	lw	s4,8(sp)
  memmove(res, tmp, (size_t)((result + bufsize) - tmp));
80000b48:	00098513          	mv	a0,s3
}
80000b4c:	00c12983          	lw	s3,12(sp)
80000b50:	02010113          	addi	sp,sp,32
  memmove(res, tmp, (size_t)((result + bufsize) - tmp));
80000b54:	7400506f          	j	80006294 <memmove>

80000b58 <lwip_standard_chksum>:
 * @param len length of data to be summed
 * @return host order (!) lwip checksum (non-inverted Internet sum)
 */
u16_t
lwip_standard_chksum(const void *dataptr, int len)
{
80000b58:	ff010113          	addi	sp,sp,-16
  const u8_t *pb = (const u8_t *)dataptr;
  const u16_t *ps;
  u16_t t = 0;
80000b5c:	00011723          	sh	zero,14(sp)
  u32_t sum = 0;
  int odd = ((mem_ptr_t)pb & 1);
80000b60:	00157693          	andi	a3,a0,1

  /* Get aligned to u16_t */
  if (odd && len > 0) {
80000b64:	00068c63          	beqz	a3,80000b7c <lwip_standard_chksum+0x24>
80000b68:	00b05a63          	blez	a1,80000b7c <lwip_standard_chksum+0x24>
    ((u8_t *)&t)[1] = *pb++;
80000b6c:	00054783          	lbu	a5,0(a0)
    len--;
80000b70:	fff58593          	addi	a1,a1,-1
    ((u8_t *)&t)[1] = *pb++;
80000b74:	00150513          	addi	a0,a0,1
80000b78:	00f107a3          	sb	a5,15(sp)
  u32_t sum = 0;
80000b7c:	00000793          	li	a5,0
  }

  /* Add the bulk of the data */
  ps = (const u16_t *)(const void *)pb;
  while (len > 1) {
80000b80:	00100713          	li	a4,1
80000b84:	06b74063          	blt	a4,a1,80000be4 <lwip_standard_chksum+0x8c>
    sum += *ps++;
    len -= 2;
  }

  /* Consume left-over byte, if any */
  if (len > 0) {
80000b88:	00e59663          	bne	a1,a4,80000b94 <lwip_standard_chksum+0x3c>
    ((u8_t *)&t)[0] = *(const u8_t *)ps;
80000b8c:	00054703          	lbu	a4,0(a0)
80000b90:	00e10723          	sb	a4,14(sp)
  }

  /* Add end bytes */
  sum += t;
80000b94:	00e15503          	lhu	a0,14(sp)

  /* Fold 32-bit sum to 16 bits
     calling this twice is probably faster than if statements... */
  sum = FOLD_U32T(sum);
80000b98:	00010737          	lui	a4,0x10
80000b9c:	fff70713          	addi	a4,a4,-1 # ffff <_stack_size+0xf7ff>
  sum += t;
80000ba0:	00f50533          	add	a0,a0,a5
  sum = FOLD_U32T(sum);
80000ba4:	01055793          	srli	a5,a0,0x10
80000ba8:	00e57533          	and	a0,a0,a4
80000bac:	00a787b3          	add	a5,a5,a0
  sum = FOLD_U32T(sum);
80000bb0:	0107d513          	srli	a0,a5,0x10
80000bb4:	00e7f7b3          	and	a5,a5,a4
80000bb8:	00f50533          	add	a0,a0,a5

  /* Swap if alignment was odd */
  if (odd) {
80000bbc:	00068c63          	beqz	a3,80000bd4 <lwip_standard_chksum+0x7c>
    sum = SWAP_BYTES_IN_WORD(sum);
80000bc0:	00851793          	slli	a5,a0,0x8
80000bc4:	00855513          	srli	a0,a0,0x8
80000bc8:	00e7f733          	and	a4,a5,a4
80000bcc:	0ff57513          	andi	a0,a0,255
80000bd0:	00a76533          	or	a0,a4,a0
  }

  return (u16_t)sum;
}
80000bd4:	01051513          	slli	a0,a0,0x10
80000bd8:	01055513          	srli	a0,a0,0x10
80000bdc:	01010113          	addi	sp,sp,16
80000be0:	00008067          	ret
    sum += *ps++;
80000be4:	00250513          	addi	a0,a0,2
80000be8:	ffe55603          	lhu	a2,-2(a0)
    len -= 2;
80000bec:	ffe58593          	addi	a1,a1,-2
    sum += *ps++;
80000bf0:	00c787b3          	add	a5,a5,a2
    len -= 2;
80000bf4:	f91ff06f          	j	80000b84 <lwip_standard_chksum+0x2c>

80000bf8 <inet_chksum_pseudo>:
 * @return checksum (as u16_t) to be saved directly in the protocol header
 */
u16_t
inet_chksum_pseudo(struct pbuf *p, u8_t proto, u16_t proto_len,
                   const ip4_addr_t *src, const ip4_addr_t *dest)
{
80000bf8:	fe010113          	addi	sp,sp,-32
80000bfc:	00812c23          	sw	s0,24(sp)
80000c00:	00912a23          	sw	s1,20(sp)
80000c04:	01212823          	sw	s2,16(sp)
80000c08:	01312623          	sw	s3,12(sp)
80000c0c:	01412423          	sw	s4,8(sp)
80000c10:	01512223          	sw	s5,4(sp)
80000c14:	00112e23          	sw	ra,28(sp)
  u32_t addr;

  addr = ip4_addr_get_u32(src);
  acc = (addr & 0xffffUL);
  acc = (u32_t)(acc + ((addr >> 16) & 0xffffUL));
  addr = ip4_addr_get_u32(dest);
80000c18:	00072703          	lw	a4,0(a4)
  addr = ip4_addr_get_u32(src);
80000c1c:	0006a683          	lw	a3,0(a3) # ff0000 <_stack_size+0xfef800>
  acc = (u32_t)(acc + (addr & 0xffffUL));
80000c20:	000107b7          	lui	a5,0x10
80000c24:	fff78793          	addi	a5,a5,-1 # ffff <_stack_size+0xf7ff>
80000c28:	00f77433          	and	s0,a4,a5
  acc = (u32_t)(acc + ((addr >> 16) & 0xffffUL));
80000c2c:	01075713          	srli	a4,a4,0x10
80000c30:	00e40433          	add	s0,s0,a4
  acc = (addr & 0xffffUL);
80000c34:	00f6f733          	and	a4,a3,a5
80000c38:	00e40433          	add	s0,s0,a4
  acc = (u32_t)(acc + ((addr >> 16) & 0xffffUL));
80000c3c:	0106d693          	srli	a3,a3,0x10
  acc = (u32_t)(acc + ((addr >> 16) & 0xffffUL));
80000c40:	008686b3          	add	a3,a3,s0
  /* fold down to 16 bits */
  acc = FOLD_U32T(acc);
80000c44:	0106d413          	srli	s0,a3,0x10
80000c48:	00f6f6b3          	and	a3,a3,a5
80000c4c:	00d406b3          	add	a3,s0,a3
  acc = FOLD_U32T(acc);
80000c50:	0106d413          	srli	s0,a3,0x10
80000c54:	00f6f6b3          	and	a3,a3,a5
{
80000c58:	00050993          	mv	s3,a0
80000c5c:	00058a13          	mv	s4,a1
80000c60:	00060913          	mv	s2,a2
  acc = FOLD_U32T(acc);
80000c64:	00d40433          	add	s0,s0,a3
  int swapped = 0;
80000c68:	00000a93          	li	s5,0
    acc = FOLD_U32T(acc);
80000c6c:	00078493          	mv	s1,a5
  for (q = p; q != NULL; q = q->next) {
80000c70:	08099863          	bnez	s3,80000d00 <inet_chksum_pseudo+0x108>
  if (swapped) {
80000c74:	020a8063          	beqz	s5,80000c94 <inet_chksum_pseudo+0x9c>
    acc = SWAP_BYTES_IN_WORD(acc);
80000c78:	00010737          	lui	a4,0x10
80000c7c:	00841793          	slli	a5,s0,0x8
80000c80:	fff70713          	addi	a4,a4,-1 # ffff <_stack_size+0xf7ff>
80000c84:	00845413          	srli	s0,s0,0x8
80000c88:	00e7f7b3          	and	a5,a5,a4
80000c8c:	0ff47413          	andi	s0,s0,255
80000c90:	0087e433          	or	s0,a5,s0
  acc += (u32_t)lwip_htons((u16_t)proto);
80000c94:	000a0513          	mv	a0,s4
80000c98:	c51ff0ef          	jal	ra,800008e8 <lwip_htons>
80000c9c:	00050493          	mv	s1,a0
  acc += (u32_t)lwip_htons(proto_len);
80000ca0:	00090513          	mv	a0,s2
80000ca4:	c45ff0ef          	jal	ra,800008e8 <lwip_htons>
80000ca8:	00a48533          	add	a0,s1,a0
  acc = FOLD_U32T(acc);
80000cac:	000107b7          	lui	a5,0x10
  acc += (u32_t)lwip_htons(proto_len);
80000cb0:	00850433          	add	s0,a0,s0
  acc = FOLD_U32T(acc);
80000cb4:	fff78793          	addi	a5,a5,-1 # ffff <_stack_size+0xf7ff>
80000cb8:	01045513          	srli	a0,s0,0x10
80000cbc:	00f47433          	and	s0,s0,a5
80000cc0:	00850433          	add	s0,a0,s0
  acc = FOLD_U32T(acc);
80000cc4:	01045513          	srli	a0,s0,0x10
80000cc8:	00f47433          	and	s0,s0,a5
80000ccc:	00850433          	add	s0,a0,s0
  return (u16_t)~(acc & 0xffffUL);
80000cd0:	fff44513          	not	a0,s0

  return inet_cksum_pseudo_base(p, proto, proto_len, acc);
}
80000cd4:	01c12083          	lw	ra,28(sp)
80000cd8:	01812403          	lw	s0,24(sp)
80000cdc:	01051513          	slli	a0,a0,0x10
80000ce0:	01412483          	lw	s1,20(sp)
80000ce4:	01012903          	lw	s2,16(sp)
80000ce8:	00c12983          	lw	s3,12(sp)
80000cec:	00812a03          	lw	s4,8(sp)
80000cf0:	00412a83          	lw	s5,4(sp)
80000cf4:	01055513          	srli	a0,a0,0x10
80000cf8:	02010113          	addi	sp,sp,32
80000cfc:	00008067          	ret
    acc += LWIP_CHKSUM(q->payload, q->len);
80000d00:	00a9d583          	lhu	a1,10(s3)
80000d04:	0049a503          	lw	a0,4(s3)
80000d08:	e51ff0ef          	jal	ra,80000b58 <lwip_standard_chksum>
80000d0c:	00850433          	add	s0,a0,s0
    acc = FOLD_U32T(acc);
80000d10:	01045793          	srli	a5,s0,0x10
80000d14:	00947433          	and	s0,s0,s1
80000d18:	00878433          	add	s0,a5,s0
    if (q->len % 2 != 0) {
80000d1c:	00a9d783          	lhu	a5,10(s3)
80000d20:	0017f793          	andi	a5,a5,1
80000d24:	00078e63          	beqz	a5,80000d40 <inet_chksum_pseudo+0x148>
      acc = SWAP_BYTES_IN_WORD(acc);
80000d28:	00841793          	slli	a5,s0,0x8
80000d2c:	00845413          	srli	s0,s0,0x8
80000d30:	0097f7b3          	and	a5,a5,s1
80000d34:	0ff47413          	andi	s0,s0,255
80000d38:	001aca93          	xori	s5,s5,1
80000d3c:	0087e433          	or	s0,a5,s0
  for (q = p; q != NULL; q = q->next) {
80000d40:	0009a983          	lw	s3,0(s3)
80000d44:	f2dff06f          	j	80000c70 <inet_chksum_pseudo+0x78>

80000d48 <ip_chksum_pseudo>:
#if LWIP_IPV4 && LWIP_IPV6
  else
#endif /* LWIP_IPV4 && LWIP_IPV6 */
#if LWIP_IPV4
  {
    return inet_chksum_pseudo(p, proto, proto_len, ip_2_ip4(src), ip_2_ip4(dest));
80000d48:	eb1ff06f          	j	80000bf8 <inet_chksum_pseudo>

80000d4c <inet_chksum_pseudo_partial>:
 * @return checksum (as u16_t) to be saved directly in the protocol header
 */
u16_t
inet_chksum_pseudo_partial(struct pbuf *p, u8_t proto, u16_t proto_len,
                           u16_t chksum_len, const ip4_addr_t *src, const ip4_addr_t *dest)
{
80000d4c:	fd010113          	addi	sp,sp,-48
80000d50:	02812423          	sw	s0,40(sp)
80000d54:	02912223          	sw	s1,36(sp)
80000d58:	03212023          	sw	s2,32(sp)
80000d5c:	01412c23          	sw	s4,24(sp)
80000d60:	01512a23          	sw	s5,20(sp)
80000d64:	01612823          	sw	s6,16(sp)
80000d68:	02112623          	sw	ra,44(sp)
80000d6c:	01312e23          	sw	s3,28(sp)
80000d70:	00060913          	mv	s2,a2
  u32_t addr;

  addr = ip4_addr_get_u32(src);
  acc = (addr & 0xffffUL);
  acc = (u32_t)(acc + ((addr >> 16) & 0xffffUL));
  addr = ip4_addr_get_u32(dest);
80000d74:	0007a603          	lw	a2,0(a5)
  addr = ip4_addr_get_u32(src);
80000d78:	00072703          	lw	a4,0(a4)
  acc = (u32_t)(acc + (addr & 0xffffUL));
80000d7c:	000107b7          	lui	a5,0x10
80000d80:	fff78793          	addi	a5,a5,-1 # ffff <_stack_size+0xf7ff>
80000d84:	00f67433          	and	s0,a2,a5
  acc = (u32_t)(acc + ((addr >> 16) & 0xffffUL));
80000d88:	01065613          	srli	a2,a2,0x10
80000d8c:	00c40433          	add	s0,s0,a2
  acc = (addr & 0xffffUL);
80000d90:	00f77633          	and	a2,a4,a5
80000d94:	00c40433          	add	s0,s0,a2
  acc = (u32_t)(acc + ((addr >> 16) & 0xffffUL));
80000d98:	01075713          	srli	a4,a4,0x10
  acc = (u32_t)(acc + ((addr >> 16) & 0xffffUL));
80000d9c:	00870733          	add	a4,a4,s0
  /* fold down to 16 bits */
  acc = FOLD_U32T(acc);
80000da0:	01075413          	srli	s0,a4,0x10
80000da4:	00f77733          	and	a4,a4,a5
80000da8:	00e40733          	add	a4,s0,a4
  acc = FOLD_U32T(acc);
80000dac:	01075413          	srli	s0,a4,0x10
80000db0:	00f77733          	and	a4,a4,a5
{
80000db4:	00050a13          	mv	s4,a0
80000db8:	00058a93          	mv	s5,a1
  acc = FOLD_U32T(acc);
80000dbc:	00e40433          	add	s0,s0,a4
  int swapped = 0;
80000dc0:	00000b13          	li	s6,0
    acc = FOLD_U32T(acc);
80000dc4:	00078493          	mv	s1,a5
  for (q = p; (q != NULL) && (chksum_len > 0); q = q->next) {
80000dc8:	060a1c63          	bnez	s4,80000e40 <inet_chksum_pseudo_partial+0xf4>
  if (swapped) {
80000dcc:	0e0b1663          	bnez	s6,80000eb8 <inet_chksum_pseudo_partial+0x16c>
  acc += (u32_t)lwip_htons((u16_t)proto);
80000dd0:	000a8513          	mv	a0,s5
80000dd4:	b15ff0ef          	jal	ra,800008e8 <lwip_htons>
80000dd8:	00050493          	mv	s1,a0
  acc += (u32_t)lwip_htons(proto_len);
80000ddc:	00090513          	mv	a0,s2
80000de0:	b09ff0ef          	jal	ra,800008e8 <lwip_htons>
80000de4:	00a48533          	add	a0,s1,a0
  acc = FOLD_U32T(acc);
80000de8:	000107b7          	lui	a5,0x10
  acc += (u32_t)lwip_htons(proto_len);
80000dec:	00850433          	add	s0,a0,s0
  acc = FOLD_U32T(acc);
80000df0:	fff78793          	addi	a5,a5,-1 # ffff <_stack_size+0xf7ff>
80000df4:	01045513          	srli	a0,s0,0x10
80000df8:	00f47433          	and	s0,s0,a5
80000dfc:	00850433          	add	s0,a0,s0
  acc = FOLD_U32T(acc);
80000e00:	01045513          	srli	a0,s0,0x10
80000e04:	00f47433          	and	s0,s0,a5
80000e08:	00850433          	add	s0,a0,s0
  return (u16_t)~(acc & 0xffffUL);
80000e0c:	fff44513          	not	a0,s0

  return inet_cksum_pseudo_partial_base(p, proto, proto_len, chksum_len, acc);
}
80000e10:	02c12083          	lw	ra,44(sp)
80000e14:	02812403          	lw	s0,40(sp)
80000e18:	01051513          	slli	a0,a0,0x10
80000e1c:	02412483          	lw	s1,36(sp)
80000e20:	02012903          	lw	s2,32(sp)
80000e24:	01c12983          	lw	s3,28(sp)
80000e28:	01812a03          	lw	s4,24(sp)
80000e2c:	01412a83          	lw	s5,20(sp)
80000e30:	01012b03          	lw	s6,16(sp)
80000e34:	01055513          	srli	a0,a0,0x10
80000e38:	03010113          	addi	sp,sp,48
80000e3c:	00008067          	ret
  for (q = p; (q != NULL) && (chksum_len > 0); q = q->next) {
80000e40:	f80686e3          	beqz	a3,80000dcc <inet_chksum_pseudo_partial+0x80>
    chklen = q->len;
80000e44:	00aa5783          	lhu	a5,10(s4)
    if (chklen > chksum_len) {
80000e48:	00078993          	mv	s3,a5
80000e4c:	00f6f463          	bgeu	a3,a5,80000e54 <inet_chksum_pseudo_partial+0x108>
80000e50:	00068993          	mv	s3,a3
80000e54:	01099993          	slli	s3,s3,0x10
    acc += LWIP_CHKSUM(q->payload, chklen);
80000e58:	004a2503          	lw	a0,4(s4)
80000e5c:	0109d993          	srli	s3,s3,0x10
80000e60:	00098593          	mv	a1,s3
80000e64:	00d12623          	sw	a3,12(sp)
80000e68:	cf1ff0ef          	jal	ra,80000b58 <lwip_standard_chksum>
80000e6c:	00850433          	add	s0,a0,s0
    acc = FOLD_U32T(acc);
80000e70:	01045793          	srli	a5,s0,0x10
    chksum_len = (u16_t)(chksum_len - chklen);
80000e74:	00c12683          	lw	a3,12(sp)
    acc = FOLD_U32T(acc);
80000e78:	00947433          	and	s0,s0,s1
80000e7c:	00878433          	add	s0,a5,s0
    if (q->len % 2 != 0) {
80000e80:	00aa5783          	lhu	a5,10(s4)
    chksum_len = (u16_t)(chksum_len - chklen);
80000e84:	413689b3          	sub	s3,a3,s3
80000e88:	01099693          	slli	a3,s3,0x10
    if (q->len % 2 != 0) {
80000e8c:	0017f793          	andi	a5,a5,1
    chksum_len = (u16_t)(chksum_len - chklen);
80000e90:	0106d693          	srli	a3,a3,0x10
    if (q->len % 2 != 0) {
80000e94:	00078e63          	beqz	a5,80000eb0 <inet_chksum_pseudo_partial+0x164>
      acc = SWAP_BYTES_IN_WORD(acc);
80000e98:	00841793          	slli	a5,s0,0x8
80000e9c:	00845413          	srli	s0,s0,0x8
80000ea0:	0097f7b3          	and	a5,a5,s1
80000ea4:	0ff47413          	andi	s0,s0,255
80000ea8:	001b4b13          	xori	s6,s6,1
80000eac:	0087e433          	or	s0,a5,s0
  for (q = p; (q != NULL) && (chksum_len > 0); q = q->next) {
80000eb0:	000a2a03          	lw	s4,0(s4)
80000eb4:	f15ff06f          	j	80000dc8 <inet_chksum_pseudo_partial+0x7c>
    acc = SWAP_BYTES_IN_WORD(acc);
80000eb8:	00010737          	lui	a4,0x10
80000ebc:	00841793          	slli	a5,s0,0x8
80000ec0:	fff70713          	addi	a4,a4,-1 # ffff <_stack_size+0xf7ff>
80000ec4:	00845413          	srli	s0,s0,0x8
80000ec8:	00e7f7b3          	and	a5,a5,a4
80000ecc:	0ff47413          	andi	s0,s0,255
80000ed0:	0087e433          	or	s0,a5,s0
80000ed4:	efdff06f          	j	80000dd0 <inet_chksum_pseudo_partial+0x84>

80000ed8 <ip_chksum_pseudo_partial>:
#if LWIP_IPV4 && LWIP_IPV6
  else
#endif /* LWIP_IPV4 && LWIP_IPV6 */
#if LWIP_IPV4
  {
    return inet_chksum_pseudo_partial(p, proto, proto_len, chksum_len, ip_2_ip4(src), ip_2_ip4(dest));
80000ed8:	e75ff06f          	j	80000d4c <inet_chksum_pseudo_partial>

80000edc <inet_chksum>:
 * @return checksum (as u16_t) to be saved directly in the protocol header
 */

u16_t
inet_chksum(const void *dataptr, u16_t len)
{
80000edc:	ff010113          	addi	sp,sp,-16
80000ee0:	00112623          	sw	ra,12(sp)
  return (u16_t)~(unsigned int)LWIP_CHKSUM(dataptr, len);
80000ee4:	c75ff0ef          	jal	ra,80000b58 <lwip_standard_chksum>
}
80000ee8:	00c12083          	lw	ra,12(sp)
  return (u16_t)~(unsigned int)LWIP_CHKSUM(dataptr, len);
80000eec:	fff54513          	not	a0,a0
}
80000ef0:	01051513          	slli	a0,a0,0x10
80000ef4:	01055513          	srli	a0,a0,0x10
80000ef8:	01010113          	addi	sp,sp,16
80000efc:	00008067          	ret

80000f00 <inet_chksum_pbuf>:
 * @param p pbuf chain over that the checksum should be calculated
 * @return checksum (as u16_t) to be saved directly in the protocol header
 */
u16_t
inet_chksum_pbuf(struct pbuf *p)
{
80000f00:	fe010113          	addi	sp,sp,-32
80000f04:	01212823          	sw	s2,16(sp)
  int swapped = 0;

  acc = 0;
  for (q = p; q != NULL; q = q->next) {
    acc += LWIP_CHKSUM(q->payload, q->len);
    acc = FOLD_U32T(acc);
80000f08:	00010937          	lui	s2,0x10
{
80000f0c:	00812c23          	sw	s0,24(sp)
80000f10:	00912a23          	sw	s1,20(sp)
80000f14:	01312623          	sw	s3,12(sp)
80000f18:	00112e23          	sw	ra,28(sp)
80000f1c:	00050493          	mv	s1,a0
  int swapped = 0;
80000f20:	00000993          	li	s3,0
  acc = 0;
80000f24:	00000413          	li	s0,0
    acc = FOLD_U32T(acc);
80000f28:	fff90913          	addi	s2,s2,-1 # ffff <_stack_size+0xf7ff>
  for (q = p; q != NULL; q = q->next) {
80000f2c:	04049663          	bnez	s1,80000f78 <inet_chksum_pbuf+0x78>
      swapped = !swapped;
      acc = SWAP_BYTES_IN_WORD(acc);
    }
  }

  if (swapped) {
80000f30:	02098063          	beqz	s3,80000f50 <inet_chksum_pbuf+0x50>
    acc = SWAP_BYTES_IN_WORD(acc);
80000f34:	00010737          	lui	a4,0x10
80000f38:	00841793          	slli	a5,s0,0x8
80000f3c:	fff70713          	addi	a4,a4,-1 # ffff <_stack_size+0xf7ff>
80000f40:	00845413          	srli	s0,s0,0x8
80000f44:	00e7f7b3          	and	a5,a5,a4
80000f48:	0ff47413          	andi	s0,s0,255
80000f4c:	0087e433          	or	s0,a5,s0
  }
  return (u16_t)~(acc & 0xffffUL);
80000f50:	fff44513          	not	a0,s0
}
80000f54:	01c12083          	lw	ra,28(sp)
80000f58:	01812403          	lw	s0,24(sp)
80000f5c:	01051513          	slli	a0,a0,0x10
80000f60:	01412483          	lw	s1,20(sp)
80000f64:	01012903          	lw	s2,16(sp)
80000f68:	00c12983          	lw	s3,12(sp)
80000f6c:	01055513          	srli	a0,a0,0x10
80000f70:	02010113          	addi	sp,sp,32
80000f74:	00008067          	ret
    acc += LWIP_CHKSUM(q->payload, q->len);
80000f78:	00a4d583          	lhu	a1,10(s1)
80000f7c:	0044a503          	lw	a0,4(s1)
80000f80:	bd9ff0ef          	jal	ra,80000b58 <lwip_standard_chksum>
80000f84:	00850433          	add	s0,a0,s0
    acc = FOLD_U32T(acc);
80000f88:	01045793          	srli	a5,s0,0x10
80000f8c:	01247433          	and	s0,s0,s2
80000f90:	00878433          	add	s0,a5,s0
    if (q->len % 2 != 0) {
80000f94:	00a4d783          	lhu	a5,10(s1)
80000f98:	0017f793          	andi	a5,a5,1
80000f9c:	00078e63          	beqz	a5,80000fb8 <inet_chksum_pbuf+0xb8>
      acc = SWAP_BYTES_IN_WORD(acc);
80000fa0:	00841793          	slli	a5,s0,0x8
80000fa4:	00845413          	srli	s0,s0,0x8
80000fa8:	0127f7b3          	and	a5,a5,s2
80000fac:	0ff47413          	andi	s0,s0,255
80000fb0:	0019c993          	xori	s3,s3,1
80000fb4:	0087e433          	or	s0,a5,s0
  for (q = p; q != NULL; q = q->next) {
80000fb8:	0004a483          	lw	s1,0(s1)
80000fbc:	f71ff06f          	j	80000f2c <inet_chksum_pbuf+0x2c>

80000fc0 <mem_init>:
 * C library malloc().
 */
void
mem_init(void)
{
}
80000fc0:	00008067          	ret

80000fc4 <mem_trim>:
void *
mem_trim(void *mem, mem_size_t size)
{
  LWIP_UNUSED_ARG(size);
  return mem;
}
80000fc4:	00008067          	ret

80000fc8 <malloc>:
 *
 * Note that the returned value must always be aligned (as defined by MEM_ALIGNMENT).
 */
void *
mem_malloc(mem_size_t size)
{
80000fc8:	ff010113          	addi	sp,sp,-16
80000fcc:	00812423          	sw	s0,8(sp)
80000fd0:	00112623          	sw	ra,12(sp)
  void *ret = mem_clib_malloc(size + MEM_LIBC_STATSHELPER_SIZE);
80000fd4:	ff5ff0ef          	jal	ra,80000fc8 <malloc>
80000fd8:	00050413          	mv	s0,a0
  if (ret == NULL) {
80000fdc:	00051663          	bnez	a0,80000fe8 <malloc+0x20>
    MEM_STATS_INC_LOCKED(err);
80000fe0:	dd8ff0ef          	jal	ra,800005b8 <sys_arch_protect>
80000fe4:	dd8ff0ef          	jal	ra,800005bc <sys_arch_unprotect>
    ret = (u8_t *)ret + MEM_LIBC_STATSHELPER_SIZE;
    MEM_STATS_INC_USED_LOCKED(used, size);
#endif
  }
  return ret;
}
80000fe8:	00040513          	mv	a0,s0
80000fec:	00c12083          	lw	ra,12(sp)
80000ff0:	00812403          	lw	s0,8(sp)
80000ff4:	01010113          	addi	sp,sp,16
80000ff8:	00008067          	ret

80000ffc <free>:
  LWIP_ASSERT("rmem == MEM_ALIGN(rmem)", (rmem == LWIP_MEM_ALIGN(rmem)));
#if LWIP_STATS && MEM_STATS
  rmem = (u8_t *)rmem - MEM_LIBC_STATSHELPER_SIZE;
  MEM_STATS_DEC_USED_LOCKED(used, *(mem_size_t *)rmem);
#endif
  mem_clib_free(rmem);
80000ffc:	0000006f          	j	80000ffc <free>

80001000 <calloc>:

#if MEM_LIBC_MALLOC && (!LWIP_STATS || !MEM_STATS)
void *
mem_calloc(mem_size_t count, mem_size_t size)
{
  return mem_clib_calloc(count, size);
80001000:	0000006f          	j	80001000 <calloc>

80001004 <do_memp_malloc_pool.isra.0>:
{
  struct memp *memp;
  SYS_ARCH_DECL_PROTECT(old_level);

#if MEMP_MEM_MALLOC
  memp = (struct memp *)mem_malloc(MEMP_SIZE + MEMP_ALIGN_SIZE(desc->size));
80001004:	00350513          	addi	a0,a0,3
do_memp_malloc_pool(const struct memp_desc *desc)
80001008:	ff010113          	addi	sp,sp,-16
  memp = (struct memp *)mem_malloc(MEMP_SIZE + MEMP_ALIGN_SIZE(desc->size));
8000100c:	ffc57513          	andi	a0,a0,-4
do_memp_malloc_pool(const struct memp_desc *desc)
80001010:	00112623          	sw	ra,12(sp)
80001014:	00812423          	sw	s0,8(sp)
  memp = (struct memp *)mem_malloc(MEMP_SIZE + MEMP_ALIGN_SIZE(desc->size));
80001018:	fb1ff0ef          	jal	ra,80000fc8 <malloc>
8000101c:	00050413          	mv	s0,a0
  SYS_ARCH_PROTECT(old_level);
80001020:	d98ff0ef          	jal	ra,800005b8 <sys_arch_protect>
    return ((u8_t *)memp + MEMP_SIZE);
  } else {
#if MEMP_STATS
    desc->stats->err++;
#endif
    SYS_ARCH_UNPROTECT(old_level);
80001024:	d98ff0ef          	jal	ra,800005bc <sys_arch_unprotect>
    LWIP_DEBUGF(MEMP_DEBUG | LWIP_DBG_LEVEL_SERIOUS, ("memp_malloc: out of memory in pool %s\n", desc->desc));
  }

  return NULL;
}
80001028:	00040513          	mv	a0,s0
8000102c:	00c12083          	lw	ra,12(sp)
80001030:	00812403          	lw	s0,8(sp)
80001034:	01010113          	addi	sp,sp,16
80001038:	00008067          	ret

8000103c <memp_init_pool>:
}
8000103c:	00008067          	ret

80001040 <memp_init>:
}
80001040:	00008067          	ret

80001044 <memp_malloc_pool>:
#else
memp_malloc_pool_fn(const struct memp_desc *desc, const char *file, const int line)
#endif
{
  LWIP_ASSERT("invalid pool desc", desc != NULL);
  if (desc == NULL) {
80001044:	00050663          	beqz	a0,80001050 <memp_malloc_pool+0xc>
    return NULL;
  }

#if !MEMP_OVERFLOW_CHECK
  return do_memp_malloc_pool(desc);
80001048:	00455503          	lhu	a0,4(a0)
8000104c:	fb9ff06f          	j	80001004 <do_memp_malloc_pool.isra.0>
#else
  return do_memp_malloc_pool_fn(desc, file, line);
#endif
}
80001050:	00008067          	ret

80001054 <memp_malloc>:
#else
memp_malloc_fn(memp_t type, const char *file, const int line)
#endif
{
  void *memp;
  LWIP_ERROR("memp_malloc: type < MEMP_MAX", (type < MEMP_MAX), return NULL;);
80001054:	00600793          	li	a5,6
80001058:	02a7e063          	bltu	a5,a0,80001078 <memp_malloc+0x24>
#if MEMP_OVERFLOW_CHECK >= 2
  memp_overflow_check_all();
#endif /* MEMP_OVERFLOW_CHECK >= 2 */

#if !MEMP_OVERFLOW_CHECK
  memp = do_memp_malloc_pool(memp_pools[type]);
8000105c:	00251793          	slli	a5,a0,0x2
80001060:	80000537          	lui	a0,0x80000
80001064:	16450513          	addi	a0,a0,356 # 80000164 <_stack_start+0xffff93d4>
80001068:	00f50533          	add	a0,a0,a5
8000106c:	00052783          	lw	a5,0(a0)
80001070:	0047d503          	lhu	a0,4(a5)
80001074:	f91ff06f          	j	80001004 <do_memp_malloc_pool.isra.0>
#else
  memp = do_memp_malloc_pool_fn(memp_pools[type], file, line);
#endif

  return memp;
}
80001078:	00000513          	li	a0,0
8000107c:	00008067          	ret

80001080 <memp_free_pool>:
 */
void
memp_free_pool(const struct memp_desc *desc, void *mem)
{
  LWIP_ASSERT("invalid pool desc", desc != NULL);
  if ((desc == NULL) || (mem == NULL)) {
80001080:	02050a63          	beqz	a0,800010b4 <memp_free_pool+0x34>
80001084:	02058863          	beqz	a1,800010b4 <memp_free_pool+0x34>
{
80001088:	ff010113          	addi	sp,sp,-16
8000108c:	00112623          	sw	ra,12(sp)
80001090:	00812423          	sw	s0,8(sp)
80001094:	00058413          	mv	s0,a1
  SYS_ARCH_PROTECT(old_level);
80001098:	d20ff0ef          	jal	ra,800005b8 <sys_arch_protect>
  SYS_ARCH_UNPROTECT(old_level);
8000109c:	d20ff0ef          	jal	ra,800005bc <sys_arch_unprotect>
  mem_free(memp);
800010a0:	00040513          	mv	a0,s0
    return;
  }

  do_memp_free_pool(desc, mem);
}
800010a4:	00812403          	lw	s0,8(sp)
800010a8:	00c12083          	lw	ra,12(sp)
800010ac:	01010113          	addi	sp,sp,16
  mem_free(memp);
800010b0:	f4dff06f          	j	80000ffc <free>
800010b4:	00008067          	ret

800010b8 <memp_free>:
{
#ifdef LWIP_HOOK_MEMP_AVAILABLE
  struct memp *old_first;
#endif

  LWIP_ERROR("memp_free: type < MEMP_MAX", (type < MEMP_MAX), return;);
800010b8:	00600793          	li	a5,6
800010bc:	02a7ea63          	bltu	a5,a0,800010f0 <memp_free+0x38>

  if (mem == NULL) {
800010c0:	02058863          	beqz	a1,800010f0 <memp_free+0x38>
{
800010c4:	ff010113          	addi	sp,sp,-16
800010c8:	00112623          	sw	ra,12(sp)
800010cc:	00812423          	sw	s0,8(sp)
800010d0:	00058413          	mv	s0,a1
  SYS_ARCH_PROTECT(old_level);
800010d4:	ce4ff0ef          	jal	ra,800005b8 <sys_arch_protect>
  SYS_ARCH_UNPROTECT(old_level);
800010d8:	ce4ff0ef          	jal	ra,800005bc <sys_arch_unprotect>
  mem_free(memp);
800010dc:	00040513          	mv	a0,s0
#ifdef LWIP_HOOK_MEMP_AVAILABLE
  if (old_first == NULL) {
    LWIP_HOOK_MEMP_AVAILABLE(type);
  }
#endif
}
800010e0:	00812403          	lw	s0,8(sp)
800010e4:	00c12083          	lw	ra,12(sp)
800010e8:	01010113          	addi	sp,sp,16
  mem_free(memp);
800010ec:	f11ff06f          	j	80000ffc <free>
800010f0:	00008067          	ret

800010f4 <netif_null_output_ip4>:
  LWIP_UNUSED_ARG(netif);
  LWIP_UNUSED_ARG(p);
  LWIP_UNUSED_ARG(ipaddr);

  return ERR_IF;
}
800010f4:	ff400513          	li	a0,-12
800010f8:	00008067          	ret

800010fc <netif_issue_reports>:
  if (!(netif->flags & NETIF_FLAG_LINK_UP) ||
800010fc:	03154783          	lbu	a5,49(a0)
80001100:	00500693          	li	a3,5
80001104:	0057f613          	andi	a2,a5,5
80001108:	02d61263          	bne	a2,a3,8000112c <netif_issue_reports+0x30>
  if ((report_type & NETIF_REPORT_TYPE_IPV4) &&
8000110c:	0015f593          	andi	a1,a1,1
80001110:	00058e63          	beqz	a1,8000112c <netif_issue_reports+0x30>
80001114:	00452683          	lw	a3,4(a0)
80001118:	00068a63          	beqz	a3,8000112c <netif_issue_reports+0x30>
    if (netif->flags & (NETIF_FLAG_ETHARP)) {
8000111c:	0087f793          	andi	a5,a5,8
80001120:	00078663          	beqz	a5,8000112c <netif_issue_reports+0x30>
      etharp_gratuitous(netif);
80001124:	00450593          	addi	a1,a0,4
80001128:	3310206f          	j	80003c58 <etharp_request>
}
8000112c:	00008067          	ret

80001130 <netif_do_set_ipaddr>:
  if (ip4_addr_cmp(ipaddr, netif_ip4_addr(netif)) == 0) {
80001130:	00452783          	lw	a5,4(a0)
80001134:	0005a703          	lw	a4,0(a1)
80001138:	08f70263          	beq	a4,a5,800011bc <netif_do_set_ipaddr+0x8c>
{
8000113c:	fe010113          	addi	sp,sp,-32
    ip_addr_copy(*old_addr, *netif_ip_addr4(netif));
80001140:	00f62023          	sw	a5,0(a2)
{
80001144:	00812c23          	sw	s0,24(sp)
80001148:	01212823          	sw	s2,16(sp)
8000114c:	00050413          	mv	s0,a0
    *ip_2_ip4(&new_addr) = *ipaddr;
80001150:	00058913          	mv	s2,a1
  udp_netif_ip_addr_changed(old_addr, new_addr);
80001154:	00060513          	mv	a0,a2
80001158:	00c10593          	addi	a1,sp,12
{
8000115c:	00112e23          	sw	ra,28(sp)
    *ip_2_ip4(&new_addr) = *ipaddr;
80001160:	00e12623          	sw	a4,12(sp)
{
80001164:	00912a23          	sw	s1,20(sp)
80001168:	00060493          	mv	s1,a2
  udp_netif_ip_addr_changed(old_addr, new_addr);
8000116c:	3e0020ef          	jal	ra,8000354c <udp_netif_ip_addr_changed>
  raw_netif_ip_addr_changed(old_addr, new_addr);
80001170:	00c10593          	addi	a1,sp,12
80001174:	00048513          	mv	a0,s1
80001178:	075010ef          	jal	ra,800029ec <raw_netif_ip_addr_changed>
    ip4_addr_set(ip_2_ip4(&netif->ip_addr), ipaddr);
8000117c:	00092783          	lw	a5,0(s2)
    netif_issue_reports(netif, NETIF_REPORT_TYPE_IPV4);
80001180:	00100593          	li	a1,1
80001184:	00040513          	mv	a0,s0
    ip4_addr_set(ip_2_ip4(&netif->ip_addr), ipaddr);
80001188:	00f42223          	sw	a5,4(s0)
    netif_issue_reports(netif, NETIF_REPORT_TYPE_IPV4);
8000118c:	f71ff0ef          	jal	ra,800010fc <netif_issue_reports>
    NETIF_STATUS_CALLBACK(netif);
80001190:	01c42783          	lw	a5,28(s0)
80001194:	00078663          	beqz	a5,800011a0 <netif_do_set_ipaddr+0x70>
80001198:	00040513          	mv	a0,s0
8000119c:	000780e7          	jalr	a5
}
800011a0:	01c12083          	lw	ra,28(sp)
800011a4:	01812403          	lw	s0,24(sp)
800011a8:	01412483          	lw	s1,20(sp)
800011ac:	01012903          	lw	s2,16(sp)
    return 1; /* address changed */
800011b0:	00100513          	li	a0,1
}
800011b4:	02010113          	addi	sp,sp,32
800011b8:	00008067          	ret
  return 0; /* address unchanged */
800011bc:	00000513          	li	a0,0
}
800011c0:	00008067          	ret

800011c4 <netif_init>:
}
800011c4:	00008067          	ret

800011c8 <netif_input>:
  if (inp->flags & (NETIF_FLAG_ETHARP | NETIF_FLAG_ETHERNET)) {
800011c8:	0315c783          	lbu	a5,49(a1)
800011cc:	0187f793          	andi	a5,a5,24
800011d0:	00078463          	beqz	a5,800011d8 <netif_input+0x10>
    return ethernet_input(p, inp);
800011d4:	cc0ff06f          	j	80000694 <ethernet_input>
    return ip_input(p, inp);
800011d8:	2780406f          	j	80005450 <ip4_input>

800011dc <netif_set_ipaddr>:
  LWIP_ERROR("netif_set_ipaddr: invalid netif", netif != NULL, return);
800011dc:	02050463          	beqz	a0,80001204 <netif_set_ipaddr+0x28>
{
800011e0:	fe010113          	addi	sp,sp,-32
800011e4:	00112e23          	sw	ra,28(sp)
  if (ipaddr == NULL) {
800011e8:	00059463          	bnez	a1,800011f0 <netif_set_ipaddr+0x14>
    ipaddr = IP4_ADDR_ANY4;
800011ec:	85818593          	addi	a1,gp,-1960 # 800003a0 <ip_addr_any>
  if (netif_do_set_ipaddr(netif, ipaddr, &old_addr)) {
800011f0:	00c10613          	addi	a2,sp,12
800011f4:	f3dff0ef          	jal	ra,80001130 <netif_do_set_ipaddr>
}
800011f8:	01c12083          	lw	ra,28(sp)
800011fc:	02010113          	addi	sp,sp,32
80001200:	00008067          	ret
80001204:	00008067          	ret

80001208 <netif_set_netmask>:
  LWIP_ERROR("netif_set_netmask: invalid netif", netif != NULL, return);
80001208:	00050e63          	beqz	a0,80001224 <netif_set_netmask+0x1c>
  if (netmask == NULL) {
8000120c:	00059463          	bnez	a1,80001214 <netif_set_netmask+0xc>
    netmask = IP4_ADDR_ANY4;
80001210:	85818593          	addi	a1,gp,-1960 # 800003a0 <ip_addr_any>
  if (netif_do_set_netmask(netif, netmask, old_nm)) {
80001214:	0005a783          	lw	a5,0(a1)
  if (ip4_addr_cmp(netmask, netif_ip4_netmask(netif)) == 0) {
80001218:	00852703          	lw	a4,8(a0)
8000121c:	00e78463          	beq	a5,a4,80001224 <netif_set_netmask+0x1c>
    ip4_addr_set(ip_2_ip4(&netif->netmask), netmask);
80001220:	00f52423          	sw	a5,8(a0)
}
80001224:	00008067          	ret

80001228 <netif_set_gw>:
  LWIP_ERROR("netif_set_gw: invalid netif", netif != NULL, return);
80001228:	00050e63          	beqz	a0,80001244 <netif_set_gw+0x1c>
  if (gw == NULL) {
8000122c:	00059463          	bnez	a1,80001234 <netif_set_gw+0xc>
    gw = IP4_ADDR_ANY4;
80001230:	85818593          	addi	a1,gp,-1960 # 800003a0 <ip_addr_any>
  if (netif_do_set_gw(netif, gw, old_gw)) {
80001234:	0005a783          	lw	a5,0(a1)
  if (ip4_addr_cmp(gw, netif_ip4_gw(netif)) == 0) {
80001238:	00c52703          	lw	a4,12(a0)
8000123c:	00e78463          	beq	a5,a4,80001244 <netif_set_gw+0x1c>
    ip4_addr_set(ip_2_ip4(&netif->gw), gw);
80001240:	00f52623          	sw	a5,12(a0)
}
80001244:	00008067          	ret

80001248 <netif_set_addr>:
{
80001248:	fd010113          	addi	sp,sp,-48
8000124c:	02812423          	sw	s0,40(sp)
80001250:	02912223          	sw	s1,36(sp)
80001254:	03212023          	sw	s2,32(sp)
80001258:	02112623          	sw	ra,44(sp)
8000125c:	00050493          	mv	s1,a0
80001260:	00060913          	mv	s2,a2
  if (ipaddr == NULL) {
80001264:	00058413          	mv	s0,a1
80001268:	00059463          	bnez	a1,80001270 <netif_set_addr+0x28>
    ipaddr = IP4_ADDR_ANY4;
8000126c:	85818413          	addi	s0,gp,-1960 # 800003a0 <ip_addr_any>
  if (netmask == NULL) {
80001270:	00091463          	bnez	s2,80001278 <netif_set_addr+0x30>
    netmask = IP4_ADDR_ANY4;
80001274:	85818913          	addi	s2,gp,-1960 # 800003a0 <ip_addr_any>
  if (gw == NULL) {
80001278:	00069463          	bnez	a3,80001280 <netif_set_addr+0x38>
    gw = IP4_ADDR_ANY4;
8000127c:	85818693          	addi	a3,gp,-1960 # 800003a0 <ip_addr_any>
  remove = ip4_addr_isany(ipaddr);
80001280:	00042783          	lw	a5,0(s0)
80001284:	00078663          	beqz	a5,80001290 <netif_set_addr+0x48>
80001288:	00000793          	li	a5,0
8000128c:	0200006f          	j	800012ac <netif_set_addr+0x64>
    if (netif_do_set_ipaddr(netif, ipaddr, &old_addr)) {
80001290:	01c10613          	addi	a2,sp,28
80001294:	00040593          	mv	a1,s0
80001298:	00048513          	mv	a0,s1
8000129c:	00d12623          	sw	a3,12(sp)
800012a0:	e91ff0ef          	jal	ra,80001130 <netif_do_set_ipaddr>
800012a4:	00c12683          	lw	a3,12(sp)
800012a8:	00100793          	li	a5,1
  if (netif_do_set_netmask(netif, netmask, old_nm)) {
800012ac:	00092703          	lw	a4,0(s2)
  if (ip4_addr_cmp(netmask, netif_ip4_netmask(netif)) == 0) {
800012b0:	0084a603          	lw	a2,8(s1)
800012b4:	00c70463          	beq	a4,a2,800012bc <netif_set_addr+0x74>
    ip4_addr_set(ip_2_ip4(&netif->netmask), netmask);
800012b8:	00e4a423          	sw	a4,8(s1)
  if (netif_do_set_gw(netif, gw, old_gw)) {
800012bc:	0006a703          	lw	a4,0(a3)
  if (ip4_addr_cmp(gw, netif_ip4_gw(netif)) == 0) {
800012c0:	00c4a683          	lw	a3,12(s1)
800012c4:	00d70463          	beq	a4,a3,800012cc <netif_set_addr+0x84>
    ip4_addr_set(ip_2_ip4(&netif->gw), gw);
800012c8:	00e4a623          	sw	a4,12(s1)
  if (!remove) {
800012cc:	00079a63          	bnez	a5,800012e0 <netif_set_addr+0x98>
    if (netif_do_set_ipaddr(netif, ipaddr, &old_addr)) {
800012d0:	01c10613          	addi	a2,sp,28
800012d4:	00040593          	mv	a1,s0
800012d8:	00048513          	mv	a0,s1
800012dc:	e55ff0ef          	jal	ra,80001130 <netif_do_set_ipaddr>
}
800012e0:	02c12083          	lw	ra,44(sp)
800012e4:	02812403          	lw	s0,40(sp)
800012e8:	02412483          	lw	s1,36(sp)
800012ec:	02012903          	lw	s2,32(sp)
800012f0:	03010113          	addi	sp,sp,48
800012f4:	00008067          	ret

800012f8 <netif_add>:
{
800012f8:	fe010113          	addi	sp,sp,-32
800012fc:	00112e23          	sw	ra,28(sp)
80001300:	00812c23          	sw	s0,24(sp)
80001304:	00912a23          	sw	s1,20(sp)
  LWIP_ERROR("netif_add: invalid netif", netif != NULL, return NULL);
80001308:	02051063          	bnez	a0,80001328 <netif_add+0x30>
8000130c:	00000413          	li	s0,0
}
80001310:	00040513          	mv	a0,s0
80001314:	01c12083          	lw	ra,28(sp)
80001318:	01812403          	lw	s0,24(sp)
8000131c:	01412483          	lw	s1,20(sp)
80001320:	02010113          	addi	sp,sp,32
80001324:	00008067          	ret
  LWIP_ERROR("netif_add: No init function given", init != NULL, return NULL);
80001328:	fe0782e3          	beqz	a5,8000130c <netif_add+0x14>
8000132c:	00050413          	mv	s0,a0
  if (ipaddr == NULL) {
80001330:	00059463          	bnez	a1,80001338 <netif_add+0x40>
    ipaddr = ip_2_ip4(IP4_ADDR_ANY);
80001334:	85818593          	addi	a1,gp,-1960 # 800003a0 <ip_addr_any>
  if (netmask == NULL) {
80001338:	00061463          	bnez	a2,80001340 <netif_add+0x48>
    netmask = ip_2_ip4(IP4_ADDR_ANY);
8000133c:	85818613          	addi	a2,gp,-1960 # 800003a0 <ip_addr_any>
  if (gw == NULL) {
80001340:	00069463          	bnez	a3,80001348 <netif_add+0x50>
    gw = ip_2_ip4(IP4_ADDR_ANY);
80001344:	85818693          	addi	a3,gp,-1960 # 800003a0 <ip_addr_any>
  netif->state = state;
80001348:	02e42223          	sw	a4,36(s0)
  netif->num = netif_num;
8000134c:	8601c703          	lbu	a4,-1952(gp) # 800003a8 <netif_num>
  netif->output = netif_null_output_ip4;
80001350:	5ac18513          	addi	a0,gp,1452 # 800010f4 <netif_null_output_ip4>
80001354:	00a42a23          	sw	a0,20(s0)
  ip_addr_set_zero_ip4(&netif->ip_addr);
80001358:	00042223          	sw	zero,4(s0)
  netif_set_addr(netif, ipaddr, netmask, gw);
8000135c:	00040513          	mv	a0,s0
  ip_addr_set_zero_ip4(&netif->netmask);
80001360:	00042423          	sw	zero,8(s0)
  ip_addr_set_zero_ip4(&netif->gw);
80001364:	00042623          	sw	zero,12(s0)
  netif->mtu = 0;
80001368:	02041423          	sh	zero,40(s0)
  netif->flags = 0;
8000136c:	020408a3          	sb	zero,49(s0)
  netif->status_callback = NULL;
80001370:	00042e23          	sw	zero,28(s0)
  netif->link_callback = NULL;
80001374:	02042023          	sw	zero,32(s0)
  netif->num = netif_num;
80001378:	02e40a23          	sb	a4,52(s0)
  netif->input = input;
8000137c:	01042823          	sw	a6,16(s0)
  NETIF_RESET_HINTS(netif);
80001380:	02042c23          	sw	zero,56(s0)
80001384:	00f12623          	sw	a5,12(sp)
  netif_set_addr(netif, ipaddr, netmask, gw);
80001388:	ec1ff0ef          	jal	ra,80001248 <netif_set_addr>
  if (init(netif) != ERR_OK) {
8000138c:	00c12783          	lw	a5,12(sp)
80001390:	00040513          	mv	a0,s0
80001394:	000780e7          	jalr	a5
80001398:	f6051ae3          	bnez	a0,8000130c <netif_add+0x14>
      for (netif2 = netif_list; netif2 != NULL; netif2 = netif2->next) {
8000139c:	8681a603          	lw	a2,-1944(gp) # 800003b0 <netif_list>
800013a0:	00078713          	mv	a4,a5
      if (netif->num == 255) {
800013a4:	0ff00593          	li	a1,255
800013a8:	03444783          	lbu	a5,52(s0)
800013ac:	00b79463          	bne	a5,a1,800013b4 <netif_add+0xbc>
        netif->num = 0;
800013b0:	02040a23          	sb	zero,52(s0)
      for (netif2 = netif_list; netif2 != NULL; netif2 = netif2->next) {
800013b4:	03444783          	lbu	a5,52(s0)
800013b8:	00060693          	mv	a3,a2
800013bc:	02069463          	bnez	a3,800013e4 <netif_add+0xec>
  if (netif->num == 254) {
800013c0:	0fe00593          	li	a1,254
    netif_num = 0;
800013c4:	00000693          	li	a3,0
  if (netif->num == 254) {
800013c8:	00b78663          	beq	a5,a1,800013d4 <netif_add+0xdc>
    netif_num = (u8_t)(netif->num + 1);
800013cc:	00178793          	addi	a5,a5,1
800013d0:	0ff7f693          	andi	a3,a5,255
  netif->next = netif_list;
800013d4:	00c42023          	sw	a2,0(s0)
800013d8:	86d18023          	sb	a3,-1952(gp) # 800003a8 <netif_num>
  netif_list = netif;
800013dc:	8681a423          	sw	s0,-1944(gp) # 800003b0 <netif_list>
  return netif;
800013e0:	f31ff06f          	j	80001310 <netif_add+0x18>
        if (netif2->num == netif->num) {
800013e4:	0346c503          	lbu	a0,52(a3)
800013e8:	00f51863          	bne	a0,a5,800013f8 <netif_add+0x100>
          netif->num++;
800013ec:	00178793          	addi	a5,a5,1
800013f0:	02f40a23          	sb	a5,52(s0)
          break;
800013f4:	fb5ff06f          	j	800013a8 <netif_add+0xb0>
      for (netif2 = netif_list; netif2 != NULL; netif2 = netif2->next) {
800013f8:	0006a683          	lw	a3,0(a3)
800013fc:	fc1ff06f          	j	800013bc <netif_add+0xc4>

80001400 <netif_add_noaddr>:
  return netif_add(netif,
80001400:	00068813          	mv	a6,a3
80001404:	00060793          	mv	a5,a2
80001408:	00058713          	mv	a4,a1
8000140c:	00000693          	li	a3,0
80001410:	00000613          	li	a2,0
80001414:	00000593          	li	a1,0
80001418:	ee1ff06f          	j	800012f8 <netif_add>

8000141c <netif_set_default>:
  netif_default = netif;
8000141c:	86a1a223          	sw	a0,-1948(gp) # 800003ac <netif_default>
}
80001420:	00008067          	ret

80001424 <netif_set_up>:
  LWIP_ERROR("netif_set_up: invalid netif", netif != NULL, return);
80001424:	04050e63          	beqz	a0,80001480 <netif_set_up+0x5c>
{
80001428:	ff010113          	addi	sp,sp,-16
8000142c:	00812423          	sw	s0,8(sp)
80001430:	00112623          	sw	ra,12(sp)
  if (!(netif->flags & NETIF_FLAG_UP)) {
80001434:	03154783          	lbu	a5,49(a0)
80001438:	00050413          	mv	s0,a0
8000143c:	0017f713          	andi	a4,a5,1
80001440:	02071863          	bnez	a4,80001470 <netif_set_up+0x4c>
    netif_set_flags(netif, NETIF_FLAG_UP);
80001444:	0017e793          	ori	a5,a5,1
80001448:	02f508a3          	sb	a5,49(a0)
    NETIF_STATUS_CALLBACK(netif);
8000144c:	01c52783          	lw	a5,28(a0)
80001450:	00078463          	beqz	a5,80001458 <netif_set_up+0x34>
80001454:	000780e7          	jalr	a5
    netif_issue_reports(netif, NETIF_REPORT_TYPE_IPV4 | NETIF_REPORT_TYPE_IPV6);
80001458:	00040513          	mv	a0,s0
}
8000145c:	00812403          	lw	s0,8(sp)
80001460:	00c12083          	lw	ra,12(sp)
    netif_issue_reports(netif, NETIF_REPORT_TYPE_IPV4 | NETIF_REPORT_TYPE_IPV6);
80001464:	00300593          	li	a1,3
}
80001468:	01010113          	addi	sp,sp,16
    netif_issue_reports(netif, NETIF_REPORT_TYPE_IPV4 | NETIF_REPORT_TYPE_IPV6);
8000146c:	c91ff06f          	j	800010fc <netif_issue_reports>
}
80001470:	00c12083          	lw	ra,12(sp)
80001474:	00812403          	lw	s0,8(sp)
80001478:	01010113          	addi	sp,sp,16
8000147c:	00008067          	ret
80001480:	00008067          	ret

80001484 <netif_set_down>:
  LWIP_ERROR("netif_set_down: invalid netif", netif != NULL, return);
80001484:	06050063          	beqz	a0,800014e4 <netif_set_down+0x60>
{
80001488:	ff010113          	addi	sp,sp,-16
8000148c:	00812423          	sw	s0,8(sp)
80001490:	00112623          	sw	ra,12(sp)
  if (netif->flags & NETIF_FLAG_UP) {
80001494:	03154783          	lbu	a5,49(a0)
80001498:	00050413          	mv	s0,a0
8000149c:	0017f713          	andi	a4,a5,1
800014a0:	02070a63          	beqz	a4,800014d4 <netif_set_down+0x50>
    netif_clear_flags(netif, NETIF_FLAG_UP);
800014a4:	ffe7f713          	andi	a4,a5,-2
800014a8:	02e508a3          	sb	a4,49(a0)
    if (netif->flags & NETIF_FLAG_ETHARP) {
800014ac:	0087f793          	andi	a5,a5,8
800014b0:	00078463          	beqz	a5,800014b8 <netif_set_down+0x34>
      etharp_cleanup_netif(netif);
800014b4:	43c020ef          	jal	ra,800038f0 <etharp_cleanup_netif>
    NETIF_STATUS_CALLBACK(netif);
800014b8:	01c42303          	lw	t1,28(s0)
800014bc:	00030c63          	beqz	t1,800014d4 <netif_set_down+0x50>
800014c0:	00040513          	mv	a0,s0
}
800014c4:	00812403          	lw	s0,8(sp)
800014c8:	00c12083          	lw	ra,12(sp)
800014cc:	01010113          	addi	sp,sp,16
    NETIF_STATUS_CALLBACK(netif);
800014d0:	00030067          	jr	t1
}
800014d4:	00c12083          	lw	ra,12(sp)
800014d8:	00812403          	lw	s0,8(sp)
800014dc:	01010113          	addi	sp,sp,16
800014e0:	00008067          	ret
800014e4:	00008067          	ret

800014e8 <netif_remove>:
  if (netif == NULL) {
800014e8:	08050e63          	beqz	a0,80001584 <netif_remove+0x9c>
{
800014ec:	ff010113          	addi	sp,sp,-16
800014f0:	00812423          	sw	s0,8(sp)
800014f4:	00112623          	sw	ra,12(sp)
800014f8:	00912223          	sw	s1,4(sp)
  if (!ip4_addr_isany_val(*netif_ip4_addr(netif))) {
800014fc:	00452783          	lw	a5,4(a0)
80001500:	00050413          	mv	s0,a0
80001504:	02078063          	beqz	a5,80001524 <netif_remove+0x3c>
    netif_do_ip_addr_changed(netif_ip_addr4(netif), NULL);
80001508:	00450493          	addi	s1,a0,4
  udp_netif_ip_addr_changed(old_addr, new_addr);
8000150c:	00000593          	li	a1,0
80001510:	00048513          	mv	a0,s1
80001514:	038020ef          	jal	ra,8000354c <udp_netif_ip_addr_changed>
  raw_netif_ip_addr_changed(old_addr, new_addr);
80001518:	00000593          	li	a1,0
8000151c:	00048513          	mv	a0,s1
80001520:	4cc010ef          	jal	ra,800029ec <raw_netif_ip_addr_changed>
  if (netif_is_up(netif)) {
80001524:	03144783          	lbu	a5,49(s0)
80001528:	0017f793          	andi	a5,a5,1
8000152c:	00078663          	beqz	a5,80001538 <netif_remove+0x50>
    netif_set_down(netif);
80001530:	00040513          	mv	a0,s0
80001534:	f51ff0ef          	jal	ra,80001484 <netif_set_down>
  if (netif_default == netif) {
80001538:	8641a703          	lw	a4,-1948(gp) # 800003ac <netif_default>
8000153c:	00871463          	bne	a4,s0,80001544 <netif_remove+0x5c>
  netif_default = netif;
80001540:	8601a223          	sw	zero,-1948(gp) # 800003ac <netif_default>
  if (netif_list == netif) {
80001544:	8681a783          	lw	a5,-1944(gp) # 800003b0 <netif_list>
80001548:	02879263          	bne	a5,s0,8000156c <netif_remove+0x84>
    netif_list = netif->next;
8000154c:	00042783          	lw	a5,0(s0)
80001550:	86f1a423          	sw	a5,-1944(gp) # 800003b0 <netif_list>
}
80001554:	00c12083          	lw	ra,12(sp)
80001558:	00812403          	lw	s0,8(sp)
8000155c:	00412483          	lw	s1,4(sp)
80001560:	01010113          	addi	sp,sp,16
80001564:	00008067          	ret
80001568:	00070793          	mv	a5,a4
    NETIF_FOREACH(tmp_netif) {
8000156c:	fe0784e3          	beqz	a5,80001554 <netif_remove+0x6c>
      if (tmp_netif->next == netif) {
80001570:	0007a703          	lw	a4,0(a5)
80001574:	fe871ae3          	bne	a4,s0,80001568 <netif_remove+0x80>
        tmp_netif->next = netif->next;
80001578:	00042703          	lw	a4,0(s0)
8000157c:	00e7a023          	sw	a4,0(a5)
        break;
80001580:	fd5ff06f          	j	80001554 <netif_remove+0x6c>
80001584:	00008067          	ret

80001588 <netif_set_status_callback>:
  if (netif) {
80001588:	00050463          	beqz	a0,80001590 <netif_set_status_callback+0x8>
    netif->status_callback = status_callback;
8000158c:	00b52e23          	sw	a1,28(a0)
}
80001590:	00008067          	ret

80001594 <netif_set_link_up>:
  LWIP_ERROR("netif_set_link_up: invalid netif", netif != NULL, return);
80001594:	04050e63          	beqz	a0,800015f0 <netif_set_link_up+0x5c>
{
80001598:	ff010113          	addi	sp,sp,-16
8000159c:	00812423          	sw	s0,8(sp)
800015a0:	00112623          	sw	ra,12(sp)
  if (!(netif->flags & NETIF_FLAG_LINK_UP)) {
800015a4:	03154783          	lbu	a5,49(a0)
800015a8:	00050413          	mv	s0,a0
800015ac:	0047f713          	andi	a4,a5,4
800015b0:	02071863          	bnez	a4,800015e0 <netif_set_link_up+0x4c>
    netif_set_flags(netif, NETIF_FLAG_LINK_UP);
800015b4:	0047e793          	ori	a5,a5,4
800015b8:	02f508a3          	sb	a5,49(a0)
    netif_issue_reports(netif, NETIF_REPORT_TYPE_IPV4 | NETIF_REPORT_TYPE_IPV6);
800015bc:	00300593          	li	a1,3
800015c0:	b3dff0ef          	jal	ra,800010fc <netif_issue_reports>
    NETIF_LINK_CALLBACK(netif);
800015c4:	02042303          	lw	t1,32(s0)
800015c8:	00030c63          	beqz	t1,800015e0 <netif_set_link_up+0x4c>
800015cc:	00040513          	mv	a0,s0
}
800015d0:	00812403          	lw	s0,8(sp)
800015d4:	00c12083          	lw	ra,12(sp)
800015d8:	01010113          	addi	sp,sp,16
    NETIF_LINK_CALLBACK(netif);
800015dc:	00030067          	jr	t1
}
800015e0:	00c12083          	lw	ra,12(sp)
800015e4:	00812403          	lw	s0,8(sp)
800015e8:	01010113          	addi	sp,sp,16
800015ec:	00008067          	ret
800015f0:	00008067          	ret

800015f4 <netif_set_link_down>:
  LWIP_ERROR("netif_set_link_down: invalid netif", netif != NULL, return);
800015f4:	02050263          	beqz	a0,80001618 <netif_set_link_down+0x24>
  if (netif->flags & NETIF_FLAG_LINK_UP) {
800015f8:	03154703          	lbu	a4,49(a0)
800015fc:	00477693          	andi	a3,a4,4
80001600:	00068c63          	beqz	a3,80001618 <netif_set_link_down+0x24>
    NETIF_LINK_CALLBACK(netif);
80001604:	02052303          	lw	t1,32(a0)
    netif_clear_flags(netif, NETIF_FLAG_LINK_UP);
80001608:	ffb77713          	andi	a4,a4,-5
8000160c:	02e508a3          	sb	a4,49(a0)
    NETIF_LINK_CALLBACK(netif);
80001610:	00030463          	beqz	t1,80001618 <netif_set_link_down+0x24>
80001614:	00030067          	jr	t1
}
80001618:	00008067          	ret

8000161c <netif_set_link_callback>:
  if (netif) {
8000161c:	00050463          	beqz	a0,80001624 <netif_set_link_callback+0x8>
    netif->link_callback = link_callback;
80001620:	02b52023          	sw	a1,32(a0)
}
80001624:	00008067          	ret

80001628 <netif_get_by_index>:
        return netif; /* found! */
      }
    }
  }

  return NULL;
80001628:	00000793          	li	a5,0
  if (idx != NETIF_NO_INDEX) {
8000162c:	00050663          	beqz	a0,80001638 <netif_get_by_index+0x10>
    NETIF_FOREACH(netif) {
80001630:	8681a783          	lw	a5,-1944(gp) # 800003b0 <netif_list>
80001634:	00079663          	bnez	a5,80001640 <netif_get_by_index+0x18>
}
80001638:	00078513          	mv	a0,a5
8000163c:	00008067          	ret
      if (idx == netif_get_index(netif)) {
80001640:	0347c703          	lbu	a4,52(a5)
80001644:	00170713          	addi	a4,a4,1
80001648:	0ff77713          	andi	a4,a4,255
8000164c:	fea706e3          	beq	a4,a0,80001638 <netif_get_by_index+0x10>
    NETIF_FOREACH(netif) {
80001650:	0007a783          	lw	a5,0(a5)
80001654:	fe1ff06f          	j	80001634 <netif_get_by_index+0xc>

80001658 <netif_index_to_name>:
{
80001658:	ff010113          	addi	sp,sp,-16
8000165c:	00812423          	sw	s0,8(sp)
80001660:	00912223          	sw	s1,4(sp)
80001664:	00112623          	sw	ra,12(sp)
80001668:	00050493          	mv	s1,a0
8000166c:	00058413          	mv	s0,a1
  struct netif *netif = netif_get_by_index(idx);
80001670:	fb9ff0ef          	jal	ra,80001628 <netif_get_by_index>
  if (netif != NULL) {
80001674:	02050463          	beqz	a0,8000169c <netif_index_to_name+0x44>
    name[0] = netif->name[0];
80001678:	03254783          	lbu	a5,50(a0)
    lwip_itoa(&name[2], NETIF_NAMESIZE - 2, netif_index_to_num(idx));
8000167c:	fff48613          	addi	a2,s1,-1
80001680:	00400593          	li	a1,4
    name[0] = netif->name[0];
80001684:	00f40023          	sb	a5,0(s0)
    name[1] = netif->name[1];
80001688:	03354783          	lbu	a5,51(a0)
    lwip_itoa(&name[2], NETIF_NAMESIZE - 2, netif_index_to_num(idx));
8000168c:	00240513          	addi	a0,s0,2
    name[1] = netif->name[1];
80001690:	00f400a3          	sb	a5,1(s0)
    lwip_itoa(&name[2], NETIF_NAMESIZE - 2, netif_index_to_num(idx));
80001694:	bc8ff0ef          	jal	ra,80000a5c <lwip_itoa>
    return name;
80001698:	00040513          	mv	a0,s0
}
8000169c:	00c12083          	lw	ra,12(sp)
800016a0:	00812403          	lw	s0,8(sp)
800016a4:	00412483          	lw	s1,4(sp)
800016a8:	01010113          	addi	sp,sp,16
800016ac:	00008067          	ret

800016b0 <netif_find>:
  struct netif *netif;
  u8_t num;

  LWIP_ASSERT_CORE_LOCKED();

  if (name == NULL) {
800016b0:	00051663          	bnez	a0,800016bc <netif_find+0xc>
    return NULL;
800016b4:	00000513          	li	a0,0
      return netif;
    }
  }
  LWIP_DEBUGF(NETIF_DEBUG, ("netif_find: didn't find %c%c\n", name[0], name[1]));
  return NULL;
}
800016b8:	00008067          	ret
{
800016bc:	ff010113          	addi	sp,sp,-16
800016c0:	00812423          	sw	s0,8(sp)
800016c4:	00050413          	mv	s0,a0
  num = (u8_t)atoi(&name[2]);
800016c8:	00250513          	addi	a0,a0,2
{
800016cc:	00112623          	sw	ra,12(sp)
  num = (u8_t)atoi(&name[2]);
800016d0:	2a5040ef          	jal	ra,80006174 <atoi>
800016d4:	0ff57793          	andi	a5,a0,255
  NETIF_FOREACH(netif) {
800016d8:	8681a503          	lw	a0,-1944(gp) # 800003b0 <netif_list>
800016dc:	00051c63          	bnez	a0,800016f4 <netif_find+0x44>
    return NULL;
800016e0:	00000513          	li	a0,0
}
800016e4:	00c12083          	lw	ra,12(sp)
800016e8:	00812403          	lw	s0,8(sp)
800016ec:	01010113          	addi	sp,sp,16
800016f0:	00008067          	ret
    if (num == netif->num &&
800016f4:	03454703          	lbu	a4,52(a0)
800016f8:	00f71e63          	bne	a4,a5,80001714 <netif_find+0x64>
800016fc:	00044683          	lbu	a3,0(s0)
80001700:	03254703          	lbu	a4,50(a0)
80001704:	00e69863          	bne	a3,a4,80001714 <netif_find+0x64>
        name[0] == netif->name[0] &&
80001708:	00144683          	lbu	a3,1(s0)
8000170c:	03354703          	lbu	a4,51(a0)
80001710:	fce68ae3          	beq	a3,a4,800016e4 <netif_find+0x34>
  NETIF_FOREACH(netif) {
80001714:	00052503          	lw	a0,0(a0)
80001718:	fc5ff06f          	j	800016dc <netif_find+0x2c>

8000171c <netif_name_to_index>:
{
8000171c:	ff010113          	addi	sp,sp,-16
80001720:	00112623          	sw	ra,12(sp)
  struct netif *netif = netif_find(name);
80001724:	f8dff0ef          	jal	ra,800016b0 <netif_find>
  if (netif != NULL) {
80001728:	00050e63          	beqz	a0,80001744 <netif_name_to_index+0x28>
    return netif_get_index(netif);
8000172c:	03454503          	lbu	a0,52(a0)
80001730:	00150513          	addi	a0,a0,1
80001734:	0ff57513          	andi	a0,a0,255
}
80001738:	00c12083          	lw	ra,12(sp)
8000173c:	01010113          	addi	sp,sp,16
80001740:	00008067          	ret
  return NETIF_NO_INDEX;
80001744:	00000513          	li	a0,0
80001748:	ff1ff06f          	j	80001738 <netif_name_to_index+0x1c>

8000174c <pbuf_skip_const>:
{
  u16_t offset_left = in_offset;
  const struct pbuf *q = in;

  /* get the correct pbuf */
  while ((q != NULL) && (q->len <= offset_left)) {
8000174c:	00050663          	beqz	a0,80001758 <pbuf_skip_const+0xc>
80001750:	00a55783          	lhu	a5,10(a0)
80001754:	00f5f863          	bgeu	a1,a5,80001764 <pbuf_skip_const+0x18>
    offset_left = (u16_t)(offset_left - q->len);
    q = q->next;
  }
  if (out_offset != NULL) {
80001758:	00060463          	beqz	a2,80001760 <pbuf_skip_const+0x14>
    *out_offset = offset_left;
8000175c:	00b61023          	sh	a1,0(a2)
  }
  return q;
}
80001760:	00008067          	ret
    offset_left = (u16_t)(offset_left - q->len);
80001764:	40f585b3          	sub	a1,a1,a5
80001768:	01059593          	slli	a1,a1,0x10
8000176c:	0105d593          	srli	a1,a1,0x10
    q = q->next;
80001770:	00052503          	lw	a0,0(a0)
80001774:	fd9ff06f          	j	8000174c <pbuf_skip_const>

80001778 <pbuf_add_header_impl>:
{
80001778:	00050793          	mv	a5,a0
  if ((p == NULL) || (header_size_increment > 0xFFFF)) {
8000177c:	08050263          	beqz	a0,80001800 <pbuf_add_header_impl+0x88>
80001780:	00010737          	lui	a4,0x10
    return 1;
80001784:	00100513          	li	a0,1
  if ((p == NULL) || (header_size_increment > 0xFFFF)) {
80001788:	06e5fe63          	bgeu	a1,a4,80001804 <pbuf_add_header_impl+0x8c>
    return 0;
8000178c:	00000513          	li	a0,0
  if (header_size_increment == 0) {
80001790:	06058a63          	beqz	a1,80001804 <pbuf_add_header_impl+0x8c>
  if ((u16_t)(increment_magnitude + p->tot_len) < increment_magnitude) {
80001794:	0087d703          	lhu	a4,8(a5)
  increment_magnitude = (u16_t)header_size_increment;
80001798:	01059693          	slli	a3,a1,0x10
8000179c:	0106d693          	srli	a3,a3,0x10
  if ((u16_t)(increment_magnitude + p->tot_len) < increment_magnitude) {
800017a0:	00e68733          	add	a4,a3,a4
800017a4:	01071713          	slli	a4,a4,0x10
800017a8:	01075713          	srli	a4,a4,0x10
    return 1;
800017ac:	00100513          	li	a0,1
  if ((u16_t)(increment_magnitude + p->tot_len) < increment_magnitude) {
800017b0:	04d76a63          	bltu	a4,a3,80001804 <pbuf_add_header_impl+0x8c>
  if (type_internal & PBUF_TYPE_FLAG_STRUCT_DATA_CONTIGUOUS) {
800017b4:	00c78503          	lb	a0,12(a5)
800017b8:	00055e63          	bgez	a0,800017d4 <pbuf_add_header_impl+0x5c>
    payload = (u8_t *)p->payload - header_size_increment;
800017bc:	0047a603          	lw	a2,4(a5)
    return 1;
800017c0:	00100513          	li	a0,1
    payload = (u8_t *)p->payload - header_size_increment;
800017c4:	40b605b3          	sub	a1,a2,a1
    if ((u8_t *)payload < (u8_t *)p + SIZEOF_STRUCT_PBUF) {
800017c8:	01078613          	addi	a2,a5,16
800017cc:	00c5fc63          	bgeu	a1,a2,800017e4 <pbuf_add_header_impl+0x6c>
800017d0:	00008067          	ret
    return 1;
800017d4:	00100513          	li	a0,1
    if (force) {
800017d8:	02060663          	beqz	a2,80001804 <pbuf_add_header_impl+0x8c>
      payload = (u8_t *)p->payload - header_size_increment;
800017dc:	0047a603          	lw	a2,4(a5)
800017e0:	40b605b3          	sub	a1,a2,a1
  p->len = (u16_t)(p->len + increment_magnitude);
800017e4:	00a7d603          	lhu	a2,10(a5)
  p->payload = payload;
800017e8:	00b7a223          	sw	a1,4(a5)
  p->tot_len = (u16_t)(p->tot_len + increment_magnitude);
800017ec:	00e79423          	sh	a4,8(a5)
  p->len = (u16_t)(p->len + increment_magnitude);
800017f0:	00c686b3          	add	a3,a3,a2
800017f4:	00d79523          	sh	a3,10(a5)
  return 0;
800017f8:	00000513          	li	a0,0
800017fc:	00008067          	ret
    return 1;
80001800:	00100513          	li	a0,1
}
80001804:	00008067          	ret

80001808 <pbuf_alloc_reference>:
{
80001808:	fe010113          	addi	sp,sp,-32
8000180c:	00812c23          	sw	s0,24(sp)
80001810:	00050413          	mv	s0,a0
  p = (struct pbuf *)memp_malloc(MEMP_PBUF);
80001814:	00500513          	li	a0,5
{
80001818:	00112e23          	sw	ra,28(sp)
8000181c:	00b12623          	sw	a1,12(sp)
80001820:	00c12423          	sw	a2,8(sp)
  p = (struct pbuf *)memp_malloc(MEMP_PBUF);
80001824:	831ff0ef          	jal	ra,80001054 <memp_malloc>
  if (p == NULL) {
80001828:	02050663          	beqz	a0,80001854 <pbuf_alloc_reference+0x4c>
  p->tot_len = tot_len;
8000182c:	00c12583          	lw	a1,12(sp)
  p->type_internal = (u8_t)type;
80001830:	00812603          	lw	a2,8(sp)
  p->ref = 1;
80001834:	00100793          	li	a5,1
  p->next = NULL;
80001838:	00052023          	sw	zero,0(a0)
  p->payload = payload;
8000183c:	00852223          	sw	s0,4(a0)
  p->tot_len = tot_len;
80001840:	00b51423          	sh	a1,8(a0)
  p->len = len;
80001844:	00b51523          	sh	a1,10(a0)
  p->type_internal = (u8_t)type;
80001848:	00c50623          	sb	a2,12(a0)
  p->flags = flags;
8000184c:	000506a3          	sb	zero,13(a0)
  p->ref = 1;
80001850:	00f51723          	sh	a5,14(a0)
}
80001854:	01c12083          	lw	ra,28(sp)
80001858:	01812403          	lw	s0,24(sp)
8000185c:	02010113          	addi	sp,sp,32
80001860:	00008067          	ret

80001864 <pbuf_alloced_custom>:
  if (LWIP_MEM_ALIGN_SIZE(offset) + length > payload_mem_len) {
80001864:	01051513          	slli	a0,a0,0x10
80001868:	01055513          	srli	a0,a0,0x10
8000186c:	00350513          	addi	a0,a0,3
80001870:	ffc57513          	andi	a0,a0,-4
80001874:	00a58833          	add	a6,a1,a0
80001878:	0307ec63          	bltu	a5,a6,800018b0 <pbuf_alloced_custom+0x4c>
  if (payload_mem != NULL) {
8000187c:	00070463          	beqz	a4,80001884 <pbuf_alloced_custom+0x20>
    payload = (u8_t *)payload_mem + LWIP_MEM_ALIGN_SIZE(offset);
80001880:	00a70733          	add	a4,a4,a0
  p->flags = flags;
80001884:	00200793          	li	a5,2
80001888:	00f686a3          	sb	a5,13(a3)
  p->ref = 1;
8000188c:	00100793          	li	a5,1
  pbuf_init_alloced_pbuf(&p->pbuf, payload, length, length, type, PBUF_FLAG_IS_CUSTOM);
80001890:	00068513          	mv	a0,a3
  p->next = NULL;
80001894:	0006a023          	sw	zero,0(a3)
  p->payload = payload;
80001898:	00e6a223          	sw	a4,4(a3)
  p->tot_len = tot_len;
8000189c:	00b69423          	sh	a1,8(a3)
  p->len = len;
800018a0:	00b69523          	sh	a1,10(a3)
  p->type_internal = (u8_t)type;
800018a4:	00c68623          	sb	a2,12(a3)
  p->ref = 1;
800018a8:	00f69723          	sh	a5,14(a3)
  return &p->pbuf;
800018ac:	00008067          	ret
    return NULL;
800018b0:	00000513          	li	a0,0
}
800018b4:	00008067          	ret

800018b8 <pbuf_add_header>:
  return pbuf_add_header_impl(p, header_size_increment, 0);
800018b8:	00000613          	li	a2,0
800018bc:	ebdff06f          	j	80001778 <pbuf_add_header_impl>

800018c0 <pbuf_add_header_force>:
  return pbuf_add_header_impl(p, header_size_increment, 1);
800018c0:	00100613          	li	a2,1
800018c4:	eb5ff06f          	j	80001778 <pbuf_add_header_impl>

800018c8 <pbuf_remove_header>:
{
800018c8:	00050793          	mv	a5,a0
  if ((p == NULL) || (header_size_decrement > 0xFFFF)) {
800018cc:	04050a63          	beqz	a0,80001920 <pbuf_remove_header+0x58>
800018d0:	00010737          	lui	a4,0x10
    return 1;
800018d4:	00100513          	li	a0,1
  if ((p == NULL) || (header_size_decrement > 0xFFFF)) {
800018d8:	04e5f663          	bgeu	a1,a4,80001924 <pbuf_remove_header+0x5c>
    return 0;
800018dc:	00000513          	li	a0,0
  if (header_size_decrement == 0) {
800018e0:	04058263          	beqz	a1,80001924 <pbuf_remove_header+0x5c>
  LWIP_ERROR("increment_magnitude <= p->len", (increment_magnitude <= p->len), return 1;);
800018e4:	00a7d703          	lhu	a4,10(a5)
  increment_magnitude = (u16_t)header_size_decrement;
800018e8:	01059693          	slli	a3,a1,0x10
800018ec:	0106d693          	srli	a3,a3,0x10
    return 1;
800018f0:	00100513          	li	a0,1
  LWIP_ERROR("increment_magnitude <= p->len", (increment_magnitude <= p->len), return 1;);
800018f4:	02d76863          	bltu	a4,a3,80001924 <pbuf_remove_header+0x5c>
  p->len = (u16_t)(p->len - increment_magnitude);
800018f8:	40d70733          	sub	a4,a4,a3
  p->payload = (u8_t *)p->payload + header_size_decrement;
800018fc:	0047a603          	lw	a2,4(a5)
  p->len = (u16_t)(p->len - increment_magnitude);
80001900:	00e79523          	sh	a4,10(a5)
  p->tot_len = (u16_t)(p->tot_len - increment_magnitude);
80001904:	0087d703          	lhu	a4,8(a5)
  p->payload = (u8_t *)p->payload + header_size_decrement;
80001908:	00b605b3          	add	a1,a2,a1
8000190c:	00b7a223          	sw	a1,4(a5)
  p->tot_len = (u16_t)(p->tot_len - increment_magnitude);
80001910:	40d70733          	sub	a4,a4,a3
80001914:	00e79423          	sh	a4,8(a5)
  return 0;
80001918:	00000513          	li	a0,0
8000191c:	00008067          	ret
    return 1;
80001920:	00100513          	li	a0,1
}
80001924:	00008067          	ret

80001928 <pbuf_header>:
  if (header_size_increment < 0) {
80001928:	0005d663          	bgez	a1,80001934 <pbuf_header+0xc>
    return pbuf_remove_header(p, (size_t) - header_size_increment);
8000192c:	40b005b3          	neg	a1,a1
80001930:	f99ff06f          	j	800018c8 <pbuf_remove_header>
    return pbuf_add_header_impl(p, (size_t)header_size_increment, force);
80001934:	00000613          	li	a2,0
80001938:	e41ff06f          	j	80001778 <pbuf_add_header_impl>

8000193c <pbuf_header_force>:
  if (header_size_increment < 0) {
8000193c:	0005d663          	bgez	a1,80001948 <pbuf_header_force+0xc>
    return pbuf_remove_header(p, (size_t) - header_size_increment);
80001940:	40b005b3          	neg	a1,a1
80001944:	f85ff06f          	j	800018c8 <pbuf_remove_header>
    return pbuf_add_header_impl(p, (size_t)header_size_increment, force);
80001948:	00100613          	li	a2,1
8000194c:	e2dff06f          	j	80001778 <pbuf_add_header_impl>

80001950 <pbuf_free>:
{
80001950:	fd010113          	addi	sp,sp,-48
80001954:	02812423          	sw	s0,40(sp)
80001958:	02112623          	sw	ra,44(sp)
8000195c:	02912223          	sw	s1,36(sp)
80001960:	03212023          	sw	s2,32(sp)
80001964:	01312e23          	sw	s3,28(sp)
    return 0;
80001968:	00000413          	li	s0,0
  if (p == NULL) {
8000196c:	06050463          	beqz	a0,800019d4 <pbuf_free+0x84>
80001970:	00050593          	mv	a1,a0
  count = 0;
80001974:	00000413          	li	s0,0
        if (alloc_src == PBUF_TYPE_ALLOC_SRC_MASK_STD_MEMP_PBUF_POOL) {
80001978:	00200913          	li	s2,2
        } else if (alloc_src == PBUF_TYPE_ALLOC_SRC_MASK_STD_MEMP_PBUF) {
8000197c:	00100993          	li	s3,1
80001980:	00b12623          	sw	a1,12(sp)
    SYS_ARCH_PROTECT(old_level);
80001984:	c35fe0ef          	jal	ra,800005b8 <sys_arch_protect>
    ref = --(p->ref);
80001988:	00c12583          	lw	a1,12(sp)
8000198c:	00e5c483          	lbu	s1,14(a1)
80001990:	fff48493          	addi	s1,s1,-1
80001994:	0ff4f493          	andi	s1,s1,255
80001998:	00958723          	sb	s1,14(a1)
    SYS_ARCH_UNPROTECT(old_level);
8000199c:	c21fe0ef          	jal	ra,800005bc <sys_arch_unprotect>
    if (ref == 0) {
800019a0:	02049a63          	bnez	s1,800019d4 <pbuf_free+0x84>
      q = p->next;
800019a4:	00c12583          	lw	a1,12(sp)
      if ((p->flags & PBUF_FLAG_IS_CUSTOM) != 0) {
800019a8:	00d5c783          	lbu	a5,13(a1)
      q = p->next;
800019ac:	0005a483          	lw	s1,0(a1)
      if ((p->flags & PBUF_FLAG_IS_CUSTOM) != 0) {
800019b0:	0027f793          	andi	a5,a5,2
800019b4:	04078063          	beqz	a5,800019f4 <pbuf_free+0xa4>
        pc->custom_free_function(p);
800019b8:	0105a783          	lw	a5,16(a1)
800019bc:	00058513          	mv	a0,a1
800019c0:	000780e7          	jalr	a5
      count++;
800019c4:	00140413          	addi	s0,s0,1
800019c8:	0ff47413          	andi	s0,s0,255
      p = q;
800019cc:	00048593          	mv	a1,s1
  while (p != NULL) {
800019d0:	fa0498e3          	bnez	s1,80001980 <pbuf_free+0x30>
}
800019d4:	00040513          	mv	a0,s0
800019d8:	02c12083          	lw	ra,44(sp)
800019dc:	02812403          	lw	s0,40(sp)
800019e0:	02412483          	lw	s1,36(sp)
800019e4:	02012903          	lw	s2,32(sp)
800019e8:	01c12983          	lw	s3,28(sp)
800019ec:	03010113          	addi	sp,sp,48
800019f0:	00008067          	ret
      alloc_src = pbuf_get_allocsrc(p);
800019f4:	00c5c783          	lbu	a5,12(a1)
          memp_free(MEMP_PBUF_POOL, p);
800019f8:	00600513          	li	a0,6
      alloc_src = pbuf_get_allocsrc(p);
800019fc:	00f7f793          	andi	a5,a5,15
        if (alloc_src == PBUF_TYPE_ALLOC_SRC_MASK_STD_MEMP_PBUF_POOL) {
80001a00:	01278663          	beq	a5,s2,80001a0c <pbuf_free+0xbc>
        } else if (alloc_src == PBUF_TYPE_ALLOC_SRC_MASK_STD_MEMP_PBUF) {
80001a04:	01379863          	bne	a5,s3,80001a14 <pbuf_free+0xc4>
          memp_free(MEMP_PBUF, p);
80001a08:	00500513          	li	a0,5
80001a0c:	eacff0ef          	jal	ra,800010b8 <memp_free>
80001a10:	fb5ff06f          	j	800019c4 <pbuf_free+0x74>
        } else if (alloc_src == PBUF_TYPE_ALLOC_SRC_MASK_STD_HEAP) {
80001a14:	fa0798e3          	bnez	a5,800019c4 <pbuf_free+0x74>
          mem_free(p);
80001a18:	00058513          	mv	a0,a1
80001a1c:	de0ff0ef          	jal	ra,80000ffc <free>
80001a20:	fa5ff06f          	j	800019c4 <pbuf_free+0x74>

80001a24 <pbuf_alloc>:
  switch (type) {
80001a24:	04100793          	li	a5,65
80001a28:	1af60863          	beq	a2,a5,80001bd8 <pbuf_alloc+0x1b4>
{
80001a2c:	fd010113          	addi	sp,sp,-48
80001a30:	02112623          	sw	ra,44(sp)
80001a34:	02812423          	sw	s0,40(sp)
80001a38:	02912223          	sw	s1,36(sp)
80001a3c:	03212023          	sw	s2,32(sp)
80001a40:	01312e23          	sw	s3,28(sp)
80001a44:	01412c23          	sw	s4,24(sp)
  switch (type) {
80001a48:	02c7ea63          	bltu	a5,a2,80001a7c <pbuf_alloc+0x58>
80001a4c:	00100793          	li	a5,1
80001a50:	0af60c63          	beq	a2,a5,80001b08 <pbuf_alloc+0xe4>
          return NULL;
80001a54:	00000493          	li	s1,0
}
80001a58:	02c12083          	lw	ra,44(sp)
80001a5c:	02812403          	lw	s0,40(sp)
80001a60:	00048513          	mv	a0,s1
80001a64:	02012903          	lw	s2,32(sp)
80001a68:	02412483          	lw	s1,36(sp)
80001a6c:	01c12983          	lw	s3,28(sp)
80001a70:	01812a03          	lw	s4,24(sp)
80001a74:	03010113          	addi	sp,sp,48
80001a78:	00008067          	ret
80001a7c:	01051413          	slli	s0,a0,0x10
  switch (type) {
80001a80:	18200793          	li	a5,386
80001a84:	01045413          	srli	s0,s0,0x10
80001a88:	0af60263          	beq	a2,a5,80001b2c <pbuf_alloc+0x108>
80001a8c:	28000793          	li	a5,640
80001a90:	fcf612e3          	bne	a2,a5,80001a54 <pbuf_alloc+0x30>
      u16_t payload_len = (u16_t)(LWIP_MEM_ALIGN_SIZE(offset) + LWIP_MEM_ALIGN_SIZE(length));
80001a94:	00340413          	addi	s0,s0,3
80001a98:	00358793          	addi	a5,a1,3
80001a9c:	ffc47413          	andi	s0,s0,-4
80001aa0:	ffc7f793          	andi	a5,a5,-4
80001aa4:	00f40433          	add	s0,s0,a5
      mem_size_t alloc_len = (mem_size_t)(LWIP_MEM_ALIGN_SIZE(SIZEOF_STRUCT_PBUF) + payload_len);
80001aa8:	01041413          	slli	s0,s0,0x10
80001aac:	01045413          	srli	s0,s0,0x10
      u16_t payload_len = (u16_t)(LWIP_MEM_ALIGN_SIZE(offset) + LWIP_MEM_ALIGN_SIZE(length));
80001ab0:	00b12623          	sw	a1,12(sp)
80001ab4:	00050913          	mv	s2,a0
      mem_size_t alloc_len = (mem_size_t)(LWIP_MEM_ALIGN_SIZE(SIZEOF_STRUCT_PBUF) + payload_len);
80001ab8:	01040513          	addi	a0,s0,16
      if ((payload_len < LWIP_MEM_ALIGN_SIZE(length)) ||
80001abc:	f8f46ce3          	bltu	s0,a5,80001a54 <pbuf_alloc+0x30>
80001ac0:	f8f56ae3          	bltu	a0,a5,80001a54 <pbuf_alloc+0x30>
      p = (struct pbuf *)mem_malloc(alloc_len);
80001ac4:	d04ff0ef          	jal	ra,80000fc8 <malloc>
80001ac8:	00050493          	mv	s1,a0
      if (p == NULL) {
80001acc:	f80506e3          	beqz	a0,80001a58 <pbuf_alloc+0x34>
      pbuf_init_alloced_pbuf(p, LWIP_MEM_ALIGN((void *)((u8_t *)p + SIZEOF_STRUCT_PBUF + offset)),
80001ad0:	01091913          	slli	s2,s2,0x10
80001ad4:	01095913          	srli	s2,s2,0x10
  p->tot_len = tot_len;
80001ad8:	00c12583          	lw	a1,12(sp)
      pbuf_init_alloced_pbuf(p, LWIP_MEM_ALIGN((void *)((u8_t *)p + SIZEOF_STRUCT_PBUF + offset)),
80001adc:	01250933          	add	s2,a0,s2
80001ae0:	01390913          	addi	s2,s2,19
  p->type_internal = (u8_t)type;
80001ae4:	000107b7          	lui	a5,0x10
      pbuf_init_alloced_pbuf(p, LWIP_MEM_ALIGN((void *)((u8_t *)p + SIZEOF_STRUCT_PBUF + offset)),
80001ae8:	ffc97913          	andi	s2,s2,-4
  p->type_internal = (u8_t)type;
80001aec:	08078793          	addi	a5,a5,128 # 10080 <_stack_size+0xf880>
  p->next = NULL;
80001af0:	00052023          	sw	zero,0(a0)
  p->payload = payload;
80001af4:	01252223          	sw	s2,4(a0)
  p->tot_len = tot_len;
80001af8:	00b51423          	sh	a1,8(a0)
  p->len = len;
80001afc:	00b51523          	sh	a1,10(a0)
  p->type_internal = (u8_t)type;
80001b00:	00f52623          	sw	a5,12(a0)
      break;
80001b04:	f55ff06f          	j	80001a58 <pbuf_alloc+0x34>
}
80001b08:	02812403          	lw	s0,40(sp)
80001b0c:	02c12083          	lw	ra,44(sp)
80001b10:	02412483          	lw	s1,36(sp)
80001b14:	02012903          	lw	s2,32(sp)
80001b18:	01c12983          	lw	s3,28(sp)
80001b1c:	01812a03          	lw	s4,24(sp)
      p = pbuf_alloc_reference(NULL, length, type);
80001b20:	00000513          	li	a0,0
}
80001b24:	03010113          	addi	sp,sp,48
      p = pbuf_alloc_reference(NULL, length, type);
80001b28:	ce1ff06f          	j	80001808 <pbuf_alloc_reference>
  p->type_internal = (u8_t)type;
80001b2c:	00010937          	lui	s2,0x10
      last = NULL;
80001b30:	00000993          	li	s3,0
      p = NULL;
80001b34:	00000493          	li	s1,0
        qlen = LWIP_MIN(rem_len, (u16_t)(PBUF_POOL_BUFSIZE_ALIGNED - LWIP_MEM_ALIGN_SIZE(offset)));
80001b38:	5ec00a13          	li	s4,1516
  p->type_internal = (u8_t)type;
80001b3c:	08290913          	addi	s2,s2,130 # 10082 <_stack_size+0xf882>
        q = (struct pbuf *)memp_malloc(MEMP_PBUF_POOL);
80001b40:	00600513          	li	a0,6
80001b44:	00b12623          	sw	a1,12(sp)
80001b48:	d0cff0ef          	jal	ra,80001054 <memp_malloc>
        if (q == NULL) {
80001b4c:	00c12583          	lw	a1,12(sp)
80001b50:	00051a63          	bnez	a0,80001b64 <pbuf_alloc+0x140>
          if (p) {
80001b54:	f00480e3          	beqz	s1,80001a54 <pbuf_alloc+0x30>
            pbuf_free(p);
80001b58:	00048513          	mv	a0,s1
80001b5c:	df5ff0ef          	jal	ra,80001950 <pbuf_free>
80001b60:	ef5ff06f          	j	80001a54 <pbuf_alloc+0x30>
        qlen = LWIP_MIN(rem_len, (u16_t)(PBUF_POOL_BUFSIZE_ALIGNED - LWIP_MEM_ALIGN_SIZE(offset)));
80001b64:	00340793          	addi	a5,s0,3
80001b68:	ffc7f793          	andi	a5,a5,-4
80001b6c:	40fa07b3          	sub	a5,s4,a5
80001b70:	00078713          	mv	a4,a5
80001b74:	01079793          	slli	a5,a5,0x10
80001b78:	0107d793          	srli	a5,a5,0x10
80001b7c:	00f5f463          	bgeu	a1,a5,80001b84 <pbuf_alloc+0x160>
80001b80:	00058713          	mv	a4,a1
        pbuf_init_alloced_pbuf(q, LWIP_MEM_ALIGN((void *)((u8_t *)q + SIZEOF_STRUCT_PBUF + offset)),
80001b84:	00850433          	add	s0,a0,s0
80001b88:	01340413          	addi	s0,s0,19
80001b8c:	ffc47413          	andi	s0,s0,-4
        qlen = LWIP_MIN(rem_len, (u16_t)(PBUF_POOL_BUFSIZE_ALIGNED - LWIP_MEM_ALIGN_SIZE(offset)));
80001b90:	01071793          	slli	a5,a4,0x10
  p->next = NULL;
80001b94:	00052023          	sw	zero,0(a0)
  p->payload = payload;
80001b98:	00852223          	sw	s0,4(a0)
  p->tot_len = tot_len;
80001b9c:	00b51423          	sh	a1,8(a0)
  p->len = len;
80001ba0:	00e51523          	sh	a4,10(a0)
  p->type_internal = (u8_t)type;
80001ba4:	01252623          	sw	s2,12(a0)
        qlen = LWIP_MIN(rem_len, (u16_t)(PBUF_POOL_BUFSIZE_ALIGNED - LWIP_MEM_ALIGN_SIZE(offset)));
80001ba8:	0107d793          	srli	a5,a5,0x10
        if (p == NULL) {
80001bac:	02048263          	beqz	s1,80001bd0 <pbuf_alloc+0x1ac>
          last->next = q;
80001bb0:	00a9a023          	sw	a0,0(s3)
        rem_len = (u16_t)(rem_len - qlen);
80001bb4:	40f585b3          	sub	a1,a1,a5
80001bb8:	01059593          	slli	a1,a1,0x10
80001bbc:	0105d593          	srli	a1,a1,0x10
        offset = 0;
80001bc0:	00000413          	li	s0,0
80001bc4:	00050993          	mv	s3,a0
      } while (rem_len > 0);
80001bc8:	f6059ce3          	bnez	a1,80001b40 <pbuf_alloc+0x11c>
80001bcc:	e8dff06f          	j	80001a58 <pbuf_alloc+0x34>
80001bd0:	00050493          	mv	s1,a0
80001bd4:	fe1ff06f          	j	80001bb4 <pbuf_alloc+0x190>
      p = pbuf_alloc_reference(NULL, length, type);
80001bd8:	00000513          	li	a0,0
80001bdc:	f4dff06f          	j	80001b28 <pbuf_alloc+0x104>

80001be0 <pbuf_realloc>:
  if (new_len >= p->tot_len) {
80001be0:	00855783          	lhu	a5,8(a0)
80001be4:	0af5f463          	bgeu	a1,a5,80001c8c <pbuf_realloc+0xac>
{
80001be8:	ff010113          	addi	sp,sp,-16
80001bec:	00812423          	sw	s0,8(sp)
80001bf0:	00912223          	sw	s1,4(sp)
80001bf4:	00112623          	sw	ra,12(sp)
80001bf8:	00050413          	mv	s0,a0
80001bfc:	00058493          	mv	s1,a1
    q->tot_len = (u16_t)(q->tot_len - shrink);
80001c00:	40f587b3          	sub	a5,a1,a5
  while (rem_len > q->len) {
80001c04:	00a45583          	lhu	a1,10(s0)
80001c08:	0695e263          	bltu	a1,s1,80001c6c <pbuf_realloc+0x8c>
  if (pbuf_match_allocsrc(q, PBUF_TYPE_ALLOC_SRC_MASK_STD_HEAP) && (rem_len != q->len)
80001c0c:	00c44783          	lbu	a5,12(s0)
80001c10:	00f7f793          	andi	a5,a5,15
80001c14:	02079663          	bnez	a5,80001c40 <pbuf_realloc+0x60>
80001c18:	02958463          	beq	a1,s1,80001c40 <pbuf_realloc+0x60>
      && ((q->flags & PBUF_FLAG_IS_CUSTOM) == 0)
80001c1c:	00d44783          	lbu	a5,13(s0)
80001c20:	0027f793          	andi	a5,a5,2
80001c24:	00079e63          	bnez	a5,80001c40 <pbuf_realloc+0x60>
    q = (struct pbuf *)mem_trim(q, (mem_size_t)(((u8_t *)q->payload - (u8_t *)q) + rem_len));
80001c28:	00442583          	lw	a1,4(s0)
80001c2c:	00040513          	mv	a0,s0
80001c30:	408585b3          	sub	a1,a1,s0
80001c34:	009585b3          	add	a1,a1,s1
80001c38:	b8cff0ef          	jal	ra,80000fc4 <mem_trim>
80001c3c:	00050413          	mv	s0,a0
  if (q->next != NULL) {
80001c40:	00042503          	lw	a0,0(s0)
  q->len = rem_len;
80001c44:	00941523          	sh	s1,10(s0)
  q->tot_len = q->len;
80001c48:	00941423          	sh	s1,8(s0)
  if (q->next != NULL) {
80001c4c:	00050463          	beqz	a0,80001c54 <pbuf_realloc+0x74>
    pbuf_free(q->next);
80001c50:	d01ff0ef          	jal	ra,80001950 <pbuf_free>
  q->next = NULL;
80001c54:	00042023          	sw	zero,0(s0)
}
80001c58:	00c12083          	lw	ra,12(sp)
80001c5c:	00812403          	lw	s0,8(sp)
80001c60:	00412483          	lw	s1,4(sp)
80001c64:	01010113          	addi	sp,sp,16
80001c68:	00008067          	ret
    q->tot_len = (u16_t)(q->tot_len - shrink);
80001c6c:	00845703          	lhu	a4,8(s0)
    rem_len = (u16_t)(rem_len - q->len);
80001c70:	40b484b3          	sub	s1,s1,a1
80001c74:	01049493          	slli	s1,s1,0x10
    q->tot_len = (u16_t)(q->tot_len - shrink);
80001c78:	00e78733          	add	a4,a5,a4
80001c7c:	00e41423          	sh	a4,8(s0)
    rem_len = (u16_t)(rem_len - q->len);
80001c80:	0104d493          	srli	s1,s1,0x10
    q = q->next;
80001c84:	00042403          	lw	s0,0(s0)
    LWIP_ASSERT("pbuf_realloc: q != NULL", q != NULL);
80001c88:	f7dff06f          	j	80001c04 <pbuf_realloc+0x24>
80001c8c:	00008067          	ret

80001c90 <pbuf_free_header>:
{
80001c90:	fe010113          	addi	sp,sp,-32
80001c94:	00812c23          	sw	s0,24(sp)
80001c98:	00112e23          	sw	ra,28(sp)
80001c9c:	00912a23          	sw	s1,20(sp)
  struct pbuf *p = q;
80001ca0:	00050413          	mv	s0,a0
  while (free_left && p) {
80001ca4:	04058263          	beqz	a1,80001ce8 <pbuf_free_header+0x58>
80001ca8:	04040063          	beqz	s0,80001ce8 <pbuf_free_header+0x58>
    if (free_left >= p->len) {
80001cac:	00a45783          	lhu	a5,10(s0)
80001cb0:	02f5e863          	bltu	a1,a5,80001ce0 <pbuf_free_header+0x50>
      free_left = (u16_t)(free_left - p->len);
80001cb4:	40f585b3          	sub	a1,a1,a5
      p = p->next;
80001cb8:	00042483          	lw	s1,0(s0)
      free_left = (u16_t)(free_left - p->len);
80001cbc:	01059593          	slli	a1,a1,0x10
80001cc0:	0105d593          	srli	a1,a1,0x10
      f->next = 0;
80001cc4:	00042023          	sw	zero,0(s0)
      pbuf_free(f);
80001cc8:	00040513          	mv	a0,s0
      free_left = (u16_t)(free_left - p->len);
80001ccc:	00b12623          	sw	a1,12(sp)
      pbuf_free(f);
80001cd0:	c81ff0ef          	jal	ra,80001950 <pbuf_free>
      p = p->next;
80001cd4:	00048413          	mv	s0,s1
80001cd8:	00c12583          	lw	a1,12(sp)
80001cdc:	fc9ff06f          	j	80001ca4 <pbuf_free_header+0x14>
      pbuf_remove_header(p, free_left);
80001ce0:	00040513          	mv	a0,s0
80001ce4:	be5ff0ef          	jal	ra,800018c8 <pbuf_remove_header>
}
80001ce8:	00040513          	mv	a0,s0
80001cec:	01c12083          	lw	ra,28(sp)
80001cf0:	01812403          	lw	s0,24(sp)
80001cf4:	01412483          	lw	s1,20(sp)
80001cf8:	02010113          	addi	sp,sp,32
80001cfc:	00008067          	ret

80001d00 <pbuf_clen>:
  len = 0;
80001d00:	00000793          	li	a5,0
  while (p != NULL) {
80001d04:	00051663          	bnez	a0,80001d10 <pbuf_clen+0x10>
}
80001d08:	00078513          	mv	a0,a5
80001d0c:	00008067          	ret
    ++len;
80001d10:	00178793          	addi	a5,a5,1
80001d14:	01079793          	slli	a5,a5,0x10
80001d18:	0107d793          	srli	a5,a5,0x10
    p = p->next;
80001d1c:	00052503          	lw	a0,0(a0)
80001d20:	fe5ff06f          	j	80001d04 <pbuf_clen+0x4>

80001d24 <pbuf_ref>:
  if (p != NULL) {
80001d24:	02050a63          	beqz	a0,80001d58 <pbuf_ref+0x34>
{
80001d28:	ff010113          	addi	sp,sp,-16
80001d2c:	00812423          	sw	s0,8(sp)
80001d30:	00112623          	sw	ra,12(sp)
80001d34:	00050413          	mv	s0,a0
    SYS_ARCH_SET(p->ref, (LWIP_PBUF_REF_T)(p->ref + 1));
80001d38:	881fe0ef          	jal	ra,800005b8 <sys_arch_protect>
80001d3c:	00e44783          	lbu	a5,14(s0)
80001d40:	00178793          	addi	a5,a5,1
80001d44:	00f40723          	sb	a5,14(s0)
}
80001d48:	00812403          	lw	s0,8(sp)
80001d4c:	00c12083          	lw	ra,12(sp)
80001d50:	01010113          	addi	sp,sp,16
    SYS_ARCH_SET(p->ref, (LWIP_PBUF_REF_T)(p->ref + 1));
80001d54:	869fe06f          	j	800005bc <sys_arch_unprotect>
80001d58:	00008067          	ret

80001d5c <pbuf_cat>:
  LWIP_ERROR("(h != NULL) && (t != NULL) (programmer violates API)",
80001d5c:	02050a63          	beqz	a0,80001d90 <pbuf_cat+0x34>
80001d60:	00059663          	bnez	a1,80001d6c <pbuf_cat+0x10>
80001d64:	00008067          	ret
    p->tot_len = (u16_t)(p->tot_len + t->tot_len);
80001d68:	00070513          	mv	a0,a4
80001d6c:	00855783          	lhu	a5,8(a0)
80001d70:	0085d683          	lhu	a3,8(a1)
  for (p = h; p->next != NULL; p = p->next) {
80001d74:	00052703          	lw	a4,0(a0)
80001d78:	00d787b3          	add	a5,a5,a3
80001d7c:	01079793          	slli	a5,a5,0x10
80001d80:	0107d793          	srli	a5,a5,0x10
    p->tot_len = (u16_t)(p->tot_len + t->tot_len);
80001d84:	00f51423          	sh	a5,8(a0)
  for (p = h; p->next != NULL; p = p->next) {
80001d88:	fe0710e3          	bnez	a4,80001d68 <pbuf_cat+0xc>
  p->next = t;
80001d8c:	00b52023          	sw	a1,0(a0)
}
80001d90:	00008067          	ret

80001d94 <pbuf_chain>:
{
80001d94:	ff010113          	addi	sp,sp,-16
80001d98:	00812423          	sw	s0,8(sp)
80001d9c:	00112623          	sw	ra,12(sp)
80001da0:	00058413          	mv	s0,a1
  pbuf_cat(h, t);
80001da4:	fb9ff0ef          	jal	ra,80001d5c <pbuf_cat>
  pbuf_ref(t);
80001da8:	00040513          	mv	a0,s0
}
80001dac:	00812403          	lw	s0,8(sp)
80001db0:	00c12083          	lw	ra,12(sp)
80001db4:	01010113          	addi	sp,sp,16
  pbuf_ref(t);
80001db8:	f6dff06f          	j	80001d24 <pbuf_ref>

80001dbc <pbuf_dechain>:
{
80001dbc:	ff010113          	addi	sp,sp,-16
80001dc0:	00112623          	sw	ra,12(sp)
80001dc4:	00812423          	sw	s0,8(sp)
  q = p->next;
80001dc8:	00052403          	lw	s0,0(a0)
  if (q != NULL) {
80001dcc:	02040663          	beqz	s0,80001df8 <pbuf_dechain+0x3c>
    q->tot_len = (u16_t)(p->tot_len - p->len);
80001dd0:	00855783          	lhu	a5,8(a0)
80001dd4:	00a55703          	lhu	a4,10(a0)
80001dd8:	40e787b3          	sub	a5,a5,a4
80001ddc:	00f41423          	sh	a5,8(s0)
    p->tot_len = p->len;
80001de0:	00a55783          	lhu	a5,10(a0)
    p->next = NULL;
80001de4:	00052023          	sw	zero,0(a0)
    p->tot_len = p->len;
80001de8:	00f51423          	sh	a5,8(a0)
    tail_gone = pbuf_free(q);
80001dec:	00040513          	mv	a0,s0
80001df0:	b61ff0ef          	jal	ra,80001950 <pbuf_free>
  return ((tail_gone > 0) ? NULL : q);
80001df4:	00050463          	beqz	a0,80001dfc <pbuf_dechain+0x40>
80001df8:	00000413          	li	s0,0
}
80001dfc:	00040513          	mv	a0,s0
80001e00:	00c12083          	lw	ra,12(sp)
80001e04:	00812403          	lw	s0,8(sp)
80001e08:	01010113          	addi	sp,sp,16
80001e0c:	00008067          	ret

80001e10 <pbuf_copy>:
  LWIP_ERROR("pbuf_copy: target not big enough to hold source", ((p_to != NULL) &&
80001e10:	10050463          	beqz	a0,80001f18 <pbuf_copy+0x108>
{
80001e14:	fe010113          	addi	sp,sp,-32
80001e18:	00812c23          	sw	s0,24(sp)
80001e1c:	00112e23          	sw	ra,28(sp)
80001e20:	00912a23          	sw	s1,20(sp)
80001e24:	01212823          	sw	s2,16(sp)
80001e28:	01312623          	sw	s3,12(sp)
80001e2c:	01412423          	sw	s4,8(sp)
80001e30:	00058413          	mv	s0,a1
  LWIP_ERROR("pbuf_copy: target not big enough to hold source", ((p_to != NULL) &&
80001e34:	06058e63          	beqz	a1,80001eb0 <pbuf_copy+0xa0>
80001e38:	00855703          	lhu	a4,8(a0)
80001e3c:	0085d783          	lhu	a5,8(a1)
80001e40:	00050493          	mv	s1,a0
80001e44:	06f76663          	bltu	a4,a5,80001eb0 <pbuf_copy+0xa0>
  size_t offset_to = 0, offset_from = 0, len;
80001e48:	00000a13          	li	s4,0
80001e4c:	00000993          	li	s3,0
    if ((p_to->len - offset_to) >= (p_from->len - offset_from)) {
80001e50:	00a4d903          	lhu	s2,10(s1)
80001e54:	00a45783          	lhu	a5,10(s0)
80001e58:	41390933          	sub	s2,s2,s3
80001e5c:	414787b3          	sub	a5,a5,s4
80001e60:	0127f463          	bgeu	a5,s2,80001e68 <pbuf_copy+0x58>
80001e64:	00078913          	mv	s2,a5
    MEMCPY((u8_t *)p_to->payload + offset_to, (u8_t *)p_from->payload + offset_from, len);
80001e68:	0044a503          	lw	a0,4(s1)
80001e6c:	00442583          	lw	a1,4(s0)
80001e70:	00090613          	mv	a2,s2
80001e74:	01350533          	add	a0,a0,s3
80001e78:	014585b3          	add	a1,a1,s4
80001e7c:	340040ef          	jal	ra,800061bc <memcpy>
    LWIP_ASSERT("offset_from <= p_from->len", offset_from <= p_from->len);
80001e80:	00a45703          	lhu	a4,10(s0)
    offset_from += len;
80001e84:	012a0a33          	add	s4,s4,s2
    offset_to += len;
80001e88:	012989b3          	add	s3,s3,s2
    LWIP_ASSERT("offset_to <= p_to->len", offset_to <= p_to->len);
80001e8c:	00a4d783          	lhu	a5,10(s1)
    if (offset_from >= p_from->len) {
80001e90:	00ea6663          	bltu	s4,a4,80001e9c <pbuf_copy+0x8c>
      p_from = p_from->next;
80001e94:	00042403          	lw	s0,0(s0)
      offset_from = 0;
80001e98:	00000a13          	li	s4,0
    if (offset_to == p_to->len) {
80001e9c:	02f99e63          	bne	s3,a5,80001ed8 <pbuf_copy+0xc8>
      p_to = p_to->next;
80001ea0:	0004a483          	lw	s1,0(s1)
      LWIP_ERROR("p_to != NULL", (p_to != NULL) || (p_from == NULL), return ERR_ARG;);
80001ea4:	02049863          	bnez	s1,80001ed4 <pbuf_copy+0xc4>
  return ERR_OK;
80001ea8:	00000513          	li	a0,0
      LWIP_ERROR("p_to != NULL", (p_to != NULL) || (p_from == NULL), return ERR_ARG;);
80001eac:	00040463          	beqz	s0,80001eb4 <pbuf_copy+0xa4>
80001eb0:	ff000513          	li	a0,-16
}
80001eb4:	01c12083          	lw	ra,28(sp)
80001eb8:	01812403          	lw	s0,24(sp)
80001ebc:	01412483          	lw	s1,20(sp)
80001ec0:	01012903          	lw	s2,16(sp)
80001ec4:	00c12983          	lw	s3,12(sp)
80001ec8:	00812a03          	lw	s4,8(sp)
80001ecc:	02010113          	addi	sp,sp,32
80001ed0:	00008067          	ret
      offset_to = 0;
80001ed4:	00000993          	li	s3,0
    if ((p_from != NULL) && (p_from->len == p_from->tot_len)) {
80001ed8:	00040c63          	beqz	s0,80001ef0 <pbuf_copy+0xe0>
80001edc:	00a45703          	lhu	a4,10(s0)
80001ee0:	00845783          	lhu	a5,8(s0)
80001ee4:	00f71663          	bne	a4,a5,80001ef0 <pbuf_copy+0xe0>
      LWIP_ERROR("pbuf_copy() does not allow packet queues!",
80001ee8:	00042783          	lw	a5,0(s0)
80001eec:	02079263          	bnez	a5,80001f10 <pbuf_copy+0x100>
    if ((p_to != NULL) && (p_to->len == p_to->tot_len)) {
80001ef0:	00a4d703          	lhu	a4,10(s1)
80001ef4:	0084d783          	lhu	a5,8(s1)
80001ef8:	00f71663          	bne	a4,a5,80001f04 <pbuf_copy+0xf4>
      LWIP_ERROR("pbuf_copy() does not allow packet queues!",
80001efc:	0004a783          	lw	a5,0(s1)
80001f00:	00079863          	bnez	a5,80001f10 <pbuf_copy+0x100>
  } while (p_from);
80001f04:	f40416e3          	bnez	s0,80001e50 <pbuf_copy+0x40>
  return ERR_OK;
80001f08:	00000513          	li	a0,0
80001f0c:	fa9ff06f          	j	80001eb4 <pbuf_copy+0xa4>
      LWIP_ERROR("pbuf_copy() does not allow packet queues!",
80001f10:	ffa00513          	li	a0,-6
80001f14:	fa1ff06f          	j	80001eb4 <pbuf_copy+0xa4>
      LWIP_ERROR("p_to != NULL", (p_to != NULL) || (p_from == NULL), return ERR_ARG;);
80001f18:	ff000513          	li	a0,-16
}
80001f1c:	00008067          	ret

80001f20 <pbuf_copy_partial>:
{
80001f20:	fe010113          	addi	sp,sp,-32
80001f24:	00812c23          	sw	s0,24(sp)
80001f28:	00112e23          	sw	ra,28(sp)
80001f2c:	00912a23          	sw	s1,20(sp)
80001f30:	01212823          	sw	s2,16(sp)
80001f34:	01312623          	sw	s3,12(sp)
80001f38:	01412423          	sw	s4,8(sp)
  LWIP_ERROR("pbuf_copy_partial: invalid buf", (buf != NULL), return 0;);
80001f3c:	00000413          	li	s0,0
80001f40:	02050063          	beqz	a0,80001f60 <pbuf_copy_partial+0x40>
80001f44:	00058a13          	mv	s4,a1
80001f48:	00000413          	li	s0,0
  LWIP_ERROR("pbuf_copy_partial: invalid dataptr", (dataptr != NULL), return 0;);
80001f4c:	00058a63          	beqz	a1,80001f60 <pbuf_copy_partial+0x40>
80001f50:	00050913          	mv	s2,a0
80001f54:	00060493          	mv	s1,a2
  for (p = buf; len != 0 && p != NULL; p = p->next) {
80001f58:	00048463          	beqz	s1,80001f60 <pbuf_copy_partial+0x40>
80001f5c:	02091463          	bnez	s2,80001f84 <pbuf_copy_partial+0x64>
}
80001f60:	00040513          	mv	a0,s0
80001f64:	01c12083          	lw	ra,28(sp)
80001f68:	01812403          	lw	s0,24(sp)
80001f6c:	01412483          	lw	s1,20(sp)
80001f70:	01012903          	lw	s2,16(sp)
80001f74:	00c12983          	lw	s3,12(sp)
80001f78:	00812a03          	lw	s4,8(sp)
80001f7c:	02010113          	addi	sp,sp,32
80001f80:	00008067          	ret
    if ((offset != 0) && (offset >= p->len)) {
80001f84:	00a95583          	lhu	a1,10(s2)
80001f88:	00068e63          	beqz	a3,80001fa4 <pbuf_copy_partial+0x84>
80001f8c:	00b6ec63          	bltu	a3,a1,80001fa4 <pbuf_copy_partial+0x84>
      offset = (u16_t)(offset - p->len);
80001f90:	40b686b3          	sub	a3,a3,a1
80001f94:	01069693          	slli	a3,a3,0x10
80001f98:	0106d693          	srli	a3,a3,0x10
  for (p = buf; len != 0 && p != NULL; p = p->next) {
80001f9c:	00092903          	lw	s2,0(s2)
80001fa0:	fb9ff06f          	j	80001f58 <pbuf_copy_partial+0x38>
      buf_copy_len = (u16_t)(p->len - offset);
80001fa4:	40d585b3          	sub	a1,a1,a3
80001fa8:	01059593          	slli	a1,a1,0x10
80001fac:	0105d593          	srli	a1,a1,0x10
      if (buf_copy_len > len) {
80001fb0:	00058993          	mv	s3,a1
80001fb4:	00b4f463          	bgeu	s1,a1,80001fbc <pbuf_copy_partial+0x9c>
80001fb8:	00048993          	mv	s3,s1
      MEMCPY(&((char *)dataptr)[left], &((char *)p->payload)[offset], buf_copy_len);
80001fbc:	00492583          	lw	a1,4(s2)
80001fc0:	01099993          	slli	s3,s3,0x10
80001fc4:	0109d993          	srli	s3,s3,0x10
80001fc8:	00d585b3          	add	a1,a1,a3
80001fcc:	008a0533          	add	a0,s4,s0
80001fd0:	00098613          	mv	a2,s3
      copied_total = (u16_t)(copied_total + buf_copy_len);
80001fd4:	01340433          	add	s0,s0,s3
      len = (u16_t)(len - buf_copy_len);
80001fd8:	413484b3          	sub	s1,s1,s3
      MEMCPY(&((char *)dataptr)[left], &((char *)p->payload)[offset], buf_copy_len);
80001fdc:	1e0040ef          	jal	ra,800061bc <memcpy>
      copied_total = (u16_t)(copied_total + buf_copy_len);
80001fe0:	01041413          	slli	s0,s0,0x10
      len = (u16_t)(len - buf_copy_len);
80001fe4:	01049493          	slli	s1,s1,0x10
      copied_total = (u16_t)(copied_total + buf_copy_len);
80001fe8:	01045413          	srli	s0,s0,0x10
      len = (u16_t)(len - buf_copy_len);
80001fec:	0104d493          	srli	s1,s1,0x10
      offset = 0;
80001ff0:	00000693          	li	a3,0
80001ff4:	fa9ff06f          	j	80001f9c <pbuf_copy_partial+0x7c>

80001ff8 <pbuf_get_contiguous>:
{
80001ff8:	fe010113          	addi	sp,sp,-32
80001ffc:	00112e23          	sw	ra,28(sp)
80002000:	00812c23          	sw	s0,24(sp)
80002004:	00912a23          	sw	s1,20(sp)
  LWIP_ERROR("pbuf_get_contiguous: invalid buf", (p != NULL), return NULL;);
80002008:	02051063          	bnez	a0,80002028 <pbuf_get_contiguous+0x30>
8000200c:	00000413          	li	s0,0
}
80002010:	00040513          	mv	a0,s0
80002014:	01c12083          	lw	ra,28(sp)
80002018:	01812403          	lw	s0,24(sp)
8000201c:	01412483          	lw	s1,20(sp)
80002020:	02010113          	addi	sp,sp,32
80002024:	00008067          	ret
80002028:	00058413          	mv	s0,a1
  LWIP_ERROR("pbuf_get_contiguous: invalid dataptr", (buffer != NULL), return NULL;);
8000202c:	fe0580e3          	beqz	a1,8000200c <pbuf_get_contiguous+0x14>
80002030:	00068493          	mv	s1,a3
  LWIP_ERROR("pbuf_get_contiguous: invalid dataptr", (bufsize >= len), return NULL;);
80002034:	fcd66ce3          	bltu	a2,a3,8000200c <pbuf_get_contiguous+0x14>
  q = pbuf_skip_const(p, offset, &out_offset);
80002038:	00e10613          	addi	a2,sp,14
8000203c:	00070593          	mv	a1,a4
80002040:	f0cff0ef          	jal	ra,8000174c <pbuf_skip_const>
  if (q != NULL) {
80002044:	fc0504e3          	beqz	a0,8000200c <pbuf_get_contiguous+0x14>
    if (q->len >= (out_offset + len)) {
80002048:	00e15683          	lhu	a3,14(sp)
8000204c:	00a55603          	lhu	a2,10(a0)
80002050:	00968733          	add	a4,a3,s1
80002054:	00e64863          	blt	a2,a4,80002064 <pbuf_get_contiguous+0x6c>
      return (u8_t *)q->payload + out_offset;
80002058:	00452403          	lw	s0,4(a0)
8000205c:	00d40433          	add	s0,s0,a3
80002060:	fb1ff06f          	j	80002010 <pbuf_get_contiguous+0x18>
    if (pbuf_copy_partial(q, buffer, len, out_offset) != len) {
80002064:	00048613          	mv	a2,s1
80002068:	00040593          	mv	a1,s0
8000206c:	eb5ff0ef          	jal	ra,80001f20 <pbuf_copy_partial>
80002070:	fa9500e3          	beq	a0,s1,80002010 <pbuf_get_contiguous+0x18>
80002074:	f99ff06f          	j	8000200c <pbuf_get_contiguous+0x14>

80002078 <pbuf_skip>:
 * @return the pbuf in the queue where the offset is
 */
struct pbuf *
pbuf_skip(struct pbuf *in, u16_t in_offset, u16_t *out_offset)
{
  const struct pbuf *out = pbuf_skip_const(in, in_offset, out_offset);
80002078:	ed4ff06f          	j	8000174c <pbuf_skip_const>

8000207c <pbuf_take>:
  struct pbuf *p;
  size_t buf_copy_len;
  size_t total_copy_len = len;
  size_t copied_total = 0;

  LWIP_ERROR("pbuf_take: invalid buf", (buf != NULL), return ERR_ARG;);
8000207c:	08050c63          	beqz	a0,80002114 <pbuf_take+0x98>
{
80002080:	fe010113          	addi	sp,sp,-32
80002084:	00912a23          	sw	s1,20(sp)
80002088:	01412423          	sw	s4,8(sp)
8000208c:	00112e23          	sw	ra,28(sp)
80002090:	00812c23          	sw	s0,24(sp)
80002094:	01212823          	sw	s2,16(sp)
80002098:	01312623          	sw	s3,12(sp)
8000209c:	00050493          	mv	s1,a0
800020a0:	00058a13          	mv	s4,a1
  LWIP_ERROR("pbuf_take: invalid buf", (buf != NULL), return ERR_ARG;);
800020a4:	ff000513          	li	a0,-16
  LWIP_ERROR("pbuf_take: invalid dataptr", (dataptr != NULL), return ERR_ARG;);
800020a8:	00058c63          	beqz	a1,800020c0 <pbuf_take+0x44>
  LWIP_ERROR("pbuf_take: buf not large enough", (buf->tot_len >= len), return ERR_MEM;);
800020ac:	0084d783          	lhu	a5,8(s1)
800020b0:	00060413          	mv	s0,a2
800020b4:	fff00513          	li	a0,-1
  size_t copied_total = 0;
800020b8:	00000993          	li	s3,0
  LWIP_ERROR("pbuf_take: buf not large enough", (buf->tot_len >= len), return ERR_MEM;);
800020bc:	04c7f663          	bgeu	a5,a2,80002108 <pbuf_take+0x8c>
    total_copy_len -= buf_copy_len;
    copied_total += buf_copy_len;
  }
  LWIP_ASSERT("did not copy all data", total_copy_len == 0 && copied_total == len);
  return ERR_OK;
}
800020c0:	01c12083          	lw	ra,28(sp)
800020c4:	01812403          	lw	s0,24(sp)
800020c8:	01412483          	lw	s1,20(sp)
800020cc:	01012903          	lw	s2,16(sp)
800020d0:	00c12983          	lw	s3,12(sp)
800020d4:	00812a03          	lw	s4,8(sp)
800020d8:	02010113          	addi	sp,sp,32
800020dc:	00008067          	ret
    if (buf_copy_len > p->len) {
800020e0:	00a4d903          	lhu	s2,10(s1)
800020e4:	01247463          	bgeu	s0,s2,800020ec <pbuf_take+0x70>
800020e8:	00040913          	mv	s2,s0
    MEMCPY(p->payload, &((const char *)dataptr)[copied_total], buf_copy_len);
800020ec:	0044a503          	lw	a0,4(s1)
800020f0:	013a05b3          	add	a1,s4,s3
800020f4:	00090613          	mv	a2,s2
800020f8:	0c4040ef          	jal	ra,800061bc <memcpy>
  for (p = buf; total_copy_len != 0; p = p->next) {
800020fc:	0004a483          	lw	s1,0(s1)
    total_copy_len -= buf_copy_len;
80002100:	41240433          	sub	s0,s0,s2
    copied_total += buf_copy_len;
80002104:	012989b3          	add	s3,s3,s2
  for (p = buf; total_copy_len != 0; p = p->next) {
80002108:	fc041ce3          	bnez	s0,800020e0 <pbuf_take+0x64>
  return ERR_OK;
8000210c:	00000513          	li	a0,0
80002110:	fb1ff06f          	j	800020c0 <pbuf_take+0x44>
  LWIP_ERROR("pbuf_take: invalid buf", (buf != NULL), return ERR_ARG;);
80002114:	ff000513          	li	a0,-16
}
80002118:	00008067          	ret

8000211c <pbuf_take_at>:
 *
 * @return ERR_OK if successful, ERR_MEM if the pbuf is not big enough
 */
err_t
pbuf_take_at(struct pbuf *buf, const void *dataptr, u16_t len, u16_t offset)
{
8000211c:	fd010113          	addi	sp,sp,-48
80002120:	02812423          	sw	s0,40(sp)
80002124:	01312e23          	sw	s3,28(sp)
80002128:	00060413          	mv	s0,a2
8000212c:	00058993          	mv	s3,a1
  const struct pbuf *out = pbuf_skip_const(in, in_offset, out_offset);
80002130:	00e10613          	addi	a2,sp,14
80002134:	00068593          	mv	a1,a3
{
80002138:	02112623          	sw	ra,44(sp)
8000213c:	02912223          	sw	s1,36(sp)
80002140:	03212023          	sw	s2,32(sp)
  const struct pbuf *out = pbuf_skip_const(in, in_offset, out_offset);
80002144:	e08ff0ef          	jal	ra,8000174c <pbuf_skip_const>
  u16_t target_offset;
  struct pbuf *q = pbuf_skip(buf, offset, &target_offset);

  /* return requested data if pbuf is OK */
  if ((q != NULL) && (q->tot_len >= target_offset + len)) {
80002148:	08050263          	beqz	a0,800021cc <pbuf_take_at+0xb0>
8000214c:	00e15703          	lhu	a4,14(sp)
80002150:	00855683          	lhu	a3,8(a0)
80002154:	00050493          	mv	s1,a0
80002158:	008707b3          	add	a5,a4,s0
    if (remaining_len > 0) {
      return pbuf_take(q->next, src_ptr, remaining_len);
    }
    return ERR_OK;
  }
  return ERR_MEM;
8000215c:	fff00513          	li	a0,-1
  if ((q != NULL) && (q->tot_len >= target_offset + len)) {
80002160:	04f6c863          	blt	a3,a5,800021b0 <pbuf_take_at+0x94>
    LWIP_ASSERT("check pbuf_skip result", target_offset < q->len);
80002164:	00a4d783          	lhu	a5,10(s1)
    first_copy_len = (u16_t)LWIP_MIN(q->len - target_offset, len);
80002168:	00040913          	mv	s2,s0
8000216c:	40e787b3          	sub	a5,a5,a4
80002170:	0087d663          	bge	a5,s0,8000217c <pbuf_take_at+0x60>
80002174:	01079913          	slli	s2,a5,0x10
80002178:	01095913          	srli	s2,s2,0x10
    MEMCPY(((u8_t *)q->payload) + target_offset, dataptr, first_copy_len);
8000217c:	0044a503          	lw	a0,4(s1)
80002180:	00090613          	mv	a2,s2
80002184:	00098593          	mv	a1,s3
80002188:	00e50533          	add	a0,a0,a4
8000218c:	030040ef          	jal	ra,800061bc <memcpy>
    remaining_len = (u16_t)(remaining_len - first_copy_len);
80002190:	41240633          	sub	a2,s0,s2
80002194:	01061613          	slli	a2,a2,0x10
80002198:	01065613          	srli	a2,a2,0x10
    return ERR_OK;
8000219c:	00000513          	li	a0,0
    if (remaining_len > 0) {
800021a0:	00060863          	beqz	a2,800021b0 <pbuf_take_at+0x94>
      return pbuf_take(q->next, src_ptr, remaining_len);
800021a4:	0004a503          	lw	a0,0(s1)
800021a8:	012985b3          	add	a1,s3,s2
800021ac:	ed1ff0ef          	jal	ra,8000207c <pbuf_take>
}
800021b0:	02c12083          	lw	ra,44(sp)
800021b4:	02812403          	lw	s0,40(sp)
800021b8:	02412483          	lw	s1,36(sp)
800021bc:	02012903          	lw	s2,32(sp)
800021c0:	01c12983          	lw	s3,28(sp)
800021c4:	03010113          	addi	sp,sp,48
800021c8:	00008067          	ret
  return ERR_MEM;
800021cc:	fff00513          	li	a0,-1
800021d0:	fe1ff06f          	j	800021b0 <pbuf_take_at+0x94>

800021d4 <pbuf_clone>:
 *
 * @return a new pbuf or NULL if allocation fails
 */
struct pbuf *
pbuf_clone(pbuf_layer layer, pbuf_type type, struct pbuf *p)
{
800021d4:	ff010113          	addi	sp,sp,-16
800021d8:	00912223          	sw	s1,4(sp)
800021dc:	00060493          	mv	s1,a2
  struct pbuf *q;
  err_t err;
  q = pbuf_alloc(layer, p->tot_len, type);
800021e0:	00058613          	mv	a2,a1
800021e4:	0084d583          	lhu	a1,8(s1)
{
800021e8:	00812423          	sw	s0,8(sp)
800021ec:	00112623          	sw	ra,12(sp)
  q = pbuf_alloc(layer, p->tot_len, type);
800021f0:	835ff0ef          	jal	ra,80001a24 <pbuf_alloc>
800021f4:	00050413          	mv	s0,a0
  if (q == NULL) {
800021f8:	00050663          	beqz	a0,80002204 <pbuf_clone+0x30>
    return NULL;
  }
  err = pbuf_copy(q, p);
800021fc:	00048593          	mv	a1,s1
80002200:	c11ff0ef          	jal	ra,80001e10 <pbuf_copy>
  LWIP_UNUSED_ARG(err); /* in case of LWIP_NOASSERT */
  LWIP_ASSERT("pbuf_copy failed", err == ERR_OK);
  return q;
}
80002204:	00040513          	mv	a0,s0
80002208:	00c12083          	lw	ra,12(sp)
8000220c:	00812403          	lw	s0,8(sp)
80002210:	00412483          	lw	s1,4(sp)
80002214:	01010113          	addi	sp,sp,16
80002218:	00008067          	ret

8000221c <pbuf_coalesce>:
{
8000221c:	ff010113          	addi	sp,sp,-16
80002220:	00812423          	sw	s0,8(sp)
80002224:	00112623          	sw	ra,12(sp)
80002228:	00912223          	sw	s1,4(sp)
  if (p->next == NULL) {
8000222c:	00052783          	lw	a5,0(a0)
{
80002230:	00050413          	mv	s0,a0
  if (p->next == NULL) {
80002234:	02079063          	bnez	a5,80002254 <pbuf_coalesce+0x38>
{
80002238:	00040493          	mv	s1,s0
}
8000223c:	00c12083          	lw	ra,12(sp)
80002240:	00812403          	lw	s0,8(sp)
80002244:	00048513          	mv	a0,s1
80002248:	00412483          	lw	s1,4(sp)
8000224c:	01010113          	addi	sp,sp,16
80002250:	00008067          	ret
80002254:	00058513          	mv	a0,a1
  q = pbuf_clone(layer, PBUF_RAM, p);
80002258:	00040613          	mv	a2,s0
8000225c:	28000593          	li	a1,640
80002260:	f75ff0ef          	jal	ra,800021d4 <pbuf_clone>
80002264:	00050493          	mv	s1,a0
  if (q == NULL) {
80002268:	fc0508e3          	beqz	a0,80002238 <pbuf_coalesce+0x1c>
  pbuf_free(p);
8000226c:	00040513          	mv	a0,s0
80002270:	ee0ff0ef          	jal	ra,80001950 <pbuf_free>
  return q;
80002274:	fc9ff06f          	j	8000223c <pbuf_coalesce+0x20>

80002278 <pbuf_try_get_at>:
 * @param offset offset into p of the byte to return
 * @return byte at an offset into p [0..0xFF] OR negative if 'offset' >= p->tot_len
 */
int
pbuf_try_get_at(const struct pbuf *p, u16_t offset)
{
80002278:	fe010113          	addi	sp,sp,-32
  u16_t q_idx;
  const struct pbuf *q = pbuf_skip_const(p, offset, &q_idx);
8000227c:	00e10613          	addi	a2,sp,14
{
80002280:	00112e23          	sw	ra,28(sp)
  const struct pbuf *q = pbuf_skip_const(p, offset, &q_idx);
80002284:	cc8ff0ef          	jal	ra,8000174c <pbuf_skip_const>

  /* return requested data if pbuf is OK */
  if ((q != NULL) && (q->len > q_idx)) {
    return ((u8_t *)q->payload)[q_idx];
  }
  return -1;
80002288:	fff00793          	li	a5,-1
  if ((q != NULL) && (q->len > q_idx)) {
8000228c:	02050063          	beqz	a0,800022ac <pbuf_try_get_at+0x34>
80002290:	00e15703          	lhu	a4,14(sp)
80002294:	00a55683          	lhu	a3,10(a0)
  return -1;
80002298:	fff00793          	li	a5,-1
  if ((q != NULL) && (q->len > q_idx)) {
8000229c:	00d77863          	bgeu	a4,a3,800022ac <pbuf_try_get_at+0x34>
    return ((u8_t *)q->payload)[q_idx];
800022a0:	00452783          	lw	a5,4(a0)
800022a4:	00e787b3          	add	a5,a5,a4
800022a8:	0007c783          	lbu	a5,0(a5)
}
800022ac:	01c12083          	lw	ra,28(sp)
800022b0:	00078513          	mv	a0,a5
800022b4:	02010113          	addi	sp,sp,32
800022b8:	00008067          	ret

800022bc <pbuf_get_at>:
{
800022bc:	ff010113          	addi	sp,sp,-16
800022c0:	00112623          	sw	ra,12(sp)
  int ret = pbuf_try_get_at(p, offset);
800022c4:	fb5ff0ef          	jal	ra,80002278 <pbuf_try_get_at>
  if (ret >= 0) {
800022c8:	00055463          	bgez	a0,800022d0 <pbuf_get_at+0x14>
800022cc:	00000513          	li	a0,0
}
800022d0:	00c12083          	lw	ra,12(sp)
800022d4:	0ff57513          	andi	a0,a0,255
800022d8:	01010113          	addi	sp,sp,16
800022dc:	00008067          	ret

800022e0 <pbuf_put_at>:
 * @param offset offset into p of the byte to write
 * @param data byte to write at an offset into p
 */
void
pbuf_put_at(struct pbuf *p, u16_t offset, u8_t data)
{
800022e0:	fe010113          	addi	sp,sp,-32
800022e4:	00812c23          	sw	s0,24(sp)
800022e8:	00060413          	mv	s0,a2
  const struct pbuf *out = pbuf_skip_const(in, in_offset, out_offset);
800022ec:	00e10613          	addi	a2,sp,14
{
800022f0:	00112e23          	sw	ra,28(sp)
  const struct pbuf *out = pbuf_skip_const(in, in_offset, out_offset);
800022f4:	c58ff0ef          	jal	ra,8000174c <pbuf_skip_const>
  u16_t q_idx;
  struct pbuf *q = pbuf_skip(p, offset, &q_idx);

  /* write requested data if pbuf is OK */
  if ((q != NULL) && (q->len > q_idx)) {
800022f8:	00050e63          	beqz	a0,80002314 <pbuf_put_at+0x34>
800022fc:	00e15703          	lhu	a4,14(sp)
80002300:	00a55783          	lhu	a5,10(a0)
80002304:	00f77863          	bgeu	a4,a5,80002314 <pbuf_put_at+0x34>
    ((u8_t *)q->payload)[q_idx] = data;
80002308:	00452783          	lw	a5,4(a0)
8000230c:	00e787b3          	add	a5,a5,a4
80002310:	00878023          	sb	s0,0(a5)
  }
}
80002314:	01c12083          	lw	ra,28(sp)
80002318:	01812403          	lw	s0,24(sp)
8000231c:	02010113          	addi	sp,sp,32
80002320:	00008067          	ret

80002324 <pbuf_memcmp>:
  u16_t start = offset;
  const struct pbuf *q = p;
  u16_t i;

  /* pbuf long enough to perform check? */
  if (p->tot_len < (offset + n)) {
80002324:	00855703          	lhu	a4,8(a0)
80002328:	00d587b3          	add	a5,a1,a3
8000232c:	00f75863          	bge	a4,a5,8000233c <pbuf_memcmp+0x18>
    return 0xffff;
80002330:	00010537          	lui	a0,0x10
80002334:	fff50513          	addi	a0,a0,-1 # ffff <_stack_size+0xf7ff>
    if (a != b) {
      return (u16_t)LWIP_MIN(i + 1, 0xFFFF);
    }
  }
  return 0;
}
80002338:	00008067          	ret
{
8000233c:	fd010113          	addi	sp,sp,-48
80002340:	02812423          	sw	s0,40(sp)
80002344:	02912223          	sw	s1,36(sp)
80002348:	02112623          	sw	ra,44(sp)
8000234c:	03212023          	sw	s2,32(sp)
80002350:	01312e23          	sw	s3,28(sp)
80002354:	00050493          	mv	s1,a0
80002358:	00058413          	mv	s0,a1
  while ((q != NULL) && (q->len <= start)) {
8000235c:	00a4d783          	lhu	a5,10(s1)
80002360:	00f46c63          	bltu	s0,a5,80002378 <pbuf_memcmp+0x54>
    q = q->next;
80002364:	0004a483          	lw	s1,0(s1)
    start = (u16_t)(start - q->len);
80002368:	40f40433          	sub	s0,s0,a5
8000236c:	01041413          	slli	s0,s0,0x10
80002370:	01045413          	srli	s0,s0,0x10
  while ((q != NULL) && (q->len <= start)) {
80002374:	fe0494e3          	bnez	s1,8000235c <pbuf_memcmp+0x38>
{
80002378:	00000993          	li	s3,0
8000237c:	01099913          	slli	s2,s3,0x10
80002380:	01095913          	srli	s2,s2,0x10
  for (i = 0; i < n; i++) {
80002384:	00d96663          	bltu	s2,a3,80002390 <pbuf_memcmp+0x6c>
  return 0;
80002388:	00000513          	li	a0,0
8000238c:	0580006f          	j	800023e4 <pbuf_memcmp+0xc0>
    u8_t a = pbuf_get_at(q, (u16_t)(start + i));
80002390:	008905b3          	add	a1,s2,s0
80002394:	01059593          	slli	a1,a1,0x10
80002398:	0105d593          	srli	a1,a1,0x10
8000239c:	00048513          	mv	a0,s1
800023a0:	00d12623          	sw	a3,12(sp)
800023a4:	00c12423          	sw	a2,8(sp)
800023a8:	f15ff0ef          	jal	ra,800022bc <pbuf_get_at>
    u8_t b = ((const u8_t *)s2)[i];
800023ac:	00812603          	lw	a2,8(sp)
800023b0:	00198993          	addi	s3,s3,1
    if (a != b) {
800023b4:	00c12683          	lw	a3,12(sp)
    u8_t b = ((const u8_t *)s2)[i];
800023b8:	013607b3          	add	a5,a2,s3
    if (a != b) {
800023bc:	fff7c783          	lbu	a5,-1(a5)
800023c0:	faa78ee3          	beq	a5,a0,8000237c <pbuf_memcmp+0x58>
      return (u16_t)LWIP_MIN(i + 1, 0xFFFF);
800023c4:	000107b7          	lui	a5,0x10
800023c8:	ffe78793          	addi	a5,a5,-2 # fffe <_stack_size+0xf7fe>
800023cc:	00090513          	mv	a0,s2
800023d0:	0127f463          	bgeu	a5,s2,800023d8 <pbuf_memcmp+0xb4>
800023d4:	ffe00513          	li	a0,-2
800023d8:	00150513          	addi	a0,a0,1
800023dc:	01051513          	slli	a0,a0,0x10
800023e0:	01055513          	srli	a0,a0,0x10
}
800023e4:	02c12083          	lw	ra,44(sp)
800023e8:	02812403          	lw	s0,40(sp)
800023ec:	02412483          	lw	s1,36(sp)
800023f0:	02012903          	lw	s2,32(sp)
800023f4:	01c12983          	lw	s3,28(sp)
800023f8:	03010113          	addi	sp,sp,48
800023fc:	00008067          	ret

80002400 <pbuf_memfind>:
 * @param start_offset offset into p at which to start searching
 * @return 0xFFFF if substr was not found in p or the index where it was found
 */
u16_t
pbuf_memfind(const struct pbuf *p, const void *mem, u16_t mem_len, u16_t start_offset)
{
80002400:	fe010113          	addi	sp,sp,-32
80002404:	00112e23          	sw	ra,28(sp)
80002408:	00812c23          	sw	s0,24(sp)
8000240c:	00912a23          	sw	s1,20(sp)
80002410:	01212823          	sw	s2,16(sp)
80002414:	01312623          	sw	s3,12(sp)
80002418:	01412423          	sw	s4,8(sp)
  u16_t i;
  u16_t max_cmp_start = (u16_t)(p->tot_len - mem_len);
8000241c:	00855783          	lhu	a5,8(a0)
  if (p->tot_len >= mem_len + start_offset) {
80002420:	00d60733          	add	a4,a2,a3
80002424:	02e7c263          	blt	a5,a4,80002448 <pbuf_memfind+0x48>
  u16_t max_cmp_start = (u16_t)(p->tot_len - mem_len);
80002428:	40c787b3          	sub	a5,a5,a2
    for (i = start_offset; i <= max_cmp_start; i++) {
8000242c:	01079493          	slli	s1,a5,0x10
80002430:	00050993          	mv	s3,a0
80002434:	00058a13          	mv	s4,a1
80002438:	00060913          	mv	s2,a2
8000243c:	00068413          	mv	s0,a3
80002440:	0104d493          	srli	s1,s1,0x10
80002444:	0284f863          	bgeu	s1,s0,80002474 <pbuf_memfind+0x74>
      if (plus == 0) {
        return i;
      }
    }
  }
  return 0xFFFF;
80002448:	00010437          	lui	s0,0x10
8000244c:	fff40413          	addi	s0,s0,-1 # ffff <_stack_size+0xf7ff>
}
80002450:	00040513          	mv	a0,s0
80002454:	01c12083          	lw	ra,28(sp)
80002458:	01812403          	lw	s0,24(sp)
8000245c:	01412483          	lw	s1,20(sp)
80002460:	01012903          	lw	s2,16(sp)
80002464:	00c12983          	lw	s3,12(sp)
80002468:	00812a03          	lw	s4,8(sp)
8000246c:	02010113          	addi	sp,sp,32
80002470:	00008067          	ret
      u16_t plus = pbuf_memcmp(p, i, mem, mem_len);
80002474:	00090693          	mv	a3,s2
80002478:	000a0613          	mv	a2,s4
8000247c:	00040593          	mv	a1,s0
80002480:	00098513          	mv	a0,s3
80002484:	ea1ff0ef          	jal	ra,80002324 <pbuf_memcmp>
      if (plus == 0) {
80002488:	fc0504e3          	beqz	a0,80002450 <pbuf_memfind+0x50>
    for (i = start_offset; i <= max_cmp_start; i++) {
8000248c:	00140413          	addi	s0,s0,1
80002490:	01041413          	slli	s0,s0,0x10
80002494:	01045413          	srli	s0,s0,0x10
80002498:	fadff06f          	j	80002444 <pbuf_memfind+0x44>

8000249c <pbuf_strstr>:
 * @param substr string to search for in p, maximum length is 0xFFFE
 * @return 0xFFFF if substr was not found in p or the index where it was found
 */
u16_t
pbuf_strstr(const struct pbuf *p, const char *substr)
{
8000249c:	fe010113          	addi	sp,sp,-32
800024a0:	00812c23          	sw	s0,24(sp)
800024a4:	01212823          	sw	s2,16(sp)
800024a8:	00112e23          	sw	ra,28(sp)
800024ac:	00010937          	lui	s2,0x10
800024b0:	00912a23          	sw	s1,20(sp)
800024b4:	fff90413          	addi	s0,s2,-1 # ffff <_stack_size+0xf7ff>
  size_t substr_len;
  if ((substr == NULL) || (substr[0] == 0) || (p->tot_len == 0xFFFF)) {
800024b8:	04058c63          	beqz	a1,80002510 <pbuf_strstr+0x74>
800024bc:	0005c783          	lbu	a5,0(a1)
800024c0:	04078863          	beqz	a5,80002510 <pbuf_strstr+0x74>
800024c4:	00855783          	lhu	a5,8(a0)
800024c8:	04878663          	beq	a5,s0,80002514 <pbuf_strstr+0x78>
800024cc:	00050493          	mv	s1,a0
    return 0xFFFF;
  }
  substr_len = strlen(substr);
800024d0:	00058513          	mv	a0,a1
800024d4:	00b12623          	sw	a1,12(sp)
  if (substr_len >= 0xFFFF) {
800024d8:	ffe90913          	addi	s2,s2,-2
  substr_len = strlen(substr);
800024dc:	615030ef          	jal	ra,800062f0 <strlen>
  if (substr_len >= 0xFFFF) {
800024e0:	02a96863          	bltu	s2,a0,80002510 <pbuf_strstr+0x74>
    return 0xFFFF;
  }
  return pbuf_memfind(p, substr, (u16_t)substr_len, 0);
}
800024e4:	01812403          	lw	s0,24(sp)
  return pbuf_memfind(p, substr, (u16_t)substr_len, 0);
800024e8:	01051613          	slli	a2,a0,0x10
800024ec:	00c12583          	lw	a1,12(sp)
}
800024f0:	01c12083          	lw	ra,28(sp)
800024f4:	01012903          	lw	s2,16(sp)
  return pbuf_memfind(p, substr, (u16_t)substr_len, 0);
800024f8:	00048513          	mv	a0,s1
}
800024fc:	01412483          	lw	s1,20(sp)
  return pbuf_memfind(p, substr, (u16_t)substr_len, 0);
80002500:	00000693          	li	a3,0
80002504:	01065613          	srli	a2,a2,0x10
}
80002508:	02010113          	addi	sp,sp,32
  return pbuf_memfind(p, substr, (u16_t)substr_len, 0);
8000250c:	ef5ff06f          	j	80002400 <pbuf_memfind>
    return 0xFFFF;
80002510:	00040793          	mv	a5,s0
}
80002514:	01c12083          	lw	ra,28(sp)
80002518:	01812403          	lw	s0,24(sp)
8000251c:	01412483          	lw	s1,20(sp)
80002520:	01012903          	lw	s2,16(sp)
80002524:	00078513          	mv	a0,a5
80002528:	02010113          	addi	sp,sp,32
8000252c:	00008067          	ret

80002530 <raw_input>:
 *           caller).
 *
 */
raw_input_state_t
raw_input(struct pbuf *p, struct netif *inp)
{
80002530:	fd010113          	addi	sp,sp,-48
80002534:	02912223          	sw	s1,36(sp)
  struct raw_pcb *pcb, *prev;
  s16_t proto;
  raw_input_state_t ret = RAW_INPUT_NONE;
  u8_t broadcast = ip_addr_isbroadcast(ip_current_dest_addr(), ip_current_netif());
80002538:	8c418793          	addi	a5,gp,-1852 # 8000040c <ip_data>
{
8000253c:	02812423          	sw	s0,40(sp)
80002540:	03212023          	sw	s2,32(sp)
80002544:	01312e23          	sw	s3,28(sp)
80002548:	01412c23          	sw	s4,24(sp)
8000254c:	01512a23          	sw	s5,20(sp)
80002550:	01612823          	sw	s6,16(sp)
80002554:	01712623          	sw	s7,12(sp)
80002558:	02112623          	sw	ra,44(sp)
8000255c:	00050a13          	mv	s4,a0
  u8_t broadcast = ip_addr_isbroadcast(ip_current_dest_addr(), ip_current_netif());
80002560:	0007a583          	lw	a1,0(a5)
80002564:	0147a503          	lw	a0,20(a5)
  {
    proto = IPH_PROTO((struct ip_hdr *)p->payload);
  }
#endif /* LWIP_IPV4 */

  prev = NULL;
80002568:	00000993          	li	s3,0
8000256c:	8c418493          	addi	s1,gp,-1852 # 8000040c <ip_data>
  u8_t broadcast = ip_addr_isbroadcast(ip_current_dest_addr(), ip_current_netif());
80002570:	5c4030ef          	jal	ra,80005b34 <ip4_addr_isbroadcast_u32>
    proto = IPH_PROTO((struct ip_hdr *)p->payload);
80002574:	004a2783          	lw	a5,4(s4)
  u8_t broadcast = ip_addr_isbroadcast(ip_current_dest_addr(), ip_current_netif());
80002578:	00050b13          	mv	s6,a0
    proto = IPH_PROTO((struct ip_hdr *)p->payload);
8000257c:	0097ca83          	lbu	s5,9(a5)
  pcb = raw_pcbs;
80002580:	86c1a403          	lw	s0,-1940(gp) # 800003b4 <raw_pcbs>
  raw_input_state_t ret = RAW_INPUT_NONE;
80002584:	00000513          	li	a0,0
80002588:	00078913          	mv	s2,a5
  /* loop through all raw pcbs until the packet is eaten by one */
  /* this allows multiple pcbs to match against the packet by design */
  while (pcb != NULL) {
8000258c:	08040a63          	beqz	s0,80002620 <raw_input+0xf0>
    if ((pcb->protocol == proto) && raw_input_local_match(pcb, broadcast) &&
80002590:	01444783          	lbu	a5,20(s0)
80002594:	0d579463          	bne	a5,s5,8000265c <raw_input+0x12c>
  if ((pcb->netif_idx != NETIF_NO_INDEX) &&
80002598:	00844703          	lbu	a4,8(s0)
8000259c:	00070c63          	beqz	a4,800025b4 <raw_input+0x84>
      (pcb->netif_idx != netif_get_index(ip_data.current_input_netif))) {
800025a0:	0044a783          	lw	a5,4(s1)
800025a4:	0347c783          	lbu	a5,52(a5)
800025a8:	00178793          	addi	a5,a5,1
  if ((pcb->netif_idx != NETIF_NO_INDEX) &&
800025ac:	0ff7f793          	andi	a5,a5,255
800025b0:	0af71663          	bne	a4,a5,8000265c <raw_input+0x12c>
    if (broadcast != 0) {
800025b4:	080b0c63          	beqz	s6,8000264c <raw_input+0x11c>
      if (ip_get_option(pcb, SOF_BROADCAST))
800025b8:	00944783          	lbu	a5,9(s0)
800025bc:	0207f793          	andi	a5,a5,32
800025c0:	08078e63          	beqz	a5,8000265c <raw_input+0x12c>
        if (ip4_addr_isany(ip_2_ip4(&pcb->local_ip))) {
800025c4:	00042783          	lw	a5,0(s0)
800025c8:	08079a63          	bnez	a5,8000265c <raw_input+0x12c>
    if ((pcb->protocol == proto) && raw_input_local_match(pcb, broadcast) &&
800025cc:	01544783          	lbu	a5,21(s0)
800025d0:	0017f793          	andi	a5,a5,1
800025d4:	00078863          	beqz	a5,800025e4 <raw_input+0xb4>
        (((pcb->flags & RAW_FLAGS_CONNECTED) == 0) ||
800025d8:	00442703          	lw	a4,4(s0)
800025dc:	0104a783          	lw	a5,16(s1)
800025e0:	06f71e63          	bne	a4,a5,8000265c <raw_input+0x12c>
         ip_addr_cmp(&pcb->remote_ip, ip_current_src_addr()))) {
      /* receive callback function available? */
      if (pcb->recv != NULL) {
800025e4:	01842783          	lw	a5,24(s0)
800025e8:	06078a63          	beqz	a5,8000265c <raw_input+0x12c>
#ifndef LWIP_NOASSERT
        void *old_payload = p->payload;
#endif
        ret = RAW_INPUT_DELIVERED;
        /* the receive callback function did not eat the packet? */
        eaten = pcb->recv(pcb->recv_arg, pcb, p, ip_current_src_addr());
800025ec:	01c42503          	lw	a0,28(s0)
800025f0:	8d418693          	addi	a3,gp,-1836 # 8000041c <ip_data+0x10>
800025f4:	000a0613          	mv	a2,s4
800025f8:	00040593          	mv	a1,s0
800025fc:	000780e7          	jalr	a5
        if (eaten != 0) {
80002600:	06050463          	beqz	a0,80002668 <raw_input+0x138>
               found faster next time */
            prev->next = pcb->next;
            pcb->next = raw_pcbs;
            raw_pcbs = pcb;
          }
          return RAW_INPUT_EATEN;
80002604:	00100513          	li	a0,1
          if (prev != NULL) {
80002608:	00098c63          	beqz	s3,80002620 <raw_input+0xf0>
            prev->next = pcb->next;
8000260c:	01042783          	lw	a5,16(s0)
80002610:	00f9a823          	sw	a5,16(s3)
            pcb->next = raw_pcbs;
80002614:	86c1a783          	lw	a5,-1940(gp) # 800003b4 <raw_pcbs>
            raw_pcbs = pcb;
80002618:	8681a623          	sw	s0,-1940(gp) # 800003b4 <raw_pcbs>
            pcb->next = raw_pcbs;
8000261c:	00f42823          	sw	a5,16(s0)
    /* drop the packet */
    prev = pcb;
    pcb = pcb->next;
  }
  return ret;
}
80002620:	02c12083          	lw	ra,44(sp)
80002624:	02812403          	lw	s0,40(sp)
80002628:	02412483          	lw	s1,36(sp)
8000262c:	02012903          	lw	s2,32(sp)
80002630:	01c12983          	lw	s3,28(sp)
80002634:	01812a03          	lw	s4,24(sp)
80002638:	01412a83          	lw	s5,20(sp)
8000263c:	01012b03          	lw	s6,16(sp)
80002640:	00c12b83          	lw	s7,12(sp)
80002644:	03010113          	addi	sp,sp,48
80002648:	00008067          	ret
      if (ip_addr_isany(&pcb->local_ip) ||
8000264c:	00042783          	lw	a5,0(s0)
80002650:	f6078ee3          	beqz	a5,800025cc <raw_input+0x9c>
80002654:	0144a703          	lw	a4,20(s1)
80002658:	f6e78ae3          	beq	a5,a4,800025cc <raw_input+0x9c>
    pcb = pcb->next;
8000265c:	00040993          	mv	s3,s0
80002660:	01042403          	lw	s0,16(s0)
80002664:	f29ff06f          	j	8000258c <raw_input+0x5c>
        ret = RAW_INPUT_DELIVERED;
80002668:	00200513          	li	a0,2
8000266c:	ff1ff06f          	j	8000265c <raw_input+0x12c>

80002670 <raw_bind>:
err_t
raw_bind(struct raw_pcb *pcb, const ip_addr_t *ipaddr)
{
  LWIP_ASSERT_CORE_LOCKED();
  if ((pcb == NULL) || (ipaddr == NULL)) {
    return ERR_VAL;
80002670:	ffa00793          	li	a5,-6
  if ((pcb == NULL) || (ipaddr == NULL)) {
80002674:	00050a63          	beqz	a0,80002688 <raw_bind+0x18>
80002678:	00058863          	beqz	a1,80002688 <raw_bind+0x18>
  }
  ip_addr_set_ipaddr(&pcb->local_ip, ipaddr);
8000267c:	0005a783          	lw	a5,0(a1)
80002680:	00f52023          	sw	a5,0(a0)
  if (IP_IS_V6(&pcb->local_ip) &&
      ip6_addr_lacks_zone(ip_2_ip6(&pcb->local_ip), IP6_UNKNOWN)) {
    ip6_addr_select_zone(ip_2_ip6(&pcb->local_ip), ip_2_ip6(&pcb->local_ip));
  }
#endif /* LWIP_IPV6 && LWIP_IPV6_SCOPES */
  return ERR_OK;
80002684:	00000793          	li	a5,0
}
80002688:	00078513          	mv	a0,a5
8000268c:	00008067          	ret

80002690 <raw_bind_netif>:
{
  LWIP_ASSERT_CORE_LOCKED();
  if (netif != NULL) {
    pcb->netif_idx = netif_get_index(netif);
  } else {
    pcb->netif_idx = NETIF_NO_INDEX;
80002690:	00000793          	li	a5,0
  if (netif != NULL) {
80002694:	00058863          	beqz	a1,800026a4 <raw_bind_netif+0x14>
    pcb->netif_idx = netif_get_index(netif);
80002698:	0345c783          	lbu	a5,52(a1)
8000269c:	00178793          	addi	a5,a5,1
800026a0:	0ff7f793          	andi	a5,a5,255
800026a4:	00f50423          	sb	a5,8(a0)
  }
}
800026a8:	00008067          	ret

800026ac <raw_connect>:
 *
 * @see raw_disconnect() and raw_sendto()
 */
err_t
raw_connect(struct raw_pcb *pcb, const ip_addr_t *ipaddr)
{
800026ac:	00050793          	mv	a5,a0
  LWIP_ASSERT_CORE_LOCKED();
  if ((pcb == NULL) || (ipaddr == NULL)) {
    return ERR_VAL;
800026b0:	ffa00513          	li	a0,-6
  if ((pcb == NULL) || (ipaddr == NULL)) {
800026b4:	02078263          	beqz	a5,800026d8 <raw_connect+0x2c>
800026b8:	02058063          	beqz	a1,800026d8 <raw_connect+0x2c>
  }
  ip_addr_set_ipaddr(&pcb->remote_ip, ipaddr);
800026bc:	0005a703          	lw	a4,0(a1)
      ip6_addr_lacks_zone(ip_2_ip6(&pcb->remote_ip), IP6_UNKNOWN)) {
    ip6_addr_select_zone(ip_2_ip6(&pcb->remote_ip), ip_2_ip6(&pcb->local_ip));
  }
#endif /* LWIP_IPV6 && LWIP_IPV6_SCOPES */
  raw_set_flags(pcb, RAW_FLAGS_CONNECTED);
  return ERR_OK;
800026c0:	00000513          	li	a0,0
  ip_addr_set_ipaddr(&pcb->remote_ip, ipaddr);
800026c4:	00e7a223          	sw	a4,4(a5)
  raw_set_flags(pcb, RAW_FLAGS_CONNECTED);
800026c8:	0157c703          	lbu	a4,21(a5)
800026cc:	00176713          	ori	a4,a4,1
800026d0:	00e78aa3          	sb	a4,21(a5)
  return ERR_OK;
800026d4:	00008067          	ret
}
800026d8:	00008067          	ret

800026dc <raw_disconnect>:
#if LWIP_IPV4 && LWIP_IPV6
  }
#endif
  pcb->netif_idx = NETIF_NO_INDEX;
  /* mark PCB as unconnected */
  raw_clear_flags(pcb, RAW_FLAGS_CONNECTED);
800026dc:	01554783          	lbu	a5,21(a0)
    ip_addr_set_any(IP_IS_V6_VAL(pcb->remote_ip), &pcb->remote_ip);
800026e0:	00052223          	sw	zero,4(a0)
  pcb->netif_idx = NETIF_NO_INDEX;
800026e4:	00050423          	sb	zero,8(a0)
  raw_clear_flags(pcb, RAW_FLAGS_CONNECTED);
800026e8:	ffe7f793          	andi	a5,a5,-2
800026ec:	00f50aa3          	sb	a5,21(a0)
}
800026f0:	00008067          	ret

800026f4 <raw_recv>:
void
raw_recv(struct raw_pcb *pcb, raw_recv_fn recv, void *recv_arg)
{
  LWIP_ASSERT_CORE_LOCKED();
  /* remember recv() callback and user data */
  pcb->recv = recv;
800026f4:	00b52c23          	sw	a1,24(a0)
  pcb->recv_arg = recv_arg;
800026f8:	00c52e23          	sw	a2,28(a0)
}
800026fc:	00008067          	ret

80002700 <raw_sendto_if_src>:
 * @param src_ip source IP address
 */
err_t
raw_sendto_if_src(struct raw_pcb *pcb, struct pbuf *p, const ip_addr_t *dst_ip,
                  struct netif *netif, const ip_addr_t *src_ip)
{
80002700:	fe010113          	addi	sp,sp,-32
80002704:	00112e23          	sw	ra,28(sp)
80002708:	00812c23          	sw	s0,24(sp)
8000270c:	00912a23          	sw	s1,20(sp)
80002710:	01212823          	sw	s2,16(sp)
80002714:	01312623          	sw	s3,12(sp)
80002718:	01412423          	sw	s4,8(sp)
8000271c:	01512223          	sw	s5,4(sp)
  u16_t header_size;
  u8_t ttl;

  LWIP_ASSERT_CORE_LOCKED();

  if ((pcb == NULL) || (dst_ip == NULL) || (netif == NULL) || (src_ip == NULL) ||
80002720:	00051663          	bnez	a0,8000272c <raw_sendto_if_src+0x2c>
      LWIP_DEBUGF(RAW_DEBUG | LWIP_DBG_LEVEL_WARNING, ("raw_sendto: SOF_BROADCAST not enabled on pcb %p\n", (void *)pcb));
      /* free any temporary header pbuf allocated by pbuf_header() */
      if (q != p) {
        pbuf_free(q);
      }
      return ERR_VAL;
80002724:	ffa00493          	li	s1,-6
80002728:	06c0006f          	j	80002794 <raw_sendto_if_src+0x94>
  if ((pcb == NULL) || (dst_ip == NULL) || (netif == NULL) || (src_ip == NULL) ||
8000272c:	fe060ce3          	beqz	a2,80002724 <raw_sendto_if_src+0x24>
80002730:	fe068ae3          	beqz	a3,80002724 <raw_sendto_if_src+0x24>
      !IP_ADDR_PCB_VERSION_MATCH(pcb, src_ip) || !IP_ADDR_PCB_VERSION_MATCH(pcb, dst_ip)) {
80002734:	fe0708e3          	beqz	a4,80002724 <raw_sendto_if_src+0x24>
  if (pcb->flags & RAW_FLAGS_HDRINCL) {
80002738:	01554783          	lbu	a5,21(a0)
8000273c:	00070a13          	mv	s4,a4
80002740:	00068913          	mv	s2,a3
80002744:	0027f793          	andi	a5,a5,2
80002748:	00060a93          	mv	s5,a2
8000274c:	00058413          	mv	s0,a1
80002750:	00050493          	mv	s1,a0
80002754:	06078463          	beqz	a5,800027bc <raw_sendto_if_src+0xbc>
    if (p->len < header_size) {
80002758:	00a5d703          	lhu	a4,10(a1)
8000275c:	01300793          	li	a5,19
80002760:	fce7f2e3          	bgeu	a5,a4,80002724 <raw_sendto_if_src+0x24>
    NETIF_SET_HINTS(netif, &pcb->netif_hints);
80002764:	00c50493          	addi	s1,a0,12
80002768:	0296ac23          	sw	s1,56(a3)
    err = ip_output_if_hdrincl(p, src_ip, dst_ip, netif);
8000276c:	00068813          	mv	a6,a3
80002770:	00000793          	li	a5,0
80002774:	00000713          	li	a4,0
80002778:	00000693          	li	a3,0
8000277c:	00000613          	li	a2,0
80002780:	000a0593          	mv	a1,s4
80002784:	00040513          	mv	a0,s0
80002788:	280030ef          	jal	ra,80005a08 <ip4_output_if>
8000278c:	00050493          	mv	s1,a0
    NETIF_RESET_HINTS(netif);
80002790:	02092c23          	sw	zero,56(s2)
  if (q != p) {
    /* free the header */
    pbuf_free(q);
  }
  return err;
}
80002794:	01c12083          	lw	ra,28(sp)
80002798:	01812403          	lw	s0,24(sp)
8000279c:	00048513          	mv	a0,s1
800027a0:	01012903          	lw	s2,16(sp)
800027a4:	01412483          	lw	s1,20(sp)
800027a8:	00c12983          	lw	s3,12(sp)
800027ac:	00812a03          	lw	s4,8(sp)
800027b0:	00412a83          	lw	s5,4(sp)
800027b4:	02010113          	addi	sp,sp,32
800027b8:	00008067          	ret
  if ((u16_t)(p->tot_len + header_size) < p->tot_len) {
800027bc:	0085d703          	lhu	a4,8(a1)
800027c0:	000107b7          	lui	a5,0x10
800027c4:	feb78793          	addi	a5,a5,-21 # ffeb <_stack_size+0xf7eb>
800027c8:	00e7f663          	bgeu	a5,a4,800027d4 <raw_sendto_if_src+0xd4>
    return ERR_MEM;
800027cc:	fff00493          	li	s1,-1
800027d0:	fc5ff06f          	j	80002794 <raw_sendto_if_src+0x94>
  if (pbuf_add_header(p, header_size)) {
800027d4:	01400593          	li	a1,20
800027d8:	00040513          	mv	a0,s0
800027dc:	8dcff0ef          	jal	ra,800018b8 <pbuf_add_header>
800027e0:	04050c63          	beqz	a0,80002838 <raw_sendto_if_src+0x138>
    q = pbuf_alloc(PBUF_IP, 0, PBUF_RAM);
800027e4:	28000613          	li	a2,640
800027e8:	00000593          	li	a1,0
800027ec:	02400513          	li	a0,36
800027f0:	a34ff0ef          	jal	ra,80001a24 <pbuf_alloc>
800027f4:	00050993          	mv	s3,a0
    if (q == NULL) {
800027f8:	fc050ae3          	beqz	a0,800027cc <raw_sendto_if_src+0xcc>
    if (p->tot_len != 0) {
800027fc:	00845783          	lhu	a5,8(s0)
80002800:	00078663          	beqz	a5,8000280c <raw_sendto_if_src+0x10c>
      pbuf_chain(q, p);
80002804:	00040593          	mv	a1,s0
80002808:	d8cff0ef          	jal	ra,80001d94 <pbuf_chain>
    if (!ip_get_option(pcb, SOF_BROADCAST) && ip_addr_isbroadcast(dst_ip, netif)) {
8000280c:	0094c783          	lbu	a5,9(s1)
80002810:	0207f793          	andi	a5,a5,32
80002814:	02079e63          	bnez	a5,80002850 <raw_sendto_if_src+0x150>
80002818:	000aa503          	lw	a0,0(s5)
8000281c:	00090593          	mv	a1,s2
80002820:	314030ef          	jal	ra,80005b34 <ip4_addr_isbroadcast_u32>
80002824:	02050663          	beqz	a0,80002850 <raw_sendto_if_src+0x150>
      if (q != p) {
80002828:	ef340ee3          	beq	s0,s3,80002724 <raw_sendto_if_src+0x24>
        pbuf_free(q);
8000282c:	00098513          	mv	a0,s3
80002830:	920ff0ef          	jal	ra,80001950 <pbuf_free>
80002834:	ef1ff06f          	j	80002724 <raw_sendto_if_src+0x24>
    if (pbuf_remove_header(q, header_size)) {
80002838:	01400593          	li	a1,20
8000283c:	00040513          	mv	a0,s0
80002840:	888ff0ef          	jal	ra,800018c8 <pbuf_remove_header>
80002844:	f80514e3          	bnez	a0,800027cc <raw_sendto_if_src+0xcc>
80002848:	00040993          	mv	s3,s0
8000284c:	fc1ff06f          	j	8000280c <raw_sendto_if_src+0x10c>
  NETIF_SET_HINTS(netif, &pcb->netif_hints);
80002850:	00c48793          	addi	a5,s1,12
  ttl = pcb->ttl;
80002854:	00b4c683          	lbu	a3,11(s1)
  NETIF_SET_HINTS(netif, &pcb->netif_hints);
80002858:	02f92c23          	sw	a5,56(s2)
  err = ip_output_if(q, src_ip, dst_ip, ttl, pcb->tos, pcb->protocol, netif);
8000285c:	0144c783          	lbu	a5,20(s1)
80002860:	00a4c703          	lbu	a4,10(s1)
80002864:	00090813          	mv	a6,s2
80002868:	000a8613          	mv	a2,s5
8000286c:	000a0593          	mv	a1,s4
80002870:	00098513          	mv	a0,s3
80002874:	194030ef          	jal	ra,80005a08 <ip4_output_if>
  NETIF_RESET_HINTS(netif);
80002878:	02092c23          	sw	zero,56(s2)
  err = ip_output_if(q, src_ip, dst_ip, ttl, pcb->tos, pcb->protocol, netif);
8000287c:	00050493          	mv	s1,a0
  if (q != p) {
80002880:	f1340ae3          	beq	s0,s3,80002794 <raw_sendto_if_src+0x94>
    pbuf_free(q);
80002884:	00098513          	mv	a0,s3
80002888:	8c8ff0ef          	jal	ra,80001950 <pbuf_free>
8000288c:	f09ff06f          	j	80002794 <raw_sendto_if_src+0x94>

80002890 <raw_sendto>:
  if ((pcb == NULL) || (ipaddr == NULL) || !IP_ADDR_PCB_VERSION_MATCH(pcb, ipaddr)) {
80002890:	08050663          	beqz	a0,8000291c <raw_sendto+0x8c>
80002894:	08060463          	beqz	a2,8000291c <raw_sendto+0x8c>
{
80002898:	ff010113          	addi	sp,sp,-16
8000289c:	00812423          	sw	s0,8(sp)
800028a0:	00912223          	sw	s1,4(sp)
800028a4:	01212023          	sw	s2,0(sp)
800028a8:	00112623          	sw	ra,12(sp)
800028ac:	00050413          	mv	s0,a0
  if (pcb->netif_idx != NETIF_NO_INDEX) {
800028b0:	00854503          	lbu	a0,8(a0)
800028b4:	00060493          	mv	s1,a2
800028b8:	00058913          	mv	s2,a1
800028bc:	04050663          	beqz	a0,80002908 <raw_sendto+0x78>
    netif = netif_get_by_index(pcb->netif_idx);
800028c0:	d69fe0ef          	jal	ra,80001628 <netif_get_by_index>
  if (netif == NULL) {
800028c4:	06050063          	beqz	a0,80002924 <raw_sendto+0x94>
  if (ip_addr_isany(&pcb->local_ip) || ip_addr_ismulticast(&pcb->local_ip)) {
800028c8:	00042783          	lw	a5,0(s0)
800028cc:	04078463          	beqz	a5,80002914 <raw_sendto+0x84>
800028d0:	0f07f793          	andi	a5,a5,240
800028d4:	0e000713          	li	a4,224
800028d8:	02e78e63          	beq	a5,a4,80002914 <raw_sendto+0x84>
800028dc:	00040713          	mv	a4,s0
  return raw_sendto_if_src(pcb, p, ipaddr, netif, src_ip);
800028e0:	00050693          	mv	a3,a0
800028e4:	00040513          	mv	a0,s0
}
800028e8:	00812403          	lw	s0,8(sp)
800028ec:	00c12083          	lw	ra,12(sp)
  return raw_sendto_if_src(pcb, p, ipaddr, netif, src_ip);
800028f0:	00048613          	mv	a2,s1
800028f4:	00090593          	mv	a1,s2
}
800028f8:	00412483          	lw	s1,4(sp)
800028fc:	00012903          	lw	s2,0(sp)
80002900:	01010113          	addi	sp,sp,16
  return raw_sendto_if_src(pcb, p, ipaddr, netif, src_ip);
80002904:	dfdff06f          	j	80002700 <raw_sendto_if_src>
      netif = ip_route(&pcb->local_ip, ipaddr);
80002908:	00060513          	mv	a0,a2
8000290c:	2a5020ef          	jal	ra,800053b0 <ip4_route>
80002910:	fb5ff06f          	j	800028c4 <raw_sendto+0x34>
    src_ip = ip_netif_get_local_ip(netif, ipaddr);
80002914:	00450713          	addi	a4,a0,4
80002918:	fc9ff06f          	j	800028e0 <raw_sendto+0x50>
    return ERR_VAL;
8000291c:	ffa00513          	li	a0,-6
}
80002920:	00008067          	ret
80002924:	00c12083          	lw	ra,12(sp)
80002928:	00812403          	lw	s0,8(sp)
8000292c:	00412483          	lw	s1,4(sp)
80002930:	00012903          	lw	s2,0(sp)
    return ERR_RTE;
80002934:	ffc00513          	li	a0,-4
}
80002938:	01010113          	addi	sp,sp,16
8000293c:	00008067          	ret

80002940 <raw_send>:
 *
 */
err_t
raw_send(struct raw_pcb *pcb, struct pbuf *p)
{
  return raw_sendto(pcb, p, &pcb->remote_ip);
80002940:	00450613          	addi	a2,a0,4
80002944:	f4dff06f          	j	80002890 <raw_sendto>

80002948 <raw_remove>:
raw_remove(struct raw_pcb *pcb)
{
  struct raw_pcb *pcb2;
  LWIP_ASSERT_CORE_LOCKED();
  /* pcb to be removed is first in list? */
  if (raw_pcbs == pcb) {
80002948:	86c1a783          	lw	a5,-1940(gp) # 800003b4 <raw_pcbs>
{
8000294c:	00050593          	mv	a1,a0
  if (raw_pcbs == pcb) {
80002950:	00a79c63          	bne	a5,a0,80002968 <raw_remove+0x20>
    /* make list start at 2nd pcb */
    raw_pcbs = raw_pcbs->next;
80002954:	01052783          	lw	a5,16(a0)
80002958:	86f1a623          	sw	a5,-1940(gp) # 800003b4 <raw_pcbs>
        pcb2->next = pcb->next;
        break;
      }
    }
  }
  memp_free(MEMP_RAW_PCB, pcb);
8000295c:	00000513          	li	a0,0
80002960:	f58fe06f          	j	800010b8 <memp_free>
80002964:	00070793          	mv	a5,a4
    for (pcb2 = raw_pcbs; pcb2 != NULL; pcb2 = pcb2->next) {
80002968:	fe078ae3          	beqz	a5,8000295c <raw_remove+0x14>
      if (pcb2->next != NULL && pcb2->next == pcb) {
8000296c:	0107a703          	lw	a4,16(a5)
80002970:	fe0706e3          	beqz	a4,8000295c <raw_remove+0x14>
80002974:	feb718e3          	bne	a4,a1,80002964 <raw_remove+0x1c>
        pcb2->next = pcb->next;
80002978:	0105a703          	lw	a4,16(a1)
8000297c:	00e7a823          	sw	a4,16(a5)
        break;
80002980:	fddff06f          	j	8000295c <raw_remove+0x14>

80002984 <raw_new>:
 *
 * @see raw_remove()
 */
struct raw_pcb *
raw_new(u8_t proto)
{
80002984:	ff010113          	addi	sp,sp,-16
80002988:	00912223          	sw	s1,4(sp)
8000298c:	00050493          	mv	s1,a0
  struct raw_pcb *pcb;

  LWIP_DEBUGF(RAW_DEBUG | LWIP_DBG_TRACE, ("raw_new\n"));
  LWIP_ASSERT_CORE_LOCKED();

  pcb = (struct raw_pcb *)memp_malloc(MEMP_RAW_PCB);
80002990:	00000513          	li	a0,0
{
80002994:	00812423          	sw	s0,8(sp)
80002998:	00112623          	sw	ra,12(sp)
  pcb = (struct raw_pcb *)memp_malloc(MEMP_RAW_PCB);
8000299c:	eb8fe0ef          	jal	ra,80001054 <memp_malloc>
800029a0:	00050413          	mv	s0,a0
  /* could allocate RAW PCB? */
  if (pcb != NULL) {
800029a4:	02050463          	beqz	a0,800029cc <raw_new+0x48>
    /* initialize PCB to all zeroes */
    memset(pcb, 0, sizeof(struct raw_pcb));
800029a8:	02000613          	li	a2,32
800029ac:	00000593          	li	a1,0
800029b0:	600030ef          	jal	ra,80005fb0 <memset>
    pcb->protocol = proto;
    pcb->ttl = RAW_TTL;
800029b4:	fff00793          	li	a5,-1
800029b8:	00f405a3          	sb	a5,11(s0)
#if LWIP_MULTICAST_TX_OPTIONS
    raw_set_multicast_ttl(pcb, RAW_TTL);
#endif /* LWIP_MULTICAST_TX_OPTIONS */
    pcb->next = raw_pcbs;
800029bc:	86c1a703          	lw	a4,-1940(gp) # 800003b4 <raw_pcbs>
    pcb->protocol = proto;
800029c0:	00940a23          	sb	s1,20(s0)
    raw_pcbs = pcb;
800029c4:	8681a623          	sw	s0,-1940(gp) # 800003b4 <raw_pcbs>
    pcb->next = raw_pcbs;
800029c8:	00e42823          	sw	a4,16(s0)
  }
  return pcb;
}
800029cc:	00040513          	mv	a0,s0
800029d0:	00c12083          	lw	ra,12(sp)
800029d4:	00812403          	lw	s0,8(sp)
800029d8:	00412483          	lw	s1,4(sp)
800029dc:	01010113          	addi	sp,sp,16
800029e0:	00008067          	ret

800029e4 <raw_new_ip_type>:
struct raw_pcb *
raw_new_ip_type(u8_t type, u8_t proto)
{
  struct raw_pcb *pcb;
  LWIP_ASSERT_CORE_LOCKED();
  pcb = raw_new(proto);
800029e4:	00058513          	mv	a0,a1
800029e8:	f9dff06f          	j	80002984 <raw_new>

800029ec <raw_netif_ip_addr_changed>:
 */
void raw_netif_ip_addr_changed(const ip_addr_t *old_addr, const ip_addr_t *new_addr)
{
  struct raw_pcb *rpcb;

  if (!ip_addr_isany(old_addr) && !ip_addr_isany(new_addr)) {
800029ec:	02050063          	beqz	a0,80002a0c <raw_netif_ip_addr_changed+0x20>
800029f0:	00052783          	lw	a5,0(a0)
800029f4:	00078c63          	beqz	a5,80002a0c <raw_netif_ip_addr_changed+0x20>
800029f8:	00058a63          	beqz	a1,80002a0c <raw_netif_ip_addr_changed+0x20>
800029fc:	0005a783          	lw	a5,0(a1)
80002a00:	00078663          	beqz	a5,80002a0c <raw_netif_ip_addr_changed+0x20>
    for (rpcb = raw_pcbs; rpcb != NULL; rpcb = rpcb->next) {
80002a04:	86c1a783          	lw	a5,-1940(gp) # 800003b4 <raw_pcbs>
80002a08:	00079463          	bnez	a5,80002a10 <raw_netif_ip_addr_changed+0x24>
         * is set to bound to the new one instead */
        ip_addr_copy(rpcb->local_ip, *new_addr);
      }
    }
  }
}
80002a0c:	00008067          	ret
      if (ip_addr_cmp(&rpcb->local_ip, old_addr)) {
80002a10:	0007a683          	lw	a3,0(a5)
80002a14:	00052703          	lw	a4,0(a0)
80002a18:	00e69663          	bne	a3,a4,80002a24 <raw_netif_ip_addr_changed+0x38>
        ip_addr_copy(rpcb->local_ip, *new_addr);
80002a1c:	0005a703          	lw	a4,0(a1)
80002a20:	00e7a023          	sw	a4,0(a5)
    for (rpcb = raw_pcbs; rpcb != NULL; rpcb = rpcb->next) {
80002a24:	0107a783          	lw	a5,16(a5)
80002a28:	fe1ff06f          	j	80002a08 <raw_netif_ip_addr_changed+0x1c>

80002a2c <stats_init>:
#ifdef LWIP_DEBUG
#if MEM_STATS
  lwip_stats.mem.name = "MEM";
#endif /* MEM_STATS */
#endif /* LWIP_DEBUG */
}
80002a2c:	00008067          	ret

80002a30 <sys_timeout_abs>:
#if LWIP_DEBUG_TIMERNAMES
sys_timeout_abs(u32_t abs_time, sys_timeout_handler handler, void *arg, const char *handler_name)
#else /* LWIP_DEBUG_TIMERNAMES */
sys_timeout_abs(u32_t abs_time, sys_timeout_handler handler, void *arg)
#endif
{
80002a30:	fe010113          	addi	sp,sp,-32
80002a34:	00812c23          	sw	s0,24(sp)
80002a38:	00050413          	mv	s0,a0
  struct sys_timeo *timeout, *t;

  timeout = (struct sys_timeo *)memp_malloc(MEMP_SYS_TIMEOUT);
80002a3c:	00400513          	li	a0,4
{
80002a40:	00112e23          	sw	ra,28(sp)
80002a44:	00b12623          	sw	a1,12(sp)
80002a48:	00c12423          	sw	a2,8(sp)
  timeout = (struct sys_timeo *)memp_malloc(MEMP_SYS_TIMEOUT);
80002a4c:	e08fe0ef          	jal	ra,80001054 <memp_malloc>
  if (timeout == NULL) {
80002a50:	02050463          	beqz	a0,80002a78 <sys_timeout_abs+0x48>
    LWIP_ASSERT("sys_timeout: timeout != NULL, pool MEMP_SYS_TIMEOUT is empty", timeout != NULL);
    return;
  }

  timeout->next = NULL;
  timeout->h = handler;
80002a54:	00c12583          	lw	a1,12(sp)
  timeout->arg = arg;
80002a58:	00812603          	lw	a2,8(sp)
  timeout->handler_name = handler_name;
  LWIP_DEBUGF(TIMERS_DEBUG, ("sys_timeout: %p abs_time=%"U32_F" handler=%s arg=%p\n",
                             (void *)timeout, abs_time, handler_name, (void *)arg));
#endif /* LWIP_DEBUG_TIMERNAMES */

  if (next_timeout == NULL) {
80002a5c:	8741a783          	lw	a5,-1932(gp) # 800003bc <next_timeout>
  timeout->next = NULL;
80002a60:	00052023          	sw	zero,0(a0)
  timeout->h = handler;
80002a64:	00b52423          	sw	a1,8(a0)
  timeout->arg = arg;
80002a68:	00c52623          	sw	a2,12(a0)
  timeout->time = abs_time;
80002a6c:	00852223          	sw	s0,4(a0)
  if (next_timeout == NULL) {
80002a70:	00079c63          	bnez	a5,80002a88 <sys_timeout_abs+0x58>
    next_timeout = timeout;
    return;
  }
  if (TIME_LESS_THAN(timeout->time, next_timeout->time)) {
    timeout->next = next_timeout;
    next_timeout = timeout;
80002a74:	86a1aa23          	sw	a0,-1932(gp) # 800003bc <next_timeout>
        t->next = timeout;
        break;
      }
    }
  }
}
80002a78:	01c12083          	lw	ra,28(sp)
80002a7c:	01812403          	lw	s0,24(sp)
80002a80:	02010113          	addi	sp,sp,32
80002a84:	00008067          	ret
  if (TIME_LESS_THAN(timeout->time, next_timeout->time)) {
80002a88:	0047a683          	lw	a3,4(a5)
80002a8c:	40d406b3          	sub	a3,s0,a3
80002a90:	0006d863          	bgez	a3,80002aa0 <sys_timeout_abs+0x70>
    timeout->next = next_timeout;
80002a94:	00f52023          	sw	a5,0(a0)
80002a98:	fddff06f          	j	80002a74 <sys_timeout_abs+0x44>
80002a9c:	00070793          	mv	a5,a4
      if ((t->next == NULL) || TIME_LESS_THAN(timeout->time, t->next->time)) {
80002aa0:	0007a703          	lw	a4,0(a5)
80002aa4:	00070863          	beqz	a4,80002ab4 <sys_timeout_abs+0x84>
80002aa8:	00472683          	lw	a3,4(a4) # 10004 <_stack_size+0xf804>
80002aac:	40d406b3          	sub	a3,s0,a3
80002ab0:	fe06d6e3          	bgez	a3,80002a9c <sys_timeout_abs+0x6c>
        timeout->next = t->next;
80002ab4:	00e52023          	sw	a4,0(a0)
        t->next = timeout;
80002ab8:	00a7a023          	sw	a0,0(a5)
        break;
80002abc:	fbdff06f          	j	80002a78 <sys_timeout_abs+0x48>

80002ac0 <lwip_cyclic_timer>:
#if !LWIP_TESTMODE
static
#endif
void
lwip_cyclic_timer(void *arg)
{
80002ac0:	fe010113          	addi	sp,sp,-32
80002ac4:	00112e23          	sw	ra,28(sp)
  const struct lwip_cyclic_timer *cyclic = (const struct lwip_cyclic_timer *)arg;

#if LWIP_DEBUG_TIMERNAMES
  LWIP_DEBUGF(TIMERS_DEBUG, ("tcpip: %s()\n", cyclic->handler_name));
#endif
  cyclic->handler();
80002ac8:	00452783          	lw	a5,4(a0)
80002acc:	00a12623          	sw	a0,12(sp)
80002ad0:	000780e7          	jalr	a5

  now = sys_now();
80002ad4:	addfd0ef          	jal	ra,800005b0 <sys_now>
  next_timeout_time = (u32_t)(current_timeout_due_time + cyclic->interval_ms);  /* overflow handled by TIME_LESS_THAN macro */ 
80002ad8:	00c12603          	lw	a2,12(sp)
80002adc:	8701a783          	lw	a5,-1936(gp) # 800003b8 <current_timeout_due_time>
80002ae0:	00062703          	lw	a4,0(a2)
80002ae4:	800035b7          	lui	a1,0x80003
  if (TIME_LESS_THAN(next_timeout_time, now)) {
    /* timer would immediately expire again -> "overload" -> restart without any correction */
#if LWIP_DEBUG_TIMERNAMES
    sys_timeout_abs((u32_t)(now + cyclic->interval_ms), lwip_cyclic_timer, arg, cyclic->handler_name);
#else
    sys_timeout_abs((u32_t)(now + cyclic->interval_ms), lwip_cyclic_timer, arg);
80002ae8:	ac058593          	addi	a1,a1,-1344 # 80002ac0 <_stack_start+0xffffbd30>
  next_timeout_time = (u32_t)(current_timeout_due_time + cyclic->interval_ms);  /* overflow handled by TIME_LESS_THAN macro */ 
80002aec:	00f707b3          	add	a5,a4,a5
  if (TIME_LESS_THAN(next_timeout_time, now)) {
80002af0:	40a786b3          	sub	a3,a5,a0
    sys_timeout_abs((u32_t)(now + cyclic->interval_ms), lwip_cyclic_timer, arg);
80002af4:	00a70533          	add	a0,a4,a0
  if (TIME_LESS_THAN(next_timeout_time, now)) {
80002af8:	0006c463          	bltz	a3,80002b00 <lwip_cyclic_timer+0x40>
  } else {
    /* correct cyclic interval with handler execution delay and sys_check_timeouts jitter */
#if LWIP_DEBUG_TIMERNAMES
    sys_timeout_abs(next_timeout_time, lwip_cyclic_timer, arg, cyclic->handler_name);
#else
    sys_timeout_abs(next_timeout_time, lwip_cyclic_timer, arg);
80002afc:	00078513          	mv	a0,a5
#endif
  }
}
80002b00:	01c12083          	lw	ra,28(sp)
80002b04:	02010113          	addi	sp,sp,32
    sys_timeout_abs(next_timeout_time, lwip_cyclic_timer, arg);
80002b08:	f29ff06f          	j	80002a30 <sys_timeout_abs>

80002b0c <sys_timeout>:
sys_timeout_debug(u32_t msecs, sys_timeout_handler handler, void *arg, const char *handler_name)
#else /* LWIP_DEBUG_TIMERNAMES */
void
sys_timeout(u32_t msecs, sys_timeout_handler handler, void *arg)
#endif /* LWIP_DEBUG_TIMERNAMES */
{
80002b0c:	fe010113          	addi	sp,sp,-32
80002b10:	00812c23          	sw	s0,24(sp)
80002b14:	00112e23          	sw	ra,28(sp)
80002b18:	00050413          	mv	s0,a0
80002b1c:	00b12623          	sw	a1,12(sp)
80002b20:	00c12423          	sw	a2,8(sp)

  LWIP_ASSERT_CORE_LOCKED();

  LWIP_ASSERT("Timeout time too long, max is LWIP_UINT32_MAX/4 msecs", msecs <= (LWIP_UINT32_MAX / 4));

  next_timeout_time = (u32_t)(sys_now() + msecs); /* overflow handled by TIME_LESS_THAN macro */ 
80002b24:	a8dfd0ef          	jal	ra,800005b0 <sys_now>

#if LWIP_DEBUG_TIMERNAMES
  sys_timeout_abs(next_timeout_time, handler, arg, handler_name);
#else
  sys_timeout_abs(next_timeout_time, handler, arg);
80002b28:	00850533          	add	a0,a0,s0
#endif
}
80002b2c:	01812403          	lw	s0,24(sp)
  sys_timeout_abs(next_timeout_time, handler, arg);
80002b30:	00812603          	lw	a2,8(sp)
80002b34:	00c12583          	lw	a1,12(sp)
}
80002b38:	01c12083          	lw	ra,28(sp)
80002b3c:	02010113          	addi	sp,sp,32
  sys_timeout_abs(next_timeout_time, handler, arg);
80002b40:	ef1ff06f          	j	80002a30 <sys_timeout_abs>

80002b44 <sys_timeouts_init>:
{
80002b44:	ff010113          	addi	sp,sp,-16
80002b48:	00812423          	sw	s0,8(sp)
    sys_timeout(lwip_cyclic_timers[i].interval_ms, lwip_cyclic_timer, LWIP_CONST_CAST(void *, &lwip_cyclic_timers[i]));
80002b4c:	80000637          	lui	a2,0x80000
80002b50:	80003437          	lui	s0,0x80003
{
80002b54:	00912223          	sw	s1,4(sp)
    sys_timeout(lwip_cyclic_timers[i].interval_ms, lwip_cyclic_timer, LWIP_CONST_CAST(void *, &lwip_cyclic_timers[i]));
80002b58:	ac040593          	addi	a1,s0,-1344 # 80002ac0 <_stack_start+0xffffbd30>
80002b5c:	1d060493          	addi	s1,a2,464 # 800001d0 <_stack_start+0xffff9440>
80002b60:	3e800513          	li	a0,1000
80002b64:	1d060613          	addi	a2,a2,464
{
80002b68:	00112623          	sw	ra,12(sp)
    sys_timeout(lwip_cyclic_timers[i].interval_ms, lwip_cyclic_timer, LWIP_CONST_CAST(void *, &lwip_cyclic_timers[i]));
80002b6c:	fa1ff0ef          	jal	ra,80002b0c <sys_timeout>
80002b70:	ac040593          	addi	a1,s0,-1344
}
80002b74:	00812403          	lw	s0,8(sp)
80002b78:	00c12083          	lw	ra,12(sp)
    sys_timeout(lwip_cyclic_timers[i].interval_ms, lwip_cyclic_timer, LWIP_CONST_CAST(void *, &lwip_cyclic_timers[i]));
80002b7c:	00848613          	addi	a2,s1,8
}
80002b80:	00412483          	lw	s1,4(sp)
    sys_timeout(lwip_cyclic_timers[i].interval_ms, lwip_cyclic_timer, LWIP_CONST_CAST(void *, &lwip_cyclic_timers[i]));
80002b84:	3e800513          	li	a0,1000
}
80002b88:	01010113          	addi	sp,sp,16
    sys_timeout(lwip_cyclic_timers[i].interval_ms, lwip_cyclic_timer, LWIP_CONST_CAST(void *, &lwip_cyclic_timers[i]));
80002b8c:	f81ff06f          	j	80002b0c <sys_timeout>

80002b90 <sys_untimeout>:
{
  struct sys_timeo *prev_t, *t;

  LWIP_ASSERT_CORE_LOCKED();

  if (next_timeout == NULL) {
80002b90:	8741a783          	lw	a5,-1932(gp) # 800003bc <next_timeout>
80002b94:	00070693          	mv	a3,a4
    return;
  }

  for (t = next_timeout, prev_t = NULL; t != NULL; prev_t = t, t = t->next) {
80002b98:	00000613          	li	a2,0
  if (next_timeout == NULL) {
80002b9c:	00079e63          	bnez	a5,80002bb8 <sys_untimeout+0x28>
      memp_free(MEMP_SYS_TIMEOUT, t);
      return;
    }
  }
  return;
}
80002ba0:	00008067          	ret
        prev_t->next = t->next;
80002ba4:	00e62023          	sw	a4,0(a2)
80002ba8:	02c0006f          	j	80002bd4 <sys_untimeout+0x44>
80002bac:	00078613          	mv	a2,a5
  for (t = next_timeout, prev_t = NULL; t != NULL; prev_t = t, t = t->next) {
80002bb0:	fe0708e3          	beqz	a4,80002ba0 <sys_untimeout+0x10>
80002bb4:	00070793          	mv	a5,a4
    if ((t->h == handler) && (t->arg == arg)) {
80002bb8:	0087a803          	lw	a6,8(a5)
80002bbc:	0007a703          	lw	a4,0(a5)
80002bc0:	fea816e3          	bne	a6,a0,80002bac <sys_untimeout+0x1c>
80002bc4:	00c7a803          	lw	a6,12(a5)
80002bc8:	feb812e3          	bne	a6,a1,80002bac <sys_untimeout+0x1c>
      if (prev_t == NULL) {
80002bcc:	fc061ce3          	bnez	a2,80002ba4 <sys_untimeout+0x14>
        next_timeout = t->next;
80002bd0:	86e1aa23          	sw	a4,-1932(gp) # 800003bc <next_timeout>
      memp_free(MEMP_SYS_TIMEOUT, t);
80002bd4:	00078593          	mv	a1,a5
80002bd8:	00400513          	li	a0,4
80002bdc:	cdcfe06f          	j	800010b8 <memp_free>

80002be0 <sys_check_timeouts>:
 *
 * Must be called periodically from your main loop.
 */
void
sys_check_timeouts(void)
{
80002be0:	fe010113          	addi	sp,sp,-32
80002be4:	00912a23          	sw	s1,20(sp)
80002be8:	01312623          	sw	s3,12(sp)
80002bec:	01412423          	sw	s4,8(sp)
80002bf0:	00112e23          	sw	ra,28(sp)
80002bf4:	00812c23          	sw	s0,24(sp)
80002bf8:	01212823          	sw	s2,16(sp)
  u32_t now;

  LWIP_ASSERT_CORE_LOCKED();

  /* Process only timers expired at the start of the function. */
  now = sys_now();
80002bfc:	9b5fd0ef          	jal	ra,800005b0 <sys_now>
80002c00:	00050993          	mv	s3,a0
    sys_timeout_handler handler;
    void *arg;

    PBUF_CHECK_FREE_OOSEQ();

    tmptimeout = next_timeout;
80002c04:	8741a583          	lw	a1,-1932(gp) # 800003bc <next_timeout>
    if (tmptimeout == NULL) {
80002c08:	02058e63          	beqz	a1,80002c44 <sys_check_timeouts+0x64>
      return;
    }

    if (TIME_LESS_THAN(now, tmptimeout->time)) {
80002c0c:	0045a783          	lw	a5,4(a1)
80002c10:	40f98733          	sub	a4,s3,a5
80002c14:	02074863          	bltz	a4,80002c44 <sys_check_timeouts+0x64>
      return;
    }

    /* Timeout has expired */
    next_timeout = tmptimeout->next;
80002c18:	0005a703          	lw	a4,0(a1)
    handler = tmptimeout->h;
80002c1c:	0085a403          	lw	s0,8(a1)
    if (handler != NULL) {
      LWIP_DEBUGF(TIMERS_DEBUG, ("sct calling h=%s t=%"U32_F" arg=%p\n",
                                 tmptimeout->handler_name, sys_now() - tmptimeout->time, arg));
    }
#endif /* LWIP_DEBUG_TIMERNAMES */
    memp_free(MEMP_SYS_TIMEOUT, tmptimeout);
80002c20:	00400513          	li	a0,4
    next_timeout = tmptimeout->next;
80002c24:	86e1aa23          	sw	a4,-1932(gp) # 800003bc <next_timeout>
    arg = tmptimeout->arg;
80002c28:	00c5a903          	lw	s2,12(a1)
    current_timeout_due_time = tmptimeout->time;
80002c2c:	86f1a823          	sw	a5,-1936(gp) # 800003b8 <current_timeout_due_time>
    memp_free(MEMP_SYS_TIMEOUT, tmptimeout);
80002c30:	c88fe0ef          	jal	ra,800010b8 <memp_free>
    if (handler != NULL) {
80002c34:	fc0408e3          	beqz	s0,80002c04 <sys_check_timeouts+0x24>
      handler(arg);
80002c38:	00090513          	mv	a0,s2
80002c3c:	000400e7          	jalr	s0
80002c40:	fc5ff06f          	j	80002c04 <sys_check_timeouts+0x24>
    }
    LWIP_TCPIP_THREAD_ALIVE();

    /* Repeat until all expired timers have been called */
  } while (1);
}
80002c44:	01c12083          	lw	ra,28(sp)
80002c48:	01812403          	lw	s0,24(sp)
80002c4c:	01412483          	lw	s1,20(sp)
80002c50:	01012903          	lw	s2,16(sp)
80002c54:	00c12983          	lw	s3,12(sp)
80002c58:	00812a03          	lw	s4,8(sp)
80002c5c:	02010113          	addi	sp,sp,32
80002c60:	00008067          	ret

80002c64 <sys_restart_timeouts>:
 * time (e.g. while saving energy) to prevent all timer functions of that
 * period being called.
 */
void
sys_restart_timeouts(void)
{
80002c64:	ff010113          	addi	sp,sp,-16
80002c68:	00812423          	sw	s0,8(sp)
  u32_t now;
  u32_t base;
  struct sys_timeo *t;

  if (next_timeout == NULL) {
80002c6c:	8741a783          	lw	a5,-1932(gp) # 800003bc <next_timeout>
{
80002c70:	00112623          	sw	ra,12(sp)
  if (next_timeout == NULL) {
80002c74:	02078463          	beqz	a5,80002c9c <sys_restart_timeouts+0x38>
    return;
  }

  now = sys_now();
80002c78:	939fd0ef          	jal	ra,800005b0 <sys_now>
  base = next_timeout->time;
80002c7c:	8741a783          	lw	a5,-1932(gp) # 800003bc <next_timeout>
80002c80:	0047a703          	lw	a4,4(a5)

  for (t = next_timeout; t != NULL; t = t->next) {
    t->time = (t->time - base) + now;
80002c84:	40e50533          	sub	a0,a0,a4
80002c88:	0047a703          	lw	a4,4(a5)
80002c8c:	00a70733          	add	a4,a4,a0
80002c90:	00e7a223          	sw	a4,4(a5)
  for (t = next_timeout; t != NULL; t = t->next) {
80002c94:	0007a783          	lw	a5,0(a5)
80002c98:	fe0798e3          	bnez	a5,80002c88 <sys_restart_timeouts+0x24>
  }
}
80002c9c:	00c12083          	lw	ra,12(sp)
80002ca0:	00812403          	lw	s0,8(sp)
80002ca4:	01010113          	addi	sp,sp,16
80002ca8:	00008067          	ret

80002cac <sys_timeouts_sleeptime>:
/** Return the time left before the next timeout is due. If no timeouts are
 * enqueued, returns 0xffffffff
 */
u32_t
sys_timeouts_sleeptime(void)
{
80002cac:	ff010113          	addi	sp,sp,-16
80002cb0:	00812423          	sw	s0,8(sp)
  u32_t now;

  LWIP_ASSERT_CORE_LOCKED();

  if (next_timeout == NULL) {
80002cb4:	8741a783          	lw	a5,-1932(gp) # 800003bc <next_timeout>
{
80002cb8:	00112623          	sw	ra,12(sp)
    return SYS_TIMEOUTS_SLEEPTIME_INFINITE;
80002cbc:	fff00513          	li	a0,-1
  if (next_timeout == NULL) {
80002cc0:	00078e63          	beqz	a5,80002cdc <sys_timeouts_sleeptime+0x30>
  }
  now = sys_now();
80002cc4:	8edfd0ef          	jal	ra,800005b0 <sys_now>
  if (TIME_LESS_THAN(next_timeout->time, now)) {
80002cc8:	8741a783          	lw	a5,-1932(gp) # 800003bc <next_timeout>
80002ccc:	0047a783          	lw	a5,4(a5)
80002cd0:	40a78533          	sub	a0,a5,a0
80002cd4:	00055463          	bgez	a0,80002cdc <sys_timeouts_sleeptime+0x30>
    return 0;
80002cd8:	00000513          	li	a0,0
  } else {
    u32_t ret = (u32_t)(next_timeout->time - now);
    LWIP_ASSERT("invalid sleeptime", ret <= LWIP_MAX_TIMEOUT);
    return ret;
  }
}
80002cdc:	00c12083          	lw	ra,12(sp)
80002ce0:	00812403          	lw	s0,8(sp)
80002ce4:	01010113          	addi	sp,sp,16
80002ce8:	00008067          	ret

80002cec <udp_init>:
udp_init(void)
{
#ifdef LWIP_RAND
  udp_port = UDP_ENSURE_LOCAL_PORT_RANGE(LWIP_RAND());
#endif /* LWIP_RAND */
}
80002cec:	00008067          	ret

80002cf0 <udp_input>:
 * @param inp network interface on which the datagram was received.
 *
 */
void
udp_input(struct pbuf *p, struct netif *inp)
{
80002cf0:	fd010113          	addi	sp,sp,-48
80002cf4:	02812423          	sw	s0,40(sp)
80002cf8:	02112623          	sw	ra,44(sp)
80002cfc:	02912223          	sw	s1,36(sp)
80002d00:	03212023          	sw	s2,32(sp)
80002d04:	01312e23          	sw	s3,28(sp)
80002d08:	01412c23          	sw	s4,24(sp)
80002d0c:	01512a23          	sw	s5,20(sp)
80002d10:	01612823          	sw	s6,16(sp)
80002d14:	01712623          	sw	s7,12(sp)
  LWIP_ASSERT("udp_input: invalid pbuf", p != NULL);
  LWIP_ASSERT("udp_input: invalid netif", inp != NULL);

  PERF_START;

  UDP_STATS_INC(udp.recv);
80002d18:	8dc18793          	addi	a5,gp,-1828 # 80000424 <lwip_stats>
80002d1c:	0627d703          	lhu	a4,98(a5)
80002d20:	00170713          	addi	a4,a4,1
80002d24:	06e79123          	sh	a4,98(a5)

  /* Check minimum length (UDP header) */
  if (p->len < UDP_HLEN) {
80002d28:	00a55683          	lhu	a3,10(a0)
80002d2c:	00700713          	li	a4,7
80002d30:	04d76463          	bltu	a4,a3,80002d78 <udp_input+0x88>
    /* drop short packets */
    LWIP_DEBUGF(UDP_DEBUG,
                ("udp_input: short UDP datagram (%"U16_F" bytes) discarded\n", p->tot_len));
    UDP_STATS_INC(udp.lenerr);
80002d34:	06a7d703          	lhu	a4,106(a5)
80002d38:	00170713          	addi	a4,a4,1
80002d3c:	06e79523          	sh	a4,106(a5)
    UDP_STATS_INC(udp.drop);
80002d40:	0667d703          	lhu	a4,102(a5)
80002d44:	00170713          	addi	a4,a4,1
80002d48:	06e79323          	sh	a4,102(a5)
  UDP_STATS_INC(udp.drop);
  MIB2_STATS_INC(mib2.udpinerrors);
  pbuf_free(p);
  PERF_STOP("udp_input");
#endif /* CHECKSUM_CHECK_UDP */
}
80002d4c:	02812403          	lw	s0,40(sp)
80002d50:	02c12083          	lw	ra,44(sp)
80002d54:	02412483          	lw	s1,36(sp)
80002d58:	02012903          	lw	s2,32(sp)
80002d5c:	01c12983          	lw	s3,28(sp)
80002d60:	01812a03          	lw	s4,24(sp)
80002d64:	01412a83          	lw	s5,20(sp)
80002d68:	01012b03          	lw	s6,16(sp)
80002d6c:	00c12b83          	lw	s7,12(sp)
80002d70:	03010113          	addi	sp,sp,48
      pbuf_free(p);
80002d74:	bddfe06f          	j	80001950 <pbuf_free>
  broadcast = ip_addr_isbroadcast(ip_current_dest_addr(), ip_current_netif());
80002d78:	8c418913          	addi	s2,gp,-1852 # 8000040c <ip_data>
  udphdr = (struct udp_hdr *)p->payload;
80002d7c:	00452983          	lw	s3,4(a0)
80002d80:	00050493          	mv	s1,a0
80002d84:	00058b93          	mv	s7,a1
  broadcast = ip_addr_isbroadcast(ip_current_dest_addr(), ip_current_netif());
80002d88:	01492503          	lw	a0,20(s2)
80002d8c:	00092583          	lw	a1,0(s2)
80002d90:	8dc18413          	addi	s0,gp,-1828 # 80000424 <lwip_stats>
80002d94:	8c418a13          	addi	s4,gp,-1852 # 8000040c <ip_data>
80002d98:	59d020ef          	jal	ra,80005b34 <ip4_addr_isbroadcast_u32>
  src = lwip_ntohs(udphdr->src);
80002d9c:	0019c783          	lbu	a5,1(s3)
  broadcast = ip_addr_isbroadcast(ip_current_dest_addr(), ip_current_netif());
80002da0:	00050a93          	mv	s5,a0
  src = lwip_ntohs(udphdr->src);
80002da4:	0009c503          	lbu	a0,0(s3)
80002da8:	00879793          	slli	a5,a5,0x8
80002dac:	00a7e533          	or	a0,a5,a0
80002db0:	b39fd0ef          	jal	ra,800008e8 <lwip_htons>
  dest = lwip_ntohs(udphdr->dest);
80002db4:	0039c783          	lbu	a5,3(s3)
  src = lwip_ntohs(udphdr->src);
80002db8:	00050b13          	mv	s6,a0
  dest = lwip_ntohs(udphdr->dest);
80002dbc:	0029c503          	lbu	a0,2(s3)
80002dc0:	00879793          	slli	a5,a5,0x8
80002dc4:	00a7e533          	or	a0,a5,a0
80002dc8:	b21fd0ef          	jal	ra,800008e8 <lwip_htons>
  for (pcb = udp_pcbs; pcb != NULL; pcb = pcb->next) {
80002dcc:	8781a803          	lw	a6,-1928(gp) # 800003c0 <udp_pcbs>
      (pcb->netif_idx != netif_get_index(ip_data.current_input_netif))) {
80002dd0:	00492303          	lw	t1,4(s2)
      if (ip_addr_isany(&pcb->local_ip) || ip_addr_cmp(&pcb->local_ip, ip_current_dest_addr())) {
80002dd4:	01492683          	lw	a3,20(s2)
           ip_addr_cmp(&pcb->remote_ip, ip_current_src_addr()))) {
80002dd8:	01092e03          	lw	t3,16(s2)
  for (pcb = udp_pcbs; pcb != NULL; pcb = pcb->next) {
80002ddc:	00080793          	mv	a5,a6
  uncon_pcb = NULL;
80002de0:	00000913          	li	s2,0
  prev = NULL;
80002de4:	00000593          	li	a1,0
        } else if (broadcast && ip4_current_dest_addr()->addr == IPADDR_BROADCAST) {
80002de8:	fff00893          	li	a7,-1
  for (pcb = udp_pcbs; pcb != NULL; pcb = pcb->next) {
80002dec:	00079c63          	bnez	a5,80002e04 <udp_input+0x114>
  if (pcb != NULL) {
80002df0:	0a091e63          	bnez	s2,80002eac <udp_input+0x1bc>
  if (for_us) {
80002df4:	004ba783          	lw	a5,4(s7)
80002df8:	0ad78a63          	beq	a5,a3,80002eac <udp_input+0x1bc>
      pbuf_free(p);
80002dfc:	00048513          	mv	a0,s1
80002e00:	f4dff06f          	j	80002d4c <udp_input+0x5c>
    if ((pcb->local_port == dest) &&
80002e04:	0167d703          	lhu	a4,22(a5)
80002e08:	0ea71e63          	bne	a4,a0,80002f04 <udp_input+0x214>
  if ((pcb->netif_idx != NETIF_NO_INDEX) &&
80002e0c:	0087ce83          	lbu	t4,8(a5)
80002e10:	000e8a63          	beqz	t4,80002e24 <udp_input+0x134>
      (pcb->netif_idx != netif_get_index(ip_data.current_input_netif))) {
80002e14:	03434703          	lbu	a4,52(t1)
80002e18:	00170713          	addi	a4,a4,1
  if ((pcb->netif_idx != NETIF_NO_INDEX) &&
80002e1c:	0ff77713          	andi	a4,a4,255
80002e20:	0eee9263          	bne	t4,a4,80002f04 <udp_input+0x214>
    if (broadcast != 0) {
80002e24:	0c0a8a63          	beqz	s5,80002ef8 <udp_input+0x208>
      if (ip_get_option(pcb, SOF_BROADCAST))
80002e28:	0097c703          	lbu	a4,9(a5)
80002e2c:	02077713          	andi	a4,a4,32
80002e30:	0c070a63          	beqz	a4,80002f04 <udp_input+0x214>
        if (ip4_addr_isany(ip_2_ip4(&pcb->local_ip)) ||
80002e34:	0007a703          	lw	a4,0(a5)
80002e38:	00070c63          	beqz	a4,80002e50 <udp_input+0x160>
80002e3c:	01168a63          	beq	a3,a7,80002e50 <udp_input+0x160>
            ip4_addr_netcmp(ip_2_ip4(&pcb->local_ip), ip4_current_dest_addr(), netif_ip4_netmask(inp))) {
80002e40:	008bae83          	lw	t4,8(s7)
80002e44:	00d74733          	xor	a4,a4,a3
80002e48:	01d77733          	and	a4,a4,t4
            ((ip4_current_dest_addr()->addr == IPADDR_BROADCAST)) ||
80002e4c:	0a071c63          	bnez	a4,80002f04 <udp_input+0x214>
      if ((pcb->flags & UDP_FLAGS_CONNECTED) == 0) {
80002e50:	0147c703          	lbu	a4,20(a5)
80002e54:	00477713          	andi	a4,a4,4
80002e58:	02071463          	bnez	a4,80002e80 <udp_input+0x190>
        if (uncon_pcb == NULL) {
80002e5c:	02090063          	beqz	s2,80002e7c <udp_input+0x18c>
        } else if (broadcast && ip4_current_dest_addr()->addr == IPADDR_BROADCAST) {
80002e60:	020a8063          	beqz	s5,80002e80 <udp_input+0x190>
80002e64:	01169e63          	bne	a3,a7,80002e80 <udp_input+0x190>
          if (!IP_IS_V4_VAL(uncon_pcb->local_ip) || !ip4_addr_cmp(ip_2_ip4(&uncon_pcb->local_ip), netif_ip4_addr(inp))) {
80002e68:	004ba703          	lw	a4,4(s7)
80002e6c:	00092e83          	lw	t4,0(s2)
80002e70:	00ee8863          	beq	t4,a4,80002e80 <udp_input+0x190>
            if (IP_IS_V4_VAL(pcb->local_ip) && ip4_addr_cmp(ip_2_ip4(&pcb->local_ip), netif_ip4_addr(inp))) {
80002e74:	0007ae83          	lw	t4,0(a5)
80002e78:	01d71463          	bne	a4,t4,80002e80 <udp_input+0x190>
80002e7c:	00078913          	mv	s2,a5
      if ((pcb->remote_port == src) &&
80002e80:	0187d703          	lhu	a4,24(a5)
80002e84:	09671063          	bne	a4,s6,80002f04 <udp_input+0x214>
          (ip_addr_isany_val(pcb->remote_ip) ||
80002e88:	0047a703          	lw	a4,4(a5)
      if ((pcb->remote_port == src) &&
80002e8c:	00070463          	beqz	a4,80002e94 <udp_input+0x1a4>
          (ip_addr_isany_val(pcb->remote_ip) ||
80002e90:	07c71a63          	bne	a4,t3,80002f04 <udp_input+0x214>
        if (prev != NULL) {
80002e94:	06058e63          	beqz	a1,80002f10 <udp_input+0x220>
          prev->next = pcb->next;
80002e98:	0107a703          	lw	a4,16(a5)
          udp_pcbs = pcb;
80002e9c:	86f1ac23          	sw	a5,-1928(gp) # 800003c0 <udp_pcbs>
          prev->next = pcb->next;
80002ea0:	00e5a823          	sw	a4,16(a1)
          pcb->next = udp_pcbs;
80002ea4:	0107a823          	sw	a6,16(a5)
          UDP_STATS_INC(udp.cachehit);
80002ea8:	00078913          	mv	s2,a5
        if (udphdr->chksum != 0) {
80002eac:	0079c783          	lbu	a5,7(s3)
80002eb0:	0069c703          	lbu	a4,6(s3)
80002eb4:	00879793          	slli	a5,a5,0x8
80002eb8:	00e7e7b3          	or	a5,a5,a4
80002ebc:	06078263          	beqz	a5,80002f20 <udp_input+0x230>
          if (ip_chksum_pseudo(p, IP_PROTO_UDP, p->tot_len,
80002ec0:	0084d603          	lhu	a2,8(s1)
80002ec4:	8d818713          	addi	a4,gp,-1832 # 80000420 <ip_data+0x14>
80002ec8:	8d418693          	addi	a3,gp,-1836 # 8000041c <ip_data+0x10>
80002ecc:	01100593          	li	a1,17
80002ed0:	00048513          	mv	a0,s1
80002ed4:	e75fd0ef          	jal	ra,80000d48 <ip_chksum_pseudo>
80002ed8:	04050463          	beqz	a0,80002f20 <udp_input+0x230>
  UDP_STATS_INC(udp.chkerr);
80002edc:	06845783          	lhu	a5,104(s0)
80002ee0:	00178793          	addi	a5,a5,1
80002ee4:	06f41423          	sh	a5,104(s0)
      UDP_STATS_INC(udp.drop);
80002ee8:	06645783          	lhu	a5,102(s0)
80002eec:	00178793          	addi	a5,a5,1
80002ef0:	06f41323          	sh	a5,102(s0)
80002ef4:	f09ff06f          	j	80002dfc <udp_input+0x10c>
      if (ip_addr_isany(&pcb->local_ip) || ip_addr_cmp(&pcb->local_ip, ip_current_dest_addr())) {
80002ef8:	0007a703          	lw	a4,0(a5)
80002efc:	f4070ae3          	beqz	a4,80002e50 <udp_input+0x160>
80002f00:	f4d708e3          	beq	a4,a3,80002e50 <udp_input+0x160>
  for (pcb = udp_pcbs; pcb != NULL; pcb = pcb->next) {
80002f04:	00078593          	mv	a1,a5
80002f08:	0107a783          	lw	a5,16(a5)
80002f0c:	ee1ff06f          	j	80002dec <udp_input+0xfc>
          UDP_STATS_INC(udp.cachehit);
80002f10:	07645703          	lhu	a4,118(s0)
80002f14:	00170713          	addi	a4,a4,1
80002f18:	06e41b23          	sh	a4,118(s0)
80002f1c:	f8dff06f          	j	80002ea8 <udp_input+0x1b8>
    if (pbuf_remove_header(p, UDP_HLEN)) {
80002f20:	00800593          	li	a1,8
80002f24:	00048513          	mv	a0,s1
80002f28:	9a1fe0ef          	jal	ra,800018c8 <pbuf_remove_header>
80002f2c:	fa051ee3          	bnez	a0,80002ee8 <udp_input+0x1f8>
    if (pcb != NULL) {
80002f30:	04090663          	beqz	s2,80002f7c <udp_input+0x28c>
      if (pcb->recv != NULL) {
80002f34:	01c92303          	lw	t1,28(s2)
80002f38:	ec0302e3          	beqz	t1,80002dfc <udp_input+0x10c>
}
80002f3c:	02812403          	lw	s0,40(sp)
        pcb->recv(pcb->recv_arg, pcb, p, ip_current_src_addr(), src);
80002f40:	02092503          	lw	a0,32(s2)
}
80002f44:	02c12083          	lw	ra,44(sp)
80002f48:	01c12983          	lw	s3,28(sp)
80002f4c:	01812a03          	lw	s4,24(sp)
80002f50:	01412a83          	lw	s5,20(sp)
80002f54:	00c12b83          	lw	s7,12(sp)
        pcb->recv(pcb->recv_arg, pcb, p, ip_current_src_addr(), src);
80002f58:	000b0713          	mv	a4,s6
80002f5c:	00048613          	mv	a2,s1
}
80002f60:	01012b03          	lw	s6,16(sp)
80002f64:	02412483          	lw	s1,36(sp)
        pcb->recv(pcb->recv_arg, pcb, p, ip_current_src_addr(), src);
80002f68:	00090593          	mv	a1,s2
}
80002f6c:	02012903          	lw	s2,32(sp)
        pcb->recv(pcb->recv_arg, pcb, p, ip_current_src_addr(), src);
80002f70:	8d418693          	addi	a3,gp,-1836 # 8000041c <ip_data+0x10>
}
80002f74:	03010113          	addi	sp,sp,48
        pcb->recv(pcb->recv_arg, pcb, p, ip_current_src_addr(), src);
80002f78:	00030067          	jr	t1
      if (!broadcast && !ip_addr_ismulticast(ip_current_dest_addr())) {
80002f7c:	020a9c63          	bnez	s5,80002fb4 <udp_input+0x2c4>
80002f80:	014a2783          	lw	a5,20(s4)
80002f84:	0e000713          	li	a4,224
80002f88:	0f07f793          	andi	a5,a5,240
80002f8c:	02e78463          	beq	a5,a4,80002fb4 <udp_input+0x2c4>
        pbuf_header_force(p, (s16_t)(ip_current_header_tot_len() + UDP_HLEN));
80002f90:	00ca5583          	lhu	a1,12(s4)
80002f94:	00048513          	mv	a0,s1
80002f98:	00858593          	addi	a1,a1,8
80002f9c:	01059593          	slli	a1,a1,0x10
80002fa0:	4105d593          	srai	a1,a1,0x10
80002fa4:	999fe0ef          	jal	ra,8000193c <pbuf_header_force>
        icmp_port_unreach(ip_current_is_v6(), p);
80002fa8:	00300593          	li	a1,3
80002fac:	00048513          	mv	a0,s1
80002fb0:	660010ef          	jal	ra,80004610 <icmp_dest_unreach>
      UDP_STATS_INC(udp.proterr);
80002fb4:	07045783          	lhu	a5,112(s0)
80002fb8:	00178793          	addi	a5,a5,1
80002fbc:	06f41823          	sh	a5,112(s0)
80002fc0:	f29ff06f          	j	80002ee8 <udp_input+0x1f8>

80002fc4 <udp_bind>:

  LWIP_ASSERT_CORE_LOCKED();

#if LWIP_IPV4
  /* Don't propagate NULL pointer (IPv4 ANY) to subsequent functions */
  if (ipaddr == NULL) {
80002fc4:	00059463          	bnez	a1,80002fcc <udp_bind+0x8>
    ipaddr = IP4_ADDR_ANY;
80002fc8:	85818593          	addi	a1,gp,-1960 # 800003a0 <ip_addr_any>
  }
#else /* LWIP_IPV4 */
  LWIP_ERROR("udp_bind: invalid ipaddr", ipaddr != NULL, return ERR_ARG);
#endif /* LWIP_IPV4 */

  LWIP_ERROR("udp_bind: invalid pcb", pcb != NULL, return ERR_ARG);
80002fcc:	ff000793          	li	a5,-16
80002fd0:	0a050463          	beqz	a0,80003078 <udp_bind+0xb4>
  ip_addr_debug_print(UDP_DEBUG | LWIP_DBG_TRACE, ipaddr);
  LWIP_DEBUGF(UDP_DEBUG | LWIP_DBG_TRACE, (", port = %"U16_F")\n", port));

  rebind = 0;
  /* Check for double bind and rebind of the same pcb */
  for (ipcb = udp_pcbs; ipcb != NULL; ipcb = ipcb->next) {
80002fd4:	8781a783          	lw	a5,-1928(gp) # 800003c0 <udp_pcbs>
80002fd8:	00078693          	mv	a3,a5
80002fdc:	06069063          	bnez	a3,8000303c <udp_bind+0x78>
  rebind = 0;
80002fe0:	00000e13          	li	t3,0
    ipaddr = &zoned_ipaddr;
  }
#endif /* LWIP_IPV6 && LWIP_IPV6_SCOPES */

  /* no port specified? */
  if (port == 0) {
80002fe4:	0c061463          	bnez	a2,800030ac <udp_bind+0xe8>
80002fe8:	800006b7          	lui	a3,0x80000
80002fec:	3486d603          	lhu	a2,840(a3) # 80000348 <_stack_start+0xffff95b8>
  if (udp_port++ == UDP_LOCAL_PORT_RANGE_END) {
80002ff0:	00010337          	lui	t1,0x10
  if (port == 0) {
80002ff4:	00004837          	lui	a6,0x4
  if (udp_port++ == UDP_LOCAL_PORT_RANGE_END) {
80002ff8:	fff30313          	addi	t1,t1,-1 # ffff <_stack_size+0xf7ff>
80002ffc:	04660a63          	beq	a2,t1,80003050 <udp_bind+0x8c>
80003000:	00160613          	addi	a2,a2,1
80003004:	01061613          	slli	a2,a2,0x10
80003008:	01065613          	srli	a2,a2,0x10
  for (pcb = udp_pcbs; pcb != NULL; pcb = pcb->next) {
8000300c:	00078893          	mv	a7,a5
80003010:	04089463          	bnez	a7,80003058 <udp_bind+0x94>
80003014:	34c69423          	sh	a2,840(a3)
        }
      }
    }
  }

  ip_addr_set_ipaddr(&pcb->local_ip, ipaddr);
80003018:	0005a783          	lw	a5,0(a1)

  pcb->local_port = port;
8000301c:	00c51b23          	sh	a2,22(a0)
  ip_addr_set_ipaddr(&pcb->local_ip, ipaddr);
80003020:	00f52023          	sw	a5,0(a0)
  mib2_udp_bind(pcb);
  /* pcb not active yet? */
  if (rebind == 0) {
80003024:	000e1863          	bnez	t3,80003034 <udp_bind+0x70>
    /* place the PCB on the active list if not already there */
    pcb->next = udp_pcbs;
80003028:	8781a783          	lw	a5,-1928(gp) # 800003c0 <udp_pcbs>
    udp_pcbs = pcb;
8000302c:	86a1ac23          	sw	a0,-1928(gp) # 800003c0 <udp_pcbs>
    pcb->next = udp_pcbs;
80003030:	00f52823          	sw	a5,16(a0)
  }
  LWIP_DEBUGF(UDP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_STATE, ("udp_bind: bound to "));
  ip_addr_debug_print_val(UDP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_STATE, pcb->local_ip);
  LWIP_DEBUGF(UDP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_STATE, (", port %"U16_F")\n", pcb->local_port));
  return ERR_OK;
80003034:	00000793          	li	a5,0
80003038:	0400006f          	j	80003078 <udp_bind+0xb4>
    if (pcb == ipcb) {
8000303c:	00a68663          	beq	a3,a0,80003048 <udp_bind+0x84>
  for (ipcb = udp_pcbs; ipcb != NULL; ipcb = ipcb->next) {
80003040:	0106a683          	lw	a3,16(a3)
80003044:	f99ff06f          	j	80002fdc <udp_bind+0x18>
      rebind = 1;
80003048:	00100e13          	li	t3,1
8000304c:	f99ff06f          	j	80002fe4 <udp_bind+0x20>
    udp_port = UDP_LOCAL_PORT_RANGE_START;
80003050:	0000c637          	lui	a2,0xc
80003054:	fb9ff06f          	j	8000300c <udp_bind+0x48>
    if (pcb->local_port == udp_port) {
80003058:	0168de83          	lhu	t4,22(a7)
8000305c:	02ce9263          	bne	t4,a2,80003080 <udp_bind+0xbc>
      if (++n > (UDP_LOCAL_PORT_RANGE_END - UDP_LOCAL_PORT_RANGE_START)) {
80003060:	fff80813          	addi	a6,a6,-1 # 3fff <_stack_size+0x37ff>
80003064:	01081813          	slli	a6,a6,0x10
80003068:	01085813          	srli	a6,a6,0x10
8000306c:	f80818e3          	bnez	a6,80002ffc <udp_bind+0x38>
80003070:	34c69423          	sh	a2,840(a3)
      return ERR_USE;
80003074:	ff800793          	li	a5,-8
}
80003078:	00078513          	mv	a0,a5
8000307c:	00008067          	ret
  for (pcb = udp_pcbs; pcb != NULL; pcb = pcb->next) {
80003080:	0108a883          	lw	a7,16(a7)
80003084:	f8dff06f          	j	80003010 <udp_bind+0x4c>
      if (pcb != ipcb) {
80003088:	02a78063          	beq	a5,a0,800030a8 <udp_bind+0xe4>
          if ((ipcb->local_port == port) &&
8000308c:	0167d683          	lhu	a3,22(a5)
80003090:	00c69c63          	bne	a3,a2,800030a8 <udp_bind+0xe4>
80003094:	0005a803          	lw	a6,0(a1)
              (ip_addr_cmp(&ipcb->local_ip, ipaddr) || ip_addr_isany(ipaddr) ||
80003098:	0007a683          	lw	a3,0(a5)
          if ((ipcb->local_port == port) &&
8000309c:	fd068ce3          	beq	a3,a6,80003074 <udp_bind+0xb0>
              (ip_addr_cmp(&ipcb->local_ip, ipaddr) || ip_addr_isany(ipaddr) ||
800030a0:	fc080ae3          	beqz	a6,80003074 <udp_bind+0xb0>
              ip_addr_isany(&ipcb->local_ip))) {
800030a4:	fc0688e3          	beqz	a3,80003074 <udp_bind+0xb0>
    for (ipcb = udp_pcbs; ipcb != NULL; ipcb = ipcb->next) {
800030a8:	0107a783          	lw	a5,16(a5)
800030ac:	fc079ee3          	bnez	a5,80003088 <udp_bind+0xc4>
800030b0:	f69ff06f          	j	80003018 <udp_bind+0x54>

800030b4 <udp_sendto_if_src>:
{
800030b4:	fd010113          	addi	sp,sp,-48
800030b8:	02912223          	sw	s1,36(sp)
800030bc:	02112623          	sw	ra,44(sp)
800030c0:	02812423          	sw	s0,40(sp)
800030c4:	03212023          	sw	s2,32(sp)
800030c8:	01312e23          	sw	s3,28(sp)
800030cc:	01412c23          	sw	s4,24(sp)
800030d0:	01512a23          	sw	s5,20(sp)
800030d4:	01612823          	sw	s6,16(sp)
800030d8:	01712623          	sw	s7,12(sp)
  LWIP_ERROR("udp_sendto_if_src: invalid pcb", pcb != NULL, return ERR_ARG);
800030dc:	ff000493          	li	s1,-16
800030e0:	08050263          	beqz	a0,80003164 <udp_sendto_if_src+0xb0>
  LWIP_ERROR("udp_sendto_if_src: invalid pbuf", p != NULL, return ERR_ARG);
800030e4:	08058063          	beqz	a1,80003164 <udp_sendto_if_src+0xb0>
  LWIP_ERROR("udp_sendto_if_src: invalid dst_ip", dst_ip != NULL, return ERR_ARG);
800030e8:	06060e63          	beqz	a2,80003164 <udp_sendto_if_src+0xb0>
  LWIP_ERROR("udp_sendto_if_src: invalid src_ip", src_ip != NULL, return ERR_ARG);
800030ec:	06078c63          	beqz	a5,80003164 <udp_sendto_if_src+0xb0>
  LWIP_ERROR("udp_sendto_if_src: invalid netif", netif != NULL, return ERR_ARG);
800030f0:	06070a63          	beqz	a4,80003164 <udp_sendto_if_src+0xb0>
800030f4:	00078b13          	mv	s6,a5
  if (!ip_get_option(pcb, SOF_BROADCAST) &&
800030f8:	00954783          	lbu	a5,9(a0)
800030fc:	00070a13          	mv	s4,a4
80003100:	00068b93          	mv	s7,a3
80003104:	0207f793          	andi	a5,a5,32
80003108:	00060a93          	mv	s5,a2
8000310c:	00058993          	mv	s3,a1
80003110:	00050413          	mv	s0,a0
80003114:	02078e63          	beqz	a5,80003150 <udp_sendto_if_src+0x9c>
  if (pcb->local_port == 0) {
80003118:	01645783          	lhu	a5,22(s0)
8000311c:	00079e63          	bnez	a5,80003138 <udp_sendto_if_src+0x84>
    err = udp_bind(pcb, &pcb->local_ip, pcb->local_port);
80003120:	00000613          	li	a2,0
80003124:	00040593          	mv	a1,s0
80003128:	00040513          	mv	a0,s0
8000312c:	e99ff0ef          	jal	ra,80002fc4 <udp_bind>
80003130:	00050493          	mv	s1,a0
    if (err != ERR_OK) {
80003134:	02051863          	bnez	a0,80003164 <udp_sendto_if_src+0xb0>
  if ((u16_t)(p->tot_len + UDP_HLEN) < p->tot_len) {
80003138:	0089d703          	lhu	a4,8(s3)
8000313c:	000107b7          	lui	a5,0x10
80003140:	ff778793          	addi	a5,a5,-9 # fff7 <_stack_size+0xf7f7>
80003144:	04e7f863          	bgeu	a5,a4,80003194 <udp_sendto_if_src+0xe0>
    return ERR_MEM;
80003148:	fff00493          	li	s1,-1
8000314c:	0180006f          	j	80003164 <udp_sendto_if_src+0xb0>
      ip_addr_isbroadcast(dst_ip, netif)) {
80003150:	00062503          	lw	a0,0(a2) # c000 <_stack_size+0xb800>
80003154:	00070593          	mv	a1,a4
    return ERR_VAL;
80003158:	ffa00493          	li	s1,-6
      ip_addr_isbroadcast(dst_ip, netif)) {
8000315c:	1d9020ef          	jal	ra,80005b34 <ip4_addr_isbroadcast_u32>
  if (!ip_get_option(pcb, SOF_BROADCAST) &&
80003160:	fa050ce3          	beqz	a0,80003118 <udp_sendto_if_src+0x64>
}
80003164:	02c12083          	lw	ra,44(sp)
80003168:	02812403          	lw	s0,40(sp)
8000316c:	00048513          	mv	a0,s1
80003170:	02012903          	lw	s2,32(sp)
80003174:	02412483          	lw	s1,36(sp)
80003178:	01c12983          	lw	s3,28(sp)
8000317c:	01812a03          	lw	s4,24(sp)
80003180:	01412a83          	lw	s5,20(sp)
80003184:	01012b03          	lw	s6,16(sp)
80003188:	00c12b83          	lw	s7,12(sp)
8000318c:	03010113          	addi	sp,sp,48
80003190:	00008067          	ret
  if (pbuf_add_header(p, UDP_HLEN)) {
80003194:	00800593          	li	a1,8
80003198:	00098513          	mv	a0,s3
8000319c:	f1cfe0ef          	jal	ra,800018b8 <pbuf_add_header>
800031a0:	10050063          	beqz	a0,800032a0 <udp_sendto_if_src+0x1ec>
    q = pbuf_alloc(PBUF_IP, UDP_HLEN, PBUF_RAM);
800031a4:	28000613          	li	a2,640
800031a8:	00800593          	li	a1,8
800031ac:	02400513          	li	a0,36
800031b0:	875fe0ef          	jal	ra,80001a24 <pbuf_alloc>
800031b4:	00050913          	mv	s2,a0
    if (q == NULL) {
800031b8:	f80508e3          	beqz	a0,80003148 <udp_sendto_if_src+0x94>
    if (p->tot_len != 0) {
800031bc:	0089d783          	lhu	a5,8(s3)
800031c0:	00078663          	beqz	a5,800031cc <udp_sendto_if_src+0x118>
      pbuf_chain(q, p);
800031c4:	00098593          	mv	a1,s3
800031c8:	bcdfe0ef          	jal	ra,80001d94 <pbuf_chain>
  udphdr->src = lwip_htons(pcb->local_port);
800031cc:	01645503          	lhu	a0,22(s0)
  udphdr = (struct udp_hdr *)q->payload;
800031d0:	00492483          	lw	s1,4(s2)
  udphdr->src = lwip_htons(pcb->local_port);
800031d4:	f14fd0ef          	jal	ra,800008e8 <lwip_htons>
800031d8:	00a48023          	sb	a0,0(s1)
800031dc:	00855513          	srli	a0,a0,0x8
800031e0:	00a480a3          	sb	a0,1(s1)
  udphdr->dest = lwip_htons(dst_port);
800031e4:	000b8513          	mv	a0,s7
800031e8:	f00fd0ef          	jal	ra,800008e8 <lwip_htons>
800031ec:	00a48123          	sb	a0,2(s1)
800031f0:	00855513          	srli	a0,a0,0x8
800031f4:	00a481a3          	sb	a0,3(s1)
  udphdr->chksum = 0x0000;
800031f8:	00048323          	sb	zero,6(s1)
800031fc:	000483a3          	sb	zero,7(s1)
    udphdr->len = lwip_htons(q->tot_len);
80003200:	00895503          	lhu	a0,8(s2)
80003204:	ee4fd0ef          	jal	ra,800008e8 <lwip_htons>
80003208:	00a48223          	sb	a0,4(s1)
8000320c:	00855513          	srli	a0,a0,0x8
80003210:	00a482a3          	sb	a0,5(s1)
      if (IP_IS_V6(dst_ip) || (pcb->flags & UDP_FLAGS_NOCHKSUM) == 0) {
80003214:	01444783          	lbu	a5,20(s0)
80003218:	0017f793          	andi	a5,a5,1
8000321c:	02079a63          	bnez	a5,80003250 <udp_sendto_if_src+0x19c>
          udpchksum = ip_chksum_pseudo(q, IP_PROTO_UDP, q->tot_len,
80003220:	00895603          	lhu	a2,8(s2)
80003224:	000a8713          	mv	a4,s5
80003228:	000b0693          	mv	a3,s6
8000322c:	01100593          	li	a1,17
80003230:	00090513          	mv	a0,s2
80003234:	b15fd0ef          	jal	ra,80000d48 <ip_chksum_pseudo>
        if (udpchksum == 0x0000) {
80003238:	00051663          	bnez	a0,80003244 <udp_sendto_if_src+0x190>
          udpchksum = 0xffff;
8000323c:	00010537          	lui	a0,0x10
80003240:	fff50513          	addi	a0,a0,-1 # ffff <_stack_size+0xf7ff>
        udphdr->chksum = udpchksum;
80003244:	00a48323          	sb	a0,6(s1)
80003248:	00855513          	srli	a0,a0,0x8
8000324c:	00a483a3          	sb	a0,7(s1)
  NETIF_SET_HINTS(netif, &(pcb->netif_hints));
80003250:	00c40793          	addi	a5,s0,12
  ttl = pcb->ttl;
80003254:	00b44683          	lbu	a3,11(s0)
  NETIF_SET_HINTS(netif, &(pcb->netif_hints));
80003258:	02fa2c23          	sw	a5,56(s4)
  err = ip_output_if_src(q, src_ip, dst_ip, ttl, pcb->tos, ip_proto, netif);
8000325c:	00a44703          	lbu	a4,10(s0)
80003260:	000a0813          	mv	a6,s4
80003264:	01100793          	li	a5,17
80003268:	000a8613          	mv	a2,s5
8000326c:	000b0593          	mv	a1,s6
80003270:	00090513          	mv	a0,s2
80003274:	550020ef          	jal	ra,800057c4 <ip4_output_if_src>
  NETIF_RESET_HINTS(netif);
80003278:	020a2c23          	sw	zero,56(s4)
  err = ip_output_if_src(q, src_ip, dst_ip, ttl, pcb->tos, ip_proto, netif);
8000327c:	00050493          	mv	s1,a0
  if (q != p) {
80003280:	01390663          	beq	s2,s3,8000328c <udp_sendto_if_src+0x1d8>
    pbuf_free(q);
80003284:	00090513          	mv	a0,s2
80003288:	ec8fe0ef          	jal	ra,80001950 <pbuf_free>
  UDP_STATS_INC(udp.xmit);
8000328c:	8dc18793          	addi	a5,gp,-1828 # 80000424 <lwip_stats>
80003290:	0607d703          	lhu	a4,96(a5)
80003294:	00170713          	addi	a4,a4,1
80003298:	06e79023          	sh	a4,96(a5)
  return err;
8000329c:	ec9ff06f          	j	80003164 <udp_sendto_if_src+0xb0>
800032a0:	00098913          	mv	s2,s3
800032a4:	f29ff06f          	j	800031cc <udp_sendto_if_src+0x118>

800032a8 <udp_sendto_if>:
  LWIP_ERROR("udp_sendto_if: invalid pcb", pcb != NULL, return ERR_ARG);
800032a8:	02050e63          	beqz	a0,800032e4 <udp_sendto_if+0x3c>
  LWIP_ERROR("udp_sendto_if: invalid pbuf", p != NULL, return ERR_ARG);
800032ac:	02058c63          	beqz	a1,800032e4 <udp_sendto_if+0x3c>
  LWIP_ERROR("udp_sendto_if: invalid dst_ip", dst_ip != NULL, return ERR_ARG);
800032b0:	02060a63          	beqz	a2,800032e4 <udp_sendto_if+0x3c>
  LWIP_ERROR("udp_sendto_if: invalid netif", netif != NULL, return ERR_ARG);
800032b4:	02070863          	beqz	a4,800032e4 <udp_sendto_if+0x3c>
    if (ip4_addr_isany(ip_2_ip4(&pcb->local_ip)) ||
800032b8:	00052783          	lw	a5,0(a0)
800032bc:	00078863          	beqz	a5,800032cc <udp_sendto_if+0x24>
        ip4_addr_ismulticast(ip_2_ip4(&pcb->local_ip))) {
800032c0:	0f07f813          	andi	a6,a5,240
    if (ip4_addr_isany(ip_2_ip4(&pcb->local_ip)) ||
800032c4:	0e000893          	li	a7,224
800032c8:	01181663          	bne	a6,a7,800032d4 <udp_sendto_if+0x2c>
      src_ip = netif_ip_addr4(netif);
800032cc:	00470793          	addi	a5,a4,4
  return udp_sendto_if_src(pcb, p, dst_ip, dst_port, netif, src_ip);
800032d0:	de5ff06f          	j	800030b4 <udp_sendto_if_src>
      if (!ip4_addr_cmp(ip_2_ip4(&(pcb->local_ip)), netif_ip4_addr(netif))) {
800032d4:	00472803          	lw	a6,4(a4)
800032d8:	01079a63          	bne	a5,a6,800032ec <udp_sendto_if+0x44>
    if (ip4_addr_isany(ip_2_ip4(&pcb->local_ip)) ||
800032dc:	00050793          	mv	a5,a0
800032e0:	ff1ff06f          	j	800032d0 <udp_sendto_if+0x28>
  LWIP_ERROR("udp_sendto_if: invalid pcb", pcb != NULL, return ERR_ARG);
800032e4:	ff000513          	li	a0,-16
800032e8:	00008067          	ret
        return ERR_RTE;
800032ec:	ffc00513          	li	a0,-4
}
800032f0:	00008067          	ret

800032f4 <udp_sendto>:
  LWIP_ERROR("udp_sendto: invalid pcb", pcb != NULL, return ERR_ARG);
800032f4:	0a050863          	beqz	a0,800033a4 <udp_sendto+0xb0>
  LWIP_ERROR("udp_sendto: invalid pbuf", p != NULL, return ERR_ARG);
800032f8:	0a058663          	beqz	a1,800033a4 <udp_sendto+0xb0>
  LWIP_ERROR("udp_sendto: invalid dst_ip", dst_ip != NULL, return ERR_ARG);
800032fc:	0a060463          	beqz	a2,800033a4 <udp_sendto+0xb0>
{
80003300:	fe010113          	addi	sp,sp,-32
80003304:	00812c23          	sw	s0,24(sp)
80003308:	00912a23          	sw	s1,20(sp)
8000330c:	01212823          	sw	s2,16(sp)
80003310:	01312623          	sw	s3,12(sp)
80003314:	00112e23          	sw	ra,28(sp)
80003318:	00050413          	mv	s0,a0
  if (pcb->netif_idx != NETIF_NO_INDEX) {
8000331c:	00854503          	lbu	a0,8(a0)
80003320:	00068993          	mv	s3,a3
80003324:	00060493          	mv	s1,a2
80003328:	00058913          	mv	s2,a1
8000332c:	02050e63          	beqz	a0,80003368 <udp_sendto+0x74>
    netif = netif_get_by_index(pcb->netif_idx);
80003330:	af8fe0ef          	jal	ra,80001628 <netif_get_by_index>
  if (netif == NULL) {
80003334:	04051063          	bnez	a0,80003374 <udp_sendto+0x80>
    UDP_STATS_INC(udp.rterr);
80003338:	8dc18793          	addi	a5,gp,-1828 # 80000424 <lwip_stats>
8000333c:	06e7d703          	lhu	a4,110(a5)
80003340:	ffc00513          	li	a0,-4
80003344:	00170713          	addi	a4,a4,1
80003348:	06e79723          	sh	a4,110(a5)
}
8000334c:	01c12083          	lw	ra,28(sp)
80003350:	01812403          	lw	s0,24(sp)
80003354:	01412483          	lw	s1,20(sp)
80003358:	01012903          	lw	s2,16(sp)
8000335c:	00c12983          	lw	s3,12(sp)
80003360:	02010113          	addi	sp,sp,32
80003364:	00008067          	ret
      netif = ip_route(&pcb->local_ip, dst_ip);
80003368:	00060513          	mv	a0,a2
8000336c:	044020ef          	jal	ra,800053b0 <ip4_route>
80003370:	fc5ff06f          	j	80003334 <udp_sendto+0x40>
  return udp_sendto_if(pcb, p, dst_ip, dst_port, netif);
80003374:	00050713          	mv	a4,a0
80003378:	00040513          	mv	a0,s0
}
8000337c:	01812403          	lw	s0,24(sp)
80003380:	01c12083          	lw	ra,28(sp)
  return udp_sendto_if(pcb, p, dst_ip, dst_port, netif);
80003384:	00098693          	mv	a3,s3
80003388:	00048613          	mv	a2,s1
}
8000338c:	00c12983          	lw	s3,12(sp)
80003390:	01412483          	lw	s1,20(sp)
  return udp_sendto_if(pcb, p, dst_ip, dst_port, netif);
80003394:	00090593          	mv	a1,s2
}
80003398:	01012903          	lw	s2,16(sp)
8000339c:	02010113          	addi	sp,sp,32
  return udp_sendto_if(pcb, p, dst_ip, dst_port, netif);
800033a0:	f09ff06f          	j	800032a8 <udp_sendto_if>
  LWIP_ERROR("udp_sendto: invalid pcb", pcb != NULL, return ERR_ARG);
800033a4:	ff000513          	li	a0,-16
}
800033a8:	00008067          	ret

800033ac <udp_send>:
  LWIP_ERROR("udp_send: invalid pcb", pcb != NULL, return ERR_ARG);
800033ac:	00050a63          	beqz	a0,800033c0 <udp_send+0x14>
  LWIP_ERROR("udp_send: invalid pbuf", p != NULL, return ERR_ARG);
800033b0:	00058863          	beqz	a1,800033c0 <udp_send+0x14>
  return udp_sendto(pcb, p, &pcb->remote_ip, pcb->remote_port);
800033b4:	01855683          	lhu	a3,24(a0)
800033b8:	00450613          	addi	a2,a0,4
800033bc:	f39ff06f          	j	800032f4 <udp_sendto>
}
800033c0:	ff000513          	li	a0,-16
800033c4:	00008067          	ret

800033c8 <udp_bind_netif>:
  LWIP_ASSERT_CORE_LOCKED();

  if (netif != NULL) {
    pcb->netif_idx = netif_get_index(netif);
  } else {
    pcb->netif_idx = NETIF_NO_INDEX;
800033c8:	00000793          	li	a5,0
  if (netif != NULL) {
800033cc:	00058863          	beqz	a1,800033dc <udp_bind_netif+0x14>
    pcb->netif_idx = netif_get_index(netif);
800033d0:	0345c783          	lbu	a5,52(a1)
800033d4:	00178793          	addi	a5,a5,1
800033d8:	0ff7f793          	andi	a5,a5,255
800033dc:	00f50423          	sb	a5,8(a0)
  }
}
800033e0:	00008067          	ret

800033e4 <udp_connect>:
{
  struct udp_pcb *ipcb;

  LWIP_ASSERT_CORE_LOCKED();

  LWIP_ERROR("udp_connect: invalid pcb", pcb != NULL, return ERR_ARG);
800033e4:	ff000793          	li	a5,-16
800033e8:	0a050063          	beqz	a0,80003488 <udp_connect+0xa4>
  LWIP_ERROR("udp_connect: invalid ipaddr", ipaddr != NULL, return ERR_ARG);
800033ec:	08058e63          	beqz	a1,80003488 <udp_connect+0xa4>
{
800033f0:	ff010113          	addi	sp,sp,-16
800033f4:	00812423          	sw	s0,8(sp)
800033f8:	00912223          	sw	s1,4(sp)
800033fc:	01212023          	sw	s2,0(sp)
80003400:	00112623          	sw	ra,12(sp)

  if (pcb->local_port == 0) {
80003404:	01655783          	lhu	a5,22(a0)
80003408:	00050413          	mv	s0,a0
8000340c:	00060913          	mv	s2,a2
80003410:	00058493          	mv	s1,a1
80003414:	02078c63          	beqz	a5,8000344c <udp_connect+0x68>
    if (err != ERR_OK) {
      return err;
    }
  }

  ip_addr_set_ipaddr(&pcb->remote_ip, ipaddr);
80003418:	0004a783          	lw	a5,0(s1)
      ip6_addr_lacks_zone(ip_2_ip6(&pcb->remote_ip), IP6_UNKNOWN)) {
    ip6_addr_select_zone(ip_2_ip6(&pcb->remote_ip), ip_2_ip6(&pcb->local_ip));
  }
#endif /* LWIP_IPV6 && LWIP_IPV6_SCOPES */

  pcb->remote_port = port;
8000341c:	01241c23          	sh	s2,24(s0)
  ip_addr_set_ipaddr(&pcb->remote_ip, ipaddr);
80003420:	00f42223          	sw	a5,4(s0)
  pcb->flags |= UDP_FLAGS_CONNECTED;
80003424:	01444783          	lbu	a5,20(s0)
80003428:	0047e793          	ori	a5,a5,4
8000342c:	00f40a23          	sb	a5,20(s0)
  ip_addr_debug_print_val(UDP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_STATE,
                          pcb->remote_ip);
  LWIP_DEBUGF(UDP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_STATE, (", port %"U16_F")\n", pcb->remote_port));

  /* Insert UDP PCB into the list of active UDP PCBs. */
  for (ipcb = udp_pcbs; ipcb != NULL; ipcb = ipcb->next) {
80003430:	8781a683          	lw	a3,-1928(gp) # 800003c0 <udp_pcbs>
80003434:	00068713          	mv	a4,a3
80003438:	04071263          	bnez	a4,8000347c <udp_connect+0x98>
      /* already on the list, just return */
      return ERR_OK;
    }
  }
  /* PCB not yet on the list, add PCB now */
  pcb->next = udp_pcbs;
8000343c:	00d42823          	sw	a3,16(s0)
  udp_pcbs = pcb;
80003440:	8681ac23          	sw	s0,-1928(gp) # 800003c0 <udp_pcbs>
      return ERR_OK;
80003444:	00000793          	li	a5,0
80003448:	0180006f          	j	80003460 <udp_connect+0x7c>
    err_t err = udp_bind(pcb, &pcb->local_ip, pcb->local_port);
8000344c:	00000613          	li	a2,0
80003450:	00050593          	mv	a1,a0
80003454:	b71ff0ef          	jal	ra,80002fc4 <udp_bind>
80003458:	00050793          	mv	a5,a0
    if (err != ERR_OK) {
8000345c:	fa050ee3          	beqz	a0,80003418 <udp_connect+0x34>
  return ERR_OK;
}
80003460:	00c12083          	lw	ra,12(sp)
80003464:	00812403          	lw	s0,8(sp)
80003468:	00412483          	lw	s1,4(sp)
8000346c:	00012903          	lw	s2,0(sp)
80003470:	00078513          	mv	a0,a5
80003474:	01010113          	addi	sp,sp,16
80003478:	00008067          	ret
    if (pcb == ipcb) {
8000347c:	fc8704e3          	beq	a4,s0,80003444 <udp_connect+0x60>
  for (ipcb = udp_pcbs; ipcb != NULL; ipcb = ipcb->next) {
80003480:	01072703          	lw	a4,16(a4)
80003484:	fb5ff06f          	j	80003438 <udp_connect+0x54>
}
80003488:	00078513          	mv	a0,a5
8000348c:	00008067          	ret

80003490 <udp_disconnect>:
void
udp_disconnect(struct udp_pcb *pcb)
{
  LWIP_ASSERT_CORE_LOCKED();

  LWIP_ERROR("udp_disconnect: invalid pcb", pcb != NULL, return);
80003490:	00050e63          	beqz	a0,800034ac <udp_disconnect+0x1c>
  }
#endif
  pcb->remote_port = 0;
  pcb->netif_idx = NETIF_NO_INDEX;
  /* mark PCB as unconnected */
  udp_clear_flags(pcb, UDP_FLAGS_CONNECTED);
80003494:	01454783          	lbu	a5,20(a0)
    ip_addr_set_any(IP_IS_V6_VAL(pcb->remote_ip), &pcb->remote_ip);
80003498:	00052223          	sw	zero,4(a0)
  pcb->remote_port = 0;
8000349c:	00051c23          	sh	zero,24(a0)
  udp_clear_flags(pcb, UDP_FLAGS_CONNECTED);
800034a0:	ffb7f793          	andi	a5,a5,-5
  pcb->netif_idx = NETIF_NO_INDEX;
800034a4:	00050423          	sb	zero,8(a0)
  udp_clear_flags(pcb, UDP_FLAGS_CONNECTED);
800034a8:	00f50a23          	sb	a5,20(a0)
}
800034ac:	00008067          	ret

800034b0 <udp_recv>:
void
udp_recv(struct udp_pcb *pcb, udp_recv_fn recv, void *recv_arg)
{
  LWIP_ASSERT_CORE_LOCKED();

  LWIP_ERROR("udp_recv: invalid pcb", pcb != NULL, return);
800034b0:	00050663          	beqz	a0,800034bc <udp_recv+0xc>

  /* remember recv() callback and user data */
  pcb->recv = recv;
800034b4:	00b52e23          	sw	a1,28(a0)
  pcb->recv_arg = recv_arg;
800034b8:	02c52023          	sw	a2,32(a0)
}
800034bc:	00008067          	ret

800034c0 <udp_remove>:
 *
 * @see udp_new()
 */
void
udp_remove(struct udp_pcb *pcb)
{
800034c0:	00050593          	mv	a1,a0
  struct udp_pcb *pcb2;

  LWIP_ASSERT_CORE_LOCKED();

  LWIP_ERROR("udp_remove: invalid pcb", pcb != NULL, return);
800034c4:	02050e63          	beqz	a0,80003500 <udp_remove+0x40>

  mib2_udp_unbind(pcb);
  /* pcb to be removed is first in list? */
  if (udp_pcbs == pcb) {
800034c8:	8781a783          	lw	a5,-1928(gp) # 800003c0 <udp_pcbs>
800034cc:	00a79c63          	bne	a5,a0,800034e4 <udp_remove+0x24>
    /* make list start at 2nd pcb */
    udp_pcbs = udp_pcbs->next;
800034d0:	01052783          	lw	a5,16(a0)
800034d4:	86f1ac23          	sw	a5,-1928(gp) # 800003c0 <udp_pcbs>
        pcb2->next = pcb->next;
        break;
      }
    }
  }
  memp_free(MEMP_UDP_PCB, pcb);
800034d8:	00100513          	li	a0,1
800034dc:	bddfd06f          	j	800010b8 <memp_free>
800034e0:	00070793          	mv	a5,a4
    for (pcb2 = udp_pcbs; pcb2 != NULL; pcb2 = pcb2->next) {
800034e4:	fe078ae3          	beqz	a5,800034d8 <udp_remove+0x18>
      if (pcb2->next != NULL && pcb2->next == pcb) {
800034e8:	0107a703          	lw	a4,16(a5)
800034ec:	fe0706e3          	beqz	a4,800034d8 <udp_remove+0x18>
800034f0:	feb718e3          	bne	a4,a1,800034e0 <udp_remove+0x20>
        pcb2->next = pcb->next;
800034f4:	0105a703          	lw	a4,16(a1)
800034f8:	00e7a823          	sw	a4,16(a5)
        break;
800034fc:	fddff06f          	j	800034d8 <udp_remove+0x18>
}
80003500:	00008067          	ret

80003504 <udp_new>:
 *
 * @see udp_remove()
 */
struct udp_pcb *
udp_new(void)
{
80003504:	ff010113          	addi	sp,sp,-16
  struct udp_pcb *pcb;

  LWIP_ASSERT_CORE_LOCKED();

  pcb = (struct udp_pcb *)memp_malloc(MEMP_UDP_PCB);
80003508:	00100513          	li	a0,1
{
8000350c:	00812423          	sw	s0,8(sp)
80003510:	00112623          	sw	ra,12(sp)
  pcb = (struct udp_pcb *)memp_malloc(MEMP_UDP_PCB);
80003514:	b41fd0ef          	jal	ra,80001054 <memp_malloc>
80003518:	00050413          	mv	s0,a0
  /* could allocate UDP PCB? */
  if (pcb != NULL) {
8000351c:	00050c63          	beqz	a0,80003534 <udp_new+0x30>
    /* UDP Lite: by initializing to all zeroes, chksum_len is set to 0
     * which means checksum is generated over the whole datagram per default
     * (recommended as default by RFC 3828). */
    /* initialize PCB to all zeroes */
    memset(pcb, 0, sizeof(struct udp_pcb));
80003520:	02400613          	li	a2,36
80003524:	00000593          	li	a1,0
80003528:	289020ef          	jal	ra,80005fb0 <memset>
    pcb->ttl = UDP_TTL;
8000352c:	fff00793          	li	a5,-1
80003530:	00f405a3          	sb	a5,11(s0)
#if LWIP_MULTICAST_TX_OPTIONS
    udp_set_multicast_ttl(pcb, UDP_TTL);
#endif /* LWIP_MULTICAST_TX_OPTIONS */
  }
  return pcb;
}
80003534:	00040513          	mv	a0,s0
80003538:	00c12083          	lw	ra,12(sp)
8000353c:	00812403          	lw	s0,8(sp)
80003540:	01010113          	addi	sp,sp,16
80003544:	00008067          	ret

80003548 <udp_new_ip_type>:
{
  struct udp_pcb *pcb;

  LWIP_ASSERT_CORE_LOCKED();

  pcb = udp_new();
80003548:	fbdff06f          	j	80003504 <udp_new>

8000354c <udp_netif_ip_addr_changed>:
 */
void udp_netif_ip_addr_changed(const ip_addr_t *old_addr, const ip_addr_t *new_addr)
{
  struct udp_pcb *upcb;

  if (!ip_addr_isany(old_addr) && !ip_addr_isany(new_addr)) {
8000354c:	02050063          	beqz	a0,8000356c <udp_netif_ip_addr_changed+0x20>
80003550:	00052783          	lw	a5,0(a0)
80003554:	00078c63          	beqz	a5,8000356c <udp_netif_ip_addr_changed+0x20>
80003558:	00058a63          	beqz	a1,8000356c <udp_netif_ip_addr_changed+0x20>
8000355c:	0005a783          	lw	a5,0(a1)
80003560:	00078663          	beqz	a5,8000356c <udp_netif_ip_addr_changed+0x20>
    for (upcb = udp_pcbs; upcb != NULL; upcb = upcb->next) {
80003564:	8781a783          	lw	a5,-1928(gp) # 800003c0 <udp_pcbs>
80003568:	00079463          	bnez	a5,80003570 <udp_netif_ip_addr_changed+0x24>
         * is set to bound to the new one instead */
        ip_addr_copy(upcb->local_ip, *new_addr);
      }
    }
  }
}
8000356c:	00008067          	ret
      if (ip_addr_cmp(&upcb->local_ip, old_addr)) {
80003570:	0007a683          	lw	a3,0(a5)
80003574:	00052703          	lw	a4,0(a0)
80003578:	00e69663          	bne	a3,a4,80003584 <udp_netif_ip_addr_changed+0x38>
        ip_addr_copy(upcb->local_ip, *new_addr);
8000357c:	0005a703          	lw	a4,0(a1)
80003580:	00e7a023          	sw	a4,0(a5)
    for (upcb = udp_pcbs; upcb != NULL; upcb = upcb->next) {
80003584:	0107a783          	lw	a5,16(a5)
80003588:	fe1ff06f          	j	80003568 <udp_netif_ip_addr_changed+0x1c>

8000358c <etharp_free_entry>:
#endif /* ARP_QUEUEING */

/** Clean up ARP table entries */
static void
etharp_free_entry(int i)
{
8000358c:	fe010113          	addi	sp,sp,-32
80003590:	00912a23          	sw	s1,20(sp)
  /* remove from SNMP ARP index tree */
  mib2_remove_arp_entry(arp_table[i].netif, &arp_table[i].ipaddr);
  /* and empty packet queue */
  if (arp_table[i].q != NULL) {
80003594:	00151493          	slli	s1,a0,0x1
80003598:	00a48733          	add	a4,s1,a0
{
8000359c:	01212823          	sw	s2,16(sp)
  if (arp_table[i].q != NULL) {
800035a0:	00371713          	slli	a4,a4,0x3
800035a4:	95418913          	addi	s2,gp,-1708 # 8000049c <arp_table>
{
800035a8:	00812c23          	sw	s0,24(sp)
800035ac:	01312623          	sw	s3,12(sp)
800035b0:	00112e23          	sw	ra,28(sp)
  if (arp_table[i].q != NULL) {
800035b4:	00e90933          	add	s2,s2,a4
{
800035b8:	00050993          	mv	s3,a0
  if (arp_table[i].q != NULL) {
800035bc:	00092503          	lw	a0,0(s2)
800035c0:	95418413          	addi	s0,gp,-1708 # 8000049c <arp_table>
800035c4:	00050663          	beqz	a0,800035d0 <etharp_free_entry+0x44>
    /* remove all queued packets */
    LWIP_DEBUGF(ETHARP_DEBUG, ("etharp_free_entry: freeing entry %"U16_F", packet queue %p.\n", (u16_t)i, (void *)(arp_table[i].q)));
    free_etharp_q(arp_table[i].q);
800035c8:	b88fe0ef          	jal	ra,80001950 <pbuf_free>
    arp_table[i].q = NULL;
800035cc:	00092023          	sw	zero,0(s2)
  }
  /* recycle entry for re-use */
  arp_table[i].state = ETHARP_STATE_EMPTY;
800035d0:	01348533          	add	a0,s1,s3
800035d4:	00351513          	slli	a0,a0,0x3
800035d8:	00a40533          	add	a0,s0,a0
#ifdef LWIP_DEBUG
  /* for debugging, clean out the complete entry */
  arp_table[i].ctime = 0;
  arp_table[i].netif = NULL;
  ip4_addr_set_zero(&arp_table[i].ipaddr);
  arp_table[i].ethaddr = ethzero;
800035dc:	800005b7          	lui	a1,0x80000
  arp_table[i].state = ETHARP_STATE_EMPTY;
800035e0:	00050a23          	sb	zero,20(a0)
  arp_table[i].ctime = 0;
800035e4:	00051923          	sh	zero,18(a0)
  arp_table[i].netif = NULL;
800035e8:	00052423          	sw	zero,8(a0)
  ip4_addr_set_zero(&arp_table[i].ipaddr);
800035ec:	00052223          	sw	zero,4(a0)
  arp_table[i].ethaddr = ethzero;
800035f0:	00600613          	li	a2,6
800035f4:	35058593          	addi	a1,a1,848 # 80000350 <_stack_start+0xffff95c0>
800035f8:	00c50513          	addi	a0,a0,12
800035fc:	3c1020ef          	jal	ra,800061bc <memcpy>
#endif /* LWIP_DEBUG */
}
80003600:	01c12083          	lw	ra,28(sp)
80003604:	01812403          	lw	s0,24(sp)
80003608:	01412483          	lw	s1,20(sp)
8000360c:	01012903          	lw	s2,16(sp)
80003610:	00c12983          	lw	s3,12(sp)
80003614:	02010113          	addi	sp,sp,32
80003618:	00008067          	ret

8000361c <etharp_find_entry>:
 * @return The ARP entry index that matched or is created, ERR_MEM if no
 * entry is found or could be recycled.
 */
static s16_t
etharp_find_entry(const ip4_addr_t *ipaddr, u8_t flags, struct netif *netif)
{
8000361c:	fe010113          	addi	sp,sp,-32
80003620:	00812c23          	sw	s0,24(sp)
80003624:	00912a23          	sw	s1,20(sp)
80003628:	95418693          	addi	a3,gp,-1708 # 8000049c <arp_table>
8000362c:	00112e23          	sw	ra,28(sp)
80003630:	01212823          	sw	s2,16(sp)
80003634:	01312623          	sw	s3,12(sp)
  s16_t empty = ARP_TABLE_SIZE;
  s16_t i = 0;
  /* oldest entry with packets on queue */
  s16_t old_queue = ARP_TABLE_SIZE;
  /* its age */
  u16_t age_queue = 0, age_pending = 0, age_stable = 0;
80003638:	00000293          	li	t0,0
8000363c:	00000f93          	li	t6,0
80003640:	00000f13          	li	t5,0
  s16_t old_queue = ARP_TABLE_SIZE;
80003644:	00a00313          	li	t1,10
   * 4) remember the oldest pending entry with queued packets (if any)
   * 5) search for a matching IP entry, either pending or stable
   *    until 5 matches, or all entries are searched for.
   */

  for (i = 0; i < ARP_TABLE_SIZE; ++i) {
80003648:	00000793          	li	a5,0
  s16_t empty = ARP_TABLE_SIZE;
8000364c:	00a00713          	li	a4,10
  s16_t old_pending = ARP_TABLE_SIZE, old_stable = ARP_TABLE_SIZE;
80003650:	00a00893          	li	a7,10
80003654:	00a00493          	li	s1,10
80003658:	95418413          	addi	s0,gp,-1708 # 8000049c <arp_table>
    u8_t state = arp_table[i].state;
    /* no empty entry found yet and now we do find one? */
    if ((empty == ARP_TABLE_SIZE) && (state == ETHARP_STATE_EMPTY)) {
8000365c:	00a00e13          	li	t3,10
        LWIP_DEBUGF(ETHARP_DEBUG | LWIP_DBG_TRACE, ("etharp_find_entry: found matching entry %d\n", (int)i));
        /* found exact IP address match, simply bail out */
        return i;
      }
      /* pending entry? */
      if (state == ETHARP_STATE_PENDING) {
80003660:	00100393          	li	t2,1
    u8_t state = arp_table[i].state;
80003664:	0146ce83          	lbu	t4,20(a3)
    if ((empty == ARP_TABLE_SIZE) && (state == ETHARP_STATE_EMPTY)) {
80003668:	05c71263          	bne	a4,t3,800036ac <etharp_find_entry+0x90>
8000366c:	100e8463          	beqz	t4,80003774 <etharp_find_entry+0x158>
      if (ipaddr && ip4_addr_cmp(ipaddr, &arp_table[i].ipaddr)
80003670:	00050e63          	beqz	a0,8000368c <etharp_find_entry+0x70>
80003674:	00052903          	lw	s2,0(a0)
80003678:	0046a803          	lw	a6,4(a3)
8000367c:	01091863          	bne	s2,a6,8000368c <etharp_find_entry+0x70>
          && ((netif == NULL) || (netif == arp_table[i].netif))
80003680:	0a060a63          	beqz	a2,80003734 <etharp_find_entry+0x118>
80003684:	0086a803          	lw	a6,8(a3)
80003688:	0ac80663          	beq	a6,a2,80003734 <etharp_find_entry+0x118>
      if (state == ETHARP_STATE_PENDING) {
8000368c:	0126d803          	lhu	a6,18(a3)
80003690:	0c7e9a63          	bne	t4,t2,80003764 <etharp_find_entry+0x148>
        /* pending with queued packets? */
        if (arp_table[i].q != NULL) {
80003694:	0006ae83          	lw	t4,0(a3)
80003698:	0a0e8e63          	beqz	t4,80003754 <etharp_find_entry+0x138>
          if (arp_table[i].ctime >= age_queue) {
8000369c:	01e86a63          	bltu	a6,t5,800036b0 <etharp_find_entry+0x94>
800036a0:	00080f13          	mv	t5,a6
800036a4:	00078313          	mv	t1,a5
800036a8:	0080006f          	j	800036b0 <etharp_find_entry+0x94>
    } else if (state != ETHARP_STATE_EMPTY) {
800036ac:	fc0e92e3          	bnez	t4,80003670 <etharp_find_entry+0x54>
800036b0:	00178793          	addi	a5,a5,1
800036b4:	01079813          	slli	a6,a5,0x10
800036b8:	01085813          	srli	a6,a6,0x10
800036bc:	01081793          	slli	a5,a6,0x10
800036c0:	01868693          	addi	a3,a3,24
800036c4:	4107d793          	srai	a5,a5,0x10
  for (i = 0; i < ARP_TABLE_SIZE; ++i) {
800036c8:	f9c81ee3          	bne	a6,t3,80003664 <etharp_find_entry+0x48>
    }
  }
  /* { we have no match } => try to create a new entry */

  /* don't create new entry, only search? */
  if (((flags & ETHARP_FLAG_FIND_ONLY) != 0) ||
800036cc:	00100693          	li	a3,1
      /* or no empty entry found and not allowed to recycle? */
      ((empty == ARP_TABLE_SIZE) && ((flags & ETHARP_FLAG_TRY_HARD) == 0))) {
    LWIP_DEBUGF(ETHARP_DEBUG | LWIP_DBG_TRACE, ("etharp_find_entry: no empty entry found and not allowed to recycle\n"));
    return (s16_t)ERR_MEM;
800036d0:	fff00793          	li	a5,-1
  if (((flags & ETHARP_FLAG_FIND_ONLY) != 0) ||
800036d4:	06d59063          	bne	a1,a3,80003734 <etharp_find_entry+0x118>
800036d8:	00060993          	mv	s3,a2
800036dc:	00050913          	mv	s2,a0
800036e0:	03c71063          	bne	a4,t3,80003700 <etharp_find_entry+0xe4>
  if (empty < ARP_TABLE_SIZE) {
    i = empty;
    LWIP_DEBUGF(ETHARP_DEBUG | LWIP_DBG_TRACE, ("etharp_find_entry: selecting empty entry %d\n", (int)i));
  } else {
    /* 2) found recyclable stable entry? */
    if (old_stable < ARP_TABLE_SIZE) {
800036e4:	08e89c63          	bne	a7,a4,8000377c <etharp_find_entry+0x160>
      i = old_stable;
      LWIP_DEBUGF(ETHARP_DEBUG | LWIP_DBG_TRACE, ("etharp_find_entry: selecting oldest stable entry %d\n", (int)i));
      /* no queued packets should exist on stable entries */
      LWIP_ASSERT("arp_table[i].q == NULL", arp_table[i].q == NULL);
      /* 3) found recyclable pending entry without queued packets? */
    } else if (old_pending < ARP_TABLE_SIZE) {
800036e8:	01149663          	bne	s1,a7,800036f4 <etharp_find_entry+0xd8>
      /* recycle oldest pending */
      i = old_pending;
      LWIP_DEBUGF(ETHARP_DEBUG | LWIP_DBG_TRACE, ("etharp_find_entry: selecting oldest pending entry %d (without queue)\n", (int)i));
      /* 4) found recyclable pending entry with queued packets? */
    } else if (old_queue < ARP_TABLE_SIZE) {
800036ec:	04930463          	beq	t1,s1,80003734 <etharp_find_entry+0x118>
800036f0:	00030493          	mv	s1,t1
      return (s16_t)ERR_MEM;
    }

    /* { empty or recyclable entry found } */
    LWIP_ASSERT("i < ARP_TABLE_SIZE", i < ARP_TABLE_SIZE);
    etharp_free_entry(i);
800036f4:	00048513          	mv	a0,s1
800036f8:	e95ff0ef          	jal	ra,8000358c <etharp_free_entry>
800036fc:	00048713          	mv	a4,s1
  LWIP_ASSERT("i < ARP_TABLE_SIZE", i < ARP_TABLE_SIZE);
  LWIP_ASSERT("arp_table[i].state == ETHARP_STATE_EMPTY",
              arp_table[i].state == ETHARP_STATE_EMPTY);

  /* IP address given? */
  if (ipaddr != NULL) {
80003700:	00171793          	slli	a5,a4,0x1
80003704:	00090c63          	beqz	s2,8000371c <etharp_find_entry+0x100>
    /* set IP address */
    ip4_addr_copy(arp_table[i].ipaddr, *ipaddr);
80003708:	00092603          	lw	a2,0(s2)
8000370c:	00e786b3          	add	a3,a5,a4
80003710:	00369693          	slli	a3,a3,0x3
80003714:	00d406b3          	add	a3,s0,a3
80003718:	00c6a223          	sw	a2,4(a3)
  }
  arp_table[i].ctime = 0;
8000371c:	00e787b3          	add	a5,a5,a4
80003720:	00379793          	slli	a5,a5,0x3
80003724:	00f407b3          	add	a5,s0,a5
80003728:	00079923          	sh	zero,18(a5)
#if ETHARP_TABLE_MATCH_NETIF
  arp_table[i].netif = netif;
8000372c:	0137a423          	sw	s3,8(a5)
#endif /* ETHARP_TABLE_MATCH_NETIF */
  return (s16_t)i;
80003730:	00070793          	mv	a5,a4
}
80003734:	01c12083          	lw	ra,28(sp)
80003738:	01812403          	lw	s0,24(sp)
8000373c:	01412483          	lw	s1,20(sp)
80003740:	01012903          	lw	s2,16(sp)
80003744:	00c12983          	lw	s3,12(sp)
80003748:	00078513          	mv	a0,a5
8000374c:	02010113          	addi	sp,sp,32
80003750:	00008067          	ret
          if (arp_table[i].ctime >= age_pending) {
80003754:	f5f86ee3          	bltu	a6,t6,800036b0 <etharp_find_entry+0x94>
80003758:	00080f93          	mv	t6,a6
8000375c:	00078493          	mv	s1,a5
80003760:	f51ff06f          	j	800036b0 <etharp_find_entry+0x94>
          if (arp_table[i].ctime >= age_stable) {
80003764:	f45866e3          	bltu	a6,t0,800036b0 <etharp_find_entry+0x94>
80003768:	00080293          	mv	t0,a6
8000376c:	00078893          	mv	a7,a5
80003770:	f41ff06f          	j	800036b0 <etharp_find_entry+0x94>
80003774:	00078713          	mv	a4,a5
80003778:	f39ff06f          	j	800036b0 <etharp_find_entry+0x94>
8000377c:	00088493          	mv	s1,a7
80003780:	f75ff06f          	j	800036f4 <etharp_find_entry+0xd8>

80003784 <etharp_raw>:
etharp_raw(struct netif *netif, const struct eth_addr *ethsrc_addr,
           const struct eth_addr *ethdst_addr,
           const struct eth_addr *hwsrc_addr, const ip4_addr_t *ipsrc_addr,
           const struct eth_addr *hwdst_addr, const ip4_addr_t *ipdst_addr,
           const u16_t opcode)
{
80003784:	fc010113          	addi	sp,sp,-64
80003788:	03312623          	sw	s3,44(sp)
8000378c:	03412423          	sw	s4,40(sp)
80003790:	03512223          	sw	s5,36(sp)
80003794:	00050993          	mv	s3,a0
80003798:	00058a13          	mv	s4,a1
8000379c:	00060a93          	mv	s5,a2
  struct etharp_hdr *hdr;

  LWIP_ASSERT("netif != NULL", netif != NULL);

  /* allocate a pbuf for the outgoing ARP request packet */
  p = pbuf_alloc(PBUF_LINK, SIZEOF_ETHARP_HDR, PBUF_RAM);
800037a0:	01c00593          	li	a1,28
800037a4:	28000613          	li	a2,640
800037a8:	01000513          	li	a0,16
{
800037ac:	02912a23          	sw	s1,52(sp)
800037b0:	03612023          	sw	s6,32(sp)
800037b4:	01712e23          	sw	s7,28(sp)
800037b8:	01812c23          	sw	s8,24(sp)
800037bc:	01912a23          	sw	s9,20(sp)
800037c0:	01112623          	sw	a7,12(sp)
800037c4:	02112e23          	sw	ra,60(sp)
800037c8:	02812c23          	sw	s0,56(sp)
800037cc:	03212823          	sw	s2,48(sp)
800037d0:	00068c93          	mv	s9,a3
800037d4:	00070b93          	mv	s7,a4
800037d8:	00078c13          	mv	s8,a5
800037dc:	00080b13          	mv	s6,a6
  p = pbuf_alloc(PBUF_LINK, SIZEOF_ETHARP_HDR, PBUF_RAM);
800037e0:	a44fe0ef          	jal	ra,80001a24 <pbuf_alloc>
  /* could allocate a pbuf for an ARP request? */
  if (p == NULL) {
800037e4:	8dc18493          	addi	s1,gp,-1828 # 80000424 <lwip_stats>
800037e8:	00c12883          	lw	a7,12(sp)
800037ec:	04051463          	bnez	a0,80003834 <etharp_raw+0xb0>
    LWIP_DEBUGF(ETHARP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_LEVEL_SERIOUS,
                ("etharp_raw: could not allocate pbuf for ARP request.\n"));
    ETHARP_STATS_INC(etharp.memerr);
800037f0:	00c4d783          	lhu	a5,12(s1)
    return ERR_MEM;
800037f4:	fff00513          	li	a0,-1
    ETHARP_STATS_INC(etharp.memerr);
800037f8:	00178793          	addi	a5,a5,1
800037fc:	00f49623          	sh	a5,12(s1)
  pbuf_free(p);
  p = NULL;
  /* could not allocate pbuf for ARP request */

  return result;
}
80003800:	03c12083          	lw	ra,60(sp)
80003804:	03812403          	lw	s0,56(sp)
80003808:	03412483          	lw	s1,52(sp)
8000380c:	03012903          	lw	s2,48(sp)
80003810:	02c12983          	lw	s3,44(sp)
80003814:	02812a03          	lw	s4,40(sp)
80003818:	02412a83          	lw	s5,36(sp)
8000381c:	02012b03          	lw	s6,32(sp)
80003820:	01c12b83          	lw	s7,28(sp)
80003824:	01812c03          	lw	s8,24(sp)
80003828:	01412c83          	lw	s9,20(sp)
8000382c:	04010113          	addi	sp,sp,64
80003830:	00008067          	ret
  hdr = (struct etharp_hdr *)p->payload;
80003834:	00452403          	lw	s0,4(a0)
80003838:	00050913          	mv	s2,a0
  hdr->opcode = lwip_htons(opcode);
8000383c:	00088513          	mv	a0,a7
80003840:	8a8fd0ef          	jal	ra,800008e8 <lwip_htons>
80003844:	00a40323          	sb	a0,6(s0)
80003848:	00855513          	srli	a0,a0,0x8
8000384c:	00a403a3          	sb	a0,7(s0)
  SMEMCPY(&hdr->shwaddr, hwsrc_addr, ETH_HWADDR_LEN);
80003850:	00600613          	li	a2,6
80003854:	000c8593          	mv	a1,s9
80003858:	00840513          	addi	a0,s0,8
8000385c:	161020ef          	jal	ra,800061bc <memcpy>
  SMEMCPY(&hdr->dhwaddr, hwdst_addr, ETH_HWADDR_LEN);
80003860:	00600613          	li	a2,6
80003864:	000c0593          	mv	a1,s8
80003868:	01240513          	addi	a0,s0,18
8000386c:	151020ef          	jal	ra,800061bc <memcpy>
  IPADDR_WORDALIGNED_COPY_FROM_IP4_ADDR_T(&hdr->sipaddr, ipsrc_addr);
80003870:	00400613          	li	a2,4
80003874:	000b8593          	mv	a1,s7
80003878:	00e40513          	addi	a0,s0,14
8000387c:	141020ef          	jal	ra,800061bc <memcpy>
  IPADDR_WORDALIGNED_COPY_FROM_IP4_ADDR_T(&hdr->dipaddr, ipdst_addr);
80003880:	00400613          	li	a2,4
80003884:	000b0593          	mv	a1,s6
80003888:	01840513          	addi	a0,s0,24
8000388c:	131020ef          	jal	ra,800061bc <memcpy>
  hdr->hwtype = PP_HTONS(LWIP_IANA_HWTYPE_ETHERNET);
80003890:	00100793          	li	a5,1
80003894:	00f400a3          	sb	a5,1(s0)
  hdr->proto = PP_HTONS(ETHTYPE_IP);
80003898:	00800793          	li	a5,8
8000389c:	00f40123          	sb	a5,2(s0)
  hdr->hwlen = ETH_HWADDR_LEN;
800038a0:	00600793          	li	a5,6
800038a4:	00f40223          	sb	a5,4(s0)
    ethernet_output(netif, p, ethsrc_addr, ethdst_addr, ETHTYPE_ARP);
800038a8:	00001737          	lui	a4,0x1
  hdr->protolen = sizeof(ip4_addr_t);
800038ac:	00400793          	li	a5,4
800038b0:	00f402a3          	sb	a5,5(s0)
    ethernet_output(netif, p, ethsrc_addr, ethdst_addr, ETHTYPE_ARP);
800038b4:	80670713          	addi	a4,a4,-2042 # 806 <_stack_size+0x6>
800038b8:	000a8693          	mv	a3,s5
800038bc:	000a0613          	mv	a2,s4
800038c0:	00090593          	mv	a1,s2
  hdr->hwtype = PP_HTONS(LWIP_IANA_HWTYPE_ETHERNET);
800038c4:	00040023          	sb	zero,0(s0)
  hdr->proto = PP_HTONS(ETHTYPE_IP);
800038c8:	000401a3          	sb	zero,3(s0)
    ethernet_output(netif, p, ethsrc_addr, ethdst_addr, ETHTYPE_ARP);
800038cc:	00098513          	mv	a0,s3
800038d0:	f25fc0ef          	jal	ra,800007f4 <ethernet_output>
  ETHARP_STATS_INC(etharp.xmit);
800038d4:	0004d783          	lhu	a5,0(s1)
  pbuf_free(p);
800038d8:	00090513          	mv	a0,s2
  ETHARP_STATS_INC(etharp.xmit);
800038dc:	00178793          	addi	a5,a5,1
800038e0:	00f49023          	sh	a5,0(s1)
  pbuf_free(p);
800038e4:	86cfe0ef          	jal	ra,80001950 <pbuf_free>
  return result;
800038e8:	00000513          	li	a0,0
800038ec:	f15ff06f          	j	80003800 <etharp_raw+0x7c>

800038f0 <etharp_cleanup_netif>:
{
800038f0:	fe010113          	addi	sp,sp,-32
800038f4:	00812c23          	sw	s0,24(sp)
800038f8:	00912a23          	sw	s1,20(sp)
800038fc:	01212823          	sw	s2,16(sp)
80003900:	01312623          	sw	s3,12(sp)
80003904:	00112e23          	sw	ra,28(sp)
80003908:	00050993          	mv	s3,a0
8000390c:	95418413          	addi	s0,gp,-1708 # 8000049c <arp_table>
  for (i = 0; i < ARP_TABLE_SIZE; ++i) {
80003910:	00000493          	li	s1,0
80003914:	00a00913          	li	s2,10
    if ((state != ETHARP_STATE_EMPTY) && (arp_table[i].netif == netif)) {
80003918:	01444783          	lbu	a5,20(s0)
8000391c:	00078a63          	beqz	a5,80003930 <etharp_cleanup_netif+0x40>
80003920:	00842783          	lw	a5,8(s0)
80003924:	01379663          	bne	a5,s3,80003930 <etharp_cleanup_netif+0x40>
      etharp_free_entry(i);
80003928:	00048513          	mv	a0,s1
8000392c:	c61ff0ef          	jal	ra,8000358c <etharp_free_entry>
  for (i = 0; i < ARP_TABLE_SIZE; ++i) {
80003930:	00148493          	addi	s1,s1,1
80003934:	01840413          	addi	s0,s0,24
80003938:	ff2490e3          	bne	s1,s2,80003918 <etharp_cleanup_netif+0x28>
}
8000393c:	01c12083          	lw	ra,28(sp)
80003940:	01812403          	lw	s0,24(sp)
80003944:	01412483          	lw	s1,20(sp)
80003948:	01012903          	lw	s2,16(sp)
8000394c:	00c12983          	lw	s3,12(sp)
80003950:	02010113          	addi	sp,sp,32
80003954:	00008067          	ret

80003958 <etharp_find_addr>:
{
80003958:	fe010113          	addi	sp,sp,-32
8000395c:	00058793          	mv	a5,a1
80003960:	00812c23          	sw	s0,24(sp)
  i = etharp_find_entry(ipaddr, ETHARP_FLAG_FIND_ONLY, netif);
80003964:	00200593          	li	a1,2
{
80003968:	00060413          	mv	s0,a2
  i = etharp_find_entry(ipaddr, ETHARP_FLAG_FIND_ONLY, netif);
8000396c:	00050613          	mv	a2,a0
80003970:	00078513          	mv	a0,a5
{
80003974:	00112e23          	sw	ra,28(sp)
80003978:	00d12623          	sw	a3,12(sp)
  i = etharp_find_entry(ipaddr, ETHARP_FLAG_FIND_ONLY, netif);
8000397c:	ca1ff0ef          	jal	ra,8000361c <etharp_find_entry>
  if ((i >= 0) && (arp_table[i].state >= ETHARP_STATE_STABLE)) {
80003980:	04054863          	bltz	a0,800039d0 <etharp_find_addr+0x78>
80003984:	00151793          	slli	a5,a0,0x1
80003988:	00a787b3          	add	a5,a5,a0
8000398c:	95418713          	addi	a4,gp,-1708 # 8000049c <arp_table>
80003990:	00379793          	slli	a5,a5,0x3
80003994:	00f70633          	add	a2,a4,a5
80003998:	01464583          	lbu	a1,20(a2)
8000399c:	00100613          	li	a2,1
800039a0:	02b67863          	bgeu	a2,a1,800039d0 <etharp_find_addr+0x78>
    *eth_ret = &arp_table[i].ethaddr;
800039a4:	00c78613          	addi	a2,a5,12
    *ip_ret = &arp_table[i].ipaddr;
800039a8:	00c12683          	lw	a3,12(sp)
    *eth_ret = &arp_table[i].ethaddr;
800039ac:	00c70633          	add	a2,a4,a2
    *ip_ret = &arp_table[i].ipaddr;
800039b0:	00478793          	addi	a5,a5,4
    *eth_ret = &arp_table[i].ethaddr;
800039b4:	00c42023          	sw	a2,0(s0)
    *ip_ret = &arp_table[i].ipaddr;
800039b8:	00f707b3          	add	a5,a4,a5
800039bc:	00f6a023          	sw	a5,0(a3)
}
800039c0:	01c12083          	lw	ra,28(sp)
800039c4:	01812403          	lw	s0,24(sp)
800039c8:	02010113          	addi	sp,sp,32
800039cc:	00008067          	ret
  return -1;
800039d0:	fff00513          	li	a0,-1
800039d4:	fedff06f          	j	800039c0 <etharp_find_addr+0x68>

800039d8 <etharp_get_entry>:
  if ((i < ARP_TABLE_SIZE) && (arp_table[i].state >= ETHARP_STATE_STABLE)) {
800039d8:	00900793          	li	a5,9
800039dc:	04a7e863          	bltu	a5,a0,80003a2c <etharp_get_entry+0x54>
800039e0:	00151793          	slli	a5,a0,0x1
800039e4:	00a787b3          	add	a5,a5,a0
800039e8:	95418713          	addi	a4,gp,-1708 # 8000049c <arp_table>
800039ec:	00379793          	slli	a5,a5,0x3
800039f0:	00f70833          	add	a6,a4,a5
800039f4:	01484303          	lbu	t1,20(a6)
800039f8:	00100893          	li	a7,1
    return 0;
800039fc:	00000513          	li	a0,0
  if ((i < ARP_TABLE_SIZE) && (arp_table[i].state >= ETHARP_STATE_STABLE)) {
80003a00:	0268f863          	bgeu	a7,t1,80003a30 <etharp_get_entry+0x58>
    *ipaddr  = &arp_table[i].ipaddr;
80003a04:	00478513          	addi	a0,a5,4
80003a08:	00a70533          	add	a0,a4,a0
80003a0c:	00a5a023          	sw	a0,0(a1)
    *netif   = arp_table[i].netif;
80003a10:	00882583          	lw	a1,8(a6)
    *eth_ret = &arp_table[i].ethaddr;
80003a14:	00c78793          	addi	a5,a5,12
80003a18:	00f707b3          	add	a5,a4,a5
    *netif   = arp_table[i].netif;
80003a1c:	00b62023          	sw	a1,0(a2)
    *eth_ret = &arp_table[i].ethaddr;
80003a20:	00f6a023          	sw	a5,0(a3)
    return 1;
80003a24:	00100513          	li	a0,1
80003a28:	00008067          	ret
    return 0;
80003a2c:	00000513          	li	a0,0
}
80003a30:	00008067          	ret

80003a34 <etharp_input>:
  LWIP_ERROR("netif != NULL", (netif != NULL), return;);
80003a34:	22058063          	beqz	a1,80003c54 <etharp_input+0x220>
{
80003a38:	fc010113          	addi	sp,sp,-64
80003a3c:	02812c23          	sw	s0,56(sp)
80003a40:	03212823          	sw	s2,48(sp)
80003a44:	03312623          	sw	s3,44(sp)
80003a48:	02112e23          	sw	ra,60(sp)
80003a4c:	02912a23          	sw	s1,52(sp)
80003a50:	03412423          	sw	s4,40(sp)
80003a54:	03512223          	sw	s5,36(sp)
80003a58:	03612023          	sw	s6,32(sp)
80003a5c:	01712e23          	sw	s7,28(sp)
  hdr = (struct etharp_hdr *)p->payload;
80003a60:	00452483          	lw	s1,4(a0)
80003a64:	00058413          	mv	s0,a1
  if ((hdr->hwtype != PP_HTONS(LWIP_IANA_HWTYPE_ETHERNET)) ||
80003a68:	0014c783          	lbu	a5,1(s1)
80003a6c:	0004c703          	lbu	a4,0(s1)
80003a70:	00050993          	mv	s3,a0
80003a74:	00879793          	slli	a5,a5,0x8
80003a78:	00e7e7b3          	or	a5,a5,a4
80003a7c:	10000713          	li	a4,256
80003a80:	8dc18913          	addi	s2,gp,-1828 # 80000424 <lwip_stats>
80003a84:	02e79a63          	bne	a5,a4,80003ab8 <etharp_input+0x84>
80003a88:	0044c703          	lbu	a4,4(s1)
80003a8c:	00600793          	li	a5,6
80003a90:	02f71463          	bne	a4,a5,80003ab8 <etharp_input+0x84>
      (hdr->hwlen != ETH_HWADDR_LEN) ||
80003a94:	0054c703          	lbu	a4,5(s1)
80003a98:	00400793          	li	a5,4
80003a9c:	00f71e63          	bne	a4,a5,80003ab8 <etharp_input+0x84>
      (hdr->proto != PP_HTONS(ETHTYPE_IP)))  {
80003aa0:	0034c783          	lbu	a5,3(s1)
80003aa4:	0024c703          	lbu	a4,2(s1)
80003aa8:	00879793          	slli	a5,a5,0x8
      (hdr->protolen != sizeof(ip4_addr_t)) ||
80003aac:	00e7e7b3          	or	a5,a5,a4
80003ab0:	00800713          	li	a4,8
80003ab4:	04e78863          	beq	a5,a4,80003b04 <etharp_input+0xd0>
    ETHARP_STATS_INC(etharp.proterr);
80003ab8:	01095783          	lhu	a5,16(s2)
80003abc:	00178793          	addi	a5,a5,1
80003ac0:	00f91823          	sh	a5,16(s2)
    ETHARP_STATS_INC(etharp.drop);
80003ac4:	00695783          	lhu	a5,6(s2)
80003ac8:	00178793          	addi	a5,a5,1
80003acc:	00f91323          	sh	a5,6(s2)
  pbuf_free(p);
80003ad0:	00098513          	mv	a0,s3
80003ad4:	e7dfd0ef          	jal	ra,80001950 <pbuf_free>
}
80003ad8:	03c12083          	lw	ra,60(sp)
80003adc:	03812403          	lw	s0,56(sp)
80003ae0:	03412483          	lw	s1,52(sp)
80003ae4:	03012903          	lw	s2,48(sp)
80003ae8:	02c12983          	lw	s3,44(sp)
80003aec:	02812a03          	lw	s4,40(sp)
80003af0:	02412a83          	lw	s5,36(sp)
80003af4:	02012b03          	lw	s6,32(sp)
80003af8:	01c12b83          	lw	s7,28(sp)
80003afc:	04010113          	addi	sp,sp,64
80003b00:	00008067          	ret
  ETHARP_STATS_INC(etharp.recv);
80003b04:	00295783          	lhu	a5,2(s2)
  IPADDR_WORDALIGNED_COPY_TO_IP4_ADDR_T(&sipaddr, &hdr->sipaddr);
80003b08:	00400613          	li	a2,4
80003b0c:	00e48593          	addi	a1,s1,14
  ETHARP_STATS_INC(etharp.recv);
80003b10:	00178793          	addi	a5,a5,1
  IPADDR_WORDALIGNED_COPY_TO_IP4_ADDR_T(&sipaddr, &hdr->sipaddr);
80003b14:	00810513          	addi	a0,sp,8
  ETHARP_STATS_INC(etharp.recv);
80003b18:	00f91123          	sh	a5,2(s2)
  IPADDR_WORDALIGNED_COPY_TO_IP4_ADDR_T(&sipaddr, &hdr->sipaddr);
80003b1c:	6a0020ef          	jal	ra,800061bc <memcpy>
  IPADDR_WORDALIGNED_COPY_TO_IP4_ADDR_T(&dipaddr, &hdr->dipaddr);
80003b20:	00400613          	li	a2,4
80003b24:	01848593          	addi	a1,s1,24
80003b28:	00c10513          	addi	a0,sp,12
80003b2c:	690020ef          	jal	ra,800061bc <memcpy>
  if (ip4_addr_isany_val(*netif_ip4_addr(netif))) {
80003b30:	00442783          	lw	a5,4(s0)
80003b34:	00848a13          	addi	s4,s1,8
80003b38:	00078663          	beqz	a5,80003b44 <etharp_input+0x110>
  etharp_update_arp_entry(netif, &sipaddr, &(hdr->shwaddr),
80003b3c:	00c12703          	lw	a4,12(sp)
80003b40:	0ce78e63          	beq	a5,a4,80003c1c <etharp_input+0x1e8>
    for_us = 0;
80003b44:	00000a93          	li	s5,0
  etharp_update_arp_entry(netif, &sipaddr, &(hdr->shwaddr),
80003b48:	00200b13          	li	s6,2
  if (ip4_addr_isany(ipaddr) ||
80003b4c:	00812503          	lw	a0,8(sp)
80003b50:	08050e63          	beqz	a0,80003bec <etharp_input+0x1b8>
      ip4_addr_isbroadcast(ipaddr, netif) ||
80003b54:	00040593          	mv	a1,s0
80003b58:	7dd010ef          	jal	ra,80005b34 <ip4_addr_isbroadcast_u32>
  if (ip4_addr_isany(ipaddr) ||
80003b5c:	08051863          	bnez	a0,80003bec <etharp_input+0x1b8>
      ip4_addr_ismulticast(ipaddr)) {
80003b60:	00812783          	lw	a5,8(sp)
      ip4_addr_isbroadcast(ipaddr, netif) ||
80003b64:	0e000713          	li	a4,224
      ip4_addr_ismulticast(ipaddr)) {
80003b68:	0f07f793          	andi	a5,a5,240
      ip4_addr_isbroadcast(ipaddr, netif) ||
80003b6c:	08e78063          	beq	a5,a4,80003bec <etharp_input+0x1b8>
  i = etharp_find_entry(ipaddr, flags, netif);
80003b70:	00040613          	mv	a2,s0
80003b74:	000b0593          	mv	a1,s6
80003b78:	00810513          	addi	a0,sp,8
80003b7c:	aa1ff0ef          	jal	ra,8000361c <etharp_find_entry>
  if (i < 0) {
80003b80:	06054663          	bltz	a0,80003bec <etharp_input+0x1b8>
    arp_table[i].state = ETHARP_STATE_STABLE;
80003b84:	00151793          	slli	a5,a0,0x1
80003b88:	00a787b3          	add	a5,a5,a0
80003b8c:	95418713          	addi	a4,gp,-1708 # 8000049c <arp_table>
80003b90:	00379793          	slli	a5,a5,0x3
80003b94:	00f70b33          	add	s6,a4,a5
80003b98:	00200693          	li	a3,2
  SMEMCPY(&arp_table[i].ethaddr, ethaddr, ETH_HWADDR_LEN);
80003b9c:	00c78793          	addi	a5,a5,12
    arp_table[i].state = ETHARP_STATE_STABLE;
80003ba0:	00db0a23          	sb	a3,20(s6)
  arp_table[i].netif = netif;
80003ba4:	008b2423          	sw	s0,8(s6)
  SMEMCPY(&arp_table[i].ethaddr, ethaddr, ETH_HWADDR_LEN);
80003ba8:	00600613          	li	a2,6
80003bac:	000a0593          	mv	a1,s4
80003bb0:	00f70533          	add	a0,a4,a5
80003bb4:	608020ef          	jal	ra,800061bc <memcpy>
  if (arp_table[i].q != NULL) {
80003bb8:	000b2b83          	lw	s7,0(s6)
  arp_table[i].ctime = 0;
80003bbc:	000b1923          	sh	zero,18(s6)
  if (arp_table[i].q != NULL) {
80003bc0:	020b8663          	beqz	s7,80003bec <etharp_input+0x1b8>
    ethernet_output(netif, p, (struct eth_addr *)(netif->hwaddr), ethaddr, ETHTYPE_IP);
80003bc4:	00001737          	lui	a4,0x1
    arp_table[i].q = NULL;
80003bc8:	000b2023          	sw	zero,0(s6)
    ethernet_output(netif, p, (struct eth_addr *)(netif->hwaddr), ethaddr, ETHTYPE_IP);
80003bcc:	80070713          	addi	a4,a4,-2048 # 800 <_stack_size>
80003bd0:	000a0693          	mv	a3,s4
80003bd4:	02a40613          	addi	a2,s0,42
80003bd8:	000b8593          	mv	a1,s7
80003bdc:	00040513          	mv	a0,s0
80003be0:	c15fc0ef          	jal	ra,800007f4 <ethernet_output>
    pbuf_free(p);
80003be4:	000b8513          	mv	a0,s7
80003be8:	d69fd0ef          	jal	ra,80001950 <pbuf_free>
  switch (hdr->opcode) {
80003bec:	0074c783          	lbu	a5,7(s1)
80003bf0:	0064c703          	lbu	a4,6(s1)
80003bf4:	00879793          	slli	a5,a5,0x8
80003bf8:	00e7e7b3          	or	a5,a5,a4
80003bfc:	10000713          	li	a4,256
80003c00:	02e78463          	beq	a5,a4,80003c28 <etharp_input+0x1f4>
80003c04:	20000713          	li	a4,512
80003c08:	ece784e3          	beq	a5,a4,80003ad0 <etharp_input+0x9c>
      ETHARP_STATS_INC(etharp.err);
80003c0c:	01495783          	lhu	a5,20(s2)
80003c10:	00178793          	addi	a5,a5,1
80003c14:	00f91a23          	sh	a5,20(s2)
      break;
80003c18:	eb9ff06f          	j	80003ad0 <etharp_input+0x9c>
    for_us = (u8_t)ip4_addr_cmp(&dipaddr, netif_ip4_addr(netif));
80003c1c:	00100a93          	li	s5,1
  etharp_update_arp_entry(netif, &sipaddr, &(hdr->shwaddr),
80003c20:	00100b13          	li	s6,1
80003c24:	f29ff06f          	j	80003b4c <etharp_input+0x118>
      if (for_us) {
80003c28:	ea0a84e3          	beqz	s5,80003ad0 <etharp_input+0x9c>
                   (struct eth_addr *)netif->hwaddr, &hdr->shwaddr,
80003c2c:	02a40693          	addi	a3,s0,42
        etharp_raw(netif,
80003c30:	00200893          	li	a7,2
80003c34:	00810813          	addi	a6,sp,8
80003c38:	000a0793          	mv	a5,s4
80003c3c:	00440713          	addi	a4,s0,4
80003c40:	000a0613          	mv	a2,s4
80003c44:	00068593          	mv	a1,a3
80003c48:	00040513          	mv	a0,s0
80003c4c:	b39ff0ef          	jal	ra,80003784 <etharp_raw>
80003c50:	e81ff06f          	j	80003ad0 <etharp_input+0x9c>
80003c54:	00008067          	ret

80003c58 <etharp_request>:
 *         any other err_t on failure
 */
static err_t
etharp_request_dst(struct netif *netif, const ip4_addr_t *ipaddr, const struct eth_addr *hw_dst_addr)
{
  return etharp_raw(netif, (struct eth_addr *)netif->hwaddr, hw_dst_addr,
80003c58:	02a50693          	addi	a3,a0,42
80003c5c:	800007b7          	lui	a5,0x80000
80003c60:	00058813          	mv	a6,a1
80003c64:	00100893          	li	a7,1
80003c68:	35078793          	addi	a5,a5,848 # 80000350 <_stack_start+0xffff95c0>
80003c6c:	00450713          	addi	a4,a0,4
80003c70:	81018613          	addi	a2,gp,-2032 # 80000358 <ethbroadcast>
80003c74:	00068593          	mv	a1,a3
80003c78:	b0dff06f          	j	80003784 <etharp_raw>

80003c7c <etharp_tmr>:
{
80003c7c:	fd010113          	addi	sp,sp,-48
80003c80:	02812423          	sw	s0,40(sp)
80003c84:	02912223          	sw	s1,36(sp)
80003c88:	03212023          	sw	s2,32(sp)
80003c8c:	01312e23          	sw	s3,28(sp)
80003c90:	01412c23          	sw	s4,24(sp)
80003c94:	01512a23          	sw	s5,20(sp)
80003c98:	01612823          	sw	s6,16(sp)
80003c9c:	01712623          	sw	s7,12(sp)
80003ca0:	02112623          	sw	ra,44(sp)
80003ca4:	95418413          	addi	s0,gp,-1708 # 8000049c <arp_table>
  for (i = 0; i < ARP_TABLE_SIZE; ++i) {
80003ca8:	00000493          	li	s1,0
      if ((arp_table[i].ctime >= ARP_MAXAGE) ||
80003cac:	12b00a13          	li	s4,299
80003cb0:	00100a93          	li	s5,1
      } else if (arp_table[i].state == ETHARP_STATE_STABLE_REREQUESTING_1) {
80003cb4:	00300b13          	li	s6,3
      } else if (arp_table[i].state == ETHARP_STATE_STABLE_REREQUESTING_2) {
80003cb8:	00400913          	li	s2,4
        arp_table[i].state = ETHARP_STATE_STABLE;
80003cbc:	00200b93          	li	s7,2
  for (i = 0; i < ARP_TABLE_SIZE; ++i) {
80003cc0:	00a00993          	li	s3,10
    u8_t state = arp_table[i].state;
80003cc4:	01444703          	lbu	a4,20(s0)
    if (state != ETHARP_STATE_EMPTY
80003cc8:	02070663          	beqz	a4,80003cf4 <etharp_tmr+0x78>
      arp_table[i].ctime++;
80003ccc:	01245783          	lhu	a5,18(s0)
80003cd0:	00178793          	addi	a5,a5,1
80003cd4:	01079793          	slli	a5,a5,0x10
80003cd8:	0107d793          	srli	a5,a5,0x10
80003cdc:	00f41923          	sh	a5,18(s0)
      if ((arp_table[i].ctime >= ARP_MAXAGE) ||
80003ce0:	00fa6663          	bltu	s4,a5,80003cec <etharp_tmr+0x70>
80003ce4:	05571463          	bne	a4,s5,80003d2c <etharp_tmr+0xb0>
          ((arp_table[i].state == ETHARP_STATE_PENDING)  &&
80003ce8:	04f97e63          	bgeu	s2,a5,80003d44 <etharp_tmr+0xc8>
        etharp_free_entry(i);
80003cec:	00048513          	mv	a0,s1
80003cf0:	89dff0ef          	jal	ra,8000358c <etharp_free_entry>
  for (i = 0; i < ARP_TABLE_SIZE; ++i) {
80003cf4:	00148493          	addi	s1,s1,1
80003cf8:	01840413          	addi	s0,s0,24
80003cfc:	fd3494e3          	bne	s1,s3,80003cc4 <etharp_tmr+0x48>
}
80003d00:	02c12083          	lw	ra,44(sp)
80003d04:	02812403          	lw	s0,40(sp)
80003d08:	02412483          	lw	s1,36(sp)
80003d0c:	02012903          	lw	s2,32(sp)
80003d10:	01c12983          	lw	s3,28(sp)
80003d14:	01812a03          	lw	s4,24(sp)
80003d18:	01412a83          	lw	s5,20(sp)
80003d1c:	01012b03          	lw	s6,16(sp)
80003d20:	00c12b83          	lw	s7,12(sp)
80003d24:	03010113          	addi	sp,sp,48
80003d28:	00008067          	ret
      } else if (arp_table[i].state == ETHARP_STATE_STABLE_REREQUESTING_1) {
80003d2c:	01671663          	bne	a4,s6,80003d38 <etharp_tmr+0xbc>
        arp_table[i].state = ETHARP_STATE_STABLE_REREQUESTING_2;
80003d30:	01240a23          	sb	s2,20(s0)
80003d34:	fc1ff06f          	j	80003cf4 <etharp_tmr+0x78>
      } else if (arp_table[i].state == ETHARP_STATE_STABLE_REREQUESTING_2) {
80003d38:	fb271ee3          	bne	a4,s2,80003cf4 <etharp_tmr+0x78>
        arp_table[i].state = ETHARP_STATE_STABLE;
80003d3c:	01740a23          	sb	s7,20(s0)
80003d40:	fb5ff06f          	j	80003cf4 <etharp_tmr+0x78>
        etharp_request(arp_table[i].netif, &arp_table[i].ipaddr);
80003d44:	00842503          	lw	a0,8(s0)
80003d48:	00440593          	addi	a1,s0,4
80003d4c:	f0dff0ef          	jal	ra,80003c58 <etharp_request>
80003d50:	fa5ff06f          	j	80003cf4 <etharp_tmr+0x78>

80003d54 <etharp_output_to_arp_index>:
{
80003d54:	fe010113          	addi	sp,sp,-32
80003d58:	01412423          	sw	s4,8(sp)
80003d5c:	00058a13          	mv	s4,a1
  if (arp_table[arp_idx].state == ETHARP_STATE_STABLE) {
80003d60:	00161593          	slli	a1,a2,0x1
80003d64:	00c58633          	add	a2,a1,a2
80003d68:	00361613          	slli	a2,a2,0x3
80003d6c:	95418593          	addi	a1,gp,-1708 # 8000049c <arp_table>
{
80003d70:	00812c23          	sw	s0,24(sp)
80003d74:	00912a23          	sw	s1,20(sp)
80003d78:	01212823          	sw	s2,16(sp)
80003d7c:	01312623          	sw	s3,12(sp)
80003d80:	00112e23          	sw	ra,28(sp)
  LWIP_ASSERT("arp_table[arp_idx].state >= ETHARP_STATE_STABLE",
80003d84:	00c584b3          	add	s1,a1,a2
  if (arp_table[arp_idx].state == ETHARP_STATE_STABLE) {
80003d88:	0144c703          	lbu	a4,20(s1)
80003d8c:	00c60413          	addi	s0,a2,12
80003d90:	00200793          	li	a5,2
{
80003d94:	00050913          	mv	s2,a0
80003d98:	00858433          	add	s0,a1,s0
80003d9c:	02a50993          	addi	s3,a0,42
  if (arp_table[arp_idx].state == ETHARP_STATE_STABLE) {
80003da0:	02f71463          	bne	a4,a5,80003dc8 <etharp_output_to_arp_index+0x74>
    if (arp_table[arp_idx].ctime >= ARP_AGE_REREQUEST_USED_BROADCAST) {
80003da4:	0124d783          	lhu	a5,18(s1)
80003da8:	11c00713          	li	a4,284
80003dac:	04f77a63          	bgeu	a4,a5,80003e00 <etharp_output_to_arp_index+0xac>
      if (etharp_request(netif, &arp_table[arp_idx].ipaddr) == ERR_OK) {
80003db0:	00460613          	addi	a2,a2,4
80003db4:	00c585b3          	add	a1,a1,a2
80003db8:	ea1ff0ef          	jal	ra,80003c58 <etharp_request>
      if (etharp_request_dst(netif, &arp_table[arp_idx].ipaddr, &arp_table[arp_idx].ethaddr) == ERR_OK) {
80003dbc:	00051663          	bnez	a0,80003dc8 <etharp_output_to_arp_index+0x74>
        arp_table[arp_idx].state = ETHARP_STATE_STABLE_REREQUESTING_1;
80003dc0:	00300793          	li	a5,3
80003dc4:	00f48a23          	sb	a5,20(s1)
  return ethernet_output(netif, q, (struct eth_addr *)(netif->hwaddr), &arp_table[arp_idx].ethaddr, ETHTYPE_IP);
80003dc8:	00040693          	mv	a3,s0
}
80003dcc:	01812403          	lw	s0,24(sp)
80003dd0:	01c12083          	lw	ra,28(sp)
80003dd4:	01412483          	lw	s1,20(sp)
  return ethernet_output(netif, q, (struct eth_addr *)(netif->hwaddr), &arp_table[arp_idx].ethaddr, ETHTYPE_IP);
80003dd8:	00098613          	mv	a2,s3
80003ddc:	000a0593          	mv	a1,s4
}
80003de0:	00c12983          	lw	s3,12(sp)
80003de4:	00812a03          	lw	s4,8(sp)
  return ethernet_output(netif, q, (struct eth_addr *)(netif->hwaddr), &arp_table[arp_idx].ethaddr, ETHTYPE_IP);
80003de8:	00090513          	mv	a0,s2
}
80003dec:	01012903          	lw	s2,16(sp)
  return ethernet_output(netif, q, (struct eth_addr *)(netif->hwaddr), &arp_table[arp_idx].ethaddr, ETHTYPE_IP);
80003df0:	00001737          	lui	a4,0x1
80003df4:	80070713          	addi	a4,a4,-2048 # 800 <_stack_size>
}
80003df8:	02010113          	addi	sp,sp,32
  return ethernet_output(netif, q, (struct eth_addr *)(netif->hwaddr), &arp_table[arp_idx].ethaddr, ETHTYPE_IP);
80003dfc:	9f9fc06f          	j	800007f4 <ethernet_output>
    } else if (arp_table[arp_idx].ctime >= ARP_AGE_REREQUEST_USED_UNICAST) {
80003e00:	10d00713          	li	a4,269
80003e04:	fcf772e3          	bgeu	a4,a5,80003dc8 <etharp_output_to_arp_index+0x74>
      if (etharp_request_dst(netif, &arp_table[arp_idx].ipaddr, &arp_table[arp_idx].ethaddr) == ERR_OK) {
80003e08:	00460613          	addi	a2,a2,4
  return etharp_raw(netif, (struct eth_addr *)netif->hwaddr, hw_dst_addr,
80003e0c:	800007b7          	lui	a5,0x80000
80003e10:	00c58833          	add	a6,a1,a2
80003e14:	00100893          	li	a7,1
80003e18:	35078793          	addi	a5,a5,848 # 80000350 <_stack_start+0xffff95c0>
80003e1c:	00450713          	addi	a4,a0,4
80003e20:	00098693          	mv	a3,s3
80003e24:	00040613          	mv	a2,s0
80003e28:	00098593          	mv	a1,s3
80003e2c:	959ff0ef          	jal	ra,80003784 <etharp_raw>
80003e30:	f8dff06f          	j	80003dbc <etharp_output_to_arp_index+0x68>

80003e34 <etharp_query>:
{
80003e34:	fe010113          	addi	sp,sp,-32
80003e38:	01512223          	sw	s5,4(sp)
80003e3c:	00058a93          	mv	s5,a1
80003e40:	00912a23          	sw	s1,20(sp)
  if (ip4_addr_isbroadcast(ipaddr, netif) ||
80003e44:	00050593          	mv	a1,a0
{
80003e48:	00050493          	mv	s1,a0
  if (ip4_addr_isbroadcast(ipaddr, netif) ||
80003e4c:	000aa503          	lw	a0,0(s5)
{
80003e50:	01312623          	sw	s3,12(sp)
80003e54:	00112e23          	sw	ra,28(sp)
80003e58:	00812c23          	sw	s0,24(sp)
80003e5c:	01212823          	sw	s2,16(sp)
80003e60:	01412423          	sw	s4,8(sp)
80003e64:	00060993          	mv	s3,a2
  if (ip4_addr_isbroadcast(ipaddr, netif) ||
80003e68:	4cd010ef          	jal	ra,80005b34 <ip4_addr_isbroadcast_u32>
80003e6c:	16051a63          	bnez	a0,80003fe0 <etharp_query+0x1ac>
      ip4_addr_ismulticast(ipaddr) ||
80003e70:	000aa783          	lw	a5,0(s5)
  if (ip4_addr_isbroadcast(ipaddr, netif) ||
80003e74:	0e000713          	li	a4,224
      ip4_addr_ismulticast(ipaddr) ||
80003e78:	0f07f693          	andi	a3,a5,240
  if (ip4_addr_isbroadcast(ipaddr, netif) ||
80003e7c:	16e68263          	beq	a3,a4,80003fe0 <etharp_query+0x1ac>
      ip4_addr_isany(ipaddr)) {
80003e80:	16078063          	beqz	a5,80003fe0 <etharp_query+0x1ac>
  i_err = etharp_find_entry(ipaddr, ETHARP_FLAG_TRY_HARD, netif);
80003e84:	00048613          	mv	a2,s1
80003e88:	00100593          	li	a1,1
80003e8c:	000a8513          	mv	a0,s5
80003e90:	f8cff0ef          	jal	ra,8000361c <etharp_find_entry>
  if (i_err < 0) {
80003e94:	02055e63          	bgez	a0,80003ed0 <etharp_query+0x9c>
    if (q) {
80003e98:	00098a63          	beqz	s3,80003eac <etharp_query+0x78>
      ETHARP_STATS_INC(etharp.memerr);
80003e9c:	8dc18793          	addi	a5,gp,-1828 # 80000424 <lwip_stats>
80003ea0:	00c7d703          	lhu	a4,12(a5)
80003ea4:	00170713          	addi	a4,a4,1
80003ea8:	00e79623          	sh	a4,12(a5)
}
80003eac:	01c12083          	lw	ra,28(sp)
80003eb0:	01812403          	lw	s0,24(sp)
80003eb4:	01412483          	lw	s1,20(sp)
80003eb8:	01012903          	lw	s2,16(sp)
80003ebc:	00c12983          	lw	s3,12(sp)
80003ec0:	00812a03          	lw	s4,8(sp)
80003ec4:	00412a83          	lw	s5,4(sp)
80003ec8:	02010113          	addi	sp,sp,32
80003ecc:	00008067          	ret
  i = (netif_addr_idx_t)i_err;
80003ed0:	0ff57413          	andi	s0,a0,255
  if (arp_table[i].state == ETHARP_STATE_EMPTY) {
80003ed4:	00141a13          	slli	s4,s0,0x1
80003ed8:	008a0733          	add	a4,s4,s0
80003edc:	95418793          	addi	a5,gp,-1708 # 8000049c <arp_table>
80003ee0:	00371713          	slli	a4,a4,0x3
80003ee4:	00e787b3          	add	a5,a5,a4
80003ee8:	0147c703          	lbu	a4,20(a5)
80003eec:	95418913          	addi	s2,gp,-1708 # 8000049c <arp_table>
80003ef0:	08070663          	beqz	a4,80003f7c <etharp_query+0x148>
  err_t result = ERR_MEM;
80003ef4:	fff00513          	li	a0,-1
  if (is_new_entry || (q == NULL)) {
80003ef8:	00099a63          	bnez	s3,80003f0c <etharp_query+0xd8>
    result = etharp_request(netif, ipaddr);
80003efc:	000a8593          	mv	a1,s5
80003f00:	00048513          	mv	a0,s1
80003f04:	d55ff0ef          	jal	ra,80003c58 <etharp_request>
    if (q == NULL) {
80003f08:	fa0982e3          	beqz	s3,80003eac <etharp_query+0x78>
  if (arp_table[i].state >= ETHARP_STATE_STABLE) {
80003f0c:	008a07b3          	add	a5,s4,s0
80003f10:	00379793          	slli	a5,a5,0x3
80003f14:	00f907b3          	add	a5,s2,a5
80003f18:	0147c703          	lbu	a4,20(a5)
80003f1c:	00100793          	li	a5,1
80003f20:	06e7f663          	bgeu	a5,a4,80003f8c <etharp_query+0x158>
    ETHARP_SET_ADDRHINT(netif, i);
80003f24:	00048863          	beqz	s1,80003f34 <etharp_query+0x100>
80003f28:	0384a783          	lw	a5,56(s1)
80003f2c:	00078463          	beqz	a5,80003f34 <etharp_query+0x100>
80003f30:	00878023          	sb	s0,0(a5)
    result = ethernet_output(netif, q, srcaddr, &(arp_table[i].ethaddr), ETHTYPE_IP);
80003f34:	008a06b3          	add	a3,s4,s0
80003f38:	00369693          	slli	a3,a3,0x3
}
80003f3c:	01812403          	lw	s0,24(sp)
    result = ethernet_output(netif, q, srcaddr, &(arp_table[i].ethaddr), ETHTYPE_IP);
80003f40:	00c68693          	addi	a3,a3,12
}
80003f44:	01c12083          	lw	ra,28(sp)
80003f48:	00812a03          	lw	s4,8(sp)
80003f4c:	00412a83          	lw	s5,4(sp)
    result = ethernet_output(netif, q, srcaddr, &(arp_table[i].ethaddr), ETHTYPE_IP);
80003f50:	00d906b3          	add	a3,s2,a3
80003f54:	02a48613          	addi	a2,s1,42
}
80003f58:	01012903          	lw	s2,16(sp)
    result = ethernet_output(netif, q, srcaddr, &(arp_table[i].ethaddr), ETHTYPE_IP);
80003f5c:	00098593          	mv	a1,s3
80003f60:	00048513          	mv	a0,s1
}
80003f64:	00c12983          	lw	s3,12(sp)
80003f68:	01412483          	lw	s1,20(sp)
    result = ethernet_output(netif, q, srcaddr, &(arp_table[i].ethaddr), ETHTYPE_IP);
80003f6c:	00001737          	lui	a4,0x1
80003f70:	80070713          	addi	a4,a4,-2048 # 800 <_stack_size>
}
80003f74:	02010113          	addi	sp,sp,32
    result = ethernet_output(netif, q, srcaddr, &(arp_table[i].ethaddr), ETHTYPE_IP);
80003f78:	87dfc06f          	j	800007f4 <ethernet_output>
    arp_table[i].state = ETHARP_STATE_PENDING;
80003f7c:	00100713          	li	a4,1
80003f80:	00e78a23          	sb	a4,20(a5)
    arp_table[i].netif = netif;
80003f84:	0097a423          	sw	s1,8(a5)
  if (is_new_entry || (q == NULL)) {
80003f88:	f75ff06f          	j	80003efc <etharp_query+0xc8>
  } else if (arp_table[i].state == ETHARP_STATE_PENDING) {
80003f8c:	f2f710e3          	bne	a4,a5,80003eac <etharp_query+0x78>
80003f90:	00098793          	mv	a5,s3
      if (PBUF_NEEDS_COPY(p)) {
80003f94:	00c7c703          	lbu	a4,12(a5)
80003f98:	04077713          	andi	a4,a4,64
80003f9c:	04071663          	bnez	a4,80003fe8 <etharp_query+0x1b4>
      p = p->next;
80003fa0:	0007a783          	lw	a5,0(a5)
    while (p) {
80003fa4:	fe0798e3          	bnez	a5,80003f94 <etharp_query+0x160>
      pbuf_ref(p);
80003fa8:	00098513          	mv	a0,s3
80003fac:	d79fd0ef          	jal	ra,80001d24 <pbuf_ref>
      if (arp_table[i].q != NULL) {
80003fb0:	008a07b3          	add	a5,s4,s0
80003fb4:	00379793          	slli	a5,a5,0x3
80003fb8:	00f907b3          	add	a5,s2,a5
80003fbc:	0007a503          	lw	a0,0(a5)
80003fc0:	00050463          	beqz	a0,80003fc8 <etharp_query+0x194>
        pbuf_free(arp_table[i].q);
80003fc4:	98dfd0ef          	jal	ra,80001950 <pbuf_free>
      arp_table[i].q = p;
80003fc8:	008a0433          	add	s0,s4,s0
80003fcc:	00341413          	slli	s0,s0,0x3
80003fd0:	00890433          	add	s0,s2,s0
80003fd4:	01342023          	sw	s3,0(s0)
      result = ERR_OK;
80003fd8:	00000513          	li	a0,0
80003fdc:	ed1ff06f          	j	80003eac <etharp_query+0x78>
    return ERR_ARG;
80003fe0:	ff000513          	li	a0,-16
80003fe4:	ec9ff06f          	j	80003eac <etharp_query+0x78>
      p = pbuf_clone(PBUF_LINK, PBUF_RAM, q);
80003fe8:	00098613          	mv	a2,s3
80003fec:	28000593          	li	a1,640
80003ff0:	01000513          	li	a0,16
80003ff4:	9e0fe0ef          	jal	ra,800021d4 <pbuf_clone>
80003ff8:	00050993          	mv	s3,a0
    if (p != NULL) {
80003ffc:	fa051ae3          	bnez	a0,80003fb0 <etharp_query+0x17c>
      ETHARP_STATS_INC(etharp.memerr);
80004000:	8dc18793          	addi	a5,gp,-1828 # 80000424 <lwip_stats>
80004004:	00c7d703          	lhu	a4,12(a5)
      result = ERR_MEM;
80004008:	fff00513          	li	a0,-1
      ETHARP_STATS_INC(etharp.memerr);
8000400c:	00170713          	addi	a4,a4,1
80004010:	00e79623          	sh	a4,12(a5)
      result = ERR_MEM;
80004014:	e99ff06f          	j	80003eac <etharp_query+0x78>

80004018 <etharp_output>:
{
80004018:	fe010113          	addi	sp,sp,-32
8000401c:	00812c23          	sw	s0,24(sp)
80004020:	00912a23          	sw	s1,20(sp)
80004024:	01212823          	sw	s2,16(sp)
80004028:	00112e23          	sw	ra,28(sp)
8000402c:	00050413          	mv	s0,a0
80004030:	00058913          	mv	s2,a1
  if (ip4_addr_isbroadcast(ipaddr, netif)) {
80004034:	00050593          	mv	a1,a0
80004038:	00062503          	lw	a0,0(a2)
{
8000403c:	00060493          	mv	s1,a2
  if (ip4_addr_isbroadcast(ipaddr, netif)) {
80004040:	2f5010ef          	jal	ra,80005b34 <ip4_addr_isbroadcast_u32>
80004044:	16051863          	bnez	a0,800041b4 <etharp_output+0x19c>
  } else if (ip4_addr_ismulticast(ipaddr)) {
80004048:	0004a783          	lw	a5,0(s1)
8000404c:	0e000713          	li	a4,224
80004050:	0f07f613          	andi	a2,a5,240
80004054:	04e61863          	bne	a2,a4,800040a4 <etharp_output+0x8c>
    mcastaddr.addr[0] = LL_IP4_MULTICAST_ADDR_0;
80004058:	00100793          	li	a5,1
8000405c:	00f11423          	sh	a5,8(sp)
    mcastaddr.addr[2] = LL_IP4_MULTICAST_ADDR_2;
80004060:	05e00793          	li	a5,94
80004064:	00f10523          	sb	a5,10(sp)
    mcastaddr.addr[3] = ip4_addr2(ipaddr) & 0x7f;
80004068:	0014c783          	lbu	a5,1(s1)
    dest = &mcastaddr;
8000406c:	00810693          	addi	a3,sp,8
    mcastaddr.addr[3] = ip4_addr2(ipaddr) & 0x7f;
80004070:	07f7f793          	andi	a5,a5,127
80004074:	00f105a3          	sb	a5,11(sp)
    mcastaddr.addr[4] = ip4_addr3(ipaddr);
80004078:	0024c783          	lbu	a5,2(s1)
8000407c:	00f10623          	sb	a5,12(sp)
    mcastaddr.addr[5] = ip4_addr4(ipaddr);
80004080:	0034c783          	lbu	a5,3(s1)
80004084:	00f106a3          	sb	a5,13(sp)
  return ethernet_output(netif, q, (struct eth_addr *)(netif->hwaddr), dest, ETHTYPE_IP);
80004088:	00001737          	lui	a4,0x1
8000408c:	80070713          	addi	a4,a4,-2048 # 800 <_stack_size>
80004090:	02a40613          	addi	a2,s0,42
80004094:	00090593          	mv	a1,s2
80004098:	00040513          	mv	a0,s0
8000409c:	f58fc0ef          	jal	ra,800007f4 <ethernet_output>
800040a0:	0a40006f          	j	80004144 <etharp_output+0x12c>
    if (!ip4_addr_netcmp(ipaddr, netif_ip4_addr(netif), netif_ip4_netmask(netif)) &&
800040a4:	00442703          	lw	a4,4(s0)
800040a8:	00842603          	lw	a2,8(s0)
800040ac:	00050693          	mv	a3,a0
800040b0:	00e7c733          	xor	a4,a5,a4
800040b4:	00c77733          	and	a4,a4,a2
800040b8:	02070463          	beqz	a4,800040e0 <etharp_output+0xc8>
        !ip4_addr_islinklocal(ipaddr)) {
800040bc:	01079793          	slli	a5,a5,0x10
    if (!ip4_addr_netcmp(ipaddr, netif_ip4_addr(netif), netif_ip4_netmask(netif)) &&
800040c0:	00010737          	lui	a4,0x10
        !ip4_addr_islinklocal(ipaddr)) {
800040c4:	0107d793          	srli	a5,a5,0x10
    if (!ip4_addr_netcmp(ipaddr, netif_ip4_addr(netif), netif_ip4_netmask(netif)) &&
800040c8:	ea970713          	addi	a4,a4,-343 # fea9 <_stack_size+0xf6a9>
800040cc:	00e78a63          	beq	a5,a4,800040e0 <etharp_output+0xc8>
          if (!ip4_addr_isany_val(*netif_ip4_gw(netif))) {
800040d0:	00c42783          	lw	a5,12(s0)
            return ERR_RTE;
800040d4:	ffc00513          	li	a0,-4
          if (!ip4_addr_isany_val(*netif_ip4_gw(netif))) {
800040d8:	06078663          	beqz	a5,80004144 <etharp_output+0x12c>
            dst_addr = netif_ip4_gw(netif);
800040dc:	00c40493          	addi	s1,s0,12
    if (netif->hints != NULL) {
800040e0:	03842583          	lw	a1,56(s0)
800040e4:	95418793          	addi	a5,gp,-1708 # 8000049c <arp_table>
800040e8:	06058a63          	beqz	a1,8000415c <etharp_output+0x144>
      netif_addr_idx_t etharp_cached_entry = netif->hints->addr_hint;
800040ec:	0005c603          	lbu	a2,0(a1)
      if (etharp_cached_entry < ARP_TABLE_SIZE) {
800040f0:	00900713          	li	a4,9
800040f4:	06c76463          	bltu	a4,a2,8000415c <etharp_output+0x144>
        if ((arp_table[etharp_cached_entry].state >= ETHARP_STATE_STABLE) &&
800040f8:	00161713          	slli	a4,a2,0x1
800040fc:	00c70733          	add	a4,a4,a2
80004100:	00371713          	slli	a4,a4,0x3
80004104:	00e78733          	add	a4,a5,a4
80004108:	01474803          	lbu	a6,20(a4)
8000410c:	00100513          	li	a0,1
80004110:	05057663          	bgeu	a0,a6,8000415c <etharp_output+0x144>
80004114:	00872503          	lw	a0,8(a4)
80004118:	04851263          	bne	a0,s0,8000415c <etharp_output+0x144>
            (arp_table[etharp_cached_entry].netif == netif) &&
8000411c:	0004a503          	lw	a0,0(s1)
80004120:	00472703          	lw	a4,4(a4)
80004124:	02e51c63          	bne	a0,a4,8000415c <etharp_output+0x144>
          ETHARP_STATS_INC(etharp.cachehit);
80004128:	8dc18793          	addi	a5,gp,-1828 # 80000424 <lwip_stats>
8000412c:	0167d703          	lhu	a4,22(a5)
80004130:	00170713          	addi	a4,a4,1
80004134:	00e79b23          	sh	a4,22(a5)
        return etharp_output_to_arp_index(netif, q, i);
80004138:	00090593          	mv	a1,s2
8000413c:	00040513          	mv	a0,s0
80004140:	c15ff0ef          	jal	ra,80003d54 <etharp_output_to_arp_index>
}
80004144:	01c12083          	lw	ra,28(sp)
80004148:	01812403          	lw	s0,24(sp)
8000414c:	01412483          	lw	s1,20(sp)
80004150:	01012903          	lw	s2,16(sp)
80004154:	02010113          	addi	sp,sp,32
80004158:	00008067          	ret
      if ((arp_table[i].state >= ETHARP_STATE_STABLE) &&
8000415c:	00100613          	li	a2,1
    for (i = 0; i < ARP_TABLE_SIZE; i++) {
80004160:	00a00713          	li	a4,10
      if ((arp_table[i].state >= ETHARP_STATE_STABLE) &&
80004164:	0147c503          	lbu	a0,20(a5)
80004168:	02a67463          	bgeu	a2,a0,80004190 <etharp_output+0x178>
8000416c:	0087a503          	lw	a0,8(a5)
80004170:	02851063          	bne	a0,s0,80004190 <etharp_output+0x178>
          (arp_table[i].netif == netif) &&
80004174:	0004a803          	lw	a6,0(s1)
80004178:	0047a503          	lw	a0,4(a5)
8000417c:	00a81a63          	bne	a6,a0,80004190 <etharp_output+0x178>
        ETHARP_SET_ADDRHINT(netif, i);
80004180:	00058463          	beqz	a1,80004188 <etharp_output+0x170>
80004184:	00d58023          	sb	a3,0(a1)
        return etharp_output_to_arp_index(netif, q, i);
80004188:	00068613          	mv	a2,a3
8000418c:	fadff06f          	j	80004138 <etharp_output+0x120>
    for (i = 0; i < ARP_TABLE_SIZE; i++) {
80004190:	00168693          	addi	a3,a3,1
80004194:	0ff6f693          	andi	a3,a3,255
80004198:	01878793          	addi	a5,a5,24
8000419c:	fce694e3          	bne	a3,a4,80004164 <etharp_output+0x14c>
    return etharp_query(netif, dst_addr, q);
800041a0:	00090613          	mv	a2,s2
800041a4:	00048593          	mv	a1,s1
800041a8:	00040513          	mv	a0,s0
800041ac:	c89ff0ef          	jal	ra,80003e34 <etharp_query>
800041b0:	f95ff06f          	j	80004144 <etharp_output+0x12c>
    dest = (const struct eth_addr *)&ethbroadcast;
800041b4:	81018693          	addi	a3,gp,-2032 # 80000358 <ethbroadcast>
800041b8:	ed1ff06f          	j	80004088 <etharp_output+0x70>

800041bc <icmp_send_response.isra.0>:
 *          p->payload pointing to the IP header
 * @param type Type of the ICMP header
 * @param code Code of the ICMP header
 */
static void
icmp_send_response(struct pbuf *p, u8_t type, u8_t code)
800041bc:	fc010113          	addi	sp,sp,-64
800041c0:	03312623          	sw	s3,44(sp)
800041c4:	03412423          	sw	s4,40(sp)
800041c8:	03512223          	sw	s5,36(sp)
800041cc:	00050993          	mv	s3,a0
800041d0:	00058a93          	mv	s5,a1
800041d4:	00060a13          	mv	s4,a2

  /* increase number of messages attempted to send */
  MIB2_STATS_INC(mib2.icmpoutmsgs);

  /* ICMP header + IP header + 8 bytes of data */
  q = pbuf_alloc(PBUF_IP, sizeof(struct icmp_echo_hdr) + IP_HLEN + ICMP_DEST_UNREACH_DATASIZE,
800041d8:	02400593          	li	a1,36
800041dc:	28000613          	li	a2,640
800041e0:	02400513          	li	a0,36
icmp_send_response(struct pbuf *p, u8_t type, u8_t code)
800041e4:	02112e23          	sw	ra,60(sp)
800041e8:	02812c23          	sw	s0,56(sp)
800041ec:	02912a23          	sw	s1,52(sp)
800041f0:	03212823          	sw	s2,48(sp)
  q = pbuf_alloc(PBUF_IP, sizeof(struct icmp_echo_hdr) + IP_HLEN + ICMP_DEST_UNREACH_DATASIZE,
800041f4:	831fd0ef          	jal	ra,80001a24 <pbuf_alloc>
                 PBUF_RAM);
  if (q == NULL) {
800041f8:	0c050863          	beqz	a0,800042c8 <icmp_send_response.isra.0+0x10c>
  ip4_addr_debug_print_val(ICMP_DEBUG, iphdr->src);
  LWIP_DEBUGF(ICMP_DEBUG, (" to "));
  ip4_addr_debug_print_val(ICMP_DEBUG, iphdr->dest);
  LWIP_DEBUGF(ICMP_DEBUG, ("\n"));

  icmphdr = (struct icmp_echo_hdr *)q->payload;
800041fc:	00452403          	lw	s0,4(a0)
  iphdr = (struct ip_hdr *)p->payload;
80004200:	0009a903          	lw	s2,0(s3)
80004204:	00050493          	mv	s1,a0
  icmphdr->type = type;
80004208:	01540023          	sb	s5,0(s0)
  icmphdr->code = code;
8000420c:	014400a3          	sb	s4,1(s0)
  icmphdr->id = 0;
80004210:	00040223          	sb	zero,4(s0)
80004214:	000402a3          	sb	zero,5(s0)
  icmphdr->seqno = 0;
80004218:	00040323          	sb	zero,6(s0)
8000421c:	000403a3          	sb	zero,7(s0)

  /* copy fields from original packet */
  SMEMCPY((u8_t *)q->payload + sizeof(struct icmp_echo_hdr), (u8_t *)p->payload,
80004220:	00452503          	lw	a0,4(a0)
80004224:	0009a583          	lw	a1,0(s3)
80004228:	01c00613          	li	a2,28
8000422c:	00850513          	addi	a0,a0,8
80004230:	78d010ef          	jal	ra,800061bc <memcpy>
          IP_HLEN + ICMP_DEST_UNREACH_DATASIZE);

  ip4_addr_copy(iphdr_src, iphdr->src);
80004234:	00d94703          	lbu	a4,13(s2)
80004238:	00c94783          	lbu	a5,12(s2)
    ip4_addr_t iphdr_dst;
    ip4_addr_copy(iphdr_dst, iphdr->dest);
    netif = ip4_route_src(&iphdr_dst, &iphdr_src);
  }
#else
  netif = ip4_route(&iphdr_src);
8000423c:	01c10513          	addi	a0,sp,28
  ip4_addr_copy(iphdr_src, iphdr->src);
80004240:	00871713          	slli	a4,a4,0x8
80004244:	00f76733          	or	a4,a4,a5
80004248:	00e94783          	lbu	a5,14(s2)
8000424c:	01079793          	slli	a5,a5,0x10
80004250:	00e7e733          	or	a4,a5,a4
80004254:	00f94783          	lbu	a5,15(s2)
80004258:	01879793          	slli	a5,a5,0x18
8000425c:	00e7e7b3          	or	a5,a5,a4
80004260:	00f12e23          	sw	a5,28(sp)
  netif = ip4_route(&iphdr_src);
80004264:	14c010ef          	jal	ra,800053b0 <ip4_route>
#endif
  if (netif != NULL) {
80004268:	00a12623          	sw	a0,12(sp)
8000426c:	04050a63          	beqz	a0,800042c0 <icmp_send_response.isra.0+0x104>
    /* calculate checksum */
    icmphdr->chksum = 0;
80004270:	00040123          	sb	zero,2(s0)
80004274:	000401a3          	sb	zero,3(s0)
#if CHECKSUM_GEN_ICMP
    IF__NETIF_CHECKSUM_ENABLED(netif, NETIF_CHECKSUM_GEN_ICMP) {
      icmphdr->chksum = inet_chksum(icmphdr, q->len);
80004278:	00a4d583          	lhu	a1,10(s1)
8000427c:	00040513          	mv	a0,s0
80004280:	c5dfc0ef          	jal	ra,80000edc <inet_chksum>
80004284:	00a40123          	sb	a0,2(s0)
80004288:	00855513          	srli	a0,a0,0x8
8000428c:	00a401a3          	sb	a0,3(s0)
    }
#endif
    ICMP_STATS_INC(icmp.xmit);
80004290:	8dc18793          	addi	a5,gp,-1828 # 80000424 <lwip_stats>
80004294:	0487d703          	lhu	a4,72(a5)
    ip4_output_if(q, NULL, &iphdr_src, ICMP_TTL, 0, IP_PROTO_ICMP, netif);
80004298:	00c12803          	lw	a6,12(sp)
8000429c:	0ff00693          	li	a3,255
    ICMP_STATS_INC(icmp.xmit);
800042a0:	00170713          	addi	a4,a4,1
800042a4:	04e79423          	sh	a4,72(a5)
    ip4_output_if(q, NULL, &iphdr_src, ICMP_TTL, 0, IP_PROTO_ICMP, netif);
800042a8:	01c10613          	addi	a2,sp,28
800042ac:	00100793          	li	a5,1
800042b0:	00000713          	li	a4,0
800042b4:	00000593          	li	a1,0
800042b8:	00048513          	mv	a0,s1
800042bc:	74c010ef          	jal	ra,80005a08 <ip4_output_if>
  }
  pbuf_free(q);
800042c0:	00048513          	mv	a0,s1
800042c4:	e8cfd0ef          	jal	ra,80001950 <pbuf_free>
}
800042c8:	03c12083          	lw	ra,60(sp)
800042cc:	03812403          	lw	s0,56(sp)
800042d0:	03412483          	lw	s1,52(sp)
800042d4:	03012903          	lw	s2,48(sp)
800042d8:	02c12983          	lw	s3,44(sp)
800042dc:	02812a03          	lw	s4,40(sp)
800042e0:	02412a83          	lw	s5,36(sp)
800042e4:	04010113          	addi	sp,sp,64
800042e8:	00008067          	ret

800042ec <icmp_input>:
{
800042ec:	fd010113          	addi	sp,sp,-48
800042f0:	02912223          	sw	s1,36(sp)
800042f4:	02812423          	sw	s0,40(sp)
800042f8:	01712623          	sw	s7,12(sp)
800042fc:	02112623          	sw	ra,44(sp)
80004300:	03212023          	sw	s2,32(sp)
80004304:	01312e23          	sw	s3,28(sp)
80004308:	01412c23          	sw	s4,24(sp)
8000430c:	01512a23          	sw	s5,20(sp)
80004310:	01612823          	sw	s6,16(sp)
80004314:	01812423          	sw	s8,8(sp)
  ICMP_STATS_INC(icmp.recv);
80004318:	8dc18793          	addi	a5,gp,-1828 # 80000424 <lwip_stats>
8000431c:	04a7d703          	lhu	a4,74(a5)
{
80004320:	00050413          	mv	s0,a0
  ICMP_STATS_INC(icmp.recv);
80004324:	00170713          	addi	a4,a4,1
80004328:	04e79523          	sh	a4,74(a5)
  iphdr_in = ip4_current_header();
8000432c:	8c418713          	addi	a4,gp,-1852 # 8000040c <ip_data>
80004330:	00872c03          	lw	s8,8(a4)
  if (hlen < IP_HLEN) {
80004334:	01300713          	li	a4,19
80004338:	8dc18493          	addi	s1,gp,-1828 # 80000424 <lwip_stats>
  hlen = IPH_HL_BYTES(iphdr_in);
8000433c:	000c4a83          	lbu	s5,0(s8)
80004340:	00fafa93          	andi	s5,s5,15
80004344:	002a9a93          	slli	s5,s5,0x2
  if (hlen < IP_HLEN) {
80004348:	2b577863          	bgeu	a4,s5,800045f8 <icmp_input+0x30c>
  if (p->len < sizeof(u16_t) * 2) {
8000434c:	00a55683          	lhu	a3,10(a0)
80004350:	00300713          	li	a4,3
80004354:	2ad77263          	bgeu	a4,a3,800045f8 <icmp_input+0x30c>
  type = *((u8_t *)p->payload);
80004358:	00452703          	lw	a4,4(a0)
8000435c:	00074703          	lbu	a4,0(a4)
  switch (type) {
80004360:	22070663          	beqz	a4,8000458c <icmp_input+0x2a0>
80004364:	00800693          	li	a3,8
80004368:	26d71a63          	bne	a4,a3,800045dc <icmp_input+0x2f0>
8000436c:	8c418b93          	addi	s7,gp,-1852 # 8000040c <ip_data>
      if (ip4_addr_ismulticast(ip4_current_dest_addr())) {
80004370:	014ba503          	lw	a0,20(s7)
80004374:	0e000793          	li	a5,224
80004378:	00058993          	mv	s3,a1
8000437c:	0f057713          	andi	a4,a0,240
        src = netif_ip4_addr(inp);
80004380:	00458913          	addi	s2,a1,4
      if (ip4_addr_ismulticast(ip4_current_dest_addr())) {
80004384:	00f70463          	beq	a4,a5,8000438c <icmp_input+0xa0>
      src = ip4_current_dest_addr();
80004388:	8d818913          	addi	s2,gp,-1832 # 80000420 <ip_data+0x14>
      if (ip4_addr_isbroadcast(ip4_current_dest_addr(), ip_current_netif())) {
8000438c:	000ba583          	lw	a1,0(s7)
80004390:	7a4010ef          	jal	ra,80005b34 <ip4_addr_isbroadcast_u32>
80004394:	00050463          	beqz	a0,8000439c <icmp_input+0xb0>
        src = netif_ip4_addr(inp);
80004398:	00498913          	addi	s2,s3,4
      if (p->tot_len < sizeof(struct icmp_echo_hdr)) {
8000439c:	00845703          	lhu	a4,8(s0)
800043a0:	00700793          	li	a5,7
800043a4:	24e7fa63          	bgeu	a5,a4,800045f8 <icmp_input+0x30c>
        if (inet_chksum_pbuf(p) != 0) {
800043a8:	00040513          	mv	a0,s0
800043ac:	b55fc0ef          	jal	ra,80000f00 <inet_chksum_pbuf>
800043b0:	04050463          	beqz	a0,800043f8 <icmp_input+0x10c>
          pbuf_free(p);
800043b4:	00040513          	mv	a0,s0
800043b8:	d98fd0ef          	jal	ra,80001950 <pbuf_free>
          ICMP_STATS_INC(icmp.chkerr);
800043bc:	0504d783          	lhu	a5,80(s1)
800043c0:	00178793          	addi	a5,a5,1
800043c4:	04f49823          	sh	a5,80(s1)
}
800043c8:	02c12083          	lw	ra,44(sp)
800043cc:	02812403          	lw	s0,40(sp)
800043d0:	02412483          	lw	s1,36(sp)
800043d4:	02012903          	lw	s2,32(sp)
800043d8:	01c12983          	lw	s3,28(sp)
800043dc:	01812a03          	lw	s4,24(sp)
800043e0:	01412a83          	lw	s5,20(sp)
800043e4:	01012b03          	lw	s6,16(sp)
800043e8:	00c12b83          	lw	s7,12(sp)
800043ec:	00812c03          	lw	s8,8(sp)
800043f0:	03010113          	addi	sp,sp,48
800043f4:	00008067          	ret
      if (pbuf_add_header(p, hlen + PBUF_LINK_HLEN + PBUF_LINK_ENCAPSULATION_HLEN)) {
800043f8:	010a8b13          	addi	s6,s5,16
  hlen = IPH_HL_BYTES(iphdr_in);
800043fc:	010a9a13          	slli	s4,s5,0x10
      if (pbuf_add_header(p, hlen + PBUF_LINK_HLEN + PBUF_LINK_ENCAPSULATION_HLEN)) {
80004400:	000b0593          	mv	a1,s6
80004404:	00040513          	mv	a0,s0
  hlen = IPH_HL_BYTES(iphdr_in);
80004408:	010a5a13          	srli	s4,s4,0x10
      if (pbuf_add_header(p, hlen + PBUF_LINK_HLEN + PBUF_LINK_ENCAPSULATION_HLEN)) {
8000440c:	cacfd0ef          	jal	ra,800018b8 <pbuf_add_header>
80004410:	1a050863          	beqz	a0,800045c0 <icmp_input+0x2d4>
        u16_t alloc_len = (u16_t)(p->tot_len + hlen);
80004414:	00845783          	lhu	a5,8(s0)
80004418:	014785b3          	add	a1,a5,s4
8000441c:	01059593          	slli	a1,a1,0x10
80004420:	0105d593          	srli	a1,a1,0x10
        if (alloc_len < p->tot_len) {
80004424:	02f5e463          	bltu	a1,a5,8000444c <icmp_input+0x160>
        r = pbuf_alloc(PBUF_LINK, alloc_len, PBUF_RAM);
80004428:	28000613          	li	a2,640
8000442c:	01000513          	li	a0,16
80004430:	df4fd0ef          	jal	ra,80001a24 <pbuf_alloc>
80004434:	00050b13          	mv	s6,a0
        if (r == NULL) {
80004438:	00050a63          	beqz	a0,8000444c <icmp_input+0x160>
        if (r->len < hlen + sizeof(struct icmp_echo_hdr)) {
8000443c:	00a55703          	lhu	a4,10(a0)
80004440:	008a8793          	addi	a5,s5,8
80004444:	02f77063          	bgeu	a4,a5,80004464 <icmp_input+0x178>
          pbuf_free(r);
80004448:	d08fd0ef          	jal	ra,80001950 <pbuf_free>
  pbuf_free(p);
8000444c:	00040513          	mv	a0,s0
80004450:	d00fd0ef          	jal	ra,80001950 <pbuf_free>
  ICMP_STATS_INC(icmp.err);
80004454:	05c4d783          	lhu	a5,92(s1)
80004458:	00178793          	addi	a5,a5,1
8000445c:	04f49e23          	sh	a5,92(s1)
  return;
80004460:	f69ff06f          	j	800043c8 <icmp_input+0xdc>
        MEMCPY(r->payload, iphdr_in, hlen);
80004464:	00452503          	lw	a0,4(a0)
80004468:	000c0593          	mv	a1,s8
8000446c:	000a8613          	mv	a2,s5
80004470:	54d010ef          	jal	ra,800061bc <memcpy>
        if (pbuf_remove_header(r, hlen)) {
80004474:	000a8593          	mv	a1,s5
80004478:	000b0513          	mv	a0,s6
8000447c:	c4cfd0ef          	jal	ra,800018c8 <pbuf_remove_header>
80004480:	00050663          	beqz	a0,8000448c <icmp_input+0x1a0>
          pbuf_free(r);
80004484:	000b0513          	mv	a0,s6
80004488:	fc1ff06f          	j	80004448 <icmp_input+0x15c>
        if (pbuf_copy(r, p) != ERR_OK) {
8000448c:	00040593          	mv	a1,s0
80004490:	000b0513          	mv	a0,s6
80004494:	97dfd0ef          	jal	ra,80001e10 <pbuf_copy>
80004498:	fe0516e3          	bnez	a0,80004484 <icmp_input+0x198>
        pbuf_free(p);
8000449c:	00040513          	mv	a0,s0
800044a0:	cb0fd0ef          	jal	ra,80001950 <pbuf_free>
        p = r;
800044a4:	000b0413          	mv	s0,s6
      if (pbuf_add_header(p, hlen)) {
800044a8:	000a8593          	mv	a1,s5
800044ac:	00040513          	mv	a0,s0
      iecho = (struct icmp_echo_hdr *)p->payload;
800044b0:	00442b03          	lw	s6,4(s0)
      if (pbuf_add_header(p, hlen)) {
800044b4:	c04fd0ef          	jal	ra,800018b8 <pbuf_add_header>
800044b8:	0c051a63          	bnez	a0,8000458c <icmp_input+0x2a0>
        ip4_addr_copy(iphdr->src, *src);
800044bc:	00092783          	lw	a5,0(s2)
        struct ip_hdr *iphdr = (struct ip_hdr *)p->payload;
800044c0:	00442a83          	lw	s5,4(s0)
        ip4_addr_copy(iphdr->src, *src);
800044c4:	0087d713          	srli	a4,a5,0x8
800044c8:	00fa8623          	sb	a5,12(s5)
800044cc:	00ea86a3          	sb	a4,13(s5)
800044d0:	0107d713          	srli	a4,a5,0x10
800044d4:	0187d793          	srli	a5,a5,0x18
800044d8:	00ea8723          	sb	a4,14(s5)
800044dc:	00fa87a3          	sb	a5,15(s5)
        ip4_addr_copy(iphdr->dest, *ip4_current_src_addr());
800044e0:	010ba783          	lw	a5,16(s7)
800044e4:	0087d713          	srli	a4,a5,0x8
800044e8:	00fa8823          	sb	a5,16(s5)
800044ec:	00ea88a3          	sb	a4,17(s5)
800044f0:	0107d713          	srli	a4,a5,0x10
800044f4:	0187d793          	srli	a5,a5,0x18
800044f8:	00ea8923          	sb	a4,18(s5)
800044fc:	00fa89a3          	sb	a5,19(s5)
          if (iecho->chksum > PP_HTONS(0xffffU - (ICMP_ECHO << 8))) {
80004500:	003b4783          	lbu	a5,3(s6)
80004504:	002b4703          	lbu	a4,2(s6)
        ICMPH_TYPE_SET(iecho, ICMP_ER);
80004508:	000b0023          	sb	zero,0(s6)
          if (iecho->chksum > PP_HTONS(0xffffU - (ICMP_ECHO << 8))) {
8000450c:	00879793          	slli	a5,a5,0x8
80004510:	00e7e7b3          	or	a5,a5,a4
80004514:	00010737          	lui	a4,0x10
80004518:	ff770713          	addi	a4,a4,-9 # fff7 <_stack_size+0xf7f7>
8000451c:	0af77c63          	bgeu	a4,a5,800045d4 <icmp_input+0x2e8>
            iecho->chksum = (u16_t)(iecho->chksum + PP_HTONS((u16_t)(ICMP_ECHO << 8)) + 1);
80004520:	00978793          	addi	a5,a5,9
            iecho->chksum = (u16_t)(iecho->chksum + PP_HTONS(ICMP_ECHO << 8));
80004524:	01079793          	slli	a5,a5,0x10
80004528:	0107d793          	srli	a5,a5,0x10
8000452c:	00fb0123          	sb	a5,2(s6)
80004530:	0087d793          	srli	a5,a5,0x8
80004534:	00fb01a3          	sb	a5,3(s6)
        IPH_TTL_SET(iphdr, ICMP_TTL);
80004538:	fff00793          	li	a5,-1
8000453c:	00fa8423          	sb	a5,8(s5)
          IPH_CHKSUM_SET(iphdr, inet_chksum(iphdr, hlen));
80004540:	000a0593          	mv	a1,s4
        IPH_CHKSUM_SET(iphdr, 0);
80004544:	000a8523          	sb	zero,10(s5)
80004548:	000a85a3          	sb	zero,11(s5)
          IPH_CHKSUM_SET(iphdr, inet_chksum(iphdr, hlen));
8000454c:	000a8513          	mv	a0,s5
80004550:	98dfc0ef          	jal	ra,80000edc <inet_chksum>
80004554:	00aa8523          	sb	a0,10(s5)
80004558:	00855513          	srli	a0,a0,0x8
8000455c:	00aa85a3          	sb	a0,11(s5)
        ICMP_STATS_INC(icmp.xmit);
80004560:	0484d783          	lhu	a5,72(s1)
        ret = ip4_output_if(p, src, LWIP_IP_HDRINCL,
80004564:	00098813          	mv	a6,s3
80004568:	00000713          	li	a4,0
        ICMP_STATS_INC(icmp.xmit);
8000456c:	00178793          	addi	a5,a5,1
80004570:	04f49423          	sh	a5,72(s1)
        ret = ip4_output_if(p, src, LWIP_IP_HDRINCL,
80004574:	0ff00693          	li	a3,255
80004578:	00100793          	li	a5,1
8000457c:	00000613          	li	a2,0
80004580:	00090593          	mv	a1,s2
80004584:	00040513          	mv	a0,s0
80004588:	480010ef          	jal	ra,80005a08 <ip4_output_if>
  pbuf_free(p);
8000458c:	00040513          	mv	a0,s0
}
80004590:	02812403          	lw	s0,40(sp)
80004594:	02c12083          	lw	ra,44(sp)
80004598:	02412483          	lw	s1,36(sp)
8000459c:	02012903          	lw	s2,32(sp)
800045a0:	01c12983          	lw	s3,28(sp)
800045a4:	01812a03          	lw	s4,24(sp)
800045a8:	01412a83          	lw	s5,20(sp)
800045ac:	01012b03          	lw	s6,16(sp)
800045b0:	00c12b83          	lw	s7,12(sp)
800045b4:	00812c03          	lw	s8,8(sp)
800045b8:	03010113          	addi	sp,sp,48
  pbuf_free(p);
800045bc:	b94fd06f          	j	80001950 <pbuf_free>
        if (pbuf_remove_header(p, hlen + PBUF_LINK_HLEN + PBUF_LINK_ENCAPSULATION_HLEN)) {
800045c0:	000b0593          	mv	a1,s6
800045c4:	00040513          	mv	a0,s0
800045c8:	b00fd0ef          	jal	ra,800018c8 <pbuf_remove_header>
800045cc:	ec050ee3          	beqz	a0,800044a8 <icmp_input+0x1bc>
800045d0:	e7dff06f          	j	8000444c <icmp_input+0x160>
            iecho->chksum = (u16_t)(iecho->chksum + PP_HTONS(ICMP_ECHO << 8));
800045d4:	00878793          	addi	a5,a5,8
800045d8:	f4dff06f          	j	80004524 <icmp_input+0x238>
      ICMP_STATS_INC(icmp.proterr);
800045dc:	0587d703          	lhu	a4,88(a5)
800045e0:	00170713          	addi	a4,a4,1
800045e4:	04e79c23          	sh	a4,88(a5)
      ICMP_STATS_INC(icmp.drop);
800045e8:	04e7d703          	lhu	a4,78(a5)
800045ec:	00170713          	addi	a4,a4,1
800045f0:	04e79723          	sh	a4,78(a5)
800045f4:	f99ff06f          	j	8000458c <icmp_input+0x2a0>
  pbuf_free(p);
800045f8:	00040513          	mv	a0,s0
800045fc:	b54fd0ef          	jal	ra,80001950 <pbuf_free>
  ICMP_STATS_INC(icmp.lenerr);
80004600:	0524d783          	lhu	a5,82(s1)
80004604:	00178793          	addi	a5,a5,1
80004608:	04f49923          	sh	a5,82(s1)
  return;
8000460c:	dbdff06f          	j	800043c8 <icmp_input+0xdc>

80004610 <icmp_dest_unreach>:
  icmp_send_response(p, ICMP_DUR, t);
80004610:	0ff5f613          	andi	a2,a1,255
80004614:	00450513          	addi	a0,a0,4
80004618:	00300593          	li	a1,3
8000461c:	ba1ff06f          	j	800041bc <icmp_send_response.isra.0>

80004620 <icmp_time_exceeded>:
  icmp_send_response(p, ICMP_TE, t);
80004620:	0ff5f613          	andi	a2,a1,255
80004624:	00450513          	addi	a0,a0,4
80004628:	00b00593          	li	a1,11
8000462c:	b91ff06f          	j	800041bc <icmp_send_response.isra.0>

80004630 <ip_reass_dequeue_datagram.isra.0>:
 */
static void
ip_reass_dequeue_datagram(struct ip_reassdata *ipr, struct ip_reassdata *prev)
{
  /* dequeue the reass struct  */
  if (reassdatagrams == ipr) {
80004630:	8801a683          	lw	a3,-1920(gp) # 800003c8 <reassdatagrams>
80004634:	00052783          	lw	a5,0(a0)
80004638:	00a69a63          	bne	a3,a0,8000464c <ip_reass_dequeue_datagram.isra.0+0x1c>
    /* it was the first in the list */
    reassdatagrams = ipr->next;
8000463c:	88f1a023          	sw	a5,-1920(gp) # 800003c8 <reassdatagrams>
    LWIP_ASSERT("sanity check linked list", prev != NULL);
    prev->next = ipr->next;
  }

  /* now we can free the ip_reassdata struct */
  memp_free(MEMP_REASSDATA, ipr);
80004640:	00050593          	mv	a1,a0
80004644:	00200513          	li	a0,2
80004648:	a71fc06f          	j	800010b8 <memp_free>
    prev->next = ipr->next;
8000464c:	00f5a023          	sw	a5,0(a1)
80004650:	ff1ff06f          	j	80004640 <ip_reass_dequeue_datagram.isra.0+0x10>

80004654 <ip_reass_free_complete_datagram>:
{
80004654:	fe010113          	addi	sp,sp,-32
80004658:	00812c23          	sw	s0,24(sp)
8000465c:	00912a23          	sw	s1,20(sp)
80004660:	01412423          	sw	s4,8(sp)
80004664:	00112e23          	sw	ra,28(sp)
80004668:	01212823          	sw	s2,16(sp)
8000466c:	01312623          	sw	s3,12(sp)
  iprh = (struct ip_reass_helper *)ipr->p->payload;
80004670:	00452903          	lw	s2,4(a0)
{
80004674:	00050493          	mv	s1,a0
80004678:	00058a13          	mv	s4,a1
  iprh = (struct ip_reass_helper *)ipr->p->payload;
8000467c:	00492783          	lw	a5,4(s2)
  if (iprh->start == 0) {
80004680:	00000413          	li	s0,0
80004684:	0057c703          	lbu	a4,5(a5)
80004688:	0047c683          	lbu	a3,4(a5)
8000468c:	00871713          	slli	a4,a4,0x8
80004690:	00d76733          	or	a4,a4,a3
80004694:	06071063          	bnez	a4,800046f4 <ip_reass_free_complete_datagram+0xa0>
    ipr->p = iprh->next_pbuf;
80004698:	0017c703          	lbu	a4,1(a5)
8000469c:	0007c683          	lbu	a3,0(a5)
    SMEMCPY(p->payload, &ipr->iphdr, IP_HLEN);
800046a0:	00850593          	addi	a1,a0,8
    ipr->p = iprh->next_pbuf;
800046a4:	00871713          	slli	a4,a4,0x8
800046a8:	00d766b3          	or	a3,a4,a3
800046ac:	0027c703          	lbu	a4,2(a5)
800046b0:	0037c783          	lbu	a5,3(a5)
    SMEMCPY(p->payload, &ipr->iphdr, IP_HLEN);
800046b4:	01400613          	li	a2,20
    ipr->p = iprh->next_pbuf;
800046b8:	01071713          	slli	a4,a4,0x10
800046bc:	00d76733          	or	a4,a4,a3
800046c0:	01879793          	slli	a5,a5,0x18
800046c4:	00e7e7b3          	or	a5,a5,a4
800046c8:	00f52223          	sw	a5,4(a0)
    SMEMCPY(p->payload, &ipr->iphdr, IP_HLEN);
800046cc:	00492503          	lw	a0,4(s2)
800046d0:	2ed010ef          	jal	ra,800061bc <memcpy>
    icmp_time_exceeded(p, ICMP_TE_FRAG);
800046d4:	00100593          	li	a1,1
800046d8:	00090513          	mv	a0,s2
800046dc:	f45ff0ef          	jal	ra,80004620 <icmp_time_exceeded>
    clen = pbuf_clen(p);
800046e0:	00090513          	mv	a0,s2
800046e4:	e1cfd0ef          	jal	ra,80001d00 <pbuf_clen>
800046e8:	00050413          	mv	s0,a0
    pbuf_free(p);
800046ec:	00090513          	mv	a0,s2
800046f0:	a60fd0ef          	jal	ra,80001950 <pbuf_free>
  p = ipr->p;
800046f4:	0044a903          	lw	s2,4(s1)
  while (p != NULL) {
800046f8:	04091063          	bnez	s2,80004738 <ip_reass_free_complete_datagram+0xe4>
  ip_reass_dequeue_datagram(ipr, prev);
800046fc:	00048513          	mv	a0,s1
80004700:	000a0593          	mv	a1,s4
80004704:	f2dff0ef          	jal	ra,80004630 <ip_reass_dequeue_datagram.isra.0>
  ip_reass_pbufcount = (u16_t)(ip_reass_pbufcount - pbufs_freed);
80004708:	87c1d783          	lhu	a5,-1924(gp) # 800003c4 <ip_reass_pbufcount>
}
8000470c:	00040513          	mv	a0,s0
80004710:	01c12083          	lw	ra,28(sp)
  ip_reass_pbufcount = (u16_t)(ip_reass_pbufcount - pbufs_freed);
80004714:	408787b3          	sub	a5,a5,s0
}
80004718:	01812403          	lw	s0,24(sp)
  ip_reass_pbufcount = (u16_t)(ip_reass_pbufcount - pbufs_freed);
8000471c:	86f19e23          	sh	a5,-1924(gp) # 800003c4 <ip_reass_pbufcount>
}
80004720:	01412483          	lw	s1,20(sp)
80004724:	01012903          	lw	s2,16(sp)
80004728:	00c12983          	lw	s3,12(sp)
8000472c:	00812a03          	lw	s4,8(sp)
80004730:	02010113          	addi	sp,sp,32
80004734:	00008067          	ret
    iprh = (struct ip_reass_helper *)p->payload;
80004738:	00492703          	lw	a4,4(s2)
    clen = pbuf_clen(pcur);
8000473c:	00090513          	mv	a0,s2
    p = iprh->next_pbuf;
80004740:	00174783          	lbu	a5,1(a4)
80004744:	00074983          	lbu	s3,0(a4)
80004748:	00879793          	slli	a5,a5,0x8
8000474c:	0137e7b3          	or	a5,a5,s3
80004750:	00274983          	lbu	s3,2(a4)
80004754:	01099993          	slli	s3,s3,0x10
80004758:	00f9e7b3          	or	a5,s3,a5
8000475c:	00374983          	lbu	s3,3(a4)
80004760:	01899993          	slli	s3,s3,0x18
80004764:	00f9e9b3          	or	s3,s3,a5
    clen = pbuf_clen(pcur);
80004768:	d98fd0ef          	jal	ra,80001d00 <pbuf_clen>
    pbufs_freed = (u16_t)(pbufs_freed + clen);
8000476c:	00a40433          	add	s0,s0,a0
80004770:	01041413          	slli	s0,s0,0x10
    pbuf_free(pcur);
80004774:	00090513          	mv	a0,s2
    pbufs_freed = (u16_t)(pbufs_freed + clen);
80004778:	01045413          	srli	s0,s0,0x10
    pbuf_free(pcur);
8000477c:	9d4fd0ef          	jal	ra,80001950 <pbuf_free>
    p = iprh->next_pbuf;
80004780:	00098913          	mv	s2,s3
80004784:	f75ff06f          	j	800046f8 <ip_reass_free_complete_datagram+0xa4>

80004788 <ip_reass_remove_oldest_datagram>:
{
80004788:	fe010113          	addi	sp,sp,-32
8000478c:	00812c23          	sw	s0,24(sp)
80004790:	00912a23          	sw	s1,20(sp)
80004794:	01312623          	sw	s3,12(sp)
80004798:	01412423          	sw	s4,8(sp)
8000479c:	01512223          	sw	s5,4(sp)
800047a0:	00112e23          	sw	ra,28(sp)
800047a4:	01212823          	sw	s2,16(sp)
800047a8:	00050413          	mv	s0,a0
800047ac:	00058a13          	mv	s4,a1
  int pbufs_freed = 0, pbufs_freed_current;
800047b0:	00000493          	li	s1,0
  } while ((pbufs_freed < pbufs_needed) && (other_datagrams > 1));
800047b4:	00100993          	li	s3,1
    r = reassdatagrams;
800047b8:	8801a783          	lw	a5,-1920(gp) # 800003c8 <reassdatagrams>
    other_datagrams = 0;
800047bc:	00000913          	li	s2,0
    oldest_prev = NULL;
800047c0:	00000593          	li	a1,0
    prev = NULL;
800047c4:	00000613          	li	a2,0
    oldest = NULL;
800047c8:	00000513          	li	a0,0
    while (r != NULL) {
800047cc:	04079063          	bnez	a5,8000480c <ip_reass_remove_oldest_datagram+0x84>
    if (oldest != NULL) {
800047d0:	00050663          	beqz	a0,800047dc <ip_reass_remove_oldest_datagram+0x54>
      pbufs_freed_current = ip_reass_free_complete_datagram(oldest, oldest_prev);
800047d4:	e81ff0ef          	jal	ra,80004654 <ip_reass_free_complete_datagram>
      pbufs_freed += pbufs_freed_current;
800047d8:	00a484b3          	add	s1,s1,a0
  } while ((pbufs_freed < pbufs_needed) && (other_datagrams > 1));
800047dc:	0144d463          	bge	s1,s4,800047e4 <ip_reass_remove_oldest_datagram+0x5c>
800047e0:	fd29cce3          	blt	s3,s2,800047b8 <ip_reass_remove_oldest_datagram+0x30>
}
800047e4:	01c12083          	lw	ra,28(sp)
800047e8:	01812403          	lw	s0,24(sp)
800047ec:	00048513          	mv	a0,s1
800047f0:	01012903          	lw	s2,16(sp)
800047f4:	01412483          	lw	s1,20(sp)
800047f8:	00c12983          	lw	s3,12(sp)
800047fc:	00812a03          	lw	s4,8(sp)
80004800:	00412a83          	lw	s5,4(sp)
80004804:	02010113          	addi	sp,sp,32
80004808:	00008067          	ret
      if (!IP_ADDRESSES_AND_ID_MATCH(&r->iphdr, fraghdr)) {
8000480c:	00d44683          	lbu	a3,13(s0)
80004810:	00c44703          	lbu	a4,12(s0)
80004814:	00869693          	slli	a3,a3,0x8
80004818:	00e6e6b3          	or	a3,a3,a4
8000481c:	00e44703          	lbu	a4,14(s0)
80004820:	01071713          	slli	a4,a4,0x10
80004824:	00d766b3          	or	a3,a4,a3
80004828:	00f44703          	lbu	a4,15(s0)
8000482c:	01871713          	slli	a4,a4,0x18
80004830:	00d76733          	or	a4,a4,a3
80004834:	0147a683          	lw	a3,20(a5)
80004838:	04e69663          	bne	a3,a4,80004884 <ip_reass_remove_oldest_datagram+0xfc>
8000483c:	01144683          	lbu	a3,17(s0)
80004840:	01044703          	lbu	a4,16(s0)
80004844:	00869693          	slli	a3,a3,0x8
80004848:	00e6e6b3          	or	a3,a3,a4
8000484c:	01244703          	lbu	a4,18(s0)
80004850:	01071713          	slli	a4,a4,0x10
80004854:	00d766b3          	or	a3,a4,a3
80004858:	01344703          	lbu	a4,19(s0)
8000485c:	01871713          	slli	a4,a4,0x18
80004860:	00d76733          	or	a4,a4,a3
80004864:	0187a683          	lw	a3,24(a5)
80004868:	00e69e63          	bne	a3,a4,80004884 <ip_reass_remove_oldest_datagram+0xfc>
8000486c:	00544703          	lbu	a4,5(s0)
80004870:	00444803          	lbu	a6,4(s0)
80004874:	00c7d683          	lhu	a3,12(a5)
80004878:	00871713          	slli	a4,a4,0x8
8000487c:	01076733          	or	a4,a4,a6
80004880:	02e68063          	beq	a3,a4,800048a0 <ip_reass_remove_oldest_datagram+0x118>
        other_datagrams++;
80004884:	00190913          	addi	s2,s2,1
        if (oldest == NULL) {
80004888:	00050863          	beqz	a0,80004898 <ip_reass_remove_oldest_datagram+0x110>
        } else if (r->timer <= oldest->timer) {
8000488c:	01f7c683          	lbu	a3,31(a5)
80004890:	01f54703          	lbu	a4,31(a0)
80004894:	00d76663          	bltu	a4,a3,800048a0 <ip_reass_remove_oldest_datagram+0x118>
          oldest_prev = prev;
80004898:	00060593          	mv	a1,a2
8000489c:	00078513          	mv	a0,a5
      if (r->next != NULL) {
800048a0:	0007a703          	lw	a4,0(a5)
800048a4:	00070463          	beqz	a4,800048ac <ip_reass_remove_oldest_datagram+0x124>
800048a8:	00078613          	mv	a2,a5
800048ac:	00070793          	mv	a5,a4
800048b0:	f1dff06f          	j	800047cc <ip_reass_remove_oldest_datagram+0x44>

800048b4 <ipfrag_free_pbuf_custom>:

/** Free-callback function to free a 'struct pbuf_custom_ref', called by
 * pbuf_free. */
static void
ipfrag_free_pbuf_custom(struct pbuf *p)
{
800048b4:	00050593          	mv	a1,a0
  struct pbuf_custom_ref *pcr = (struct pbuf_custom_ref *)p;
  LWIP_ASSERT("pcr != NULL", pcr != NULL);
  LWIP_ASSERT("pcr == p", (void *)pcr == (void *)p);
  if (pcr->original != NULL) {
800048b8:	01452503          	lw	a0,20(a0)
800048bc:	02050463          	beqz	a0,800048e4 <ipfrag_free_pbuf_custom+0x30>
{
800048c0:	fe010113          	addi	sp,sp,-32
800048c4:	00112e23          	sw	ra,28(sp)
800048c8:	00b12623          	sw	a1,12(sp)
    pbuf_free(pcr->original);
800048cc:	884fd0ef          	jal	ra,80001950 <pbuf_free>
800048d0:	00c12583          	lw	a1,12(sp)
  }
  ip_frag_free_pbuf_custom_ref(pcr);
}
800048d4:	01c12083          	lw	ra,28(sp)
  memp_free(MEMP_FRAG_PBUF, p);
800048d8:	00300513          	li	a0,3
}
800048dc:	02010113          	addi	sp,sp,32
  memp_free(MEMP_FRAG_PBUF, p);
800048e0:	fd8fc06f          	j	800010b8 <memp_free>
800048e4:	00300513          	li	a0,3
800048e8:	ff9ff06f          	j	800048e0 <ipfrag_free_pbuf_custom+0x2c>

800048ec <ip_reass_tmr>:
  r = reassdatagrams;
800048ec:	8801a503          	lw	a0,-1920(gp) # 800003c8 <reassdatagrams>
{
800048f0:	ff010113          	addi	sp,sp,-16
800048f4:	00812423          	sw	s0,8(sp)
800048f8:	00112623          	sw	ra,12(sp)
800048fc:	00912223          	sw	s1,4(sp)
  struct ip_reassdata *r, *prev = NULL;
80004900:	00000413          	li	s0,0
  while (r != NULL) {
80004904:	00051c63          	bnez	a0,8000491c <ip_reass_tmr+0x30>
}
80004908:	00c12083          	lw	ra,12(sp)
8000490c:	00812403          	lw	s0,8(sp)
80004910:	00412483          	lw	s1,4(sp)
80004914:	01010113          	addi	sp,sp,16
80004918:	00008067          	ret
    if (r->timer > 0) {
8000491c:	01f54783          	lbu	a5,31(a0)
80004920:	00052483          	lw	s1,0(a0)
80004924:	00078c63          	beqz	a5,8000493c <ip_reass_tmr+0x50>
      r->timer--;
80004928:	fff78793          	addi	a5,a5,-1
8000492c:	00f50fa3          	sb	a5,31(a0)
      ip_reass_free_complete_datagram(tmp, prev);
80004930:	00050413          	mv	s0,a0
80004934:	00048513          	mv	a0,s1
80004938:	fcdff06f          	j	80004904 <ip_reass_tmr+0x18>
8000493c:	00040593          	mv	a1,s0
80004940:	d15ff0ef          	jal	ra,80004654 <ip_reass_free_complete_datagram>
80004944:	00040513          	mv	a0,s0
80004948:	fe9ff06f          	j	80004930 <ip_reass_tmr+0x44>

8000494c <ip4_reass>:
{
8000494c:	fd010113          	addi	sp,sp,-48
80004950:	01512a23          	sw	s5,20(sp)
80004954:	03212023          	sw	s2,32(sp)
80004958:	02112623          	sw	ra,44(sp)
8000495c:	02812423          	sw	s0,40(sp)
80004960:	02912223          	sw	s1,36(sp)
80004964:	01312e23          	sw	s3,28(sp)
80004968:	01412c23          	sw	s4,24(sp)
8000496c:	01612823          	sw	s6,16(sp)
80004970:	01712623          	sw	s7,12(sp)
80004974:	01812423          	sw	s8,8(sp)
80004978:	01912223          	sw	s9,4(sp)
8000497c:	01a12023          	sw	s10,0(sp)
  IPFRAG_STATS_INC(ip_frag.recv);
80004980:	8dc18713          	addi	a4,gp,-1828 # 80000424 <lwip_stats>
80004984:	01a75783          	lhu	a5,26(a4)
{
80004988:	00050913          	mv	s2,a0
8000498c:	8dc18a93          	addi	s5,gp,-1828 # 80000424 <lwip_stats>
  IPFRAG_STATS_INC(ip_frag.recv);
80004990:	00178793          	addi	a5,a5,1
80004994:	00f71d23          	sh	a5,26(a4)
  fraghdr = (struct ip_hdr *)p->payload;
80004998:	00452483          	lw	s1,4(a0)
  if (IPH_HL_BYTES(fraghdr) != IP_HLEN) {
8000499c:	01400713          	li	a4,20
800049a0:	0004c783          	lbu	a5,0(s1)
800049a4:	00f7f793          	andi	a5,a5,15
800049a8:	00279793          	slli	a5,a5,0x2
800049ac:	02e78663          	beq	a5,a4,800049d8 <ip4_reass+0x8c>
    IPFRAG_STATS_INC(ip_frag.err);
800049b0:	02cad783          	lhu	a5,44(s5)
800049b4:	00178793          	addi	a5,a5,1
800049b8:	02fa9623          	sh	a5,44(s5)
  IPFRAG_STATS_INC(ip_frag.drop);
800049bc:	01ead783          	lhu	a5,30(s5)
  pbuf_free(p);
800049c0:	00090513          	mv	a0,s2
  return NULL;
800049c4:	00000913          	li	s2,0
  IPFRAG_STATS_INC(ip_frag.drop);
800049c8:	00178793          	addi	a5,a5,1
800049cc:	00fa9f23          	sh	a5,30(s5)
  pbuf_free(p);
800049d0:	f81fc0ef          	jal	ra,80001950 <pbuf_free>
  return NULL;
800049d4:	5600006f          	j	80004f34 <ip4_reass+0x5e8>
  offset = IPH_OFFSET_BYTES(fraghdr);
800049d8:	0074c783          	lbu	a5,7(s1)
800049dc:	0064c503          	lbu	a0,6(s1)
800049e0:	00879793          	slli	a5,a5,0x8
800049e4:	00a7e533          	or	a0,a5,a0
800049e8:	f01fb0ef          	jal	ra,800008e8 <lwip_htons>
  len = lwip_ntohs(IPH_LEN(fraghdr));
800049ec:	0034c783          	lbu	a5,3(s1)
  offset = IPH_OFFSET_BYTES(fraghdr);
800049f0:	00050c13          	mv	s8,a0
  len = lwip_ntohs(IPH_LEN(fraghdr));
800049f4:	0024c503          	lbu	a0,2(s1)
800049f8:	00879793          	slli	a5,a5,0x8
800049fc:	00a7e533          	or	a0,a5,a0
80004a00:	ee9fb0ef          	jal	ra,800008e8 <lwip_htons>
  hlen = IPH_HL_BYTES(fraghdr);
80004a04:	0004c783          	lbu	a5,0(s1)
80004a08:	00f7f793          	andi	a5,a5,15
  if (hlen > len) {
80004a0c:	00279793          	slli	a5,a5,0x2
80004a10:	faf566e3          	bltu	a0,a5,800049bc <ip4_reass+0x70>
  len = (u16_t)(len - hlen);
80004a14:	40f50b33          	sub	s6,a0,a5
  clen = pbuf_clen(p);
80004a18:	00090513          	mv	a0,s2
80004a1c:	ae4fd0ef          	jal	ra,80001d00 <pbuf_clen>
  if ((ip_reass_pbufcount + clen) > IP_REASS_MAX_PBUFS) {
80004a20:	87c1d783          	lhu	a5,-1924(gp) # 800003c4 <ip_reass_pbufcount>
  len = (u16_t)(len - hlen);
80004a24:	010b1b13          	slli	s6,s6,0x10
  if ((ip_reass_pbufcount + clen) > IP_REASS_MAX_PBUFS) {
80004a28:	00800413          	li	s0,8
80004a2c:	00a787b3          	add	a5,a5,a0
  len = (u16_t)(len - hlen);
80004a30:	010b5b13          	srli	s6,s6,0x10
  clen = pbuf_clen(p);
80004a34:	00050b93          	mv	s7,a0
  if ((ip_reass_pbufcount + clen) > IP_REASS_MAX_PBUFS) {
80004a38:	02f45863          	bge	s0,a5,80004a68 <ip4_reass+0x11c>
    if (!ip_reass_remove_oldest_datagram(fraghdr, clen) ||
80004a3c:	00050593          	mv	a1,a0
80004a40:	00048513          	mv	a0,s1
80004a44:	d45ff0ef          	jal	ra,80004788 <ip_reass_remove_oldest_datagram>
80004a48:	00050863          	beqz	a0,80004a58 <ip4_reass+0x10c>
        ((ip_reass_pbufcount + clen) > IP_REASS_MAX_PBUFS))
80004a4c:	87c1d783          	lhu	a5,-1924(gp) # 800003c4 <ip_reass_pbufcount>
80004a50:	017787b3          	add	a5,a5,s7
    if (!ip_reass_remove_oldest_datagram(fraghdr, clen) ||
80004a54:	00f45a63          	bge	s0,a5,80004a68 <ip4_reass+0x11c>
      IPFRAG_STATS_INC(ip_frag.memerr);
80004a58:	024ad783          	lhu	a5,36(s5)
80004a5c:	00178793          	addi	a5,a5,1
80004a60:	02fa9223          	sh	a5,36(s5)
    if (ipr == NULL) {
80004a64:	f59ff06f          	j	800049bc <ip4_reass+0x70>
  for (ipr = reassdatagrams; ipr != NULL; ipr = ipr->next) {
80004a68:	8801a403          	lw	s0,-1920(gp) # 800003c8 <reassdatagrams>
80004a6c:	06041663          	bnez	s0,80004ad8 <ip4_reass+0x18c>
  ipr = (struct ip_reassdata *)memp_malloc(MEMP_REASSDATA);
80004a70:	00200513          	li	a0,2
80004a74:	de0fc0ef          	jal	ra,80001054 <memp_malloc>
80004a78:	00050413          	mv	s0,a0
  if (ipr == NULL) {
80004a7c:	02051263          	bnez	a0,80004aa0 <ip4_reass+0x154>
    if (ip_reass_remove_oldest_datagram(fraghdr, clen) >= clen) {
80004a80:	000b8593          	mv	a1,s7
80004a84:	00048513          	mv	a0,s1
80004a88:	d01ff0ef          	jal	ra,80004788 <ip_reass_remove_oldest_datagram>
80004a8c:	fd7546e3          	blt	a0,s7,80004a58 <ip4_reass+0x10c>
      ipr = (struct ip_reassdata *)memp_malloc(MEMP_REASSDATA);
80004a90:	00200513          	li	a0,2
80004a94:	dc0fc0ef          	jal	ra,80001054 <memp_malloc>
80004a98:	00050413          	mv	s0,a0
    if (ipr == NULL)
80004a9c:	fa050ee3          	beqz	a0,80004a58 <ip4_reass+0x10c>
  memset(ipr, 0, sizeof(struct ip_reassdata));
80004aa0:	01c00613          	li	a2,28
80004aa4:	00000593          	li	a1,0
80004aa8:	00440513          	addi	a0,s0,4
80004aac:	504010ef          	jal	ra,80005fb0 <memset>
  ipr->timer = IP_REASS_MAXAGE;
80004ab0:	00f00793          	li	a5,15
80004ab4:	00f40fa3          	sb	a5,31(s0)
  ipr->next = reassdatagrams;
80004ab8:	8801a783          	lw	a5,-1920(gp) # 800003c8 <reassdatagrams>
  reassdatagrams = ipr;
80004abc:	8881a023          	sw	s0,-1920(gp) # 800003c8 <reassdatagrams>
  ipr->next = reassdatagrams;
80004ac0:	00f42023          	sw	a5,0(s0)
      SMEMCPY(&ipr->iphdr, fraghdr, IP_HLEN);
80004ac4:	01400613          	li	a2,20
80004ac8:	00048593          	mv	a1,s1
80004acc:	00840513          	addi	a0,s0,8
80004ad0:	6ec010ef          	jal	ra,800061bc <memcpy>
80004ad4:	0cc0006f          	j	80004ba0 <ip4_reass+0x254>
    if (IP_ADDRESSES_AND_ID_MATCH(&ipr->iphdr, fraghdr)) {
80004ad8:	00d4c703          	lbu	a4,13(s1)
80004adc:	00c4c783          	lbu	a5,12(s1)
80004ae0:	00871713          	slli	a4,a4,0x8
80004ae4:	00f76733          	or	a4,a4,a5
80004ae8:	00e4c783          	lbu	a5,14(s1)
80004aec:	01079793          	slli	a5,a5,0x10
80004af0:	00e7e733          	or	a4,a5,a4
80004af4:	00f4c783          	lbu	a5,15(s1)
80004af8:	01879793          	slli	a5,a5,0x18
80004afc:	00e7e7b3          	or	a5,a5,a4
80004b00:	01442703          	lw	a4,20(s0)
80004b04:	1cf71063          	bne	a4,a5,80004cc4 <ip4_reass+0x378>
80004b08:	0114c703          	lbu	a4,17(s1)
80004b0c:	0104c783          	lbu	a5,16(s1)
80004b10:	00871713          	slli	a4,a4,0x8
80004b14:	00f76733          	or	a4,a4,a5
80004b18:	0124c783          	lbu	a5,18(s1)
80004b1c:	01079793          	slli	a5,a5,0x10
80004b20:	00e7e733          	or	a4,a5,a4
80004b24:	0134c783          	lbu	a5,19(s1)
80004b28:	01879793          	slli	a5,a5,0x18
80004b2c:	00e7e7b3          	or	a5,a5,a4
80004b30:	01842703          	lw	a4,24(s0)
80004b34:	18f71863          	bne	a4,a5,80004cc4 <ip4_reass+0x378>
80004b38:	0054c783          	lbu	a5,5(s1)
80004b3c:	0044c683          	lbu	a3,4(s1)
80004b40:	00c45703          	lhu	a4,12(s0)
80004b44:	00879793          	slli	a5,a5,0x8
80004b48:	00d7e7b3          	or	a5,a5,a3
80004b4c:	16f71c63          	bne	a4,a5,80004cc4 <ip4_reass+0x378>
      IPFRAG_STATS_INC(ip_frag.cachehit);
80004b50:	02ead783          	lhu	a5,46(s5)
    if (((lwip_ntohs(IPH_OFFSET(fraghdr)) & IP_OFFMASK) == 0) &&
80004b54:	00002cb7          	lui	s9,0x2
80004b58:	fffc8c93          	addi	s9,s9,-1 # 1fff <_stack_size+0x17ff>
      IPFRAG_STATS_INC(ip_frag.cachehit);
80004b5c:	00178793          	addi	a5,a5,1
80004b60:	02fa9723          	sh	a5,46(s5)
    if (((lwip_ntohs(IPH_OFFSET(fraghdr)) & IP_OFFMASK) == 0) &&
80004b64:	0074c783          	lbu	a5,7(s1)
80004b68:	0064c503          	lbu	a0,6(s1)
80004b6c:	00879793          	slli	a5,a5,0x8
80004b70:	00a7e533          	or	a0,a5,a0
80004b74:	d75fb0ef          	jal	ra,800008e8 <lwip_htons>
80004b78:	01957533          	and	a0,a0,s9
80004b7c:	01051513          	slli	a0,a0,0x10
80004b80:	01055513          	srli	a0,a0,0x10
80004b84:	00051e63          	bnez	a0,80004ba0 <ip4_reass+0x254>
        ((lwip_ntohs(IPH_OFFSET(&ipr->iphdr)) & IP_OFFMASK) != 0)) {
80004b88:	00e45503          	lhu	a0,14(s0)
80004b8c:	d5dfb0ef          	jal	ra,800008e8 <lwip_htons>
    if (((lwip_ntohs(IPH_OFFSET(fraghdr)) & IP_OFFMASK) == 0) &&
80004b90:	01957533          	and	a0,a0,s9
80004b94:	01051513          	slli	a0,a0,0x10
80004b98:	01055513          	srli	a0,a0,0x10
80004b9c:	f20514e3          	bnez	a0,80004ac4 <ip4_reass+0x178>
  is_last = (IPH_OFFSET(fraghdr) & PP_NTOHS(IP_MF)) == 0;
80004ba0:	0064cc83          	lbu	s9,6(s1)
  offset = IPH_OFFSET_BYTES(fraghdr);
80004ba4:	013c1c13          	slli	s8,s8,0x13
80004ba8:	013c5c13          	srli	s8,s8,0x13
80004bac:	020cfc93          	andi	s9,s9,32
80004bb0:	003c1c13          	slli	s8,s8,0x3
  if (is_last) {
80004bb4:	020c9063          	bnez	s9,80004bd4 <ip4_reass+0x288>
    u16_t datagram_len = (u16_t)(offset + len);
80004bb8:	016c07b3          	add	a5,s8,s6
80004bbc:	01079793          	slli	a5,a5,0x10
80004bc0:	0107d793          	srli	a5,a5,0x10
    if ((datagram_len < offset) || (datagram_len > (0xFFFF - IP_HLEN))) {
80004bc4:	4587ea63          	bltu	a5,s8,80005018 <ip4_reass+0x6cc>
80004bc8:	00010737          	lui	a4,0x10
80004bcc:	feb70713          	addi	a4,a4,-21 # ffeb <_stack_size+0xf7eb>
80004bd0:	44f76463          	bltu	a4,a5,80005018 <ip4_reass+0x6cc>
  fraghdr = (struct ip_hdr *)new_p->payload;
80004bd4:	00492d03          	lw	s10,4(s2)
  len = lwip_ntohs(IPH_LEN(fraghdr));
80004bd8:	003d4783          	lbu	a5,3(s10)
80004bdc:	002d4503          	lbu	a0,2(s10)
80004be0:	00879793          	slli	a5,a5,0x8
80004be4:	00a7e533          	or	a0,a5,a0
80004be8:	d01fb0ef          	jal	ra,800008e8 <lwip_htons>
  hlen = IPH_HL_BYTES(fraghdr);
80004bec:	000d4483          	lbu	s1,0(s10)
80004bf0:	00f4f493          	andi	s1,s1,15
  if (hlen > len) {
80004bf4:	00249493          	slli	s1,s1,0x2
80004bf8:	42956063          	bltu	a0,s1,80005018 <ip4_reass+0x6cc>
  len = (u16_t)(len - hlen);
80004bfc:	40950533          	sub	a0,a0,s1
  offset = IPH_OFFSET_BYTES(fraghdr);
80004c00:	007d4783          	lbu	a5,7(s10)
  len = (u16_t)(len - hlen);
80004c04:	01051493          	slli	s1,a0,0x10
  offset = IPH_OFFSET_BYTES(fraghdr);
80004c08:	006d4503          	lbu	a0,6(s10)
80004c0c:	00879793          	slli	a5,a5,0x8
  len = (u16_t)(len - hlen);
80004c10:	0104d493          	srli	s1,s1,0x10
  offset = IPH_OFFSET_BYTES(fraghdr);
80004c14:	00a7e533          	or	a0,a5,a0
80004c18:	cd1fb0ef          	jal	ra,800008e8 <lwip_htons>
80004c1c:	01351513          	slli	a0,a0,0x13
80004c20:	01355513          	srli	a0,a0,0x13
80004c24:	00351513          	slli	a0,a0,0x3
  iprh = (struct ip_reass_helper *)new_p->payload;
80004c28:	00492783          	lw	a5,4(s2)
  iprh->end = (u16_t)(offset + len);
80004c2c:	00a486b3          	add	a3,s1,a0
80004c30:	01069693          	slli	a3,a3,0x10
  iprh->start = offset;
80004c34:	00855713          	srli	a4,a0,0x8
  iprh->end = (u16_t)(offset + len);
80004c38:	0106d693          	srli	a3,a3,0x10
  iprh->start = offset;
80004c3c:	00e782a3          	sb	a4,5(a5)
  iprh->end = (u16_t)(offset + len);
80004c40:	0086d713          	srli	a4,a3,0x8
  iprh->next_pbuf = NULL;
80004c44:	00078023          	sb	zero,0(a5)
80004c48:	000780a3          	sb	zero,1(a5)
80004c4c:	00078123          	sb	zero,2(a5)
80004c50:	000781a3          	sb	zero,3(a5)
  iprh->start = offset;
80004c54:	00a78223          	sb	a0,4(a5)
  iprh->end = (u16_t)(offset + len);
80004c58:	00d78323          	sb	a3,6(a5)
80004c5c:	00e783a3          	sb	a4,7(a5)
  if (iprh->end < offset) {
80004c60:	3aa6ec63          	bltu	a3,a0,80005018 <ip4_reass+0x6cc>
  for (q = ipr->p; q != NULL;) {
80004c64:	00442603          	lw	a2,4(s0)
  int valid = 1;
80004c68:	00100893          	li	a7,1
  struct ip_reass_helper *iprh, *iprh_tmp, *iprh_prev = NULL;
80004c6c:	00000713          	li	a4,0
  for (q = ipr->p; q != NULL;) {
80004c70:	04061e63          	bnez	a2,80004ccc <ip4_reass+0x380>
    if (iprh_prev != NULL) {
80004c74:	10070c63          	beqz	a4,80004d8c <ip4_reass+0x440>
      LWIP_ASSERT("check fragments don't overlap", iprh_prev->end <= iprh->start);
80004c78:	00774683          	lbu	a3,7(a4)
80004c7c:	00674603          	lbu	a2,6(a4)
      iprh_prev->next_pbuf = new_p;
80004c80:	01270023          	sb	s2,0(a4)
      LWIP_ASSERT("check fragments don't overlap", iprh_prev->end <= iprh->start);
80004c84:	00869693          	slli	a3,a3,0x8
80004c88:	00c6e6b3          	or	a3,a3,a2
      iprh_prev->next_pbuf = new_p;
80004c8c:	00895613          	srli	a2,s2,0x8
80004c90:	00c700a3          	sb	a2,1(a4)
80004c94:	01095613          	srli	a2,s2,0x10
80004c98:	01895913          	srli	s2,s2,0x18
80004c9c:	00c70123          	sb	a2,2(a4)
80004ca0:	012701a3          	sb	s2,3(a4)
      if (iprh_prev->end != iprh->start) {
80004ca4:	0057c703          	lbu	a4,5(a5)
80004ca8:	0047c603          	lbu	a2,4(a5)
80004cac:	00871713          	slli	a4,a4,0x8
80004cb0:	00c76733          	or	a4,a4,a2
80004cb4:	0ae68463          	beq	a3,a4,80004d5c <ip4_reass+0x410>
  if (is_last || ((ipr->flags & IP_REASS_FLAG_LASTFRAG) != 0)) {
80004cb8:	00000893          	li	a7,0
80004cbc:	0a0c9263          	bnez	s9,80004d60 <ip4_reass+0x414>
80004cc0:	0b00006f          	j	80004d70 <ip4_reass+0x424>
  for (ipr = reassdatagrams; ipr != NULL; ipr = ipr->next) {
80004cc4:	00042403          	lw	s0,0(s0)
80004cc8:	da5ff06f          	j	80004a6c <ip4_reass+0x120>
    iprh_tmp = (struct ip_reass_helper *)q->payload;
80004ccc:	00462583          	lw	a1,4(a2)
    if (iprh->start < iprh_tmp->start) {
80004cd0:	0055c803          	lbu	a6,5(a1)
80004cd4:	0045c303          	lbu	t1,4(a1)
80004cd8:	00881813          	slli	a6,a6,0x8
80004cdc:	00686833          	or	a6,a6,t1
80004ce0:	0b057a63          	bgeu	a0,a6,80004d94 <ip4_reass+0x448>
      iprh->next_pbuf = q;
80004ce4:	00865813          	srli	a6,a2,0x8
80004ce8:	00c78023          	sb	a2,0(a5)
80004cec:	010780a3          	sb	a6,1(a5)
80004cf0:	01065813          	srli	a6,a2,0x10
80004cf4:	01865613          	srli	a2,a2,0x18
80004cf8:	01078123          	sb	a6,2(a5)
80004cfc:	00c781a3          	sb	a2,3(a5)
      if (iprh_prev != NULL) {
80004d00:	06070c63          	beqz	a4,80004d78 <ip4_reass+0x42c>
        if ((iprh->start < iprh_prev->end) || (iprh->end > iprh_tmp->start)) {
80004d04:	00774603          	lbu	a2,7(a4)
80004d08:	00674803          	lbu	a6,6(a4)
80004d0c:	00861613          	slli	a2,a2,0x8
80004d10:	01066633          	or	a2,a2,a6
80004d14:	30c56263          	bltu	a0,a2,80005018 <ip4_reass+0x6cc>
80004d18:	0045c503          	lbu	a0,4(a1)
80004d1c:	0055c583          	lbu	a1,5(a1)
80004d20:	00859593          	slli	a1,a1,0x8
80004d24:	00a5e5b3          	or	a1,a1,a0
80004d28:	2ed5e863          	bltu	a1,a3,80005018 <ip4_reass+0x6cc>
        iprh_prev->next_pbuf = new_p;
80004d2c:	00895693          	srli	a3,s2,0x8
80004d30:	01270023          	sb	s2,0(a4)
80004d34:	00d700a3          	sb	a3,1(a4)
80004d38:	01095693          	srli	a3,s2,0x10
80004d3c:	01895913          	srli	s2,s2,0x18
80004d40:	00d70123          	sb	a3,2(a4)
80004d44:	012701a3          	sb	s2,3(a4)
        if (iprh_prev->end != iprh->start) {
80004d48:	0057c703          	lbu	a4,5(a5)
80004d4c:	0047c683          	lbu	a3,4(a5)
80004d50:	00871713          	slli	a4,a4,0x8
80004d54:	00d76733          	or	a4,a4,a3
80004d58:	f6e610e3          	bne	a2,a4,80004cb8 <ip4_reass+0x36c>
  if (is_last || ((ipr->flags & IP_REASS_FLAG_LASTFRAG) != 0)) {
80004d5c:	000c8863          	beqz	s9,80004d6c <ip4_reass+0x420>
80004d60:	01e44703          	lbu	a4,30(s0)
80004d64:	00177713          	andi	a4,a4,1
80004d68:	00070463          	beqz	a4,80004d70 <ip4_reass+0x424>
    if (valid) {
80004d6c:	08089663          	bnez	a7,80004df8 <ip4_reass+0x4ac>
  return IP_REASS_VALIDATE_PBUF_QUEUED; /* not yet valid! */
80004d70:	00000893          	li	a7,0
80004d74:	0d00006f          	j	80004e44 <ip4_reass+0x4f8>
        if (iprh->end > iprh_tmp->start) {
80004d78:	0055c703          	lbu	a4,5(a1)
80004d7c:	0045c603          	lbu	a2,4(a1)
80004d80:	00871713          	slli	a4,a4,0x8
80004d84:	00c76733          	or	a4,a4,a2
80004d88:	28d76863          	bltu	a4,a3,80005018 <ip4_reass+0x6cc>
      ipr->p = new_p;
80004d8c:	01242223          	sw	s2,4(s0)
80004d90:	fcdff06f          	j	80004d5c <ip4_reass+0x410>
    } else if (iprh->start == iprh_tmp->start) {
80004d94:	29050263          	beq	a0,a6,80005018 <ip4_reass+0x6cc>
    } else if (iprh->start < iprh_tmp->end) {
80004d98:	0075c603          	lbu	a2,7(a1)
80004d9c:	0065c303          	lbu	t1,6(a1)
80004da0:	00861613          	slli	a2,a2,0x8
80004da4:	00666633          	or	a2,a2,t1
80004da8:	26c56863          	bltu	a0,a2,80005018 <ip4_reass+0x6cc>
      if (iprh_prev != NULL) {
80004dac:	00070e63          	beqz	a4,80004dc8 <ip4_reass+0x47c>
        if (iprh_prev->end != iprh_tmp->start) {
80004db0:	00674603          	lbu	a2,6(a4)
80004db4:	00774703          	lbu	a4,7(a4)
80004db8:	00871713          	slli	a4,a4,0x8
80004dbc:	00c76733          	or	a4,a4,a2
80004dc0:	00e80463          	beq	a6,a4,80004dc8 <ip4_reass+0x47c>
          valid = 0;
80004dc4:	00000893          	li	a7,0
    q = iprh_tmp->next_pbuf;
80004dc8:	0015c703          	lbu	a4,1(a1)
80004dcc:	0005c603          	lbu	a2,0(a1)
80004dd0:	00871713          	slli	a4,a4,0x8
80004dd4:	00c76733          	or	a4,a4,a2
80004dd8:	0025c603          	lbu	a2,2(a1)
80004ddc:	01061613          	slli	a2,a2,0x10
80004de0:	00e66733          	or	a4,a2,a4
80004de4:	0035c603          	lbu	a2,3(a1)
80004de8:	01861613          	slli	a2,a2,0x18
80004dec:	00e66633          	or	a2,a2,a4
80004df0:	00058713          	mv	a4,a1
80004df4:	e7dff06f          	j	80004c70 <ip4_reass+0x324>
      if ((ipr->p == NULL) || (((struct ip_reass_helper *)ipr->p->payload)->start != 0)) {
80004df8:	00442703          	lw	a4,4(s0)
80004dfc:	f6070ae3          	beqz	a4,80004d70 <ip4_reass+0x424>
80004e00:	00472683          	lw	a3,4(a4)
80004e04:	0056c703          	lbu	a4,5(a3)
80004e08:	0046c603          	lbu	a2,4(a3)
80004e0c:	00871713          	slli	a4,a4,0x8
80004e10:	00c76733          	or	a4,a4,a2
80004e14:	f4071ee3          	bnez	a4,80004d70 <ip4_reass+0x424>
        q = iprh->next_pbuf;
80004e18:	0017c683          	lbu	a3,1(a5)
80004e1c:	0007c703          	lbu	a4,0(a5)
80004e20:	00869693          	slli	a3,a3,0x8
80004e24:	00e6e6b3          	or	a3,a3,a4
80004e28:	0027c703          	lbu	a4,2(a5)
80004e2c:	01071713          	slli	a4,a4,0x10
80004e30:	00d766b3          	or	a3,a4,a3
80004e34:	0037c703          	lbu	a4,3(a5)
80004e38:	01871713          	slli	a4,a4,0x18
80004e3c:	00d76733          	or	a4,a4,a3
        while (q != NULL) {
80004e40:	12071863          	bnez	a4,80004f70 <ip4_reass+0x624>
  ip_reass_pbufcount = (u16_t)(ip_reass_pbufcount + clen);
80004e44:	87c1d783          	lhu	a5,-1924(gp) # 800003c4 <ip_reass_pbufcount>
80004e48:	00fb8bb3          	add	s7,s7,a5
80004e4c:	87719e23          	sh	s7,-1924(gp) # 800003c4 <ip_reass_pbufcount>
  if (is_last) {
80004e50:	000c9c63          	bnez	s9,80004e68 <ip4_reass+0x51c>
    ipr->flags |= IP_REASS_FLAG_LASTFRAG;
80004e54:	01e44783          	lbu	a5,30(s0)
    u16_t datagram_len = (u16_t)(offset + len);
80004e58:	016c0b33          	add	s6,s8,s6
    ipr->datagram_len = datagram_len;
80004e5c:	01641e23          	sh	s6,28(s0)
    ipr->flags |= IP_REASS_FLAG_LASTFRAG;
80004e60:	0017e793          	ori	a5,a5,1
80004e64:	00f40f23          	sb	a5,30(s0)
  return NULL;
80004e68:	00000913          	li	s2,0
  if (valid == IP_REASS_VALIDATE_TELEGRAM_FINISHED) {
80004e6c:	0c088463          	beqz	a7,80004f34 <ip4_reass+0x5e8>
    r = ((struct ip_reass_helper *)ipr->p->payload)->next_pbuf;
80004e70:	00442783          	lw	a5,4(s0)
    u16_t datagram_len = (u16_t)(ipr->datagram_len + IP_HLEN);
80004e74:	01c45a83          	lhu	s5,28(s0)
    SMEMCPY(fraghdr, &ipr->iphdr, IP_HLEN);
80004e78:	01400613          	li	a2,20
    r = ((struct ip_reass_helper *)ipr->p->payload)->next_pbuf;
80004e7c:	0047a903          	lw	s2,4(a5)
    u16_t datagram_len = (u16_t)(ipr->datagram_len + IP_HLEN);
80004e80:	014a8a93          	addi	s5,s5,20
    SMEMCPY(fraghdr, &ipr->iphdr, IP_HLEN);
80004e84:	00840593          	addi	a1,s0,8
    r = ((struct ip_reass_helper *)ipr->p->payload)->next_pbuf;
80004e88:	00194783          	lbu	a5,1(s2)
80004e8c:	00094483          	lbu	s1,0(s2)
    SMEMCPY(fraghdr, &ipr->iphdr, IP_HLEN);
80004e90:	00090513          	mv	a0,s2
    r = ((struct ip_reass_helper *)ipr->p->payload)->next_pbuf;
80004e94:	00879793          	slli	a5,a5,0x8
80004e98:	0097e7b3          	or	a5,a5,s1
80004e9c:	00294483          	lbu	s1,2(s2)
    u16_t datagram_len = (u16_t)(ipr->datagram_len + IP_HLEN);
80004ea0:	010a9a93          	slli	s5,s5,0x10
80004ea4:	010ada93          	srli	s5,s5,0x10
    r = ((struct ip_reass_helper *)ipr->p->payload)->next_pbuf;
80004ea8:	01049493          	slli	s1,s1,0x10
80004eac:	00f4e7b3          	or	a5,s1,a5
80004eb0:	00394483          	lbu	s1,3(s2)
80004eb4:	01849493          	slli	s1,s1,0x18
80004eb8:	00f4e4b3          	or	s1,s1,a5
    SMEMCPY(fraghdr, &ipr->iphdr, IP_HLEN);
80004ebc:	300010ef          	jal	ra,800061bc <memcpy>
    IPH_LEN_SET(fraghdr, lwip_htons(datagram_len));
80004ec0:	000a8513          	mv	a0,s5
80004ec4:	a25fb0ef          	jal	ra,800008e8 <lwip_htons>
80004ec8:	00a90123          	sb	a0,2(s2)
80004ecc:	00855513          	srli	a0,a0,0x8
80004ed0:	00a901a3          	sb	a0,3(s2)
    IPH_OFFSET_SET(fraghdr, 0);
80004ed4:	00090323          	sb	zero,6(s2)
80004ed8:	000903a3          	sb	zero,7(s2)
    IPH_CHKSUM_SET(fraghdr, 0);
80004edc:	00090523          	sb	zero,10(s2)
80004ee0:	000905a3          	sb	zero,11(s2)
      IPH_CHKSUM_SET(fraghdr, inet_chksum(fraghdr, IP_HLEN));
80004ee4:	00090513          	mv	a0,s2
80004ee8:	01400593          	li	a1,20
80004eec:	ff1fb0ef          	jal	ra,80000edc <inet_chksum>
80004ef0:	00a90523          	sb	a0,10(s2)
80004ef4:	00855513          	srli	a0,a0,0x8
80004ef8:	00a905a3          	sb	a0,11(s2)
    p = ipr->p;
80004efc:	00442903          	lw	s2,4(s0)
    while (r != NULL) {
80004f00:	0c049463          	bnez	s1,80004fc8 <ip4_reass+0x67c>
    if (ipr == reassdatagrams) {
80004f04:	8801a783          	lw	a5,-1920(gp) # 800003c8 <reassdatagrams>
        if (ipr_prev->next == ipr) {
80004f08:	00878663          	beq	a5,s0,80004f14 <ip4_reass+0x5c8>
80004f0c:	00078493          	mv	s1,a5
      for (ipr_prev = reassdatagrams; ipr_prev != NULL; ipr_prev = ipr_prev->next) {
80004f10:	10079063          	bnez	a5,80005010 <ip4_reass+0x6c4>
    ip_reass_dequeue_datagram(ipr, ipr_prev);
80004f14:	00048593          	mv	a1,s1
80004f18:	00040513          	mv	a0,s0
80004f1c:	f14ff0ef          	jal	ra,80004630 <ip_reass_dequeue_datagram.isra.0>
    clen = pbuf_clen(p);
80004f20:	00090513          	mv	a0,s2
80004f24:	dddfc0ef          	jal	ra,80001d00 <pbuf_clen>
    ip_reass_pbufcount = (u16_t)(ip_reass_pbufcount - clen);
80004f28:	87c1d783          	lhu	a5,-1924(gp) # 800003c4 <ip_reass_pbufcount>
80004f2c:	40a78533          	sub	a0,a5,a0
80004f30:	86a19e23          	sh	a0,-1924(gp) # 800003c4 <ip_reass_pbufcount>
}
80004f34:	02c12083          	lw	ra,44(sp)
80004f38:	02812403          	lw	s0,40(sp)
80004f3c:	00090513          	mv	a0,s2
80004f40:	02412483          	lw	s1,36(sp)
80004f44:	02012903          	lw	s2,32(sp)
80004f48:	01c12983          	lw	s3,28(sp)
80004f4c:	01812a03          	lw	s4,24(sp)
80004f50:	01412a83          	lw	s5,20(sp)
80004f54:	01012b03          	lw	s6,16(sp)
80004f58:	00c12b83          	lw	s7,12(sp)
80004f5c:	00812c03          	lw	s8,8(sp)
80004f60:	00412c83          	lw	s9,4(sp)
80004f64:	00012d03          	lw	s10,0(sp)
80004f68:	03010113          	addi	sp,sp,48
80004f6c:	00008067          	ret
          iprh = (struct ip_reass_helper *)q->payload;
80004f70:	00472683          	lw	a3,4(a4)
          if (iprh_prev->end != iprh->start) {
80004f74:	0067c583          	lbu	a1,6(a5)
80004f78:	0077c703          	lbu	a4,7(a5)
80004f7c:	0056c783          	lbu	a5,5(a3)
80004f80:	0046c603          	lbu	a2,4(a3)
80004f84:	00871713          	slli	a4,a4,0x8
80004f88:	00879793          	slli	a5,a5,0x8
80004f8c:	00b76733          	or	a4,a4,a1
80004f90:	00c7e7b3          	or	a5,a5,a2
80004f94:	dcf71ee3          	bne	a4,a5,80004d70 <ip4_reass+0x424>
          q = iprh->next_pbuf;
80004f98:	0016c783          	lbu	a5,1(a3)
80004f9c:	0006c703          	lbu	a4,0(a3)
80004fa0:	00879793          	slli	a5,a5,0x8
80004fa4:	00e7e7b3          	or	a5,a5,a4
80004fa8:	0026c703          	lbu	a4,2(a3)
80004fac:	01071713          	slli	a4,a4,0x10
80004fb0:	00f767b3          	or	a5,a4,a5
80004fb4:	0036c703          	lbu	a4,3(a3)
80004fb8:	01871713          	slli	a4,a4,0x18
80004fbc:	00f76733          	or	a4,a4,a5
80004fc0:	00068793          	mv	a5,a3
80004fc4:	e7dff06f          	j	80004e40 <ip4_reass+0x4f4>
      pbuf_remove_header(r, IP_HLEN);
80004fc8:	01400593          	li	a1,20
80004fcc:	00048513          	mv	a0,s1
      iprh = (struct ip_reass_helper *)r->payload;
80004fd0:	0044aa83          	lw	s5,4(s1)
      pbuf_remove_header(r, IP_HLEN);
80004fd4:	8f5fc0ef          	jal	ra,800018c8 <pbuf_remove_header>
      pbuf_cat(p, r);
80004fd8:	00048593          	mv	a1,s1
80004fdc:	00090513          	mv	a0,s2
80004fe0:	d7dfc0ef          	jal	ra,80001d5c <pbuf_cat>
      r = iprh->next_pbuf;
80004fe4:	001ac783          	lbu	a5,1(s5)
80004fe8:	000ac483          	lbu	s1,0(s5)
80004fec:	00879793          	slli	a5,a5,0x8
80004ff0:	0097e7b3          	or	a5,a5,s1
80004ff4:	002ac483          	lbu	s1,2(s5)
80004ff8:	01049493          	slli	s1,s1,0x10
80004ffc:	00f4e7b3          	or	a5,s1,a5
80005000:	003ac483          	lbu	s1,3(s5)
80005004:	01849493          	slli	s1,s1,0x18
80005008:	00f4e4b3          	or	s1,s1,a5
8000500c:	ef5ff06f          	j	80004f00 <ip4_reass+0x5b4>
        if (ipr_prev->next == ipr) {
80005010:	0004a783          	lw	a5,0(s1)
80005014:	ef5ff06f          	j	80004f08 <ip4_reass+0x5bc>
  if (ipr->p == NULL) {
80005018:	00442783          	lw	a5,4(s0)
8000501c:	9a0790e3          	bnez	a5,800049bc <ip4_reass+0x70>
    ip_reass_dequeue_datagram(ipr, NULL);
80005020:	00000593          	li	a1,0
80005024:	00040513          	mv	a0,s0
80005028:	e08ff0ef          	jal	ra,80004630 <ip_reass_dequeue_datagram.isra.0>
8000502c:	991ff06f          	j	800049bc <ip4_reass+0x70>

80005030 <ip4_frag>:
 *
 * @return ERR_OK if sent successfully, err_t otherwise
 */
err_t
ip4_frag(struct pbuf *p, struct netif *netif, const ip4_addr_t *dest)
{
80005030:	f9010113          	addi	sp,sp,-112
80005034:	06112623          	sw	ra,108(sp)
80005038:	06812423          	sw	s0,104(sp)
8000503c:	06912223          	sw	s1,100(sp)
80005040:	07212023          	sw	s2,96(sp)
80005044:	05312e23          	sw	s3,92(sp)
80005048:	05412c23          	sw	s4,88(sp)
8000504c:	05512a23          	sw	s5,84(sp)
80005050:	05612823          	sw	s6,80(sp)
80005054:	05712623          	sw	s7,76(sp)
80005058:	05812423          	sw	s8,72(sp)
8000505c:	05912223          	sw	s9,68(sp)
80005060:	05a12023          	sw	s10,64(sp)
80005064:	03b12e23          	sw	s11,60(sp)
  u16_t newpbuflen = 0;
  u16_t left_to_copy;
#endif
  struct ip_hdr *original_iphdr;
  struct ip_hdr *iphdr;
  const u16_t nfb = (u16_t)((netif->mtu - IP_HLEN) / 8);
80005068:	0285d783          	lhu	a5,40(a1)
  int last;
  u16_t poff = IP_HLEN;
  u16_t tmp;
  int mf_set;

  original_iphdr = (struct ip_hdr *)p->payload;
8000506c:	00452c03          	lw	s8,4(a0)
{
80005070:	00c12e23          	sw	a2,28(sp)
  const u16_t nfb = (u16_t)((netif->mtu - IP_HLEN) / 8);
80005074:	fec78713          	addi	a4,a5,-20
80005078:	41f75793          	srai	a5,a4,0x1f
8000507c:	0077f793          	andi	a5,a5,7
80005080:	00e787b3          	add	a5,a5,a4
80005084:	4037d793          	srai	a5,a5,0x3
80005088:	01079793          	slli	a5,a5,0x10
8000508c:	0107d793          	srli	a5,a5,0x10
80005090:	00f12823          	sw	a5,16(sp)
  iphdr = original_iphdr;
  if (IPH_HL_BYTES(iphdr) != IP_HLEN) {
80005094:	000c4783          	lbu	a5,0(s8)
80005098:	01400713          	li	a4,20
8000509c:	00f7f793          	andi	a5,a5,15
800050a0:	00279793          	slli	a5,a5,0x2
800050a4:	2ae79863          	bne	a5,a4,80005354 <ip4_frag+0x324>
    /* ip4_frag() does not support IP options */
    return ERR_VAL;
  }
  LWIP_ERROR("ip4_frag(): pbuf too short", p->len >= IP_HLEN, return ERR_VAL);
800050a8:	00a55703          	lhu	a4,10(a0)
800050ac:	01300793          	li	a5,19
800050b0:	00050413          	mv	s0,a0
    return ERR_VAL;
800050b4:	ffa00513          	li	a0,-6
  LWIP_ERROR("ip4_frag(): pbuf too short", p->len >= IP_HLEN, return ERR_VAL);
800050b8:	08e7f663          	bgeu	a5,a4,80005144 <ip4_frag+0x114>

  /* Save original offset */
  tmp = lwip_ntohs(IPH_OFFSET(iphdr));
800050bc:	007c4783          	lbu	a5,7(s8)
800050c0:	006c4503          	lbu	a0,6(s8)
800050c4:	00058c93          	mv	s9,a1
800050c8:	00879793          	slli	a5,a5,0x8
800050cc:	00a7e533          	or	a0,a5,a0
800050d0:	819fb0ef          	jal	ra,800008e8 <lwip_htons>
  ofo = tmp & IP_OFFMASK;
800050d4:	00002737          	lui	a4,0x2
800050d8:	00e57733          	and	a4,a0,a4
800050dc:	00e12c23          	sw	a4,24(sp)

  left = (u16_t)(p->tot_len - IP_HLEN);

  while (left) {
    /* Fill this fragment */
    fragsize = LWIP_MIN(left, (u16_t)(nfb * 8));
800050e0:	01012703          	lw	a4,16(sp)
  left = (u16_t)(p->tot_len - IP_HLEN);
800050e4:	00845483          	lhu	s1,8(s0)
  ofo = tmp & IP_OFFMASK;
800050e8:	000027b7          	lui	a5,0x2
    fragsize = LWIP_MIN(left, (u16_t)(nfb * 8));
800050ec:	00371713          	slli	a4,a4,0x3
800050f0:	01071713          	slli	a4,a4,0x10
  ofo = tmp & IP_OFFMASK;
800050f4:	fff78793          	addi	a5,a5,-1 # 1fff <_stack_size+0x17ff>
    fragsize = LWIP_MIN(left, (u16_t)(nfb * 8));
800050f8:	01075713          	srli	a4,a4,0x10
  ofo = tmp & IP_OFFMASK;
800050fc:	00f57bb3          	and	s7,a0,a5
  left = (u16_t)(p->tot_len - IP_HLEN);
80005100:	fec48493          	addi	s1,s1,-20
    fragsize = LWIP_MIN(left, (u16_t)(nfb * 8));
80005104:	00e12623          	sw	a4,12(sp)
80005108:	02e11123          	sh	a4,34(sp)
        pbuf_free(rambuf);
        goto memerr;
      }
      pbuf_ref(p);
      pcr->original = p;
      pcr->pc.custom_free_function = ipfrag_free_pbuf_custom;
8000510c:	80005737          	lui	a4,0x80005
  ofo = tmp & IP_OFFMASK;
80005110:	010b9b93          	slli	s7,s7,0x10
  left = (u16_t)(p->tot_len - IP_HLEN);
80005114:	01049493          	slli	s1,s1,0x10
      pcr->pc.custom_free_function = ipfrag_free_pbuf_custom;
80005118:	8b470713          	addi	a4,a4,-1868 # 800048b4 <_stack_start+0xffffdb24>

    /* Correct header */
    last = (left <= netif->mtu - IP_HLEN);

    /* Set new offset and MF flag */
    tmp = (IP_OFFMASK & (ofo));
8000511c:	02f12423          	sw	a5,40(sp)

    /* No need for separate header pbuf - we allowed room for it in rambuf
     * when allocated.
     */
    netif->output(netif, rambuf, dest);
    IPFRAG_STATS_INC(ip_frag.xmit);
80005120:	8dc18793          	addi	a5,gp,-1828 # 80000424 <lwip_stats>
  ofo = tmp & IP_OFFMASK;
80005124:	010bdb93          	srli	s7,s7,0x10
  left = (u16_t)(p->tot_len - IP_HLEN);
80005128:	0104d493          	srli	s1,s1,0x10
  u16_t poff = IP_HLEN;
8000512c:	01400a13          	li	s4,20
  u16_t newpbuflen = 0;
80005130:	00000913          	li	s2,0
      pcr->pc.custom_free_function = ipfrag_free_pbuf_custom;
80005134:	02e12223          	sw	a4,36(sp)
    IPFRAG_STATS_INC(ip_frag.xmit);
80005138:	00f12a23          	sw	a5,20(sp)
  while (left) {
8000513c:	04049263          	bnez	s1,80005180 <ip4_frag+0x150>
    pbuf_free(rambuf);
    left = (u16_t)(left - fragsize);
    ofo = (u16_t)(ofo + nfb);
  }
  MIB2_STATS_INC(mib2.ipfragoks);
  return ERR_OK;
80005140:	00000513          	li	a0,0
memerr:
  MIB2_STATS_INC(mib2.ipfragfails);
  return ERR_MEM;
}
80005144:	06c12083          	lw	ra,108(sp)
80005148:	06812403          	lw	s0,104(sp)
8000514c:	06412483          	lw	s1,100(sp)
80005150:	06012903          	lw	s2,96(sp)
80005154:	05c12983          	lw	s3,92(sp)
80005158:	05812a03          	lw	s4,88(sp)
8000515c:	05412a83          	lw	s5,84(sp)
80005160:	05012b03          	lw	s6,80(sp)
80005164:	04c12b83          	lw	s7,76(sp)
80005168:	04812c03          	lw	s8,72(sp)
8000516c:	04412c83          	lw	s9,68(sp)
80005170:	04012d03          	lw	s10,64(sp)
80005174:	03c12d83          	lw	s11,60(sp)
80005178:	07010113          	addi	sp,sp,112
8000517c:	00008067          	ret
    fragsize = LWIP_MIN(left, (u16_t)(nfb * 8));
80005180:	00c12783          	lw	a5,12(sp)
80005184:	02215a83          	lhu	s5,34(sp)
80005188:	00f4f463          	bgeu	s1,a5,80005190 <ip4_frag+0x160>
8000518c:	00048a93          	mv	s5,s1
    rambuf = pbuf_alloc(PBUF_LINK, IP_HLEN, PBUF_RAM);
80005190:	28000613          	li	a2,640
80005194:	01400593          	li	a1,20
80005198:	01000513          	li	a0,16
    fragsize = LWIP_MIN(left, (u16_t)(nfb * 8));
8000519c:	010a9a93          	slli	s5,s5,0x10
    rambuf = pbuf_alloc(PBUF_LINK, IP_HLEN, PBUF_RAM);
800051a0:	885fc0ef          	jal	ra,80001a24 <pbuf_alloc>
    fragsize = LWIP_MIN(left, (u16_t)(nfb * 8));
800051a4:	010ada93          	srli	s5,s5,0x10
    rambuf = pbuf_alloc(PBUF_LINK, IP_HLEN, PBUF_RAM);
800051a8:	00050b13          	mv	s6,a0
    if (rambuf == NULL) {
800051ac:	06050663          	beqz	a0,80005218 <ip4_frag+0x1e8>
    SMEMCPY(rambuf->payload, original_iphdr, IP_HLEN);
800051b0:	00452503          	lw	a0,4(a0)
800051b4:	01400613          	li	a2,20
800051b8:	000c0593          	mv	a1,s8
800051bc:	000010ef          	jal	ra,800061bc <memcpy>
    iphdr = (struct ip_hdr *)rambuf->payload;
800051c0:	004b2983          	lw	s3,4(s6)
    left_to_copy = fragsize;
800051c4:	000a8d93          	mv	s11,s5
    while (left_to_copy) {
800051c8:	0c0d8063          	beqz	s11,80005288 <ip4_frag+0x258>
      u16_t plen = (u16_t)(p->len - poff);
800051cc:	00a45783          	lhu	a5,10(s0)
800051d0:	414787b3          	sub	a5,a5,s4
      newpbuflen = LWIP_MIN(left_to_copy, plen);
800051d4:	00078913          	mv	s2,a5
800051d8:	01079793          	slli	a5,a5,0x10
800051dc:	0107d793          	srli	a5,a5,0x10
800051e0:	00fdf463          	bgeu	s11,a5,800051e8 <ip4_frag+0x1b8>
800051e4:	000d8913          	mv	s2,s11
800051e8:	01091913          	slli	s2,s2,0x10
800051ec:	01095913          	srli	s2,s2,0x10
      if (!newpbuflen) {
800051f0:	00091863          	bnez	s2,80005200 <ip4_frag+0x1d0>
        p = p->next;
800051f4:	00042403          	lw	s0,0(s0)
        poff = 0;
800051f8:	00000a13          	li	s4,0
800051fc:	fcdff06f          	j	800051c8 <ip4_frag+0x198>
  return (struct pbuf_custom_ref *)memp_malloc(MEMP_FRAG_PBUF);
80005200:	00300513          	li	a0,3
80005204:	e51fb0ef          	jal	ra,80001054 <memp_malloc>
80005208:	00050d13          	mv	s10,a0
      if (pcr == NULL) {
8000520c:	00051a63          	bnez	a0,80005220 <ip4_frag+0x1f0>
        pbuf_free(rambuf);
80005210:	000b0513          	mv	a0,s6
80005214:	f3cfc0ef          	jal	ra,80001950 <pbuf_free>
  return ERR_MEM;
80005218:	fff00513          	li	a0,-1
8000521c:	f29ff06f          	j	80005144 <ip4_frag+0x114>
      newpbuf = pbuf_alloced_custom(PBUF_RAW, newpbuflen, PBUF_REF, &pcr->pc,
80005220:	00442703          	lw	a4,4(s0)
80005224:	00050693          	mv	a3,a0
80005228:	00090793          	mv	a5,s2
8000522c:	01470733          	add	a4,a4,s4
80005230:	04100613          	li	a2,65
80005234:	00090593          	mv	a1,s2
80005238:	00000513          	li	a0,0
8000523c:	e28fc0ef          	jal	ra,80001864 <pbuf_alloced_custom>
      if (newpbuf == NULL) {
80005240:	00051a63          	bnez	a0,80005254 <ip4_frag+0x224>
  memp_free(MEMP_FRAG_PBUF, p);
80005244:	000d0593          	mv	a1,s10
80005248:	00300513          	li	a0,3
8000524c:	e6dfb0ef          	jal	ra,800010b8 <memp_free>
80005250:	fc1ff06f          	j	80005210 <ip4_frag+0x1e0>
80005254:	02a12623          	sw	a0,44(sp)
      pbuf_ref(p);
80005258:	00040513          	mv	a0,s0
8000525c:	ac9fc0ef          	jal	ra,80001d24 <pbuf_ref>
      pcr->pc.custom_free_function = ipfrag_free_pbuf_custom;
80005260:	02412783          	lw	a5,36(sp)
      pbuf_cat(rambuf, newpbuf);
80005264:	02c12583          	lw	a1,44(sp)
      pcr->original = p;
80005268:	008d2a23          	sw	s0,20(s10)
      pcr->pc.custom_free_function = ipfrag_free_pbuf_custom;
8000526c:	00fd2823          	sw	a5,16(s10)
      pbuf_cat(rambuf, newpbuf);
80005270:	000b0513          	mv	a0,s6
80005274:	ae9fc0ef          	jal	ra,80001d5c <pbuf_cat>
      left_to_copy = (u16_t)(left_to_copy - newpbuflen);
80005278:	412d8833          	sub	a6,s11,s2
8000527c:	01081d93          	slli	s11,a6,0x10
80005280:	010ddd93          	srli	s11,s11,0x10
      if (left_to_copy) {
80005284:	f60d98e3          	bnez	s11,800051f4 <ip4_frag+0x1c4>
    tmp = (IP_OFFMASK & (ofo));
80005288:	02812783          	lw	a5,40(sp)
    poff = (u16_t)(poff + newpbuflen);
8000528c:	01490a33          	add	s4,s2,s4
80005290:	010a1a13          	slli	s4,s4,0x10
    tmp = (IP_OFFMASK & (ofo));
80005294:	00fbf533          	and	a0,s7,a5
    last = (left <= netif->mtu - IP_HLEN);
80005298:	028cd783          	lhu	a5,40(s9)
    poff = (u16_t)(poff + newpbuflen);
8000529c:	010a5a13          	srli	s4,s4,0x10
    last = (left <= netif->mtu - IP_HLEN);
800052a0:	fed78793          	addi	a5,a5,-19
    if (!last || mf_set) {
800052a4:	00f4d663          	bge	s1,a5,800052b0 <ip4_frag+0x280>
800052a8:	01812783          	lw	a5,24(sp)
800052ac:	00078663          	beqz	a5,800052b8 <ip4_frag+0x288>
      tmp = tmp | IP_MF;
800052b0:	000027b7          	lui	a5,0x2
800052b4:	00f56533          	or	a0,a0,a5
    IPH_OFFSET_SET(iphdr, lwip_htons(tmp));
800052b8:	e30fb0ef          	jal	ra,800008e8 <lwip_htons>
800052bc:	00a98323          	sb	a0,6(s3)
800052c0:	00855513          	srli	a0,a0,0x8
800052c4:	00a983a3          	sb	a0,7(s3)
    IPH_LEN_SET(iphdr, lwip_htons((u16_t)(fragsize + IP_HLEN)));
800052c8:	014a8513          	addi	a0,s5,20
800052cc:	01051513          	slli	a0,a0,0x10
800052d0:	01055513          	srli	a0,a0,0x10
800052d4:	e14fb0ef          	jal	ra,800008e8 <lwip_htons>
800052d8:	00a98123          	sb	a0,2(s3)
800052dc:	00855513          	srli	a0,a0,0x8
800052e0:	00a981a3          	sb	a0,3(s3)
      IPH_CHKSUM_SET(iphdr, inet_chksum(iphdr, IP_HLEN));
800052e4:	01400593          	li	a1,20
    IPH_CHKSUM_SET(iphdr, 0);
800052e8:	00098523          	sb	zero,10(s3)
800052ec:	000985a3          	sb	zero,11(s3)
      IPH_CHKSUM_SET(iphdr, inet_chksum(iphdr, IP_HLEN));
800052f0:	00098513          	mv	a0,s3
800052f4:	be9fb0ef          	jal	ra,80000edc <inet_chksum>
800052f8:	00a98523          	sb	a0,10(s3)
800052fc:	00855513          	srli	a0,a0,0x8
80005300:	00a985a3          	sb	a0,11(s3)
    netif->output(netif, rambuf, dest);
80005304:	014ca783          	lw	a5,20(s9)
80005308:	01c12603          	lw	a2,28(sp)
8000530c:	000b0593          	mv	a1,s6
80005310:	000c8513          	mv	a0,s9
80005314:	000780e7          	jalr	a5 # 2000 <_stack_size+0x1800>
    IPFRAG_STATS_INC(ip_frag.xmit);
80005318:	01412783          	lw	a5,20(sp)
8000531c:	01412703          	lw	a4,20(sp)
    pbuf_free(rambuf);
80005320:	000b0513          	mv	a0,s6
    IPFRAG_STATS_INC(ip_frag.xmit);
80005324:	0187d783          	lhu	a5,24(a5)
    left = (u16_t)(left - fragsize);
80005328:	415484b3          	sub	s1,s1,s5
8000532c:	01049493          	slli	s1,s1,0x10
    IPFRAG_STATS_INC(ip_frag.xmit);
80005330:	00178793          	addi	a5,a5,1
80005334:	00f71c23          	sh	a5,24(a4)
    pbuf_free(rambuf);
80005338:	e18fc0ef          	jal	ra,80001950 <pbuf_free>
    ofo = (u16_t)(ofo + nfb);
8000533c:	01012783          	lw	a5,16(sp)
    left = (u16_t)(left - fragsize);
80005340:	0104d493          	srli	s1,s1,0x10
    ofo = (u16_t)(ofo + nfb);
80005344:	00fb8bb3          	add	s7,s7,a5
80005348:	010b9b93          	slli	s7,s7,0x10
8000534c:	010bdb93          	srli	s7,s7,0x10
80005350:	dedff06f          	j	8000513c <ip4_frag+0x10c>
    return ERR_VAL;
80005354:	ffa00513          	li	a0,-6
80005358:	dedff06f          	j	80005144 <ip4_frag+0x114>

8000535c <ip4_input_accept>:
                         ip4_addr_get_u32(ip4_current_dest_addr()) & ip4_addr_get_u32(netif_ip4_netmask(netif)),
                         ip4_addr_get_u32(netif_ip4_addr(netif)) & ip4_addr_get_u32(netif_ip4_netmask(netif)),
                         ip4_addr_get_u32(ip4_current_dest_addr()) & ~ip4_addr_get_u32(netif_ip4_netmask(netif))));

  /* interface is up and configured? */
  if ((netif_is_up(netif)) && (!ip4_addr_isany_val(*netif_ip4_addr(netif)))) {
8000535c:	03154783          	lbu	a5,49(a0)
80005360:	0017f793          	andi	a5,a5,1
80005364:	04078063          	beqz	a5,800053a4 <ip4_input_accept+0x48>
80005368:	00452703          	lw	a4,4(a0)
8000536c:	00050593          	mv	a1,a0
      /* accept on this netif */
      return 1;
    }
#endif /* LWIP_AUTOIP */
  }
  return 0;
80005370:	00000513          	li	a0,0
  if ((netif_is_up(netif)) && (!ip4_addr_isany_val(*netif_ip4_addr(netif)))) {
80005374:	02070c63          	beqz	a4,800053ac <ip4_input_accept+0x50>
    if (ip4_addr_cmp(ip4_current_dest_addr(), netif_ip4_addr(netif)) ||
80005378:	8d81a783          	lw	a5,-1832(gp) # 80000420 <ip_data+0x14>
      return 1;
8000537c:	00100513          	li	a0,1
    if (ip4_addr_cmp(ip4_current_dest_addr(), netif_ip4_addr(netif)) ||
80005380:	02f70663          	beq	a4,a5,800053ac <ip4_input_accept+0x50>
{
80005384:	ff010113          	addi	sp,sp,-16
        ip4_addr_isbroadcast(ip4_current_dest_addr(), netif)
80005388:	00078513          	mv	a0,a5
{
8000538c:	00112623          	sw	ra,12(sp)
        ip4_addr_isbroadcast(ip4_current_dest_addr(), netif)
80005390:	7a4000ef          	jal	ra,80005b34 <ip4_addr_isbroadcast_u32>
}
80005394:	00c12083          	lw	ra,12(sp)
    if (ip4_addr_cmp(ip4_current_dest_addr(), netif_ip4_addr(netif)) ||
80005398:	00a03533          	snez	a0,a0
}
8000539c:	01010113          	addi	sp,sp,16
800053a0:	00008067          	ret
  return 0;
800053a4:	00000513          	li	a0,0
800053a8:	00008067          	ret
}
800053ac:	00008067          	ret

800053b0 <ip4_route>:
  NETIF_FOREACH(netif) {
800053b0:	8681a783          	lw	a5,-1944(gp) # 800003b0 <netif_list>
800053b4:	04079463          	bnez	a5,800053fc <ip4_route+0x4c>
  if ((netif_default == NULL) || !netif_is_up(netif_default) || !netif_is_link_up(netif_default) ||
800053b8:	8641a703          	lw	a4,-1948(gp) # 800003ac <netif_default>
800053bc:	02070463          	beqz	a4,800053e4 <ip4_route+0x34>
800053c0:	03174683          	lbu	a3,49(a4)
800053c4:	00500613          	li	a2,5
800053c8:	0056f693          	andi	a3,a3,5
800053cc:	00c69c63          	bne	a3,a2,800053e4 <ip4_route+0x34>
800053d0:	00472683          	lw	a3,4(a4)
800053d4:	00068863          	beqz	a3,800053e4 <ip4_route+0x34>
      ip4_addr_isany_val(*netif_ip4_addr(netif_default)) || ip4_addr_isloopback(dest)) {
800053d8:	00054603          	lbu	a2,0(a0)
800053dc:	07f00693          	li	a3,127
800053e0:	06d61463          	bne	a2,a3,80005448 <ip4_route+0x98>
    IP_STATS_INC(ip.rterr);
800053e4:	8dc18713          	addi	a4,gp,-1828 # 80000424 <lwip_stats>
800053e8:	03e75683          	lhu	a3,62(a4)
800053ec:	00168693          	addi	a3,a3,1
800053f0:	02d71f23          	sh	a3,62(a4)
}
800053f4:	00078513          	mv	a0,a5
800053f8:	00008067          	ret
    if (netif_is_up(netif) && netif_is_link_up(netif) && !ip4_addr_isany_val(*netif_ip4_addr(netif))) {
800053fc:	0317c683          	lbu	a3,49(a5)
80005400:	0016f713          	andi	a4,a3,1
80005404:	02070e63          	beqz	a4,80005440 <ip4_route+0x90>
80005408:	0026d713          	srli	a4,a3,0x2
8000540c:	00177713          	andi	a4,a4,1
80005410:	02070863          	beqz	a4,80005440 <ip4_route+0x90>
80005414:	0047a703          	lw	a4,4(a5)
80005418:	02070463          	beqz	a4,80005440 <ip4_route+0x90>
      if (ip4_addr_netcmp(dest, netif_ip4_addr(netif), netif_ip4_netmask(netif))) {
8000541c:	00052603          	lw	a2,0(a0)
80005420:	0087a583          	lw	a1,8(a5)
80005424:	00c74733          	xor	a4,a4,a2
80005428:	00b77733          	and	a4,a4,a1
8000542c:	fc0704e3          	beqz	a4,800053f4 <ip4_route+0x44>
      if (((netif->flags & NETIF_FLAG_BROADCAST) == 0) && ip4_addr_cmp(dest, netif_ip4_gw(netif))) {
80005430:	0026f693          	andi	a3,a3,2
80005434:	00069663          	bnez	a3,80005440 <ip4_route+0x90>
80005438:	00c7a703          	lw	a4,12(a5)
8000543c:	fae60ce3          	beq	a2,a4,800053f4 <ip4_route+0x44>
  NETIF_FOREACH(netif) {
80005440:	0007a783          	lw	a5,0(a5)
80005444:	f71ff06f          	j	800053b4 <ip4_route+0x4>
80005448:	00070793          	mv	a5,a4
8000544c:	fa9ff06f          	j	800053f4 <ip4_route+0x44>

80005450 <ip4_input>:
 * @return ERR_OK if the packet was processed (could return ERR_* if it wasn't
 *         processed, but currently always returns ERR_OK)
 */
err_t
ip4_input(struct pbuf *p, struct netif *inp)
{
80005450:	fd010113          	addi	sp,sp,-48
80005454:	02912223          	sw	s1,36(sp)
80005458:	03212023          	sw	s2,32(sp)
8000545c:	02112623          	sw	ra,44(sp)
80005460:	02812423          	sw	s0,40(sp)
80005464:	01312e23          	sw	s3,28(sp)
80005468:	01412c23          	sw	s4,24(sp)
8000546c:	01512a23          	sw	s5,20(sp)
80005470:	01612823          	sw	s6,16(sp)
80005474:	01712623          	sw	s7,12(sp)
80005478:	01812423          	sw	s8,8(sp)
  raw_input_state_t raw_status;
#endif /* LWIP_RAW */

  LWIP_ASSERT_CORE_LOCKED();

  IP_STATS_INC(ip.recv);
8000547c:	8dc18913          	addi	s2,gp,-1828 # 80000424 <lwip_stats>
80005480:	03295783          	lhu	a5,50(s2)
80005484:	00178793          	addi	a5,a5,1
80005488:	02f91923          	sh	a5,50(s2)
  MIB2_STATS_INC(mib2.ipinreceives);

  /* identify the IP header */
  iphdr = (struct ip_hdr *)p->payload;
8000548c:	00452983          	lw	s3,4(a0)
  if (IPH_V(iphdr) != 4) {
80005490:	00400793          	li	a5,4
80005494:	0009cb03          	lbu	s6,0(s3)
80005498:	004b5713          	srli	a4,s6,0x4
8000549c:	04f70a63          	beq	a4,a5,800054f0 <ip4_input+0xa0>
    LWIP_DEBUGF(IP_DEBUG | LWIP_DBG_LEVEL_WARNING, ("IP packet dropped due to bad version number %"U16_F"\n", (u16_t)IPH_V(iphdr)));
    ip4_debug_print(p);
    pbuf_free(p);
800054a0:	cb0fc0ef          	jal	ra,80001950 <pbuf_free>
    IP_STATS_INC(ip.err);
800054a4:	04495783          	lhu	a5,68(s2)
800054a8:	00178793          	addi	a5,a5,1
800054ac:	04f91223          	sh	a5,68(s2)
    IP_STATS_INC(ip.drop);
800054b0:	03695783          	lhu	a5,54(s2)
800054b4:	00178793          	addi	a5,a5,1
800054b8:	02f91b23          	sh	a5,54(s2)
  ip_data.current_ip_header_tot_len = 0;
  ip4_addr_set_any(ip4_current_src_addr());
  ip4_addr_set_any(ip4_current_dest_addr());

  return ERR_OK;
}
800054bc:	02c12083          	lw	ra,44(sp)
800054c0:	02812403          	lw	s0,40(sp)
800054c4:	02412483          	lw	s1,36(sp)
800054c8:	02012903          	lw	s2,32(sp)
800054cc:	01c12983          	lw	s3,28(sp)
800054d0:	01812a03          	lw	s4,24(sp)
800054d4:	01412a83          	lw	s5,20(sp)
800054d8:	01012b03          	lw	s6,16(sp)
800054dc:	00c12b83          	lw	s7,12(sp)
800054e0:	00812c03          	lw	s8,8(sp)
800054e4:	00000513          	li	a0,0
800054e8:	03010113          	addi	sp,sp,48
800054ec:	00008067          	ret
  iphdr_len = lwip_ntohs(IPH_LEN(iphdr));
800054f0:	0039c783          	lbu	a5,3(s3)
800054f4:	00050413          	mv	s0,a0
800054f8:	0029c503          	lbu	a0,2(s3)
800054fc:	00879793          	slli	a5,a5,0x8
80005500:	00058a93          	mv	s5,a1
80005504:	00a7e533          	or	a0,a5,a0
80005508:	be0fb0ef          	jal	ra,800008e8 <lwip_htons>
  iphdr_hlen = IPH_HL_BYTES(iphdr);
8000550c:	00fb7b13          	andi	s6,s6,15
  if (iphdr_len < p->tot_len) {
80005510:	00845783          	lhu	a5,8(s0)
  iphdr_hlen = IPH_HL_BYTES(iphdr);
80005514:	002b1b13          	slli	s6,s6,0x2
80005518:	010b1913          	slli	s2,s6,0x10
8000551c:	8dc18493          	addi	s1,gp,-1828 # 80000424 <lwip_stats>
80005520:	01095913          	srli	s2,s2,0x10
  iphdr_len = lwip_ntohs(IPH_LEN(iphdr));
80005524:	00050a13          	mv	s4,a0
  if (iphdr_len < p->tot_len) {
80005528:	00f57863          	bgeu	a0,a5,80005538 <ip4_input+0xe8>
    pbuf_realloc(p, iphdr_len);
8000552c:	00050593          	mv	a1,a0
80005530:	00040513          	mv	a0,s0
80005534:	eacfc0ef          	jal	ra,80001be0 <pbuf_realloc>
  if ((iphdr_hlen > p->len) || (iphdr_len > p->tot_len) || (iphdr_hlen < IP_HLEN)) {
80005538:	00a45783          	lhu	a5,10(s0)
8000553c:	0127ea63          	bltu	a5,s2,80005550 <ip4_input+0x100>
80005540:	00845783          	lhu	a5,8(s0)
80005544:	0147e663          	bltu	a5,s4,80005550 <ip4_input+0x100>
80005548:	01300793          	li	a5,19
8000554c:	0327e463          	bltu	a5,s2,80005574 <ip4_input+0x124>
    pbuf_free(p);
80005550:	00040513          	mv	a0,s0
80005554:	bfcfc0ef          	jal	ra,80001950 <pbuf_free>
    IP_STATS_INC(ip.lenerr);
80005558:	03a4d783          	lhu	a5,58(s1)
8000555c:	00178793          	addi	a5,a5,1
80005560:	02f49d23          	sh	a5,58(s1)
      IP_STATS_INC(ip.drop);
80005564:	0364d783          	lhu	a5,54(s1)
80005568:	00178793          	addi	a5,a5,1
8000556c:	02f49b23          	sh	a5,54(s1)
      return ERR_OK;
80005570:	f4dff06f          	j	800054bc <ip4_input+0x6c>
    if (inet_chksum(iphdr, iphdr_hlen) != 0) {
80005574:	00090593          	mv	a1,s2
80005578:	00098513          	mv	a0,s3
8000557c:	961fb0ef          	jal	ra,80000edc <inet_chksum>
80005580:	00050e63          	beqz	a0,8000559c <ip4_input+0x14c>
      pbuf_free(p);
80005584:	00040513          	mv	a0,s0
80005588:	bc8fc0ef          	jal	ra,80001950 <pbuf_free>
      IP_STATS_INC(ip.chkerr);
8000558c:	0384d783          	lhu	a5,56(s1)
80005590:	00178793          	addi	a5,a5,1
80005594:	02f49c23          	sh	a5,56(s1)
80005598:	fcdff06f          	j	80005564 <ip4_input+0x114>
  ip_addr_copy_from_ip4(ip_data.current_iphdr_dest, iphdr->dest);
8000559c:	0119c703          	lbu	a4,17(s3)
800055a0:	0109c783          	lbu	a5,16(s3)
800055a4:	00871713          	slli	a4,a4,0x8
800055a8:	00f76733          	or	a4,a4,a5
800055ac:	0129c783          	lbu	a5,18(s3)
800055b0:	8c418613          	addi	a2,gp,-1852 # 8000040c <ip_data>
800055b4:	8c418913          	addi	s2,gp,-1852 # 8000040c <ip_data>
800055b8:	01079793          	slli	a5,a5,0x10
800055bc:	00e7e733          	or	a4,a5,a4
800055c0:	0139c783          	lbu	a5,19(s3)
800055c4:	01879793          	slli	a5,a5,0x18
800055c8:	00e7e7b3          	or	a5,a5,a4
800055cc:	00f62a23          	sw	a5,20(a2)
  ip_addr_copy_from_ip4(ip_data.current_iphdr_src, iphdr->src);
800055d0:	00d9c683          	lbu	a3,13(s3)
800055d4:	00c9c703          	lbu	a4,12(s3)
  if (ip4_addr_ismulticast(ip4_current_dest_addr())) {
800055d8:	0f07f793          	andi	a5,a5,240
  ip_addr_copy_from_ip4(ip_data.current_iphdr_src, iphdr->src);
800055dc:	00869693          	slli	a3,a3,0x8
800055e0:	00e6e6b3          	or	a3,a3,a4
800055e4:	00e9c703          	lbu	a4,14(s3)
800055e8:	01071713          	slli	a4,a4,0x10
800055ec:	00d766b3          	or	a3,a4,a3
800055f0:	00f9c703          	lbu	a4,15(s3)
800055f4:	01871713          	slli	a4,a4,0x18
800055f8:	00d76733          	or	a4,a4,a3
800055fc:	00e62823          	sw	a4,16(a2)
  if (ip4_addr_ismulticast(ip4_current_dest_addr())) {
80005600:	0e000713          	li	a4,224
80005604:	04e79463          	bne	a5,a4,8000564c <ip4_input+0x1fc>
    if ((netif_is_up(inp)) && (!ip4_addr_isany_val(*netif_ip4_addr(inp)))) {
80005608:	031ac783          	lbu	a5,49(s5)
8000560c:	0017f793          	andi	a5,a5,1
80005610:	04078e63          	beqz	a5,8000566c <ip4_input+0x21c>
80005614:	004aa783          	lw	a5,4(s5)
80005618:	04078a63          	beqz	a5,8000566c <ip4_input+0x21c>
      netif = NULL;
8000561c:	000a8a13          	mv	s4,s5
    if ((ip4_addr_isbroadcast(ip4_current_src_addr(), inp)) ||
80005620:	01092503          	lw	a0,16(s2)
80005624:	000a8593          	mv	a1,s5
80005628:	50c000ef          	jal	ra,80005b34 <ip4_addr_isbroadcast_u32>
8000562c:	00051a63          	bnez	a0,80005640 <ip4_input+0x1f0>
        (ip4_addr_ismulticast(ip4_current_src_addr()))) {
80005630:	01092783          	lw	a5,16(s2)
    if ((ip4_addr_isbroadcast(ip4_current_src_addr(), inp)) ||
80005634:	0e000713          	li	a4,224
        (ip4_addr_ismulticast(ip4_current_src_addr()))) {
80005638:	0f07f793          	andi	a5,a5,240
    if ((ip4_addr_isbroadcast(ip4_current_src_addr(), inp)) ||
8000563c:	04e79a63          	bne	a5,a4,80005690 <ip4_input+0x240>
      pbuf_free(p);
80005640:	00040513          	mv	a0,s0
80005644:	b0cfc0ef          	jal	ra,80001950 <pbuf_free>
      IP_STATS_INC(ip.drop);
80005648:	f1dff06f          	j	80005564 <ip4_input+0x114>
    if (ip4_input_accept(inp)) {
8000564c:	000a8513          	mv	a0,s5
80005650:	d0dff0ef          	jal	ra,8000535c <ip4_input_accept>
80005654:	fc0514e3          	bnez	a0,8000561c <ip4_input+0x1cc>
      if (!ip4_addr_isloopback(ip4_current_dest_addr()))
80005658:	01494703          	lbu	a4,20(s2)
8000565c:	07f00793          	li	a5,127
80005660:	00f70663          	beq	a4,a5,8000566c <ip4_input+0x21c>
        NETIF_FOREACH(netif) {
80005664:	8681aa03          	lw	s4,-1944(gp) # 800003b0 <netif_list>
80005668:	000a1663          	bnez	s4,80005674 <ip4_input+0x224>
      netif = NULL;
8000566c:	00000a13          	li	s4,0
80005670:	fb1ff06f          	j	80005620 <ip4_input+0x1d0>
          if (netif == inp) {
80005674:	015a1663          	bne	s4,s5,80005680 <ip4_input+0x230>
        NETIF_FOREACH(netif) {
80005678:	000a2a03          	lw	s4,0(s4)
8000567c:	fedff06f          	j	80005668 <ip4_input+0x218>
          if (ip4_input_accept(netif)) {
80005680:	000a0513          	mv	a0,s4
80005684:	cd9ff0ef          	jal	ra,8000535c <ip4_input_accept>
80005688:	fe0508e3          	beqz	a0,80005678 <ip4_input+0x228>
8000568c:	f95ff06f          	j	80005620 <ip4_input+0x1d0>
  if (netif == NULL) {
80005690:	000a1e63          	bnez	s4,800056ac <ip4_input+0x25c>
      IP_STATS_INC(ip.drop);
80005694:	0364d783          	lhu	a5,54(s1)
    pbuf_free(p);
80005698:	00040513          	mv	a0,s0
      IP_STATS_INC(ip.drop);
8000569c:	00178793          	addi	a5,a5,1
800056a0:	02f49b23          	sh	a5,54(s1)
    pbuf_free(p);
800056a4:	aacfc0ef          	jal	ra,80001950 <pbuf_free>
    return ERR_OK;
800056a8:	e15ff06f          	j	800054bc <ip4_input+0x6c>
  if ((IPH_OFFSET(iphdr) & PP_HTONS(IP_OFFMASK | IP_MF)) != 0) {
800056ac:	0069c783          	lbu	a5,6(s3)
800056b0:	0079c703          	lbu	a4,7(s3)
800056b4:	03f7f793          	andi	a5,a5,63
800056b8:	00871713          	slli	a4,a4,0x8
800056bc:	00e7e7b3          	or	a5,a5,a4
800056c0:	00078c63          	beqz	a5,800056d8 <ip4_input+0x288>
    p = ip4_reass(p);
800056c4:	00040513          	mv	a0,s0
800056c8:	a84ff0ef          	jal	ra,8000494c <ip4_reass>
800056cc:	00050413          	mv	s0,a0
    if (p == NULL) {
800056d0:	de0506e3          	beqz	a0,800054bc <ip4_input+0x6c>
    iphdr = (const struct ip_hdr *)p->payload;
800056d4:	00452983          	lw	s3,4(a0)
  ip_data.current_netif = netif;
800056d8:	01492023          	sw	s4,0(s2)
  ip_data.current_input_netif = inp;
800056dc:	01592223          	sw	s5,4(s2)
  ip_data.current_ip4_header = iphdr;
800056e0:	01392423          	sw	s3,8(s2)
  ip_data.current_ip_header_tot_len = IPH_HL_BYTES(iphdr);
800056e4:	0009c783          	lbu	a5,0(s3)
  raw_status = raw_input(p, inp);
800056e8:	000a8593          	mv	a1,s5
800056ec:	00040513          	mv	a0,s0
  ip_data.current_ip_header_tot_len = IPH_HL_BYTES(iphdr);
800056f0:	00f7f793          	andi	a5,a5,15
800056f4:	00279793          	slli	a5,a5,0x2
800056f8:	00f91623          	sh	a5,12(s2)
  if (raw_status != RAW_INPUT_EATEN)
800056fc:	00100c13          	li	s8,1
  raw_status = raw_input(p, inp);
80005700:	e31fc0ef          	jal	ra,80002530 <raw_input>
80005704:	00050b93          	mv	s7,a0
  if (raw_status != RAW_INPUT_EATEN)
80005708:	03850663          	beq	a0,s8,80005734 <ip4_input+0x2e4>
    pbuf_remove_header(p, iphdr_hlen); /* Move to payload, no check necessary. */
8000570c:	000b0593          	mv	a1,s6
80005710:	00040513          	mv	a0,s0
80005714:	9b4fc0ef          	jal	ra,800018c8 <pbuf_remove_header>
    switch (IPH_PROTO(iphdr)) {
80005718:	0099c783          	lbu	a5,9(s3)
8000571c:	03878a63          	beq	a5,s8,80005750 <ip4_input+0x300>
80005720:	01100713          	li	a4,17
80005724:	02e79e63          	bne	a5,a4,80005760 <ip4_input+0x310>
        udp_input(p, inp);
80005728:	000a8593          	mv	a1,s5
8000572c:	00040513          	mv	a0,s0
80005730:	dc0fd0ef          	jal	ra,80002cf0 <udp_input>
  ip_data.current_netif = NULL;
80005734:	00092023          	sw	zero,0(s2)
  ip_data.current_input_netif = NULL;
80005738:	00092223          	sw	zero,4(s2)
  ip_data.current_ip4_header = NULL;
8000573c:	00092423          	sw	zero,8(s2)
  ip_data.current_ip_header_tot_len = 0;
80005740:	00091623          	sh	zero,12(s2)
  ip4_addr_set_any(ip4_current_src_addr());
80005744:	00092823          	sw	zero,16(s2)
  ip4_addr_set_any(ip4_current_dest_addr());
80005748:	00092a23          	sw	zero,20(s2)
  return ERR_OK;
8000574c:	d71ff06f          	j	800054bc <ip4_input+0x6c>
        icmp_input(p, inp);
80005750:	000a8593          	mv	a1,s5
80005754:	00040513          	mv	a0,s0
80005758:	b95fe0ef          	jal	ra,800042ec <icmp_input>
        break;
8000575c:	fd9ff06f          	j	80005734 <ip4_input+0x2e4>
        if (raw_status == RAW_INPUT_DELIVERED) {
80005760:	00200793          	li	a5,2
80005764:	04fb8a63          	beq	s7,a5,800057b8 <ip4_input+0x368>
          if (!ip4_addr_isbroadcast(ip4_current_dest_addr(), netif) &&
80005768:	01492503          	lw	a0,20(s2)
8000576c:	000a0593          	mv	a1,s4
80005770:	3c4000ef          	jal	ra,80005b34 <ip4_addr_isbroadcast_u32>
80005774:	02051663          	bnez	a0,800057a0 <ip4_input+0x350>
              !ip4_addr_ismulticast(ip4_current_dest_addr())) {
80005778:	01492783          	lw	a5,20(s2)
          if (!ip4_addr_isbroadcast(ip4_current_dest_addr(), netif) &&
8000577c:	0e000713          	li	a4,224
              !ip4_addr_ismulticast(ip4_current_dest_addr())) {
80005780:	0f07f793          	andi	a5,a5,240
          if (!ip4_addr_isbroadcast(ip4_current_dest_addr(), netif) &&
80005784:	00e78e63          	beq	a5,a4,800057a0 <ip4_input+0x350>
            pbuf_header_force(p, (s16_t)iphdr_hlen); /* Move to ip header, no check necessary. */
80005788:	000b0593          	mv	a1,s6
8000578c:	00040513          	mv	a0,s0
80005790:	9acfc0ef          	jal	ra,8000193c <pbuf_header_force>
            icmp_dest_unreach(p, ICMP_DUR_PROTO);
80005794:	00200593          	li	a1,2
80005798:	00040513          	mv	a0,s0
8000579c:	e75fe0ef          	jal	ra,80004610 <icmp_dest_unreach>
          IP_STATS_INC(ip.proterr);
800057a0:	0404d783          	lhu	a5,64(s1)
800057a4:	00178793          	addi	a5,a5,1
800057a8:	04f49023          	sh	a5,64(s1)
          IP_STATS_INC(ip.drop);
800057ac:	0364d783          	lhu	a5,54(s1)
800057b0:	00178793          	addi	a5,a5,1
800057b4:	02f49b23          	sh	a5,54(s1)
        pbuf_free(p);
800057b8:	00040513          	mv	a0,s0
800057bc:	994fc0ef          	jal	ra,80001950 <pbuf_free>
        break;
800057c0:	f75ff06f          	j	80005734 <ip4_input+0x2e4>

800057c4 <ip4_output_if_src>:
 */
err_t
ip4_output_if_src(struct pbuf *p, const ip4_addr_t *src, const ip4_addr_t *dest,
                  u8_t ttl, u8_t tos,
                  u8_t proto, struct netif *netif)
{
800057c4:	fc010113          	addi	sp,sp,-64
800057c8:	03212823          	sw	s2,48(sp)
800057cc:	03412423          	sw	s4,40(sp)
800057d0:	03512223          	sw	s5,36(sp)
800057d4:	02112e23          	sw	ra,60(sp)
800057d8:	02812c23          	sw	s0,56(sp)
800057dc:	02912a23          	sw	s1,52(sp)
800057e0:	03312623          	sw	s3,44(sp)
800057e4:	03612023          	sw	s6,32(sp)
800057e8:	00d12623          	sw	a3,12(sp)
800057ec:	00f12423          	sw	a5,8(sp)
  LWIP_IP_CHECK_PBUF_REF_COUNT_FOR_TX(p);

  MIB2_STATS_INC(mib2.ipoutrequests);

  /* Should the IP header be generated or is it already included in p? */
  if (dest != LWIP_IP_HDRINCL) {
800057f0:	00c12223          	sw	a2,4(sp)
{
800057f4:	00050913          	mv	s2,a0
800057f8:	00080a13          	mv	s4,a6
800057fc:	8dc18a93          	addi	s5,gp,-1828 # 80000424 <lwip_stats>
  if (dest != LWIP_IP_HDRINCL) {
80005800:	1a060863          	beqz	a2,800059b0 <ip4_output_if_src+0x1ec>
80005804:	00058993          	mv	s3,a1
      }
#endif /* CHECKSUM_GEN_IP_INLINE */
    }
#endif /* IP_OPTIONS_SEND */
    /* generate IP header */
    if (pbuf_add_header(p, IP_HLEN)) {
80005808:	01400593          	li	a1,20
8000580c:	00070493          	mv	s1,a4
80005810:	8a8fc0ef          	jal	ra,800018b8 <pbuf_add_header>
80005814:	00412603          	lw	a2,4(sp)
80005818:	00812783          	lw	a5,8(sp)
8000581c:	00c12683          	lw	a3,12(sp)
80005820:	02050e63          	beqz	a0,8000585c <ip4_output_if_src+0x98>
#endif /* CHECKSUM_GEN_IP_INLINE */
  } else {
    /* IP header already included in p */
    if (p->len < IP_HLEN) {
      LWIP_DEBUGF(IP_DEBUG | LWIP_DBG_LEVEL_SERIOUS, ("ip4_output: LWIP_IP_HDRINCL but pbuf is too short\n"));
      IP_STATS_INC(ip.err);
80005824:	044ad783          	lhu	a5,68(s5)
      MIB2_STATS_INC(mib2.ipoutdiscards);
      return ERR_BUF;
80005828:	ffe00513          	li	a0,-2
      IP_STATS_INC(ip.err);
8000582c:	00178793          	addi	a5,a5,1
80005830:	04fa9223          	sh	a5,68(s5)
  }
#endif /* IP_FRAG */

  LWIP_DEBUGF(IP_DEBUG, ("ip4_output_if: call netif->output()\n"));
  return netif->output(netif, p, dest);
}
80005834:	03c12083          	lw	ra,60(sp)
80005838:	03812403          	lw	s0,56(sp)
8000583c:	03412483          	lw	s1,52(sp)
80005840:	03012903          	lw	s2,48(sp)
80005844:	02c12983          	lw	s3,44(sp)
80005848:	02812a03          	lw	s4,40(sp)
8000584c:	02412a83          	lw	s5,36(sp)
80005850:	02012b03          	lw	s6,32(sp)
80005854:	04010113          	addi	sp,sp,64
80005858:	00008067          	ret
    iphdr = (struct ip_hdr *)p->payload;
8000585c:	00492403          	lw	s0,4(s2)
    ip4_addr_copy(iphdr->dest, *dest);
80005860:	00c12223          	sw	a2,4(sp)
    IPH_TTL_SET(iphdr, ttl);
80005864:	00d40423          	sb	a3,8(s0)
    IPH_PROTO_SET(iphdr, proto);
80005868:	00f404a3          	sb	a5,9(s0)
    chk_sum += PP_NTOHS(proto | (ttl << 8));
8000586c:	00869693          	slli	a3,a3,0x8
80005870:	00f6e7b3          	or	a5,a3,a5
    ip4_addr_copy(iphdr->dest, *dest);
80005874:	00062683          	lw	a3,0(a2)
    IPH_TOS_SET(iphdr, tos);
80005878:	009400a3          	sb	s1,1(s0)
    chk_sum += PP_NTOHS(tos | (iphdr->_v_hl << 8));
8000587c:	00849493          	slli	s1,s1,0x8
    ip4_addr_copy(iphdr->dest, *dest);
80005880:	0086d593          	srli	a1,a3,0x8
80005884:	00d40823          	sb	a3,16(s0)
80005888:	00b408a3          	sb	a1,17(s0)
8000588c:	0186d513          	srli	a0,a3,0x18
80005890:	0106d593          	srli	a1,a3,0x10
    chk_sum += ip4_addr_get_u32(&iphdr->dest) & 0xFFFF;
80005894:	01069693          	slli	a3,a3,0x10
80005898:	0106d693          	srli	a3,a3,0x10
    chk_sum += PP_NTOHS(tos | (iphdr->_v_hl << 8));
8000589c:	00b686b3          	add	a3,a3,a1
    ip4_addr_copy(iphdr->dest, *dest);
800058a0:	00a409a3          	sb	a0,19(s0)
    chk_sum += PP_NTOHS(tos | (iphdr->_v_hl << 8));
800058a4:	0454e493          	ori	s1,s1,69
    IPH_VHL_SET(iphdr, 4, ip_hlen / 4);
800058a8:	04500513          	li	a0,69
    ip4_addr_copy(iphdr->dest, *dest);
800058ac:	00b40923          	sb	a1,18(s0)
    IPH_VHL_SET(iphdr, 4, ip_hlen / 4);
800058b0:	00a40023          	sb	a0,0(s0)
    chk_sum += PP_NTOHS(tos | (iphdr->_v_hl << 8));
800058b4:	00d484b3          	add	s1,s1,a3
    chk_sum += PP_NTOHS(proto | (ttl << 8));
800058b8:	00879693          	slli	a3,a5,0x8
800058bc:	4087d793          	srai	a5,a5,0x8
800058c0:	00f6e7b3          	or	a5,a3,a5
    IPH_LEN_SET(iphdr, lwip_htons(p->tot_len));
800058c4:	00895503          	lhu	a0,8(s2)
    chk_sum += PP_NTOHS(proto | (ttl << 8));
800058c8:	01079793          	slli	a5,a5,0x10
800058cc:	0107d793          	srli	a5,a5,0x10
    chk_sum += PP_NTOHS(tos | (iphdr->_v_hl << 8));
800058d0:	00f484b3          	add	s1,s1,a5
    IPH_LEN_SET(iphdr, lwip_htons(p->tot_len));
800058d4:	814fb0ef          	jal	ra,800008e8 <lwip_htons>
800058d8:	00855793          	srli	a5,a0,0x8
800058dc:	00a40123          	sb	a0,2(s0)
    chk_sum += iphdr->_len;
800058e0:	009504b3          	add	s1,a0,s1
    IPH_ID_SET(iphdr, lwip_htons(ip_id));
800058e4:	8841d503          	lhu	a0,-1916(gp) # 800003cc <ip_id>
    IPH_LEN_SET(iphdr, lwip_htons(p->tot_len));
800058e8:	00f401a3          	sb	a5,3(s0)
    IPH_OFFSET_SET(iphdr, 0);
800058ec:	00040323          	sb	zero,6(s0)
800058f0:	000403a3          	sb	zero,7(s0)
    IPH_ID_SET(iphdr, lwip_htons(ip_id));
800058f4:	ff5fa0ef          	jal	ra,800008e8 <lwip_htons>
800058f8:	00855793          	srli	a5,a0,0x8
800058fc:	00f402a3          	sb	a5,5(s0)
    ++ip_id;
80005900:	8841d783          	lhu	a5,-1916(gp) # 800003cc <ip_id>
    IPH_ID_SET(iphdr, lwip_htons(ip_id));
80005904:	00a40223          	sb	a0,4(s0)
    if (src == NULL) {
80005908:	00412603          	lw	a2,4(sp)
    ++ip_id;
8000590c:	00178793          	addi	a5,a5,1
80005910:	88f19223          	sh	a5,-1916(gp) # 800003cc <ip_id>
    chk_sum += iphdr->_id;
80005914:	00950533          	add	a0,a0,s1
    if (src == NULL) {
80005918:	08099863          	bnez	s3,800059a8 <ip4_output_if_src+0x1e4>
      ip4_addr_copy(iphdr->src, *IP4_ADDR_ANY4);
8000591c:	8581a703          	lw	a4,-1960(gp) # 800003a0 <ip_addr_any>
80005920:	01875693          	srli	a3,a4,0x18
80005924:	00d407a3          	sb	a3,15(s0)
    chk_sum += ip4_addr_get_u32(&iphdr->src) & 0xFFFF;
80005928:	000106b7          	lui	a3,0x10
8000592c:	00875793          	srli	a5,a4,0x8
80005930:	fff68693          	addi	a3,a3,-1 # ffff <_stack_size+0xf7ff>
80005934:	00e40623          	sb	a4,12(s0)
80005938:	00f406a3          	sb	a5,13(s0)
8000593c:	01075793          	srli	a5,a4,0x10
80005940:	00d77733          	and	a4,a4,a3
80005944:	00a70733          	add	a4,a4,a0
    chk_sum += ip4_addr_get_u32(&iphdr->src) >> 16;
80005948:	00e78733          	add	a4,a5,a4
    chk_sum = (chk_sum >> 16) + (chk_sum & 0xFFFF);
8000594c:	00f40723          	sb	a5,14(s0)
80005950:	01075793          	srli	a5,a4,0x10
80005954:	00d77733          	and	a4,a4,a3
80005958:	00e78733          	add	a4,a5,a4
    chk_sum = (chk_sum >> 16) + chk_sum;
8000595c:	01075793          	srli	a5,a4,0x10
80005960:	00e787b3          	add	a5,a5,a4
    chk_sum = ~chk_sum;
80005964:	fff7c793          	not	a5,a5
      iphdr->_chksum = (u16_t)chk_sum; /* network order */
80005968:	01079793          	slli	a5,a5,0x10
8000596c:	0107d793          	srli	a5,a5,0x10
80005970:	00f40523          	sb	a5,10(s0)
80005974:	0087d793          	srli	a5,a5,0x8
80005978:	00f405a3          	sb	a5,11(s0)
  IP_STATS_INC(ip.xmit);
8000597c:	030ad783          	lhu	a5,48(s5)
80005980:	00178793          	addi	a5,a5,1
80005984:	02fa9823          	sh	a5,48(s5)
  if (netif->mtu && (p->tot_len > netif->mtu)) {
80005988:	028a5783          	lhu	a5,40(s4)
8000598c:	06078463          	beqz	a5,800059f4 <ip4_output_if_src+0x230>
80005990:	00895703          	lhu	a4,8(s2)
80005994:	06e7f063          	bgeu	a5,a4,800059f4 <ip4_output_if_src+0x230>
    return ip4_frag(p, netif, dest);
80005998:	000a0593          	mv	a1,s4
8000599c:	00090513          	mv	a0,s2
800059a0:	e90ff0ef          	jal	ra,80005030 <ip4_frag>
800059a4:	e91ff06f          	j	80005834 <ip4_output_if_src+0x70>
      ip4_addr_copy(iphdr->src, *src);
800059a8:	0009a703          	lw	a4,0(s3)
800059ac:	f75ff06f          	j	80005920 <ip4_output_if_src+0x15c>
    if (p->len < IP_HLEN) {
800059b0:	00a55703          	lhu	a4,10(a0)
800059b4:	01300793          	li	a5,19
800059b8:	e6e7f6e3          	bgeu	a5,a4,80005824 <ip4_output_if_src+0x60>
    ip4_addr_copy(dest_addr, iphdr->dest);
800059bc:	00452683          	lw	a3,4(a0)
    dest = &dest_addr;
800059c0:	01c10613          	addi	a2,sp,28
    ip4_addr_copy(dest_addr, iphdr->dest);
800059c4:	0116c703          	lbu	a4,17(a3)
800059c8:	0106c783          	lbu	a5,16(a3)
800059cc:	00871713          	slli	a4,a4,0x8
800059d0:	00f76733          	or	a4,a4,a5
800059d4:	0126c783          	lbu	a5,18(a3)
800059d8:	01079793          	slli	a5,a5,0x10
800059dc:	00e7e733          	or	a4,a5,a4
800059e0:	0136c783          	lbu	a5,19(a3)
800059e4:	01879793          	slli	a5,a5,0x18
800059e8:	00e7e7b3          	or	a5,a5,a4
800059ec:	00f12e23          	sw	a5,28(sp)
    dest = &dest_addr;
800059f0:	f8dff06f          	j	8000597c <ip4_output_if_src+0x1b8>
  return netif->output(netif, p, dest);
800059f4:	014a2783          	lw	a5,20(s4)
800059f8:	00090593          	mv	a1,s2
800059fc:	000a0513          	mv	a0,s4
80005a00:	000780e7          	jalr	a5
80005a04:	e31ff06f          	j	80005834 <ip4_output_if_src+0x70>

80005a08 <ip4_output_if>:
  if (dest != LWIP_IP_HDRINCL) {
80005a08:	00060a63          	beqz	a2,80005a1c <ip4_output_if+0x14>
    if (ip4_addr_isany(src)) {
80005a0c:	00058663          	beqz	a1,80005a18 <ip4_output_if+0x10>
80005a10:	0005a883          	lw	a7,0(a1)
80005a14:	00089463          	bnez	a7,80005a1c <ip4_output_if+0x14>
      src_used = netif_ip4_addr(netif);
80005a18:	00480593          	addi	a1,a6,4
  return ip4_output_if_src(p, src_used, dest, ttl, tos, proto, netif);
80005a1c:	da9ff06f          	j	800057c4 <ip4_output_if_src>

80005a20 <ip4_output>:
 *         see ip_output_if() for more return values
 */
err_t
ip4_output(struct pbuf *p, const ip4_addr_t *src, const ip4_addr_t *dest,
           u8_t ttl, u8_t tos, u8_t proto)
{
80005a20:	fd010113          	addi	sp,sp,-48
80005a24:	02812423          	sw	s0,40(sp)
80005a28:	00050413          	mv	s0,a0
  struct netif *netif;

  LWIP_IP_CHECK_PBUF_REF_COUNT_FOR_TX(p);

  if ((netif = ip4_route_src(src, dest)) == NULL) {
80005a2c:	00060513          	mv	a0,a2
{
80005a30:	00b12e23          	sw	a1,28(sp)
80005a34:	00d12c23          	sw	a3,24(sp)
80005a38:	00e12a23          	sw	a4,20(sp)
80005a3c:	00f12823          	sw	a5,16(sp)
  if ((netif = ip4_route_src(src, dest)) == NULL) {
80005a40:	00c12623          	sw	a2,12(sp)
{
80005a44:	02112623          	sw	ra,44(sp)
  if ((netif = ip4_route_src(src, dest)) == NULL) {
80005a48:	969ff0ef          	jal	ra,800053b0 <ip4_route>
80005a4c:	00c12603          	lw	a2,12(sp)
80005a50:	01012783          	lw	a5,16(sp)
80005a54:	01412703          	lw	a4,20(sp)
80005a58:	01812683          	lw	a3,24(sp)
80005a5c:	01c12583          	lw	a1,28(sp)
80005a60:	00050e63          	beqz	a0,80005a7c <ip4_output+0x5c>
                           ip4_addr1_16(dest), ip4_addr2_16(dest), ip4_addr3_16(dest), ip4_addr4_16(dest)));
    IP_STATS_INC(ip.rterr);
    return ERR_RTE;
  }

  return ip4_output_if(p, src, dest, ttl, tos, proto, netif);
80005a64:	00050813          	mv	a6,a0
80005a68:	00040513          	mv	a0,s0
}
80005a6c:	02812403          	lw	s0,40(sp)
80005a70:	02c12083          	lw	ra,44(sp)
80005a74:	03010113          	addi	sp,sp,48
  return ip4_output_if(p, src, dest, ttl, tos, proto, netif);
80005a78:	f91ff06f          	j	80005a08 <ip4_output_if>
    IP_STATS_INC(ip.rterr);
80005a7c:	8dc18793          	addi	a5,gp,-1828 # 80000424 <lwip_stats>
80005a80:	03e7d703          	lhu	a4,62(a5)
}
80005a84:	ffc00513          	li	a0,-4
    IP_STATS_INC(ip.rterr);
80005a88:	00170713          	addi	a4,a4,1
80005a8c:	02e79f23          	sh	a4,62(a5)
}
80005a90:	02c12083          	lw	ra,44(sp)
80005a94:	02812403          	lw	s0,40(sp)
80005a98:	03010113          	addi	sp,sp,48
80005a9c:	00008067          	ret

80005aa0 <ip4_output_hinted>:
 *         see ip_output_if() for more return values
 */
err_t
ip4_output_hinted(struct pbuf *p, const ip4_addr_t *src, const ip4_addr_t *dest,
                  u8_t ttl, u8_t tos, u8_t proto, struct netif_hint *netif_hint)
{
80005aa0:	fd010113          	addi	sp,sp,-48
80005aa4:	02912223          	sw	s1,36(sp)
80005aa8:	00050493          	mv	s1,a0
  struct netif *netif;
  err_t err;

  LWIP_IP_CHECK_PBUF_REF_COUNT_FOR_TX(p);

  if ((netif = ip4_route_src(src, dest)) == NULL) {
80005aac:	00060513          	mv	a0,a2
{
80005ab0:	00b12e23          	sw	a1,28(sp)
80005ab4:	00d12c23          	sw	a3,24(sp)
80005ab8:	00e12a23          	sw	a4,20(sp)
80005abc:	00f12823          	sw	a5,16(sp)
80005ac0:	01012623          	sw	a6,12(sp)
  if ((netif = ip4_route_src(src, dest)) == NULL) {
80005ac4:	00c12423          	sw	a2,8(sp)
{
80005ac8:	02112623          	sw	ra,44(sp)
80005acc:	02812423          	sw	s0,40(sp)
  if ((netif = ip4_route_src(src, dest)) == NULL) {
80005ad0:	8e1ff0ef          	jal	ra,800053b0 <ip4_route>
80005ad4:	00812603          	lw	a2,8(sp)
80005ad8:	00c12803          	lw	a6,12(sp)
80005adc:	01012783          	lw	a5,16(sp)
80005ae0:	01412703          	lw	a4,20(sp)
80005ae4:	01812683          	lw	a3,24(sp)
80005ae8:	01c12583          	lw	a1,28(sp)
80005aec:	02051663          	bnez	a0,80005b18 <ip4_output_hinted+0x78>
    LWIP_DEBUGF(IP_DEBUG, ("ip4_output: No route to %"U16_F".%"U16_F".%"U16_F".%"U16_F"\n",
                           ip4_addr1_16(dest), ip4_addr2_16(dest), ip4_addr3_16(dest), ip4_addr4_16(dest)));
    IP_STATS_INC(ip.rterr);
80005af0:	8dc18793          	addi	a5,gp,-1828 # 80000424 <lwip_stats>
80005af4:	03e7d703          	lhu	a4,62(a5)
    return ERR_RTE;
80005af8:	ffc00513          	li	a0,-4
    IP_STATS_INC(ip.rterr);
80005afc:	00170713          	addi	a4,a4,1
80005b00:	02e79f23          	sh	a4,62(a5)
  NETIF_SET_HINTS(netif, netif_hint);
  err = ip4_output_if(p, src, dest, ttl, tos, proto, netif);
  NETIF_RESET_HINTS(netif);

  return err;
}
80005b04:	02c12083          	lw	ra,44(sp)
80005b08:	02812403          	lw	s0,40(sp)
80005b0c:	02412483          	lw	s1,36(sp)
80005b10:	03010113          	addi	sp,sp,48
80005b14:	00008067          	ret
  NETIF_SET_HINTS(netif, netif_hint);
80005b18:	03052c23          	sw	a6,56(a0)
80005b1c:	00050413          	mv	s0,a0
  err = ip4_output_if(p, src, dest, ttl, tos, proto, netif);
80005b20:	00050813          	mv	a6,a0
80005b24:	00048513          	mv	a0,s1
80005b28:	ee1ff0ef          	jal	ra,80005a08 <ip4_output_if>
  NETIF_RESET_HINTS(netif);
80005b2c:	02042c23          	sw	zero,56(s0)
  return err;
80005b30:	fd5ff06f          	j	80005b04 <ip4_output_hinted+0x64>

80005b34 <ip4_addr_isbroadcast_u32>:
{
  ip4_addr_t ipaddr;
  ip4_addr_set_u32(&ipaddr, addr);

  /* all ones (broadcast) or all zeroes (old skool broadcast) */
  if ((~addr == IPADDR_ANY) ||
80005b34:	fff50693          	addi	a3,a0,-1
80005b38:	ffd00793          	li	a5,-3
{
80005b3c:	00050713          	mv	a4,a0
  if ((~addr == IPADDR_ANY) ||
80005b40:	04d7e063          	bltu	a5,a3,80005b80 <ip4_addr_isbroadcast_u32+0x4c>
      (addr == IPADDR_ANY)) {
    return 1;
    /* no broadcast support on this network interface? */
  } else if ((netif->flags & NETIF_FLAG_BROADCAST) == 0) {
80005b44:	0315c783          	lbu	a5,49(a1)
80005b48:	0027f513          	andi	a0,a5,2
80005b4c:	02050c63          	beqz	a0,80005b84 <ip4_addr_isbroadcast_u32+0x50>
    /* the given address cannot be a broadcast address
     * nor can we check against any broadcast addresses */
    return 0;
    /* address matches network interface address exactly? => no broadcast */
  } else if (addr == ip4_addr_get_u32(netif_ip4_addr(netif))) {
80005b50:	0045a783          	lw	a5,4(a1)
             && ((addr & ~ip4_addr_get_u32(netif_ip4_netmask(netif))) ==
                 (IPADDR_BROADCAST & ~ip4_addr_get_u32(netif_ip4_netmask(netif))))) {
    /* => network broadcast address */
    return 1;
  } else {
    return 0;
80005b54:	00000513          	li	a0,0
  } else if (addr == ip4_addr_get_u32(netif_ip4_addr(netif))) {
80005b58:	02e78663          	beq	a5,a4,80005b84 <ip4_addr_isbroadcast_u32+0x50>
  } else if (ip4_addr_netcmp(&ipaddr, netif_ip4_addr(netif), netif_ip4_netmask(netif))
80005b5c:	0085a683          	lw	a3,8(a1)
80005b60:	00e7c7b3          	xor	a5,a5,a4
80005b64:	00d7f7b3          	and	a5,a5,a3
80005b68:	00079e63          	bnez	a5,80005b84 <ip4_addr_isbroadcast_u32+0x50>
             && ((addr & ~ip4_addr_get_u32(netif_ip4_netmask(netif))) ==
80005b6c:	fff6c693          	not	a3,a3
80005b70:	00e6f533          	and	a0,a3,a4
80005b74:	40d50533          	sub	a0,a0,a3
    return 0;
80005b78:	00153513          	seqz	a0,a0
80005b7c:	00008067          	ret
    return 1;
80005b80:	00100513          	li	a0,1
  }
}
80005b84:	00008067          	ret

80005b88 <ip4_addr_netmask_valid>:
 * @param netmask the IPv4 netmask to check (in network byte order!)
 * @return 1 if the netmask is valid, 0 if it is not
 */
u8_t
ip4_addr_netmask_valid(u32_t netmask)
{
80005b88:	ff010113          	addi	sp,sp,-16
80005b8c:	00112623          	sw	ra,12(sp)
  u32_t mask;
  u32_t nm_hostorder = lwip_htonl(netmask);
80005b90:	d71fa0ef          	jal	ra,80000900 <lwip_htonl>
80005b94:	02000713          	li	a4,32

  /* first, check for the first zero */
  for (mask = 1UL << 31 ; mask != 0; mask >>= 1) {
80005b98:	800007b7          	lui	a5,0x80000
    if ((nm_hostorder & mask) == 0) {
80005b9c:	00f576b3          	and	a3,a0,a5
80005ba0:	02068663          	beqz	a3,80005bcc <ip4_addr_netmask_valid+0x44>
80005ba4:	fff70713          	addi	a4,a4,-1
  for (mask = 1UL << 31 ; mask != 0; mask >>= 1) {
80005ba8:	0017d793          	srli	a5,a5,0x1
80005bac:	fe0718e3          	bnez	a4,80005b9c <ip4_addr_netmask_valid+0x14>
      /* there is a one after the first zero -> invalid */
      return 0;
    }
  }
  /* no one after the first zero -> valid */
  return 1;
80005bb0:	00100513          	li	a0,1
}
80005bb4:	00c12083          	lw	ra,12(sp)
80005bb8:	01010113          	addi	sp,sp,16
80005bbc:	00008067          	ret
    if ((nm_hostorder & mask) != 0) {
80005bc0:	00a7f733          	and	a4,a5,a0
80005bc4:	00071863          	bnez	a4,80005bd4 <ip4_addr_netmask_valid+0x4c>
  for (; mask != 0; mask >>= 1) {
80005bc8:	0017d793          	srli	a5,a5,0x1
80005bcc:	fe079ae3          	bnez	a5,80005bc0 <ip4_addr_netmask_valid+0x38>
80005bd0:	fe1ff06f          	j	80005bb0 <ip4_addr_netmask_valid+0x28>
      return 0;
80005bd4:	00000513          	li	a0,0
80005bd8:	fddff06f          	j	80005bb4 <ip4_addr_netmask_valid+0x2c>

80005bdc <ip4addr_aton>:
 * @param addr pointer to which to save the ip address in network order
 * @return 1 if cp could be converted to addr, 0 on failure
 */
int
ip4addr_aton(const char *cp, ip4_addr_t *addr)
{
80005bdc:	fb010113          	addi	sp,sp,-80
80005be0:	04812423          	sw	s0,72(sp)
80005be4:	05212023          	sw	s2,64(sp)
80005be8:	03312e23          	sw	s3,60(sp)
80005bec:	03512a23          	sw	s5,52(sp)
80005bf0:	03712623          	sw	s7,44(sp)
80005bf4:	03812423          	sw	s8,40(sp)
80005bf8:	03912223          	sw	s9,36(sp)
80005bfc:	03a12023          	sw	s10,32(sp)
80005c00:	01b12e23          	sw	s11,28(sp)
80005c04:	04112623          	sw	ra,76(sp)
80005c08:	04912223          	sw	s1,68(sp)
80005c0c:	03412c23          	sw	s4,56(sp)
80005c10:	03612823          	sw	s6,48(sp)
  u8_t base;
  char c;
  u32_t parts[4];
  u32_t *pp = parts;

  c = *cp;
80005c14:	00054483          	lbu	s1,0(a0)
  u32_t *pp = parts;
80005c18:	00010913          	mv	s2,sp
    /*
     * Collect number up to ``.''.
     * Values are specified as for C:
     * 0x=hex, 0=octal, 1-9=decimal.
     */
    if (!lwip_isdigit(c)) {
80005c1c:	80000ab7          	lui	s5,0x80000
{
80005c20:	00058993          	mv	s3,a1
80005c24:	00090b93          	mv	s7,s2
    if (!lwip_isdigit(c)) {
80005c28:	1e1a8a93          	addi	s5,s5,481 # 800001e1 <_stack_start+0xffff9451>
      return 0;
    }
    val = 0;
    base = 10;
    if (c == '0') {
80005c2c:	03000413          	li	s0,48
      c = *++cp;
      if (c == 'x' || c == 'X') {
80005c30:	05800c13          	li	s8,88
        base = 16;
80005c34:	01000c93          	li	s9,16
    for (;;) {
      if (lwip_isdigit(c)) {
        val = (val * base) + (u32_t)(c - '0');
        c = *++cp;
      } else if (base == 16 && lwip_isxdigit(c)) {
        val = (val << 4) | (u32_t)(c + 10 - (lwip_islower(c) ? 'a' : 'A'));
80005c38:	00200d13          	li	s10,2
        c = *++cp;
      } else {
        break;
      }
    }
    if (c == '.') {
80005c3c:	02e00d93          	li	s11,46
       * Internet format:
       *  a.b.c.d
       *  a.b.c   (with c treated as 16 bits)
       *  a.b (with b treated as 24 bits)
       */
      if (pp >= parts + 3) {
80005c40:	00c10713          	addi	a4,sp,12
    if (!lwip_isdigit(c)) {
80005c44:	015487b3          	add	a5,s1,s5
80005c48:	0007c783          	lbu	a5,0(a5) # 80000000 <_stack_start+0xffff9270>
80005c4c:	0047f793          	andi	a5,a5,4
80005c50:	1c078a63          	beqz	a5,80005e24 <ip4addr_aton+0x248>
    base = 10;
80005c54:	00a00b13          	li	s6,10
    if (c == '0') {
80005c58:	00849c63          	bne	s1,s0,80005c70 <ip4addr_aton+0x94>
      c = *++cp;
80005c5c:	00154483          	lbu	s1,1(a0)
      if (c == 'x' || c == 'X') {
80005c60:	0df4f793          	andi	a5,s1,223
80005c64:	05878463          	beq	a5,s8,80005cac <ip4addr_aton+0xd0>
      c = *++cp;
80005c68:	00150513          	addi	a0,a0,1
        base = 8;
80005c6c:	00800b13          	li	s6,8
80005c70:	00050a13          	mv	s4,a0
    base = 10;
80005c74:	00000513          	li	a0,0
      if (lwip_isdigit(c)) {
80005c78:	009a87b3          	add	a5,s5,s1
80005c7c:	0007c783          	lbu	a5,0(a5)
80005c80:	0047f693          	andi	a3,a5,4
80005c84:	02068c63          	beqz	a3,80005cbc <ip4addr_aton+0xe0>
        val = (val * base) + (u32_t)(c - '0');
80005c88:	00050593          	mv	a1,a0
80005c8c:	000b0513          	mv	a0,s6
80005c90:	3fc000ef          	jal	ra,8000608c <__mulsi3>
80005c94:	00950533          	add	a0,a0,s1
        c = *++cp;
80005c98:	001a4483          	lbu	s1,1(s4)
        val = (val * base) + (u32_t)(c - '0');
80005c9c:	fd050513          	addi	a0,a0,-48
        c = *++cp;
80005ca0:	00c10713          	addi	a4,sp,12
80005ca4:	001a0a13          	addi	s4,s4,1
80005ca8:	fd1ff06f          	j	80005c78 <ip4addr_aton+0x9c>
        c = *++cp;
80005cac:	00254483          	lbu	s1,2(a0)
        base = 16;
80005cb0:	01000b13          	li	s6,16
        c = *++cp;
80005cb4:	00250513          	addi	a0,a0,2
80005cb8:	fb9ff06f          	j	80005c70 <ip4addr_aton+0x94>
      } else if (base == 16 && lwip_isxdigit(c)) {
80005cbc:	039b1a63          	bne	s6,s9,80005cf0 <ip4addr_aton+0x114>
80005cc0:	0447f693          	andi	a3,a5,68
80005cc4:	02068663          	beqz	a3,80005cf0 <ip4addr_aton+0x114>
        val = (val << 4) | (u32_t)(c + 10 - (lwip_islower(c) ? 'a' : 'A'));
80005cc8:	0037f793          	andi	a5,a5,3
80005ccc:	00451513          	slli	a0,a0,0x4
80005cd0:	00a48493          	addi	s1,s1,10
80005cd4:	06100593          	li	a1,97
80005cd8:	01a78463          	beq	a5,s10,80005ce0 <ip4addr_aton+0x104>
80005cdc:	04100593          	li	a1,65
80005ce0:	40b484b3          	sub	s1,s1,a1
80005ce4:	00a4e533          	or	a0,s1,a0
        c = *++cp;
80005ce8:	001a4483          	lbu	s1,1(s4)
80005cec:	fb9ff06f          	j	80005ca4 <ip4addr_aton+0xc8>
    if (c == '.') {
80005cf0:	01b49e63          	bne	s1,s11,80005d0c <ip4addr_aton+0x130>
      if (pp >= parts + 3) {
80005cf4:	12e90863          	beq	s2,a4,80005e24 <ip4addr_aton+0x248>
        return 0;
      }
      *pp++ = val;
80005cf8:	00490913          	addi	s2,s2,4
80005cfc:	fea92e23          	sw	a0,-4(s2)
      c = *++cp;
80005d00:	001a4483          	lbu	s1,1(s4)
80005d04:	001a0513          	addi	a0,s4,1
    if (!lwip_isdigit(c)) {
80005d08:	f3dff06f          	j	80005c44 <ip4addr_aton+0x68>
    }
  }
  /*
   * Check for trailing characters.
   */
  if (c != '\0' && !lwip_isspace(c)) {
80005d0c:	00048863          	beqz	s1,80005d1c <ip4addr_aton+0x140>
80005d10:	0087f793          	andi	a5,a5,8
      return 0;
80005d14:	00000413          	li	s0,0
  if (c != '\0' && !lwip_isspace(c)) {
80005d18:	02078863          	beqz	a5,80005d48 <ip4addr_aton+0x16c>
  }
  /*
   * Concoct the address according to
   * the number of parts specified.
   */
  switch (pp - parts + 1) {
80005d1c:	41790433          	sub	s0,s2,s7
80005d20:	40245413          	srai	s0,s0,0x2
80005d24:	00140413          	addi	s0,s0,1
80005d28:	00200793          	li	a5,2
80005d2c:	0af40263          	beq	s0,a5,80005dd0 <ip4addr_aton+0x1f4>
80005d30:	0487cc63          	blt	a5,s0,80005d88 <ip4addr_aton+0x1ac>
80005d34:	00040a63          	beqz	s0,80005d48 <ip4addr_aton+0x16c>
      break;
  }
  if (addr) {
    ip4_addr_set_u32(addr, lwip_htonl(val));
  }
  return 1;
80005d38:	00100413          	li	s0,1
  if (addr) {
80005d3c:	00098663          	beqz	s3,80005d48 <ip4addr_aton+0x16c>
    ip4_addr_set_u32(addr, lwip_htonl(val));
80005d40:	bc1fa0ef          	jal	ra,80000900 <lwip_htonl>
80005d44:	00a9a023          	sw	a0,0(s3)
}
80005d48:	00040513          	mv	a0,s0
80005d4c:	04c12083          	lw	ra,76(sp)
80005d50:	04812403          	lw	s0,72(sp)
80005d54:	04412483          	lw	s1,68(sp)
80005d58:	04012903          	lw	s2,64(sp)
80005d5c:	03c12983          	lw	s3,60(sp)
80005d60:	03812a03          	lw	s4,56(sp)
80005d64:	03412a83          	lw	s5,52(sp)
80005d68:	03012b03          	lw	s6,48(sp)
80005d6c:	02c12b83          	lw	s7,44(sp)
80005d70:	02812c03          	lw	s8,40(sp)
80005d74:	02412c83          	lw	s9,36(sp)
80005d78:	02012d03          	lw	s10,32(sp)
80005d7c:	01c12d83          	lw	s11,28(sp)
80005d80:	05010113          	addi	sp,sp,80
80005d84:	00008067          	ret
  switch (pp - parts + 1) {
80005d88:	00300793          	li	a5,3
80005d8c:	06f40463          	beq	s0,a5,80005df4 <ip4addr_aton+0x218>
80005d90:	00400793          	li	a5,4
80005d94:	faf412e3          	bne	s0,a5,80005d38 <ip4addr_aton+0x15c>
      if (val > 0xff) {
80005d98:	0ff00613          	li	a2,255
      return 0;
80005d9c:	00000413          	li	s0,0
      if (val > 0xff) {
80005da0:	faa664e3          	bltu	a2,a0,80005d48 <ip4addr_aton+0x16c>
      if ((parts[0] > 0xff) || (parts[1] > 0xff) || (parts[2] > 0xff)) {
80005da4:	00012783          	lw	a5,0(sp)
80005da8:	faf660e3          	bltu	a2,a5,80005d48 <ip4addr_aton+0x16c>
80005dac:	00412683          	lw	a3,4(sp)
80005db0:	f8d66ce3          	bltu	a2,a3,80005d48 <ip4addr_aton+0x16c>
80005db4:	00812703          	lw	a4,8(sp)
80005db8:	f8e668e3          	bltu	a2,a4,80005d48 <ip4addr_aton+0x16c>
      val |= (parts[0] << 24) | (parts[1] << 16) | (parts[2] << 8);
80005dbc:	01879793          	slli	a5,a5,0x18
80005dc0:	01069693          	slli	a3,a3,0x10
80005dc4:	00d7e7b3          	or	a5,a5,a3
80005dc8:	00871713          	slli	a4,a4,0x8
80005dcc:	0500006f          	j	80005e1c <ip4addr_aton+0x240>
      if (val > 0xffffffUL) {
80005dd0:	010007b7          	lui	a5,0x1000
      return 0;
80005dd4:	00000413          	li	s0,0
      if (val > 0xffffffUL) {
80005dd8:	f6f578e3          	bgeu	a0,a5,80005d48 <ip4addr_aton+0x16c>
      if (parts[0] > 0xff) {
80005ddc:	00012783          	lw	a5,0(sp)
80005de0:	0ff00713          	li	a4,255
80005de4:	f6f762e3          	bltu	a4,a5,80005d48 <ip4addr_aton+0x16c>
      val |= parts[0] << 24;
80005de8:	01879793          	slli	a5,a5,0x18
      val |= (parts[0] << 24) | (parts[1] << 16) | (parts[2] << 8);
80005dec:	00f56533          	or	a0,a0,a5
      break;
80005df0:	f49ff06f          	j	80005d38 <ip4addr_aton+0x15c>
      if (val > 0xffff) {
80005df4:	000107b7          	lui	a5,0x10
      return 0;
80005df8:	00000413          	li	s0,0
      if (val > 0xffff) {
80005dfc:	f4f576e3          	bgeu	a0,a5,80005d48 <ip4addr_aton+0x16c>
      if ((parts[0] > 0xff) || (parts[1] > 0xff)) {
80005e00:	00012783          	lw	a5,0(sp)
80005e04:	0ff00693          	li	a3,255
80005e08:	f4f6e0e3          	bltu	a3,a5,80005d48 <ip4addr_aton+0x16c>
80005e0c:	00412703          	lw	a4,4(sp)
80005e10:	f2e6ece3          	bltu	a3,a4,80005d48 <ip4addr_aton+0x16c>
      val |= (parts[0] << 24) | (parts[1] << 16);
80005e14:	01879793          	slli	a5,a5,0x18
80005e18:	01071713          	slli	a4,a4,0x10
      val |= (parts[0] << 24) | (parts[1] << 16) | (parts[2] << 8);
80005e1c:	00e7e7b3          	or	a5,a5,a4
80005e20:	fcdff06f          	j	80005dec <ip4addr_aton+0x210>
      return 0;
80005e24:	00000413          	li	s0,0
80005e28:	f21ff06f          	j	80005d48 <ip4addr_aton+0x16c>

80005e2c <ipaddr_addr>:
{
80005e2c:	fe010113          	addi	sp,sp,-32
  if (ip4addr_aton(cp, &val)) {
80005e30:	00c10593          	addi	a1,sp,12
{
80005e34:	00112e23          	sw	ra,28(sp)
  if (ip4addr_aton(cp, &val)) {
80005e38:	da5ff0ef          	jal	ra,80005bdc <ip4addr_aton>
80005e3c:	00050a63          	beqz	a0,80005e50 <ipaddr_addr+0x24>
    return ip4_addr_get_u32(&val);
80005e40:	00c12503          	lw	a0,12(sp)
}
80005e44:	01c12083          	lw	ra,28(sp)
80005e48:	02010113          	addi	sp,sp,32
80005e4c:	00008067          	ret
  return (IPADDR_NONE);
80005e50:	fff00513          	li	a0,-1
80005e54:	ff1ff06f          	j	80005e44 <ipaddr_addr+0x18>

80005e58 <ip4addr_ntoa_r>:
 * @return either pointer to buf which now holds the ASCII
 *         representation of addr or NULL if buf was too small
 */
char *
ip4addr_ntoa_r(const ip4_addr_t *addr, char *buf, int buflen)
{
80005e58:	fb010113          	addi	sp,sp,-80
80005e5c:	04912223          	sw	s1,68(sp)
80005e60:	05212023          	sw	s2,64(sp)
80005e64:	03312e23          	sw	s3,60(sp)
80005e68:	03612823          	sw	s6,48(sp)
80005e6c:	03812423          	sw	s8,40(sp)
80005e70:	03912223          	sw	s9,36(sp)
80005e74:	03a12023          	sw	s10,32(sp)
80005e78:	04112623          	sw	ra,76(sp)
80005e7c:	04812423          	sw	s0,72(sp)
80005e80:	03412c23          	sw	s4,56(sp)
80005e84:	03512a23          	sw	s5,52(sp)
80005e88:	03712623          	sw	s7,44(sp)
  u8_t rem;
  u8_t n;
  u8_t i;
  int len = 0;

  s_addr = ip4_addr_get_u32(addr);
80005e8c:	00052783          	lw	a5,0(a0)
{
80005e90:	00058493          	mv	s1,a1

  rp = buf;
80005e94:	00058b13          	mv	s6,a1
  s_addr = ip4_addr_get_u32(addr);
80005e98:	00f12e23          	sw	a5,28(sp)
  int len = 0;
80005e9c:	00000993          	li	s3,0
  ap = (u8_t *)&s_addr;
80005ea0:	01c10913          	addi	s2,sp,28
    i = 0;
    do {
      rem = *ap % (u8_t)10;
      *ap /= (u8_t)10;
      inv[i++] = (char)('0' + rem);
    } while (*ap);
80005ea4:	00900c93          	li	s9,9
    while (i--) {
80005ea8:	0ff00d13          	li	s10,255
      *rp++ = inv[i];
    }
    if (len++ >= buflen) {
      return NULL;
    }
    *rp++ = '.';
80005eac:	02e00c13          	li	s8,46
80005eb0:	00094a03          	lbu	s4,0(s2)
    i = 0;
80005eb4:	00000413          	li	s0,0
      *ap /= (u8_t)10;
80005eb8:	00a00593          	li	a1,10
80005ebc:	000a0513          	mv	a0,s4
80005ec0:	00c12623          	sw	a2,12(sp)
80005ec4:	1f4000ef          	jal	ra,800060b8 <__udivsi3>
80005ec8:	0ff57b93          	andi	s7,a0,255
      rem = *ap % (u8_t)10;
80005ecc:	00a00593          	li	a1,10
80005ed0:	000a0513          	mv	a0,s4
80005ed4:	22c000ef          	jal	ra,80006100 <__umodsi3>
      inv[i++] = (char)('0' + rem);
80005ed8:	02010793          	addi	a5,sp,32
80005edc:	03050513          	addi	a0,a0,48
80005ee0:	008787b3          	add	a5,a5,s0
80005ee4:	00140a93          	addi	s5,s0,1
80005ee8:	fea78c23          	sb	a0,-8(a5) # fff8 <_stack_size+0xf7f8>
80005eec:	0ffafa93          	andi	s5,s5,255
    } while (*ap);
80005ef0:	00c12603          	lw	a2,12(sp)
80005ef4:	074ce863          	bltu	s9,s4,80005f64 <ip4addr_ntoa_r+0x10c>
80005ef8:	01790023          	sb	s7,0(s2)
80005efc:	00098713          	mv	a4,s3
80005f00:	000b0793          	mv	a5,s6
80005f04:	00198993          	addi	s3,s3,1
    while (i--) {
80005f08:	07a41463          	bne	s0,s10,80005f70 <ip4addr_ntoa_r+0x118>
    if (len++ >= buflen) {
80005f0c:	08c75863          	bge	a4,a2,80005f9c <ip4addr_ntoa_r+0x144>
    *rp++ = '.';
80005f10:	01878023          	sb	s8,0(a5)
    ap++;
80005f14:	00190913          	addi	s2,s2,1
  for (n = 0; n < 4; n++) {
80005f18:	02010713          	addi	a4,sp,32
    *rp++ = '.';
80005f1c:	00178b13          	addi	s6,a5,1
  for (n = 0; n < 4; n++) {
80005f20:	f8e918e3          	bne	s2,a4,80005eb0 <ip4addr_ntoa_r+0x58>
  }
  *--rp = 0;
80005f24:	00078023          	sb	zero,0(a5)
  return buf;
80005f28:	00048513          	mv	a0,s1
}
80005f2c:	04c12083          	lw	ra,76(sp)
80005f30:	04812403          	lw	s0,72(sp)
80005f34:	04412483          	lw	s1,68(sp)
80005f38:	04012903          	lw	s2,64(sp)
80005f3c:	03c12983          	lw	s3,60(sp)
80005f40:	03812a03          	lw	s4,56(sp)
80005f44:	03412a83          	lw	s5,52(sp)
80005f48:	03012b03          	lw	s6,48(sp)
80005f4c:	02c12b83          	lw	s7,44(sp)
80005f50:	02812c03          	lw	s8,40(sp)
80005f54:	02412c83          	lw	s9,36(sp)
80005f58:	02012d03          	lw	s10,32(sp)
80005f5c:	05010113          	addi	sp,sp,80
80005f60:	00008067          	ret
      *ap /= (u8_t)10;
80005f64:	000b8a13          	mv	s4,s7
      inv[i++] = (char)('0' + rem);
80005f68:	000a8413          	mv	s0,s5
80005f6c:	f4dff06f          	j	80005eb8 <ip4addr_ntoa_r+0x60>
      if (len++ >= buflen) {
80005f70:	fff40693          	addi	a3,s0,-1
80005f74:	0ff6f693          	andi	a3,a3,255
80005f78:	02c75263          	bge	a4,a2,80005f9c <ip4addr_ntoa_r+0x144>
      *rp++ = inv[i];
80005f7c:	02010713          	addi	a4,sp,32
80005f80:	00870433          	add	s0,a4,s0
80005f84:	ff844703          	lbu	a4,-8(s0)
80005f88:	00178793          	addi	a5,a5,1
80005f8c:	00068413          	mv	s0,a3
80005f90:	fee78fa3          	sb	a4,-1(a5)
80005f94:	00098713          	mv	a4,s3
80005f98:	f6dff06f          	j	80005f04 <ip4addr_ntoa_r+0xac>
        return NULL;
80005f9c:	00000513          	li	a0,0
80005fa0:	f8dff06f          	j	80005f2c <ip4addr_ntoa_r+0xd4>

80005fa4 <ip4addr_ntoa>:
  return ip4addr_ntoa_r(addr, str, IP4ADDR_STRLEN_MAX);
80005fa4:	01000613          	li	a2,16
80005fa8:	a4418593          	addi	a1,gp,-1468 # 8000058c <str.4072>
80005fac:	eadff06f          	j	80005e58 <ip4addr_ntoa_r>

80005fb0 <memset>:
80005fb0:	00f00313          	li	t1,15
80005fb4:	00050713          	mv	a4,a0
80005fb8:	02c37e63          	bgeu	t1,a2,80005ff4 <memset+0x44>
80005fbc:	00f77793          	andi	a5,a4,15
80005fc0:	0a079063          	bnez	a5,80006060 <memset+0xb0>
80005fc4:	08059263          	bnez	a1,80006048 <memset+0x98>
80005fc8:	ff067693          	andi	a3,a2,-16
80005fcc:	00f67613          	andi	a2,a2,15
80005fd0:	00e686b3          	add	a3,a3,a4
80005fd4:	00b72023          	sw	a1,0(a4)
80005fd8:	00b72223          	sw	a1,4(a4)
80005fdc:	00b72423          	sw	a1,8(a4)
80005fe0:	00b72623          	sw	a1,12(a4)
80005fe4:	01070713          	addi	a4,a4,16
80005fe8:	fed766e3          	bltu	a4,a3,80005fd4 <memset+0x24>
80005fec:	00061463          	bnez	a2,80005ff4 <memset+0x44>
80005ff0:	00008067          	ret
80005ff4:	40c306b3          	sub	a3,t1,a2
80005ff8:	00269693          	slli	a3,a3,0x2
80005ffc:	00000297          	auipc	t0,0x0
80006000:	005686b3          	add	a3,a3,t0
80006004:	00c68067          	jr	12(a3)
80006008:	00b70723          	sb	a1,14(a4)
8000600c:	00b706a3          	sb	a1,13(a4)
80006010:	00b70623          	sb	a1,12(a4)
80006014:	00b705a3          	sb	a1,11(a4)
80006018:	00b70523          	sb	a1,10(a4)
8000601c:	00b704a3          	sb	a1,9(a4)
80006020:	00b70423          	sb	a1,8(a4)
80006024:	00b703a3          	sb	a1,7(a4)
80006028:	00b70323          	sb	a1,6(a4)
8000602c:	00b702a3          	sb	a1,5(a4)
80006030:	00b70223          	sb	a1,4(a4)
80006034:	00b701a3          	sb	a1,3(a4)
80006038:	00b70123          	sb	a1,2(a4)
8000603c:	00b700a3          	sb	a1,1(a4)
80006040:	00b70023          	sb	a1,0(a4)
80006044:	00008067          	ret
80006048:	0ff5f593          	andi	a1,a1,255
8000604c:	00859693          	slli	a3,a1,0x8
80006050:	00d5e5b3          	or	a1,a1,a3
80006054:	01059693          	slli	a3,a1,0x10
80006058:	00d5e5b3          	or	a1,a1,a3
8000605c:	f6dff06f          	j	80005fc8 <memset+0x18>
80006060:	00279693          	slli	a3,a5,0x2
80006064:	00000297          	auipc	t0,0x0
80006068:	005686b3          	add	a3,a3,t0
8000606c:	00008293          	mv	t0,ra
80006070:	fa0680e7          	jalr	-96(a3)
80006074:	00028093          	mv	ra,t0
80006078:	ff078793          	addi	a5,a5,-16
8000607c:	40f70733          	sub	a4,a4,a5
80006080:	00f60633          	add	a2,a2,a5
80006084:	f6c378e3          	bgeu	t1,a2,80005ff4 <memset+0x44>
80006088:	f3dff06f          	j	80005fc4 <memset+0x14>

8000608c <__mulsi3>:
8000608c:	00050613          	mv	a2,a0
80006090:	00000513          	li	a0,0
80006094:	0015f693          	andi	a3,a1,1
80006098:	00068463          	beqz	a3,800060a0 <__mulsi3+0x14>
8000609c:	00c50533          	add	a0,a0,a2
800060a0:	0015d593          	srli	a1,a1,0x1
800060a4:	00161613          	slli	a2,a2,0x1
800060a8:	fe0596e3          	bnez	a1,80006094 <__mulsi3+0x8>
800060ac:	00008067          	ret

800060b0 <__divsi3>:
800060b0:	06054063          	bltz	a0,80006110 <__umodsi3+0x10>
800060b4:	0605c663          	bltz	a1,80006120 <__umodsi3+0x20>

800060b8 <__udivsi3>:
800060b8:	00058613          	mv	a2,a1
800060bc:	00050593          	mv	a1,a0
800060c0:	fff00513          	li	a0,-1
800060c4:	02060c63          	beqz	a2,800060fc <__udivsi3+0x44>
800060c8:	00100693          	li	a3,1
800060cc:	00b67a63          	bgeu	a2,a1,800060e0 <__udivsi3+0x28>
800060d0:	00c05863          	blez	a2,800060e0 <__udivsi3+0x28>
800060d4:	00161613          	slli	a2,a2,0x1
800060d8:	00169693          	slli	a3,a3,0x1
800060dc:	feb66ae3          	bltu	a2,a1,800060d0 <__udivsi3+0x18>
800060e0:	00000513          	li	a0,0
800060e4:	00c5e663          	bltu	a1,a2,800060f0 <__udivsi3+0x38>
800060e8:	40c585b3          	sub	a1,a1,a2
800060ec:	00d56533          	or	a0,a0,a3
800060f0:	0016d693          	srli	a3,a3,0x1
800060f4:	00165613          	srli	a2,a2,0x1
800060f8:	fe0696e3          	bnez	a3,800060e4 <__udivsi3+0x2c>
800060fc:	00008067          	ret

80006100 <__umodsi3>:
80006100:	00008293          	mv	t0,ra
80006104:	fb5ff0ef          	jal	ra,800060b8 <__udivsi3>
80006108:	00058513          	mv	a0,a1
8000610c:	00028067          	jr	t0 # 80006064 <memset+0xb4>
80006110:	40a00533          	neg	a0,a0
80006114:	0005d863          	bgez	a1,80006124 <__umodsi3+0x24>
80006118:	40b005b3          	neg	a1,a1
8000611c:	f9dff06f          	j	800060b8 <__udivsi3>
80006120:	40b005b3          	neg	a1,a1
80006124:	00008293          	mv	t0,ra
80006128:	f91ff0ef          	jal	ra,800060b8 <__udivsi3>
8000612c:	40a00533          	neg	a0,a0
80006130:	00028067          	jr	t0

80006134 <__modsi3>:
80006134:	00008293          	mv	t0,ra
80006138:	0005ca63          	bltz	a1,8000614c <__modsi3+0x18>
8000613c:	00054c63          	bltz	a0,80006154 <__modsi3+0x20>
80006140:	f79ff0ef          	jal	ra,800060b8 <__udivsi3>
80006144:	00058513          	mv	a0,a1
80006148:	00028067          	jr	t0
8000614c:	40b005b3          	neg	a1,a1
80006150:	fe0558e3          	bgez	a0,80006140 <__modsi3+0xc>
80006154:	40a00533          	neg	a0,a0
80006158:	f61ff0ef          	jal	ra,800060b8 <__udivsi3>
8000615c:	40b00533          	neg	a0,a1
80006160:	00028067          	jr	t0

Disassembly of section .text.startup:

80006164 <main>:

//
// Main entry point

int main(void)
{
80006164:	ff010113          	addi	sp,sp,-16
80006168:	00112623          	sw	ra,12(sp)
    //mch_timestamp_init();       // Initialize timestamp generator
    mch_net_init();
8000616c:	c54fa0ef          	jal	ra,800005c0 <mch_net_init>
}
80006170:	0000006f          	j	80006170 <main+0xc>

Disassembly of section .text.atoi:

80006174 <atoi>:
80006174:	00a00613          	li	a2,10
80006178:	00000593          	li	a1,0
8000617c:	3f80006f          	j	80006574 <strtol>

Disassembly of section .text._atoi_r:

80006180 <_atoi_r>:
80006180:	00a00693          	li	a3,10
80006184:	00000613          	li	a2,0
80006188:	3cc0006f          	j	80006554 <_strtol_r>

Disassembly of section .text.memcmp:

8000618c <memcmp>:
8000618c:	00000713          	li	a4,0
80006190:	00e61663          	bne	a2,a4,8000619c <memcmp+0x10>
80006194:	00000513          	li	a0,0
80006198:	00008067          	ret
8000619c:	00e507b3          	add	a5,a0,a4
800061a0:	00170713          	addi	a4,a4,1
800061a4:	00e586b3          	add	a3,a1,a4
800061a8:	0007c783          	lbu	a5,0(a5)
800061ac:	fff6c683          	lbu	a3,-1(a3)
800061b0:	fed780e3          	beq	a5,a3,80006190 <memcmp+0x4>
800061b4:	40d78533          	sub	a0,a5,a3
800061b8:	00008067          	ret

Disassembly of section .text.memcpy:

800061bc <memcpy>:
800061bc:	00a5c7b3          	xor	a5,a1,a0
800061c0:	0037f793          	andi	a5,a5,3
800061c4:	00c50733          	add	a4,a0,a2
800061c8:	00079663          	bnez	a5,800061d4 <memcpy+0x18>
800061cc:	00300793          	li	a5,3
800061d0:	02c7e263          	bltu	a5,a2,800061f4 <memcpy+0x38>
800061d4:	00050793          	mv	a5,a0
800061d8:	0ae57c63          	bgeu	a0,a4,80006290 <memcpy+0xd4>
800061dc:	0005c683          	lbu	a3,0(a1)
800061e0:	00178793          	addi	a5,a5,1
800061e4:	00158593          	addi	a1,a1,1
800061e8:	fed78fa3          	sb	a3,-1(a5)
800061ec:	fee7e8e3          	bltu	a5,a4,800061dc <memcpy+0x20>
800061f0:	00008067          	ret
800061f4:	00357693          	andi	a3,a0,3
800061f8:	00050793          	mv	a5,a0
800061fc:	00068e63          	beqz	a3,80006218 <memcpy+0x5c>
80006200:	0005c683          	lbu	a3,0(a1)
80006204:	00178793          	addi	a5,a5,1
80006208:	00158593          	addi	a1,a1,1
8000620c:	fed78fa3          	sb	a3,-1(a5)
80006210:	0037f693          	andi	a3,a5,3
80006214:	fe9ff06f          	j	800061fc <memcpy+0x40>
80006218:	ffc77693          	andi	a3,a4,-4
8000621c:	fe068613          	addi	a2,a3,-32
80006220:	06c7f463          	bgeu	a5,a2,80006288 <memcpy+0xcc>
80006224:	0005a383          	lw	t2,0(a1)
80006228:	0045a283          	lw	t0,4(a1)
8000622c:	0085af83          	lw	t6,8(a1)
80006230:	00c5af03          	lw	t5,12(a1)
80006234:	0105ae83          	lw	t4,16(a1)
80006238:	0145ae03          	lw	t3,20(a1)
8000623c:	0185a303          	lw	t1,24(a1)
80006240:	01c5a883          	lw	a7,28(a1)
80006244:	02458593          	addi	a1,a1,36
80006248:	0077a023          	sw	t2,0(a5)
8000624c:	ffc5a803          	lw	a6,-4(a1)
80006250:	0057a223          	sw	t0,4(a5)
80006254:	01f7a423          	sw	t6,8(a5)
80006258:	01e7a623          	sw	t5,12(a5)
8000625c:	01d7a823          	sw	t4,16(a5)
80006260:	01c7aa23          	sw	t3,20(a5)
80006264:	0067ac23          	sw	t1,24(a5)
80006268:	0117ae23          	sw	a7,28(a5)
8000626c:	02478793          	addi	a5,a5,36
80006270:	ff07ae23          	sw	a6,-4(a5)
80006274:	fadff06f          	j	80006220 <memcpy+0x64>
80006278:	0005a603          	lw	a2,0(a1)
8000627c:	00478793          	addi	a5,a5,4
80006280:	00458593          	addi	a1,a1,4
80006284:	fec7ae23          	sw	a2,-4(a5)
80006288:	fed7e8e3          	bltu	a5,a3,80006278 <memcpy+0xbc>
8000628c:	f4e7e8e3          	bltu	a5,a4,800061dc <memcpy+0x20>
80006290:	00008067          	ret

Disassembly of section .text.memmove:

80006294 <memmove>:
80006294:	04a5fa63          	bgeu	a1,a0,800062e8 <memmove+0x54>
80006298:	00c586b3          	add	a3,a1,a2
8000629c:	04d57663          	bgeu	a0,a3,800062e8 <memmove+0x54>
800062a0:	fff64593          	not	a1,a2
800062a4:	00000793          	li	a5,0
800062a8:	fff78793          	addi	a5,a5,-1
800062ac:	00f59463          	bne	a1,a5,800062b4 <memmove+0x20>
800062b0:	00008067          	ret
800062b4:	00f68733          	add	a4,a3,a5
800062b8:	00074803          	lbu	a6,0(a4)
800062bc:	00c78733          	add	a4,a5,a2
800062c0:	00e50733          	add	a4,a0,a4
800062c4:	01070023          	sb	a6,0(a4)
800062c8:	fe1ff06f          	j	800062a8 <memmove+0x14>
800062cc:	00f58733          	add	a4,a1,a5
800062d0:	00074683          	lbu	a3,0(a4)
800062d4:	00f50733          	add	a4,a0,a5
800062d8:	00178793          	addi	a5,a5,1
800062dc:	00d70023          	sb	a3,0(a4)
800062e0:	fef616e3          	bne	a2,a5,800062cc <memmove+0x38>
800062e4:	00008067          	ret
800062e8:	00000793          	li	a5,0
800062ec:	ff5ff06f          	j	800062e0 <memmove+0x4c>

Disassembly of section .text.strlen:

800062f0 <strlen>:
800062f0:	00050793          	mv	a5,a0
800062f4:	00178793          	addi	a5,a5,1
800062f8:	fff7c703          	lbu	a4,-1(a5)
800062fc:	fe071ce3          	bnez	a4,800062f4 <strlen+0x4>
80006300:	40a78533          	sub	a0,a5,a0
80006304:	fff50513          	addi	a0,a0,-1
80006308:	00008067          	ret

Disassembly of section .text.strncmp:

8000630c <strncmp>:
8000630c:	02060a63          	beqz	a2,80006340 <strncmp+0x34>
80006310:	fff60613          	addi	a2,a2,-1
80006314:	00000713          	li	a4,0
80006318:	00e507b3          	add	a5,a0,a4
8000631c:	00e586b3          	add	a3,a1,a4
80006320:	0007c783          	lbu	a5,0(a5)
80006324:	0006c683          	lbu	a3,0(a3)
80006328:	00d79863          	bne	a5,a3,80006338 <strncmp+0x2c>
8000632c:	00c70663          	beq	a4,a2,80006338 <strncmp+0x2c>
80006330:	00170713          	addi	a4,a4,1
80006334:	fe0792e3          	bnez	a5,80006318 <strncmp+0xc>
80006338:	40d78533          	sub	a0,a5,a3
8000633c:	00008067          	ret
80006340:	00000513          	li	a0,0
80006344:	00008067          	ret

Disassembly of section .text._strtol_l.isra.0:

80006348 <_strtol_l.isra.0>:
80006348:	fc010113          	addi	sp,sp,-64
8000634c:	03212823          	sw	s2,48(sp)
80006350:	03512223          	sw	s5,36(sp)
80006354:	03612023          	sw	s6,32(sp)
80006358:	01812c23          	sw	s8,24(sp)
8000635c:	00068913          	mv	s2,a3
80006360:	02112e23          	sw	ra,60(sp)
80006364:	02812c23          	sw	s0,56(sp)
80006368:	02912a23          	sw	s1,52(sp)
8000636c:	03312623          	sw	s3,44(sp)
80006370:	03412423          	sw	s4,40(sp)
80006374:	01712e23          	sw	s7,28(sp)
80006378:	01912a23          	sw	s9,20(sp)
8000637c:	01a12823          	sw	s10,16(sp)
80006380:	01b12623          	sw	s11,12(sp)
80006384:	00050a93          	mv	s5,a0
80006388:	00058b13          	mv	s6,a1
8000638c:	00060c13          	mv	s8,a2
80006390:	00058793          	mv	a5,a1
80006394:	ffffa697          	auipc	a3,0xffffa
80006398:	e4d68693          	addi	a3,a3,-435 # 800001e1 <_ctype_+0x1>
8000639c:	00178493          	addi	s1,a5,1
800063a0:	fff4c403          	lbu	s0,-1(s1)
800063a4:	00d40733          	add	a4,s0,a3
800063a8:	00074703          	lbu	a4,0(a4)
800063ac:	00877713          	andi	a4,a4,8
800063b0:	0c071863          	bnez	a4,80006480 <_strtol_l.isra.0+0x138>
800063b4:	02d00713          	li	a4,45
800063b8:	0ce41863          	bne	s0,a4,80006488 <_strtol_l.isra.0+0x140>
800063bc:	0004c403          	lbu	s0,0(s1)
800063c0:	00100a13          	li	s4,1
800063c4:	00278493          	addi	s1,a5,2
800063c8:	16090e63          	beqz	s2,80006544 <_strtol_l.isra.0+0x1fc>
800063cc:	01000793          	li	a5,16
800063d0:	02f91463          	bne	s2,a5,800063f8 <_strtol_l.isra.0+0xb0>
800063d4:	03000793          	li	a5,48
800063d8:	02f41063          	bne	s0,a5,800063f8 <_strtol_l.isra.0+0xb0>
800063dc:	0004c783          	lbu	a5,0(s1)
800063e0:	05800713          	li	a4,88
800063e4:	0df7f793          	andi	a5,a5,223
800063e8:	14e79663          	bne	a5,a4,80006534 <_strtol_l.isra.0+0x1ec>
800063ec:	0014c403          	lbu	s0,1(s1)
800063f0:	01000913          	li	s2,16
800063f4:	00248493          	addi	s1,s1,2
800063f8:	800009b7          	lui	s3,0x80000
800063fc:	000a1463          	bnez	s4,80006404 <_strtol_l.isra.0+0xbc>
80006400:	fff9c993          	not	s3,s3
80006404:	00090593          	mv	a1,s2
80006408:	00098513          	mv	a0,s3
8000640c:	cf5ff0ef          	jal	ra,80006100 <__umodsi3>
80006410:	00050b93          	mv	s7,a0
80006414:	00090593          	mv	a1,s2
80006418:	00098513          	mv	a0,s3
8000641c:	c9dff0ef          	jal	ra,800060b8 <__udivsi3>
80006420:	00050c93          	mv	s9,a0
80006424:	00000713          	li	a4,0
80006428:	00000513          	li	a0,0
8000642c:	00900d93          	li	s11,9
80006430:	01900d13          	li	s10,25
80006434:	fff00693          	li	a3,-1
80006438:	fd040793          	addi	a5,s0,-48
8000643c:	06fde463          	bltu	s11,a5,800064a4 <_strtol_l.isra.0+0x15c>
80006440:	00078413          	mv	s0,a5
80006444:	09245063          	bge	s0,s2,800064c4 <_strtol_l.isra.0+0x17c>
80006448:	02d70663          	beq	a4,a3,80006474 <_strtol_l.isra.0+0x12c>
8000644c:	fff00713          	li	a4,-1
80006450:	02ace263          	bltu	s9,a0,80006474 <_strtol_l.isra.0+0x12c>
80006454:	00ac9463          	bne	s9,a0,8000645c <_strtol_l.isra.0+0x114>
80006458:	008bce63          	blt	s7,s0,80006474 <_strtol_l.isra.0+0x12c>
8000645c:	00050593          	mv	a1,a0
80006460:	00090513          	mv	a0,s2
80006464:	c29ff0ef          	jal	ra,8000608c <__mulsi3>
80006468:	00a40533          	add	a0,s0,a0
8000646c:	00100713          	li	a4,1
80006470:	fff00693          	li	a3,-1
80006474:	00148493          	addi	s1,s1,1
80006478:	fff4c403          	lbu	s0,-1(s1)
8000647c:	fbdff06f          	j	80006438 <_strtol_l.isra.0+0xf0>
80006480:	00048793          	mv	a5,s1
80006484:	f19ff06f          	j	8000639c <_strtol_l.isra.0+0x54>
80006488:	02b00713          	li	a4,43
8000648c:	00e40663          	beq	s0,a4,80006498 <_strtol_l.isra.0+0x150>
80006490:	00000a13          	li	s4,0
80006494:	f35ff06f          	j	800063c8 <_strtol_l.isra.0+0x80>
80006498:	0004c403          	lbu	s0,0(s1)
8000649c:	00278493          	addi	s1,a5,2
800064a0:	ff1ff06f          	j	80006490 <_strtol_l.isra.0+0x148>
800064a4:	fbf40793          	addi	a5,s0,-65
800064a8:	00fd6663          	bltu	s10,a5,800064b4 <_strtol_l.isra.0+0x16c>
800064ac:	fc940413          	addi	s0,s0,-55
800064b0:	f95ff06f          	j	80006444 <_strtol_l.isra.0+0xfc>
800064b4:	f9f40793          	addi	a5,s0,-97
800064b8:	00fd6663          	bltu	s10,a5,800064c4 <_strtol_l.isra.0+0x17c>
800064bc:	fa940413          	addi	s0,s0,-87
800064c0:	f85ff06f          	j	80006444 <_strtol_l.isra.0+0xfc>
800064c4:	fff00793          	li	a5,-1
800064c8:	04f71863          	bne	a4,a5,80006518 <_strtol_l.isra.0+0x1d0>
800064cc:	02200793          	li	a5,34
800064d0:	00faa023          	sw	a5,0(s5)
800064d4:	00098513          	mv	a0,s3
800064d8:	040c1863          	bnez	s8,80006528 <_strtol_l.isra.0+0x1e0>
800064dc:	03c12083          	lw	ra,60(sp)
800064e0:	03812403          	lw	s0,56(sp)
800064e4:	03412483          	lw	s1,52(sp)
800064e8:	03012903          	lw	s2,48(sp)
800064ec:	02c12983          	lw	s3,44(sp)
800064f0:	02812a03          	lw	s4,40(sp)
800064f4:	02412a83          	lw	s5,36(sp)
800064f8:	02012b03          	lw	s6,32(sp)
800064fc:	01c12b83          	lw	s7,28(sp)
80006500:	01812c03          	lw	s8,24(sp)
80006504:	01412c83          	lw	s9,20(sp)
80006508:	01012d03          	lw	s10,16(sp)
8000650c:	00c12d83          	lw	s11,12(sp)
80006510:	04010113          	addi	sp,sp,64
80006514:	00008067          	ret
80006518:	000a0463          	beqz	s4,80006520 <_strtol_l.isra.0+0x1d8>
8000651c:	40a00533          	neg	a0,a0
80006520:	fa0c0ee3          	beqz	s8,800064dc <_strtol_l.isra.0+0x194>
80006524:	00070463          	beqz	a4,8000652c <_strtol_l.isra.0+0x1e4>
80006528:	fff48b13          	addi	s6,s1,-1
8000652c:	016c2023          	sw	s6,0(s8)
80006530:	fadff06f          	j	800064dc <_strtol_l.isra.0+0x194>
80006534:	03000413          	li	s0,48
80006538:	ec0910e3          	bnez	s2,800063f8 <_strtol_l.isra.0+0xb0>
8000653c:	00800913          	li	s2,8
80006540:	eb9ff06f          	j	800063f8 <_strtol_l.isra.0+0xb0>
80006544:	03000793          	li	a5,48
80006548:	e8f40ae3          	beq	s0,a5,800063dc <_strtol_l.isra.0+0x94>
8000654c:	00a00913          	li	s2,10
80006550:	ea9ff06f          	j	800063f8 <_strtol_l.isra.0+0xb0>

Disassembly of section .text._strtol_r:

80006554 <_strtol_r>:
80006554:	df5ff06f          	j	80006348 <_strtol_l.isra.0>

Disassembly of section .text.strtol_l:

80006558 <strtol_l>:
80006558:	ffffa797          	auipc	a5,0xffffa
8000655c:	df478793          	addi	a5,a5,-524 # 8000034c <_impure_ptr>
80006560:	00060693          	mv	a3,a2
80006564:	00058613          	mv	a2,a1
80006568:	00050593          	mv	a1,a0
8000656c:	0007a503          	lw	a0,0(a5)
80006570:	dd9ff06f          	j	80006348 <_strtol_l.isra.0>

Disassembly of section .text.strtol:

80006574 <strtol>:
80006574:	ffffa797          	auipc	a5,0xffffa
80006578:	dd878793          	addi	a5,a5,-552 # 8000034c <_impure_ptr>
8000657c:	00060693          	mv	a3,a2
80006580:	00058613          	mv	a2,a1
80006584:	00050593          	mv	a1,a0
80006588:	0007a503          	lw	a0,0(a5)
8000658c:	dbdff06f          	j	80006348 <_strtol_l.isra.0>
