/**
**************************************************************************
@File    :  STM32F4xx_HAL_Driver\re_src\hal_init.c
@Author  :  Duke Lee
@Verdion :  V1.0
@Date    :  09-June-2014
@brief   :  To do the initial configuration for the hardware I will use
**************************************************************************
**/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "stm32f4xx.h"
#include "stm32f4xx_hal_conf.h"
#include "stm32f4xx_hal.h"
#include "veth_common.h"
#include "veth_com_uart.h"
#include "veth_hal_init.h"
#include "veth_marvell_88e6097f.h"
#include "veth_InVariate.h"
#include "veth_serial.h"
//#include "veth_web.h"
//#include "board_info.h"
#include "veth_mac.h"
/*********************************************************************************************/
//MICRO define 
#ifndef STM32F429xx

#define STM32F429xx

#endif

/*Define the ETH tx_buffer and rx_buffer */
uint8_t  TX_BUFFER[DESCRIPTORLIST_SIZE*MAX_TX_BUFSIZE] __attribute__((section("EXTSDRAM")));
uint8_t  RX_BUFFER[DESCRIPTORLIST_SIZE*MAX_RX_BUFSIZE] __attribute__((section("EXTSDRAM")));

/*Define the TX and RX buffer for USART6/USART1/UART5*/

uint8_t COM6_TX_buffer[MAX_TX_BUFSIZE];
uint8_t COM6_RX_buffer[MAX_RX_BUFSIZE];
uint8_t COM1_TX_buffer[MAX_TX_BUFSIZE];
uint8_t COM1_RX_buffer[MAX_RX_BUFSIZE];
uint8_t COM5_TX_buffer[MAX_TX_BUFSIZE];
uint8_t COM5_RX_buffer[MAX_RX_BUFSIZE];
uint8_t COM4_TX_buffer[MAX_TX_BUFSIZE];
uint8_t COM4_RX_buffer[MAX_RX_BUFSIZE];
uint8_t COM7_TX_buffer[MAX_TX_BUFSIZE];
uint8_t COM7_RX_buffer[MAX_RX_BUFSIZE];

uint8_t *read_com6;
uint8_t *read_com1;
uint8_t *read_com5;
/*Private definition**************************************************************************/
TIM_HandleTypeDef         TimHandle;
TIM_HandleTypeDef         TimsHandle;
TIM_HandleTypeDef         TimrHandle;
UART_HandleTypeDef        Uart6Handle;
UART_HandleTypeDef        Uart5Handle;
UART_HandleTypeDef        Uart1Handle;
UART_HandleTypeDef        Uart4Handle;
UART_HandleTypeDef        Uart7Handle;
SDRAM_HandleTypeDef       SDRAMHandle;
FMC_SDRAM_TimingTypeDef   SDRAM_Timing;
FMC_SDRAM_CommandTypeDef  SDRAM_Command;
NOR_HandleTypeDef         norHandle;
FMC_NORSRAM_TimingTypeDef Timing;
I2C_HandleTypeDef         I2C1Handle;
ETH_DMADescTypeDef        TX_LIST[DESCRIPTORLIST_SIZE];
ETH_DMADescTypeDef        RX_LIST[DESCRIPTORLIST_SIZE];
//ETH_HandleTypeDef  EthHandle;
uint32_t sCount;                                            //Target for the timer2 count ,'1s'
uint32_t msCount;                                           //TIM5 count ,'1ms'
uint32_t rCount;                                            //TIM3 count ,'500ms' 
//uint8_t KEY_MAC[6]  = {0x00,0x08,0x11,0x46,0xdd,0xde};      //The MAC address of my chip or set
uint8_t KEY_MAC[6]  = {0x00,0x05,0xd0,0x06,0xdd,0xde};      //The MAC address of my chip or set
uint8_t MGMT_MAC[6] = {0x01,0x80,0xc2,0x00,0x00,0x00};      //For the RSTP dest MAC address
uint8_t GET_NEXT[6] = {/*0x01,0x80,0xc1,0x00,0x00,0x00*/0xff,0xff,0xff,0xff,0xff,0xff};
VETH_T  SET_INFO = {"VETH3000","VethTeam@vorx","VETH3000(2gx-8t)","v1.0.00"};
VETH_IP_T SET_IP = {{192,168,1,1},{192,168,1,1},{255,255,255,0}};
USER_INFO_T USER_INFO = {"vorx","vorx"};
char the_magic[14] = "vorx_veth2000";
extern RSTP_COUNTER bpdu_count;

uint8_t g_set_name[20] = "VETH3000-4S";
uint8_t VETH3000_4S[20]= "VETH3000-4S";
uint8_t g_set_version[10] = "v1.1";
uint8_t g_set_type1[20]  ="2fx-6t";
uint8_t g_set_type2[20]  ="2gx-8t";
uint8_t g_set_type3[20]  ="2gx-2fx-6t-4s";
/**Pre-statement****************************************************************************/
//ETH_DMADescTypeDef *Creat_desc_list();
/*******************************************************************************/
//Function:  RCC configuration
//  To enable some clock for my source
/*******************************************************************************/
// To enabled the clock of source "XX",use  '__XX_CLK_ENABLE();' function 

void RCC_Configuration(void)
{
	__GPIOA_CLK_ENABLE();
	__GPIOB_CLK_ENABLE();
	__GPIOC_CLK_ENABLE();
	__GPIOD_CLK_ENABLE();
	__GPIOE_CLK_ENABLE();	
	__GPIOF_CLK_ENABLE();
	__GPIOG_CLK_ENABLE();
	__GPIOH_CLK_ENABLE();
	__GPIOI_CLK_ENABLE();	
	__USART6_CLK_ENABLE();
	__USART1_CLK_ENABLE();
	__UART5_CLK_ENABLE();
	__UART7_CLK_ENABLE();
	__UART4_CLK_ENABLE();
	__TIM2_CLK_ENABLE();
	__TIM5_CLK_ENABLE();
	__TIM3_CLK_ENABLE();	
	__ETHMAC_CLK_ENABLE();
	__ETHMACTX_CLK_ENABLE();
	__ETHMACRX_CLK_ENABLE();
	__I2C1_CLK_ENABLE();
	__FMC_CLK_ENABLE();
	__DMA1_CLK_ENABLE();
	__DMA2_CLK_ENABLE();
}

/*******************************************************************************/
//Function:  NVIC configuration
// To configure the IT priority for the IRQ handler 
/*******************************************************************************/
void NVIC_Configuration(void)	
{
//Set the priotity group for my interrupt,
	HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_2);

//USART6 priority
//	HAL_NVIC_SetPriority(USART6_IRQn,0,0);	
//	HAL_NVIC_EnableIRQ(USART6_IRQn);
//USART1 priority
	HAL_NVIC_SetPriority(USART1_IRQn,3,2);	
	HAL_NVIC_EnableIRQ(USART1_IRQn);
////UART5  priority
//	HAL_NVIC_SetPriority(UART5_IRQn,0,0);	
//	HAL_NVIC_EnableIRQ(UART5_IRQn);
////UART7  priority
//	HAL_NVIC_SetPriority(UART7_IRQn,0,0);	
//	HAL_NVIC_EnableIRQ(UART7_IRQn);	
////UART4  priority
//	HAL_NVIC_SetPriority(UART4_IRQn,0,0);	
//	HAL_NVIC_EnableIRQ(UART4_IRQn);		
//TIM2 priority	    
	HAL_NVIC_SetPriority(TIM2_IRQn,2,0);	
	HAL_NVIC_EnableIRQ(TIM2_IRQn);
/*TIM3 priority*/
	HAL_NVIC_SetPriority(TIM3_IRQn,2,1);	
	HAL_NVIC_EnableIRQ(TIM3_IRQn);	
//TIM5 priority
	HAL_NVIC_SetPriority(TIM5_IRQn,2,2);	
	HAL_NVIC_EnableIRQ(TIM5_IRQn);
//ETH Priority
	HAL_NVIC_SetPriority(ETH_IRQn,2,1);
	HAL_NVIC_EnableIRQ(ETH_IRQn);
	
//I2C priority
  HAL_NVIC_SetPriority(I2C1_ER_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(I2C1_ER_IRQn);
  HAL_NVIC_SetPriority(I2C1_EV_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(I2C1_EV_IRQn);
  
/* NVIC configuration for DMA transfer complete interrupt (USART6_TX) */
  HAL_NVIC_SetPriority(USART6_DMA_TX_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(USART6_DMA_TX_IRQn);
    
/* NVIC configuration for DMA transfer complete interrupt (USART6_RX) */
  HAL_NVIC_SetPriority(USART6_DMA_RX_IRQn, 1, 1);   
  HAL_NVIC_EnableIRQ(USART6_DMA_RX_IRQn);
  
/* NVIC configuration for DMA transfer complete interrupt (UART5_TX) */
  HAL_NVIC_SetPriority(UART5_DMA_TX_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(UART5_DMA_TX_IRQn);
    
/* NVIC configuration for DMA transfer complete interrupt (UART5_RX) */
  HAL_NVIC_SetPriority(UART5_DMA_RX_IRQn, 1, 1);   
  HAL_NVIC_EnableIRQ(UART5_DMA_RX_IRQn);
  
/* NVIC configuration for DMA transfer complete interrupt (UART7_TX) */
  HAL_NVIC_SetPriority(UART7_DMA_TX_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(UART7_DMA_TX_IRQn);
    
/* NVIC configuration for DMA transfer complete interrupt (UART7_RX) */
  HAL_NVIC_SetPriority(UART7_DMA_RX_IRQn, 1, 1);   
  HAL_NVIC_EnableIRQ(UART7_DMA_RX_IRQn);
  
/* NVIC configuration for DMA transfer complete interrupt (UART4_TX) */
  HAL_NVIC_SetPriority(UART4_DMA_TX_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(UART4_DMA_TX_IRQn);
    
/* NVIC configuration for DMA transfer complete interrupt (UART4_RX) */
  HAL_NVIC_SetPriority(UART4_DMA_RX_IRQn, 1, 1);   
  HAL_NVIC_EnableIRQ(UART4_DMA_RX_IRQn);
	
}

/*******************************************************************************/
//Function:  GPIO configuration
// To configure GPIO for  proper functions 
/*******************************************************************************/
void GPIO_Configuration(void)
  {
	GPIO_InitTypeDef GPIO_InitStructure;

//Configure PC6 as USART6_TX,and PC7 as USART6_RX	
	GPIO_InitStructure.Pin = GPIO_PIN_6;	  
	GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
	GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStructure.Pull	= GPIO_PULLUP;
	GPIO_InitStructure.Alternate = GPIO_AF8_USART6;
	HAL_GPIO_Init(GPIOC,&GPIO_InitStructure);
	
	GPIO_InitStructure.Pin = GPIO_PIN_7;
	GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
	GPIO_InitStructure.Pull = GPIO_NOPULL; 
	GPIO_InitStructure.Alternate = GPIO_AF8_USART6;
	HAL_GPIO_Init(GPIOC,&GPIO_InitStructure);

//Configure PA9 as USART1_TX , PA10 as USART1_RX
	GPIO_InitStructure.Pin = GPIO_PIN_9;	  
	GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
	GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStructure.Pull	= GPIO_PULLUP;
	GPIO_InitStructure.Alternate = GPIO_AF7_USART1;
	HAL_GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_InitStructure.Pin = GPIO_PIN_10;	  
	GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
	GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStructure.Pull	= GPIO_NOPULL;
	GPIO_InitStructure.Alternate = GPIO_AF7_USART1;
	HAL_GPIO_Init(GPIOA,&GPIO_InitStructure);

//Configure PC12 as UART5_TX,PD2 as UART5_RX 
	GPIO_InitStructure.Pin = GPIO_PIN_12;	  
	GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
	GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStructure.Pull	= GPIO_PULLUP;
	GPIO_InitStructure.Alternate = GPIO_AF8_UART5;
	HAL_GPIO_Init(GPIOC,&GPIO_InitStructure);

	GPIO_InitStructure.Pin = GPIO_PIN_2;	  
	GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
	GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStructure.Pull	= GPIO_NOPULL;
	GPIO_InitStructure.Alternate = GPIO_AF8_UART5;
	HAL_GPIO_Init(GPIOD,&GPIO_InitStructure);

//#ifdef STM32F429xx  //These UART4 and UART7 port are only availiable of the stm32f429xx
//Configure the PF7 as the  UART7_TX,while the PF6 as the UART7_RX   
	GPIO_InitStructure.Pin = GPIO_PIN_7;	  
	GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
	GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStructure.Pull	= GPIO_PULLUP;
	GPIO_InitStructure.Alternate = GPIO_AF8_UART7;
	HAL_GPIO_Init(GPIOF,&GPIO_InitStructure);

	GPIO_InitStructure.Pin = GPIO_PIN_6;	  
	GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
	GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStructure.Pull	= GPIO_NOPULL;
	GPIO_InitStructure.Alternate = GPIO_AF8_UART7;
	HAL_GPIO_Init(GPIOF,&GPIO_InitStructure);

//Configure the PC10 as UART4_TX,while the PC11 as UART4_RX
	GPIO_InitStructure.Pin = GPIO_PIN_10;	  
	GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
	GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStructure.Pull	= GPIO_PULLUP;
	GPIO_InitStructure.Alternate = GPIO_AF8_UART4;
	HAL_GPIO_Init(GPIOC,&GPIO_InitStructure);

	GPIO_InitStructure.Pin = GPIO_PIN_11;	  
	GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
	GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStructure.Pull	= GPIO_NOPULL;
	GPIO_InitStructure.Alternate = GPIO_AF8_UART4;
	HAL_GPIO_Init(GPIOC,&GPIO_InitStructure);
//#endif

//Configure PD12,PD13,PD14,PD15 as OUTPUT   
//PS:these pins are connect to LEDs on STM32F4XX_DISCOVERY
//  GPIO_InitStructure.Pin = GPIO_PIN_15 | GPIO_PIN_13 | GPIO_PIN_14 |GPIO_PIN_12 ;
//  GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
//  GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
//  GPIO_InitStructure.Pull = GPIO_NOPULL;
//  HAL_GPIO_Init(GPIOD, &GPIO_InitStructure);

/***Configure the ports that are used as GPIO output***/
//Configure the PA5 pin as GPIO output LED_RUN , PA6 as COM1_CTR for UART chip 1
	GPIO_InitStructure.Pin = GPIO_PIN_5 | GPIO_PIN_6;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_6,GPIO_PIN_RESET);
//Configure the PH6\PH7 as MODE1_0\MODE1_1 for UART chip 1
	GPIO_InitStructure.Pin   = GPIO_PIN_6 | GPIO_PIN_7;
	GPIO_InitStructure.Mode  = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
	GPIO_InitStructure.Pull  = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOH, &GPIO_InitStructure);
//Configure the PA4 as MODE1_2 for UART chip 1
	GPIO_InitStructure.Pin   = GPIO_PIN_4 ;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOH,GPIO_PIN_7,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOH,GPIO_PIN_6,GPIO_PIN_SET);
//Configure the PG12 PG9 PG10 as MODE2_0 MODE2_1 MODE2_2 for the UART chip 2, while PG6 as the COM4_CTR for uart4
	GPIO_InitStructure.Pin   = GPIO_PIN_12 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_6;
	GPIO_InitStructure.Mode  = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
	GPIO_InitStructure.Pull  = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOG, &GPIO_InitStructure);
	HAL_GPIO_WritePin(GPIOG,GPIO_PIN_10,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOG,GPIO_PIN_9,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOG,GPIO_PIN_12,GPIO_PIN_SET);
//Configure PC8,PC9 as COM2_CTR and COM3_CTR for thr uart port
	GPIO_InitStructure.Pin   = GPIO_PIN_8 | GPIO_PIN_9;
	GPIO_InitStructure.Mode  = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
	GPIO_InitStructure.Pull  = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_9,GPIO_PIN_RESET);
/***Configure the port that are configured as GPIO input***/
//Configure the PF8 PF9 PF10 as INPUT for the GLOBAL_MODE0~2
	GPIO_InitStructure.Pin   = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10;
	GPIO_InitStructure.Mode  = GPIO_MODE_INPUT;
	GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
	GPIO_InitStructure.Pull  = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOF, &GPIO_InitStructure);
//Configure the PC0 as INPUT for the GLOBAL_MODE3
	GPIO_InitStructure.Pin   = GPIO_PIN_0;
	GPIO_InitStructure.Mode  = GPIO_MODE_INPUT;
	GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
	GPIO_InitStructure.Pull  = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);

/***Configure The ETH PINS***/
//ETH pin base on GPIOA  
	GPIO_InitStructure.Pin   = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_7;
	GPIO_InitStructure.Mode  = GPIO_MODE_AF_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
	GPIO_InitStructure.Pull  = GPIO_NOPULL;
	GPIO_InitStructure.Alternate = GPIO_AF11_ETH;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
//ETH pin base on  GPIOB	
	GPIO_InitStructure.Pin   = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_8 | GPIO_PIN_10 | GPIO_PIN_11 | 
							   GPIO_PIN_12 | GPIO_PIN_13;
	GPIO_InitStructure.Mode  = GPIO_MODE_AF_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
	GPIO_InitStructure.Pull  = GPIO_NOPULL;
	GPIO_InitStructure.Alternate = GPIO_AF11_ETH;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);

//ETH pin base on GPIOC	
	GPIO_InitStructure.Pin   = GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5;
	GPIO_InitStructure.Mode  = GPIO_MODE_AF_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
	GPIO_InitStructure.Pull  = GPIO_NOPULL;
	GPIO_InitStructure.Alternate = GPIO_AF11_ETH;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);

//I2C pin on GPIOB
	GPIO_InitStructure.Pin        = GPIO_PIN_6 | GPIO_PIN_9;
	GPIO_InitStructure.Alternate  = GPIO_AF4_I2C1;
	GPIO_InitStructure.Mode       = GPIO_MODE_AF_OD;
	GPIO_InitStructure.Pull       = GPIO_PULLUP;
	GPIO_InitStructure.Speed      = GPIO_SPEED_MEDIUM;
	HAL_GPIO_Init(GPIOB,&GPIO_InitStructure);

//PA11 for RST_FROM_CPU
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_11,GPIO_PIN_SET);
	GPIO_InitStructure.Pin = GPIO_PIN_11;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
	GPIO_InitStructure.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
//PA12 for RST_STA	
	GPIO_InitStructure.Pin   = GPIO_PIN_12;
	GPIO_InitStructure.Mode  = GPIO_MODE_INPUT;
	GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
	GPIO_InitStructure.Pull  = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);	
  }


/****************************************************************************/
//Function : USART_Configuration
//Configure the way in which my USART works
/****************************************************************************/
void USART_Configuration(UART_HandleTypeDef *huart,UART_T *puart)
{
	UART_InitTypeDef UART_InitStructure;
//UartHandle 
	UART_InitStructure.BaudRate   = puart->buad_rate ;
	UART_InitStructure.WordLength =  puart->wordlen ;
	UART_InitStructure.StopBits   =  puart->stopbits ;
	UART_InitStructure.Parity     =  puart->parity ;
	//UART_InitStructure.Mode = UART_MODE_TX | UART_MODE_RX;
	UART_InitStructure.Mode = (USART1 == huart->Instance)? UART_MODE_TX | UART_MODE_RX
	                                                     : UART_MODE_TX ;	
	UART_InitStructure.HwFlowCtl = UART_HWCONTROL_NONE;
	UART_InitStructure.Parity = UART_PARITY_NONE;
	
	huart->Init = UART_InitStructure;
	HAL_UART_Init(huart);
#ifdef MY_DEBUG
	printf("hahahahahahahaha\n\r");
#endif
	if(USART1 == huart->Instance)
	{ 
	  __HAL_UART_ENABLE_IT(huart,UART_IT_RXNE); 
	  __HAL_UART_ENABLE(huart);
	}		
	huart->State = HAL_UART_STATE_READY;
}
/********************************************************************************/
//Function :  TIMx_Configuration
//Set the indicated timers , '1s' counter and '1ms' counter. 
//The computing method('x'refers to the time): (prescaler+1) * (peiod+1) = hclk/2 * x.    
/********************************************************************************/
void TIMx_Configuration()
{
    uint32_t hclk;
	
/* Get hclk frequency value */
	hclk = HAL_RCC_GetHCLKFreq();
	 
/* TimHandle for TIMx (also TIM2) ,a '1s' counter */	
	TimHandle.Instance = TIM2; 
	TimHandle.Init.Prescaler   =  ((hclk/2) / 3000) - 1 ; 
	TimHandle.Init.Period      =   3000 -1 ;
	TimHandle.Init.CounterMode =   0x0000;
	TimHandle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;

	HAL_TIM_Base_Init(&TimHandle);
	sCount = 0;
	__HAL_TIM_CLEAR_FLAG(&TimHandle,TIM_FLAG_UPDATE);

	__HAL_TIM_ENABLE_IT(&TimHandle,TIM_IT_UPDATE);
	
	__HAL_TIM_ENABLE(&TimHandle);
   
/*TimsHandle for TIMs(also TIM5) ,a '1ms' counter*/
	TimsHandle.Instance = TIM5; 
	TimsHandle.Init.Prescaler   =  ((hclk/2)/1000) / 300 - 1 ;
	TimsHandle.Init.Period      =   300 - 1;
	TimsHandle.Init.CounterMode =   0x0000;
	TimsHandle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
   
	HAL_TIM_Base_Init(&TimsHandle);
	msCount = 0;
	__HAL_TIM_CLEAR_FLAG(&TimsHandle,TIM_FLAG_UPDATE);
	
	__HAL_TIM_ENABLE_IT(&TimsHandle,TIM_IT_UPDATE);

	__HAL_TIM_ENABLE(&TimsHandle);

/*TimrHandle for TIMr(also TIM3),a '500ms' counter*/
	TimrHandle.Instance = TIM3; 
	TimrHandle.Init.Prescaler   =  ((hclk/2)) / 3000 - 1 ;
	TimrHandle.Init.Period      =   3000 - 1;
	TimrHandle.Init.CounterMode =   0x0000;
	TimrHandle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
   
	HAL_TIM_Base_Init(&TimrHandle);
	rCount = 0;
	__HAL_TIM_CLEAR_FLAG(&TimrHandle,TIM_FLAG_UPDATE);
	
	__HAL_TIM_ENABLE_IT(&TimrHandle,TIM_IT_UPDATE);

	__HAL_TIM_ENABLE(&TimrHandle);
}

/********Get_ms_Count()**********/
uint32_t Get_ms_Count()
{
   return msCount;
}
/********************************************************************************/
//Function :  ETH_Configuration
//Set the TIMx to works in the right way
/********************************************************************************/
void ETH_Configuration(ETH_HandleTypeDef *hal_eth,uint8_t *pMAC)
{	
	MAC_ENTRY_T MAC_entry;
	uint8_t loop;
	
	hal_eth->Init.AutoNegotiation = ETH_AUTONEGOTIATION_ENABLE ; 
	hal_eth->Init.Speed = ETH_SPEED_100M;
	hal_eth->Init.DuplexMode = ETH_MODE_FULLDUPLEX;
	hal_eth->Init.PhyAddress = 2;
	hal_eth->Init.MACAddr = pMAC;
	hal_eth->Init.RxMode = ETH_RXPOLLING_MODE;
	hal_eth->Init.MediaInterface = ETH_MEDIA_INTERFACE_MII;
	hal_eth->Init.ChecksumMode = ETH_CHECKSUM_BY_SOFTWARE;

	hal_eth->Instance = ETH;
 	 
	HAL_ETH_Init(hal_eth);   
//	TX_LIST = Creat_desc_list();
//	RX_LIST = Creat_desc_list();
#ifdef MY_DEBUG
	printf("TX BUFFER:");
	printf("%x\n\r",(uint32_t)TX_BUFFER);
	printf("RX_BUFFER:");
	printf("%x\n\r",(uint32_t)RX_BUFFER);
#endif
	HAL_ETH_DMATxDescListInit(hal_eth,&TX_LIST[0],TX_BUFFER,DESCRIPTORLIST_SIZE);
	HAL_ETH_DMARxDescListInit(hal_eth,&RX_LIST[0],RX_BUFFER,DESCRIPTORLIST_SIZE);
	Set_Switch_Port_state(10,UID_PORT_DISABLED);
///*Add a static MAC entry into the MAC table ,to indicate the port is the CPU port*/
//	MAC_entry.mac_entry_oper = LOAD_PURGE_ENTRY_FID;
//#ifdef CPU_PORT_10
//	MAC_entry.mac_entry_data = 0x0e | (0x01 << 14) | 0x0000;      //bit[15](trunk):'0'; bits[14:4]:port_vector; bits[3:0]:0x07 means the static entry ;	 
//#endif
//	for(loop= 0 ; loop<6 ; loop++)
//	{
//	  MAC_entry.mac_entry_MAC[loop]  = pMAC[loop];
//	}
//	MAC_entry.mac_entry_fid = 1;
//	Load_Purge_ATUentry(&MAC_entry);
//	Flush_Move_ATUentry(ALL_NONSTA,FLUSH_ENTRY,0x00,0x00); 
}

/********************************************************************************/
//Function : Variate_init()
//Do the initialization for some variates 
/********************************************************************************/
void Variate_init()
{
/*point the read pointer to the COMx_RX_buffer*/
	read_com6 = COM6_RX_buffer;
	read_com5 = COM5_RX_buffer;
	read_com1 = COM1_RX_buffer;
}

/********************************************************************************/
//Function : MII_Init()
//Brief    : Forced the CPU port on with 100M FULLDUPLEX
/********************************************************************************/
void MII_Init()
{
	uint16_t tem_reg;
/*Change the MII port to be 100M force mode*/
/*At first,change the port to 'link_down' mode,for the speed can be forced only if the link is down*/
	Eth_Handle.Init.PhyAddress = PORT_10;
	HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_PORT_STATUS,&tem_reg);
#ifdef MY_DEBUG
	printf("Before forced:");
	printf("%x \n\r",tem_reg);
#endif 
/*Force the port link down*/
	HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_PCS_CONTROL,&tem_reg);
	tem_reg &= ~(0x0030);     //Clear the bit 4 and bit 5
	tem_reg |= 0x0010;        //Set the link value '0' and forced link value '1' to force the link down 
	HAL_ETH_WritePHYRegister(&Eth_Handle,REG_PCS_CONTROL,tem_reg);
/*Force the port 100M full DPX*/
	HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_PCS_CONTROL,&tem_reg); 	
	tem_reg &= 0xfff0;          
	tem_reg |= 0x000d;
	HAL_ETH_WritePHYRegister(&Eth_Handle,REG_PCS_CONTROL,tem_reg);
/*And then force the port link-on*/
	HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_PCS_CONTROL,&tem_reg);
	tem_reg &= ~(0x0030);     //Clear the bit 4 and bit 5
	tem_reg |= 0x0030;        //Set the link value '0' and forced link value '1' to force the link down 
	HAL_ETH_WritePHYRegister(&Eth_Handle,REG_PCS_CONTROL,tem_reg);

	HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_PORT_STATUS,&tem_reg);
#ifdef MY_DEBUG
	printf("After the force operation: %x \n\r",tem_reg);
#endif		 
}
/********************************************************************************/
//Function  : forceLinkDowm(int portNo)
//Brief     : force the port's MAC link down
//Parameter : portNo ,the number of the port (ranges from 0~9)
/********************************************************************************/
void forceLinkDown(int portNo)
  {
	uint16_t tem_reg;
	Eth_Handle.Init.PhyAddress = PORT_0 + portNo;
/*Get the value out ,modify it and then fill in back*/		
	HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_PCS_CONTROL,&tem_reg);
	tem_reg &= ~(0x0030);     //Clear the bit 4 and bit 5
	tem_reg |= 0x0010;        //Set the link value '0' and forced link value '1' to force the link down 
	HAL_ETH_WritePHYRegister(&Eth_Handle,REG_PCS_CONTROL,tem_reg);
  }
/********************************************************************************/
//Function  : forceLinkUp(int portNo)
//Brief     : force the port's MAC link up
//Parameter : portNo ,the number of the port (ranges from 0~9)
/********************************************************************************/
void forceLinkUp(int portNo)
  {
	uint16_t tem_reg;		
	Eth_Handle.Init.PhyAddress = PORT_0 + portNo;
/*Get the value out ,modify it and then fill in back*/		
	HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_PCS_CONTROL,&tem_reg);
	tem_reg &= ~(0x0030);     //Clear the bit 4 and bit 5
	tem_reg |= 0x0030;        //Set the link value '0' and forced link value '1' to force the link down 
	HAL_ETH_WritePHYRegister(&Eth_Handle,REG_PCS_CONTROL,tem_reg);
  }
/********************************************************************************/
//Function  : cancleForceLink(int portNo)
//Brief     : Cancle  the port's force mode
//Parameter : portNo ,the number of the port (ranges from 0~9)
/********************************************************************************/
void cancleForceLink(int portNo)
  {
	uint16_t tem_reg;		
	Eth_Handle.Init.PhyAddress = PORT_0 + portNo;
/*Get the value out ,modify it and then fill in back*/		
	HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_PCS_CONTROL,&tem_reg);
	tem_reg &= ~(0x0030);     //Clear the bit 4 and bit 5      
	HAL_ETH_WritePHYRegister(&Eth_Handle,REG_PCS_CONTROL,tem_reg);
  }
/******************************************************************/
//Function :  RSTP_Init()
//Brief    : Initialize the switch 88E6097F for the RSTP protocol
/******************************************************************/
void RSTP_Init()
  {
    MAC_ENTRY_T  mac_entry;
	uint8_t      loop;
	uint16_t     tem_reg;
	/*Configure the CPU port in DSA tag mode*/
	Eth_Handle.Init.PhyAddress = PORT_10;
	HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_PORT_CONTROL,&tem_reg);
	tem_reg &= PORT_MODE_RESET;       //Clear the bits[13:12](EGRESS MODE bits)
	tem_reg &= PORT_FRAME_RESET;      //Clear the bits[9:8](FRAME MODE bits)
	tem_reg |= PORT_UNMODIFIED_MODE;  //Set the bits[13:12] as unmodified mode
//	tem_reg |= PORT_FRAME_DSAMODE;    //Set the bits[9:8]   as DSA mode port
	tem_reg |= PORT_FRAME_ETHDSAMODE;
	HAL_ETH_WritePHYRegister(&Eth_Handle,REG_PORT_CONTROL,tem_reg);
	/*After set the mode ,set the port eth type value*/
	HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_PORTE_TYPE,&tem_reg);
	tem_reg &= 0;
	tem_reg |= 0x1234;
	HAL_ETH_WritePHYRegister(&Eth_Handle,REG_PORTE_TYPE,tem_reg);	
    /*Add a static entry with the DA as MGMT frames into the ATU(MAC table)*/
	mac_entry.mac_entry_oper = LOAD_PURGE_ENTRY_FID;
#ifdef CPU_PORT_10
	mac_entry.mac_entry_data = 0x0e | (0x01 << 14) | 0x0000; //bit[15](trunk):'0'; bits[14:4]:port_vector; bits[3:0]:0x06 means the static entry as MGMT;	 
#endif
	for(loop= 0 ; loop<6 ; loop++)
	{
	  mac_entry.mac_entry_MAC[loop]  = MGMT_MAC[loop];
	}
	mac_entry.mac_entry_fid = 1;
	Load_Purge_ATUentry(&mac_entry);
#ifdef MY_DEBUG
	for(loop =0 ;loop< 6 ; loop++ )
	{		
	  mac_entry.mac_entry_MAC[loop] = GET_NEXT[loop] ;
	}

	Read_ATU_entry(&mac_entry);
	printf("The MAC read out,check if the MAC entry is load in:");
	for(loop=0 ;loop<6 ; loop++ )
	  {
		printf("%x \n\r",mac_entry.mac_entry_MAC[loop]);
	  }
	printf("The DATA read out: %x \n\r",mac_entry.mac_entry_data);
	printf("The OPER read out: %x \n\r",mac_entry.mac_entry_oper);
#endif
	/*Set the CPU dest PORT in REG:Global 1->offset 1A -> bits[7:4]*/
//	Eth_Handle.Init.PhyAddress = GLOBAL_1_ADDR;
//	HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_MONITOR_DESTINATIONS,&tem_reg);
//	tem_reg &= ~GLOBAL_CPU_DEST;                   //Clear the CPU DEST bits
//	tem_reg |= (0x000a << 4)&GLOBAL_CPU_DEST ;     //Set the CPU dest port bits,'0x0a' refers to PORT 10
//	HAL_ETH_WritePHYRegister(&Eth_Handle,REG_MONITOR_DESTINATIONS,tem_reg);
//	
//	/*Set the target device '0x00' for my single chip mode : Global 1 -> Offset 1C ->bits*/
//	HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_GLOBAL_CONTROL2,&tem_reg);
//	tem_reg &= GLOBAL_DEVICE_NUMBER;               //Set the bits[4:0] as 0x00 for my single chip mode
//	HAL_ETH_WritePHYRegister(&Eth_Handle,REG_MONITOR_DESTINATIONS,tem_reg);
	
	 /*Initilize the RSTP counter*/
	bpdu_count.RECIEVE_AVAILIABLE_BPDU   = 0x00000000;
	bpdu_count.RECIEVE_NONE_BPDU         = 0x00000000;
    bpdu_count.RECIEVE_UNAVAILIABLE_BPDU = 0x00000000;	
  }
	
/******************************************************************/
//Function :  uip_MAC_Init()
//Brief    : Initialize the MCU MAC for the running of uIP 
/******************************************************************/
void uip_MAC_Init(uint8_t *pMAC)
  {
	MAC_ENTRY_T  mac_entry;
	uint8_t      loop;
	uint16_t     tem_reg;
	uint8_t      circle;
	uint8_t      vlan[2]={1,2};
	MAC_ENTRY_T  entryx;
	MAC_API_T    entry_out;
	uint8_t      entry_counter;
/*Add a static entry with the DA as MGMT frames into the ATU(MAC table)*/
    mac_entry.mac_entry_oper = LOAD_PURGE_ENTRY_FID;
#ifdef CPU_PORT_10
    mac_entry.mac_entry_data = 0x0e | (0x01 << 14) | 0x0000; //bit[15](trunk):'0'; bits[14:4]:port_vector; bits[3:0]:0x06 means the static entry as MGMT;	 
#endif
   for(loop= 0 ; loop<6 ; loop++)
    {
	  mac_entry.mac_entry_MAC[loop]  = pMAC[loop];
	}
	mac_entry.mac_entry_fid = 1;
	Load_Purge_ATUentry(&mac_entry);
#ifdef MY_DEBUG
	printf("MAC:%02x.%02x.%02x.%02x.%02x.%02x\n\r",pMAC[0],pMAC[1],pMAC[2],pMAC[3],pMAC[4],pMAC[5]);
#endif
	//Delay(1000);
	//Flush_Move_ATUentry(ALL_NONSTA,FLUSH_ENTRY,0xfa);
	Flush_Move_ATUentry(ALL_NONSTA,FLUSH_ENTRY,0x00,0x00);
	//printf("uip_MAC init done\n\r");
/*Set the CPU dest PORT in REG:Global 1->offset 1A -> bits[7:4]*/
	Eth_Handle.Init.PhyAddress = GLOBAL_1_ADDR;
	HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_MONITOR_DESTINATIONS,&tem_reg);
	tem_reg &= ~GLOBAL_CPU_DEST;                   //Clear the CPU DEST bits
	tem_reg |= (0x000a << 4)&GLOBAL_CPU_DEST ;     //Set the CPU dest port bits,'0x0a' refers to PORT 10
	HAL_ETH_WritePHYRegister(&Eth_Handle,REG_MONITOR_DESTINATIONS,tem_reg);
	 
/*Set the target device '0x00' for my single chip mode : Global 1 -> Offset 1C ->bits*/
	HAL_ETH_ReadPHYRegister(&Eth_Handle,REG_GLOBAL_CONTROL2,&tem_reg);
	tem_reg &= GLOBAL_DEVICE_NUMBER;               //Set the bits[4:0] as 0x00 for my single chip mode
	HAL_ETH_WritePHYRegister(&Eth_Handle,REG_MONITOR_DESTINATIONS,tem_reg);
		
  /*Start the ETH TX and RX*/
	HAL_ETH_Start(&Eth_Handle);
#ifdef MY_DEBUG
	for(circle = 0;circle <2 ;circle++ )
	{
	  printf("The fid: %d\n\r",vlan[circle]);
	  for(loop = 0 ;loop < 6; loop++ )
		{
		  entryx.mac_entry_MAC[loop] = 0xff;          //MAC{0xff,0xff,0xff,0xff,0xff,0xff} used to get the first ATU entry
		}
	  while(Display_ALL_MACentry(&entry_out,&entryx,vlan[circle])) //Get All the MAC entry
		{
		  entry_counter++;
		}
	}
	
#endif
  }

/********************************************************************************/
//Function : Creat_desc_list()
//Creat the linklist for the descriptor
/********************************************************************************/
//@PS   : this function is unused
#if 0
ETH_DMADescTypeDef *Creat_desc_list()
{
	ETH_DMADescTypeDef *head;
	ETH_DMADescTypeDef *p;
	ETH_DMADescTypeDef *q;
	uint8_t            cnt;
	   
	q = head = (ETH_DMADescTypeDef *)malloc(sizeof(ETH_DMADescTypeDef));
	for( cnt = 0 ;cnt < DESCRIPTORLIST_SIZE -1 ;  cnt++ )
	{
	  p = (ETH_DMADescTypeDef *)malloc(sizeof(ETH_DMADescTypeDef));
	  p->Buffer2NextDescAddr = NULL;
	  if(cnt == DESCRIPTORLIST_SIZE - 2 )
		p->Buffer2NextDescAddr = (uint32_t)head;
	  q->Buffer2NextDescAddr = (uint32_t)p;
	  q = (ETH_DMADescTypeDef *)q->Buffer2NextDescAddr;
	}   
	return head;
}
#endif

/********************************************************************************/
//Function :  I2C_Configuration
//Set the TIMx to works in the right way
/********************************************************************************/
void I2C_Configuration(I2C_HandleTypeDef *hi2c)
  {
	I2C_InitTypeDef  I2C_Init;
	  
	I2C_Init.ClockSpeed      = 100000;
	I2C_Init.DutyCycle       = I2C_DUTYCYCLE_2;
	I2C_Init.AddressingMode  = I2C_ADDRESSINGMODE_7BIT;
	I2C_Init.DualAddressMode = I2C_DUALADDRESS_DISABLED;
	I2C_Init.NoStretchMode   = I2C_NOSTRETCH_DISABLED;
	I2C_Init.GeneralCallMode = I2C_GENERALCALL_DISABLED;
	I2C_Init.OwnAddress1     = I2C_ADDRESS;
	I2C_Init.OwnAddress2     = 0xFE;
  
	hi2c->Instance    = I2Cx;
	hi2c->Init        = I2C_Init;
	  
	if(HAL_I2C_Init(hi2c) != HAL_OK)
	{
	  printf("Oh!! what the fuck! The I2C initialization failed!");
	}
  }

/********************************************************************************/
//Function :  SDRAM_Configuration
//This contains the SDRAM's(GPIO\RCC)configuration
//PS : But the configuration function isn't used because the SDRAM are initialiazed in 
//the file system_stm32f4xx.c
/********************************************************************************/
/**
  * @brief  Perform the SDRAM exernal memory inialization sequence
  * @param  hsdram: SDRAM handle
  * @param  Command: Pointer to SDRAM command structure
  * @retval None
  */
static void BSP_SDRAM_Initialization_Sequence(SDRAM_HandleTypeDef *hsdram, FMC_SDRAM_CommandTypeDef *Command)
{
  __IO uint32_t tmpmrd =0;
  /* Step 3:  Configure a clock configuration enable command */
  Command->CommandMode = FMC_SDRAM_CMD_CLK_ENABLE;
  Command->CommandTarget = FMC_SDRAM_CMD_TARGET_BANK1;
  Command->AutoRefreshNumber = 1;
  Command->ModeRegisterDefinition = 0;

  /* Send the command */
  HAL_SDRAM_SendCommand(hsdram, Command, 0x1000);

  /* Step 4: Insert 100 us minimum delay */ 
  /* Inserted delay is equal to 1 ms due to systick time base unit (ms) */
  HAL_Delay(1);
    
  /* Step 5: Configure a PALL (precharge all) command */ 
  Command->CommandMode = FMC_SDRAM_CMD_PALL;
  Command->CommandTarget = FMC_SDRAM_CMD_TARGET_BANK1;
  Command->AutoRefreshNumber = 1;
  Command->ModeRegisterDefinition = 0;

  /* Send the command */
  HAL_SDRAM_SendCommand(hsdram, Command, 0x1000);  
  
  /* Step 6 : Configure a Auto-Refresh command */ 
  Command->CommandMode = FMC_SDRAM_CMD_AUTOREFRESH_MODE;
  Command->CommandTarget = FMC_SDRAM_CMD_TARGET_BANK1;
  Command->AutoRefreshNumber = 8;
  Command->ModeRegisterDefinition = 0;

  /* Send the command */
  HAL_SDRAM_SendCommand(hsdram, Command, 0x1000);
  
  /* Step 7: Program the external memory mode register */
  tmpmrd = (uint32_t)SDRAM_MODEREG_BURST_LENGTH_1          |
                     SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL   |
                     SDRAM_MODEREG_CAS_LATENCY_3           |
                     SDRAM_MODEREG_OPERATING_MODE_STANDARD |
                     SDRAM_MODEREG_WRITEBURST_MODE_SINGLE;
  
  Command->CommandMode = FMC_SDRAM_CMD_LOAD_MODE;
  Command->CommandTarget = FMC_SDRAM_CMD_TARGET_BANK1;
  Command->AutoRefreshNumber = 1;
  Command->ModeRegisterDefinition = tmpmrd;

  /* Send the command */
  HAL_SDRAM_SendCommand(hsdram, Command, 0x1000);
  
  /* Step 8: Set the refresh rate counter */
  /* (15.62 us x Freq) - 20 */
  /* Set the device refresh counter */
  hsdram->Instance->SDRTR |= ((uint32_t)((1292)<< 1));
}

/*The SDRAM_Configuration function*/
void SDRAM_Configuration(SDRAM_HandleTypeDef* hsdram)
{
  hsdram->Instance = FMC_SDRAM_DEVICE;
  
  SDRAM_Timing.LoadToActiveDelay    = 2;
  SDRAM_Timing.ExitSelfRefreshDelay = 6;
  SDRAM_Timing.SelfRefreshTime      = 4;
  SDRAM_Timing.RowCycleDelay        = 6;
  SDRAM_Timing.WriteRecoveryTime    = 2;
  SDRAM_Timing.RPDelay              = 2;
  SDRAM_Timing.RCDDelay             = 2;
  
  hsdram->Init.SDBank             = FMC_SDRAM_BANK1;
  hsdram->Init.ColumnBitsNumber   = FMC_SDRAM_COLUMN_BITS_NUM_8;
  hsdram->Init.RowBitsNumber      = FMC_SDRAM_ROW_BITS_NUM_12;
  hsdram->Init.MemoryDataWidth    = SDRAM_MEMORY_WIDTH;
  hsdram->Init.InternalBankNumber = FMC_SDRAM_INTERN_BANKS_NUM_4;
  hsdram->Init.CASLatency         = FMC_SDRAM_CAS_LATENCY_3;
  hsdram->Init.WriteProtection    = FMC_SDRAM_WRITE_PROTECTION_DISABLE;
  hsdram->Init.SDClockPeriod      = SDCLOCK_PERIOD;
  hsdram->Init.ReadBurst          = FMC_SDRAM_RBURST_ENABLE;
  hsdram->Init.ReadPipeDelay      = FMC_SDRAM_RPIPE_DELAY_0;
  
  /* Initialize the SDRAM controller */
  if(HAL_SDRAM_Init(hsdram, &SDRAM_Timing) != HAL_OK)
  {
    printf("Initialization Error!\n\r");
  }
  /* Program the SDRAM external device */
  BSP_SDRAM_Initialization_Sequence(hsdram, &SDRAM_Command);
}

/********************************************************************************/
//Function :  FLASH_Configuration
//Brief  : Initilize the flash that attached to the STM32F429
/********************************************************************************/
uint8_t FLASH_Configuration()
{
	/*Init the instance for FMC_BANK1*/
  norHandle.Instance  = FMC_NORSRAM_DEVICE;
  norHandle.Extended  = FMC_NORSRAM_EXTENDED_DEVICE;
  
  /*NOR device configuration*/   
  Timing.AddressSetupTime      = 15;
  Timing.AddressHoldTime       = 3;
  Timing.DataSetupTime         = 10;
  Timing.BusTurnAroundDuration = 1;
  Timing.CLKDivision           = 1;
  Timing.DataLatency           = 0;
  Timing.AccessMode            = FMC_ACCESS_MODE_A;
  
  norHandle.Init.NSBank             = FMC_NORSRAM_BANK1;
  norHandle.Init.DataAddressMux     = FMC_DATA_ADDRESS_MUX_DISABLE;
  norHandle.Init.MemoryType         = FMC_MEMORY_TYPE_NOR;
  norHandle.Init.MemoryDataWidth    = NOR_MEMORY_WIDTH;
  norHandle.Init.BurstAccessMode    = NOR_BURSTACCESS;
  norHandle.Init.WriteOperation     = FMC_WRITE_OPERATION_ENABLE;
  norHandle.Init.WaitSignal         = FMC_WAIT_SIGNAL_DISABLE;
  norHandle.Init.ExtendedMode       = FMC_EXTENDED_MODE_DISABLE;
  norHandle.Init.AsynchronousWait   = FMC_ASYNCHRONOUS_WAIT_DISABLE;
  norHandle.Init.WriteBurst         = NOR_WRITEBURST;
  norHandle.Init.ContinuousClock    = CONTINUOUSCLOCK_FEATURE;

//  Timing.AddressSetupTime      = 4;
//  Timing.AddressHoldTime       = 3;
//  Timing.DataSetupTime         = 24;
//  Timing.BusTurnAroundDuration = 1;
//  Timing.CLKDivision           = 2;
//  Timing.DataLatency           = 2;
//  Timing.AccessMode            = FMC_ACCESS_MODE_A;
//  
//  norHandle.Init.NSBank             = FMC_NORSRAM_BANK1;
//  norHandle.Init.DataAddressMux     = FMC_DATA_ADDRESS_MUX_DISABLE;
//  norHandle.Init.MemoryType         = FMC_MEMORY_TYPE_NOR;
//  norHandle.Init.MemoryDataWidth    = NOR_MEMORY_WIDTH;
//  norHandle.Init.BurstAccessMode    = NOR_BURSTACCESS;
//  norHandle.Init.WaitSignalPolarity = FMC_WAIT_SIGNAL_POLARITY_LOW;
//  norHandle.Init.WrapMode           = FMC_WRAP_MODE_DISABLE;
//  norHandle.Init.WaitSignalActive   = FMC_WAIT_TIMING_BEFORE_WS;
//  norHandle.Init.WriteOperation     = FMC_WRITE_OPERATION_ENABLE;
//  norHandle.Init.WaitSignal         = FMC_WAIT_SIGNAL_ENABLE;
//  norHandle.Init.ExtendedMode       = FMC_EXTENDED_MODE_DISABLE;
//  norHandle.Init.AsynchronousWait   = FMC_ASYNCHRONOUS_WAIT_ENABLE;
//  norHandle.Init.WriteBurst         = NOR_WRITEBURST;
//  norHandle.Init.ContinuousClock    = CONTINUOUSCLOCK_FEATURE;

//    norHandle.Instance->BTCR[0] = 0x000021d8;
//		norHandle.Instance->BTCR[1] = 0x04200000;
//	  norHandle.Instance->BTCR[0] |= 0x00000001;
    return HAL_NOR_Init(&norHandle, &Timing, &Timing);
}


/********************************************************************************/
//Function  : Modify_the_MAC()
//Brief     : Modify the MAC of my set
//Parameter : pMAC(Input) :point to a MAC array
/********************************************************************************/
#if 1
uint8_t Modify_the_MAC(INVARIATE_T *invarite,uint8_t *pMAC)
  {
	uint8_t loop;
	uint8_t i;
	uint8_t tem_MAC[30];
	uint8_t tem;
	MAC_ENTRY_T MAC_entry;
	/*For the input MAC address are all chars,Here must transmit them*/
	i=0;
	for(loop=7; loop<24; loop++)   /*The format of the command is 7~24 bit*/
	  {
		if(pMAC[loop] >= 48 && pMAC[loop] <= 57)
		  {
			tem_MAC[i++]=pMAC[loop] - 48;
		  }
		else 
		  if(pMAC[loop] >= 65 && pMAC[loop] <= 70)
			{
			  tem_MAC[i++] = pMAC[loop] - 65 +10 ;
			}
		else
		  if(pMAC[loop]>= 97 && pMAC[loop]<= 102)
			{
			  tem_MAC[i++] = pMAC[loop] - 97 + 10;
			}
		else
		  if(pMAC[loop] == 32 || pMAC[loop] == '.')
			{ }
		else
		  {
			//printf("The MAC input error 01! Please check!\n\r");
			//printf("%x\n\r",loop);
			return 0; 
		  }
	  }/*out of the for circle*/
	/*Judge if the MAC is compatiable*/
	if(i> 12)
	  {
		return 0;
	  }
	/*The MAC address need to be updated,and before this,purge the static MAC entry for my CPU port*/			
	MAC_entry.mac_entry_oper = LOAD_PURGE_ENTRY_FID;
#ifdef CPU_PORT_10
	MAC_entry.mac_entry_data = 0x00 | (0x01 << 14) | 0x0000;      //bit[15](trunk):'0'; bits[14:4]:port_vector; bits[3:0]:0x07 means the static entry ;	 
#endif
	for(loop= 0 ; loop<6 ; loop++)
	{
	MAC_entry.mac_entry_MAC[loop]  = invarite->The_MAC[loop];
	}
	MAC_entry.mac_entry_fid = 1;
	Load_Purge_ATUentry(&MAC_entry);
	/*Compatiable! Then get the MAC in the tem register*/
	i=0;
	for(loop = 0;loop<12;loop++)
	  {
		if(loop%2 == 0)
		  {
			tem = tem_MAC[loop];
		  }
		else
		  {
			invarite->The_MAC[i++] = (tem<<4) | (tem_MAC[loop] &0x0f);
		  }
	  }
/*After modified,the MAC need to be added as a static entry for my CPU port*/
	MAC_entry.mac_entry_oper = LOAD_PURGE_ENTRY_FID;
#ifdef CPU_PORT_10
	MAC_entry.mac_entry_data = 0x0e | (0x01 << 14) | 0x0000;      //bit[15](trunk):'0'; bits[14:4]:port_vector; bits[3:0]:0x07 means the static entry ;	 
#endif
	for(loop= 0 ; loop<6 ; loop++)
	{
	  MAC_entry.mac_entry_MAC[loop]  = invarite->The_MAC[loop];
	}
	MAC_entry.mac_entry_fid = 1;
	Load_Purge_ATUentry(&MAC_entry);
//	Set_uip_MAC(KEY_MAC);
	return updateNewConfig();
  }
#endif
/********************************************************************************/
//Function  : Display_the_MAC()
//Brief     : Display the MAC of my set
//Parameter : pMAC(Input) :point to a MAC array
/********************************************************************************/
void Display_the_MAC(uint8_t *pMAC)
  {
	printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n\r\n\r");
	printf("MAC address:  %02x.%02x.%02x.%02x.%02x.%02x\n\r\n\r",pMAC[0],pMAC[1],pMAC[2],pMAC[3],pMAC[4],pMAC[5]);
	printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n\r\n\r");	
  }
	
/*******************************************************************/
//函数名   : getSetType(VETH_TYPE_T *type)
//功能     : 获取设备基本信息
//参数     ：VETH_TYPE_T结构体指针type 用来存放获取到的该设备的基本信息
//返回值   ：None
/*******************************************************************/
void getSetType(VETH_TYPE_T *type)
  {
	uint16_t  tem_reg;
	uint8_t   fiber_port = 0;
	uint8_t   loop;
/*获取每个端口的光口Media还是电口Media*/		
	for(loop = 0 ;loop < 8 ;loop++ )
	  { 
		READ_PHYREGISTER(SWITCH_PHY_0+loop,REG_PHY_SPECIFIC_CONTROL2,&tem_reg);
		if((tem_reg & PHY_SOFTMEDIA_SELECT) != RESET)
		  {fiber_port++;}
	  }
/*获取设备的基本型号信息*/
	if(!Str_cmp(g_set_name,VETH3000_4S,12)){
	   type->max_portNo=10;
	   Str_cpy(type->set_type,g_set_type3,14);
	}else{
		if(fiber_port == 2)
		  {
			type->max_portNo = 8;
			Str_cpy(type->set_type,g_set_type1,6);	 
		  }else{  
			type->max_portNo = 10;
			Str_cpy(type->set_type,g_set_type2,6);
		  }
		
		Str_cpy(type->set_name,g_set_name,8);
		Str_cpy(type->set_version,g_set_version,4);
	}
  }
	
