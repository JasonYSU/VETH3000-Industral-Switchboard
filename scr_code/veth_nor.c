/**
  ******************************************************************************
  * @file    BSP/Src/nor.c 
  * @author  MCD Application Team
  * @version V0.8.0
  * @date    03-January-2014
  * @brief   This example code shows how to use the NOR Driver
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
#include <stdint.h>
#include <stdio.h>
#include "veth_common.h"
#include "stm32f4xx_hal_conf.h"
#include "stm324x9i_eval_nor.h"
#include "veth_hal_init.h"
//#include "main.h"



/** @addtogroup STM32F4xx_HAL_Examples
  * @{
  */

/** @addtogroup BSP
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/*Choose one of the defination for the test operation*/
#define NOR_READ
#define NOR_WRITE

/* Private define ------------------------------------------------------------*/
#define NOR_BUFFER_SIZE     ((uint32_t)0x00002000)
#define WRITE_READ_ADDR     ((uint32_t)0x00000000)
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

uint16_t nor_aRxBuffer[NOR_BUFFER_SIZE*512];
uint16_t nor_aTxBuffer[NOR_BUFFER_SIZE];
uint8_t ubIDStatus = 0, ubEraseStatus = 0, ubWriteStatus = 0, ubReadStatus = 0, ubInitStatus = 0;



void Fill_Buffer(uint16_t *pBuffer, uint32_t uwBufferLenght, uint32_t uwOffset);
//static uint8_t Buffercmp(uint16_t* pBuffer1, uint16_t* pBuffer2, uint16_t BufferLength);
uint8_t BSP_NOR_ReadData(uint32_t uwStartAddress, uint16_t* pData, uint32_t uwDataSize);
/**
  * @brief  NOR Demo
  * @param  None
  * @retval None
  */
void NOR_demo (void)
  { 
	/* NOR IDs structure */
  NOR_IDTypeDef pNOR_ID;

//  uint16_t   tem;
//  static uint16_t tem_value;
//  uint32_t address_offset;
//	uint32_t i;
	uint32_t loop;
//  uint32_t my_counter = 0 ;
/*##-2- Read & check the NOR device IDs ####################################*/
/* Initialize the ID structure */
#if 1
	pNOR_ID.Manufacturer_Code = (uint16_t)0x00;
	pNOR_ID.Device_Code1 = (uint16_t)0x00;
	pNOR_ID.Device_Code2 = (uint16_t)0x00;
	pNOR_ID.Device_Code3 = (uint16_t)0x00;

	/* Read the NOR memory ID */
	BSP_NOR_Read_ID(&pNOR_ID);

	printf("The result of Geting The Chip ID :\n\r");
	printf("%x\n\r",pNOR_ID.Manufacturer_Code);
	printf("%x\n\r",pNOR_ID.Device_Code1);
	printf("%x\n\r",pNOR_ID.Device_Code2);
	printf("%x\n\r",pNOR_ID.Device_Code3);
	/* Test the NOR ID correctness */
	if(pNOR_ID.Manufacturer_Code != (uint16_t)0x0001)
	ubIDStatus++;
	else if(pNOR_ID.Device_Code1 != (uint16_t)0x227E)
	ubIDStatus++;
	else if (pNOR_ID.Device_Code2 != (uint16_t)0x2222)
	ubIDStatus++;
	else if (pNOR_ID.Device_Code3 != (uint16_t)0x2201)
	ubIDStatus++;

	/*##-3- Erase NOR memory ###################################################*/ 
	/* Return to read mode */
	//  BSP_NOR_ReturnToReadMode();
	__NOR_WRITE(NOR_MEMORY_ADRESS, 0x00F0);
#endif
	/**/
  	Delay(50000);
//	tem_value = 0x5a5a;
//   __NOR_ADDR_SHIFT(__ADDRESS__);
// 	for(address_offset =0 ;address_offset < 65536 ;address_offset ++)
//	{
//	  tem_value = (uint16_t) address_offset;
//	  HAL_NOR_Program(&norHandle,(uint32_t *)__NOR_ADDR_SHIFT(address_offset),&tem_value);
//	}
//	for(address_offset=0 ; address_offset < 65536 ;address_offset ++)
//	{
//	  HAL_NOR_Read(&norHandle,(uint32_t *)__NOR_ADDR_SHIFT(address_offset),&tem_value);
//	  if(tem_value != (uint16_t)address_offset)
//	  {
//		my_counter++;
//	  }				 
//	}
//	printf("My write and read operation result is:\n\r");
//	if(my_counter == 0)
//		printf("Of course the FLASH operation is very beautiful!\n\r");
//	else
//	{
//	  printf("What the fuck!!FLASH operation failed\n\r");
//	  printf("%x\n\r",my_counter);
//	}   	

#ifdef NOR_WRITE 
/*Erase the whole chip to set all '0xFFFFs'*/
	if( BSP_NOR_Erase_Chip()!= NOR_OK)
	{
	  ubEraseStatus++; 
	}
	/*##-4- NOR memory read/write access  ######################################*/   
	/* Fill the buffer to write */
	Fill_Buffer(nor_aTxBuffer, NOR_BUFFER_SIZE, 0x5A5A);
//	Fill_Buffer(nor_aTxBuffer, BUFFER_SIZE, 0xC20F); 	
	/* Write data to the NOR memory */
	for(loop = 0 ;loop < 1024;loop++)
	{	
	  if(BSP_NOR_WriteData((WRITE_READ_ADDR + 2 * loop * NOR_BUFFER_SIZE), nor_aTxBuffer, NOR_BUFFER_SIZE) != NOR_OK)
	  {
		ubWriteStatus++; 
	  }
	}
#endif
#ifdef NOR_READ
/* Read back data from the NOR memory */
	for(loop =0 ;loop< 512 ;loop++)
	{ 
	  if(BSP_NOR_ReadData(WRITE_READ_ADDR + 2*loop*NOR_BUFFER_SIZE, nor_aRxBuffer + loop*NOR_BUFFER_SIZE, NOR_BUFFER_SIZE) != NOR_OK)
	  {
		ubReadStatus++; 
	  }
	}
#endif
//	while(1)
//	{
//	  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,GPIO_PIN_SET);
//	}	 
	if(ubInitStatus != 0)
	{
	  printf("There must be some problem at initialization!!\n\r");
	}
    if(ubEraseStatus != 0)
    {
      printf("The erase operation has some problem!!\n\r");
    }
    if(ubWriteStatus != 0)
    {
       printf("Write operation uncrect!!\n\r");
    }
    if(ubReadStatus != 0)
    {
      printf("Read operation has met some problem\n\r");
    }  
//	length = Buffercmp(nor_aRxBuffer, nor_aTxBuffer, BUFFER_SIZE);
//	if(Buffercmp(nor_aRxBuffer, nor_aTxBuffer, BUFFER_SIZE) > 0)
//	{
//	  printf("%x \n\r",length);
//	  printf("The readout data is not the same as that write into the FLASH\n\r");
//	}
	printf("Read done!!!\n\r");
}

#if 1
/**
  * @brief  Fills buffer with user predefined data.
  * @param  pBuffer: pointer on the buffer to fill
  * @param  uwBufferLenght: size of the buffer to fill
  * @param  uwOffset: first value to fill on the buffer
  * @retval None
  */
void Fill_Buffer(uint16_t *pBuffer, uint32_t uwBufferLenght, uint32_t uwOffset)
  {
	uint32_t tmpIndex = 0;

	/* Put in global buffer different values */
	for (tmpIndex = 0; tmpIndex < uwBufferLenght; tmpIndex++ )
	{
	  pBuffer[tmpIndex] = /*tmpIndex +*/uwOffset;
	}
  }

/**
  * @brief  Compares two buffers.
  * @param  pBuffer1, pBuffer2: buffers to be compared.
  * @param  BufferLength: buffer's length
  * @retval 1: pBuffer identical to pBuffer1
  *         0: pBuffer differs from pBuffer1
  */
static uint8_t Buffercmp(uint16_t* pBuffer1, uint16_t* pBuffer2, uint16_t BufferLength)
  {
	while (BufferLength--)
	{
	  if (*pBuffer1 != *pBuffer2)
	  {
		return BufferLength;
	  }
      pBuffer1++;
      pBuffer2++;
	}

    return 0;
  }

#endif
/**
  * @brief  Returns the NOR memory to read mode.
  * @param  None 
  * @retval None
  */
void BSP_NOR_ReturnToReadMode(void)
  {
	HAL_NOR_ReturnToReadMode(&norHandle);
  }
/**
  * @brief  Initialize the NOR flash
  * @param  None 
  * @retval return 0 in case of init successful
  */
uint8_t BSP_NOR_Init(void)
  {
	return FLASH_Configuration();
  }
/**
  * @brief  Writes an amount of data to the NOR device.
  * @param  uwStartAddress: Write start address
  * @param  pData: Pointer to data to be written
  * @param  uwDataSize: Size of data to write    
  * @retval NOR memory status
  */
uint8_t BSP_NOR_WriteData(uint32_t uwStartAddress, uint16_t* pData, uint32_t uwDataSize)
{
  uint32_t index = uwDataSize;
  
  while(index >= 0x0000001)
	{
    /* Write data to NOR */
    HAL_NOR_Program(&norHandle, (uint16_t *)(NOR_DEVICE_ADDR + uwStartAddress), pData);
    /* Read NOR device status */
    if(HAL_NOR_GetStatus(&norHandle, NOR_DEVICE_ADDR, PROGRAM_TIMEOUT) != NOR_SUCCESS)
    {
      return NOR_ERROR;
    }
    
    /* Update the counters */
    index--;
    uwStartAddress += 2;
    pData++; 
	}
  
  return NOR_OK;
}

/**
  * @brief  Programs an amount of data to the NOR device.
  * @param  uwStartAddress: Write start address
  * @param  pData: Pointer to data to be written
  * @param  uwDataSize: Size of data to write    
  * @retval NOR memory status
  */
uint8_t BSP_NOR_ProgramData(uint32_t uwStartAddress, uint16_t* pData, uint32_t uwDataSize)
{
  /* Send NOR program buffer operation */
  HAL_NOR_ProgramBuffer(&norHandle, uwStartAddress, pData, uwDataSize);
  
  /* Return the NOR memory status */
  if(HAL_NOR_GetStatus(&norHandle, NOR_DEVICE_ADDR, PROGRAM_TIMEOUT) != NOR_SUCCESS)
  {
    return NOR_ERROR;
  }
  else
  {
    return NOR_OK;
  }
}

/**
  * @brief  Erases the specified block of the NOR device. 
  * @param  BlockAddress: Block address to erase  
  * @retval NOR memory status
  */
uint8_t BSP_NOR_Erase_Block(uint32_t BlockAddress)
{
  /* Send NOR erase block operation */
  HAL_NOR_Erase_Block(&norHandle, BlockAddress, NOR_DEVICE_ADDR);
  
  /* Return the NOR memory status */  
  if(HAL_NOR_GetStatus(&norHandle, NOR_DEVICE_ADDR, BLOCKERASE_TIMEOUT) != NOR_SUCCESS)
  {
    return NOR_ERROR;
  }
  else
  {
    return NOR_OK;
  }
}

/**
  * @brief  Erases the entire NOR chip.
  * @param  None
  * @retval NOR memory status
  */
uint8_t BSP_NOR_Erase_Chip(void)
{
  /* Send NOR Erase chip operation */
  HAL_NOR_Erase_Chip(&norHandle, NOR_DEVICE_ADDR);
  
  /* Return the NOR memory status */
  if(HAL_NOR_GetStatus(&norHandle, NOR_DEVICE_ADDR, CHIPERASE_TIMEOUT) != NOR_SUCCESS)
  {
    return NOR_ERROR;
  }
  else
  {
    return NOR_OK;
  } 
}

/**
  * @brief  Reads NOR flash IDs.
  * @param  pNOR_ID : Pointer to NOR ID structure
  * @retval NOR memory status
  */
uint8_t BSP_NOR_Read_ID(NOR_IDTypeDef *pNOR_ID)
{
  if(HAL_NOR_Read_ID(&norHandle, pNOR_ID) != HAL_OK)
  {
    return NOR_ERROR;
  }
  else
  {
    return NOR_OK;
  }
}



uint8_t BSP_NOR_ReadData(uint32_t uwStartAddress, uint16_t* pData, uint32_t uwDataSize)
{
  if(HAL_NOR_ReadBuffer(&norHandle, NOR_DEVICE_ADDR + uwStartAddress, pData, uwDataSize) != HAL_OK)
  {
    return NOR_ERROR;
  }
  else
  {
    return NOR_OK;
  }
}

/**
  * @}
  */ 
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
