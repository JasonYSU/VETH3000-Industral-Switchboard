#ifndef LINK_NODE_H
#define LINK_NODE_H
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "veth_mac.h"
#include "veth_common.h"
#include "board_info.h"
struct MacLink{
	unsigned char id;
	MAC_API_T mac_entry;
	struct MacLink *next;
  
};
extern int age_time;
extern struct MacLink *mac_root;
extern unsigned char isFirstRequest;
extern unsigned char isRecoverMacConfig; //是否需要回复mac配置
extern int mac_addr_num ;
struct MacLink ** addMacLink(struct MacLink **root,struct MacLink *mac);
struct MacLink ** deleteMacLinkByVid(struct MacLink **root,uint8_t vid,uint8_t* macAddr);
struct MacLink ** deleteDymMacEntry(struct MacLink **root);
struct MacLink ** mofidyMacLinkId(struct MacLink **root);
struct MacLink *initRoot();
int getMacAddrNum(void);
void setMacAddrNum(int num);
unsigned char isExistNode(struct MacLink *root,struct MacLink *mac,char *str_mac);
unsigned char getMacString(unsigned char * mac_address,uint8_t *mac);
int emptyMacLink(struct MacLink **root);
int isFindVlanIdArray(uint16_t *vlan_id,uint16_t id);//判断vlan id 数组中是否已经有该id
void printMacLink(struct MacLink *root);
#endif
