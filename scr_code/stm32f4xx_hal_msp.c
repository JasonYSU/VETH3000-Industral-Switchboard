/**
  ******************************************************************************
  * @file    UART/UART_HyperTerminal_DMA/Src/stm32f4xx_hal_msp.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    26-June-2014
  * @brief   HAL MSP module.    
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
#include "veth_common.h"
#include "veth_serial.h"



/******************************************************************************/
/*Function : HAL_UART_MspInita(UART_HandleTypeDef *huart)*/
/*Brief    : DMA configuration for transmission request by peripheral of USART6*/
/*Parameter: UART_HandleTypeDef : huart point to the correspinding USART*/
/*retval   : None*/ 
/******************************************************************************/
void HAL_UART_MspInita(UART_HandleTypeDef *huart)
{
  static DMA_HandleTypeDef hdma_tx;
  static DMA_HandleTypeDef hdma_rx;
  
  /*##-3- Configure the DMA streams ##########################################*/
  /* Configure the DMA handler for Transmission process */
  hdma_tx.Instance                 = USART6_TX_DMA_STREAM;  
  hdma_tx.Init.Channel             = USART6_TX_DMA_CHANNEL;
  hdma_tx.Init.Direction           = DMA_MEMORY_TO_PERIPH;
  hdma_tx.Init.PeriphInc           = DMA_PINC_DISABLE;
  hdma_tx.Init.MemInc              = DMA_MINC_ENABLE;
  hdma_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  hdma_tx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
  hdma_tx.Init.Mode                = DMA_NORMAL;
  hdma_tx.Init.Priority            = DMA_PRIORITY_LOW;
  hdma_tx.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;         
  hdma_tx.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
  hdma_tx.Init.MemBurst            = DMA_MBURST_INC4;
  hdma_tx.Init.PeriphBurst         = DMA_PBURST_INC4;
  HAL_DMA_Init(&hdma_tx);   
  
  /* Associate the initialized DMA handle to the UART handle */
  __HAL_LINKDMA(huart, hdmatx, hdma_tx);
  /* Configure the DMA handler for reception process */
  hdma_rx.Instance                 = USART6_RX_DMA_STREAM;
  hdma_rx.Init.Channel             = USART6_RX_DMA_CHANNEL;
  hdma_rx.Init.Direction           = DMA_PERIPH_TO_MEMORY;
  hdma_rx.Init.PeriphInc           = DMA_PINC_DISABLE;
  hdma_rx.Init.MemInc              = DMA_MINC_ENABLE;
  hdma_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  hdma_rx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
  hdma_rx.Init.Mode                = DMA_NORMAL;
  hdma_rx.Init.Priority            = DMA_PRIORITY_HIGH;
  hdma_rx.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;         
  hdma_rx.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
  hdma_rx.Init.MemBurst            = DMA_MBURST_INC4;
  hdma_rx.Init.PeriphBurst         = DMA_PBURST_INC4; 
  
  HAL_DMA_Init(&hdma_rx);
  /* Associate the initialized DMA handle to the the UART handle */
  __HAL_LINKDMA(huart, hdmarx, hdma_rx);
    
}

/******************************************************************************/
/*Function : HAL_UART_MspInitb(UART_HandleTypeDef *huart)*/
/*Brief    : DMA configuration for transmission request by peripheral of UART5*/
/*Parameter: UART_HandleTypeDef : huart point to the correspinding USART*/
/*retval   : None*/ 
/******************************************************************************/
void HAL_UART_MspInitb(UART_HandleTypeDef *huart)
{
  static DMA_HandleTypeDef hdma_tx;
  static DMA_HandleTypeDef hdma_rx;
  
  /*##-3- Configure the DMA streams ##########################################*/
  /* Configure the DMA handler for Transmission process */
  hdma_tx.Instance                 = UART5_TX_DMA_STREAM;  
  hdma_tx.Init.Channel             = UART5_TX_DMA_CHANNEL;
  hdma_tx.Init.Direction           = DMA_MEMORY_TO_PERIPH;
  hdma_tx.Init.PeriphInc           = DMA_PINC_DISABLE;
  hdma_tx.Init.MemInc              = DMA_MINC_ENABLE;
  hdma_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  hdma_tx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
  hdma_tx.Init.Mode                = DMA_NORMAL;
  hdma_tx.Init.Priority            = DMA_PRIORITY_LOW;
  hdma_tx.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;         
  hdma_tx.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
  hdma_tx.Init.MemBurst            = DMA_MBURST_INC4;
  hdma_tx.Init.PeriphBurst         = DMA_PBURST_INC4;
  HAL_DMA_Init(&hdma_tx);   
  
  /* Associate the initialized DMA handle to the UART handle */
  __HAL_LINKDMA(huart, hdmatx, hdma_tx);
  /* Configure the DMA handler for reception process */
  hdma_rx.Instance                 = UART5_RX_DMA_STREAM;
  hdma_rx.Init.Channel             = UART5_RX_DMA_CHANNEL;
  hdma_rx.Init.Direction           = DMA_PERIPH_TO_MEMORY;
  hdma_rx.Init.PeriphInc           = DMA_PINC_DISABLE;
  hdma_rx.Init.MemInc              = DMA_MINC_ENABLE;
  hdma_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  hdma_rx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
  hdma_rx.Init.Mode                = DMA_NORMAL;
  hdma_rx.Init.Priority            = DMA_PRIORITY_HIGH;
  hdma_rx.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;         
  hdma_rx.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
  hdma_rx.Init.MemBurst            = DMA_MBURST_INC4;
  hdma_rx.Init.PeriphBurst         = DMA_PBURST_INC4; 
  
  HAL_DMA_Init(&hdma_rx);
  /* Associate the initialized DMA handle to the the UART handle */
  __HAL_LINKDMA(huart, hdmarx, hdma_rx);
    
}

/******************************************************************************/
/*Function : HAL_UART_MspInitc(UART_HandleTypeDef *huart)*/
/*Brief    : DMA configuration for transmission request by peripheral of UART7*/
/*Parameter: UART_HandleTypeDef : huart point to the correspinding USART*/
/*retval   : None*/ 
/******************************************************************************/
void HAL_UART_MspInitc(UART_HandleTypeDef *huart)
{
  static DMA_HandleTypeDef hdma_tx;
  static DMA_HandleTypeDef hdma_rx;
  
  /*##-3- Configure the DMA streams ##########################################*/
  /* Configure the DMA handler for Transmission process */
  hdma_tx.Instance                 = UART7_TX_DMA_STREAM;
  hdma_tx.Init.Channel             = UART7_TX_DMA_CHANNEL;
  hdma_tx.Init.Direction           = DMA_MEMORY_TO_PERIPH;
  hdma_tx.Init.PeriphInc           = DMA_PINC_DISABLE;
  hdma_tx.Init.MemInc              = DMA_MINC_ENABLE;
  hdma_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  hdma_tx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
  hdma_tx.Init.Mode                = DMA_NORMAL;
  hdma_tx.Init.Priority            = DMA_PRIORITY_LOW;
  hdma_tx.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;         
  hdma_tx.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
  hdma_tx.Init.MemBurst            = DMA_MBURST_INC4;
  hdma_tx.Init.PeriphBurst         = DMA_PBURST_INC4;
  HAL_DMA_Init(&hdma_tx);   
  /* Associate the initialized DMA handle to the UART handle */
  __HAL_LINKDMA(huart, hdmatx, hdma_tx);
  /* Configure the DMA handler for reception process */
  hdma_rx.Instance                 = UART7_RX_DMA_STREAM;
  hdma_rx.Init.Channel             = UART7_RX_DMA_CHANNEL;
  hdma_rx.Init.Direction           = DMA_PERIPH_TO_MEMORY;
  hdma_rx.Init.PeriphInc           = DMA_PINC_DISABLE;
  hdma_rx.Init.MemInc              = DMA_MINC_ENABLE;
  hdma_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  hdma_rx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
  hdma_rx.Init.Mode                = DMA_NORMAL;
  hdma_rx.Init.Priority            = DMA_PRIORITY_HIGH;
  hdma_rx.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;         
  hdma_rx.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
  hdma_rx.Init.MemBurst            = DMA_MBURST_INC4;
  hdma_rx.Init.PeriphBurst         = DMA_PBURST_INC4; 
  HAL_DMA_Init(&hdma_rx);
  /* Associate the initialized DMA handle to the the UART handle */
  __HAL_LINKDMA(huart, hdmarx, hdma_rx);
}

/******************************************************************************/
/*Function : HAL_UART_MspInitb(UART_HandleTypeDef *huart)*/
/*Brief    : DMA configuration for transmission request by peripheral of UART4*/
/*Parameter: UART_HandleTypeDef : huart point to the correspinding USART*/
/*retval   : None*/ 
/******************************************************************************/
void HAL_UART_MspInitd(UART_HandleTypeDef *huart)
{
  static DMA_HandleTypeDef hdma_tx;
  static DMA_HandleTypeDef hdma_rx;
  
  /*##-3- Configure the DMA streams ##########################################*/
  /* Configure the DMA handler for Transmission process */
  hdma_tx.Instance                 = UART4_TX_DMA_STREAM;  
  hdma_tx.Init.Channel             = UART4_TX_DMA_CHANNEL;
  hdma_tx.Init.Direction           = DMA_MEMORY_TO_PERIPH;
  hdma_tx.Init.PeriphInc           = DMA_PINC_DISABLE;
  hdma_tx.Init.MemInc              = DMA_MINC_ENABLE;
  hdma_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  hdma_tx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
  hdma_tx.Init.Mode                = DMA_NORMAL;
  hdma_tx.Init.Priority            = DMA_PRIORITY_LOW;
  hdma_tx.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;         
  hdma_tx.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
  hdma_tx.Init.MemBurst            = DMA_MBURST_INC4;
  hdma_tx.Init.PeriphBurst         = DMA_PBURST_INC4;
  HAL_DMA_Init(&hdma_tx);   
  
  /* Associate the initialized DMA handle to the UART handle */
  __HAL_LINKDMA(huart, hdmatx, hdma_tx);
  /* Configure the DMA handler for reception process */
  hdma_rx.Instance                 = UART4_RX_DMA_STREAM;
  hdma_rx.Init.Channel             = UART4_RX_DMA_CHANNEL;
  hdma_rx.Init.Direction           = DMA_PERIPH_TO_MEMORY;
  hdma_rx.Init.PeriphInc           = DMA_PINC_DISABLE;
  hdma_rx.Init.MemInc              = DMA_MINC_ENABLE;
  hdma_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  hdma_rx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
  hdma_rx.Init.Mode                = DMA_NORMAL;
  hdma_rx.Init.Priority            = DMA_PRIORITY_HIGH;
  hdma_rx.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;         
  hdma_rx.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
  hdma_rx.Init.MemBurst            = DMA_MBURST_INC4;
  hdma_rx.Init.PeriphBurst         = DMA_PBURST_INC4; 
  
  HAL_DMA_Init(&hdma_rx);
  /* Associate the initialized DMA handle to the the UART handle */
  __HAL_LINKDMA(huart, hdmarx, hdma_rx);
    
}