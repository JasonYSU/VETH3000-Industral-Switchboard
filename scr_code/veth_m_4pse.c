/**
****************************************************************************************
@File    : D:\ProgramFiles\KEIL\PRO\scr_code\m_4pse.c
@Author  : Duke Lee
@Date    : 06-August-2014
@Version : V1.0.0
@Breif   : The functions for the operation of 'M_4PSE' board.
           These functions are comunicate with each other through I2C bus.
		   For the VETH1000 sets Only 2 piece of board can availiable.
		   PS:Only 8 M_PSE board can be atted to the I2C bus in total.For my M-4PSE board only 3 bit of chip address availiable.
		      And the chip address are '01000xxx'. 
		       
****************************************************************************************
**/
#include <stdio.h>
#include <stdint.h>
#include "veth_common.h"
#include "stm32f4xx_hal_conf.h"
#include "veth_m_4pse.h"

#ifdef   VETH2000
#include "veth_marvell_88e6097f.h"
#endif




/*The basic functions for the POE operation*/
HAL_StatusTypeDef HAL_I2C_Mem_Read(I2C_HandleTypeDef *hi2c, uint16_t DevAddress,
								uint16_t MemAddress, uint16_t MemAddSize, 
								uint8_t *pData, uint16_t Size, uint32_t Timeout);

HAL_StatusTypeDef HAL_I2C_Mem_Write(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, 
								uint16_t MemAddress, uint16_t MemAddSize, 
								uint8_t *pData, uint16_t Size, uint32_t Timeout);

/*Statement of the private functions*/
static uint8_t getPseChipNum();

/***********************************************************************/
//函数名  ： poeDemo()
//功能    ： poe项的web配置样例程序
//参数    ： None
//返回值  ： None
/***********************************************************************/
void poeDemo()
  {
	uint8_t tem;
	tem = getPseChipNum();
  	//printf( "PSE ChipNo:%x\n\r",tem);
  }
/***********************************************************************/
//函数名  ： getPseChipNum()
//功能    ： 获取PSE芯片的编号
//参数    ： None
//返回值  ： uint8_t
/***********************************************************************/																 
static uint8_t getPseChipNum()
  {
	uint8_t tem_reg;
	HAL_I2C_Mem_Read(&I2C1Handle,CHIP_1,REG_CHIP_ID,MEM_ADDR_SIZE,&tem_reg,1,TIME_OUT);
	return (tem_reg>>3);
  }

/*********************************************************************************************/
/*********The functions for display the states of each ports and for each M_4PSE board********/
/*********************************************************************************************/
//GET PORT STATUS 
/***********************************************************************/
//函数名  ： getPoePortStatus(uint8_t portNo,PSE_PORT_T* portPSE)
//功能    ： 获取POE端口的供电状态
//参数    ： portNo：POE端口号  portPSE:存放获取到的POE端口状态
//返回值  ： None
/***********************************************************************/ 
void getPoePortStatus(uint8_t portNo,PSE_PORT_T* portPSE)
  {
	uint8_t  tem_reg1;                        //When the read and write operations use IT mode,this variable must be gloabal
	uint8_t  tem_reg2;                        //
	uint16_t tem_reg;                         //Used to contain the 16-bit value 
	uint8_t  chip_offset;                     //Used for offset value
	uint8_t  reg_offset;
		 
	chip_offset = portNo/4;
	reg_offset  = (portNo%4) * 4;
/*Read out the value of the specific registers,and the specific value can be availiable*/
/*Get the status value of the PSE port*/
	HAL_I2C_Mem_Read(&I2C1Handle,CHIP_1 + chip_offset ,REG_PORT1_STATUS + reg_offset,
					MEM_ADDR_SIZE,&tem_reg1,1,TIME_OUT);
	HAL_I2C_Mem_Read(&I2C1Handle,CHIP_1 + chip_offset ,REG_POWER_STATUS + reg_offset,
					MEM_ADDR_SIZE,&tem_reg2,1,TIME_OUT);
	portPSE->detection      = (PSE_DETECT_T)(tem_reg1 & 0x03);
	portPSE->classification = (PSE_CLASS_T)((tem_reg1 >> 4) & 0x03);
	portPSE->power          = tem_reg2;
/*Get the current value for the PSE port*/ //The current = 122.07 * tem_reg (uA)
	HAL_I2C_Mem_Read(&I2C1Handle,CHIP_1 + chip_offset,REG_PORT1_CURRENT_LO + reg_offset,
					MEM_ADDR_SIZE,&tem_reg1,1,TIME_OUT);			 
	HAL_I2C_Mem_Read(&I2C1Handle,CHIP_1 + chip_offset,REG_PORT1_CURRENT_HI + reg_offset,
					MEM_ADDR_SIZE,&tem_reg2,1,TIME_OUT);
	tem_reg =  (((uint16_t)tem_reg2) << 8) | ((uint16_t)tem_reg1);
	portPSE->current  = tem_reg;
/*Get the voltage value for the PSE port*/ //The voltage = 5.835 * tem_reg (mV)
	HAL_I2C_Mem_Read(&I2C1Handle,CHIP_1 + chip_offset,REG_PORT1_VOLTAGE_LO + reg_offset,
					MEM_ADDR_SIZE,&tem_reg1,1,TIME_OUT);			 
	HAL_I2C_Mem_Read(&I2C1Handle,CHIP_1 + chip_offset,REG_PORT1_VOLTAGE_HI + reg_offset,
					MEM_ADDR_SIZE,&tem_reg2,1,TIME_OUT);
	tem_reg =  (((uint16_t)tem_reg2) << 8) | ((uint16_t)tem_reg1);
	portPSE->voltage  = tem_reg;
  }
 
//GET PORT EVENTS 
/***********************************************************************/
//函数名  ： getPoePortEvent(uint8_t portNo,PSE_PORT_T* portPSE)
//功能    ： 获取POE端口的供电状态
//参数    ： portNo：POE端口号  portPSE:存放获取到的POE端口状态
//返回值  ： None
/***********************************************************************/
void getPoePortEvent(uint8_t portNo , PORT_EVENT_T *pevent)
  {
	uint8_t tem_reg ;
	uint8_t  chip_offset;                     //Used for offset value for which chip
	uint8_t  bit_offset;
		 
	chip_offset = portNo/4;
	bit_offset  = portNo%4;
/*Get power event value*/
	HAL_I2C_Mem_Read(&I2C1Handle,CHIP_1 + chip_offset,REG_POWER_EVENT_COR,MEM_ADDR_SIZE,&tem_reg,1,TIME_OUT);		 
	pevent->port_power.PG_CHG   = (tem_reg >> (bit_offset + 4)) & 0x01;
	pevent->port_power.PWEN_CHG = (tem_reg >> (bit_offset)) & 0x01;
/*Get detect event value*/
	HAL_I2C_Mem_Read(&I2C1Handle,CHIP_1 + chip_offset,REG_DETECT_EVENT_COR,MEM_ADDR_SIZE,&tem_reg,1,TIME_OUT);
	pevent->port_end.DET_END  =(tem_reg >> (bit_offset)) & 0x01;
	pevent->port_power.PWEN_CHG = (tem_reg >> (bit_offset + 4)) & 0x01;
/*Get fault event value*/
	HAL_I2C_Mem_Read(&I2C1Handle,CHIP_1 + chip_offset,REG_FAULT_EVENT_COR,MEM_ADDR_SIZE,&tem_reg,1,TIME_OUT);
	pevent->port_fault.TCUT =(tem_reg >> (bit_offset)) & 0x01;
	pevent->port_fault.DIS  = (tem_reg >> (bit_offset + 4)) & 0x01;
/*Get startup event value*/
	HAL_I2C_Mem_Read(&I2C1Handle,CHIP_1 + chip_offset,REG_STARTUP_EVENT_COR,MEM_ADDR_SIZE,&tem_reg,1,TIME_OUT);
	pevent->port_startup.TSTART = (tem_reg >> (bit_offset)) & 0x01;
	pevent->port_startup.ICV    = (tem_reg >> (bit_offset + 4)) & 0x01;
  }


/*********************************************************************************************/
/*******The functions for set the configuration of each ports and for each M_4PSE board*******/
/*********************************************************************************************/
/***********************************************************************/
//函数名  ： setPoePortConfig(uint8_t portNo,PORT_CONFIG_T* port_config)
//功能    ： 获取POE端口的供电状态
//参数    ： portNo：POE端口号  port_config:存放需要的配置信息
//返回值  ： uint8_t : '0'for set configration failed
/***********************************************************************/		 
uint8_t setPoePortConfig(uint8_t portNo ,PORT_CONFIG_T* port_config)
  {
	uint8_t tem_reg;
	uint8_t  chip_offset;                     //Used for offset value for which chip
	uint8_t  bit_offset;
		 
	chip_offset = portNo/4;
	bit_offset  = portNo%4;
/*Set the port operation mode*/			     
	HAL_I2C_Mem_Read(&I2C1Handle,CHIP_1 + chip_offset,REG_OPERATING_MODE,MEM_ADDR_SIZE,&tem_reg,1,TIME_OUT); 
	tem_reg &=  ~(0x03 << (bit_offset*2));
	tem_reg |= (port_config->port_op_mode << (bit_offset*2));
	if(HAL_OK != HAL_I2C_Mem_Write(&I2C1Handle,CHIP_1 + chip_offset,REG_OPERATING_MODE,MEM_ADDR_SIZE,
								   &tem_reg,1,TIME_OUT)){return 0;}
/*Set the port mid_span mode*/
	HAL_I2C_Mem_Read(&I2C1Handle,CHIP_1 + chip_offset,REG_MIDSPAN_ENBALE,MEM_ADDR_SIZE,&tem_reg,1,TIME_OUT); 
	tem_reg &= ~(0x01 << (bit_offset));
	tem_reg |= (port_config->port_op_mode << bit_offset);
	if(HAL_OK != HAL_I2C_Mem_Write(&I2C1Handle,CHIP_1 + chip_offset,REG_MIDSPAN_ENBALE,MEM_ADDR_SIZE,
								   &tem_reg,1,TIME_OUT) ){return 0;}
/*Set ICUT value for the port*/
	if( HAL_OK != HAL_I2C_Mem_Write(&I2C1Handle,CHIP_1 + chip_offset,REG_PORT1_ICUT + bit_offset*5,MEM_ADDR_SIZE,
									&tem_reg,1,TIME_OUT)){return 0;}
	return 1;
  }		

/***********************************************************************/
//函数名  ： setPoePortOn(uint8_t portNo)
//功能    ： 设置端口强制48V供电
//参数    ： portNo：POE端口号（0~8为正常情况，设为‘0xff’即可以对多有端口进行设置操作）
//返回值  ： uint8_t : '1'表示设置成功  ‘0’表示设置失败
/***********************************************************************/	
uint8_t setPoePortOn(uint8_t portNo)
  {
	uint8_t tem_reg;
	uint8_t chip_offset;      //used for the chip seclect
	uint8_t bit_offset;       //used for the port seclect

	chip_offset = portNo/4;
	bit_offset  = portNo%4;
/*Before force the port on , do clear the AC disconnect and DC disconnect*/
	if (portNo == 0xff)  //The portNo '0xff' indicates that its a all-port operation
	{
	  tem_reg =  0x00;
	  if( HAL_OK != HAL_I2C_Mem_Write(&I2C1Handle,CHIP_1,REG_DISCONNECT_ENBALE,MEM_ADDR_SIZE,
									  &tem_reg,1,TIME_OUT) ){return 0;}
	  if( HAL_OK != HAL_I2C_Mem_Write(&I2C1Handle,CHIP_2,REG_DISCONNECT_ENBALE,MEM_ADDR_SIZE,
									  &tem_reg,1,TIME_OUT) ){return 0;}
	}
	else// if( portNo > 0x00 && portNo < 0x08)
	{
	  HAL_I2C_Mem_Read(&I2C1Handle,CHIP_1 + chip_offset,REG_DISCONNECT_ENBALE,MEM_ADDR_SIZE,
					   &tem_reg,1,TIME_OUT);
	  tem_reg &= ~((0x01) << bit_offset);    //Clear the DC disconnect bit
	  tem_reg &= ~((0x01) << (bit_offset +4)); //Clear the AC disconnect bit	
	  if( HAL_OK != HAL_I2C_Mem_Write(&I2C1Handle,CHIP_1 + chip_offset,REG_DISCONNECT_ENBALE,
	                                  MEM_ADDR_SIZE,&tem_reg,1,TIME_OUT) ){ return 0;}	  
	}
/*Set the the port's POWER_ON bit*/
	if (portNo ==0xff)    //The portNo '0xff' indicates that its a all-port operation
	{
	  tem_reg = 0x0f;
	  if( HAL_OK != HAL_I2C_Mem_Write(&I2C1Handle,CHIP_1,REG_POWER_ENABLE_PUSHBUTTON,MEM_ADDR_SIZE,
									  &tem_reg,1,TIME_OUT) ){return 0;}
	  if( HAL_OK != HAL_I2C_Mem_Write(&I2C1Handle,CHIP_2,REG_POWER_ENABLE_PUSHBUTTON,MEM_ADDR_SIZE,
									  &tem_reg,1,TIME_OUT) ){return 0;}
	}
	else
	{   
	  tem_reg = (0x01 << bit_offset);
	  if( HAL_OK != HAL_I2C_Mem_Write(&I2C1Handle,CHIP_1 + chip_offset,REG_POWER_ENABLE_PUSHBUTTON,
									  MEM_ADDR_SIZE,&tem_reg,1,TIME_OUT)){return 0;}	  
	}
	return 1;
  }
	
/***********************************************************************/
//函数名  ： setPoePortDown(uint8_t portNo)
//功能    ： 设置端口强制48V供电关闭
//参数    ： portNo：POE端口号（0~8为正常情况，设为‘0xff’即可以对所有端口进行设置操作）
//返回值  ： uint8_t : '1'表示设置成功  ‘0’表示设置失败
/***********************************************************************/
uint8_t setPoePortDown(uint8_t portNo)
  {
	uint8_t tem_reg;
	uint8_t chip_offset;      //used for the chip seclect
	uint8_t bit_offset;       //used for the port seclect

	chip_offset = portNo/4;
	bit_offset  = portNo%4 +4;
/*Set the the port's POWER_OFF bit*/
	if (portNo == 0xff)       //The portNo '0xff' indicates that its a all-port operation
	{ 
	  tem_reg = 0xf0;
	  if( HAL_OK != HAL_I2C_Mem_Write(&I2C1Handle,CHIP_1,REG_POWER_ENABLE_PUSHBUTTON,MEM_ADDR_SIZE,
	                                  &tem_reg,1,TIME_OUT) ){return 0;}
	  if( HAL_OK != HAL_I2C_Mem_Write(&I2C1Handle,CHIP_2,REG_POWER_ENABLE_PUSHBUTTON,MEM_ADDR_SIZE,
									  &tem_reg,1,TIME_OUT)){return 0;}					
	}
	else
	{
	  tem_reg = (0x01 << bit_offset);
	  if( HAL_OK != HAL_I2C_Mem_Write(&I2C1Handle,CHIP_1 + chip_offset,REG_POWER_ENABLE_PUSHBUTTON,
									  MEM_ADDR_SIZE,&tem_reg,1,TIME_OUT)){return 0;}			  
	}
/*After power off , AC disconnect should be enabled */
	if(portNo == 0xff)       //The portNo '0xff' indicates that its a all-port operation
	{
	  tem_reg = 0xf0;
	  if( HAL_OK != HAL_I2C_Mem_Write(&I2C1Handle,CHIP_1,REG_DISCONNECT_ENBALE,MEM_ADDR_SIZE,
									  &tem_reg,1,TIME_OUT)){return 0;}	
	  if( HAL_OK != HAL_I2C_Mem_Write(&I2C1Handle,CHIP_2,REG_DISCONNECT_ENBALE,MEM_ADDR_SIZE,
									  &tem_reg,1,TIME_OUT)){return 0;}					
	}
	else
	{
	  HAL_I2C_Mem_Read(&I2C1Handle,CHIP_1 + chip_offset,REG_DISCONNECT_ENBALE,MEM_ADDR_SIZE,
					   &tem_reg,1,TIME_OUT);
	  tem_reg |= ((0x01) << (bit_offset + 4)) ;
	  if( HAL_OK != HAL_I2C_Mem_Write(&I2C1Handle,CHIP_1 + chip_offset,REG_DISCONNECT_ENBALE,
									  MEM_ADDR_SIZE,&tem_reg,1,TIME_OUT)){return 0;}			
	}
	return 1;
  }	
  
/***********************************************************************/
//函数名  ： setPoeChipReset(uint8_t portNo)
//功能    ： 设置POE四路模块复位
//参数    ： portNo：端口所在的模块
//返回值  ： uint8_t : '1'表示设置成功  ‘0’表示设置失败
/***********************************************************************/
uint8_t setPoeChipReset(uint8_t portNo)
  {
	uint8_t tem_reg;
	uint8_t chip_offset;
				  
	chip_offset = portNo/4;
/*Set the bit of chip reset*/
	tem_reg = (0x01 << 4);        //The chip RESET is bit 4
	if(HAL_OK != HAL_I2C_Mem_Write(&I2C1Handle,CHIP_1 + chip_offset,REG_GLOBAL_PUSHBUTTON,
	                               MEM_ADDR_SIZE,&tem_reg,1,TIME_OUT)){return 0;}
	return 1;
  }
			
/***********************************************************************/
//函数名  ： setPoePortReset(uint8_t portNo)
//功能    ： 设置端口配置复位
//参数    ： portNo：POE端口号
//返回值  ： uint8_t : '1'表示设置成功  ‘0’表示设置失败
/***********************************************************************/		
uint8_t setPoePortReset(uint8_t portNo)
  {
	uint8_t tem_reg;
	uint8_t chip_offset;
	uint8_t bit_offset;
				
	chip_offset = portNo/4;
	bit_offset  = portNo%4;
/*Set the bit of port reset*/
	tem_reg = (0x01 << bit_offset);        //The chip RESET is bit 4
	if(HAL_OK != HAL_I2C_Mem_Write(&I2C1Handle,CHIP_1 + chip_offset,REG_GLOBAL_PUSHBUTTON,
	                               MEM_ADDR_SIZE,&tem_reg,1,TIME_OUT)){return 0;}
	return 1;
  }		
 
