/**
****************************************************************************************
@File    : D:\ProgramFiles\KEIL\hal\incl\veth_nor.h
@Author  : Duke Lee
@Date    : 11-Sept-2014
@Version : V1.0.0
@Breif   : The header file for veth_nor.c
		       
****************************************************************************************
**/

#ifndef __VETH_NOR_H_
#define __VETH_NOR_H_
#include <stdint.h>

void NOR_demo (void);
uint8_t BSP_NOR_Init(void);
uint8_t BSP_NOR_ReadData(uint32_t uwStartAddress, uint16_t *pData, uint32_t uwDataSize);
uint8_t BSP_NOR_WriteData(uint32_t uwStartAddress, uint16_t *pData, uint32_t uwDataSize);
uint8_t BSP_NOR_ProgramData(uint32_t uwStartAddress, uint16_t *pData, uint32_t uwDataSize);
uint8_t BSP_NOR_Erase_Block(uint32_t BlockAddress);
uint8_t BSP_NOR_Erase_Chip(void);
void BSP_NOR_ReturnToReadMode(void); 
void Fill_Buffer(uint16_t *pBuffer, uint32_t uwBufferLenght, uint32_t uwOffset);

#endif

