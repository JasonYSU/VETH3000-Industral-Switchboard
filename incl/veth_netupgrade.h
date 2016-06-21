/**
**************************************************************************
@File    :  hal\incl\veth_netupgrade.h
@Author  :  Duke Lee
@Verdion :  V1.0
@Date    :  22-Dec-2014
@brief   :  The header file for veth_netupgrade.c  
**************************************************************************
**/

#ifndef __VETH_NETUPGRADE_H__
#define __VETH_NETUPGRADE_H__

#include <stdint.h>

#define TEMP_UPGRADE_ADDR  0xC0600000

typedef struct bin_buffer_t {
  uint8_t   temp_buff[848*1024];
  uint32_t  checksum;
  uint32_t  length;  
}BIN_BUFFER_T;

void saveNetData(BIN_BUFFER_T *package);

#endif