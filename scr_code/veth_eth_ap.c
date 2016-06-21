/**
****************************************************************************************
@Flie    :   STM32F4xx_HAL_Driver\re_src\eth_ap.c
@Author  :   Duke_Lee
@Version :   V1.0
@Date    :   12-June-2014
@Brief   :   Send or recieve the packets 
****************************************************************************************
**/

/*******************************header include***********************************/
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_conf.h"
#include "stp_bpdu.h"
#include "veth_common.h"
#include "veth_marvell_88e6097f.h"
#include "stp_in.h"   //For STP_IN_check_bpdu_header()

///****************************micro define***************************************/
//#define BPDU_L_SAP              0x42
//#define LLC_UI                  0x03
//#define RSTP_PTP                0x02            
#define LOOP_CNT                20
///*******************************************************************************/

RSTP_COUNTER bpdu_count;



#ifdef MY_DEBUG


/********************************************************************************/
typedef struct tx_tcn_bpdu_t {
  MAC_HEADER_T  mac;
  ETH_HEADER_T  eth;
  BPDU_HEADER_T hdr;
} TCN_BPDU_T;

typedef struct tx_stp_bpdu_t {
  MAC_HEADER_T  mac;
  ETH_HEADER_T  eth;
  BPDU_HEADER_T hdr;
  BPDU_BODY_T   body;
} CONFIG_BPDU_T;

typedef struct tx_rstp_bpdu_t {
  MAC_HEADER_T  mac;
  ETH_HEADER_T  eth;
  BPDU_HEADER_T hdr;
  BPDU_BODY_T   body;
  unsigned char ver_1_length[2];
} RSTP_BPDU_T;



RSTP_BPDU_T bpdu_packet __attribute__((section("EXTSDRAM")))  = {
  {/* MAC_HEADER_T */
    {0x01, 0x80, 0xc2, 0x00, 0x00, 0x00}, /* dst_mac */
    {0x20, 0x15, 0x11, 0x31, 0x13, 0xf8},  /* src_mac */
  },
  { /* ETH_HEADER_T */
    {0x00, 0x00},                         /* len8023 */
    BPDU_L_SAP, BPDU_L_SAP, LLC_UI        /* dsap, ssap, llc */
  },
  {/* BPDU_HEADER_T */
    {0x00, 0x00},                         /* protocol */
    BPDU_VERSION_RAPID_ID, 0x00           /* version, bpdu_type */
  },
  {
    0x00,                                 /*  flags; */
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},  /*  root_id[8]; */
    {0x00,0x00,0x00,0x00},                /*  root_path_cost[4]; */
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},  /*  bridge_id[8]; */
    {0x00,0x00},                          /*  port_id[2]; */
    {0x00,0x00},                          /*  message_age[2]; */
    {0x00,0x00},                          /*  max_age[2]; */
    {0x00,0x00},                          /*  hello_time[2]; */
    {0x11,0xaa},                          /*  forward_delay[2]; */
  },
   {0xbb,0xcc},                           /*  ver_1_length[2]; */
};


/**************************************************************************************/
//Function :  BPDU_form()
//Make the BPDU packets
 
void BPDU_form()
{
	unsigned char rootid[8]    = {0x80,0x00,0x00,0x05,0xd1,0x12,0x40,0x00};
	unsigned char pacost[4]    = {0x00,0x03,0x0d,0x40};
	unsigned char bridgeid[8]  = {0x80,0x00,0x00,0x05,0xd1,0x12,0x40,0x00};
	unsigned char portid[2]    = {0x80,0x01};
	unsigned char message[2]	 =  {0x00,0x03}; 
	unsigned char maxage[2]    =    {0x00,0x14};
	unsigned char hellotime[2] = {0x00,0x02};
	unsigned char frddelay[2]  = {0x00,0x0f};       
	uint16_t i;
    
//Here forms the simulation body
#ifdef MY_DEBUG_P
    printf("Is the smulate in ?\n");    
#endif  	   				  
	bpdu_packet.body.flags = 0x4e;
    
	for(i= 0 ;i<8 ;i++)
	{
	  bpdu_packet.body.root_id[i] = rootid[i];
	}
    for(i=0 ;i<4;i++)
	{
	  bpdu_packet.body.root_path_cost[i]= pacost[i];
	}
	for(i=0 ;i<8 ;i++)
	{
	  bpdu_packet.body.bridge_id[i]= bridgeid[i];
	}
	bpdu_packet.body.port_id[0] = portid[0];
	bpdu_packet.body.port_id[1]	= portid[1];
	bpdu_packet.body.message_age[0]= message[0];
	bpdu_packet.body.message_age[1]= message[1];
	bpdu_packet.body.max_age[0] = maxage[0];
	bpdu_packet.body.max_age[1] = maxage[1];
	bpdu_packet.body.hello_time[0] = hellotime[0];
	bpdu_packet.body.hello_time[1]= hellotime[1];
	bpdu_packet.body.forward_delay[0] = frddelay[0];
	bpdu_packet.body.forward_delay[1] = frddelay[1];
	  
}
#endif
/**************************************************************************************/
//Function  : Add_From_CPU_tag()
// In order to forward the packets through indicated port
/**************************************************************************************/
void Add_From_CPU_tag(uint8_t port_index,void *packets)
{
	uint8_t cnt;  
	/*Add the From_CPU tag,the DSA tag offset is THE_TAG_ADDR,and the total tag bytes is 4*/
	for (cnt = 1 ; cnt < THE_TAG_LENGTH  ; cnt++ )
	{
	  if(cnt == 1)   /*Target port is the high 5-bit of the 2nd byte*/  
		*( (uint8_t *)packets + THE_TAG_ADDR + cnt ) =  (port_index << 3) & 0xFF;
	  else 
		*( (uint8_t *)packets +THE_TAG_ADDR + cnt )  =  0x00;
	}
#ifdef MY_DEBUG	
	printf("After add the DSA tag:\n\r");
	for(cnt = 0 ;cnt < 20 ; cnt++ )
	{
	  printf("%x \n\r ", *((uint8_t *)packets + cnt));
    }
#endif

}
/**************************************************************************************/
//Function  : Fillin_TXBUF()
// Fill in the TXBUFFER with the frames to be transmited 
/**************************************************************************************/
void ETH_Fillin_TXBUF(ETH_HandleTypeDef *hal_eth,uint32_t frame_length,void * packets)
 {
	uint16_t  i;
/*Force 'hal_eth->TxDesc->Buffer1Addr' to type 'unsigned char *'*/
	for(i=0 ; i< frame_length; i++)
	{       			
	  *((unsigned char *)(hal_eth->TxDesc->Buffer1Addr)+i)  = * ((uint8_t *)packets + i);			
	}
#ifdef MY_DEBUG		 
	for(i=0 ;i<LOOP_CNT;i++)
	{
	  printf("%x\n\r",*((unsigned char *)(hal_eth->TxDesc->Buffer1Addr)+i));
	  Delay(5000);
	}
#endif
 }
/**************************************************************************************/
//Function  : Set_ALLOWN()
// Set all of the descriptor OWN bit 
/**************************************************************************************/
void ETH_Set_Recieve_ALLOWN(ETH_HandleTypeDef *hal_eth)
  {
	uint8_t i;
		 
	for(i=0 ;i < DESCRIPTORLIST_SIZE;i++)
	{
	  hal_eth->RxDesc->Status |= ETH_DMARXDESC_OWN;
	  hal_eth->RxDesc  = (ETH_DMADescTypeDef *)hal_eth->RxDesc->Buffer2NextDescAddr;
	}
  }
 
/**************************************************************************************/
//Function  : ETH_TX_PACKET
//Brief     : TX the packets to the indicated port(port_index)
/**************************************************************************************/
void ETH_TX_PACKET(ETH_HandleTypeDef *hal_eth,uint32_t frame_length,void * packets,uint8_t port_index)
  {
#ifdef VETH2000_WEB
#ifdef DSA_ADDED
	Add_From_CPU_tag(port_index,packets);
#endif
#endif		
	ETH_Fillin_TXBUF(hal_eth,frame_length,packets);
	HAL_ETH_TransmitFrame(hal_eth,frame_length);
  }
/************************************************************************/
//Function   £ºuip_Tx_Packet()
//Brief      : Only called by uip TCP/IP stack
//Parameter  : pbuf(Input)_________Point to the buffer to be send
//             frame_len(Input)____Indicate how many chars the buffer has
void uip_Tx_Packet(uint8_t * pbuf,uint16_t frame_len)
  {
	/**Fill in the packets in ETH_TXBUFFER**/
	ETH_Fillin_TXBUF(&Eth_Handle,frame_len,pbuf);
	HAL_ETH_TransmitFrame(&Eth_Handle,frame_len);
  }	
/************************************************************************/
//Function   £ºuip_Rx_Packet()
//Brief      : Only called by uip TCP/IP stack
//Parameter  : pbuf(Input)_________Point to the buffer for the recieved packets
//             frame_len(Input)____Indicate how large the buffer is
//             time_out(Input)_____Indicate the max polling time when uip_Rx_Operation
//             *portNo(Output)______Get from which port the packet comes
unsigned int uip_Rx_Packet(uint8_t *pbuf,uint16_t buffer_size,uint32_t timeout)
{
	uint32_t  i;
	int j;
	uint32_t  tim_value;
	uint32_t  tem_point;
	uint16_t  ret;
	uint8_t   special_MAC[6] = {0x00,0x00,0x00,0x00,0x00,0x01};
	uint8_t   temp_MAC[6];
	ETH_DMADescTypeDef *pdescri;
	ETH_DMARxFrameInfos  pinfo;
	
	tim_value   =  msCount;
/*Loop till a whole packet is recieved or timeout*/			
	while(HAL_ETH_GetReceivedFrame(&Eth_Handle) != HAL_OK)
	{
	/*Polling till a frame is recieved or the time out */
	if(msCount - tim_value >= timeout)
	  return 0;
	}
	//printf("The recieve time out\n\r");
/*Get the received data from the RXBUFFER,and set the corresponding descriptor's OWN bit '1'*/	 
	pinfo = Eth_Handle.RxFrameInfos;
	(Eth_Handle.RxFrameInfos).SegCount = 0;			
	for(i=0 ; i < pinfo.length; i++)
	{ 
	  pbuf[i] = *((unsigned char *)(pinfo.buffer)+i);
	}
#ifdef MY_DEBUG
	printf("Recieved packets:\n\r");
	for(i=0 ;i< 20 ;i++ )
	{
	  printf("%02x.",*((unsigned char *)(pinfo.buffer)+i) );
	}
	printf("\n\r");
	for(i=20 ;i< 40 ;i++ )
	{
	  printf("%02x.",*((unsigned char *)(pinfo.buffer)+i) );
	}
	 printf("\n\r");
	for(i=40 ;i< 60 ;i++ )
	{
	  printf("%02x.",*((unsigned char *)(pinfo.buffer)+i) );
	}
	printf("\n\r");
#endif
	ret =i;
/*Judge if the buffer overlap*/
	if(i > buffer_size)
	{
	  printf("uip Buffer overlap!");
	}
#ifdef MY_DEBUG
	for(i=6;i<11;i++)
	{
	 temp_MAC[i-6] = pbuf[i];
	}
	if(Str_cmp(temp_MAC,special_MAC,6) ==0)
	{
	  printf("Can recieve the packet from Smartbit\n\r");
	}
#endif	
/*Set the OWN bit of the descriptor that the recieved data in the corresponding buffer has been read out*/
	pdescri = pinfo.FSRxDesc;
	for(i=0 ; i < pinfo.SegCount ; i++)  
	  {
		tem_point = pdescri->Buffer2NextDescAddr;
		pdescri->Status |= ETH_DMARXDESC_OWN;
		pdescri = (ETH_DMADescTypeDef *)(tem_point);
	  }
/*Return the length of the recieved packets*/
//	if(ret >= 1500){
//		printf("uip_Rx_Packet read data is\n\r");
//		for(j = 54; j < (ret - 54); j++){
//			printf("%02x ",pbuf[j]);
//		}
//		printf("\n\r");
//	}
	return ret;	
}

/*************************************************************************************/
//Function : ETH_RX_PACKET()
//Brief    : Recieve a packet into my memory
//return   : '0' for no packet recieved or no appropriate packet recieved,'1' indicates that an available packet recieved
int
ETH_RX_PACKET(ETH_HandleTypeDef *hal_eth,uint8_t * packets,BPDU_T *bpdu,uint8_t *port_index,uint32_t timeout)
  {
	uint32_t i;
	uint32_t  tem_point;
	ETH_DMADescTypeDef *pdescri;
	ETH_DMARxFrameInfos  pinfo;
	uint32_t           tim_value;		 
		 
	tim_value   =  msCount;
   /*Loop till a whole packet is recieved or timeout*/			
	while(HAL_ETH_GetReceivedFrame(hal_eth) != HAL_OK)
	{
	/*Polling till a frame is recieved or the time out */
	  if(msCount - tim_value >= timeout)
	  return 0;
	}
/*Get the received data from the RXBUFFER,and set the corresponding descriptor's OWN bit '1'*/	 
	pinfo = Eth_Handle.RxFrameInfos;
	(Eth_Handle.RxFrameInfos).SegCount = 0;			
	for(i=0 ; i < pinfo.length ; i++)
	{ 
//           if( pinfo.buffer % 4 )       /*Check if the DATA buffer pointer is aligned to the data bus*/						 
//					   packets[i] = *((unsigned char *)(pinfo.buffer)+i+2);
//					 else
	  packets[i] = *((unsigned char *)(pinfo.buffer)+i);
	}
/*Set the OWN bit of the descriptor that the recieved data in the corresponding buffer has been read out*/
	pdescri = pinfo.FSRxDesc;
	for(i=0 ; i < pinfo.SegCount ; i++)  
	{
	  tem_point = pdescri->Buffer2NextDescAddr;
	  pdescri->Status |= ETH_DMARXDESC_OWN;
	  pdescri = (ETH_DMADescTypeDef *)(tem_point);
	}
/*Print the recieved packet out*/
#ifdef MY_DEBUG
	printf("Recieved packets:\n\r");
	for(i=0 ;i< 20 ;i++ )
	{
	  printf("%x\n\r",packets[i]);
	}
#endif
#ifdef VETH2000_RSTP
/*For RSTP: analyze the packet,and check if the packet should be update into the RSTP instanse*/
     /*At first check if the destination address id a multicast address(01:80:c2:00:00:00)*/	
	for(i =0 ;i<6 ;i++)
	{
	  if(packets[i] !=  MGMT_MAC[i])  //if The destination address doesn't match
	  {
		bpdu_count.RECIEVE_NONE_BPDU ++;
		return 0;
	  }
	}
	//PS: the bytes 6~11 are skipped for we needn't to know the source address
	/*Then get the packet into the struct BPDU_T*/
	for( i=12 ;i< 12+ THE_TAG_LENGTH ; i++ )			 //Check the DSA tag which is 4-byte,to get the port_index in which the packet comes.
	{ 
	  if(i==13) //Check the second bytes to getthe port index
	  {
		*port_index = (packets[i] >> 3) & 0xff;
	  }
	}
	memcpy(&bpdu->eth,&packets[16], sizeof(bpdu->eth));                   //Get the bpdu eth header
	memcpy(&bpdu->hdr,&packets[16+sizeof(bpdu->eth)], sizeof(bpdu->hdr)); //Get the bpdu header
	memcpy(&bpdu->body,&packets[16+sizeof(bpdu->eth)+sizeof(bpdu->hdr)] ,sizeof(bpdu->body)); //Get the bpdu body
	memcpy(&bpdu->ver_1_len,&packets[16+sizeof(bpdu)-1],1);               //Get the bpdu extra bytes
				
	if(! STP_IN_check_bpdu_header (bpdu,1518))
	{ 
	  bpdu_count.RECIEVE_AVAILIABLE_BPDU ++;
	  return 1;     
	}
	bpdu_count.RECIEVE_UNAVAILIABLE_BPDU ++;
	  return 0;
#endif
	  return 1;
  }//end for the function 

	 
/**************************************************************************************/
//Function  : ETH_Convert_DASA()
//Convert the DA and SA for the recieved packet
void ETH_Convert_DASA(uint8_t *packet)
  {
	uint16_t i;
	uint8_t temp;
/*0~5 bit of packet is the destination address , 6~11 bit of the packet is the source address*/	
	for(i = 0 ; i<6 ; i++ )
	{
	  temp        = packet[6+i];
	  packet[6+i] = packet[i];
	  packet[i]   = temp;
	}
  }
/**************************************************************************************/
//Function  : ETH_EXCS
//Brief     : TX the packets and RX them in loopback mode ,then print them out,
//PS        : This function used for my test for the MII port with smartbit600B
/**************************************************************************************/
void ETH_EXCS(ETH_HandleTypeDef *hal_eth)
{
	uint8_t   packets[1600]; 
//uint32_t  cnt;   
	uint32_t  i;
//uint32_t  x;	
//uint32_t  loop;
	uint32_t  tem_point;
	ETH_DMADescTypeDef *pdescri;
	ETH_DMARxFrameInfos  pinfo;	
	HAL_ETH_Start(hal_eth);
//	BPDU_form();
//	ETH_Fillin_TXBUF(hal_eth,sizeof(RSTP_BPDU_T),&bpdu_packet);
//	HAL_ETH_TransmitFrame(hal_eth,sizeof(RSTP_BPDU_T));
#ifdef MY_DEBUG
	for(i=0 ;i< sizeof(RSTP_BPDU_T); i++)
	{
	  printf("%x\n\r",*((unsigned char *)(&bpdu_packet) +i));
	  Delay(5000);
	}
	printf("%x\n\r",hal_eth->TxDesc ->Buffer1Addr);	
	BPDU_form();	
	sCount = 0;
#endif
/*Test for the Tx operation of the MAC through DMA way*/
//  ETH_Set_Recieve_ALLOWN(hal_eth);
	while(1)
	{
//strncpy(&packets[0],(char *)(hal_eth->RxFrameInfos.buffer),hal_eth->RxFrameInfos.length); 
//   printf("%d\n",hal_eth->RxFrameInfos.buffer);
//	 printf("The last recieved packets : \n\r");
/*Loop till a whole packet is recieved*/			
	while(HAL_ETH_GetReceivedFrame(hal_eth) != HAL_OK)
	{}
			
/*Get the received data from the buffer,and set the corresponding descriptor's OWN bit '1'*/	 
	pinfo = Eth_Handle.RxFrameInfos;
	(Eth_Handle.RxFrameInfos).SegCount = 0;			
	for(i=0 ; i < pinfo.length ; i++)
	{ 
	  if( pinfo.buffer % 4 )       /*Check if the DATA buffer pointer is aligned to the data bus*/						 
		packets[i] = *((unsigned char *)(pinfo.buffer)+i+2);
	  else
		packets[i] = *((unsigned char *)(pinfo.buffer)+i);
	}
/*Set the OWN bit of the descriptor that the recieved data in the corresponding buffer has been read out*/
	pdescri = pinfo.FSRxDesc;
	for(i=0 ; i < pinfo.SegCount ; i++)  
	{
	  tem_point = pdescri->Buffer2NextDescAddr;
	  pdescri->Status |= ETH_DMARXDESC_OWN;
	  pdescri = (ETH_DMADescTypeDef *)(tem_point);
	}
//  ETH_DMADescTypeDef *FSRxDesc;          /*!< First Segment Rx Desc */  
//  ETH_DMADescTypeDef *LSRxDesc;          /*!< Last Segment Rx Desc */  
//  uint32_t  SegCount;                    /*!< Segment count */  
//  uint32_t length;                       /*!< Frame length */  
//  uint32_t buffer;                       /*!< Frame buffer */
#ifdef MY_DEBUG
	printf("Before convert:\n\r");	 
	for(cnt= 0; cnt < LOOP_CNT ; cnt++)
	{
	  printf("%x\n\r",packets[cnt]);
	  Delay(50000);
	}
#endif
/*Then convert the  DA and SA of the frame ,and forward it*/
	ETH_Convert_DASA(packets);
//	ETH_Fillin_TXBUF(hal_eth,sizeof(RSTP_BPDU_T),&bpdu_packet);
//	HAL_ETH_TransmitFrame(hal_eth,sizeof(RSTP_BPDU_T));
	ETH_Fillin_TXBUF(hal_eth,pinfo.length,packets);
	HAL_ETH_TransmitFrame(hal_eth,pinfo.length);			
/*Print some of the data recieved*/      
#ifdef MY_DEBUG
	printf("After convert:\n\r"); 
	for(cnt= 0; cnt < LOOP_CNT ; cnt++)
	{
	  printf("%x\n\r",packets[cnt]);
	  Delay(50000);
	}
#endif
	}//end of while(1)					
}


/***************************************************************/
//Function : Tx_packets()
//Brief    : Tx a packet to the indicated port,
//PS       : This function only used for my test for transmit a packet to the indicated port
void Tx_packet(ETH_HandleTypeDef *hal_eth)
 {
//  ETH_TX_PACKET(hal_eth,sizeof(RSTP_BPDU_T),(uint8_t *)(&bpdu_packet),0);
//	 uip_Tx_Packet((uint8_t *)(&bpdu_packet),sizeof(RSTP_BPDU_T));
 }

/***************************************************************/
//Function : Rx_packets()
//Brief    : Rx a packet to the indicated port,
//PS       : This function only used for my test for recieving a packet
 void Rx_packet(ETH_HandleTypeDef *hal_eth)
  {
	uint8_t   packets[1600];
	BPDU_T    my_bpdu ;
	uint8_t   port_index;
		 
	ETH_RX_PACKET(hal_eth,packets,&my_bpdu,&port_index,200);
  }
  

 
 
 

