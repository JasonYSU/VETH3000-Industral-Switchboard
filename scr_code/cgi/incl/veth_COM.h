/**
**************************************************************************
@File    :  hal\src_code\cgi\inc\veth_COM.h
@Author  :  Duke Lee
@Verdion :  V1.0
@Date    :  26-Jan-2015
@brief   :  cgi接口函数：veth_COM.c的头文件
**************************************************************************
**/

#ifndef _VETH_COM_H__
#define _VETH_COM_H__

#include <stdint.h>
#include "board_info.h"

#define RS232_MODE   0
#define RS485_MODE   1
#define RS422_MODE   2

/*Define a struct for configing the mode of the uart chip*/
typedef struct com_mode_t {
  uint8_t  mode_com12;
  uint8_t  mode_com34;
}COM_MODE_T;

void    comDemo(void);
uint8_t setComProperty(struct serial_config *hcom);
void    setComMode(COM_MODE_T *pcom);
void	setChipMode(uint8_t chipNo,uint8_t mode);
#endif 