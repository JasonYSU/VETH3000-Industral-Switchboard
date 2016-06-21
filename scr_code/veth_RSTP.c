/**
*************************************************************************
@File   : hal\src_code\veth_RSTP.c
@Author : Duke Lee
@Date   : 22-August-2014
@Brief  : Registers of Marvell_88e6097f and functions  
*************************************************************************
**/

#include <stdio.h>
#include <stdint.h>

#include "base.h"
#include "stpm.h"
#include "stp_in.h"
#include "uid_stp.h"
#include "stp_to.h"
#include "uid.h"
#include "stp_bpdu.h"
#include "veth_common.h"
#include "veth_RSTP.h"
#include "veth_marvell_88e6097f.h"
#include "veth_hal_init.h"
#include "veth_eth_ap.h"
#include "veth_ntoh.h"


#define VLAN_ID       1	        	
#define THE_MAX_NAME  10
#define THE_RECIEVE_TIMEOUT 0x0000000a


char      bridge_name[THE_MAX_NAME] = "VETH2000";   /*Bridge name*/
BITMAP_T  enabled_ports;                            /*Define the port maps, each bit represent a port '1' for on,while '0' for down*/
uint8_t   timer_targ;
struct pt rstp_pt;
/*Statement of the private functions*/
//static uint8_t timerExpiredOneSecond();
static uint8_t timerExpiredOneSecond(uint8_t *bp_targ);
/******************************************************************/
//Functions :void printfStatus()
//Brief :    Print the port status 
/******************************************************************/
static void printfStatus()
  {
    uint8_t loop;
	
//	for(loop = 0 ;loop< 10 ;loop++)
//	  {
//		getSwitchPortState(loop);
//	  }
	printf("\n\r");
  }

/******************************************************************/
//Functions :void RSTP()
//Brief     :Do the neccessary configuration for running RSTP instanse
//Parameter :None 
//retval    :uint8_t : '1'for RSTP instanse creation successful,'0'for failure        
/******************************************************************/
uint8_t configRSTP()
  {	
/*Initialize the marvell 88E6097F chip for the RSTP*/
	RSTP_Init();
/*Before start the RSTP instanse , flush all of the MAC address first*/
	Flush_Move_ATUentry(ALL_NONSTA,FLUSH_ENTRY,0,0);
//	Delays(1);
/*Start the ETH TX and RX*/
	HAL_ETH_Start(&Eth_Handle);
/*Get the initial parameter value tem_scount*/
//	tem_scount = sCount;
/*Create the RSTP instance*/		
	return CREATE_RSTP_INSTANCE();
	//Delays(20);
	//return 1;
  }
/******************************************************************/
//Functions :void RSTP()
//Brief :    Run the RSTP protocal   
/******************************************************************/
PT_THREAD(RSTP(struct pt *pt,uint8_t *rx_packet,uint8_t *bpdu_targ))
  {
//	BITMAP_T  current_ports;    //Used to store the current port link status 
//	BITMAP_T  result_ports;     //Used to contian the result enabled_port vectors
	static BPDU_T    bpdu;
	static uint8_t   port_num;
#ifdef MY_DEBUG
	MAC_ENTRY_T  entry;
	uint8_t   i;
#endif
//	uint8_t   port_index;
//	uint8_t   packets[1600];
//	int       loop;
	PT_BEGIN(pt);
/*Check if the DIAL-UP mode indicates that the RSTP should be shut up */
//			   if(0x01 == Get_DIAL_Mode())
//				   {
//					    /*Get the pointer of the RSTP instance,then delete it*/
//						  this = stpapi_stpm_find (VLAN_ID);
//						  STP_stpm_delete (this);
//						  return;
//           }
	
//		printf("one second reached\n\r");
//		printf("bpdu_targ :%d\n\r",*bpdu_targ);
	//timerExpiredOneSecond(bpdu_targ);
	PT_WAIT_UNTIL( pt,timerExpiredOneSecond(bpdu_targ) );
	//printf("timer expired one second\n\r");
	if(timer_targ == 1)
	{
	  STP_IN_one_second();
	}
#ifdef MY_DEBUG
	  for (loop = 0 ;loop < 6 ;loop++ )
	  {
		entry.mac_entry_MAC[loop] = GET_NEXT[loop];
	  }
	  for(i = 0 ; i< 10 ; i++)	 
	  {
		Read_ATU_entry(&entry);
		printf("The read entry from ATU entry: ");
		printf("%x\n\r",entry.mac_entry_data);
		for(loop = 0;loop <6 ;loop++)
		{
		  printf("%x\n\r",entry.mac_entry_MAC[loop]);
		  Delay(1);
		}
	  }
#endif
//	  /*Check the status of the ports and get the current_ports vector*/
//	  for(loop = 0 ; loop < NUMBER_OF_PORTS ; loop++)
//	  {
//	  /*Check if the port is link on or not*/
//	    if( STP_OUT_get_port_link_status(loop) )
//		{
//		  BitmapSetBit(&current_ports,loop);
//	    }
//		else
//		{
//		  BitmapClearBit(&current_ports,loop);
//		}
//	  }
//	  /*judge if the port status has changed*/
//	  BitmapXor(&result_ports,&current_ports,&enabled_ports); 
//	  if(result_ports.part0 != RESET)    /*The vector is not the same ,port status has changed*/
//	  {
//		for(loop = 0 ;loop < NUMBER_OF_PORTS; loop++)
//		{
//		  if(BitmapGetBit(&result_ports,loop))    /*Check which port link status changes */
//		  {
//			if(BitmapGetBit(&current_ports,loop)) /*Check the port changes to link on status*/
//			{
//			  STP_IN_enable_port (loop, ENABLE);
//			  BitmapSetBit(&enabled_ports,loop);
//			}
//			else     /*the port changed to link down status*/
//			{
//			  STP_IN_enable_port (loop, DISABLE);
//			  BitmapClearBit(&enabled_ports,loop);
//			}									
//		  }
//		}
//	  }//end of if(result port)

/*For RSTP: analyze the packet,and check if the packet should be update into the RSTP instanse*/
	//PS: the bytes 6~11 are skipped for we needn't to know the source address
	if( 1 == *bpdu_targ )
	{
#ifdef MY_DEBUG
	  printf("The bpdu targ:%d\n\r",*bpdu_targ);
	  printf("Recieved packets:\n\r");
	  for(i=0 ;i< 20 ;i++ )
	  {
		printf("%02x.",*(rx_packet+i));
	  }
	  printf("\n\r");
	  for(i=20 ;i< 40 ;i++ )
	  {
		printf("%02x.",*(rx_packet+i));
	  }
	  printf("\n\r");
	  for(i=40 ;i< 60 ;i++ )
	  {
		printf("%02x.",*(rx_packet+i));
	  }
	  printf("\n\r");
#endif
	  *bpdu_targ = 0;
	  /*Check the DSA targ and get the port num*/
	  port_num = *(rx_packet + 17 ) >> 3;
//	  printf("PortNo:%d\n\r",port_num);
	  /*Then get the packet into the struct BPDU_T*/
	  memcpy(&bpdu.eth,rx_packet+20, sizeof(bpdu.eth));                   //Get the bpdu eth header
	  memcpy(&bpdu.hdr,rx_packet+20+sizeof(bpdu.eth), sizeof(bpdu.hdr)); //Get the bpdu header
	  memcpy(&bpdu.body,rx_packet+20+sizeof(bpdu.eth)+sizeof(bpdu.hdr),sizeof(bpdu.body)); //Get the bpdu body
	  memcpy(&bpdu.ver_1_len,rx_packet+20+sizeof(bpdu)-1,1);               //Get the bpdu extra bytes
	  /*After get the needed information,clear the bytes of the bpdu _buf*/
#ifdef MY_DEBUG
	  memcpy(rx_packet,&bpdu,sizeof(BPDU_T));
	  printf("The bpdu:\n\r");
	  for(i=0 ;i< 20 ;i++ )
	  {
		printf("%02x.",*(rx_packet+i));
	  }
	  printf("\n\r");
	  for(i=20 ;i< 40 ;i++ )
	  {
		printf("%02x.",*(rx_packet+i));
	  }
	  printf("\n\r");
	  for(i=40 ;i< 60 ;i++ )
	  {
		printf("%02x.",*(rx_packet+i));
	  }
	  printf("\n\r");
#endif
	  memset(rx_packet,0,4096);
	  /*Check the bpdu header*/				
	  if(! STP_IN_check_bpdu_header (&bpdu,1518))
	  { 
		bpdu_count.RECIEVE_AVAILIABLE_BPDU ++;
		/*Valid bpdu packet*/
		STP_IN_rx_bpdu (1,(int)port_num,&bpdu,64);
	  }
	  else
	  {
	   bpdu_count.RECIEVE_UNAVAILIABLE_BPDU ++;
	  } 
	}//end of if( 1 == *bpdu_targ )
	PT_END(pt);
  }
/******************************************************************/
//Function  :   uint8_t CREATE_RSTP_INSTANCE();
//Brief     :   Create the bridge instance,and then set the initial configuration for the instance.
//Parameter :   No parameters 
//retval    :   uint8_t : '1'for create successful,and '0'for failure
/******************************************************************/
uint8_t CREATE_RSTP_INSTANCE()
  {
	uint8_t  loop;
	int      The_EOR;
	UID_STP_CFG_T  uid_cfg;
	BITMAP_T  ports;
       
//init the port number for the bridges and set the bits high for the port creation
/*Initlize the maximum number of the bridge*/
	STP_IN_init(NUMBER_OF_PORTS);
/*Clear the bitmap vector*/
	BitmapClear(&enabled_ports);
    BitmapClear(&ports);
	for (loop= 0; loop < NUMBER_OF_PORTS; loop++) 
	  {
		BitmapSetBit(&ports, loop);
	  }
	uid_cfg.field_mask      = BR_CFG_STATE;
	uid_cfg.stp_enabled	    = STP_ENABLED;	
/*Check the status of the ports and init enabled_ports vector*/
	for(loop = 0 ; loop < NUMBER_OF_PORTS ; loop++)
	  {
	/*Check if the port is link on or not*/
		if( STP_OUT_get_port_link_status(loop) )
		  {
			BitmapSetBit(&enabled_ports,loop);
		  }
      }
/*set the init  info for my bridge instanse,including creatation of the bridge*/
	The_EOR = STP_IN_stpm_set_cfg (VLAN_ID,&ports,&uid_cfg);
	if(The_EOR  == STP_OK)
	{  return 1;}
	else
	{  return 0;}
  }


/******************************************************************/
//Function  :  static uint8_t timerExpiredOneSecond();
//Brief     :   Create the bridge instance,and then set the initial configuration for the instance.
//Parameter :   No parameters 
//retval    :   uint8_t : '1'for create successful,and '0'for failure
/******************************************************************/
static uint8_t timerExpiredOneSecond(uint8_t *bp_targ)
  {
	BITMAP_T  current_ports;    //Used to store the current port link status 
	BITMAP_T  result_ports;     //Used to contian the result enabled_port vectors
	static    uint32_t tem_scount = 0;
	int  loop;
	
	
	//printf("The timer ExpiredOneSecond in\n\r");
	/*Check the status of the ports and get the current_ports vector*/
	for(loop = 0 ; loop < NUMBER_OF_PORTS ; loop++)
	{
	/*Check if the port is link on or not*/
	  if( STP_OUT_get_port_link_status(loop) )
	  {
		BitmapSetBit(&current_ports,loop);
	  }
	  else
	  {
		BitmapClearBit(&current_ports,loop);
	  }
	}
	/*judge if the port status has changed*/
	BitmapXor(&result_ports,&current_ports,&enabled_ports); 
	if(result_ports.part0 != RESET)    /*The vector is not the same ,port status has changed*/
	{
	  for(loop = 0 ;loop < NUMBER_OF_PORTS; loop++)
	  {
		if(BitmapGetBit(&result_ports,loop))    /*Check which port link status changes */
		{
		  if(BitmapGetBit(&current_ports,loop)) /*Check the port changes to link on status*/
		  {
			STP_IN_enable_port (loop, ENABLE);
			BitmapSetBit(&enabled_ports,loop);
		  }
		  else     /*the port changed to link down status*/
		  {
			STP_IN_enable_port (loop, DISABLE);
			BitmapClearBit(&enabled_ports,loop);
		  }									
		}
	  }
	}//end of if(result port)
	/* Check if the 1s timer counter increased by 1 */
	if(sCount - tem_scount >= 1)
	{
	  tem_scount = sCount;
	  timer_targ = 1;
	}else{
	  timer_targ = 0;
	}
	//printf("The return for timerExpiredOneSecond = %d\n\r",timer_targ || (*bp_targ));
	return (timer_targ || (*bp_targ));
  }		
		
