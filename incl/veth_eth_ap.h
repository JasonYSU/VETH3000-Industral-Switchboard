/**
****************************************************************************************
@File    : D:\ProgramFiles\KEIL\hal\incl\veth_eth_ap.h
@Author  : Duke Lee
@Date    : 06-August-2014
@Version : V1.0.0
@Breif   : The header file for veth_eth_ap.c
		       
****************************************************************************************
**/
#ifndef __ETH_AP_H_
#define __ETH_AP_H_

#include "stm32f4xx_hal_conf.h"
#include "stp_bpdu.h"
#include <stdint.h>



void 
ETH_TX_PACKET(ETH_HandleTypeDef *hal_eth,uint32_t frame_length,void * packets,uint16_t port_index);
int
ETH_RX_PACKET(ETH_HandleTypeDef *hal_eth,uint8_t * packets,BPDU_T *bpdu,uint8_t *port_index,uint32_t timeout);

void         uip_Tx_Packet(uint8_t *pbuf,uint16_t frame_len);    //Tx Driver for the uIP stack 
unsigned int uip_Rx_Packet(uint8_t *pbuf,uint16_t buffer_size,uint32_t timeout);
void Tx_packet(ETH_HandleTypeDef *hal_eth); //Only for test
void Rx_packet(ETH_HandleTypeDef *hal_eth); //Only for test
void ETH_EXCS(ETH_HandleTypeDef *hal_eth);  //Only for test
#endif



/***************************************End line*********************************************/




