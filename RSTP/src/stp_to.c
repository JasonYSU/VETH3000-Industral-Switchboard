/************************************************************************ 
 * RSTP library - Rapid Spanning Tree (802.1t, 802.1w) 
 * Copyright (C) 2001-2003 Optical Access 
 * Author: Alex Rozin 
 * 
 * This file is part of RSTP library. 
 * 
 * RSTP library is free software; you can redistribute it and/or modify it 
 * under the terms of the GNU Lesser General Public License as published by the 
 * Free Software Foundation; version 2.1 
 * 
 * RSTP library is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser 
 * General Public License for more details. 
 * 
 * You should have received a copy of the GNU Lesser General Public License 
 * along with RSTP library; see the file COPYING.  If not, write to the Free 
 * Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 
 * 02111-1307, USA. 
 **********************************************************************/

/* This file contains system dependent API
   from the RStp to a operation system (see stp_to.h) */

/* stp_to API for Linux */

#include <stddef.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <stdint.h>


#include "base.h"
#include "stpm.h"
#include "stp_in.h"
#include "stp_to.h"
#include "bitmap.h"
#include "uid_stp.h"
#include "veth_common.h"
#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_conf.h"
#include "veth_eth_ap.h"
#include "veth_marvell_88e6097f.h"
//#include "veth_invariate.h"
#include "veth_mac.h"
#include "board_info.h"
extern BITMAP_T   enabled_ports;

/*************
void
stp_trace (const char *format, ...)
{
  #define MAX_MSG_LEN  128
  char     msg[MAX_MSG_LEN];
  va_list  args;

  va_start(args, format);
  vsnprintf (msg, MAX_MSG_LEN-1, format, args);
  MY_PRINT ("%s\n", msg);
  va_end(args);
  
}
**********/
void
STP_OUT_set_which_port(int port_index)
{
//   if(port_index == 1)  
//	     WRITE_MDIO(MDIO_DEST_ROUTE,SWITCH_FORWARD_1);
//   if(port_index == 2 )
//       WRITE_MDIO(MDIO_DEST_ROUTE,SWITCH_FORWARD_1);
}



#ifdef STRONGLY_SPEC_802_1W
int
STP_OUT_set_learning (int port_index, int vlan_id, int enable)
{

  return STP_OK;
}

int
STP_OUT_set_forwarding (int port_index, int vlan_id, int enable)
{
  return STP_OK;
}
#else
/* 
 * In many kinds of hardware the state of ports may
 * be changed with another method
 */
int
STP_OUT_set_port_state (IN int port_index, IN int vlan_id,
            IN RSTP_PORT_STATE state)
{  
//	switch(state)
//	{
//	  case UID_PORT_DISABLED:
//	    printf("Set port %d disabled\n\r",port_index);
//	    break;
//	  case UID_PORT_DISCARDING:
//	    printf("Set port %d discarding\n\r",port_index);
//	    break;
//	  case UID_PORT_LEARNING:
//	    printf("Set port %d learning\n\r",port_index);
//	    break;
//	  case UID_PORT_FORWARDING:
//	    printf("Set port %d forwarding\n\r",port_index);
//	    break;
//	  default:
//	    break;
//	}
	Set_Switch_Port_state(port_index,state); 
	return STP_OK;
  //return AR_INT_STP_set_port_state (port_index, vlan_id, state);
}
#endif

/***********STP_OUT_get_port_mac***************/
//Before get the MAC address, you should init the chip's MAC address first
void
STP_OUT_get_port_mac (int port_index, unsigned char *mac)	 
{	  
    uint8_t cnt;
 	   
	for (cnt = 0 ;cnt< 6 ;cnt++)
	  {  
	/*Check if the last bit[40] of the MAC address is RESET,
	which indicates that the Port_mac_addr equals or differs from port_index */
		if (cnt == 5)  
		  *(mac+cnt) = (THE_PORT_MAC_RESET & board.sets_info.The_MAC[cnt]) | (port_index & 0x0F);  /*lower 4-bits for port_index*/
		else 
		  *(mac+cnt) = board.sets_info.The_MAC[cnt];
      }
 
/*#if 0
  static long pid = -1;
  static unsigned char mac_beg[] = {'\0', '\0', '\0', '\0', '\0', '\0'};

  if (pid < 0) {
    pid = getpid ();
    memcpy (mac_beg + 1, &pid, 4);
  }
  memcpy (mac, mac_beg, 5);
  mac[5] = port_index;
//  memcpy (mac, STP_MAIN_get_port_mac (port_index), 6);
#endif	*/
  }

int             /* 1- Up, 0- Down */
STP_OUT_get_port_link_status (int port_index)
{
   return Get_Port_status(port_index);
} 

int
STP_OUT_flush_lt (IN int port_index, IN int vlan_id, LT_FLUSH_TYPE_T type, char* reason)
{
//  MAC_ENTRY_T  entry;
	uint16_t     move;
	int          loop;
//	MAC_ENTRY_T entryx;
//	MAC_API_T  api_entry;
/****
  stp_trace("clearFDB (%d, %s, '%s')",
        port_index, 
        (type == LT_FLASH_ALL_PORTS_EXCLUDE_THIS) ? "Exclude" : "Only", 
        reason);
****/
//	if (type == LT_FLUSH_ONLY_THE_PORT)
//	  {
//		move = (uint16_t)((port_index) & 0x000f) | 0x00f0;//When flush a ports ATU table ,move  bits[7:4] = port_index,bits[3:0] = 0xf;
//		Flush_Move_ATUentry(ALL_NONSTA,MOVE_ENTRY,move,0);
//	  }else{ //LT_FLUSH_ALL_PORTS_EXCLUDE_THIS
//		for(loop =0 ;loop < MAX_PORT_NUM ;loop++)
//		  { 
//			if(loop != port_index)
//			{
//			  move = (uint16_t)((loop) & 0x000f) | 0x00f0;
//			  Flush_Move_ATUentry(ALL_NONSTA,MOVE_ENTRY,move,0);
//			}
//		  }
//	  }//end of else
	Flush_Move_ATUentry(ALL_NONSTA,FLUSH_ENTRY,0x00,0x00);
//	 for(loop = 0 ;loop < 6; loop++ )
//		{
//		  entryx.mac_entry_MAC[loop] = 0xff;          //MAC{0xff,0xff,0xff,0xff,0xff,0xff} used to get the first ATU entry
//		}
//	  while(Display_ALL_MACentry(&api_entry,&entryx,0)) //Get All the MAC entry
//		{
//		  //entry_counter++;
//		}
	return STP_OK;
}

int
STP_OUT_set_hardware_mode (int vlan_id, UID_STP_MODE_T mode)
{
  return STP_OK;
  //return AR_INT_STP_set_mode (vlan_id, mode);
}


int
STP_OUT_tx_bpdu (int port_index, int vlan_id,
         unsigned char *bpdu, size_t bpdu_len)
{
//   uint16_t tem_reg;
	 
//extern int bridge_tx_bpdu (int port_index, unsigned char *bpdu, size_t bpdu_len);
/*Set the CPU port at DSA mode so that frames can be moved to the port indicated*/

/*Then send the packets*/	
	ETH_TX_PACKET(&Eth_Handle,bpdu_len,bpdu,port_index);
	  	
	return STP_OK;
	
}

const char *
STP_OUT_get_port_name (IN int port_index)
{
  static char tmp[4];
  sprintf (tmp, "p%02d", (int) port_index);
  return tmp;
  //return port2str (port_index, &sys_config);
}

unsigned long
STP_OUT_get_deafult_port_path_cost (IN unsigned int portNo)
{
    return Get_Port_cost(portNo);
}

unsigned long STP_OUT_get_port_oper_speed (unsigned int portNo)
{
    return Get_Oper_speed(portNo);  
}

int             /* 1- Full, 0- Half */
STP_OUT_get_duplex (IN int port_index)
{
    return Get_Port_duplex(port_index);
}

int
STP_OUT_get_init_stpm_cfg (IN int vlan_id,
                           INOUT UID_STP_CFG_T* cfg)
{
  cfg->bridge_priority =        board.sets_info.my_rstp.bridge_pri;
  cfg->max_age =                board.sets_info.my_rstp.bridge_max_age;
  cfg->hello_time =             board.sets_info.my_rstp.bridge_hellot;
  cfg->forward_delay =          board.sets_info.my_rstp.bridge_fw_delay;
  cfg->force_version =          DEF_FORCE_VERS;

  return STP_OK;
}
  

int
STP_OUT_get_init_port_cfg (IN int vlan_id,
                           IN int port_index,
                           INOUT UID_STP_PORT_CFG_T* cfg)
{
  cfg->port_priority =                  DEF_PORT_PRIO;
  cfg->admin_non_stp =                  DEF_ADMIN_NON_STP;
  cfg->admin_edge =                     DEF_ADMIN_EDGE;
  cfg->admin_port_path_cost =           ADMIN_PORT_PATH_COST_AUTO;
  cfg->admin_point2point =              DEF_P2P;

  return STP_OK;
}



