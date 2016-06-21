/**
**************************************************************************
@File    :  D:\ProgramFiles\KEIL\PRO\incl\veth_com_uart.h
@Author  :  Duke Lee
@Verdion :  V1.0
@Date    :  25-June-2014
@brief   :  the statement of the functions
**************************************************************************
**/

#ifndef __COM_UART_H_
#define __COM_UART_H_

#include "stm32f4xx_hal_conf.h"
#include "veth_common.h"
#include "veth_invariate.h"
#include "pt.h"
/*Define the maximum length of a command*/
#define MAX_COMMAND_LEN   100

/*Define the buadrate enum*/
typedef enum  uart_buadrate {
  BUAD_110    = 110,
  BUAD_300    = 300,
  BUAD_600    = 600,
  BUAD_1200   = 1200,
  BUAD_2400   = 2400,
  BUAD_4800   = 4800,
  BUAD_9600   = 9600,
  BUAD_14400  = 14400,
  BUAD_19200  = 19200, 
  BUAD_38400  = 38400,
  BUAD_57600  = 57600, 
  BUAD_115200 = 115200,
  BUAD_230400 = 230400,
  BUAD_380400 = 380400,
  BUAD_460800 = 460800,
  BUAD_921600 = 921600 
}U_BUAD_T;

/*Define the UART wordlength enum*/
typedef enum U_wordlength {
  WORDLENGTH_8 = UART_WORDLENGTH_8B,
  WORDLENGTH_9 = UART_WORDLENGTH_9B
}U_WORDLENGTH_T;
/*Define the UART stop bits enum*/
typedef enum U_stop_bits {
  STOP_1  = UART_STOPBITS_1,
  STOP_2  = UART_STOPBITS_2
}U_STOP_T;
/*Define the UART parity enum*/
typedef enum U_parity    {
  PARITY_NONE = UART_PARITY_NONE,
  PARITY_EVEN = UART_PARITY_EVEN,
  PARITY_ODD  = UART_PARITY_ODD
}U_PARITY_T;

typedef struct uart_property {
  U_BUAD_T        buad_rate;
  U_WORDLENGTH_T  wordlen;
  U_STOP_T        stopbits;
  U_PARITY_T      parity;
}UART_T;

extern struct pt cli_pt;

void	UART_Transmit(UART_HandleTypeDef *huart,uint8_t* pdata,uint16_t Size);
MYERROR_INFO UART_Recieve(UART_HandleTypeDef *huart,uint16_t Size);
PT_THREAD(CLI(struct pt *pt,UART_HandleTypeDef *huart,INVARIATE_T *invarite,uint32_t timeout));
void	New_CLI(UART_HandleTypeDef *huart,INVARIATE_T *invarite);
void	getChar(uint8_t *p_data,uint16_t len);
void	sendChar(uint8_t *p_data,uint16_t len);
 
#endif
