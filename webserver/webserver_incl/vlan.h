#ifndef VLAN_H
#define VLAN_H
#include <math.h>
#include "vlan_link.h"
#include "request.h"
#include "board_info.h"

void addTestDataToLink();
void initVlanPorts();
void initFirstVlan(void);
char* generateVlanData(int vid,int total);
char* generateVlanStatus(int vid,int total);
void modifyVlanData(char *url);

char* generateVlanPortsStatus(void);
void modifyVlanPortsStatus(char *url);
void modifySwitchVlanPorts();

char* generateVlanPreviewStatus(void);
int deleteBoardVlanMember(int vlan_id,char *vlan_name);
int isNeedModfiyVlan(int vlan_id,char *vlan_name);
int isPortsTagChange(uint16_t vlan_id,uint8_t *src_tag,uint8_t *des_tag);

int concatVlanPortsTagString(char *des_str,uint8_t *ports_tag);

int addBoardVlanMember(uint16_t vlan_id,char *vlan_name,uint8_t *port_tag);
#endif
