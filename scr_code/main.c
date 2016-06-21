/**
*********************************************************************************
@File      : main.c
@Author    : Duke Lee
@Version   : V1.0
@Date      : 22-August-2014
@Breif     : Main C file for my set VETH2000 
*********************************************************************************
**/

/**
 @ref DIAL_UP_MODE
**/
/*
	0x01    : RSTP disabled
	0x02    : Reset to defaults
	others  : reserved 
*/

/**************Header include**********************/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "stm32f4xx.h"
#include "stm32f4xx_hal_conf.h"
#include "stm32f4xx_hal_iwdg.h"
#include "veth_system_clk.h"
#include "veth_hal_init.h"
#include "veth_eth_ap.h"
#include "veth_common.h"
#include "veth_com_uart.h"
#include "veth_marvell_88e6097f.h"
#include "veth_m_4pse.h"
#include "veth_RSTP.h"
#include "veth_sdram.h"
#include "veth_nor.h"
//#include "veth_web.h"
#include "veth_port.h"
#include "board_info.h"
#include "veth_netupgrade.h"
#include "veth_serial.h"
#include "veth_COM.h"
/**************parameters**********************/
ETH_HandleTypeDef  Eth_Handle;     //extern to File: stp_to.c
IWDG_HandleTypeDef IwdgHandle;
uint8_t mystr[]="abcdef";
uint8_t comdin[20];
uint8_t PC_MAC[6] = /*0xff,0xff,0xff,0xff,0xff,0xff}*/{0x54,0x04,0xa6,0xf2,0xd4,0x6e};
uint8_t command_tag = 1;
uint32_t g_reset_time;
uint8_t  g_reset_targ;
BIN_BUFFER_T recieved_package __attribute__((at(TEMP_UPGRADE_ADDR)));
/*************************************************/
#ifdef __GNUC__//for printf
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

/**************************************************************/
//Function:  Fputc for printf
PUTCHAR_PROTOTYPE
  {
  /* Place your implementation of fputc here */
  /* e.g. write a character to the EVAL_COM1 and Loop until the end of transmission */
#ifdef PRF_USART6
	HAL_UART_Transmit(&Uart6Handle, (uint8_t *)&ch, 1, 0xFFFF); 
#endif
#ifdef PRF_USART1
	HAL_UART_Transmit(&Uart1Handle,(uint8_t *)&ch,1,0xFFFF);
#endif
#ifdef PRF_UART5
	HAL_UART_Transmit(&Uart5Handle,(uint8_t *)&ch,1,0xFFFF);	
#endif
#ifdef PRF_UART4
	HAL_UART_Transmit(&Uart4Handle,(uint8_t *)&ch,1,0xFFFF);
#endif 
#ifdef PRF_UART7
	HAL_UART_Transmit(&Uart7Handle,(uint8_t *)&ch,1,0xFFFF);
#endif	
	return ch;
  } 

/***************************************************************************
	Main function
****************************************************************************/
int main ()
  {
	uint16_t    i;
	UART_T      uart_config;
	uint8_t     loop;
	struct magic_and_check magic;
	struct serial_config com;
	//printf("\n\r->I LOVE U\n\r");
/*********Set the system clock at first*********/
	SystemClock_Config();	
/**********RCC init*************/
	RCC_Configuration();
/**********NVIC init************/
	NVIC_Configuration();
/**********GPIO init************/	 
	GPIO_Configuration();
/**********UART init************/
	Uart6Handle.Instance  = USART6;
	Uart5Handle.Instance  = UART5;
	Uart1Handle.Instance  = USART1;
	Uart7Handle.Instance  = UART7;
	Uart4Handle.Instance  = UART4;

	uart_config.buad_rate = BUAD_115200;
	uart_config.wordlen   = WORDLENGTH_8;
	uart_config.stopbits  = UART_STOPBITS_1;
	uart_config.parity    = PARITY_NONE;
	 
	USART_Configuration(&Uart1Handle,&uart_config);	
	USART_Configuration(&Uart7Handle,&uart_config);
	USART_Configuration(&Uart4Handle,&uart_config);	 
	USART_Configuration(&Uart6Handle,&uart_config);
	USART_Configuration(&Uart5Handle,&uart_config); 
//	com.serial_bound_rate = 9600;
//	com.serial_data_bit   = 8;
//	com.serial_no         = 3;
//	com.serial_parity     = 0;
//	com.serial_stop_bit   = 1;
//	setComProperty(&com);
/*Some Variates initialization*/   
	printf("\n\r->Application Running...\n\r\n\r");
/**********I2C init**********/
	I2C_Configuration(&I2C1Handle);
/*********TIMER init*********/ 	 
	TIMx_Configuration();
/*For App is jumped from bootloader here the irq should be enabled*/
	__enable_irq();
/*Only read out the MAC address of the sets to do the Eth configuration*/	
	Init_InVariate();
//	printf("->Invariate config done...\n\r\n\r");
/**********ETH init**********/	 
	ETH_Configuration(&Eth_Handle,board.sets_info.The_MAC);
//	printf("->ETH config done...\n\r\n\r");
/***********MII_init************/
	MII_Init();     /*initiation for the MII port of marvell 88e6097f*/
//	printf("->MII config done...\n\r\n\r");
//	FLASH_Configuration();
//	FATfsDemo();
//	FLASH_Configuration();
//	NOR_demo();
	poeDemo();
//	resetInit();
//	while(1) 
//	{
//	  Blink_LED_RUN();
//	  resetOrReinit();
//	}
	testSerial();
/*My test for the upgrade through network*/
//	saveNetData(&tem_recieved_package);
	/**********IWDG init************/
	IwdgHandle.Instance = IWDG;
	IwdgHandle.Init.Prescaler = IWDG_PRESCALER_32;
	IwdgHandle.Init.Reload    = 0xfff;  //The maximum reload value  
	if(HAL_IWDG_Init(&IwdgHandle) != HAL_OK)
	{
	  printf("WatchDog Init failed!!!\n\r");
	}
/*The infinite loop with the all functions that VETH2000 sets has*/			
while(1)
  {
/*Check if the Flash exit data availiable*/
	getInvariate(&magic,sizeof(magic));
	if( ((0x02 & Get_DIAL_Mode()) == 0x02) || strcmp(magic.magic_num,board.magic_check.magic_num) )
	{
	  if(strcmp(magic.magic_num,board.magic_check.magic_num)){
	  }else{
	  }
	  /*Reset to defaults or the flash is empty*/
	  initBoardInfo();
	}else{
	  getInvariate(&board,sizeof(BOARD));
	}
	//initBoardInfo();
	displayBasicInfo(&board.sets_info);
	displayIpInfo(&board.ip_config);
//	printf("5 second timeout!!!\n\r");
//	resetInit();
//	while(1)
//	{
//	  Blink_LED_RUN();
//	  resetOrReinit();
//	}
	//updateNewConfig();
	
	if(configRSTP()){
	  printf("RSTP Running...\n\r");
	}else{
	  printf("RSTP instance create failed...\n\r");
	}
	/*Config the RSTP instance and start ETH RX&TX transmition*/
	/*Start the IWDG*/ 
	if(HAL_IWDG_Start(&IwdgHandle) != HAL_OK)
	{
	  printf("WatchDog Start failed!!!\n\r");
	}
	Set_Switch_Port_state(10,UID_PORT_FORWARDING);
	web_main();			
//#endif			 
  }			
}

/***************************************************************/
//Function : delay some time for my test
void Delay(int i)
  {
	int x;
	for(x=0;x<i;x++);
  }
/***************************************************************/
//Function : Delay time second  
void Delays(int time)
  {
	uint32_t tem_time;
	
	tem_time = sCount;
	while(1)
	 {
		WatchDogTimerReset();
		if(sCount - tem_time >= time)
		  break;
	 }
  }
/***************************************************************/
//Function : uint8_t Get_DIAL_MODE()
//Brief: Get the status of the MODE on board VETH2000
uint8_t Get_DIAL_Mode()
  {
	uint8_t mode_vector;
	GPIO_PinState  mode0;
	GPIO_PinState  mode1;
	GPIO_PinState  mode2;
	GPIO_PinState  mode3;
		
	mode0 = HAL_GPIO_ReadPin( GPIOF, GPIO_PIN_8  );   //PF8 : mode0
	mode1 = HAL_GPIO_ReadPin( GPIOF, GPIO_PIN_9  );   //PF9 : mode1
	mode2 = HAL_GPIO_ReadPin( GPIOF, GPIO_PIN_10 );	 //PF10: mode2
	mode3 = HAL_GPIO_ReadPin( GPIOC, GPIO_PIN_0  );   //PC0 : mode3			
/**/
	mode_vector = 0;
	if( mode0 == RESET )
	  mode_vector |=  (0x00) << 0;
	else
	  mode_vector |=  (0x01) << 0;
	if( mode1 == RESET )
	  mode_vector |=   (0x00) << 1;
	else
	  mode_vector |=   (0x01) << 1;
	if( mode2 == RESET )
	  mode_vector |=   (0x00) << 2;
	else
	  mode_vector |=   (0x01) << 2;
	if( mode3 == RESET )
	  mode_vector |=   (0x00) << 3;
	else
	  mode_vector |=   (0x01) << 3;
			
/*ensure the high 4-bit is all-zeros*/	
	mode_vector &= 0x0f;  
	return mode_vector;    
  }

/******************************************************************/
//Function : Blink_LED_RUN() 
void Blink_LED_RUN()	
  {
/*Set PA5 a '500ms' blink to drive the RUN_LED*/
	if (rCount >= 1)   
	  { 
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,GPIO_PIN_RESET);
		if(rCount >= 2)
		  {
		    WatchDogTimerReset();
			rCount = 0;
			HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,GPIO_PIN_SET);
		  }
	  }
  } 
/********************************************************************************/
//Function :  resetOrReinit()
//Brief    :  Get the reset status of the reset pin,to judge it is a reset operation or a reinit oper
//Parameter:  None 
//retval   :  None
/********************************************************************************/
void resetOrReinit()
  {	
	/*Each one second,get the status of the reset pin*/
	if(msCount - g_reset_time >= 200)
	{
	  g_reset_time = msCount;
	  if( HAL_GPIO_ReadPin( GPIOA, GPIO_PIN_12) == RESET) //PA12__reset status pin
	  {
		g_reset_targ++;	
	  }
	  else
	  {
	    if(g_reset_targ == 0)
		  {}
		else
		  if(g_reset_targ > 1 && g_reset_targ < 25)
		  {
		    printf("Reset in!!!\n\r");
			g_reset_targ = 0;
			/*Reset signal detected,reset the switch*/
			HAL_GPIO_WritePin(GPIOA,GPIO_PIN_11,GPIO_PIN_RESET);
		  }
		else 
		  if(g_reset_targ >= 25){
		    printf("The reinit in!!\n\r");
			initBoardInfo();//After reinit the configuration,update them into flash and restart 
			updateNewConfig();
			HAL_GPIO_WritePin(GPIOA,GPIO_PIN_11,GPIO_PIN_RESET);
		  }
		else{
		  g_reset_targ = 0;
		  //g_reset_time = sCount;
		}
	  }//end of else (if( HAL_GPIO_ReadPin( GPIOA, GPIO_PIN_11) == RESET))
	}//end of if(g_reset_time - sCount >=1)
  }//end of the function
/****************************resetInit***************************/
void resetInit() 
  {
	g_reset_time = msCount;
	g_reset_targ = 0;
  }
/********************************************************************************/
//Function :  WatchDogTimerRest()
//Brief    :  Reload the watchdog counter
//Parameter:  None 
//retval   :  None
/********************************************************************************/
void WatchDogTimerReset()
  {
	if(HAL_IWDG_Refresh(&IwdgHandle) != HAL_OK)
	  {
	  printf("Watchdog is hungry!!!\n\r\n\r");
	  }
  }

/********************************************************************************/
//Function :  setSelfRefresh()
//Brief    :  Set SDRAM into self-refresh mode
//Parameter:  None 
//retval   :  None
/********************************************************************************/
void setSelfRefresh()
  {
    uint16_t tem_reg;
	/* Auto refresh command */
	FMC_Bank5_6->SDCMR &= 0xffc00000;
	FMC_Bank5_6->SDCMR |= 0x000000F5;
	tem_reg = 1;
	while(tem_reg != 0x0){tem_reg = FMC_Bank5_6->SDSR & 0x00000020; };
  }
/********************************************************************************/
//Function :  setNormalMode()
//Brief    :  Wakeup SDRAM from self-refresh mode
//Parameter:  None 
//retval   :  None
/********************************************************************************/
void setNormalMode()
  {
	uint16_t tem_reg;
	/*Normal Mode command */
	FMC_Bank5_6->SDCMR &= 0xffc00000;
	FMC_Bank5_6->SDCMR |= 0x000000F3;
	tem_reg =1;
	while(tem_reg != 0x0){tem_reg = FMC_Bank5_6->SDSR & 0x00000020; };
  
  }