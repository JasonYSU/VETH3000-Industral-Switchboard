/**
*************************************************************************
@File   : hal\src_code\veth_ntoh.h
@Author : Duke Lee
@Date   : 28-August-2014
@Brief  : Header file for the veth_ntoh.c
*************************************************************************
**/

//htonl : Host to net long type convert 
//htons : Host to net short type convert   
//ntohl : Net to host long type convert 
//ntohs : Net to host short type convert


#ifndef _NTOH_H__
#define _NTOH_H__

#ifdef STM32F429xx

unsigned long htrnl(unsigned long x);
unsigned short htrns(unsigned short x);
unsigned long ntrhl(unsigned long x);
unsigned short ntrhs(unsigned short x);

#endif


#endif
