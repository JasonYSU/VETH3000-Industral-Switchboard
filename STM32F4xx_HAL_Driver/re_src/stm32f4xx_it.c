/**
  ******************************************************************************
  * @file    STM32F4xx_HAL_Driver\re_src\stm32f4xx_it.c 
  * @author  Duke Lee
  * @version V1.0.0
  * @date    10-June-2014
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
 
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_conf.h"
#include "veth_common.h"   

/** @addtogroup STM32F4xx_HAL_Examples
  * @{
  */


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

#define USARTx_IRQHandler USART6_IRQHandler
#define USARTy_IRQHandler USART1_IRQHandler
#define USARTz_IRQHandler UART5_IRQHandler
#define USARTa_IRQHandler UART7_IRQHandler
#define USARTb_IRQHandler UART4_IRQHandler

#define TIMx_IRQHandler   TIM2_IRQHandler
#define TIMs_IRQHandler   TIM5_IRQHandler 
#define TIMr_IRQHandler   TIM3_IRQHandler

#define USART6_DMA_RX_IRQHandler	  DMA2_Stream1_IRQHandler
#define USART6_DMA_TX_IRQHandler	  DMA2_Stream6_IRQHandler
#define UART5_DMA_TX_IRQHandler		  DMA1_Stream7_IRQHandler
#define UART5_DMA_RX_IRQHandler		  DMA1_Stream0_IRQHandler
#define UART7_DMA_TX_IRQHandler		  DMA1_Stream1_IRQHandler
#define UART7_DMA_RX_IRQHandler		  DMA1_Stream3_IRQHandler
#define UART4_DMA_TX_IRQHandler		  DMA1_Stream4_IRQHandler
#define UART4_DMA_RX_IRQHandler		  DMA1_Stream2_IRQHandler
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
  HAL_IncTick();
}


/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f4xx.s).                                               */
/******************************************************************************/

/**
  * @brief  This function handles TIM interrupt request.
  * @param  None
  * @retval None
  */
void TIMx_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&TimHandle);
}
/**
@ref TIMs 
**/
void TIMs_IRQHandler(void)
{
	 HAL_TIM_IRQHandler(&TimsHandle);
}
/**
@ref TIMr
**/
void TIMr_IRQHandler(void)
{
	 HAL_TIM_IRQHandler(&TimrHandle);
}

/**
  * @brief  This function handles UART interrupt request.
  * @param  None
  * @retval None
  */
/*USART6 interrupt handler*/
void USARTx_IRQHandler(void)
{
  HAL_UART_IRQHandler(&Uart6Handle);
}
/*USART1 interrupt handler*/
void USARTy_IRQHandler(void)
{
  HAL_UART_IRQHandler(&Uart1Handle);
}
/*UART5 interrupt handler*/
void USARTz_IRQHandler(void)
{
  HAL_UART_IRQHandler(&Uart5Handle);
}
/*UART7 interrupt handler*/
void USARTa_IRQHandler(void)
{
  HAL_UART_IRQHandler(&Uart7Handle);
}
/*UART4 interrupt handler*/
void USARTb_IRQHandler(void)
{
  HAL_UART_IRQHandler(&Uart4Handle);
}

/*I2C1 interrupt handler*/
void  I2C1_EV_Handler(void)
{  
   HAL_I2C_EV_IRQHandler(&I2C1Handle);
}
/*I2C1 error handler*/
void  I2C1_ER_Handler(void)
{  
   HAL_I2C_ER_IRQHandler(&I2C1Handle);
}

/**
@brief  This function handles DMA interrupt request.    
**/
void USART6_DMA_RX_IRQHandler(void)
{
  HAL_DMA_Rx_IRQHandler(Uart6Handle.hdmarx);
}

/**
@brief  This function handles DMA interrupt request.
**/
void USART6_DMA_TX_IRQHandler(void)
{
  HAL_DMA_IRQHandler(Uart6Handle.hdmatx);
}


/**
@brief  This function handles DMA interrupt request.    
**/
void UART5_DMA_RX_IRQHandler(void)
{
  HAL_DMA_Rx_IRQHandler(Uart5Handle.hdmarx);
}

/**
@brief  This function handles DMA interrupt request.
**/
void UART5_DMA_TX_IRQHandler(void)
{
  HAL_DMA_IRQHandler(Uart5Handle.hdmatx);
}


/**
@brief  This function handles DMA interrupt request.    
**/
void UART7_DMA_RX_IRQHandler(void)
{
  HAL_DMA_Rx_IRQHandler(Uart7Handle.hdmarx);
}

/**
@brief  This function handles DMA interrupt request.
**/
void UART7_DMA_TX_IRQHandler(void)
{
  HAL_DMA_IRQHandler(Uart7Handle.hdmatx);
}

/**
@brief  This function handles DMA interrupt request.    
**/
void UART4_DMA_RX_IRQHandler(void)
{
  HAL_DMA_Rx_IRQHandler(Uart4Handle.hdmarx);
}

/**
@brief  This function handles DMA interrupt request.
**/
void UART4_DMA_TX_IRQHandler(void)
{
  HAL_DMA_IRQHandler(Uart4Handle.hdmatx);
}
#if 0
#endif
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
