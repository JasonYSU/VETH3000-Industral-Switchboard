#ifndef MAC_H
#define MAC_H
#include"request.h"
#include"mac_link.h"
char* generateMacData(void);
void modifyMacData(char *url);
int getDivideMacAddress(unsigned char *des_hex_mac,char *src_mac_address);
int  modifyBoardMacAddress(struct MacLink *temp);
int deleteBoardMacAddress(uint8_t vid,uint8_t *mac_address);
void printfMacAddress(uint8_t *mac_address);
/* 把端口向量转换成数组的形式 */
int getPortsVectorArray(uint32_t ports_vector,int *vector_array);
/* 把向量拼接成要发送的字符串的形式 */
int concatVectorString(char*des_str,int*src_str);
/* 添加静态MAC地址到board中 */
int addBoardMacAddress(uint8_t vlan_id,uint8_t  mac_addr[6],uint16_t port_vector);
#endif
