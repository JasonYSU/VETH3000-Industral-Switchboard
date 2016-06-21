/**
  ******************************************************************************
  * @file    stm32f4xx_hal_dma_ex.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    18-February-2014
  * @brief   DMA Extension HAL module driver
  *         This file provides firmware functions to manage the following 
  *         functionalities of the DMA Extension peripheral:
  *           + Extended features functions
  *
  @verbatim
  ==============================================================================
                        ##### How to use this driver #####
  ==============================================================================
  [..]
  The DMA Extension HAL driver can be used as follows:
   (#) Start a multi buffer transfer using the HAL_DMA_MultiBufferStart() function
       for polling mode or HAL_DMA_MultiBufferStart_IT() for interrupt mode.
                   
     -@-  In Memory-to-Memory transfer mode, Multi (Double) Buffer mode is not allowed.
     -@-  When Multi (Double) Buffer mode is enabled the, transfer is circular by default.
     -@-  In Multi (Double) buffer mode, it is possible to update the base address for 
          the AHB memory port on-the-fly (DMA_SxM0AR or DMA_SxM1AR) when the stream is enabled. 
  
  @endverbatim
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2014 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "stm32f4xx_hal.h"
#include "veth_common.h"
/** @addtogroup STM32F4xx_HAL_Driver
  * @{
  */

/** @defgroup DMAEx 
  * @brief DMA Extended HAL module driver
  * @{
  */

#ifdef HAL_DMA_MODULE_ENABLED

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void DMA_MultiBufferSetConfig(DMA_HandleTypeDef *hdma, uint32_t SrcAddress, uint32_t DstAddress, uint32_t DataLength);

/* Private functions ---------------------------------------------------------*/

/** @defgroup DMAEx_Private_Functions
  * @{
  */


/** @defgroup DMAEx_Group1 Extended features functions 
 *  @brief   Extended features functions   
 *
@verbatim   
 ===============================================================================
                #####  Extended features functions  #####
 ===============================================================================  
    [..]  This section provides functions allowing to:
      (+) Configure the source, destination address and data length and 
          Start MultiBuffer DMA transfer
      (+) Configure the source, destination address and data length and 
          Start MultiBuffer DMA transfer with interrupt
      (+) Change on the fly the memory0 or memory1 address.
      
@endverbatim
  * @{
  */


/**
  * @brief  Starts the multi_buffer DMA Transfer.
  * @param  hdma      : pointer to a DMA_HandleTypeDef structure that contains
  *                     the configuration information for the specified DMA Stream.  
  * @param  SrcAddress: The source memory Buffer address
  * @param  DstAddress: The destination memory Buffer address
  * @param  SecondMemAddress: The second memory Buffer address in case of multi buffer Transfer  
  * @param  DataLength: The length of data to be transferred from source to destination
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_DMAEx_MultiBufferStart(DMA_HandleTypeDef *hdma, uint32_t SrcAddress, uint32_t DstAddress, uint32_t SecondMemAddress, uint32_t DataLength)
{
  /* Process Locked */
  __HAL_LOCK(hdma);

  /* Current memory buffer used is Memory 0 */
  if((hdma->Instance->CR & DMA_SxCR_CT) == 0)
  {
    hdma->State = HAL_DMA_STATE_BUSY_MEM0;
  }
  /* Current memory buffer used is Memory 1 */
  else if((hdma->Instance->CR & DMA_SxCR_CT) != 0)
  {
    hdma->State = HAL_DMA_STATE_BUSY_MEM1;
  }

   /* Check the parameters */
  assert_param(IS_DMA_BUFFER_SIZE(DataLength));

  /* Disable the peripheral */
  __HAL_DMA_DISABLE(hdma);  

  /* Enable the double buffer mode */
  hdma->Instance->CR |= (uint32_t)DMA_SxCR_DBM;

  /* Configure DMA Stream destination address */
  hdma->Instance->M1AR = SecondMemAddress;

  /* Configure the source, destination address and the data length */
  DMA_MultiBufferSetConfig(hdma, SrcAddress, DstAddress, DataLength);

  /* Enable the peripheral */
  __HAL_DMA_ENABLE(hdma);

  return HAL_OK;
}

/**
  * @brief  Starts the multi_buffer DMA Transfer with interrupt enabled.
  * @param  hdma:       pointer to a DMA_HandleTypeDef structure that contains
  *                     the configuration information for the specified DMA Stream.  
  * @param  SrcAddress: The source memory Buffer address
  * @param  DstAddress: The destination memory Buffer address
  * @param  SecondMemAddress: The second memory Buffer address in case of multi buffer Transfer  
  * @param  DataLength: The length of data to be transferred from source to destination
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_DMAEx_MultiBufferStart_IT(DMA_HandleTypeDef *hdma, uint32_t SrcAddress, uint32_t DstAddress, uint32_t SecondMemAddress, uint32_t DataLength)
{
  //uint32_t temp;
  UART_HandleTypeDef* huart = ( UART_HandleTypeDef* )((DMA_HandleTypeDef* )hdma)->Parent;
  /* Process Locked */
  //__HAL_LOCK(hdma);

  /* Current memory buffer used is Memory 0 */
  if((hdma->Instance->CR & DMA_SxCR_CT) == 0)
  {
    hdma->State = HAL_DMA_STATE_BUSY_MEM0;
  }
  /* Current memory buffer used is Memory 1 */
  else if((hdma->Instance->CR & DMA_SxCR_CT) != 0)
  {
    hdma->State = HAL_DMA_STATE_BUSY_MEM1;
  }
#ifdef MY_DEBUG
	  printf("  Instance->FCR=%x\n\r",hdma->Instance->FCR);
	  printf("  Instance->M0AR=%x\n\r",hdma->Instance->M0AR);
	  printf("  Instance->NDTR=%x\n\r",hdma->Instance->NDTR);
	  printf("  Instance->PAR=%x\n\r",hdma->Instance->PAR);
	  printf("  DMA->LISR:%x DMA2->HISR:%x\n\r",DMA2->LISR,DMA2->HISR);
#endif
  /* Check the parameters */
  assert_param(IS_DMA_BUFFER_SIZE(DataLength));

  /* Disable the peripheral */
  __HAL_DMA_DISABLE(hdma);
//  printf("Double Rcv in\n\r");
  if(0x00000001 == (hdma->Instance->CR & 0x00000001) )
    printf("The dma EN bit still enable,and may fail\n\r");
  /* Enable the Double buffer mode */
  hdma->Instance->CR |= (uint32_t)DMA_SxCR_DBM;
  /* Configure DMA Stream destination address */
  hdma->Instance->M1AR = SecondMemAddress;
  /* Configure the source, destination address and the data length */
  DMA_MultiBufferSetConfig(hdma, SrcAddress, DstAddress, DataLength); 
  /* Enable the transfer complete interrupt */
  __HAL_DMA_ENABLE_IT(hdma, DMA_IT_TC);

  /* Enable the Half transfer interrupt */
  //__HAL_DMA_ENABLE_IT(hdma, DMA_IT_HT);

  /* Enable the transfer Error interrupt */
  __HAL_DMA_ENABLE_IT(hdma, DMA_IT_TE);

  /* Enable the fifo Error interrupt */
  __HAL_DMA_ENABLE_IT(hdma, DMA_IT_FE);  

  /* Enable the direct mode Error interrupt */
  __HAL_DMA_ENABLE_IT(hdma, DMA_IT_DME); 
  
#ifdef MY_DEBUG
	  printf("  Instance->CR=%x\n\r",hdma->Instance->CR);
	  printf("  Instance->FCR=%x\n\r",hdma->Instance->FCR);
	  printf("  Instance->M0AR=%x\n\r",hdma->Instance->M0AR);
	  printf("  Instance->NDTR=%x\n\r",hdma->Instance->NDTR);
	  printf("  Instance->PAR=%x\n\r",hdma->Instance->PAR);
	  //printf("  DMA->LISR:%x DMA2->HISR:%x\n\r",DMA2->LISR,DMA2->HISR);
	  //printf("test_HaHa1 =%d\n\r",test_HaHa1);
	  //printf("fifo_error_counter=%d\n\r",fifo_error_counter);
	  //printf("transfer_error_counter=%d\n\r",transfer_error_counter);
#endif
  __HAL_DMA_ENABLE(hdma); 
  return HAL_OK; 
}

HAL_StatusTypeDef HAL_DMAeRx_MultiBufferStart_IT(DMA_HandleTypeDef *hdma, uint32_t SrcAddress, uint32_t DstAddress, uint32_t SecondMemAddress, uint32_t DataLength)
{
  //uint32_t temp;
  UART_HandleTypeDef* huart = ( UART_HandleTypeDef* )((DMA_HandleTypeDef* )hdma)->Parent;
  /* Process Locked */
  __HAL_LOCK(hdma);

  /* Current memory buffer used is Memory 0 */
  if((hdma->Instance->CR & DMA_SxCR_CT) == 0)
  {
    hdma->State = HAL_DMA_STATE_BUSY_MEM0;
  }
  /* Current memory buffer used is Memory 1 */
  else if((hdma->Instance->CR & DMA_SxCR_CT) != 0)
  {
    hdma->State = HAL_DMA_STATE_BUSY_MEM1;
  }
#ifdef MY_DEBUG
	  printf("  Instance->FCR=%x\n\r",hdma->Instance->FCR);
	  printf("  Instance->M0AR=%x\n\r",hdma->Instance->M0AR);
	  printf("  Instance->NDTR=%x\n\r",hdma->Instance->NDTR);
	  printf("  Instance->PAR=%x\n\r",hdma->Instance->PAR);
	  printf("  DMA->LISR:%x DMA2->HISR:%x\n\r",DMA2->LISR,DMA2->HISR);
#endif
  /* Check the parameters */
  assert_param(IS_DMA_BUFFER_SIZE(DataLength));

  /* Disable the peripheral */
  __HAL_DMA_DISABLE(hdma);
  //printf("Double Rcv in\n\r");
  /* Enable the Double buffer mode */
  hdma->Instance->CR |= (uint32_t)DMA_SxCR_DBM;
  /* Configure DMA Stream destination address */
  hdma->Instance->M1AR = SecondMemAddress;
  /* Configure the source, destination address and the data length */
  DMA_MultiBufferSetConfig(hdma, SrcAddress, DstAddress, DataLength); 
  /* Enable the transfer complete interrupt */
  __HAL_DMA_ENABLE_IT(hdma, DMA_IT_TC);

  /* Enable the Half transfer interrupt */
  //__HAL_DMA_ENABLE_IT(hdma, DMA_IT_HT);

  /* Enable the transfer Error interrupt */
  __HAL_DMA_ENABLE_IT(hdma, DMA_IT_TE);

  /* Enable the fifo Error interrupt */
  __HAL_DMA_ENABLE_IT(hdma, DMA_IT_FE);  

  /* Enable the direct mode Error interrupt */
  __HAL_DMA_ENABLE_IT(hdma, DMA_IT_DME); 
  
#ifdef MY_DEBUG
	  printf("  Instance->CR=%x\n\r",hdma->Instance->CR);
	  printf("  Instance->FCR=%x\n\r",hdma->Instance->FCR);
	  printf("  Instance->M0AR=%x\n\r",hdma->Instance->M0AR);
	  printf("  Instance->NDTR=%x\n\r",hdma->Instance->NDTR);
	  printf("  Instance->PAR=%x\n\r",hdma->Instance->PAR);
	  printf("  DMA->LISR:%x DMA2->HISR:%x\n\r",DMA2->LISR,DMA2->HISR);
	  printf("test_HaHa1 =%d\n\r",test_HaHa1);
	  printf("fifo_error_counter=%d\n\r",fifo_error_counter);
	  printf("transfer_error_counter=%d\n\r",transfer_error_counter);
#endif
  __HAL_DMA_ENABLE(hdma); 
  return HAL_OK; 
}
/**
  * @brief  Change the memory0 or memory1 address on the fly.
  * @param  hdma:       pointer to a DMA_HandleTypeDef structure that contains
  *                     the configuration information for the specified DMA Stream.  
  * @param  Address:    The new address
  * @param  memory:     the memory to be changed, This parameter can be one of 
  *                     the following values:
  *                      @arg MEMORY0
  *                      @arg MEMORY1
  * @note   The MEMORY0 address can be changed only when the current transfer use
  *         MEMORY1 and the MEMORY1 address can be changed only when the current 
  *         transfer use MEMORY0.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_DMAEx_ChangeMemory(DMA_HandleTypeDef *hdma, uint32_t Address, HAL_DMA_MemoryTypeDef memory)
{
  if(memory == MEMORY0)
  {
    /* change the memory0 address */
    hdma->Instance->M0AR = Address;
  }
  else
  {
    /* change the memory1 address */
    hdma->Instance->M1AR = Address;
  }

  return HAL_OK;
}

/**
  * @}
  */

/**
  * @brief  Set the DMA Transfer parameter.
  * @param  hdma:       pointer to a DMA_HandleTypeDef structure that contains
  *                     the configuration information for the specified DMA Stream.  
  * @param  SrcAddress: The source memory Buffer address
  * @param  DstAddress: The destination memory Buffer address
  * @param  DataLength: The length of data to be transferred from source to destination
  * @retval HAL status
  */
static void DMA_MultiBufferSetConfig(DMA_HandleTypeDef *hdma, uint32_t SrcAddress, uint32_t DstAddress, uint32_t DataLength)
{  
  /* Configure DMA Stream data length */
  hdma->Instance->CR &= 0xfffffffe;
  hdma->Instance->NDTR = DataLength;
//  hdma->Instance->CR &= (~(uint32_t)0x00080000);
  /* Peripheral to Memory */
  if((hdma->Init.Direction) == DMA_MEMORY_TO_PERIPH)
  {   
    /* Configure DMA Stream destination address */
    hdma->Instance->PAR = DstAddress;
    
    /* Configure DMA Stream source address */
    hdma->Instance->M0AR = SrcAddress;
  }
  /* Memory to Peripheral */
  else
  {
    /* Configure DMA Stream source address */
    hdma->Instance->PAR = SrcAddress;
    
    /* Configure DMA Stream destination address */
    hdma->Instance->M0AR = DstAddress;
  }
}

/**
  * @}
  */

#endif /* HAL_DMA_MODULE_ENABLED */
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
