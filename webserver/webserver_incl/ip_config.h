#ifndef IP_CONFIG_H
#define IP_CONFIG_H
#include "board_info.h"
#include "netif.h"
int setHostAddress(struct netif *netif,VETH_IP_T *ip);
int modifyIPConfig(char *url);
int getFormatAddress(uint8_t *array,char *str);
#endif