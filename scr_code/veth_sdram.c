/**
  ******************************************************************************
  * @file    FMC/FMC_SDRAM/Src/main.c 
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    26-June-2014
  * @brief   This sample code shows how to use STM32F4xx FMC HAL API to access 
  *          by read and write operation the SDRAM external memory device.
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
#include "veth_sdram.h"
#include "veth_common.h"

/** @addtogroup STM32F4xx_HAL_Examples
  * @{
  */

/** @addtogroup FMC_SDRAM_Basic
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

#define BUFFER_SIZE           ((uint32_t)0x0000000c)
#define WRITE_READ_ADDR       ((uint32_t)0x00200000)

#define WRITE_READ_ADDR_1     ((uint32_t)0x00000000)
#define WRITE_READ_ADDR_2     ((uint32_t)0x00400800)
#define WRITE_READ_ADDR_3     ((uint32_t)0x00800080)
#define WRITE_READ_ADDR_4     ((uint32_t)0x00c00040)



/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* SDRAM handler declaration */
SDRAM_HandleTypeDef hsdram;
extern FMC_SDRAM_TimingTypeDef SDRAM_Timing;
FMC_SDRAM_CommandTypeDef command;

/* Read/Write Buffers */
uint32_t aTxBuffer[BUFFER_SIZE] ={
    0x00ad00aa,0xadad00bb,0xafaf00cc,0x101000dd,
    0x032300ee,0x514100ff,0x45550099,0x54350088,
    0x41840077,0x53230066,0x0ada0055,0xddff0044
};
uint32_t Tem1[BUFFER_SIZE];
uint32_t Tem2[BUFFER_SIZE];
uint32_t Tem3[BUFFER_SIZE];
uint32_t Tem4[BUFFER_SIZE];

//uint32_t aRxBuffer[BUFFER_SIZE];
//uint8_t  bTxBuffer[BUFFER_SIZE];
//uint8_t  bRxBuffer[BUFFER_SIZE];

/* Status variables */
__IO uint32_t uwWriteReadStatus = 0;

/* Counter index */
uint32_t uwIndex = 0;

/* Private function prototypes -----------------------------------------------*/
//static void SystemClock_Config(void);
//static void Error_Handler(void);
//static void BSP_SDRAM_Initialization_Sequence(SDRAM_HandleTypeDef *hsdram, FMC_SDRAM_CommandTypeDef *Command);
//static void Fill_Buffer(uint32_t *pBuffer, uint32_t uwBufferLenght, uint32_t uwOffset);

/* Private functions ---------------------------------------------------------*/
void Test_for_eachbank()
  {
		 uint32_t my_counter = 0;
		 uint32_t address_offset;
		 uint32_t tem_value;
		 /*First step ,fill the array's data into bank 1 addr*/
//		for (uwIndex = 0 ; uwIndex < BUFFER_SIZE; uwIndex++) 
//		 {
//		   *(__IO uint32_t*)(SDRAM_BANK_ADDR + WRITE_READ_ADDR_1 + (uwIndex << 2) ) = aTxBuffer[uwIndex];
//     }
//		/*Get out the first bank value and print them out, then write them into the second bank*/
//		 printf("The data that write into bank 1 out:\n\r");
//		for (uwIndex = 0 ; uwIndex < BUFFER_SIZE; uwIndex++) /*Read out the data*/
//		 {
//		    Tem1[uwIndex] = *(__IO uint32_t*)(SDRAM_BANK_ADDR + WRITE_READ_ADDR_1 + (uwIndex << 2) );
//			  printf("%x \n\r",Tem1[uwIndex]);
//     }
		for (uwIndex = 0 ; uwIndex < BUFFER_SIZE; uwIndex++) /*Write into the second bank*/
		 {
		   *(__IO uint32_t*)(SDRAM_BANK_ADDR + WRITE_READ_ADDR_2 + (uwIndex << 2) ) = /*Tem1[uwIndex]*/ aTxBuffer[uwIndex];
     } 
		/*Get out the data into the 2 bank and print out and then write them into the 3rd bank*/
    printf("The data that write into bank 2 out:\n\r");
		for (uwIndex = 0 ; uwIndex < BUFFER_SIZE; uwIndex++) /*Read out the data*/
		 {
		    Tem2[uwIndex] = *(__IO uint32_t*)(SDRAM_BANK_ADDR + WRITE_READ_ADDR_2 + (uwIndex << 2) );
			  printf("%x \n\r",Tem2[uwIndex]);
     }
		for (uwIndex = 0 ; uwIndex < BUFFER_SIZE; uwIndex++) /*Write into the 3 bank*/
		 {
		   *(__IO uint32_t*)(SDRAM_BANK_ADDR + WRITE_READ_ADDR_3 + (uwIndex << 2) ) = Tem2[uwIndex];
     } 
   /*Get the data write into the bank 3 out and print out ,then write them into the 4th bank*/
    printf("The data that write into bank 3 out:\n\r");
		for (uwIndex = 0 ; uwIndex < BUFFER_SIZE; uwIndex++) /*Read out the data*/
		 {
		    Tem3[uwIndex] = *(__IO uint32_t*)(SDRAM_BANK_ADDR + WRITE_READ_ADDR_3 + (uwIndex << 2) );
			  printf("%x \n\r",Tem3[uwIndex]);
     }
		for (uwIndex = 0 ; uwIndex < BUFFER_SIZE; uwIndex++) /*Write into the 4 bank*/
		 {
		   *(__IO uint32_t*)(SDRAM_BANK_ADDR + WRITE_READ_ADDR_4 + (uwIndex << 2) ) = Tem3[uwIndex];
     } 
		/*Get the data write into the bank 4 out */
    printf("The data that write into bank 4 out:\n\r");
		for (uwIndex = 0 ; uwIndex < BUFFER_SIZE; uwIndex++) /*Read out the data*/
		 {
		    Tem4[uwIndex] = *(__IO uint32_t*)(SDRAM_BANK_ADDR + WRITE_READ_ADDR_4 + (uwIndex << 2) );
			  printf("%x \n\r",Tem4[uwIndex]);
     }
    /*Compare the tem register*/
    for(uwIndex = 0 ; uwIndex < BUFFER_SIZE ;uwIndex ++)
     {
			  /*Compare the data in tem1 and tem2*/
			   printf("uwIndex = %x \n\r",uwIndex);
//			   if(Tem1[uwIndex] != Tem2[uwIndex])
//					  printf("The data in the Tem1 and Tem2 unlike! \n\r");
//				 if(Tem1[uwIndex] != Tem3[uwIndex])
//					  printf("The data in the Tem1 and Tem3 unlike! \n\r");
//				 if(Tem1[uwIndex] != Tem4[uwIndex])
//					  printf("The data in the Tem1 and Tem4 unlike! \n\r");
				 if(Tem2[uwIndex] != Tem3[uwIndex])
					  printf("The data in the Tem2 and Tem3 unlike! \n\r");
				 if(Tem2[uwIndex] != Tem4[uwIndex])
					  printf("The data in the Tem2 and Tem4 unlike! \n\r");	
				 if(Tem3[uwIndex] != Tem4[uwIndex])
					  printf("The data in the Tem3 and Tem4 unlike! \n\r");					 
     }
		 
/*An amount of data that write into the SDRAM,Then read them out and compare with the value at first*/
		for(address_offset = 0x00000000 ;address_offset < 0x00400000 ;address_offset++)
	   {
	      *(__IO uint32_t*)(SDRAM_BANK_ADDR  + (address_offset *4) ) = address_offset;
		 }
    for(address_offset=0 ; address_offset < 0x00400000 ;address_offset ++)
		 {
       tem_value = *(__IO uint32_t*)(SDRAM_BANK_ADDR  + (address_offset *4) );
			 if(tem_value != address_offset)
			 {
          my_counter++;
       }				 
     }
	printf("My write and read operation result is:\n\r");
	if(my_counter == 0)
		 printf("Of course the SDRAM operation is very beautiful!\n\r");
	else
 	   {
	      printf("What the fuck!!SDRAM operation failed\n\r");
		    printf("%x\n\r",my_counter);
     } 
			 
  }
	
	

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */

int sdram_h(void)
{ 
//   uint32_t  i;
//   uint32_t  tem;	
  /*##-1- Configure the SDRAM device #########################################*/
  /* SDRAM device configuration */ 
  hsdram.Instance = FMC_SDRAM_DEVICE;
  
  SDRAM_Timing.LoadToActiveDelay    = 2;
  SDRAM_Timing.ExitSelfRefreshDelay = 6;
  SDRAM_Timing.SelfRefreshTime      = 4;
  SDRAM_Timing.RowCycleDelay        = 6;
  SDRAM_Timing.WriteRecoveryTime    = 2;
  SDRAM_Timing.RPDelay              = 2;
  SDRAM_Timing.RCDDelay             = 2;
  
  hsdram.Init.SDBank             = FMC_SDRAM_BANK1;
  hsdram.Init.ColumnBitsNumber   = FMC_SDRAM_COLUMN_BITS_NUM_9;
  hsdram.Init.RowBitsNumber      = FMC_SDRAM_ROW_BITS_NUM_12;
  hsdram.Init.MemoryDataWidth    = SDRAM_MEMORY_WIDTH;
  hsdram.Init.InternalBankNumber = FMC_SDRAM_INTERN_BANKS_NUM_4;
  hsdram.Init.CASLatency         = FMC_SDRAM_CAS_LATENCY_3;
  hsdram.Init.WriteProtection    = FMC_SDRAM_WRITE_PROTECTION_DISABLE;
  hsdram.Init.SDClockPeriod      = SDCLOCK_PERIOD;
  hsdram.Init.ReadBurst          = FMC_SDRAM_RBURST_ENABLE;
  hsdram.Init.ReadPipeDelay      = FMC_SDRAM_RPIPE_DELAY_0;
  
  /* Initialize the SDRAM controller */
//  if(HAL_SDRAM_Init(&hsdram, &SDRAM_Timing) != HAL_OK)
//  {
    /* Initialization Error */
//    Error_Handler(); 
//  }
//  
//  /* Program the SDRAM external device */
//   BSP_SDRAM_Initialization_Sequence(&hsdram, &command);   
//  
  /*##-2- SDRAM memory read/write access #####################################*/  
  
  /*Fill the buffer to write */
//  Fill_Buffer(aTxBuffer,
//            	BUFFER_SIZE, 
//	            0x5a5a5a5a);   
	/*Read and write operation for each bank operate*/
	 Test_for_eachbank();
	

  /* Write data to the SDRAM memory */
#ifdef MY_DEBUG
while(1){
  for (uwIndex = 0 ; uwIndex < BUFFER_SIZE; uwIndex++)
  {
		*(__IO uint32_t*) (SDRAM_BANK_ADDR + WRITE_READ_ADDR + (uwIndex << 2) ) = 0x5a5a5a5a;
  }
  }

//	for(uwIndex = 0; uwIndex < BUFFER_SIZE ; uwIndex ++)
//	    {
//         for(i=0 ;i<4;i++)
//				   {
//				      *(__IO uint32_t*) (SDRAM_BANK_ADDR + WRITE_READ_ADDR + 4*uwIndex +i) = bTxBuffer[uwIndex+i]; 
//           }
//      }
#endif  
	
//  /* Read back data from the SDRAM memory */
//  for (uwIndex = 0 ; uwIndex < BUFFER_SIZE; uwIndex++)
//  {
////aRxBuffer[uwIndex] = *(__IO uint32_t*) (SDRAM_BANK_ADDR + WRITE_READ_ADDR + 4*uwIndex);
//	  tem = *(__IO uint32_t*) (SDRAM_BANK_ADDR + WRITE_READ_ADDR + (uwIndex << 2));
//		if(tem != 0x5a5a5a5a)
//		 {
//        printf("%x \n\r",SDRAM_BANK_ADDR + WRITE_READ_ADDR + 4*uwIndex);
//     }
//  }
 /*Read back the 8-bit data from SDRAM*/
#ifdef MY_DEBUG
//	for(uwIndex = 0; uwIndex < BUFFER_SIZE ; uwIndex += 4)
//	    {
//         for(i=0 ;i<4 ;i++ )
//				   {
//				     bRxBuffer[uwIndex+i] = *(__IO uint32_t*) (SDRAM_BANK_ADDR + WRITE_READ_ADDR + 4*uwIndex +i);
//           }
//      }
#endif 
	
#ifdef MY_DEBUG
//  printf("After get the data from the SDRAM:\n\r");
//	for(uwIndex = 0 ;uwIndex < BUFFER_SIZE ;uwIndex++)
//	  {
//			 printf("%x \n\r",bRxBuffer[uwIndex]);
//    }
#endif 
		
  /*##-3- Checking data integrity ############################################*/    
  
//  for (uwIndex = 0; (uwIndex < BUFFER_SIZE) && (uwWriteReadStatus == 0); uwIndex++)
//  {
//    if (aRxBuffer[uwIndex] != aTxBuffer[uwIndex])
//    {
//      uwWriteReadStatus++;
//    }
//  }	
//  
//  if (uwWriteReadStatus)
//  {
//     printf("The Write operation or Read operation is perfect!");      
//  }

  
  /* Infinite loop */  
//  while (1)
//  {
//		 Blink_LED_RUN(); 
//  }
     return 1;
}


#if 0
/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 180000000
  *            HCLK(Hz)                       = 180000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = 25000000
  *            PLL_M                          = 25
  *            PLL_N                          = 360
  *            PLL_P                          = 2
  *            PLL_Q                          = 7
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 5
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;

  /* Enable Power Control clock */
  __PWR_CLK_ENABLE();

  /* The voltage scaling allows optimizing the power consumption when the device is 
     clocked below the maximum system frequency, to update the voltage scaling value 
     regarding system frequency refer to product datasheet.  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 360;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);
  
  /* Activate the Over-Drive mode */
  HAL_PWREx_ActivateOverDrive();  
  
  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;  
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;  
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
static void Error_Handler(void)
{
    /* Turn LED3 on */
    while(1)
    {
    }
}

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
                  
/**
  * @brief  Fills buffer with user predefined data.
  * @param  pBuffer: pointer on the buffer to fill
  * @param  uwBufferLenght: size of the buffer to fill
  * @param  uwOffset: first value to fill on the buffer
  * @retval None
  */
static void Fill_Buffer(uint32_t *pBuffer, uint32_t uwBufferLenght, uint32_t uwOffset)
{
  uint32_t tmpIndex = 0;

  /* Put in global buffer different values */
  for (tmpIndex = 0; tmpIndex < uwBufferLenght; tmpIndex++ )
  {
    pBuffer[tmpIndex] = tmpIndex + uwOffset;
  }
}

#ifdef  USE_FULL_ASSERT

#endif
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */ 

/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
