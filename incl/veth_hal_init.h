/**
****************************************************************************************
@File    : D:\ProgramFiles\KEIL\hal\incl\veth_hal_init.h
@Author  : Duke Lee
@Date    : 06-August-2014
@Version : V1.0.0
@Breif   : The header file for veth_hal_init.c
		       
****************************************************************************************
**/
#ifndef __SYSTEM_HAL_INIT_H_
#define __SYSTEM_HAL_INIT_H_

#include "stm32f4xx_hal_conf.h"
#include "veth_com_uart.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
#define SDRAM_BANK_ADDR                 ((uint32_t)0xC0000000)

/* #define SDRAM_MEMORY_WIDTH            FMC_SDRAM_MEM_BUS_WIDTH_8  */
/* #define SDRAM_MEMORY_WIDTH            FMC_SDRAM_MEM_BUS_WIDTH_16 */
#define SDRAM_MEMORY_WIDTH               FMC_SDRAM_MEM_BUS_WIDTH_32

#define SDCLOCK_PERIOD                   FMC_SDRAM_CLOCK_PERIOD_2
/* #define SDCLOCK_PERIOD                FMC_SDRAM_CLOCK_PERIOD_3 */

#define SDRAM_TIMEOUT     ((uint32_t)0xFFFF) 

#define SDRAM_MODEREG_BURST_LENGTH_1             ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_LENGTH_2             ((uint16_t)0x0001)
#define SDRAM_MODEREG_BURST_LENGTH_4             ((uint16_t)0x0002)
#define SDRAM_MODEREG_BURST_LENGTH_8             ((uint16_t)0x0004)
#define SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL      ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_TYPE_INTERLEAVED     ((uint16_t)0x0008)
#define SDRAM_MODEREG_CAS_LATENCY_2              ((uint16_t)0x0020)
#define SDRAM_MODEREG_CAS_LATENCY_3              ((uint16_t)0x0030)
#define SDRAM_MODEREG_OPERATING_MODE_STANDARD    ((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_PROGRAMMED ((uint16_t)0x0000) 
#define SDRAM_MODEREG_WRITEBURST_MODE_SINGLE     ((uint16_t)0x0200) 

/** 
  *  NOR status structure definition  
  */     
#define   NOR_OK         0x00
#define   NOR_ERROR      0x01

#define NOR_DEVICE_ADDR  ((uint32_t)0x60020000)  
  
/* #define NOR_MEMORY_WIDTH    FMC_NORSRAM_MEM_BUS_WIDTH_8  */
#define NOR_MEMORY_WIDTH    FMC_NORSRAM_MEM_BUS_WIDTH_16

#define NOR_BURSTACCESS    FMC_BURST_ACCESS_MODE_DISABLE  
//#define NOR_BURSTACCESS    FMC_BURST_ACCESS_MODE_ENABLE
  
//#define NOR_WRITEBURST    FMC_WRITE_BURST_DISABLE  
#define NOR_WRITEBURST   FMC_WRITE_BURST_ENABLE
 
#define CONTINUOUSCLOCK_FEATURE    FMC_CONTINUOUS_CLOCK_SYNC_ONLY 
//#define CONTINUOUSCLOCK_FEATURE     FMC_CONTINUOUS_CLOCK_SYNC_ASYNC

/*Statements of the functions*/
void RCC_Configuration(void);
void NVIC_Configuration(void);
void GPIO_Configuration(void);
void USART_Configuration(UART_HandleTypeDef*huart,UART_T* puart);
void TIMx_Configuration(void);
uint32_t Get_ms_Count(void);//Get the real ms timer 
void ETH_Configuration(ETH_HandleTypeDef *hal_eth,uint8_t *pMAC);
void Variate_init(void);
void I2C_Configuration(I2C_HandleTypeDef *hi2c);
void MII_Init(void);
void RSTP_Init(void);
void uip_MAC_Init(uint8_t *pMAC);
void SDRAM_Configuration(SDRAM_HandleTypeDef* hsdram);
uint8_t FLASH_Configuration(void);
uint8_t Modify_the_MAC(INVARIATE_T *invarite,uint8_t *pMAC);
void Display_the_MAC(uint8_t *pMAC);
void getSetType(VETH_TYPE_T *type);

/*These two are used to other forced operations*/
void forceLinkDown(int portNo);
void forceLinkUp(int portNo);
void cancleForceLink(int portNo);
#endif




