/**
*************************************************************************
@File   : hal\src_code\veth_InVariate.c
@Author : Duke Lee
@Date   : 13-Oct-2014
@Brief  : To keep some variate no lose When power supply drop 
*************************************************************************
**/

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "stm32f4xx.h"
#include "stm32f4xx_hal_conf.h"
#include "veth_common.h"
#include "veth_invariate.h"
#include "board_info.h"
//#ifdef VETH2000_RSTP
#include "stp_in.h"
//#endif
//#include "veth_web.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define FLASH_USER_START_ADDR   ADDR_FLASH_SECTOR_11            /* Start @ of user Flash area */
#define FLASH_TARG_START_ADDR   ADDR_FLASH_SECTOR_2             /* Start of the network upgrade targ area*/


#define NONVALID_IP 0xFFFFFFFF
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint32_t FirstSector = 0, NbOfSectors = 0, Address = 0;
uint32_t SectorError = 0;
__IO uint32_t data32 = 0 , MemoryProgramStatus = 0;
__IO uint8_t  data8=0;
/*Variable used for Erase procedure*/
static FLASH_EraseInitTypeDef EraseInitStruct;
INVARIATE_T my_invarite;

//uint8_t Update_InVariate(INVARIATE_T *p_inv);
//void Get_InVariate(INVARIATE_T *p_inv);


/******************************************************************/
//Function  :   Update_InVariate(void *p_inv,uint16_t struct_len);
//Brief     :   Update the Variables that are to be stored into The Selected FLASH Sector
//Parameter :   p_inv(Input) : contian all of the variates to be stored
//Retval    :   uint8_t 类型 ： ‘1’表示更新成功 ，‘0’表示更新失败，可能是FLASH的操作出了问题
/******************************************************************/
uint8_t Update_InVariate(void *p_inv,uint32_t struct_len)
  {
	uint32_t   i;
	/*Unlock the Flash to enable the flash control register access*/ 
	if(HAL_OK != HAL_FLASH_Unlock()){
	  printf("The Flash unlock failed\n\r");
	}
	/* Fill EraseInit structure ,Here just SECTOR_11 need to be erased*/
    EraseInitStruct.TypeErase = TYPEERASE_SECTORS;
    EraseInitStruct.VoltageRange = VOLTAGE_RANGE_3;
    EraseInitStruct.Sector = FLASH_SECTOR_11;
    EraseInitStruct.NbSectors = 1;
	/*Do the Erase operation*/
    if (HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError) != HAL_OK)
	{
       /*Erase failed*/
	  printf("Flash Erase Failed!!!\n\r");
	  return 0;
    }
	WatchDogTimerReset();
	/*Program the selected FLASH area byte by byte*/
	Address = FLASH_USER_START_ADDR;
	i=0;
	while (Address < FLASH_USER_START_ADDR + struct_len)
	  {
		if (HAL_FLASH_Program(TYPEPROGRAM_BYTE, Address, *((uint8_t *)p_inv+i)) == HAL_OK)
		  {
			Address = Address + 1;
			i++;
          }
		else
		  {
			printf("Flash Program failed!!!\n\r");
			return 0; 						
          }
	  }
/* Lock the Flash to disable the flash control register access (recommended
     to protect the FLASH memory against possible unwanted operation)*/
	HAL_FLASH_Lock();
/* Check if the programmed data is OK
MemoryProgramStatus = 0: data programmed correctly
MemoryProgramStatus != 0: number of words not programmed correctly*/
	Address = FLASH_USER_START_ADDR;
	MemoryProgramStatus = 0;
	i=0;
	while (Address < FLASH_USER_START_ADDR + struct_len)
	  {
		data8 = *(__IO uint8_t*)Address;
		if ( data8 != *( ( (uint8_t*)p_inv+i) ) )
		  {
			MemoryProgramStatus++;  
		  }
		Address = Address + 1;
		i++;
	  }
/*Check if there is an issue to program data*/
	if (MemoryProgramStatus != 0)
	  {
		printf("PS: Flash Check error!!!\n\r");
		return 0;
      }
#ifdef MY_DEBUG
	printf("The flash update operation is done\n\r\n\r");
#endif
	return 1;
  }
  
/******************************************************************/
//Function  :   setNetworkUpgradeTarg();
//Brief     :   set the targ for upgrading the flash program through network
//Parameter :   
//Retval    :   uint8_t 类型 ： ‘1’表示设置成功 ，‘0’表示设置失败，可能是FLASH的操作出了问题
/******************************************************************/
uint8_t setNetworkUpgradeTarg(uint8_t targ)
  {
	uint32_t   i;  
	/*Unlock the Flash to enable the flash control register access*/ 
	HAL_FLASH_Unlock();
	/* Fill EraseInit structure ,Here just SECTOR_11 need to be erased*/
    EraseInitStruct.TypeErase = TYPEERASE_SECTORS;
    EraseInitStruct.VoltageRange = VOLTAGE_RANGE_3;
    EraseInitStruct.Sector = FLASH_SECTOR_2;
    EraseInitStruct.NbSectors = 1;
	if (HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError) != HAL_OK)
	{
      /*Erase failed*/
	  printf("Set network Upgrade error\n\r");
	  return 0;
    }
	/*Program the selected FLASH area byte by byte*/
	Address = FLASH_TARG_START_ADDR;
	if (HAL_FLASH_Program(TYPEPROGRAM_BYTE, Address,targ) != HAL_OK)
	{
	  printf("Set network Upgrade error\n\r");
	  return 0; 						
	}
	/*Lock the flash to ensure the security of my program*/
	HAL_FLASH_Lock();
	Address = FLASH_TARG_START_ADDR;
	MemoryProgramStatus = 0;
	data8 = *(__IO uint8_t*)Address;
	if ( data8 != targ )
	{
	  MemoryProgramStatus++;  
	}
/*Check if there is an issue to program data*/
	if (MemoryProgramStatus != 0)
	{
	  printf("Set network Upgrade error\n\r");
	  return 0;
	}
	return 1;
  }
  
/******************************************************************/
//Function  :   getNetworkUpgradeTarg();
//Brief     :   get the targ for upgrading the flash program through network
//Parameter :   
//Retval    :   uint8_t 类型 ： ‘1’表示获取成功 ，‘0’表示获取失败，可能是FLASH的操作出了问题
/******************************************************************/
uint8_t getNetworkUpgradeTarg(uint8_t *ptarg)
  {
	Address = FLASH_TARG_START_ADDR;
	*ptarg = *(__IO uint8_t*)Address;
#ifdef MY_DEBUG
	printf("The flash targ: %d\n\r",*ptarg);
#endif
	return 1;
  }
/******************************************************************/
//Function  :   Get_InVariate(INVARIATE_T *p_inv);
//Brief     :   Update the Variables that are to be stored into The Selected FLASH Sector
//Parameter :   p_inv(Output) : contian all of the variates Get out from my selected FLASH sector
/******************************************************************/
uint8_t Get_InVariate(void *p_inv,uint32_t struct_len)
  {
	uint32_t   i;

	Address = FLASH_USER_START_ADDR;
	i=0;
	while (Address < FLASH_USER_START_ADDR + struct_len)
	  {
		*( ( (uint8_t*)p_inv+i) ) = *(__IO uint8_t*)Address; 
		Address = Address + 1;
		i++;
	  }
	return 1;
  }
  
/******************************************************************/
//Function  :   InVariate_Default(void);
//Brief     :   Init the global variable with the default data
//Parameter :   No
/******************************************************************/
void InVariate_Default(INVARIATE_T *invarite)
  {
	/*Set the default MAC address*/
//	Str_cpy(invarite->The_MAC,KEY_MAC,6);
	/*Set the default sets information*/
	invarite->The_Info = SET_INFO;
	/*Set the default configuration for the RSTP*/  
	invarite->my_rstp.bridge_pri     = DEF_BR_PRIO;
	invarite->my_rstp.bridge_hellot  = DEF_BR_HELLOT;
	invarite->my_rstp.bridge_max_age = DEF_BR_MAXAGE;
	invarite->my_rstp.bridge_fw_delay= DEF_BR_FWDELAY;
	/*Set the default configuration for the IP address*/
	invarite->The_ip = SET_IP;
	/*Set the default user information*/
	invarite->my_user = USER_INFO;
  }
 
static uint16_t charToUint(char The_char)
  {
	return The_char - 48;
  }
 
/******************************************************************/
//Function  :   modifyRstpInfo(char *p_num,)
//Brief     :   Modify the RSTP bridge priority  
//Parameter :   Point to the command array
/******************************************************************/ 
uint8_t modifyRstpInfo(INVARIATE_T *invarite,char *p_num,RSTP_INFO_E info)
  {
	uint16_t loop;
	char     tem_char;
	uint32_t value; 
	
	loop  = 12;
	value = 0;
	while(1)
	{
	  tem_char = *(p_num + loop);
	  if(tem_char >= 48 && tem_char <= 57)//Indicate a valid number
	  {
		value = value *10 + charToUint(tem_char);
		loop++;
	  }else
	     if(tem_char == 13 ) //A "carriage return" indicates the end of the string
		   {break;}
	   else                  //Invalid input format
	       {return 0;}    
	}//Calculate done
	switch(info)
	  {
		case BRIDGE_PRIORITY:
		  if( value < MIN_BR_PRIO || value > MAX_BR_PRIO) //Invalid input value
		    {return 0;}
		  /*Valid input value,modify it*/
		  invarite->my_rstp.bridge_pri = value & 0x00ffff;
		  break;
		case BRIDGE_HELLOTIME:
		  if( value < MIN_BR_HELLOT || value > MAX_BR_HELLOT) //Invalid input value
		    {return 0;}
		  /*Valid input value,modify it*/
		  invarite->my_rstp.bridge_hellot = value & 0x00ffff;
		  break;		  
		case BRIDGE_MAXAGE:
		  if( value < MIN_BR_MAXAGE || value > MAX_BR_MAXAGE) //Invalid input value
		    {return 0;}
		  /*Valid input value,modify it*/
		  invarite->my_rstp.bridge_max_age = value & 0x00ffff;
		  break;
		case BRIDGE_FWD_DELAY:  
		  if( value < MIN_BR_FWDELAY || value > MAX_BR_FWDELAY) //Invalid input value
		    {return 0;}
		  /*Valid input value,modify it*/
		  invarite->my_rstp.bridge_pri = value & 0x00ffff;
		  break;
		default :
		  return 0;
	  }
	return updateNewConfig(); //Update the info into flash
  }
  
/******************************************************************/
//Function  :   displayRstpInfo()
//Brief     :   Display the RSTP bridge information  
//Parameter :   None
/******************************************************************/ 
void displayRstpInfo(INVARIATE_T *invarite)
  {
	printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n\r\n\r");
	printf("               RSTP Bridge Information\n\r\n\r");
	printf("Bridge priority   : %d \n\r\n\r",invarite->my_rstp.bridge_pri);
	printf("Bridge hello time : %d \n\r\n\r",invarite->my_rstp.bridge_hellot);
	printf("Bridge max age    : %d \n\r\n\r",invarite->my_rstp.bridge_max_age);
	printf("Bridge fw delay   : %d \n\r\n\r",invarite->my_rstp.bridge_fw_delay);
	printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n\r\n\r");
  }  


static uint32_t getIpNum(char *p_num,uint16_t *loop)
  {
	char      tem_char;
	uint32_t  value;
	
	value = 0;
	while(1)
	{
	  tem_char = *(p_num + (*loop));
	  if( tem_char == '.' || tem_char == ' ' || tem_char == 13 )
	    {
		  (*loop)++;
		  return value;
		}
	  else 
		if(tem_char >= '0' && tem_char <= '9')
		  {
			value = value*10 + charToUint(tem_char);
			(*loop)++;
		  }
	  else
		{
		  return NONVALID_IP;
		}
	}//end of while(1) 
  }
/******************************************************************/
//Function  :   modifyIpAddr(char *p_num)
//Brief     :   Modify the IP address  
//Parameter :   Point to the command array
/******************************************************************/
uint8_t modifyIpAddr(VETH_IP_T *ip,char *p_num)
  {
	uint16_t  loop;
	uint16_t  circle;
	char      tem_char;
	uint32_t  value;
	
	loop = 7;
	printf("\n\r\n\r");
	printf("IP string:%s\n\r\n\r",Command);
	/*Get the IP address*/
	for(circle = 0 ;circle < 4 ;circle++ )
	{
	  value = getIpNum(p_num,&loop);
	  if(value == NONVALID_IP || value >255)
	    {return 0;}
	  else
	    ip->ip_addr[circle] = value & 0x0ff;
	}
	printf("IP address done : %d.%d.%d.%d\n\r\n\r",ip->ip_addr[0],ip->ip_addr[1],ip->ip_addr[2],ip->ip_addr[3]);
	/*Get the IP mask*/
	for(circle = 0 ;circle < 4 ;circle++ )
	{
	  value = getIpNum(p_num,&loop);
	  if(value == NONVALID_IP || value >255)
	    {return 0;}
	  else
	    ip->net_mask[circle] = value & 0x0ff;
	}
	printf("IP mask done: %d.%d.%d.%d\n\r\n\r",ip->net_mask[0],ip->net_mask[1],ip->net_mask[2],ip->net_mask[3]);
	/*Get the IP draddr*/
	for(circle = 0 ;circle < 4 ;circle++ )
	{
	  value = getIpNum(p_num,&loop);
	  if(value == NONVALID_IP || value >255)
	    {return 0;}
	  else
	    ip->route_addr[circle] = value & 0x0ff;
	}
	printf("IP draddr done: %d.%d.%d.%d\n\r\n\r",ip->route_addr[0],ip->route_addr[1],ip->route_addr[2],ip->route_addr[3]);
	/*Valid IP address input,update them into flash*/
	return updateNewConfig(); //Update the info into flash
  }
/******************************************************************/
//Function  :   displayIpInfo()
//Brief     :   Display the IP information  
//Parameter :   None
/******************************************************************/ 
void displayIpInfo(VETH_IP_T *ip)
  {
    uint16_t loop;
	printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n\r\n\r");
	printf("IP address   : %d.%d.%d.%d\n\r\n\r",ip->ip_addr[0],ip->ip_addr[1],ip->ip_addr[2],ip->ip_addr[3]);
	printf("IP mask      : %d.%d.%d.%d\n\r\n\r",ip->net_mask[0],ip->net_mask[1],ip->net_mask[2],ip->net_mask[3]);
	printf("IP router    : %d.%d.%d.%d\n\r\n\r",ip->route_addr[0],ip->route_addr[1],ip->route_addr[2],ip->route_addr[3]);
	printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n\r\n\r");
  } 
/******************************************************************/
//Function  :   modifyUserName(char *p_num)
//Brief     :   Modify username for CLI  
//Parameter :   Point to the command array
//retval    :   '1' indicates modifying success,'0' failed 
/******************************************************************/ 
uint8_t modifyUserName(INVARIATE_T *invarite,char *p_num)
  {
	uint16_t loop;
	char tem[30]={0};
	
	memset(tem,'\0',30);
	loop = 7;
	while(1)
	 {
		if(*(p_num+loop) == 13)
		  {break;}
		if(*(p_num+loop) == '\0')
		  {return 0;}
		tem[loop-7] = *(p_num+loop);
		loop++;
	 }
	/*Judge if the length mecism*/
	if(loop >= 24)
	{
	  printf("username too long! \n\r\n\r");
	  return 0;
	}
	memset(invarite->my_user.username,'\0',20);
	strncpy(invarite->my_user.username,tem,strlen(tem));
	//printf("\n\r username: %s\n\r %d\n\r",invarite->my_user.username,strlen(invarite->my_user.username));
	return updateNewConfig(); //Update the info into flash
  }
  
/******************************************************************/
//Function  :   modifyPassWord(char *p_num)
//Brief     :   Modify password for CLI  
//Parameter :   Point to the command array
//retval    :   '1' indicates modifying success,'0' failed 
/******************************************************************/ 
uint8_t modifyPassWord(INVARIATE_T *invarite,char *p_num)
  {
	uint16_t loop;
	char tem[30]={0};
	
	memset(tem,'\0',30);
	loop = 8;
	while(1)
	 {
		if(*(p_num+loop) == 13)
		  {break;}
		if(*(p_num+loop) == '\0')
		  {return 0;}
		tem[loop-8] = *(p_num+loop);
		loop++;
	 }
	/*Judge if the length mecism*/
	if(loop >= 24)
	{
	  printf("password too long! \n\r\n\r");
	  return 0;
	}
	memset(invarite->my_user.password,0,20);
	strncpy(invarite->my_user.password,tem,strlen(tem));
	//printf("\n\r password: %s\n\r %d\n\r",invarite->my_user.password,strlen(invarite->my_user.password));	
	return updateNewConfig(); //Update the info into flash
  }
/******************************************************************/
//Function  :   InVariate_Init(void);
//Brief     :   Init the global variable with the corresponding data
//Parameter :   None
/******************************************************************/
static uint8_t InVariate_Init(BOARD *the_board,uint32_t struct_len)
{
	uint32_t  loop;
	uint8_t   tem_MAC[6];
	SPECIAL_T the_special;
/*Get the Set VETH2000's MAC address that stored in the FLASH memory*/	  
	if( ! Get_InVariate(&the_special,sizeof(SPECIAL_T)) )
	  {
		return 0; 
	  }
/*Judge if the readout MAC address is all '0xFFs',if so set the initial information for the struct INVARIATE_T*/
	if( !Str_cmp(the_special.sets_info.The_MAC,GET_NEXT,6) /*||(0x02 & Get_DIAL_Mode()) == 0x02&& !strcmp(invarite->The_Info.The_type,SET_INFO.The_type)*/ )
	{//if( !Str_cmp(the_board->sets_info.The_MAC,GET_NEXT,6) )
	     Str_cpy(the_board->sets_info.The_MAC,KEY_MAC,6);
	}else{
		Get_InVariate(the_board,sizeof(BOARD));
	}
	return 1;
  }
  
uint8_t Init_InVariate()
  {
	return InVariate_Init(&board,sizeof(BOARD));
  }
/******************************************************************/
//Function  :   displayBasicInfo(BOARD *the_board);
//Brief     :   Init the global variable with the corresponding data
//Parameter :   None
/******************************************************************/
void displayBasicInfo(INVARIATE_T *inv)
  {
	printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n\r\n\r");
	printf("              Sets Information\n\r\n\r");
	printf("Hardware       : ");
	printf("%s\n\r\n\r",inv->The_Info.The_type);
	printf("Software       : ");
	printf("%s\n\r\n\r",inv->The_Info.The_Soft);
	printf("Soft Version   : ");
	printf("%s\n\r\n\r",inv->The_Info.The_Version); 
	printf("Author         : ");
	printf("%s\n\r\n\r",inv->The_Info.The_Author);
	//printf("\n\r");
	printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n\r\n\r");
	//printf("\n\r");
	printf("MAC address    : %02x.%02x.%02x.%02x.%02x.%02x\n\r\n\r",inv->The_MAC[0],inv->The_MAC[1],inv->The_MAC[2],
																	inv->The_MAC[3],inv->The_MAC[4],inv->The_MAC[5]);
	//printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n\r\n\r");
  }

/******************************************************************/
//函数名  :   updateInvariate(void *p_inv,uint16_t struct_len);
//功能    :   将配置信息保存到flash中去 
//参数    :   p_inv(Input) : 指向待保存的数据集合
//            struct_len   : 所指向的数据集合的长度（单位：字节）
//返回值  : uint8_t 类型 ： ‘1’表示更新成功 ，‘0’表示更新失败，可能是FLASH的操作出了问题
/******************************************************************/	
uint8_t updateInvariate(void *p_inv,uint32_t struct_len)
  {
	return Update_InVariate(p_inv,struct_len);
  }

/******************************************************************/
//Function  :   updateNewConfig();
//Brief     :   Different project has different key struct,when flash need to be updated  
//Parameter :   None
//retval    :   '1':success  ,'0':fail 
/******************************************************************/
uint8_t updateNewConfig()
  {
		memset(&board_temp,0,sizeof(board_temp));
		getInvariate(&board_temp,sizeof(board_temp)); //得到之前flash中的数据
		
		if(calculateChecksum(&board_temp,sizeof(board_temp),18)
			!= calculateChecksum(&board,sizeof(board),18)){ //判断两个board的检验和是否相同
			//updateInvariate(&board,sizeof(board));
			if(updateInvariate(&board,sizeof(board))){//向Flash中写入数据
				
				//printf("update flash sucessful\n\r");
				return 1;
			} else{
				//printf("update flash failed\n\r");
				return 0;
			}
		}else{
			//printf("not need update flash\n\r");
			return 2;
		}
	//return Update_InVariate(&board,sizeof(BOARD));
  }

/******************************************************************/
//函数名  :   updateInvariate(void *p_inv,uint16_t struct_len);
//功能    :   将配置信息从FLASH中读取出来 
//参数    :   p_inv(Input) : 指向数据集合所定义的全局变量，用来存放读取到的数据
//            struct_len   : 所指向的数据集合的长度（单位：字节）
//返回值  :   uint8_t 类型 ： ‘1’表示更新成功 ，‘0’表示更新失败，可能是FLASH的操作出了问题
/******************************************************************/
uint8_t getInvariate(void *p_inv,uint32_t struct_len)
  {
	return Get_InVariate(p_inv,struct_len);
  }
  

 