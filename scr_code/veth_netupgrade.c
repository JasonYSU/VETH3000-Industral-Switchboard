/**
**************************************************************************
@File    :  hal\src_code\veth_netupgrade.c
@Author  :  Duke Lee
@Verdion :  V1.0
@Date    :  22-Dec-2014
@brief   :  The lower drive for updating the application area of the main flash  
**************************************************************************
**/

#include <stdio.h>
#include <string.h>

#include "veth_common.h"
#include "veth_Invariate.h"
#include "veth_netupgrade.h"
#include "stm32f4rstp.c"
#include "crc16.h"
//uint8_t test_buff[100]  = {
// 0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70,
// 0x81, 0x91, 0xa1, 0xb1, 0xc1, 0xd1, 0xe1, 0xf1,
// 
// 0x12, 0x02, 0x32, 0x22, 0x52, 0x42, 0x72, 0x62,
// 0x93, 0x18, 0xb3, 0xaa, 0xdd, 0xcc, 0xf3, 0xe3, 
// 
// 0x24, 0x34, 0x04, 0x14, 0x64, 0x74, 0x44, 0x54, 
// 0xa5, 0xb5, 0x85, 0x95, 0xe5, 0xf5, 0xc5, 0xd5, 
// 
// 0x36, 0x26, 0x16, 0x06, 0x76, 0x66, 0x56, 0x46, 
// 0xbb, 0x7a, 0x19, 0x38, 0xdf, 0xfe, 0x9d, 0xbc, 
// 
// 0xc4, 0xe5, 0x86, 0xa7, 0x40, 0x61, 0x02, 0x23, 
// 0xcc, 0xed, 0x8e, 0xaf, 0x48, 0x69, 0x0a, 0x2b, 
// 
// 0xf5, 0xd4, 0xb7, 0x96, 0x11, 0x00, 0x33, 0x12, 
// 0xdd, 0xcb, 0xfb, 0xeb, 0x9b, 0x8b, 0xbb, 0xab, 
// 
// 0x6c, 0x7c, 0x4c, 0x5c 
// };

#ifdef MY_DEBUG_P
/*fill in the BIN_BUFFER_T*/
void fillinPackageBuffer(BIN_BUFFER_T *package)
  {
	Str_cpy(package->temp_buff,tem_bin,50*1024);
	package->length = 50*1024;
	package->checksum = 0x12af;
  }
#endif

/******************************************************************************/
/*Function : saveNetData()*/
/*Brief    : Save the recieved data through network to the specific area*/
/*Parameter: */ 
/*retval   : */ 
/******************************************************************************/
void saveNetData(BIN_BUFFER_T *package)
  {
	uint8_t  my_targ;
/*Before jump to the bootloader,print the basic information,for my_debug only*/
//	fillinPackageBuffer(package);
	getNetworkUpgradeTarg(&my_targ);
	setNetworkUpgradeTarg(FLASH_TARG_SET);
#ifdef MY_DEBUG
	getNetworkUpgradeTarg(&my_targ);
	printf("Before jump to the bootloader:\n\r");
	printf("Check the flash targ: %d\n\r",my_targ);
	printf("temp_buff :%x %x %x %x %x %x\n\r",package->temp_buff[0],package->temp_buff[1],
	                                       package->temp_buff[2],package->temp_buff[3],
										   package->temp_buff[4],package->temp_buff[5]);
	printf("checksum  :%x\n\r",package->checksum);
	printf("length    :%d\n\r",package->length);
#endif
/*After fill in the buffer,jump to the bootloader through restart operation*/
	NVIC_SystemReset();
  }