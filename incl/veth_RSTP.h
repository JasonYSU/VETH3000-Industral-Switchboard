/**
****************************************************************************************
@File    : D:\ProgramFiles\KEIL\hal\incl\veth_RSTP.h
@Author  : Duke Lee
@Date    : 26-August-2014
@Version : V1.0.0
@Breif   : The header file for veth_RSTP.c
		       
****************************************************************************************
**/
#ifndef _RSTP_H__
#define _RSTP_H__
#include <stdint.h>
#include "pt.h"

extern struct pt rstp_pt;

PT_THREAD(RSTP(struct pt *pt,uint8_t *rx_packet,uint8_t *bpdu_targ));
uint8_t CREATE_RSTP_INSTANCE(void);
uint8_t configRSTP();

#endif 
