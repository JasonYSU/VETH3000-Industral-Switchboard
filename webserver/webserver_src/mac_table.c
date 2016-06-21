/*******************************************************
File name: mac_table.c
Description: 从MAC地址链表中查找符合条件的MAC地址，封装成发送数据的格式，发送给用户
Author: dujiancheng
Version: v1.0
********************************************************/
#include "mac_table.h"
#include "mac_link.h"
#include "mac.h"
char *mac_addr[100];
char hex[] = {'a','b','c','d','e','f'};
char getRandomChar(){
	int r;
	r = rand() % 16; 
	return r < 10 ? r + '0' : hex[r % 10];
}
/**
 * Function: compareMacAddr
 * Description: 对字符串数组进行排序时的比较函数
 *
**/
int compareMacAddr(const void *s1,const void *s2){
	return strcmp(*(char**)s1,*(char**)s2);
}
/**
 * Function: getDyanMacTable
 * Description: 获取MAC地址的相关信息，以发送数据的格式返回给用户
 * @param start_vid  开始的查找的vlan ID
 * @param start_macAddr 开始的MAC地址
 * @param total_entry  用户要查询的总项数
 * @param flag   用户查询的标志
 * @return 封装后要发送的数据
**/
char* getDyanMacTable(int start_vid,char *start_macAddr,int total_entry,int flag){
	int i;
	int found_num = 0;
	int first_id;
	int last_id;
	static char all_data[4096];
	char single_entry[100];
	char header[60];
	char first_last[10];
	struct MacLink *p;
	unsigned char mac_address[20];
	unsigned char mac[10];
	int *ports_vector;
	//initMacTableEntry();
	initRoot();//初始化root节点
	ports_vector = (int*)malloc(ports_num * sizeof(int));
	memset(all_data,0,sizeof(all_data));
	all_data[0] = 0;
	single_entry[0] = 0;
	//printf("vlan_id is %d;mac_addr is %s\n\r",start_vid,start_macAddr);
	memset(first_last,0,sizeof(first_last));
	sprintf(header,"%d/%s|",total_entry,start_macAddr);
	strcat(all_data,header);
	for(p = mac_root; p ; p = p->next){
		memset(mac_address,0,sizeof(mac_address));
		getMacString(mac_address,p->mac_entry.new_mac);
		
		memset(ports_vector,0,ports_num * sizeof(int));
		getPortsVectorArray(p->mac_entry.port_vector,ports_vector);
		//printf("all mac address is %s\n\r",mac_address);
		//根据flag的值来决定比较条件
//		if( (p->mac_entry.vlan_id >= start_vid) || (flag ? strcmp((char*)mac_address,start_macAddr) > 0 
//			: strcmp((char*)mac_address,start_macAddr) >= 0)
//			){
		//printf("current vlan_id is %d\n\r",p->mac_entry.vlan_id);
		if( p->mac_entry.vlan_id == start_vid ){  //现在VLAN id相同的区域里面找
		//if(p->mac_entry.vlan_id >= start_vid){
			if( strcmp((char*)mac_address,start_macAddr) < 0){
				continue;
			}
		}else if( p->mac_entry.vlan_id < start_vid ){  //找VLAN id比当前大的项
			continue;
		}
			if(found_num == 0){  //找到第一个项
				first_id = p->id;
			}
			sprintf(single_entry,"%s/%d/%d/%d/",
					mac_address,p->mac_entry.vlan_id,
					p->mac_entry.sta_or_dym,p->mac_entry.port_vector & 0x01 << 10 ? 1 : 0 );
			concatVectorString(single_entry,ports_vector);
			strcat(all_data,single_entry);
			++found_num;
			last_id = p->id;
		//}
		if(found_num == total_entry){
			break;
		}
	}
	if(single_entry[0] == 0 ){
		sprintf(single_entry,"NoEntries/-/-/-|");
		strcat(all_data,single_entry);
		first_id = 1;
		last_id = mac_addr_num;
	}
	first_id = (first_id == 1) ? 1 : 0; //判断第一项的前面是否还有数据
	last_id = (last_id == mac_addr_num) ? 1 : 0; //判断最后一项的后面是否还有数据
	/* first_id 为1说明前面无数据，last_id为1说明后面无数据 */
	//printf("first is %d;last id is %d\n\r",first_id,last_id);
	sprintf(first_last,"%d/%d|",first_id,last_id); //
	strcat(all_data,first_last);
//	printf("total entry is %d\n\r",mac_addr_num);
//	printf("find entries is %s\n\r",all_data);
	free(ports_vector);
	ports_vector = NULL;
	return all_data;
}
/**
 * Function: getPrePageDynamicMac
 * Description: 得到上一页的数据
 * @param start_vid  开始查找的vlan ID
 * @param start_macAddr 开始的MAC地址
 * @param total_entry  用户要查询的总项数
 * @return 封装后要发送的数据
**/
char* getPrePageDynamicMac(int start_vid,char *start_macAddr,int total_entry){
	int begin_id = 0;
	int end_id = 0;
	int found_num = 0;
	static char all_data[4096];
	char single_entry[100];
	char header[60];
	char first_last[10];
	struct MacLink *p;
	unsigned char mac_address[20];
	unsigned char mac[10];
	int *ports_vector;
	/*初始化root节点,可以不用调用，调用该方法的时候说明已经进入到页面中，应经调用过
	**初始化方法，但是初始化中会进行判断，调用不会有影响
	*/
	initRoot();
	memset(all_data,0,sizeof(all_data));
	all_data[0] = 0;
	single_entry[0] = 0;
	memset(first_last,0,sizeof(first_last));
	ports_vector = (int*)malloc(ports_num * sizeof(int));
	sprintf(header,"%d/%s|",total_entry,start_macAddr);
	strcat(all_data,header);
	/* 查找要查找的mac地址对应的id 也就是查找结束的id */
	for(p = mac_root; p ; p = p->next){
		memset(mac_address,0,sizeof(mac_address));
		getMacString(mac_address,p->mac_entry.new_mac);
		if( !strcmp((char*)mac_address,start_macAddr)){
			end_id = p->id;
			break;
		}
	}
	/* 根据end_id计算出begin_id，要判定前面的项数是否满足要查找的总项数 */
	begin_id = end_id - total_entry;
	if(begin_id <= 0){  //不可能为0
		begin_id = 1;
	}
	//printf("begin id is %d;end id is %d;total entry is %d\n\r",begin_id,end_id,mac_addr_num);
	for(p = mac_root; p ; p = p->next){
		if( p->id >= begin_id && p->id < end_id){
			//printf("id is %d\n\r",p->id);
			memset(mac_address,0,sizeof(mac_address));
			getMacString(mac_address,p->mac_entry.new_mac);
			memset(ports_vector,0,ports_num * sizeof(int));
			getPortsVectorArray(p->mac_entry.port_vector,ports_vector);
			
			sprintf(single_entry,"%s/%d/%d/%d/",mac_address,
					p->mac_entry.vlan_id,p->mac_entry.sta_or_dym, p->mac_entry.port_vector & 0x01 << 10 ? 1 : 0 );
			concatVectorString(single_entry,ports_vector);
			strcat(all_data,single_entry);
			++found_num;
		}
		if(found_num == total_entry){
			break;
		}
	}
	if(single_entry[0] == 0 ){
		sprintf(single_entry,"NoEntries/-/-/-|");
		strcat(all_data,single_entry);
		begin_id = 1;
		end_id = mac_addr_num;
	}
	begin_id = (begin_id == 1) ? 1 : 0; //判断第一项的前面是否还有数据
	end_id = (end_id == mac_addr_num) ? 1 : 0; //判断最后一项的后面是否还有数据
	/* first_id 为1说明前面无数据，last_id为1说明后面无数据 */
	//printf("first is %d;last id is %d\n\r",begin_id,end_id);
	sprintf(first_last,"%d/%d|",begin_id,end_id); //
	strcat(all_data,first_last);
	free(ports_vector);
	ports_vector = NULL;
	return all_data;
}

