/*******************************************************
File name: vlan_link.c
Description: 实现了vlan缓存时使用的链表相关的操作
Author: dujiancheng
Version: v1.0
********************************************************/
#include "vlan_link.h"
#include"vlan.h"
int vlan_num = 0;
struct VlanLink * vlan_root = NULL;
/**
 * Function: addVlanLinkNode
 * Description: 向VLAN链表中添加VLAN节点，添加按照VLAN ID升序添加
 * @param root 指向VLAN头部节点指针的指针
 * @param vlanNode 指向要添加的节点的指针
**/
void addVlanLinkNode(struct VlanLink **root,struct VlanLink *vlanNode){//
	struct VlanLink *p;
	struct VlanLink *pre;
	if(*root == NULL){
		*root = vlanNode;
		vlanNode->next = NULL;
	}else{
		p = pre = *root;
		while( p && p->vlan_entry.vlan_id < vlanNode->vlan_entry.vlan_id){
			pre = p;
			p = p->next;
		}
		if( p == *root ){
			vlanNode->next = p;
			*root = vlanNode;
		}else{
			pre->next = vlanNode;
			vlanNode->next = p;
		}
	}
	++vlan_num;
	mofidyVlanLinkId(root);  //修改VLAN链表id
}
/**
 * Function: deleteVlanLinkNodeById
 * Description: 删除VLAN链表中的数据节点
 * @param root 指向VLAN头部节点指针的指针
 * @param vlanNo 被删除的VLAN节点ID
 * @param vlanName 被删除的VLAN节点的名称
**/
void deleteVlanLinkNodeById(struct VlanLink **root,int vlanNo,char *vlanName){
	struct VlanLink *pre;
	struct VlanLink *tail;
	pre = tail = *root;
	while(1){
		if(tail->vlan_entry.vlan_id == vlanNo && !strcmp(tail->vlan_name,vlanName)){
			break;
		}
		pre = tail;
		tail = tail->next;
	}
	pre->next = tail->next;
	pre = tail->next;
	if(tail == *root)
		*root = pre;
	//freeMemory(tail);
	free(tail);
	tail = NULL;
	--vlan_num;
	mofidyVlanLinkId(root);  //修改VLAN链表id
}
/**
 * Function: isExistVlan
 * Description: 判断VLAN节点是否存在于VLAN链表中
 * @param root 指向VLAN头部节点指针的指针
 * @param vlanLinkNode 被检测的VLAN节点
 * @return 1 节点已经存在
 *		   0 节点不存在
**/
int isExistVlan(struct VlanLink *root,struct VlanLink *vlanLinkNode){
	struct VlanLink *p;
	for( p  = root; p ; p = p->next){
		if( p->vlan_entry.vlan_id == vlanLinkNode->vlan_entry.vlan_id && !strcmp(p->vlan_name,vlanLinkNode->vlan_name)){
			return 1;
		}
	}
	return 0;
}
/*
* */
/**
 * Function: mofidyVlanLinkId
 * Description: 修改VLAN链表项的id号，该方法应该在添加VLAN表项以及执行完删除操作后应该调用该方法
 * 				从新对id进行从新进行编号
 * @param root 指向VLAN头部节点指针的指针
**/
struct VlanLink ** mofidyVlanLinkId(struct VlanLink **root){
	unsigned char i = 1;
	struct VlanLink *p;
	for(p = *root; p ; p = p->next){
		p->entry_no = i++;
	}
	return root;
}
/**
 * Function: emptyVlanLink
 * Description: 清空VLAN链表
 * @param root 指向VLAN头部节点的指针
**/
int emptyVlanLink(struct VlanLink *root){
	struct VlanLink *p,*q;
	if(root){
		p = root;
		while(p){
			q = p;
			p = p->next;
			deleteBoardVlanMember(q->vlan_entry.vlan_id,q->vlan_name);
			free(q);
			q = NULL;
			--vlan_num;
		}
	}
	return 1;
}