/**
*************************************************************************
@File   : hal\src_code\veth_ntoh.c
@Author : Duke Lee
@Date   : 28-August-2014
@Brief  : Functions that convert between the big endian and little endian
*************************************************************************
**/

/*************************************************************************
For the STM32F4xx ,little endian format is surported,Till the internnet use big endian 
**************************************************************************/

//htonl : Host to net long type convert 
//htons : Host to net short type convert   
//ntohl : Net to host long type convert 
//ntohs : Net to host short type convert


/**
unsigned long    4-bytes
unsigned short   2-bytes
**/

#include <stdint.h>


/****************************************************/
//Functions : htonl()
unsigned long htrnl(unsigned long x)
  {
	uint8_t a[4];
	uint8_t i;
		 
	for( i=0 ; i <4 ; i++ )
	{
	  a[i] = (uint8_t)x >> (i*8);
	}
	x= 0x00000000;
	for( i=0 ; i <4 ; i++ )
	{
	  x = x << 8;
	  x |= (0x000000ff) & (unsigned long)a[i];
	}
	return x;
  }
	 
/****************************************************/
//Functions : htons()
unsigned short htrns(unsigned short x)
  {
	uint8_t a[2];
	uint8_t i;
		 
	for( i=0 ; i <2 ; i++ )
	{
	  a[i] = (uint8_t)(x >> (i*8));
	}
	x= 0x0000;
	for( i=0 ; i <2 ; i++ )
	{
	  x = x << 8;
	  x |= (0x00ff) & (unsigned short)a[i];
	}
	return x;
   }
	 
/****************************************************/
//Functions : ntohl()
unsigned long ntrhl(unsigned long x)
  {
	uint8_t a[4];
	uint8_t i;
		 
	for( i=0 ; i <4 ; i++ )
	{
	  a[i] = (uint8_t)(x >> (i*8));
	}
	x= 0x00000000;
	for( i=0 ; i <4 ; i++ )
	{
	  x = x << 8;
	  x |= (0x000000ff) & (unsigned long)a[i];
	}
	return x;
  }
	 
/****************************************************/
//Functions : ntohs()
unsigned short ntrhs(unsigned short x)
  {
	uint8_t a[2];
	uint8_t i;
		 
	for( i=0 ; i <2 ; i++ )
	{
	  a[i] = (uint8_t)(x >> (i*8));
	}
	x= 0x0000;
	for( i=0 ; i <2 ; i++ )
	{
	  x = x << 8;
	  x |= (0x00ff) & (unsigned short)a[i];
	}
	return x;
  }