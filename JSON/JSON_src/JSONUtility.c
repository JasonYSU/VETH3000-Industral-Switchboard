/*******************************************************
File name: JSONUtility.c
Description: 提供了针对本工程使用JSON的一些工具函数
Author: dujiancheng
Version: v1.0
********************************************************/
#include <stdio.h>
#include <stdlib.h>
#include<string.h>
#include "JSONUtility.h"
#include "board_info.h"
#include "mac.h"
#include "vlan.h"
#include "SerialServer.h"
char *final_JSON_result = NULL;
/**
 * Function: parseJSONConetnt
 * Description:解析用户上传文件的配置信息，流程为先删除原来系统中的设置的所有数据，解析用户的上传的配置文件
 * 				根据内容填充board结构体的内容，根据新的board内容还原系统中的数据
 * @param content  用户上传的配置文件的内容
**/
void parseJSONConetnt(char *content){
	int i,j;
	cJSON *child;
	int array_size;
	int sub_array_size;
	unsigned char hex_mac[6];
	char str_mac[20];
	uint8_t port_tag[PORTSNUM];
	cJSON *sub_strcut;
	cJSON *sub_strcut_child;
	cJSON *root = cJSON_Parse(content); 
	deleteBoardContent();    //删除之前的board中的内容
	/* 读取用户导入的配置文件内容到board中，应该对解析过程进行判断看是否穿的文件是错误的文件内容 */
	board.mac_address.disable_age = cJSON_GetObjectItem(root,"disable_age")->valueint;
	//printf("disable_age is %d\n\r",cJSON_GetObjectItem(root,"disable_age")->valueint);
	
	board.mac_address.age_time = cJSON_GetObjectItem(root,"age_time")->valueint;
	//printf("age_time is %d\n\r",cJSON_GetObjectItem(root,"age_time")->valueint);
	
	board.mac_address.learn_mode = cJSON_GetObjectItem(root,"learn_mode")->valueint;
	//printf("learn_mode is %d\n\r",cJSON_GetObjectItem(root,"learn_mode")->valueint);
	
	sub_strcut =  cJSON_GetObjectItem(root,"mac_address");
	array_size = cJSON_GetArraySize(sub_strcut);
	//printf("mac address size is %d\n\r",array_size);
	for(i = 0; i < array_size; i++){
		memset(hex_mac,0,sizeof(hex_mac));
		memset(str_mac,0,sizeof(str_mac));
		child = cJSON_GetArrayItem(sub_strcut,i);
		strcpy(str_mac,cJSON_GetObjectItem(child,"mac_addr")->valuestring);
//		printf("vlan_id is %d;mac_addr is %s;port_vector is %d;static_dyna is %d\n\r",
//		cJSON_GetObjectItem(child,"vlan_id")->valueint,str_mac,
//		cJSON_GetObjectItem(child,"port_vector")->valueint,cJSON_GetObjectItem(child,"static_dyna")->valueint
//		);
		getDivideMacAddress(hex_mac,str_mac);
//		printf("mac array is \n\r");
//		for(j = 0; j < 6; j++){
//			printf("%02x ",hex_mac[j]);
//		}
//		printf("\n\r");
		/* 添加mac地址到board变量中 */
		addBoardMacAddress(cJSON_GetObjectItem(child,"vlan_id")->valueint,hex_mac,cJSON_GetObjectItem(child,"port_vector")->valueint);
	}
	/* 获取MAC地址相关的信息 */
	
	sub_strcut =  cJSON_GetObjectItem(root,"vlan_member");
	array_size = cJSON_GetArraySize(sub_strcut);
	for(i = 0; i < array_size;i++){
		child = cJSON_GetArrayItem(sub_strcut,i);
//		printf("vlan_id is %d;vlan_name is %s\n\r",cJSON_GetObjectItem(child,"vlan_id")->valueint,cJSON_GetObjectItem(child,"vlan_name")->valuestring);
//		printf("port tag is \n\r");
		sub_strcut_child = cJSON_GetObjectItem(child,"port_tag");
		sub_array_size = cJSON_GetArraySize(sub_strcut_child);
		memset(port_tag,0,sizeof(port_tag));
		for(j = 0; j < sub_array_size;j++){
			//printf("%d ",cJSON_GetArrayItem(sub_strcut_child,j)->valueint);
			port_tag[j] = cJSON_GetArrayItem(sub_strcut_child,j)->valueint;
		}
		//printf("\n\r");
		/* 添加配置文件中的数据到board中 */
		addBoardVlanMember(cJSON_GetObjectItem(child,"vlan_id")->valueint,cJSON_GetObjectItem(child,"vlan_name")->valuestring
							,port_tag);
	}
	
	/* 获取vlan成员相关信息 */
	sub_strcut =  cJSON_GetObjectItem(root,"vlan_port");
	array_size = cJSON_GetArraySize(sub_strcut);
	for(i = 0; i < array_size;i++){
		child = cJSON_GetArrayItem(sub_strcut,i);
//		printf("port_type is %d;filter_option is %d;frame_type is %d;port_id is %d;port_mode is %d\n\r",
//			cJSON_GetObjectItem(child,"port_type")->valueint,cJSON_GetObjectItem(child,"filter_option")->valueint
//			,cJSON_GetObjectItem(child,"frame_type")->valueint,cJSON_GetObjectItem(child,"port_id")->valueint
//			,cJSON_GetObjectItem(child,"port_mode")->valueint
//		);
		/* 添加配置文件中的vlan端口配置到board中去 */
		board.vlan_port[i].port_type = cJSON_GetObjectItem(child,"port_type")->valueint;
		board.vlan_port[i].filter_option = cJSON_GetObjectItem(child,"filter_option")->valueint;
		board.vlan_port[i].frame_type = cJSON_GetObjectItem(child,"frame_type")->valueint;
		board.vlan_port[i].port_id = cJSON_GetObjectItem(child,"port_id")->valueint;
		board.vlan_port[i].port_mode = cJSON_GetObjectItem(child,"port_mode")->valueint;
	}
	
	
	/* 获取vlan端口相关信息 */
	sub_strcut =  cJSON_GetObjectItem(root,"port_flow");
	array_size = cJSON_GetArraySize(sub_strcut);
	//printf("port flow is \n\r");
	for(i = 0; i < array_size;i++){
		//printf("%d ",cJSON_GetArrayItem(sub_strcut,i)->valueint);
		board.ports_config.port_flow[i] = cJSON_GetArrayItem(sub_strcut,i)->valueint;
	}
	//printf("\n\r");
	
	sub_strcut =  cJSON_GetObjectItem(root,"port_auto");
	array_size = cJSON_GetArraySize(sub_strcut);
	//printf("port auto is \n\r");
	for(i = 0; i < array_size;i++){
		//printf("%d ",cJSON_GetArrayItem(sub_strcut,i)->valueint);
		board.ports_config.port_auto[i] = cJSON_GetArrayItem(sub_strcut,i)->valueint;
	}
	//printf("\n\r");
	
	sub_strcut =  cJSON_GetObjectItem(root,"speed_select_enable");
	array_size = cJSON_GetArraySize(sub_strcut);
	//printf("speed select enable is \n\r");
	for(i = 0; i < array_size;i++){
		//printf("%d ",cJSON_GetArrayItem(sub_strcut,i)->valueint);
		board.ports_config.speed_select_enable[i] = cJSON_GetArrayItem(sub_strcut,i)->valueint;
	}
	//printf("\n\r");
	
	
	sub_strcut =  cJSON_GetObjectItem(root,"port_speed");
	array_size = cJSON_GetArraySize(sub_strcut);
	//printf("port speed is \n\r");
	for(i = 0; i < array_size;i++){
		//printf("%d ",cJSON_GetArrayItem(sub_strcut,i)->valueint);
		board.ports_config.port_speed[i] = cJSON_GetArrayItem(sub_strcut,i)->valueint;
	}
	//printf("\n\r");
	
	sub_strcut =  cJSON_GetObjectItem(root,"port_fdx");
	array_size = cJSON_GetArraySize(sub_strcut);
	//printf("port fdx is \n\r");
	for(i = 0; i < array_size;i++){
		//printf("%d ",cJSON_GetArrayItem(sub_strcut,i)->valueint);
		board.ports_config.port_fdx[i] = cJSON_GetArrayItem(sub_strcut,i)->valueint;
	}
	//printf("\n\r");
	
	
	sub_strcut =  cJSON_GetObjectItem(root,"port_nick");
	array_size = cJSON_GetArraySize(sub_strcut);
	//printf("port nick is \n\r");
	for(i = 0; i < array_size;i++){
		//printf("%s ",cJSON_GetArrayItem(sub_strcut,i)->valuestring);
		strcpy(board.ports_config.port_nick[i],cJSON_GetArrayItem(sub_strcut,i)->valuestring);
	}
	//printf("\n\r");
	/*获取端口相关配置,并把信息写到board结构体中*/
	
	//printf("serial1~2_mode is %d\n\r",cJSON_GetObjectItem(root,"serial1~2_mode")->valueint);
	//printf("serial3~4_mode is %d\n\r",cJSON_GetObjectItem(root,"serial3~4_mode")->valueint);
	board.serial_mode.com1_2_mode = cJSON_GetObjectItem(root,"serial1~2_mode")->valueint;
	board.serial_mode.com3_4_mode = cJSON_GetObjectItem(root,"serial3~4_mode")->valueint;
	
	sub_strcut =  cJSON_GetObjectItem(root,"serial_config");
	array_size = cJSON_GetArraySize(sub_strcut);
	for(i = 0; i < array_size;i++){
		child = cJSON_GetArrayItem(sub_strcut,i);
//		printf("serial_no is %d;serial_data_bit is %d;serial_parity is %d;serial_bound_rate is %d;serial_listen_port is %d;serial_stop_bit is %.1f\n\r",
//			cJSON_GetObjectItem(child,"serial_no")->valueint,cJSON_GetObjectItem(child,"serial_data_bit")->valueint
//			,cJSON_GetObjectItem(child,"serial_parity")->valueint,cJSON_GetObjectItem(child,"serial_bound_rate")->valueint
//			,cJSON_GetObjectItem(child,"serial_listen_port")->valueint,cJSON_GetObjectItem(child,"serial_stop_bit")->valuedouble
//		);
		board.serial_config[i].serial_no = cJSON_GetObjectItem(child,"serial_no")->valueint;
		board.serial_config[i].serial_data_bit = cJSON_GetObjectItem(child,"serial_data_bit")->valueint;
		board.serial_config[i].serial_parity = cJSON_GetObjectItem(child,"serial_parity")->valueint;
		board.serial_config[i].serial_bound_rate = cJSON_GetObjectItem(child,"serial_bound_rate")->valueint;
		board.serial_config[i].serial_listen_port = cJSON_GetObjectItem(child,"serial_listen_port")->valueint;
		board.serial_config[i].serial_stop_bit = cJSON_GetObjectItem(child,"serial_stop_bit")->valuedouble;
	}
	/* 获取串口相关配置 */
	
	sub_strcut =  cJSON_GetObjectItem(root,"serial_remote_host");
	array_size = cJSON_GetArraySize(sub_strcut);
	for(i = 0; i < array_size;i++){
		child = cJSON_GetArrayItem(sub_strcut,i);
//		printf("remote_ip is %s;remote_port is %d;serial_no is %d;is_use_udp is %d\n\r",
//			cJSON_GetObjectItem(child,"remote_ip")->valuestring,cJSON_GetObjectItem(child,"remote_port")->valueint
//			,cJSON_GetObjectItem(child,"serial_no")->valueint,cJSON_GetObjectItem(child,"is_use_udp")->valueint
//		);
		strcpy(board.serial_remote_host[i].remote_ip,cJSON_GetObjectItem(child,"remote_ip")->valuestring);
		board.serial_remote_host[i].remote_port = cJSON_GetObjectItem(child,"remote_port")->valueint;
		board.serial_remote_host[i].serial_no = cJSON_GetObjectItem(child,"serial_no")->valueint;
		board.serial_remote_host[i].is_use_udp = cJSON_GetObjectItem(child,"is_use_udp")->valueint;
	}
	/* 获取远程主机的配置 */
	
	generateBoardContent();	
	/* 恢复系统中的所有数据 */
	
	
	/*释放JSON开辟的空间*/
	cJSON_Delete(root);
}
/**
 * Function: deleteBoardContent
 * Description:删除之前board中存放的数据内容
**/
uint8_t deleteBoardContent(){
	isFirstRequest = TRUE;
	isFirstRequestPorts = TRUE;
	isFirstRequestVlanPorts = TRUE;
	isRecoverMacConfig = TRUE;
	/* 设置为第一次对mac表，ports配置，vlan成员,vlan端口配置请求 */
	//Flush_MAC_entry(); //清空交换芯片中的动态mac表
	//emptyMacLink(mac_root); //清空mac地址的数据链表
	Flush_MAC_entry();
	deleteDymMacEntry(&mac_root);
	
	emptyMacLink(&mac_root); //清空静态的mac地址
	
	/* 删除mac所有的地址项 */
	emptyVlanLink(vlan_root); //清空VLAN的数据链表
	vlan_root = NULL;
	
	return 1;
}

/**
 * Function: generateBoardContent
 * Description:根据用户导入的配置文件重新生成board结构体的内容
**/
unsigned char generateBoardContent(){
	initPorts();//初始化端口配置
	initFirstVlan();  //初始化VLAN配置以及VLAN链表
	initRoot(); //初始化mac地址配置以及mac链表
	initVlanPorts();  //初始化VLAN端口配置
	initSerialInfo();    //初始化串口参数配置
	initDefaultSerialPort();  //初始化串口默认的监听端口号
}
/**
 * Function: generateConfigJSON
 * Description:把board结构体里面的数据生成JSON数据文件格式发送给浏览器
 * @return 生成的JSON格式的数据文件
**/
char* generateConfigJSON(){

	char *JSON_result;
	int i;
	unsigned char mac_address_string[20];
	cJSON *root,*mac_address,*mac_fld;
	cJSON *vlan_member,*vlan_fld;
	cJSON *vlan_port,*vlan_port_fld;
	cJSON *serial_config,*serial_config_fld;
	cJSON *serial_remote_config,*serial_remote_config_fld;
	root=cJSON_CreateObject();	
	cJSON_AddItemToObject(root, "disable_age", cJSON_CreateNumber(board.mac_address.disable_age));
	cJSON_AddItemToObject(root, "age_time", cJSON_CreateNumber(board.mac_address.age_time));
	cJSON_AddItemToObject(root, "learn_mode", cJSON_CreateNumber(board.mac_address.learn_mode));
	cJSON_AddItemToObject(root, "mac_address", mac_address=cJSON_CreateArray());
	for(i = 0; i < MAXMACADDRNUM; i++){
		if(board.mac_address.mac_table[i].end == 1){
			break;
		}
		memset(mac_address_string,0,sizeof(mac_address_string));
		getMacString(mac_address_string,board.mac_address.mac_table[i].mac_addr);
		cJSON_AddItemToArray(mac_address,mac_fld=cJSON_CreateObject());
		cJSON_AddNumberToObject(mac_fld,"vlan_id",board.mac_address.mac_table[i].vlan_id);
		cJSON_AddStringToObject(mac_fld,"mac_addr",(char*)mac_address_string);
		cJSON_AddNumberToObject(mac_fld,"port_vector",board.mac_address.mac_table[i].port_vector);
		cJSON_AddNumberToObject(mac_fld,"static_dyna",board.mac_address.mac_table[i].static_dyna);
	}
	/* 生成配置的静态mac地址的JSON数据 */
	cJSON_AddItemToObject(root, "vlan_member", vlan_member=cJSON_CreateArray());
	for(i = 0; i < MAXVLANMEMBERNUM; i++){
		if(board.vlan_member[i].end == 1){
			break;
		}
		cJSON_AddItemToArray(vlan_member,vlan_fld=cJSON_CreateObject());
		cJSON_AddNumberToObject(vlan_fld,"vlan_id",board.vlan_member[i].vlan_id);
		cJSON_AddStringToObject(vlan_fld,"vlan_name",(char*)board.vlan_member[i].vlan_name);
		cJSON_AddUint8NumberArrayToObject(vlan_fld,"port_tag",board.vlan_member[i].port_tag,ports_num);
	}
	/* 生成配置的vlan成员的JSON数据 */
	cJSON_AddItemToObject(root, "vlan_port", vlan_port=cJSON_CreateArray());
	for(i = 0; i < ports_num; i++){
		cJSON_AddItemToArray(vlan_port,vlan_port_fld = cJSON_CreateObject());
		cJSON_AddNumberToObject(vlan_port_fld,"port_type",board.vlan_port[i].port_type);
		cJSON_AddNumberToObject(vlan_port_fld,"filter_option",board.vlan_port[i].filter_option);
		cJSON_AddNumberToObject(vlan_port_fld,"frame_type",board.vlan_port[i].frame_type);
		cJSON_AddNumberToObject(vlan_port_fld,"port_id",board.vlan_port[i].port_id);
		cJSON_AddNumberToObject(vlan_port_fld,"port_mode",board.vlan_port[i].port_mode);
	}
	/* 生成配置的vlan端口的JSON数据 */
	
	cJSON_AddNumberArrayToObject(root, "port_flow", board.ports_config.port_flow,ports_num);
	cJSON_AddNumberArrayToObject(root, "port_auto", board.ports_config.port_auto,ports_num);
	cJSON_AddNumberArrayToObject(root, "speed_select_enable", board.ports_config.speed_select_enable,ports_num);
	cJSON_AddNumberArrayToObject(root, "port_speed", board.ports_config.port_speed,ports_num);
	cJSON_AddNumberArrayToObject(root, "port_fdx", board.ports_config.port_fdx,ports_num);
	//printf("nick is %s\n\r",board.ports_config.port_nick[1]);
	cJSON_AddStringArrayToObject(root,"port_nick",board.ports_config.port_nick,ports_num);
	/* 生成端口配置的JSON数据 */
	cJSON_AddNumberToObject(root,"serial1~2_mode",board.serial_mode.com1_2_mode);
	cJSON_AddNumberToObject(root,"serial3~4_mode",board.serial_mode.com3_4_mode);
	/*生成串口模式配置的JSON数据*/
	cJSON_AddItemToObject(root, "serial_config", serial_config=cJSON_CreateArray());
	for(i = 0; i < MAX_SERIAL_NUM; i++){
		cJSON_AddItemToArray(serial_config,serial_config_fld = cJSON_CreateObject());
		cJSON_AddNumberToObject(serial_config_fld,"serial_no",board.serial_config[i].serial_no);
		cJSON_AddNumberToObject(serial_config_fld,"serial_data_bit",board.serial_config[i].serial_data_bit);
		cJSON_AddNumberToObject(serial_config_fld,"serial_parity",board.serial_config[i].serial_parity);
		cJSON_AddNumberToObject(serial_config_fld,"serial_bound_rate",board.serial_config[i].serial_bound_rate);
		cJSON_AddNumberToObject(serial_config_fld,"serial_listen_port",board.serial_config[i].serial_listen_port);
		cJSON_AddNumberToObject(serial_config_fld,"serial_stop_bit",board.serial_config[i].serial_stop_bit);
	}
	/*生成串口本地配置的JSON数据*/
	cJSON_AddItemToObject(root, "serial_remote_host", serial_remote_config=cJSON_CreateArray());
	for(i = 0; i < MAX_SERIAL_NUM;i++){
		if(board.serial_remote_host[i].remote_port != 0 && board.serial_remote_host[i].remote_ip != NULL){
			cJSON_AddItemToArray(serial_remote_config,serial_remote_config_fld = cJSON_CreateObject());
			cJSON_AddStringToObject(serial_remote_config_fld,"remote_ip",board.serial_remote_host[i].remote_ip);
			cJSON_AddNumberToObject(serial_remote_config_fld,"remote_port",board.serial_remote_host[i].remote_port);
			cJSON_AddNumberToObject(serial_remote_config_fld,"serial_no",board.serial_remote_host[i].serial_no);
			cJSON_AddNumberToObject(serial_remote_config_fld,"is_use_udp",board.serial_remote_host[i].is_use_udp);
		}
	}
	
	/* 生成远程主机配置的信息 */
	JSON_result=cJSON_Print(root);
	final_JSON_result = (char*)malloc(strlen(JSON_result) + 1);
	if(final_JSON_result == NULL){
		printf("final json malloc failed\n\r");
		return NULL;
	}
	strcpy(final_JSON_result,JSON_result);
	cJSON_Delete(root);
	free(JSON_result);
	return final_JSON_result;
}
