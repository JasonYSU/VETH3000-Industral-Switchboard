#ifndef VLAN_LINK_H
#define VLAN_LINK_H
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "veth_vlan.h"
#include "request.h"
struct VlanLink{
	int entry_no;
	//int vlan_no;
	int vlan_value;
	char vlan_name[50];
	API_VLAN_MEM_T vlan_entry;
//	int port_group_1;
//	int port_group_2;
//	int port_group_3;
	struct VlanLink *next;
};
//void initVlanLink();
extern int vlan_num;
extern struct VlanLink * vlan_root;
void addVlanLinkNode(struct VlanLink **root,struct VlanLink *vlanNode);
void deleteVlanLinkNodeById(struct VlanLink **root,int vlanNo,char *vlanName);
int isExistVlan(struct VlanLink *root,struct VlanLink *vlanLinkNode);
struct VlanLink ** mofidyVlanLinkId(struct VlanLink **root);

int emptyVlanLink(struct VlanLink *root); //清空VLAN链表
#endif
