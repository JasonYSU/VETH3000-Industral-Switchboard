/**
****************************************************************************************
@File    : D:\ProgramFiles\KEIL\PRO\scr_code\marvell_6097f.c
@Author  : Duke Lee
@Date    : 21-June-2014
@Version : V1.0.0
@Breif   : The Hardware functions of Marvell_6097F    along with the ATU(MAC table) operation
          ,the MAC update operations ,the VTU(VLAN table)operation.
****************************************************************************************
**/

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_conf.h"
#include "veth_common.h"
#include "veth_hal_init.h"
#include "veth_marvell_88e6097f.h"
#include "uid_stp.h"
#include "stp_bpdu.h"

uint16_t trunk_vector  = 0x07ff;                       //Vector dicribe which port contained in Trunk port
uint16_t mask_table[8] = {0x07ff,0x07ff,0x07ff,0x07ff,0x07ff,0x07ff,0x07ff,0x07ff} ;  //To contain all of the trunk mask table entries filled into the mask table
uint8_t KEY_MAC_1[6];

/**printf_trunk_vector(),printf_mask_table(),printf_PORT_CONTROL1()  Used for test only**/
void printf_trunk_vector()
  {
	printf("trunk_vector:");
	printf("%x \n\r",trunk_vector);
  }
void printf_mask_table()
  {
	uint8_t  i;
		   
	printf("The mask table :\n\r");
	for(i =0 ;i<8 ;i++)
	  { 
		printf("%x \n\r",mask_table[i]);
	  }
  }
void printf_PORT_CONTROL1()
  {
	uint8_t   i;
	uint16_t  tem_reg;
	printf("The port_control1 registers :\n\r");
	for (i=0 ;i<11;i++)
	  {
		Eth_Handle.Init.PhyAddress = PORT_0+i;
		HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_PORT_CONTROL1,&tem_reg);
		printf("%x\n\r",tem_reg);
	  }
  }

/**
	 
@Functions to operate the PHY of each port

**/
/******************************************************************************/
/*Function : READ_PHYREGISTER()*/
/*Brief    : Read out the data of the specific PHY register*/
/*Parameter: (Input)__phy_addr: Indicate the PHY you wanna access*/
	        // (Input)__reg_addr: Indicate the Register address
	        //(Output)__reg_value: Point to the readout data
/******************************************************************************/
HAL_StatusTypeDef READ_PHYREGISTER(uint16_t phy_addr,uint16_t reg_addr,uint16_t *reg_value)
  {
	uint16_t  tem_reg  =  0x0000;
	uint16_t  The_status = 0x8000;
	Eth_Handle.Init.PhyAddress  = GLOBAL_2_ADDR;
		    
/*At first, check the busy bit for the phy*/
	while((The_status & SMI_BUSY) != RESET)
	  {
		HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_SMI_PHY_COMMAND,&The_status);
	  }
/*Then fill in the Command register*/
	tem_reg |= reg_addr;                 //fill in the reg_addr 
	tem_reg |= (phy_addr<<5) & 0x03e0;   //fill in the phy_addr
	tem_reg |= 0x0800;                   //fill in the operate code
	tem_reg |= SMI_MODE;			 
	tem_reg |= SMI_BUSY;                 //Start the read operation bits
	HAL_ETH_WritePHYRegister(&Eth_Handle,REG_SMI_PHY_COMMAND,tem_reg);
/*Polling to wait for the operation done*/
	The_status= 0x8000;
	while((The_status & SMI_BUSY) !=	RESET)
	  {
		HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_SMI_PHY_COMMAND,&The_status);
	  }			 
/*Operation is done, get the data*/
	HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_SMI_PHY_DATA,reg_value);
	return HAL_OK;			 
  }

	 
/******************************************************************************/
/*Function : WRITE_PHYREGISTER*/
/*Brief    : Write the data to the specific PHY register*/
/*Parameter: (Input)__phy_addr: Indicate the PHY you wanna access*/
	        // (Input)__reg_addr: Indicate the Register address
	        // (Input)__reg_value:Contain the data to be writed
/******************************************************************************/
HAL_StatusTypeDef WRITE_PHYREGISTER(uint16_t phy_addr,uint16_t reg_addr,uint16_t reg_value)
  {
	uint16_t The_status = 0x8000;
	uint16_t tem_reg    = 0x0000;
	Eth_Handle.Init.PhyAddress  = GLOBAL_2_ADDR;
/*At first, polling the SMI BUSY bit to ensure the operation can begin*/
	while((The_status & SMI_BUSY) != RESET)
	  {
		HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_SMI_PHY_COMMAND,&The_status);
	  }
/*Then fill in the DATA register*/
	HAL_ETH_WritePHYRegister(&Eth_Handle,REG_SMI_PHY_DATA,reg_value);
/*Then fill in the Command register*/
	tem_reg |= reg_addr;               //fill in the reg_addr
	tem_reg |= (phy_addr<<5) & 0x03e0; //fill in the phy_addr
	tem_reg |= 0x0400;                 //fill in the operation bits
	tem_reg |= SMI_MODE;   
	tem_reg |= SMI_BUSY;               //fill in the start bit
//	printf("The READ PHY REGISTER :%x\n\r",tem_reg);
/*Write in the command register and start the operation*/			 
	HAL_ETH_WritePHYRegister(&Eth_Handle,REG_SMI_PHY_COMMAND,tem_reg);
/*Polling to wait for the operation done*/
	The_status = 0x8000;
	while((The_status & SMI_BUSY) != RESET)
	  {
		HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_SMI_PHY_COMMAND,&The_status);
	  }
	return HAL_OK;
  }	   




/**
	
@Functions  The operations of the ATU table(also MAC table)	

**/ 
/*In able to operate the ATU table,process should be in order.At first,Polling for the 'ATUbusy' bit until it is '0'.
Secondly, load the ATU DATA,ATU MAC,ATU FID if needed.Then fill in the ATU Operation register to start it. 
Then Polling till the operation is done*/

/******************************************************************************/
//Function : Set_aging_time()
//Brief    : Set the aging time for the MAC table in the marvell 88e6097F chip
//Parameter: (Input)__aging_time must be the value between 0x01 ~0xFF  
/******************************************************************************/
void set_aging_time(uint8_t aging_time)
  {
	uint16_t  tem_reg;

	Eth_Handle.Init.PhyAddress = GLOBAL_1_ADDR;
	HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_ATU_CONTROL,&tem_reg);
/*Fill in the aging time */
	tem_reg &= ~ATU_AGE_TIME;
	tem_reg |= (aging_time << 4)& ATU_AGE_TIME;
	HAL_ETH_WritePHYRegister(&Eth_Handle,REG_ATU_CONTROL,tem_reg);	
  }
/******************************************************************************/
//Function : Get_aging_time()
//Brief    : Get the aging time for the MAC table in the marvell 88e6097F chip
//Parameter: (Input)__none  
//ret      :  uint16_t
/******************************************************************************/
uint16_t get_aging_time()
  {
	uint16_t  tem_reg;

	Eth_Handle.Init.PhyAddress = GLOBAL_1_ADDR;
	HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_ATU_CONTROL,&tem_reg);
	return (tem_reg>>4);
  }

/******************************************************************************/
//Function : set_MAC_learn_forbid()
//Brief    : Set the port mac 
//Parameter: (Input)__none  
//ret      :  uint16_t
/******************************************************************************/

/*************Marvell_MAC_upt()*************/
//Update the MAC address stored in 6097f

void Marvell_MAC_upt()
  {
	uint16_t tem_reg;
	uint16_t  cnt;
	Eth_Handle.Init.PhyAddress  = GLOBAL_2_ADDR;
/*Write the MAC address into some area in the Marvell 88E6097f*/
//  	for(cnt= 0 ;cnt < 6 ; cnt++)
//		 {
//         tem_reg  = GLOBAL_MAC_UPT | ((cnt<<8)/*&GLOBAL_MAC_POINTER_SET*/)|(KEY_MAC[cnt]& 0xFF);
//			   HAL_ETH_WritePHYRegister(&heth,REG_SWITCH_MAC,tem_reg);
///*Wait till the Update operation done */
//			 while(1){
//            HAL_ETH_ReadPHYRegister(&heth,REG_SWITCH_MAC,&tem_reg);
//            if((tem_reg & GLOBAL_MAC_UPT)!= SET)
//						break;							
//         }
//     }
/*After the MAC address in, Read them out to test it*/
	for(cnt =0 ;cnt <6;cnt++)
	  {
		tem_reg = ((cnt<<8));
		HAL_ETH_WritePHYRegister(&Eth_Handle,REG_SWITCH_MAC,tem_reg);
       /*Wait till the Update operation done */
		Delay(50);
	   /*Get the MAC data*/
		HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_SWITCH_MAC,&tem_reg);
		KEY_MAC_1[cnt] = (uint8_t)(tem_reg & 0x00ff);
	  }
#ifdef MY_DEBUG_P
	printf("The MAC address in the 88e6097f:\n\r");
	for( cnt =0; cnt<6; cnt++)
	{
	  printf("%x\n\r",KEY_MAC_1[cnt]);
	}	
#endif			 
 
}


/******************************************************************************/
//Function : Marvell_ATU_operate()
//Brief    : Operate the entry in the MAC table
//Parameter: 
/******************************************************************************/
//void Marvell_ATU_operate(MAC_OPER oprt,MAC_ENTRY_T * entry,uint8_t move)
//  {

//   switch(oprt)
//	{
//		case READ_ENTRY:     
//		  Read_ATU_entry(entry); 
//		  break;
//		case LOAD_PURGE_ENTRY: 
//		  Load_Purge_ATUentry(entry);
//		  break;
//		case FLUSH_ENTRY: 
//		  Flush_Move_ATUentry(ALL,FLUSH_ENTRY,move);
//		  break;
//		case MOVE_ENTRY:  
//		  Flush_Move_ATUentry(ALL,MOVE_ENTRY,move);
//		  break;
//		default :   break;
//	}
//  }

/******************************************************************************/
//Function : Read_ATU_entry()
//Brief    : Read the specific entry for the MAC address
//Parameter: (Input)__entry : The type is a (struct MAC_ENTRY_T *) only entry.mac_entry_MAC and
//                            entry.mac_entry_fid must be filled
/******************************************************************************/ 
//if MAC_address != {0xff,0xff,0xff,0xff,0xff,0xff},otherwise The_specific_MAC_address
void Read_ATU_entry(MAC_ENTRY_T* entry)
  {  
	uint16_t tem_reg = 0x8000;
//    uint8_t  tag;   //Parameter 'tag' used to judge if the MAC_address are all-ones 		
/*Set the phy address for the  HAL_ETH_ReadPHYRegister() function*/
	Eth_Handle.Init.PhyAddress = GLOBAL_1_ADDR;
/*For we don't use VLAN now check if the register is '0x000',if not  clear it*/
	HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_ATU_FID,&tem_reg);
//    if((tem_reg & ATU_FID_BITS) != RESET)
//      {
		tem_reg &= ~ATU_FID_BITS;
		tem_reg |= entry->mac_entry_fid;
        HAL_ETH_WritePHYRegister(&Eth_Handle,REG_ATU_FID,tem_reg);		
//      }  		
/*Polling the 'ATUBUSY' bit to indicate that operation can proceed*/
    while(tem_reg & ATU_MAC_BUSY)
	  {
		HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_ATU_OPERATION,&tem_reg);
	  }			
/****Then fill in the ATU_MAC_register with the value 'The_specific_MAC_address -1'****/
/*Fill in the REG_ATU_MAC0 register*/		
	tem_reg  = ( entry->mac_entry_MAC[0] <<8 ) & 0xff00;   //MAC[0]
	tem_reg |=   entry->mac_entry_MAC[1]       & 0x00ff;   //MAC[1]
	HAL_ETH_WritePHYRegister(&Eth_Handle,REG_ATU_MAC0,tem_reg);
		
/*Fill in the REG_ATU_MAC1 register*/
	tem_reg =  ( entry->mac_entry_MAC[2] <<8 ) & 0xff00;   //MAC[2]
	tem_reg |=   entry->mac_entry_MAC[3]       & 0x00ff;   //MAC[3]
	HAL_ETH_WritePHYRegister(&Eth_Handle,REG_ATU_MAC1,tem_reg);

/*Fill in the REG_ATU_MAC2 register with the MAC_address -1 */
    tem_reg =  ( entry->mac_entry_MAC[4] <<8 ) & 0xff00;	 //MAC[4]
    tem_reg |=   entry->mac_entry_MAC[5]       & 0x00ff;   //MAC[5]
    HAL_ETH_WritePHYRegister(&Eth_Handle,REG_ATU_MAC2,tem_reg);

/*Fill in the REG_ATU_OPERATION*/
    HAL_ETH_ReadPHYRegister(&Eth_Handle, REG_ATU_OPERATION,&tem_reg);
    tem_reg &= ~ATU_OPER_SET;
    tem_reg |= ATU_GET_NEXT;
    tem_reg |= ATU_MAC_BUSY;
    HAL_ETH_WritePHYRegister(&Eth_Handle, REG_ATU_OPERATION,tem_reg);
		
/*Polling till the ATUBUSY bit is cleared*/
    while(tem_reg & ATU_MAC_BUSY)
	  {
		HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_ATU_OPERATION,&tem_reg);
      }
/*Get the entry data area*/
	HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_ATU_DATA,&tem_reg);
	entry->mac_entry_data  = tem_reg;   
/*Get the entry MAC address*/		 
	HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_ATU_MAC0,&tem_reg); 
	entry->mac_entry_MAC[0] = (tem_reg >>8) & 0x00ff;		 
	entry->mac_entry_MAC[1] = tem_reg       & 0x00ff;
	HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_ATU_MAC1,&tem_reg); 
	entry->mac_entry_MAC[2] = (tem_reg >>8) & 0x00ff;		 
	entry->mac_entry_MAC[3] = tem_reg       & 0x00ff;
	HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_ATU_MAC2,&tem_reg); 
	entry->mac_entry_MAC[4] = (tem_reg >>8) & 0x00ff;		 
	entry->mac_entry_MAC[5] = tem_reg       & 0x00ff;
/*Get the entry FID*/
	HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_ATU_FID,&tem_reg);
	entry->mac_entry_fid  = tem_reg ;
  }//end of the function
	


	
/****************Load or Purge specific entry*****************/
/*If the entry's mac_entry_data[3:0](also the Entrystate bits) are all-zero ,then the operation is a Purge operation*/
/*On the other hand ,the operation is a load operation*/
void Load_Purge_ATUentry(MAC_ENTRY_T* entry)       
  {
	uint16_t tem_reg;
	uint8_t  i;
     
	Eth_Handle.Init.PhyAddress = GLOBAL_1_ADDR;
	tem_reg = entry->mac_entry_fid;
	HAL_ETH_WritePHYRegister(&Eth_Handle,REG_ATU_FID,tem_reg);
/*Fill the entry.mac_entry_data into the REG_ATU_DATA register*/
	tem_reg = entry->mac_entry_data;
	HAL_ETH_WritePHYRegister(&Eth_Handle,REG_ATU_DATA,tem_reg);
/*Fill in the MAC address*/
	for(i=0;i<6;i++)
	  {
		tem_reg  =  (entry->mac_entry_MAC[i])<<8;
		i++;
		tem_reg |=  (entry->mac_entry_MAC[i]) &0x00FF;
		if(i<2)
			HAL_ETH_WritePHYRegister(&Eth_Handle,REG_ATU_MAC0,tem_reg);
		else 
		  if(i<4)
			HAL_ETH_WritePHYRegister(&Eth_Handle,REG_ATU_MAC1,tem_reg);
		  else
			HAL_ETH_WritePHYRegister(&Eth_Handle,REG_ATU_MAC2,tem_reg);
	  }
/*Polling if the MAC BUSY bit is reset,require '0'*/			 
	while(1){
			  HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_ATU_OPERATION,&tem_reg); 
			  if((tem_reg & ATU_MAC_BUSY) == RESET)
				break;
			}
/*Fill in the command and start the operation*/
	tem_reg  =  (uint16_t)((entry->mac_entry_oper)<<8 )& ATU_MAC_PRI_SET;
	tem_reg |=  ATU_LOAD_PURGE;       //ATU_LOAD_PURGE means '0x0B00'
	HAL_ETH_WritePHYRegister(&Eth_Handle,REG_ATU_OPERATION,tem_reg);
	while(1){
			HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_SWITCH_MAC,&tem_reg); 
			if((tem_reg & ATU_MAC_BUSY) != SET)
			break;
			}		 	
  }

	
/***************************Flush all the entries  or move single port mappings************************/
//Function  : Flush_Move_ATUentry()
//Brief     : Flush the ATU entry or Move the specific ports' entrys,also you can do this for a specific fid  
//Parameters: mold :ATU_OPER_MODE enum : used to send the operation mode
//            option : MAC_OPER enum   : used to send the operation choice
//            move   : used for move operation only,to indicate the targeted port and the source port 
//                     'move' consist of  low-4bit 'Fromport',higher-4bit[7:4] 'Toport'
//                      If move the ATU entry from one port to another,just fill in it 
//                      or If remove the ATU entry of a specific port ,fill in the 'Fromport' bits,and set the 'Toport' bits all-ones
//            fid    :  to indicate the fid if needed ,otherwise set '0'
uint8_t Flush_Move_ATUentry(ATU_OPER_MODE mold,MAC_OPER option,uint16_t move,uint16_t fid)
  {
	uint16_t tem_reg;
/*At first set the ATU_FID to 0x00*/		 
	Eth_Handle.Init.PhyAddress = GLOBAL_1_ADDR;
	tem_reg = fid;
	HAL_ETH_WritePHYRegister(&Eth_Handle,REG_ATU_FID,tem_reg);
/*Then fill in the ATU DATA Register*/
	HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_ATU_DATA,&tem_reg);
	if(option == FLUSH_ENTRY)                   //if flush ,the entry_state must be 0x00
	  tem_reg &= ~ ATU_ENTRY_STATE_SET;
	if(option == MOVE_ENTRY)                    //if move ,the entry_state  must be 0xff
	  {  
		tem_reg |= ATU_ENTRY_STATE_SET;			
		tem_reg &= ~ATU_DPV_SET;                    //Clear the bits for the DPV()
		tem_reg |= ((move<<4) & 0x7ff0);
	  }	    
	HAL_ETH_WritePHYRegister(&Eth_Handle,REG_ATU_DATA,tem_reg);	  
/*Calculate the operate command,Then Fill them in The REG_ATU_OPERATION*/				
	while(1){
		    HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_ATU_OPERATION,&tem_reg); 
		    if((tem_reg & ATU_MAC_BUSY) == RESET)
			break;
			}
	switch(mold) {
		case ALL:
		  tem_reg = ATU_FLUSH_MOVE_ALL;
		  break;
		case ALL_NONSTA:
		  tem_reg = ATU_FLUSH_MOVE_UNSTA;
		  break;
		case ALL_FID:
		  tem_reg = ATU_FLUSH_MOVE_INFID;
		  break;
		case ALL_NONSTA_FID:
		  tem_reg = ATU_FLUSH_MOVE_UNSTAFID;
		  break;
		default:
		  return 0;
	}//end of switch
	HAL_ETH_WritePHYRegister(&Eth_Handle,REG_ATU_OPERATION,tem_reg);
/*Polling till the operation is done*/
	while(1){
			HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_ATU_OPERATION,&tem_reg); 
			if((tem_reg & ATU_MAC_BUSY) == RESET)
			  break;
			}
	return 1;
  }

/***********************Function : Set_Manual_Learning_mode()**************************/
//parameter 'port_vector' indicates which port should be configed as Manual learning mode(MAC address)
//
void Set_Manual_learning_mode(uint16_t port_vector)
  {
	uint16_t tem_reg;
	uint16_t tem_vector;
	uint8_t  loop ;
		 
	tem_vector = port_vector;
/*At first ,set all of the availiable ports into manual learning mode (MAC address)*/
	for(loop = 0; loop < 10 ; loop++)
	  {
/*Check if the port's corresponding bit was set*/					
		Eth_Handle.Init.PhyAddress = PORT_0 + loop;
		HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_PORT_ASSOCIATION_VECTOR,&tem_reg);
		if(tem_vector & 0x0001)
		  {
			tem_reg |= PORT_LOCKED_PORT;  //For PORT_PAV = 0x07ff,use this to reset the higher bits of this register 							
		  }
		else
		  { 
			tem_reg &= ~PORT_LOCKED_PORT; 
		  }
	    tem_vector = tem_vector >> 1;						
		HAL_ETH_WritePHYRegister(&Eth_Handle,REG_PORT_ASSOCIATION_VECTOR,tem_reg);	
	  }//The end of the for circle

   } 
	 
/***************************Function : Get_Manual_Learning_mode()***********************/
//parameter 'port_vector' indicates which ports are configed as Manual learning mode(MAC address)
//	
uint16_t Get_Manual_learning_mode()
  {
	uint16_t tem_vector=0;
	uint16_t tem_reg;
	uint8_t loop;
		 
	loop =10;
	while(loop--)
	  {
		tem_vector = tem_vector << 1;
		Eth_Handle.Init.PhyAddress = PORT_0 + loop;
		HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_PORT_ASSOCIATION_VECTOR,&tem_reg);
		if((tem_reg & PORT_LOCKED_PORT) != RESET)
		{  tem_vector |= 0x0001;  }
					
	  }	 
    return tem_vector;
  }	 
/**************************************************************************/
//Functions for the VTU operations
/**************************************************************************/
/*The operation runs as follows:*/
/*At first ,Polling till the VTUbusy bit becomes '0',which means the operations can proceed. 
  associated register : REG_VTU_OPERATION*/
/*Second,Fill in the REG_VTU_DATA and REG_VTU_VID register if necessary*/
/*And then ,Start the operation by filling the register REG_VTU_OPERATION*/
/*Then polling till the operation is done*/
/*If required, read the value you want*/

/******************************Function:  Read_VTU_entry()*****************************/
/*Read the entry of specific VID*/
/*parameter entry:
  entry->ATU_vid 	only bit [12:0] is useful,bit12 refers to VTU_VALID,
  while bits[11:0] refers to the VTU_VID,all-ones indicate you want to find the lowest entry
*/	
void  Read_VTU_entry(VLAN_ENTRY_T *entry)
  {
	uint16_t  tem_reg;
	uint16_t  The_status = 0x8000;
	
/*PHY address*/		
	Eth_Handle.Init.PhyAddress = GLOBAL_1_ADDR;
/*For it's not multi database mode,just let the REG_ATU_FID 0x0fff*/
	HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_VTU_FID,&tem_reg);
	tem_reg |= VTU_FID_SET;		
/*Polling till the VTUBUSY bit becomes '0'*/
	while(The_status & VTU_BUSY )
	  {
		HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_VTU_OPERATION,&The_status);
	  }
/*Fill in the REG_VTU_VID register with entry->VTU_vid */
	tem_reg = entry->VTU_vid & VTU_VID_SET;
//	if(tem_reg != VTU_VID_SET) /*If VID is all-ones ,reduced by one is not required*/
//	  {
//		tem_reg = tem_reg-1;
//	  }
	HAL_ETH_WritePHYRegister(&Eth_Handle,REG_VTU_VID,tem_reg);
/*Fill in the REG_VTU_OPERATION register ,then the operation start*/
	HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_VTU_OPERATION,&tem_reg);
	tem_reg &= ~VTU_OPERATE_RESET;  //RESET the operate bits
	tem_reg |=  VTU_GET_NEXT;       //Fill in the GET_NEXT command
	tem_reg |=  VTU_BUSY;           //Set the VTUBUSY bit to start the read operation
	HAL_ETH_WritePHYRegister(&Eth_Handle,REG_VTU_OPERATION,tem_reg);
/*Polling till the operation is done*/
	The_status = 0x8000;
	while(The_status & VTU_BUSY)
	 {
	   HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_VTU_OPERATION,&The_status);
	 }
	 HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_VTU_OPERATION,&tem_reg);
	 //printf("The operation register:");
	 //printf("%x\n\r",tem_reg);
/*And then, read the value required*/
	HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_VTU_DATA_PORTS3_0,&entry->VTU_data0);
	HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_VTU_DATA_PORTS7_4,&entry->VTU_data1);
	HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_VTU_DATA_PORTSA_8,&entry->VTU_data2);
	HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_VTU_VID,&entry->VTU_vid);
	HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_VTU_FID,&entry->VTU_fid);
	HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_VTU_SID,&entry->VTU_sid);
	entry->VTU_vid &= 0x0fff;
     			 
  }

/******************************Function:transferVLANMember*****************************/
////Brief : Transfer the register data into a vector
//void transferVLANMember(uint16_t *data,uint16_t *vector,uint8_t loop)
//  {
//	if( ( (*data) & VLAN_MEM ) == 0 )
//	  {
//		(*vector) |= 0x0001;
//	  }
//	if(loop != 9)
//	  {
//		(*vector) = (*vector) << 1;
//		(*data) = (*data) << 4;
//	  }
//  }	  
/******************************Function:transferVLANMember*****************************/
//Brief : Transfer the register data into a vector
void transferVLANMember(uint16_t *data,uint8_t *p_mem,uint8_t loop)
  {
	switch( (*data)&VLAN_MEM )
	  {
		case  VLAN_MEM :
			*(p_mem + 9 - loop) = 0x00;
			break;
		case VLAN_UNMOD :
			*(p_mem + 9 - loop) = 0x01;
			break;
		case VLAN_UNTAG :
			*(p_mem + 9 - loop) = 0x02;
			break;
		case VLAN_TAGED :
			*(p_mem + 9 - loop) = 0x03;
			break;
		default :
			break;
	  }
	(*data) = (*data) <<4;
  }	
/*****************************Function :LOAD_PURGE_VTU()****************************/
//parameter 'entry' must be full of messages,before call the function
//if entry->ATU_vid[12](valid bit) '1',the operation is loading,or purging
//	
void LOAD_PURGE_VTU(VLAN_ENTRY_T *entry)
  {
	uint16_t tem_reg;
	uint16_t The_status = 0x8000;
		
/*First ,fill in the PHY address*/
	Eth_Handle.Init.PhyAddress = GLOBAL_1_ADDR;
/*Fill in the REG_VTU_FID with 0x0000*/
	HAL_ETH_WritePHYRegister(&Eth_Handle,REG_VTU_FID,entry->VTU_fid);
/*Fill in the REG_VTU_SID with 0x0000*/
	HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_VTU_SID,&tem_reg);
	tem_reg &= 0xffc0;  //clear the low 6 bits
	HAL_ETH_WritePHYRegister(&Eth_Handle,REG_VTU_SID,tem_reg);
/*Polling till the VTUBUSY bit becomes '0'*/
	while(The_status & VTU_BUSY)
	  { 
		HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_VTU_OPERATION,&The_status);
	  }
/*Nice! Fill in the register REG_VTU_VID*/
	tem_reg = entry->VTU_vid; 
	HAL_ETH_WritePHYRegister(&Eth_Handle,REG_VTU_VID,tem_reg);
/*Fill in the register REG_VTU_DATA*/
	HAL_ETH_WritePHYRegister(&Eth_Handle,REG_VTU_DATA_PORTS3_0,entry->VTU_data0);	
	HAL_ETH_WritePHYRegister(&Eth_Handle,REG_VTU_DATA_PORTS7_4,entry->VTU_data1);
	HAL_ETH_WritePHYRegister(&Eth_Handle,REG_VTU_DATA_PORTSA_8,entry->VTU_data2);
/*Then fill in the operation register to start the operation*/
	HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_VTU_OPERATION,&tem_reg);
	tem_reg &= ~VTU_OPERATE_RESET;       //reset the operation bits
	tem_reg |=  VTU_LOAD_PURGE;          //Fill in the command
	tem_reg |=  VTU_BUSY;                //Fill in the start bit
	HAL_ETH_WritePHYRegister(&Eth_Handle,REG_VTU_OPERATION,tem_reg);
/*Polling till the operation is done*/
	The_status = 0x8000;
	while(The_status & VTU_BUSY)
	  {
		HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_VTU_OPERATION,&The_status);
	  }
  }
/**********************Function :Flush_VTU_entry()**********************/
void Flush_VTU_entry()
  {
	uint16_t tem_reg;
/*Set the PHY address for the WRITE operation*/
	Eth_Handle.Init.PhyAddress  = GLOBAL_1_ADDR;		 
	tem_reg &= ~VTU_OPERATE_RESET;
	tem_reg |=  VTU_FLUSH_ALL;
	tem_reg |=  VTU_BUSY;
		 
	HAL_ETH_WritePHYRegister(&Eth_Handle,REG_VTU_OPERATION,tem_reg);
/*Polling till the operation is done*/		 
	while(tem_reg & VTU_BUSY)
	  {
		HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_VTU_OPERATION,&tem_reg);
	  }
   }
	 
/*Create a new VLAN based on 8021.1Q*/
/***********************Function : Creat_Delet_VLAN()***************************/
//parameter 'ports' represent which port belongs to the VLAN
//parameter 'vlan'  refers to the VLAN ID
//if creat is '1',then it's a creat_vlan function,or a delete
uint8_t Creat_Delet_VLAN(uint16_t vlan, uint8_t *ports,uint8_t creat)
  {
	uint8_t   loop;
	uint16_t  tem_reg;
//	uint16_t  tem_16;
	VLAN_ENTRY_T entry;
/*Get the ports 0~3 values and fill them into the register 'REG_VTU_DATA_PORTS3_0'*/
	tem_reg = VTU_DATA_RESET ;
	for(loop=0 ;loop <4 ;loop++ )
	  {
#ifdef BIT_WAY 
//		if( (ports>>loop) & 0x0001)
//			tem_reg &= VTU_PORT_UNMOD; 
		//ports = ports >> loop;
#endif	
	switch (*(ports+loop))
	   {
		case 0x00 :
			tem_reg &= VTU_PORT_NOMEM ;
			break;
	    case 0x01 :
			tem_reg &= VTU_PORT_UNMOD;
			break;
		case 0x02 :
			tem_reg &= VTU_PORT_TAGED;
			break;
		case 0x03 :
			tem_reg &= VTU_PORT_UNTAG;
			break;
		default   :
			break;
	   }
		if(loop != 3)
		  {
			tem_reg  = tem_reg >>4;
			tem_reg |= 0xf000;
		  }								
	  }
	entry.VTU_data0 = tem_reg;
/*Get the ports 4~7 values and fill them into the register 'REG_VTU_DATA_PORTS7_4'*/
	tem_reg = VTU_DATA_RESET ;
	for(loop=4 ; loop <8 ; loop++)
	  {
#ifdef BIT_WAY
//		if( (ports>>loop) & 0x0001)
//		  tem_reg &= VTU_PORT_UNMOD;
//		//ports = ports >>1;
#endif
	switch (*(ports+loop))
	   {
		case 0x00 :
			tem_reg &= VTU_PORT_NOMEM ;
			break;
	    case 0x01 :
			tem_reg &= VTU_PORT_UNMOD;
			break;
		case 0x02 :
			tem_reg &= VTU_PORT_TAGED;
			break;
		case 0x03 :
			tem_reg &= VTU_PORT_UNTAG;
			break;
		default   :
			break;
	   }
		if(loop != 7)
		  {
			tem_reg  = (tem_reg >>4) ;
			tem_reg |= 0xf000;
		  }         
	  }
	entry.VTU_data1 = tem_reg;	
/*Get the ports 8~10 values and fill them into the register 'REG_VTU_DATA_PORTSA_8'*/
	tem_reg = VTU_DATA_RESET ;
	for(loop=8 ; loop <10 ;loop++)
	  {
	switch (*(ports+loop))
	   {
		case 0x00 :
			tem_reg &= VTU_PORT_NOMEM ;
			break;
	    case 0x01 :
			tem_reg &= VTU_PORT_UNMOD;
			break;
		case 0x02 :
			tem_reg &= VTU_PORT_TAGED;
			break;
		case 0x03 :
			tem_reg &= VTU_PORT_UNTAG;
			break;
		default   :
			break;
	   }
#ifdef BIT_WAY
//		if( (ports>>loop) & 0x0001)
//		  tem_reg &= VTU_PORT_UNMOD;
#endif   
		if(loop != 9)
		  { 
			//ports    = ports >>1;
			tem_reg  = tem_reg >> 4 ;
			tem_reg |= 0xf000;
		  }
	  }
	entry.VTU_data2 = tem_reg >> 8 ;						
/*Fill in the VLAN ID as 'vlan'*/
	tem_reg = vlan & VTU_VID_SET;          
	if(creat)
	  entry.VTU_vid   =  tem_reg | VTU_VALID; //Set the valid bit '1' to indicate that it's load operation
	else 
	  entry.VTU_vid   =  tem_reg;
/*FID must be set 0x0000*/
	//entry.VTU_fid  = 0x0000;
	entry.VTU_fid  = vlan;
	entry.VTU_sid  = 0x0000;
/*Load the entry in*/
	LOAD_PURGE_VTU(&entry);
	return 1;
  }
  
/*****************************************************************************/
//Function  : Set_VLAN_ID(uint16_t vlan_id,int portNo)
//Brief     : Set the port's forced vlan id
//Parameter : vlan_id : the specific vlan id
//	          portNo  : which port you want to force the vlan ID
//retval    :  None
/*****************************************************************************/		
uint8_t Set_VLAN_ID(uint16_t vlan_id,int portNo)
  {
	uint16_t tem_reg;
	
	Eth_Handle.Init.PhyAddress = PORT_0+ portNo;
	HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_PORT_DEFAULT_VLANID,&tem_reg);
	tem_reg &= ~PORT_DEFAULT_VID;
	tem_reg |= vlan_id;
	tem_reg |= PORT_FORCE_VID;
	HAL_ETH_WritePHYRegister(&Eth_Handle,REG_PORT_DEFAULT_VLANID,tem_reg);
	return 1;
  }
 
/*****************************************************************************/
//Function  : Set_VLAN_Port_Type(uint8_t type,int portNo)
//Brief     : 设置vlan端口类型
//Parameter : unaware_or_aware: unaware mode or aware mode
//	          portNo  : The specific port to be set
//retval    : None
/*****************************************************************************/	
uint8_t Set_VLAN_port_Type(SECURITY_LEVEL_T type,int portNo)
  {
	uint16_t tem_reg;
	
	Eth_Handle.Init.PhyAddress = PORT_0 + portNo;
	HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_PORT_CONTROL2,&tem_reg);
	tem_reg &= ~PORT_VLAN_RESET;
	if(type == SECURITY)
	  {
		tem_reg |= PORT_VLAN_MODE_SECURE;
	  }
	else
	  if(type == CHECK)
	    {
		  tem_reg |= PORT_VLAN_MODE_CHECK;
		}
    else
	  if (type == FALLBACK)
	  {
		tem_reg |= PORT_VLAN_MODE_FALLBACK;
	  }
	else
	  {
		tem_reg |= PORT_VLAN_MODE_DISABLED;
	  }
	HAL_ETH_WritePHYRegister(&Eth_Handle,REG_PORT_CONTROL2,tem_reg);
	return 1;
  }

/*****************************************************************************/
//Function  : Set_VLAN_ingress_Type(TAG_T tag,int portNo)
//Brief     : 设置vlan过滤类型
//Parameter : 
//retval    : '1'表示操作成功
/*****************************************************************************/	
uint8_t Set_VLAN_ingress_Type(TAG_T tag,int portNo)
  {
	uint16_t tem_reg;
	
	HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_PORT_CONTROL2,&tem_reg);
	tem_reg &= ~( PORT_DISCARD_TAGED | PORT_DISCARD_UNTAG );
	if (tag == TAGGED)
	  {
		tem_reg |= PORT_DISCARD_UNTAG;
	  }
	else
	  if(tag == UNTAGGED)
	  {
		tem_reg |= PORT_DISCARD_TAGED;
	  }
	else
	  {}
	HAL_ETH_WritePHYRegister(&Eth_Handle,REG_PORT_CONTROL2,tem_reg);
	return 1;
  }
  
/*****************************************************************************/
//Function  : Set_VLAN_egress_Type(TAG_T tag,int portNo)
//Brief     : 设置输出frame类型
//Parameter : 
//retval    : None
/*****************************************************************************/	
void Set_VLAN_egress_Type( EGRESS_TYPE_T tag , int portNo,uint16_t vlan )
  {
//  Creat_Delet_VLAN(uint16_t vlan, uint16_t ports,uint8_t creat)
//     VLAN_ENTRY_T  entrys;
//	 
//	 entrys.VTU_vid = 
//	
  }
/************************Function : Set_PORT_VLAN_VECTOR()****************************/	
//Port based VLAN
//parameter 'port_vector' must keep its high  5-bit '0',for there are 0~10 port available in 88e6097F
void Set_PORT_VLAN_VECTOR(uint16_t port_vector)
  {
	uint16_t tem_reg ;
	uint16_t x;
	uint16_t transit;
/*set the PHY address as PORT_0,and the increased by one to set the higher port address*/
	Eth_Handle.Init.PhyAddress = PORT_0;
	for (x=1 ;x<11 ;x++ )
	  {
		HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_PORTBASE_VLAN_MAP,&tem_reg);
		transit = 0x0001 << (x-1);                  //Get a value with the corresponding PORT bit set
		if(port_vector & transit)                   //Check if the port contianed in the VLAN vector
		  {
			tem_reg = port_vector & (~ transit);    //Contained, clear the corresponding bit
			HAL_ETH_WritePHYRegister(&Eth_Handle,REG_PORTBASE_VLAN_MAP,tem_reg);
		  }
		Eth_Handle.Init.PhyAddress = PORT_0 + x;
	  }	
	 
  }	



/***********************************************************************/
//Functions for the STATS COUNTER operations
/***********************************************************************/
/****************************Function :Clear_ALL_COUNTER***************************/
//parameter 'port' indicates that the function is a clear all counters or a clear one port's counters
//if 'port' = 0xff ,then clear all of the counters.Otherwise,clear the indicated port's counters('port'must be 0x00~0x0a)
void Clear_ALL_COUNTER(uint8_t port)
  {
	uint16_t  tem_reg;
	uint16_t  The_status = 0x8000;		 
	Eth_Handle.Init.PhyAddress = GLOBAL_1_ADDR;
/*Polling till the stats is not busy*/      
	while(The_status & STATS_BUSY)
	  {
		HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_STATS_OPERATION,&The_status);
	  }
/*Then Fill in the operation code*/
	HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_STATS_OPERATION,&tem_reg);
	tem_reg &= ~STATS_OP_RESET;         //Clear the operation bits
	tem_reg &= ~STATS_PORT_BITS;        //Clear the statsport bits
	tem_reg &= ~STATS_PTR;              //Clear the statptr bits because the operation need the ptr bits to indicates the desired port 
	if( port == 0xff )
	  tem_reg |= STATS_FLUSH_ALL;			
	else
	  {
		tem_reg |= STATS_FLUSH_PORT;  // Set the flush only one port command
		tem_reg |= ((port+1) << 5) ;  //'<< 5' to align the bits for the indicated port
		tem_reg |= (port & STATS_PTR);//Set the stats_ptr bits
	  }
	tem_reg |= STATS_BUSY;              //Start bit
	HAL_ETH_WritePHYRegister(&Eth_Handle,REG_STATS_OPERATION,tem_reg);
/*Polling till the operation is done*/
	The_status = 0x8000;
	while(The_status & STATS_BUSY)
	  {
		HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_STATS_OPERATION,&The_status);
	  }
  }


/*************************Function :Read_A_CAPTURED_COUNTER()**************************/
void Read_A_CAPTURED_COUNTER(uint32_t *value,uint8_t port,uint8_t counter_addr)
  {
	uint16_t tem_reg;
	uint16_t The_status= 0x8000;
		  
	Eth_Handle.Init.PhyAddress = GLOBAL_1_ADDR; 
/*Polling till the operation is avialiable*/
	while(The_status & STATS_BUSY)
	  {
		HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_STATS_OPERATION,&The_status);
	  }
	HAL_ETH_WritePHYRegister(&Eth_Handle,REG_STATS_OPERATION,0x0c00);
/*Fill in the operation code to capture the port's counters*/
	HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_STATS_OPERATION,&tem_reg);
	tem_reg &= ~STATS_OP_RESET;         //Clear the operation bits
	tem_reg &= ~STATS_PORT_BITS;        //Clear the statsport bits
	tem_reg &= ~STATS_PTR;              //Clear the stats_ptr bits which indicates the counter's address
	tem_reg |= STATS_READ_A_COUNTER;
	tem_reg |= ((port+1) << 5);
	tem_reg |= (counter_addr & 0x1f);
	tem_reg |= STATS_BUSY;
	HAL_ETH_WritePHYRegister(&Eth_Handle,REG_STATS_OPERATION,tem_reg);
/*Polling till the operation is done*/
	The_status = 0x8000;
	while(The_status & STATS_BUSY)
	  {
		HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_STATS_OPERATION,&The_status);
	  }
	HAL_ETH_WritePHYRegister(&Eth_Handle,REG_STATS_OPERATION,0x0c00);
/*Get the value back*/
	HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_STATS_DATA_BYTES3_2,&tem_reg);
	(*value) = tem_reg;                     //Get the high 16 bits
	HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_STATS_DATA_BYTES1_0,&tem_reg);
	(*value) = ((*value)<< 16) | tem_reg;   //Get the low  16 bits
  }	 
	 
	 
/*********************Function :Read_PORT_COUNTERs******************/
//parameter 'stats'     used to get the counters value back
//parameter 'port'      indicate which port's counter you want to get 	 
void Read_PORT_COUNTERs(STATS_COUNTER* stats,uint8_t port)
  {
	uint16_t tem_reg;
	uint16_t The_status = 0x8000;
       
	Eth_Handle.Init.PhyAddress = GLOBAL_1_ADDR;		 
//first step , capture the counters of the port
/*Polling till the operation is avialiable*/
	while(The_status & STATS_BUSY)
	  {
		HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_STATS_OPERATION,&The_status);
	  }
	HAL_ETH_WritePHYRegister(&Eth_Handle,REG_STATS_OPERATION,0x0c00); 
			 
/*Fill in the operation code to capture the port's counters*/
	HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_STATS_OPERATION,&tem_reg);
	tem_reg &= ~STATS_OP_RESET;         //Clear the operation bits
	tem_reg &= ~STATS_PORT_BITS;        //Clear the statsport bits
	tem_reg &= ~STATS_PTR;              //Clear the stats_ptr bits		 
	tem_reg |=  STATS_CAPTURE_PORT_COUNTERS;
	tem_reg |= ((port+1) << 5);         //Set the statsport bits 
	tem_reg |= (port & STATS_PTR);      //When capture all counters for a disired port or flush all counters for a port,these bits must fill in the port number
		tem_reg |= STATS_BUSY;
	HAL_ETH_WritePHYRegister(&Eth_Handle,REG_STATS_OPERATION,tem_reg);
/*Polling till the operation is done*/      
	The_status = 0x8000;
	while(The_status & STATS_BUSY)
	  {
	HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_STATS_OPERATION,&The_status);
	  }
	HAL_ETH_WritePHYRegister(&Eth_Handle,REG_STATS_OPERATION,0x0c00);			 
//The second , get the counters values out		 
	Read_A_CAPTURED_COUNTER(&(stats->InGoodOctetsLo),port,REG_INGOODLO);
	Read_A_CAPTURED_COUNTER(&(stats->InGoodOctetsHi),port,REG_INGOODHI);
	Read_A_CAPTURED_COUNTER(&(stats->InBadOctets),port,REG_INBAD);
	Read_A_CAPTURED_COUNTER(&(stats->InUnicast ),port,REG_INUNICAST);
	Read_A_CAPTURED_COUNTER(&(stats->InBroadcast),port,REG_INBROADCAST);
	Read_A_CAPTURED_COUNTER(&(stats->InMulticast),port,REG_INMULTICAST);
	Read_A_CAPTURED_COUNTER(&(stats->InPause),port,REG_INPAUSE);
	Read_A_CAPTURED_COUNTER(&(stats->InUndersize),port,REG_INUNDERSIZE);
	Read_A_CAPTURED_COUNTER(&(stats->InFragment),port,REG_INFRAGMENT);
	Read_A_CAPTURED_COUNTER(&(stats->InOversize),port,REG_INOVERSIZE);
	Read_A_CAPTURED_COUNTER(&(stats->InJabber),port,REG_INJABBER);
	Read_A_CAPTURED_COUNTER(&(stats->InRxErr),port,REG_INRXERR);
	Read_A_CAPTURED_COUNTER(&(stats->InFCSErr),port,REG_INFCSERR);
	Read_A_CAPTURED_COUNTER(&(stats->OutOctetsLo),port,REG_OUTOCTETLO);
	Read_A_CAPTURED_COUNTER(&(stats->OutOctetsHi),port,REG_OUTOCTETHI);
	Read_A_CAPTURED_COUNTER(&(stats->OutUnicast),port,REG_OUTUNICAST);
	Read_A_CAPTURED_COUNTER(&(stats->OutBroadcast),port,REG_OUTBROADCAST);
	Read_A_CAPTURED_COUNTER(&(stats->OutMulticast),port,REG_OUTMULTICAST);
	Read_A_CAPTURED_COUNTER(&(stats->OutPause),port,REG_OUTPAUSE);
	Read_A_CAPTURED_COUNTER(&(stats->Deferred),port,REG_DEFERRED);
	Read_A_CAPTURED_COUNTER(&(stats->Collisions),port,REG_COLLISION);
	Read_A_CAPTURED_COUNTER(&(stats->Single),port,REG_SINGLE);
	Read_A_CAPTURED_COUNTER(&(stats->Multiple),port,REG_MULTIPLE);
	Read_A_CAPTURED_COUNTER(&(stats->Excessive),port,REG_EXCESSIVE);
	Read_A_CAPTURED_COUNTER(&(stats->Late),port,REG_LATE);
	Read_A_CAPTURED_COUNTER(&(stats->OutFCSErr),port,REG_OUTFCSERR );
	Read_A_CAPTURED_COUNTER(&(stats->Octets_64),port,REG_OCTET64);
	Read_A_CAPTURED_COUNTER(&(stats->Octets_65to127),port,REG_OCTET65_127);
	Read_A_CAPTURED_COUNTER(&(stats->Octets_128to255),port,REG_OCTET128_255);
	Read_A_CAPTURED_COUNTER(&(stats->Octets_256to511),port,REG_OCTET256_511);
	Read_A_CAPTURED_COUNTER(&(stats->Octets_512to1023),port,REG_OCTET512_1023);
	Read_A_CAPTURED_COUNTER(&(stats->Octets_1024toMAX),port,REG_OCTET1024_MAX);

  }
/*****************************************************************************/
//Function  :  Get_Port_Counters_rou(API_ROUGH_COUNTER_T* stats,uint8_t port)	
//Brief     :  Get the rough counters for the indicated port 
//Parameter :	 'stats' point to struct API_ROUGH_COUNTER_T which used to contain the gained counter value
//	           'port' refers the indicated port
//retval    :  None
/*****************************************************************************/	 
void Get_Port_Counters_rou(volatile API_ROUGH_COUNTER_T* stats,int portNo)
  {
	uint16_t tem_reg;
	uint16_t The_status = 0x8000;
	uint32_t tem_counter;
       
	Eth_Handle.Init.PhyAddress = GLOBAL_1_ADDR;		 
//First,capture all counters of the port
/*Polling till the operation is avialiable*/
	while(The_status & STATS_BUSY)
	  {
		HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_STATS_OPERATION,&The_status);
	  }
/*Fill in the operation code for capturing the port's counters*/
	HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_STATS_OPERATION,&tem_reg);
	tem_reg &= ~STATS_OP_RESET;             //Clear the operation bits
	tem_reg &= ~STATS_PORT_BITS;            //Clear the statsport bits
	tem_reg &= ~STATS_PTR;                  //Clear the stats_ptr bits		 
	tem_reg |=  STATS_CAPTURE_PORT_COUNTERS;//Set the operation bits
	tem_reg |= ((portNo+1) << 5);             //Set the statsport bits 
	tem_reg |= (portNo & STATS_PTR);          //When capture all counters for a disired port or flush all counters for a port,these bits must fill in the port number
	tem_reg |= STATS_BUSY;                  //Set the start bit
	HAL_ETH_WritePHYRegister(&Eth_Handle,REG_STATS_OPERATION,tem_reg);
/*Polling till the operation is done*/      
	The_status = 0x8000;
	while(The_status & STATS_BUSY)
	  {
		HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_STATS_OPERATION,&The_status);
	  }
//Second,Get the value of the captured counters
/*Get the 'RxGoodOctets' value*/
	Read_A_CAPTURED_COUNTER(&tem_counter,portNo,REG_INGOODHI);
	stats->RxGoodOctets = tem_counter;
	Read_A_CAPTURED_COUNTER(&tem_counter,portNo,REG_INGOODLO);
	stats->RxGoodOctets = ( stats->RxGoodOctets << 32 ) | tem_counter;
/*Get the 'RxbadOctets' value*/
	Read_A_CAPTURED_COUNTER(&stats->RxbadOctets,portNo,REG_INBAD);			 
/*Get the 'TxGoodOctets' value*/
	Read_A_CAPTURED_COUNTER(&tem_counter,portNo,REG_OUTOCTETHI);
	stats->TxGoodOctets = tem_counter;
	Read_A_CAPTURED_COUNTER(&tem_counter,portNo,REG_OUTOCTETLO);
	stats->TxGoodOctets = ( stats->TxGoodOctets <<32 ) | tem_counter;
/*PS: The Indiscard counter can be read directly from the corresponding registers*/
	Eth_Handle.Init.PhyAddress = PORT_0 + portNo;
/*Get the 'RxDiscard' value*/
	HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_INDISCARDS_nFRAME_HI,&tem_reg);
	stats->RxDiscard = tem_reg;
	HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_INDISCARDS_nFRAME_LO,&tem_reg); 
	stats->RxDiscard = (stats->RxDiscard << 16) | tem_reg;
/*Get the 'RxFiltered' value*/
	HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_INFILTERED_nFRAME,&stats->RxFiltered);
/*Get the 'TxFiltered' value*/
	HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_OUTFILTERED_nFRAME,&stats->TxFiltered);		
	 
  }/*end of the function Get_Port_Counters_rou*/

/*****************************************************************************/
//Function  :  Get_Port_Counters_rou(API_ROUGH_COUNTER_T* stats,uint8_t port)	
//Brief     :  Get the rough counters for the indicated port 
//Parameter :	 'stats' point to struct API_ROUGH_COUNTER_T which used to contain the gained counter value
//	           'port' refers the indicated port
//retval    :  None
/*****************************************************************************/	 
void Get_Port_Counters_det(API_DETAIL_COUNTER_T* stats,int portNo)
  {
	uint16_t tem_reg;
	uint16_t The_status = 0x8000;
	uint32_t tem_counter;
       
	Eth_Handle.Init.PhyAddress = GLOBAL_1_ADDR;		 
//First,capture all counters of the port
/*Polling till the operation is avialiable*/
	while(The_status & STATS_BUSY)
	  {
		HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_STATS_OPERATION,&The_status);
	  }
/*Fill in the operation code for capturing the port's counters*/
	HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_STATS_OPERATION,&tem_reg);
	tem_reg &= ~STATS_OP_RESET;             //Clear the operation bits
	tem_reg &= ~STATS_PORT_BITS;            //Clear the statsport bits
	tem_reg &= ~STATS_PTR;                  //Clear the stats_ptr bits		 
	tem_reg |=  STATS_CAPTURE_PORT_COUNTERS;//Set the operation bits
	tem_reg |= ((portNo+1) << 5);             //Set the statsport bits 
	tem_reg |= (portNo & STATS_PTR);          //When capture all counters for a disired port or flush all counters for a port,these bits must fill in the port number
	tem_reg |= STATS_BUSY;                  //Set the start bit
	HAL_ETH_WritePHYRegister(&Eth_Handle,REG_STATS_OPERATION,tem_reg);
/*Polling till the operation is done*/      
	The_status = 0x8000;
	while(The_status & STATS_BUSY)
	  {
		HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_STATS_OPERATION,&The_status);
	  }
//Second,Get the value of the captured counters
/*Get the 'RxGoodOctets' value*/
	Read_A_CAPTURED_COUNTER(&tem_counter,portNo,REG_INGOODHI);
	stats->RxGoodOctets = tem_counter;
	Read_A_CAPTURED_COUNTER(&tem_counter,portNo,REG_INGOODLO);
	stats->RxGoodOctets = ( stats->RxGoodOctets << 32 ) | tem_counter;
/*Get the 'RxbadOctets' value*/
	Read_A_CAPTURED_COUNTER(&stats->RxbadOctets,portNo,REG_INBAD);
/*set 2*/			 
	Read_A_CAPTURED_COUNTER(&(stats->RxUnicast ),portNo,REG_INUNICAST);
	Read_A_CAPTURED_COUNTER(&(stats->RxBroadcasts),portNo,REG_INBROADCAST);
	Read_A_CAPTURED_COUNTER(&(stats->RxMulticasts),portNo,REG_INMULTICAST);
	Read_A_CAPTURED_COUNTER(&(stats->RxPause),portNo,REG_INPAUSE);
	stats->RxFrames = stats->RxUnicast + stats->RxBroadcasts + stats->RxMulticasts +stats->RxPause;
/*set 3*/			 
	Read_A_CAPTURED_COUNTER(&(stats->RxUndersize),portNo,REG_INUNDERSIZE);
	Read_A_CAPTURED_COUNTER(&(stats->RxFragments),portNo,REG_INFRAGMENT);
	Read_A_CAPTURED_COUNTER(&(stats->RxOversize),portNo,REG_INOVERSIZE);
	Read_A_CAPTURED_COUNTER(&(stats->RxJabber),portNo,REG_INJABBER);
	Read_A_CAPTURED_COUNTER(&(stats->RxErr),portNo,REG_INRXERR);
	Read_A_CAPTURED_COUNTER(&(stats->RxFCSErr),portNo,REG_INFCSERR);
		/*Get the 'TxGoodOctets' value*/
	Read_A_CAPTURED_COUNTER(&tem_counter,portNo,REG_OUTOCTETHI);
	stats->TxGoodOctets = tem_counter;
	Read_A_CAPTURED_COUNTER(&tem_counter,portNo,REG_OUTOCTETLO);
	stats->TxGoodOctets = ( stats->TxGoodOctets <<32 ) | tem_counter;
/*set 6*/			 
	Read_A_CAPTURED_COUNTER(&(stats->TxUnicast),portNo,REG_OUTUNICAST);
	Read_A_CAPTURED_COUNTER(&(stats->TxBroadcasts),portNo,REG_OUTBROADCAST);
	Read_A_CAPTURED_COUNTER(&(stats->TxMulticasts),portNo,REG_OUTMULTICAST);
	Read_A_CAPTURED_COUNTER(&(stats->TxPause),portNo,REG_OUTPAUSE);
	stats->TxFrames = stats->TxUnicast +stats->TxBroadcasts + stats->TxMulticasts + stats->TxPause;
/*set 7*/
	Read_A_CAPTURED_COUNTER(&(stats->TxDeferred),portNo,REG_DEFERRED);
	Read_A_CAPTURED_COUNTER(&(stats->TxCollisions),portNo,REG_COLLISION);
	Read_A_CAPTURED_COUNTER(&(stats->TxSingle),portNo,REG_SINGLE);
	Read_A_CAPTURED_COUNTER(&(stats->TxMultiple),portNo,REG_MULTIPLE);
	Read_A_CAPTURED_COUNTER(&(stats->TxExcessive),portNo,REG_EXCESSIVE);
	Read_A_CAPTURED_COUNTER(&(stats->TxLate),portNo,REG_LATE);
	Read_A_CAPTURED_COUNTER(&(stats->TxFCSErr),portNo,REG_OUTFCSERR );
/*set 4*/
	Read_A_CAPTURED_COUNTER(&(stats->Octets_64),portNo,REG_OCTET64);
	Read_A_CAPTURED_COUNTER(&(stats->Octets_65to127),portNo,REG_OCTET65_127);
	Read_A_CAPTURED_COUNTER(&(stats->Octets_128to255),portNo,REG_OCTET128_255);
	Read_A_CAPTURED_COUNTER(&(stats->Octets_256to511),portNo,REG_OCTET256_511);
	Read_A_CAPTURED_COUNTER(&(stats->Octets_512to1023),portNo,REG_OCTET512_1023);
	Read_A_CAPTURED_COUNTER(&(stats->Octets_1024toMAX),portNo,REG_OCTET1024_MAX);			 

/*PS: The Indiscard counter can be read directly from the corresponding registers*/
	Eth_Handle.Init.PhyAddress = PORT_0 + portNo;
/*Get the 'RxDiscard' value*/
	HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_INDISCARDS_nFRAME_HI,&tem_reg);
	stats->RxDiscard = tem_reg;
	HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_INDISCARDS_nFRAME_LO,&tem_reg); 
	stats->RxDiscard = (stats->RxDiscard << 16) | tem_reg;
/*Get the 'RxFiltered' value*/
	HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_INFILTERED_nFRAME,&stats->RxFiltered);
/*Get the 'TxFiltered' value*/
	HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_OUTFILTERED_nFRAME,&stats->TxFiltered);		
			 
  }/*end of the function Get_Port_Counters_det*/
/*****************Function :Change_COUNTER_mode(uint8_t histogram)****************/
//parameter 'histogram' only bit[1:0] valid,used to show 64bytes registers as ingress registers or egress registers ,or both
void Change_COUNTER_mode(uint8_t histogram)
  {
	uint16_t tem_reg;
	uint16_t The_status= 0x8000;
		
	Eth_Handle.Init.PhyAddress = GLOBAL_1_ADDR;
/*Polling to ensure the stats_busy bit '0'*/
	while (The_status & STATS_BUSY)
	  {
		HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_STATS_OPERATION,&The_status);
	  }
/*change the histogram bits*/
	HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_STATS_OPERATION,&tem_reg);
	tem_reg &= ~STATS_HISTOGRAM_MODE;
	tem_reg |= ((histogram << 10) & STATS_HISTOGRAM_MODE);
	HAL_ETH_WritePHYRegister(&Eth_Handle,REG_STATS_OPERATION,tem_reg);
  }

/*****************Function :getCounterMode()****************/
uint8_t getCounterMode()
  {
	uint16_t tem_reg;
	Eth_Handle.Init.PhyAddress = GLOBAL_1_ADDR;
	HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_STATS_OPERATION,&tem_reg);
		
    return ((tem_reg >> 10)& 0x0003);
  }



	
/****************************************************************************************
Functions : Set_Trunk_port() 	
*****************************************************************************************/
//parameter (input): 'port_vector' only [10:0] bits availiable,bit[0] refers to port_0,
//					 and a '1' indicates that port_0 is part of the trunk,bit[1] for port_1, and so on.
//					 PS: At most 8 port members can be added to the trunk 
//parameter (input): 'trunk_id' can not exceed 16.       	
MYERROR_INFO 
Set_Trunk_port(uint16_t port_vector,uint16_t trunk_id)
  {
	uint16_t tem_reg;
	uint16_t loop;	
//     uint16_t tem_vector;
/*Save the port_vector,s value, for the Second step use*/		
//		 tem_vector = port_vector;
/*Check if the port already exit in the other trunk*/
	if((trunk_vector & port_vector) != port_vector) 
	  {
		return PORT_EXIT_IN_OTHER_TRUNK;
	  }			 
	trunk_vector &= (~port_vector);               //No port exit in the other trunk,add the port here into the trunk_vector

/*Set each port member*/
	for( loop =0 ; loop < 11 ; loop ++)
	  {
		Eth_Handle.Init.PhyAddress = PORT_0 + loop;
		if(port_vector & 0x0001)  //Check if bit[loop] is ture to be a trunk member
		  {
			HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_PORT_CONTROL1,&tem_reg);
			tem_reg &= ~PORT_TRUNK_ID;    //Clear the trunk id bits
			tem_reg |= PORT_TRUNK_BIT;    //Set the trunk enable bit
			tem_reg |= (trunk_id << 8);   //Set the trunk id	
			tem_reg &= 0xff00;  							 
			HAL_ETH_WritePHYRegister(&Eth_Handle,REG_PORT_CONTROL1,tem_reg);
#ifdef MY_DEBUG
			HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_PORT_CONTROL1,&tem_reg);
			printf("After write in the register:");
			printf("%x \n\r",tem_reg);
#endif							 
		  }    
		port_vector = port_vector >> 1;
	  }

	return NO_ERROR;
  }
/******************************Function  : Set_trunk_table()**********************/
//parameter 'port_vector' (input) : Used to transit which port belongs to this trunk
MYERROR_INFO
Set_TRUNK_TABLE(uint16_t port_vector,uint16_t trunk_id)
  { 
	uint16_t tem_reg;          //to hold the value of the register
	uint16_t  loop;            //control the circle 
	uint16_t  loop_tag = 0;    //work with the loop parameter
	uint16_t  tag = 0;         //To count the members of the trunk port
	uint16_t mask[8];          //Contain the trunk mask entry temporarily
	uint16_t tem_vector;
/*Save the port_vector for further use*/		 
	tem_vector = port_vector;
/*At first, gain the entry for the trunk mask table temporarily*/		 
	for(loop =0 ;loop < 11 ;loop++ )
	  {
		if (port_vector & 0x0001)            
		  {
			mask [tag++] = (0x0001 << loop);
		  }
		port_vector = port_vector >> 1;
		if(tag > 8)
			return THE_TRUNK_MEMBER_INVALID;
	  }
/*Then fill in the mask table with the entry vector gained here*/
	Eth_Handle.Init.PhyAddress = GLOBAL_2_ADDR;
	for(loop = 0 ;loop <8 ;loop ++)
	  {
		if(loop_tag >= tag)
			loop_tag = 0;
		mask_table[loop]  &= ~tem_vector;
		mask_table[loop]  |= mask[loop_tag++]; //calculate the final entry that will load into the trunk table
        
/*Then the mask_table contian the final mask entrys*/				   
		HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_TRUNK_MASK,&tem_reg);
		tem_reg &=  ~TRUNK_TABLE_MASKNUM;     //Clear the masknum bits
		tem_reg &=  ~TRUNK_TABLE_MASK_ENTRY;  //Clear the entry bits 
		tem_reg &=  ~TRUNK_TABLE_MASK_HASH;   //Clear the hash bit
		tem_reg |= ((loop  << 12)      & TRUNK_TABLE_MASKNUM);     //Fill in the tem_reg with specific values
		printf("%x \n\r",loop <<12);
		tem_reg |= (0x0001 << 11)      & TRUNK_TABLE_MASK_HASH;    //
		tem_reg |= ((mask_table[loop]) & TRUNK_TABLE_MASK_ENTRY);  //
		tem_reg |= TRUNK_TABLE_UPDATE;                             //
		HAL_ETH_WritePHYRegister(&Eth_Handle,REG_TRUNK_MASK,tem_reg);   
		while(tem_reg & TRUNK_TABLE_UPDATE)                        //Wait till the update operation is done
		  {
			HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_TRUNK_MASK,&tem_reg);
		  }
	  }
/*Set trunk member table*/		
	Eth_Handle.Init.PhyAddress = GLOBAL_2_ADDR;
	//HAL_ETH_ReadPHYRegister(&heth,REG_TRUNK_MEMBERS,&tem_reg);
	tem_reg  = TRUNK_UPDATE;                  //Reset the bits of this register except TRUNK_UPDATE bit 
	tem_reg |= ((trunk_id <<11) & TRUNK_ID);   //Set trunk_id bits
	tem_reg |= (tem_vector & TRUNK_MEMBERS);  //Set trunk_members bits
	tem_reg |= TRUNK_UPDATE;                   //Set update bit 
	HAL_ETH_WritePHYRegister(&Eth_Handle,REG_TRUNK_MEMBERS,tem_reg);
/*Polling till the update is done*/
	while(tem_reg & TRUNK_UPDATE)
	  {
		HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_TRUNK_MEMBERS,&tem_reg);
	  }
	return NO_ERROR;				 
  }
	
/*******************Function : Clear_a_trunk()***********************/
MYERROR_INFO
Clear_A_trunk(uint16_t port_vector,uint16_t trunk_id)
  {
	uint16_t tem_reg;
	uint8_t  loop;
	uint16_t tem_vector,tem_vector1;
/*Store the port_vector for further use*/			 
	tem_vector = tem_vector1 = port_vector;
/*Check if the trunk's port members exit in the 'trunk_vector'*/
	if ((trunk_vector & port_vector) != 0)
	  {
		return THE_TRUNK_ISNT_EXIT;
	  }
/*Check if the port_vector's member is the trunk port with the right trunk_id*/
	for(loop =0 ;loop < MAX_PORT_NUM ; loop++ )
	  {
		Eth_Handle.Init.PhyAddress  = PORT_0 + loop;
		if(port_vector & 0x0001)
		  {
			HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_PORT_CONTROL1,&tem_reg);
			if((tem_reg & PORT_TRUNK_BIT) == 0)
			  {
				return THE_TRUNK_PORT_ERROR;
			  }
			if( ((tem_reg & PORT_TRUNK_ID) >> 8) != trunk_id)
			  {
				return THE_TRUNK_ID_NOT_MATCH;
			  }
		  }
		port_vector = port_vector >> 1;
	  }
/*Check OK,Clear the corresponding bits in the 'trunk_vector'*/
	trunk_vector |= tem_vector;
/*Clear the port member's register*/
	for(loop = 0;loop< MAX_PORT_NUM ;loop ++)
	  {
		Eth_Handle.Init.PhyAddress = PORT_0 + loop;
		if(tem_vector & 0x0001)
		  {
			HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_PORT_CONTROL1,&tem_reg);
			tem_reg &= ~PORT_TRUNK_BIT;
			tem_reg &= ~PORT_TRUNK_ID;
			HAL_ETH_WritePHYRegister(&Eth_Handle,REG_PORT_CONTROL1,tem_reg);
		  }
		tem_vector = tem_vector >> 1;
	  }
/*Clear the mask table value*/
	Eth_Handle.Init.PhyAddress = GLOBAL_2_ADDR;
	for(loop =0;loop <8 ;loop ++)
	  {
		mask_table[loop] |= tem_vector1;
		HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_TRUNK_MASK,&tem_reg);
		tem_reg &=  ~TRUNK_TABLE_MASKNUM;     //Clear the masknum bits
		tem_reg &=  ~TRUNK_TABLE_MASK_ENTRY;  //Clear the entry bits 
		tem_reg &=  ~TRUNK_TABLE_MASK_HASH;   //Clear the hash bit
		tem_reg |= ((loop  << 12)      & TRUNK_TABLE_MASKNUM);     //Fill in the tem_reg with specific values
		tem_reg |= (0x0001 << 11)      & TRUNK_TABLE_MASK_HASH;    //
		tem_reg |= ((mask_table[loop]) & TRUNK_TABLE_MASK_ENTRY);  //
		tem_reg |= TRUNK_TABLE_UPDATE;                             //
		HAL_ETH_WritePHYRegister(&Eth_Handle,REG_TRUNK_MASK,tem_reg);   
		while(tem_reg & TRUNK_TABLE_UPDATE)                        //Wait till the update operation is done
		  {
			HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_TRUNK_MASK,&tem_reg);
		  }					
	  }
/*Clear the register REG_TRUNK_MEMBER*/
	HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_TRUNK_MEMBERS,&tem_reg);
	tem_reg &= ~TRUNK_ID;
	tem_reg |= ((trunk_id << 11) & TRUNK_ID);
	tem_reg &= TRUNK_MEMBERS;	
	HAL_ETH_WritePHYRegister(&Eth_Handle,REG_TRUNK_MEMBERS,tem_reg);
	while(tem_reg & TRUNK_TABLE_UPDATE)                        //Wait till the update operation is done
	  {
		HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_TRUNK_MEMBERS,&tem_reg);
	  }	
	return NO_ERROR;
}

/*Clear all trunks*/
void Clear_ALL_trunks()
  { 
	uint16_t tem_reg;
	uint8_t  loop;

/*Clear the trunk bit if the port is recorded in the trunk_vector*/
	for(loop = 0;loop < MAX_PORT_NUM ;loop ++)
	  {
		Eth_Handle.Init.PhyAddress = PORT_0 + loop;
		if(trunk_vector & 0x0001)
		  {
			HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_PORT_CONTROL1,&tem_reg);
			tem_reg &= ~PORT_TRUNK_BIT;
			tem_reg &= ~PORT_TRUNK_ID;
			HAL_ETH_WritePHYRegister(&Eth_Handle,REG_PORT_CONTROL1,tem_reg);
		  }
		trunk_vector = trunk_vector >> 1;
	  }
/*Clear the mask table value*/
	Eth_Handle.Init.PhyAddress = GLOBAL_2_ADDR;
	for(loop =0;loop <8 ;loop ++)
	  {
		mask_table[loop] |= 0x07ff;
		HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_TRUNK_MASK,&tem_reg);
		tem_reg &=  ~TRUNK_TABLE_MASKNUM;     //Clear the masknum bits
		tem_reg &=  ~TRUNK_TABLE_MASK_ENTRY;  //Clear the entry bits 
		tem_reg &=  ~TRUNK_TABLE_MASK_HASH;   //Clear the hash bit
		tem_reg |= ((loop  << 12)      & TRUNK_TABLE_MASKNUM);     //Fill in the tem_reg with specific values
		tem_reg |= TRUNK_TABLE_UPDATE;                             //
		HAL_ETH_WritePHYRegister(&Eth_Handle,REG_TRUNK_MASK,tem_reg);   
		while(tem_reg & TRUNK_TABLE_UPDATE)                        //Wait till the update operation is done
		  {
		    HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_TRUNK_MASK,&tem_reg);
		  }					
	  }					
  }
	
/**
//HAL functions for the RSTP protocal	
**/
/******************************************************************************/  
//Bottom Functions for the RSTP protocal
//Brief : set the state of the port 
/******************************************************************************/
/******************* Set_Switch_Port_state()*****************/
void Set_Switch_Port_state(int portNo,RSTP_PORT_STATE state)
  {

	uint16_t tem_reg;
	
/*set the PHY address*/
	Eth_Handle.Init.PhyAddress = PORT_0 + portNo;
	HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_PORT_CONTROL,&tem_reg);
/*Clear the bit 0 and bit 1 in REG_PORT_CONTROL*/
	tem_reg &= 0xFFFC; 
/*switch to change the state of the port*/
	switch (state)
	  {
		case UID_PORT_DISABLED:
			tem_reg |= PORT_STATE_DISABLED;	
			//printf("Port state disabled\n\r");
			break;			  
        case UID_PORT_DISCARDING:
			tem_reg |= PORT_STATE_BLOCKING;
			//printf("Port status blocking\n\r");
			break;
		case UID_PORT_LEARNING:
			tem_reg |= PORT_STATE_LEARNING;
			//printf("Port status learning\n\r");
			break;
		case UID_PORT_FORWARDING:
			tem_reg |= PORT_STATE_FORWARDING; 
			//printf("Port state forwarding\n\r");
			break;
		default:
			break;
	  }
	
	HAL_ETH_WritePHYRegister(&Eth_Handle,REG_PORT_CONTROL,tem_reg);

}	


/***********Get_Port_status()*************/
//get the current status for indicate ports
int
Get_Port_status(int portNo)
  {

	uint16_t The_status;
	
	Eth_Handle.Init.PhyAddress = PORT_0 + portNo;
/*Read The specific status*/
	HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_PORT_STATUS,&The_status);
	if ( (The_status & PORT_LINK_ON) != RESET)
		return 1;  
	return 0;
}


/********************Get_Oper_speed****************/
int Get_Oper_speed(int portNo)
  {
	uint16_t tem_reg;

	Eth_Handle.Init.PhyAddress = PORT_0 + portNo;
/*For the marvell chip 88e6097F's PORT8~PORT9 are always 1000M fiber*/
	if (Eth_Handle.Init.PhyAddress >= PORT_8 && Eth_Handle.Init.PhyAddress <= PORT_10)
	  return 1000L;
	HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_PORT_STATUS,&tem_reg);
	tem_reg &= PORT_SPEED;
	tem_reg = tem_reg >>8;	
	if(tem_reg == 0x0000)
	   return 10L;
	else 
	  if(tem_reg == 0x0001)
	      return  100L;
	else
		return 0;
}

/***************Get_Port_duplex()********************/
int Get_Port_duplex(int portNo)
  {
	uint16_t tem_reg;
	
	Eth_Handle.Init.PhyAddress = PORT_0 + portNo;
	if(portNo >= 8)
	    return 1;
	HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_PORT_STATUS,&tem_reg);
	tem_reg &= 0xffff;
	if((tem_reg & PORT_DUPLEX) != RESET)
		  return 1;
	else
		  return 0;
  }

/*********************Get_Port_cost*****************/
int Get_Port_cost(int portNo)
  {
	if (portNo < 8 )
	  {
		return 200000L; 
	  }
	return 20000L;
  }
/*Get the type of the port*/
int Get_Port_Type(int portNo)
  {
	uint16_t tem_reg;
	READ_PHYREGISTER(SWITCH_PHY_0+portNo,REG_PHY_SPECIFIC_CONTROL2,&tem_reg);
	
	if((tem_reg & PHY_SOFTMEDIA_SELECT) != RESET)
       {return 1;}
	return 0;
  }
	

/*****************************************************************************
//Set the port speed  through the PHY register
******************************************************************************/
//And the portNo should not exceed 8,for the speed of port 8,9 or 10 are fiber ports,and the speed is forced
MYERROR_INFO
Set_PORT_STATS(int portNo,PORT_STAS port_sta)
  {
  
    uint16_t tem_reg ;
//	uint32_t tem_count;
	
	if(portNo > 10)
	  {
		return THE_PORTNO_EXCEED;
	  }
/*At first, Check if the PHYPOWERDOWNMODE enabled,if so change it*/
	READ_PHYREGISTER(SWITCH_PHY_0+ portNo,REG_PHY_CONTROL,&tem_reg);	
	if( (tem_reg &PHY_POWERDOWN) == PHY_POWERDOWN)
	  {
		tem_reg &= ~PHY_POWERDOWN;  //Clear the bit to make sure the PHY is POWERUP
		WRITE_PHYREGISTER(SWITCH_PHY_0+portNo,REG_PHY_CONTROL,tem_reg);
	  } 
/*Set the speed\duplex bits write the register*/		
	if(port_sta == SPEED100_FULLDPX)
	  { 
		if(Get_Port_Type(portNo))
		  {
			cancleForceLink(portNo);
		  }
		else
		  {
			/*Set the auto advertisement to diable the 10M speed ability*/
			READ_PHYREGISTER(SWITCH_PHY_0+ portNo,REG_AUTO_NEG_ADVERTISE,&tem_reg);
			tem_reg |= PHY_ANEG_ALL;
			tem_reg &= ~PHY_ANEG_10FDX;
			tem_reg &= ~PHY_ANEG_10HALF;
			WRITE_PHYREGISTER(SWITCH_PHY_0+portNo,REG_AUTO_NEG_ADVERTISE,tem_reg);
			/*Restart auto function*/
			READ_PHYREGISTER(SWITCH_PHY_0+ portNo,REG_PHY_CONTROL,&tem_reg);
			tem_reg |= PHY_RESTART_AUTONEGOTIATION;
			WRITE_PHYREGISTER(SWITCH_PHY_0 + portNo,REG_PHY_CONTROL,tem_reg);
//				READ_PHYREGISTER(SWITCH_PHY_0+ portNo,REG_PHY_CONTROL,&tem_reg);
//				tem_reg &= ~(PHY_FULLDUPLEX_100M | PHY_AUTONEGOTIATION);
//				/*Set software reset bit*/
//				tem_reg |= PHY_SW_RESET;
//				tem_reg |= ( PHY_FULLDUPLEX_100M  );
//				WRITE_PHYREGISTER(SWITCH_PHY_0+portNo,REG_PHY_CONTROL,tem_reg);

		  }
	  }
	else
	  if(port_sta == SPEED100_HALFDPX)
		{   
			/*Set the auto advertisement to diable the 10M speed ability*/
			READ_PHYREGISTER(SWITCH_PHY_0+ portNo,REG_AUTO_NEG_ADVERTISE,&tem_reg);
			tem_reg |= PHY_ANEG_ALL;
			tem_reg &= ~PHY_ANEG_10FDX;
			tem_reg &= ~PHY_ANEG_10HALF;
			tem_reg &= ~PHY_ANEG_100FDX;
			WRITE_PHYREGISTER(SWITCH_PHY_0+portNo,REG_AUTO_NEG_ADVERTISE,tem_reg);
			/*Restart auto function*/
			READ_PHYREGISTER(SWITCH_PHY_0+ portNo,REG_PHY_CONTROL,&tem_reg);
			tem_reg |= PHY_RESTART_AUTONEGOTIATION;
			WRITE_PHYREGISTER(SWITCH_PHY_0 + portNo,REG_PHY_CONTROL,tem_reg);
		}
	else 
	  if(port_sta == SPEED10_FULLDPX)
		{ 
			/*Set the auto advertisement to diable the 10M speed ability*/
			READ_PHYREGISTER(SWITCH_PHY_0+ portNo,REG_AUTO_NEG_ADVERTISE,&tem_reg);
			tem_reg |= PHY_ANEG_ALL;
			tem_reg &= ~PHY_ANEG_100FDX;
			//tem_reg &= ~PHY_ANEG_10HALF;
			tem_reg &= ~PHY_ANEG_100HALF;
			WRITE_PHYREGISTER(SWITCH_PHY_0+portNo,REG_AUTO_NEG_ADVERTISE,tem_reg);
			/*Restart auto function*/
			READ_PHYREGISTER(SWITCH_PHY_0 + portNo,REG_PHY_CONTROL,&tem_reg);
			tem_reg |= PHY_RESTART_AUTONEGOTIATION;
			WRITE_PHYREGISTER(SWITCH_PHY_0 + portNo,REG_PHY_CONTROL,tem_reg);
		}
   else
	  if(port_sta == SPEED10_HALFDPX)
		{     
			/*Set the auto advertisement to diable the 10M speed ability*/
			READ_PHYREGISTER(SWITCH_PHY_0+ portNo,REG_AUTO_NEG_ADVERTISE,&tem_reg);
			tem_reg |= PHY_ANEG_ALL;
			tem_reg &= ~PHY_ANEG_100FDX;
			tem_reg &= ~PHY_ANEG_10FDX;
			tem_reg &= ~PHY_ANEG_100HALF;
			WRITE_PHYREGISTER(SWITCH_PHY_0+portNo,REG_AUTO_NEG_ADVERTISE,tem_reg);
			/*Restart auto function*/
			READ_PHYREGISTER(SWITCH_PHY_0 + portNo,REG_PHY_CONTROL,&tem_reg);
			tem_reg |= PHY_RESTART_AUTONEGOTIATION;
			WRITE_PHYREGISTER(SWITCH_PHY_0 + portNo,REG_PHY_CONTROL,tem_reg);	
		}
   else 
	  if(port_sta == AUTO)
		{ 
			/*Set the auto advertisement to diable the 10M speed ability*/
			READ_PHYREGISTER(SWITCH_PHY_0+ portNo,REG_AUTO_NEG_ADVERTISE,&tem_reg);
			tem_reg |= PHY_ANEG_ALL;
			WRITE_PHYREGISTER(SWITCH_PHY_0+portNo,REG_AUTO_NEG_ADVERTISE,tem_reg);		
			/*Clear the AUTO bits and speed duplex bits*/		 
			READ_PHYREGISTER(SWITCH_PHY_0+ portNo,REG_PHY_CONTROL,&tem_reg);
			tem_reg &= ~(PHY_FULLDUPLEX_100M | PHY_AUTONEGOTIATION);
			/*Set software reset bit*/
			tem_reg |= PHY_SW_RESET;		
			tem_reg |= PHY_AUTONEGOTIATION;
			WRITE_PHYREGISTER(SWITCH_PHY_0+portNo,REG_PHY_CONTROL,tem_reg);	
		}	
   else
	  if(port_sta == DISABLED)
		{  
			/*强制关闭端口*/
		  READ_PHYREGISTER(SWITCH_PHY_0 + portNo,REG_PHY_CONTROL,&tem_reg);
   		  tem_reg &= ~PHY_POWERDOWN;
		  tem_reg |= PHY_POWERDOWN;
		  WRITE_PHYREGISTER(SWITCH_PHY_0 + portNo,REG_PHY_CONTROL,tem_reg);
		  if(Get_Port_Type(portNo)!=0 || portNo >= 8)/*检测到0~7端口为光口模式，或者端口为8~9端口*/
			{
			  forceLinkDown(portNo);
			}
		}
	else	
	  if (port_sta == SPEED1000_FULLDPX)	
		{
		  cancleForceLink(portNo);
		}
   else		 
	   return SET_SPEED_DUPLEX_ERROR; 
	return NO_ERROR;
  }

/****************************Function: Set_FLOW_control()**************************/ 
//Here is the force flow control method 
MYERROR_INFO 
Set_FLOW_CONTROL(int portNo ,int flowcontrol)
  {
	uint16_t tem_reg;
	uint16_t Phy_addr;

/*Check if the port's status is atuo or forced speed */
//   	Phy_addr = SWITCH_PHY_0 + portNo ;		

	Phy_addr = SWITCH_PHY_0 + portNo ;
	if(	READ_PHYREGISTER(Phy_addr,REG_AUTO_NEG_ADVERTISE,&tem_reg) != HAL_OK)  //Get the advertise value of the PHY reg
	  { /*Rersered if operations needed*/}  
	if(flowcontrol){
					  tem_reg |= PHY_ANEG_PAUSE; //Set the MAC pause implemted bit
					  WRITE_PHYREGISTER(Phy_addr,REG_AUTO_NEG_ADVERTISE,tem_reg);//Write the bit
				   } 
	else{
		  tem_reg &= ~PHY_ANEG_PAUSE;
		  WRITE_PHYREGISTER(Phy_addr,REG_AUTO_NEG_ADVERTISE,tem_reg);//Write the bit
		} 
	if(	READ_PHYREGISTER(Phy_addr,REG_PHY_CONTROL,&tem_reg) != HAL_OK)
	  {
      //Rersered if operations needed
	  }		
    if(/*(tem_reg & PHY_AUTONEGOTIATION) == RESET*/portNo>7) //Auto negotiation disabled ,force the flow control
	  {
		/*设置强制流控之前需要把link状态强制为down*/
		forceLinkDown(portNo); 
		Eth_Handle.Init.PhyAddress = PORT_0 + portNo;
		HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_PCS_CONTROL,&tem_reg);
        tem_reg &= ~(PORT_FC_VALUE | PORT_FC_FORCED);
		if(flowcontrol){
						 tem_reg |= (PORT_FC_VALUE | PORT_FC_FORCED);
                       }
		else { 
			    tem_reg |= PORT_FC_FORCED;
             }
		HAL_ETH_WritePHYRegister(&Eth_Handle,REG_PCS_CONTROL,tem_reg);
		/*设置完成需要取消强制link down状态*/
		cancleForceLink(portNo);
		//return NO_ERROR;
	  }
	else  //Auto negotiation endabled just set the advertisement for the PHY 
	  {
		READ_PHYREGISTER(Phy_addr,REG_PHY_CONTROL,&tem_reg);
		tem_reg |= PHY_RESTART_AUTONEGOTIATION;
		WRITE_PHYREGISTER(Phy_addr,REG_PHY_CONTROL,tem_reg);		
		return NO_ERROR;
	  }//end of else
	Eth_Handle.Init.PhyAddress = PORT_0 + portNo;
	HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_PORT_STATUS,&tem_reg);
	//printf("The flow control status is: %x\n\r",tem_reg);
	return NO_ERROR;
  }//end of function

/*Function: Get_FLOW_status(portNo)*/
int Get_FLOW_STATUS(int portNo)
  {
	uint16_t tem_reg;
	Eth_Handle.Init.PhyAddress = PORT_0 + portNo;
	HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_PORT_STATUS,&tem_reg);
	if((tem_reg & PORT_PAUSE_EN) != RESET)
		return 1;
	else
		return 0;
  }	

/*自定义的字符串拷贝以及字符串比较函数*/
/***********************************************************************/
//Function : Str_cmp(uint8_t *s1,uint8_t uint8_t *s2,uint16_t len)
//Brief    : Compare the two string
//retval   : type uint8_t : '1'for unequal result ,while '0'for the equal
/***********************************************************************/
uint8_t Str_cmp(uint8_t *s1,uint8_t *s2,uint16_t len)
  {
	uint16_t loop;  
	loop= len;
	while(loop--)
	  {
		if (*(s1+loop) != *(s2+loop))  
			{
				return 1;
			}		
	  }
	return 0;
  }
	
	
	

/***********************************************************************/
//Function : Str_cpy(uint8_t *s1,uint8_t uint8_t *s2,uint16_t len)
//Brief    : copies not more than n characters (characters that follow a null
//           * character are not copied) from the array pointed to by s2 into the array
//           * pointed to by s1.
//retval   : void
/***********************************************************************/
void Str_cpy(uint8_t *s1,uint8_t *s2,uint16_t len)
  {
	uint16_t loop ;
	for(loop =0 ;loop < len ; loop ++)  
	  { 
		*(s1 + loop) = *(s2 + loop);
	  }
  }	
