#ifndef CRC16_H
#define CRC16_H
#include<stdio.h>
#include<stdint.h>
uint16_t doCRC16(uint16_t crc_init,uint32_t begin_offset,uint32_t len,uint8_t *message);
#endif