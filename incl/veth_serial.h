/**
**************************************************************************
@File    :  hal\incl\veth_serial.h
@Author  :  Duke Lee
@Verdion :  V1.0
@Date    :  14-Jan-2015
@brief   :  Header file for the veth_serial.c 
**************************************************************************
**/

#ifndef  __VETH_SERIAL_H__
#define  __VETH_SERIAL_H__
#include "stm32f4xx.h"
#include "stm32f4xx_hal_conf.h"



/* Definition for USART6's DMA and NVIC*/
#define USART6_RX_DMA_STREAM  DMA2_Stream1
#define USART6_RX_DMA_CHANNEL DMA_CHANNEL_5  
#define USART6_TX_DMA_STREAM  DMA2_Stream6
#define USART6_TX_DMA_CHANNEL DMA_CHANNEL_5
#define USART6_DMA_TX_IRQn    DMA2_Stream6_IRQn
#define USART6_DMA_RX_IRQn    DMA2_Stream1_IRQn

/* Definition for UART5's DMA and NVIC*/
#define UART5_RX_DMA_STREAM   DMA1_Stream0
#define UART5_RX_DMA_CHANNEL  DMA_CHANNEL_4  
#define UART5_TX_DMA_STREAM   DMA1_Stream7
#define UART5_TX_DMA_CHANNEL  DMA_CHANNEL_4
#define UART5_DMA_TX_IRQn     DMA1_Stream7_IRQn
#define UART5_DMA_RX_IRQn     DMA1_Stream0_IRQn

/* Definition for UART7's DMA and NVIC*/
#define UART7_RX_DMA_STREAM   DMA1_Stream3
#define UART7_RX_DMA_CHANNEL  DMA_CHANNEL_5  
#define UART7_TX_DMA_STREAM   DMA1_Stream1
#define UART7_TX_DMA_CHANNEL  DMA_CHANNEL_5
#define UART7_DMA_TX_IRQn     DMA1_Stream1_IRQn
#define UART7_DMA_RX_IRQn     DMA1_Stream3_IRQn

/* Definition for UART4's DMA and NVIC*/
#define UART4_RX_DMA_STREAM   DMA1_Stream2
#define UART4_RX_DMA_CHANNEL  DMA_CHANNEL_4  
#define UART4_TX_DMA_STREAM   DMA1_Stream4
#define UART4_TX_DMA_CHANNEL  DMA_CHANNEL_4
#define UART4_DMA_TX_IRQn     DMA1_Stream4_IRQn
#define UART4_DMA_RX_IRQn     DMA1_Stream2_IRQn

#define _UART_ENABLE_RECIEVE_(_INSTANCE__)   ((_INSTANCE__)->Instance->CR1) |= USART_CR1_RE
#define _UART_WAIT_TC_FLAG__(_INSTANCE__)    do{     \
                                               }while( (_INSTANCE__->Instance->SR & USART_SR_TC) != USART_SR_TC )
#define UART_DMA_DISABLE   (uint32_t)0xfffffffe
#define UART_DMA_ENABLE    (uint32_t)0x00000001
/**
@ref current_mem
**/
#define MEM_0   0
#define MEM_1   1
/**
@ref mem_status
**/
#define MEM_NOT_EMPTY  2
#define MEM_FULL  1
#define MEM_EMPTY 0

/**
@ref mem_status
**/
#define RCV_MEM_FULL      1
#define RCV_MEM_EMPTY     0
#define RCV_MEM_NOT_EMPTY 2


void testSerial(void);
void serialDmaInit(void);
void serialServer(void);
void serialClient(void);
void com1DmaInit(UART_HandleTypeDef *huart);
void com2DmaInit(UART_HandleTypeDef *huart);
void com3DmaInit(UART_HandleTypeDef *huart);
void com4DmaInit(UART_HandleTypeDef *huart);
void setComPortDirect(uint8_t serialNo);
void resetComPortDirect(uint8_t serialNo);
HAL_StatusTypeDef uartTransfer(UART_HandleTypeDef *huart);
#ifdef MY_DEBUG_P
HAL_StatusTypeDef halUartTransmitDma(UART_HandleTypeDef *huart);
#else
HAL_StatusTypeDef halUartTransmitDma(UART_HandleTypeDef *huart,uint8_t *the_buffer,uint32_t length);
#endif
HAL_StatusTypeDef halUartTransmitDmaLittle(UART_HandleTypeDef *huart);
#endif 
