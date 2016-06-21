/**
 * @file
 * Ethernet Interface Skeleton
 *
 */

/*
 * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
 * All rights reserved. 
 * 
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED 
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 * 
 * Author: Adam Dunkels <adam@sics.se>
 *
 */

/*
 * This file is a skeleton for developing Ethernet network interface
 * drivers for lwIP. Add code to the low_level functions and do a
 * search-and-replace for the word "ethernetif" to replace it with
 * something that better describes your network interface.
 */

#include "opt.h"

//#if 0 /* don't build, this is only a skeleton, see previous comment */

#include "def.h"
#include "mem.h"
#include "pbuf.h"
#include <stats.h>
#include <snmp.h>
#include "etharp.h"
#include "ppp_oe.h"
#include "board_info.h"
#include "veth_common.h"     //Gain the current 
#include "veth_hal_init.h"
#include "veth_eth_ap.h"
#include "ip.h"
#include "veth_RSTP.h"
/* Define those to better describe your network interface. */
#define IFNAME0 'e'
#define IFNAME1 'n'
#define BUFSIZE  4096
#define LINK_SPEED_OF_YOUR_NETIF_IN_BPS  100 * 1000 * 1000
static int netif_num = 0;   //记录添加的netif的数量
uint8_t rstp_buf[4096];
/**
 * Helper struct to hold private data used to operate your ethernet interface.
 * Keeping the ethernet address of the MAC in this struct is not necessary
 * as it is already kept in the struct netif.
 * But this is only an example, anyway...
 */
struct ethernetif {
  struct eth_addr *ethaddr;
  /* Add whatever per-interface state that is needed here. */
};

/* Forward declarations. */
//void  ethernetif_input(struct netif *netif);

/**
 * In this function, the hardware should be initialized.
 * Called from ethernetif_init().
 *
 * @param netif the already initialized lwip network interface structure
 *        for this ethernetif
 */
static void
low_level_init(struct netif *netif)
{
  struct ethernetif *ethernetif = netif->state;
  
  /* set MAC hardware address length */
  netif->hwaddr_len = ETHARP_HWADDR_LEN;

  /* set MAC hardware address */
  netif->hwaddr[0] = board.sets_info.The_MAC[0];
  netif->hwaddr[1] = board.sets_info.The_MAC[1];
  netif->hwaddr[2] = board.sets_info.The_MAC[2];
  netif->hwaddr[3] = board.sets_info.The_MAC[3];
  netif->hwaddr[4] = board.sets_info.The_MAC[4];
  //if(netif->num < ports_num){
  //netif->hwaddr[5] = board.sets_info.The_MAC[5] + netif->num + 1;
  //}else{
  netif->hwaddr[5] = board.sets_info.The_MAC[5];
  //}
  

  /* maximum transfer unit */
  netif->mtu = 1500;
  
  /* device capabilities */
  /* don't set NETIF_FLAG_ETHARP if this device is not an ethernet one */
//  if(netif->num < ports_num){
//	netif->flags = NETIF_FLAG_BROADCAST;
//  }else{
	netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP | NETIF_FLAG_IGMP;
	uip_MAC_Init(board.sets_info.The_MAC);
//  }
  
  /* Do whatever else is needed to initialize interface. */  
}

/**
 * This function should do the actual transmission of the packet. The packet is
 * contained in the pbuf that is passed to the function. This pbuf
 * might be chained.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @param p the MAC packet to send (e.g. IP packet including MAC addresses and type)
 * @return ERR_OK if the packet could be sent
 *         an err_t value if the packet couldn't be sent
 *
 * @note Returning ERR_MEM here if a DMA queue of your MAC is full can lead to
 *       strange results. You might consider waiting for space in the DMA queue
 *       to become availale since the stack doesn't retry to send a packet
 *       dropped because of memory failure (except for the TCP timers).
 */

static err_t
low_level_output(struct netif *netif, struct pbuf *p)
{
	
  struct ethernetif *ethernetif = netif->state;
  struct pbuf *q;
  
  uint8_t output_buf[2000];  //定义数据发送的数组
  uint32_t loc = 0;
  uint32_t data_len = 0;  //发送数据的总长度
  //initiate transfer();
  //printf("will exc low level output\n\r");
#if ETH_PAD_SIZE
  pbuf_header(p, -ETH_PAD_SIZE); /* drop the padding word */
#endif
  memset(output_buf,0,sizeof(output_buf));
  for(q = p; q != NULL; q = q->next) {
    /* Send the data from the pbuf to the interface, one pbuf at a
       time. The size of the data in each pbuf is kept in the ->len
       variable. */
	   
	   memcpy(&output_buf[loc],q->payload,q->len);  //拷贝pbuf中的数据到发送数据的缓冲区中
	   data_len += q->len;
	   //data_len = q->len;
	   loc += q->len;
	  // pbuf_free(q);  //释放空间
    //send data from(q->payload, q->len);
	
  }
	//printf("send packet via uip_tx_packet \n\r");
	uip_Tx_Packet(output_buf,data_len);
	//printf("data send end via low level;length is %d\n\r",data_len);
  //signal that packet should be sent();
	//uip_Tx_Packet(output_buf,data_len);//发送数据到网络
#if ETH_PAD_SIZE
  pbuf_header(p, ETH_PAD_SIZE); /* reclaim the padding word */
#endif
  
  LINK_STATS_INC(link.xmit);

  return ERR_OK;
}

/**
 * Should allocate a pbuf and transfer the bytes of the incoming
 * packet from the interface into the pbuf.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return a pbuf filled with the received packet (including MAC header)
 *         NULL on memory error
 */
static struct pbuf *
low_level_input(struct netif *netif)
{
  struct ethernetif *ethernetif = netif->state;
  struct pbuf *p, *q;
  u16_t len;
  u16_t loop;
  uint8_t input_buf[BUFSIZE];  //定义数据接收数组
  volatile uint8_t loc = 0;
  /* Obtain the size of the packet and put it into the "len"
     variable. */
	 memset(input_buf,0,sizeof(input_buf));
	 /*读网络数据到输入缓冲区*/
  //printf();
  len = uip_Rx_Packet(input_buf,BUFSIZE,UIP_RX_TIMEOUT);
//  printf("read data over\n\r");
//  if(len <= 0){
//	return NULL;
//  }
  //printf("Recieve length is not zero\n\r");
  if( len > 0 ){
	if( !Str_cmp(MGMT_MAC,input_buf,6) ) {
	  bp_targ = 1;
//	  printf("BPDU packets:\n\r");
//	  for(loop=0 ;loop< 30 ;loop++ )
//	  {
//		printf("%02x  ",input_buf[loop]);
//	  }
//	  printf("\n\r");
	  len=0;
    }else{
//      printf("Recieved packets:\n\r");
//	  for(loop=0 ;loop< 30 ;loop++ )
//	  {
//	    printf("%02x  ",input_buf[loop]);
//	  }
//	  printf("\n\r");
    }
  }
  //printf("serialmode=%d\n\r",board.serial_mode.com1_2_mode);
  RSTP(&rstp_pt,input_buf,&bp_targ);
  if(len <= 0){
	return NULL;
  }
//  printf("RSTP processor finished\n\r");
#if ETH_PAD_SIZE
  len += ETH_PAD_SIZE; /* allow room for Ethernet padding */
#endif

  /* We allocate a pbuf chain of pbufs from the pool. */
  //printf("will allocate memory for reading data\n\r");
  p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);
  //printf("memory malloc sucessful\n\r");
  if (p != NULL) {

#if ETH_PAD_SIZE
    pbuf_header(p, -ETH_PAD_SIZE); /* drop the padding word */
#endif

    /* We iterate over the pbuf chain until we have read the entire
     * packet into the pbuf. */
    for(q = p; q != NULL; q = q->next) {
      /* Read enough bytes to fill this pbuf in the chain. The
       * available data in the pbuf is given by the q->len
       * variable.
       * This does not necessarily have to be a memcpy, you can also preallocate
       * pbufs for a DMA-enabled MAC and after receiving truncate it to the
       * actually received size. In this case, ensure the tot_len member of the
       * pbuf is the sum of the chained pbuf len members.
       */
      memcpy(q->payload,&input_buf[loc],q->len);
	  loc += q->len;
	  //read data into(q->payload, q->len);
    }
    //acknowledge that packet has been read();
	//printf("read packet end loc is %d;len is %d",loc,len);
#if ETH_PAD_SIZE
    pbuf_header(p, ETH_PAD_SIZE); /* reclaim the padding word */
#endif

    LINK_STATS_INC(link.recv);
  } else {
    //drop packet();
	//printf("memory allocate failed\n\r");
    LINK_STATS_INC(link.memerr);
    LINK_STATS_INC(link.drop);
	return NULL;
  }

  return p;  
}

/**
 * This function should be called when a packet is ready to be read
 * from the interface. It uses the function low_level_input() that
 * should handle the actual reception of bytes from the network
 * interface. Then the type of the received packet is determined and
 * the appropriate input function is called.
 *
 * @param netif the lwip network interface structure for this ethernetif
 */
void
ethernetif_input(struct netif *netif)
{
  struct ethernetif *ethernetif;
  struct eth_hdr *ethhdr;
  struct pbuf *p;

  ethernetif = netif->state;

  /* move received packet into a new pbuf */
  p = low_level_input(netif);
  /* no packet could be read, silently ignore this */
  //printf("low input end\n\r");
  if (p == NULL) return;
  /* points to packet payload, which starts with an Ethernet header */
  ethhdr = p->payload;
  //printf("ether input in\n\r");
  ethernet_input(p,netif);
 #if 0
  switch (htons(ethhdr->type)) {
  /* IP or ARP packet? */
  case ETHTYPE_IP:    //IP协议，调用ip_input处理输入的数据
	ip_input(p,netif);
  case ETHTYPE_ARP:
	p
#if PPPOE_SUPPORT
  /* PPPoE packet? */
  case ETHTYPE_PPPOEDISC:
  case ETHTYPE_PPPOE:
#endif /* PPPOE_SUPPORT */
    /* full packet send to tcpip_thread to process */
    if (netif->input(p, netif)!=ERR_OK)
     { LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: IP input error\n"));
       pbuf_free(p);
       p = NULL;
     }
    break;

  default:
    pbuf_free(p);
    p = NULL;
    break;
  }
  #endif
}

/**
 * Should be called at the beginning of the program to set up the
 * network interface. It calls the function low_level_init() to do the
 * actual setup of the hardware.
 *
 * This function should be passed as a parameter to netif_add().
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return ERR_OK if the loopif is initialized
 *         ERR_MEM if private data couldn't be allocated
 *         any other err_t on error
 */
err_t
ethernetif_init(struct netif *netif)
{
  struct ethernetif *ethernetif;

  LWIP_ASSERT("netif != NULL", (netif != NULL));
  low_level_init(netif);
  //printf("low level init done\n\r");
  ethernetif = mem_malloc(sizeof(struct ethernetif));
  //ethernetif = (struct ethernetif*)malloc(sizeof(struct ethernetif));
  if (ethernetif == NULL) {
    LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_init: out of memory\n"));
    return ERR_MEM;
  }
 // printf("memory malloc sucessful\n\r");
#if LWIP_NETIF_HOSTNAME
  /* Initialize interface hostname */
  netif->hostname = "VETH";
#endif /* LWIP_NETIF_HOSTNAME */

  /*
   * Initialize the snmp variables and counters inside the struct netif.
   * The last argument should be replaced with your link speed, in units
   * of bits per second.
   */
  NETIF_INIT_SNMP(netif, snmp_ifType_ethernet_csmacd, LINK_SPEED_OF_YOUR_NETIF_IN_BPS);

  netif->state = ethernetif;
  memset(netif->name,0,sizeof(netif->name));
  sprintf(netif->name,"Port %d",(netif->num + 1));
 
//  netif->name[0] = IFNAME0;
//  netif->name[1] = IFNAME1;
  /* We directly use etharp_output() here to save a function call.
   * You can instead declare your own function an call etharp_output()
   * from it if you have to do some checks before sending (e.g. if link
   * is available...) */
  netif->output = etharp_output;
  netif->linkoutput = low_level_output;
  
  ethernetif->ethaddr = (struct eth_addr *)&(netif->hwaddr[0]);
  /* initialize the hardware */
  //printf("will init hardware\n\r");
  

  return ERR_OK;
}

//#endif /* 0 */
