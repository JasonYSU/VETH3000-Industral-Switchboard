/*******************************************************
File name: mac_link.c
Description: 实现了链表来存储所有的MAC地址，对于MAC地址的显示采用了缓存的方式进行实现
			 如果用户只是进行分页查看就从缓存链表中取MAC地址的数据。如果用户进行了刷新则需要
			 重新读取MAC地址
Author: dujiancheng
Version: v1.0
********************************************************/
#include "mac_link.h"
#include "mac.h"
#include "LogUtility.h"
#include "request.h"
int mac_addr_num = 0;
struct MacLink *mac_root = NULL;
int age_time = 300;
/**
 * Function: initRoot
 * Description: 初始化MAC地址链表，从底层寄存器中读取所有的MAC地址表项，放到MAC地址的链表中
 *
**/
struct MacLink * initRoot(){
		MAC_ENTRY_T entryx;
	    MAC_API_T  api_entry;
		uint16_t vlan_id[MAXMACADDRNUM];
		int i,j,vlan_index;
		struct MacLink  *temp;
		memset(vlan_id,0,sizeof(vlan_id));
		///printf("isFirstRequest value is %d\n\r",isFirstRequest);
		if(isFirstRequest){  //第一次调用Mac表数据生成Mac表项的缓存以及老化时间的缓存
				isFirstRequest = FALSE;
				if(isRecoverMacConfig){
					recoverConfigEntries("config_mac");
					isRecoverMacConfig = FALSE;
				}
				memset(vlan_id,0,sizeof(vlan_id));
				/* 先遍历vlan数组中找到已经添加的vlan */
				for(i = 0; i < MAXMACADDRNUM; i++ ){
					if(board.vlan_member[i].end == 1){
						break;
					}
					vlan_id[i] = board.vlan_member[i].vlan_id;
				}
				j = i; //记录vlan_id 中下一个空位
				/* 再遍历mac地址数组找到已经添加的mac地址但是对应的vlan可能不存在 */
				for(i = 0; i < MAXMACADDRNUM; i++ ){
					if(board.mac_address.mac_table[i].end == 1){
						break;
					}
					/* 判断该vlan id是否已经找到 */
					if( !isFindVlanIdArray(vlan_id,board.mac_address.mac_table[i].vlan_id) 
						&& j < MAXMACADDRNUM){
						vlan_id[j++] = board.mac_address.mac_table[i].vlan_id;
					}
				}
				/* 生成静态Mac表项的缓存 */
			
			for(vlan_index = 0; vlan_index < j; vlan_index++){
				//printf("vid is %d\n\r",vlan_id[vlan_index]);
			    memcpy(entryx.mac_entry_MAC,Targ_MAC,6);
				while(Display_ALL_MACentry(&api_entry,&entryx,vlan_id[vlan_index]))
				{
				  //printf("init dym is %d\n\r",api_entry.sta_or_dym);
				  temp = (struct MacLink *)malloc(sizeof(struct MacLink));
				  if( !temp){
						printf("init mac root memory allocate\n\r");
						break;
				  }
				  temp->mac_entry.add_or_delet = api_entry.add_or_delet;
					temp->mac_entry.vlan_id = api_entry.vlan_id;
				    memcpy(temp->mac_entry.new_mac,api_entry.new_mac,6);
					temp->mac_entry.port_vector = api_entry.port_vector;
					temp->mac_entry.sta_or_dym = api_entry.sta_or_dym;
					//printf("add to cache!\n\r");
					addMacLink(&mac_root,temp);
				}
			}
			 /* 生成老化时间的缓存 */
				//age_time = Get_Age_time();
		}
		//printMacLink(mac_root);
		return mac_root;
}
/**
 * Function: addMacLink
 * Description: 添加MAC地址到链表中，添加的原则为：先按照vlan ID升序，在同一个vlan ID中按照MAC地址的
				升序进行添加
 * @param root 二阶指针，指向MAC地址的头结点
 * @param mac  要添加的MAC地址的节点信息
 * @rerun 指向新的链表的头结点
 * @TODO 该函数可以没有返回值，由于参数是指向头结点的指针，所有对链表的整体结构不会产生影响
**/
struct MacLink ** addMacLink(struct MacLink **root,struct MacLink *mac){
	struct MacLink *p;
	struct MacLink *pre;
	unsigned char mac_address[20];
	unsigned char root_mac_address[20];
	if(*root == NULL){
		*root = mac;
		mac->next = NULL;
		mac_addr_num++;
	}else{
		if( !isExistNode(*root,mac,NULL) ){
		    memset(mac_address,0,sizeof(mac_address));
			getMacString(mac_address,mac->mac_entry.new_mac);
			p = pre = *root;
			memset(root_mac_address,0,sizeof(mac_address));
			getMacString(root_mac_address,p->mac_entry.new_mac);
			while(p){
				if(mac->mac_entry.vlan_id == p->mac_entry.vlan_id){
					if(strcmp((char*)root_mac_address,(char*)mac_address) > 0){
						break;
					}
				}else if(p->mac_entry.vlan_id > mac->mac_entry.vlan_id ){
					break;
				}
				pre = p;
				p = p->next;
				memset(root_mac_address,0,sizeof(mac_address));
				getMacString(root_mac_address,p->mac_entry.new_mac);
			}
			if( p == *root ){
				mac->next = p;
				*root = mac;
			}else{
				pre->next = mac;
				mac->next = p;
			}
			mac_addr_num++;
		}else{
			//printf("node is exist,not add!\n\r");
		}
	}
	
	mofidyMacLinkId(root);
	return root;
}
/**
 * Function: deleteDymMacEntry
 * Description: 删除MAC地址链表中所有属于动态MAC地址的表项
 * @param root 二阶指针，指向MAC地址的头结点
 * @rerun 指向新的链表的头结点
 * @TODO 该函数可以没有返回值，由于参数是指向头结点的指针，所有对链表的整体结构不会产生影响
**/
struct MacLink ** deleteDymMacEntry(struct MacLink **root){
	struct MacLink *pre;//记录前一个节点
	struct MacLink *tail;  //记录要删除的节点
	//int i;
	pre = tail = *root;
	for(tail = *root; tail ;tail = pre->next){
		if(tail->mac_entry.sta_or_dym == ENTRY_DYNAMIC){
			if(tail == *root){
				pre = *root = pre->next;
			}else if(tail->next == NULL){
				pre->next = NULL;
			}else{
				pre->next = tail->next;
			}
			free(tail);
			tail = NULL;
			--mac_addr_num;
		}else{
			pre = tail;
		}
		
	}
	
	mofidyMacLinkId(root);
	return root;
}
/**
 * Function: deleteMacLinkByVid
 * Description: 删除MAC地址链表某一个MAC地址
 * @param root 二阶指针，指向MAC地址的头结点
 * @param vid 要删除的MAC地址所属的vlan ID
 * @param macAddr 要删除的MAC地址
 * @rerun 指向新的链表的头结点
 * @TODO 该函数可以没有返回值，由于参数是指向头结点的指针，所有对链表的整体结构不会产生影响
**/
struct MacLink ** deleteMacLinkByVid(struct MacLink **root,uint8_t vid,uint8_t *macAddr){
	struct MacLink *pre;
	struct MacLink *tail;
	int i;
	pre = tail = *root;
	while(1){
		//getDivideMacAddress(hex_mac,p->mac_entry.new_mac);
		if( !memcmp(tail->mac_entry.new_mac,macAddr,6) 
			&& tail->mac_entry.vlan_id == vid){
			break;
		}
		pre = tail;
		tail = tail->next;
	}
	pre->next = tail->next;
	pre = tail->next;
	if(tail == *root)
		*root = pre;
	free(tail);
	tail = NULL;
	//printf("delete mac link sucessful\n\r");
	--mac_addr_num;
	mofidyMacLinkId(root);
	return root;
}
/**
 * Function: mofidyMacLinkId
 * Description: 修改mac链表项的id号，该方法应该在添加mac表项以及执行完删除操作后应该调用该方法
 * 				从新对id进行从新进行编号，对MAC地址的表项增加ID属性，是为了进行分页时方便处理数据
 * @param root 二阶指针，指向MAC地址的头结点
 * @rerun 指向新的链表的头结点
 * @TODO 该函数可以没有返回值，由于参数是指向头结点的指针，所有对链表的整体结构不会产生影响
**/
struct MacLink ** mofidyMacLinkId(struct MacLink **root){
	unsigned char i = 1;
	struct MacLink *p;
	for(p = *root; p ; p = p->next){
		p->id = i++;
	}
	return root;
}
/**
 * Function: isExistNode
 * Description: 判断节点是否已经存在，如果节点存在那么节点的数据是否已经发生更改，是否需要对原来的数据进行修改
 * @param root 二阶指针，指向MAC地址的头结点
 * @param mac  需要确定的节点信息
 * @param str_mac 被确定的节点的MAC地址
 * @rerun 1，则说明该节点已经存在并且不需要修改
 *		  2，则说明该节点存在并且需要修改
 *        0，则说明节点不存在
 * 
**/
unsigned char isExistNode(struct MacLink *root,struct MacLink *mac,char *str_mac){
	struct MacLink *p;
	int i;
	uint8_t pre_vector = 0x00;
	uint8_t current_vector = 0x00;
	char message[96];
	//printf("will find mac is %s\n",mac->mac_entry.new_mac);
	for( p  = root; p ; p = p->next){
		if( !Str_cmp(p->mac_entry.new_mac,mac->mac_entry.new_mac,6) 
				&& p->mac_entry.vlan_id == mac->mac_entry.vlan_id){ //VLAN id和MAC地址都相同才算存在
				if(p->mac_entry.port_vector == mac->mac_entry.port_vector ){
					//不需要修改
					return 1;
				}else if(p->mac_entry.port_vector != mac->mac_entry.port_vector){
					//节点存在但是数据被修改，修改原来的数据，并返回存在修改标志
					
					for(i = 0; i < ports_num; i++){
						pre_vector = p->mac_entry.port_vector & (0x01 << i);
						current_vector = mac->mac_entry.port_vector & (0x01 << i);
						//printf("pre vector is %02x;cur vector is %02x\n\r",pre_vector,current_vector);
						if(pre_vector != current_vector){
							memset(message,0,sizeof(message));
							if(pre_vector != 0x00){   //之前端口处于可用模式
								sprintf(message,"MAC %s 取消了成员端口 %d",str_mac,i + 1);
								addLogEntry(INFO,"vorx",message);
								printSystemLog();
							}else{
								sprintf(message,"MAC %s 增加了成员端口 %d",str_mac,i + 1);
								addLogEntry(INFO,"vorx",message);
								printSystemLog();
							}
						}
					}
					p->mac_entry.port_vector = mac->mac_entry.port_vector;
					return 2;
				}
		}
	}
	return 0;
}
/* 实现把8bit的mac地址数组转换成MAC地址字符串的形式 */
/**
 * Function: getMacString
 * Description: 把8bit的mac地址数组转换成MAC地址字符串的形式 
 * @param mac_address 转换后的MAC地址字符串数组形式表示
 * @param mac  被转换的MAC地址8bit数组形式表示
 * @rerun 0 程序正常结束
 * 
**/
unsigned char getMacString(unsigned char * mac_address,uint8_t *mac){
	int i;
	unsigned char mac_temp[10];
	//memset(mac_address,0,sizeof(unsigned char) * strlen(mac_address));
	for(i = 0; i < 6; i++){
		memset(mac_temp,0,sizeof(mac));
		if(i != 5)
			sprintf(mac_temp,"%02X-",mac[i]);
			//printf("mac_str is %s\n\r",mac[i]);
		else
			sprintf(mac_temp,"%02X",mac[i]);
		strcat(mac_address,mac_temp);
	}
	//printf("mac_str is %s\n\r",mac_address);
	return 0;
}
/**
 * Function: getMacAddrNum
 * Description: 获取MAC地址的所有数目 
**/
int getMacAddrNum(){
	return mac_addr_num;
}
/**
 * Function: setMacAddrNum
 * Description: 设置MAC地址的所有数目 
**/
void setMacAddrNum(int num){
	mac_addr_num = num;
}
/**
 * Function: emptyMacLink
 * Description: 清空所有的MAC地址链表项
 * @param root 二阶指针，指向MAC地址的头结点
 * @rerun 1，程序正常结束
 * 
**/
int emptyMacLink(struct MacLink **root){
	struct MacLink *p,*q;
	int sucess;
	uint8_t delete_mac_addr[6];
	if(root){
		p = *root;
		while(p){
			if( p->mac_entry.sta_or_dym == ENTRY_STATIC && memcmp(p->mac_entry.new_mac,KEY_MAC,6)){
				p->mac_entry.add_or_delet = 0;
				q = p;
				p = p->next;
				//printf("empty mac address \n\r");
				printfMacAddress(q->mac_entry.new_mac);
				memset(delete_mac_addr,0,sizeof(delete_mac_addr));
				memcpy(delete_mac_addr,q->mac_entry.new_mac,6);
				sucess = Add_or_Delet_MACentry(&q->mac_entry);
				if(sucess){
					//printf("delete static mac address sucessful\n\r");
					deleteMacLinkByVid(root,q->mac_entry.vlan_id,delete_mac_addr);
					deleteBoardMacAddress(q->mac_entry.vlan_id,delete_mac_addr);//删除board中的mac地址项
				}else{
					printf("delete static mac address failed\n'r");
				}
//				free(q);
//				q = NULL;
				//--mac_addr_num;
				
			}else{
				q = p;
				p = p->next;
			}
		}
	}
	return 1;
}
/**
 * Function: printMacLink
 * Description: 打印目前所有的MAC链表中的数据
 * @param root 指向MAC地址的头结点
 * 
 * 
**/
void printMacLink(struct MacLink *root){
	struct MacLink *p;
	unsigned char mac_addr[20];
	//printf("all mac address \n\r");
	for(p = root; p; p = p->next){
		memset(mac_addr,0,sizeof(mac_addr));
		getMacString(mac_addr,p->mac_entry.new_mac);
		//printf("%d;%s\n\r",p->mac_entry.vlan_id,mac_addr);
	}
}
/**
 * Function: isFindVlanIdArray
 * Description: 查找目前vlan ID数组中是否存在某一个ID
 * @param vlan_id 要查找的vlan ID数组
 * @param id      在数组中要查找的ID
 * @return  0   代表不存在
			1   代表存在
 * 
**/
int isFindVlanIdArray(uint16_t *vlan_id,uint16_t id){
	int i;
	for(i = 0; i < MAXMACADDRNUM; i++){
		if(vlan_id[i] == 0){
			break;
		}
		if(vlan_id[i] == id){
			return 1;
		}
	}
	return 0;
}
