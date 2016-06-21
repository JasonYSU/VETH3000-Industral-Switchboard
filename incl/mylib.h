#ifndef _MYLIB_H_
#define _MYLIB_H_


void RCC_Configuration(void);
void NVIC_Configuration(void);
void GPIO_Configuration(void);
void USART_Configuration(void);
void Delay(int i);
void USART6_Puts(char * str,unsigned int size);
void TIM_Configuration(void);
void TIMX_InitStructReadtempCmd(void);


#endif
