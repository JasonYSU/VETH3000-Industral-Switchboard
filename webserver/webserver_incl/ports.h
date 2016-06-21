#ifndef PORTS_H
#define PORTS_H
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include"gbk_conv_hanzi.h"
#include "veth_marvell_88e6097f.h"
#include "veth_port.h"
#include "request.h"
#include "board_info.h"
//#include "web_main.h"
//#include"web_main.h"
#define PORTS_SIZE 1024

int initPreBandWidth();    //初始化接受和发送的数组
int updatePreBandWidth();   //更新前一次的接收和发送的字节数组
int clearPreBandWidth();    //清除前一次的接收和发送的字节数组
char *generatePortsData();
char *generatePreviewData(void);
char *generateCountData();
char *getPortDetailData(int port_no);
void modifyPortsConfig(char *url);
void initPorts();
char *generateBandWidth(int port_no);

int getImgLocation(int port_no,int *array);

uint8_t modifySwitchChipPortsConfig();
#endif