#include <stdio.h>
#include "stdint.h"
#include "stm32f4xx.h"
#include "stm32f4xx_hal_conf.h"
//#include "misc.h"
//#include "mylib.h"

unsigned char Count1;
extern TIM_HandleTypeDef TIM_Handle;

/*
 * GPIO??????????
 */ 
void GPIO_InitStructReadtempCmd(void)
{
 GPIO_InitTypeDef GPIO_InitStruct;
 GPIO_InitStruct.Pin = GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
 GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
// GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
 GPIO_InitStruct.Pull = GPIO_NOPULL;	//??????????
 GPIO_InitStruct.Speed = GPIO_SPEED_FAST;   //??????
 HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);      //???
}

/*
 * RCC????????????
 */ 
void RCC_APB2PeriphReadtempyCmd(void)
{
   __GPIOD_CLK_ENABLE();  //??GPIOD????
   __TIM2_CLK_ENABLE(); //??TIM2???????
}

/*
 * ?????????NVIC???
 */ 
//#if 0
void NVIC_InitStructReadtempCmd(void)
{
// NVIC_InitTypeDef NVIC_InitStruct;
// NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);     //??????
  HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_0  );
//NVIC_InitStruct.NVIC_IRQChannel = TIM2_IRQn;     //??????
// NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;   //????????0
// NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;     //????????0
// NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;     //????
// NVIC_Init(&NVIC_InitStruct);         //???
	HAL_NVIC_SetPriority(TIM2_IRQn,2,0);
	HAL_NVIC_EnableIRQ(TIM2_IRQn);
}
//#endif
/*
 * TIMX???????????
 */ 

/*
 * ????
 */        
#if 0
int main(void)
{
// SystemInit();      //?????????72M??
 RCC_APB2PeriphReadtempyCmd();
 GPIO_InitStructReadtempCmd();
 NVIC_InitStructReadtempCmd();
 TIMX_InitStructReadtempCmd();

 Delay(50000);
 while(1)
  {
  // Delay(5000);
		if(Count1 >= 2)
         Count1 = 0;
    if(Count1 == 0)
        GPIO_ResetBits(GPIOD, GPIO_Pin_13 | GPIO_Pin_15);
    else
        GPIO_SetBits(GPIOD, GPIO_Pin_15 | GPIO_Pin_13);
  }
}

#endif

/*
 * TIM2?????????
// */ 
//void TIM2_IRQHandler(void)
//{
//  __HAL_TIM_CLEAR_FLAG(&TIM_Handle,TIM_FLAG_UPDATE);
//  Count1 += 1;
//}
