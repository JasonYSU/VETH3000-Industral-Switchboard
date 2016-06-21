/*******************************************************
File name: system_maintain.c
Description: 提供系统维护相关的接口函数，提供的操作有系统重启，恢复系统出厂设置，用户登出处理以及保存用户所有配置等
Author: dujiancheng
Version: v1.0
********************************************************/
#include "system_maintain.h"
#include "ip_config.h"
#include "httpd.h"
#include "upload_file.h"
#include "FindSubstring.h"
#include "JSONUtility.h"
#include "system_info.h"
//#include "LogInfo.h"
#include "LogUtility.h"
#include "Session.h"
#include "tcp.h"
#define BASE64BUFSIZE (2 * 1024 * 1024) //2M
char data[100];
uint8_t save_all_config_result[16];   //存放保存所有配置的结果
REMOTE_SOCKET remote_serial_server_socket[MAX_SERIAL_REMOTE_NUM];  //初始化远端host端口号
int isRestartDevice = FALSE;
uint8_t network_update_sucess = 2;  //网络升级是否成功，如果是2则说明没有进行网络升级
int url_buf_length; //记录url_buf缓存中实际数据的长度
char pre_compile_date[30];  //记录上一次的编译日期
/**
 * Function: recoverFactorySet
 * Description: 恢复设备的出厂设置
 * @return 成功恢复出厂设置之后向浏览器返回的数据
**/
char* recoverFactorySet(){
	uint8_t src_ipaddr[4]; //保存之前的用户配置的IP地址
	uint8_t src_route_addr[4]; //保存之前用户配置的网关地址
	uint8_t src_net_mask[4]; //保存之前用户配置的子网掩码
	uint8_t is_network_update;
	memcpy(src_ipaddr,board.ip_config.ip_addr,sizeof(src_ipaddr));
	memcpy(src_route_addr,board.ip_config.route_addr,sizeof(src_route_addr));
	memcpy(src_net_mask,board.ip_config.net_mask,sizeof(src_net_mask));
	is_network_update = board.is_network_update;
	/* 保存用户之前对IP地址的相关配置，恢复出厂设置时，不对IP地址进行恢复 */
	
	initBoardInfo();  //初始化系统之前的所有配置
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
	/* 清空缓存中的数据链表 */
	initBoardInfo();
	updateInvariate(&board,sizeof(board));
	initPorts();//初始化端口配置
	initFirstVlan();  //初始化VLAN配置以及VLAN链表
	
	initRoot(); //初始化mac地址配置以及mac链表
	initVlanPorts();  //初始化VLAN端口配置
	//initSerialInfo();    //初始化串口参数配置
	//initDefaultSerialPort();  //初始化串口默认的监听端口号
	
	memcpy(board.ip_config.ip_addr,src_ipaddr,sizeof(board.ip_config.ip_addr));
	memcpy(board.ip_config.route_addr,src_route_addr,sizeof(board.ip_config.route_addr));
	memcpy(board.ip_config.net_mask,src_net_mask,sizeof(board.ip_config.net_mask));
	/* 把用户之前配置的IP地址从新保存到board结构体中 */
	board.is_network_update = is_network_update;    //恢复之前的网络升级时的参数
	//setHostAddress(&board.ip_config);  //初始化IP地址配置
	
	/* 下面的操作模拟了系统重启后变量的值 */
	//deleteAllLogInfo(); //清空所有的日志项
	is_reboot = 1; //置重启系统标志，重新生成相关日志
	network_update_sucess = 2;  //设置此标志和系统重启后的标志一样，通过网络恢复出厂设置和升级不可能同时进行
	recond_system_reboot_time_out = sCount;
	return "1|";
}
/**
 * Function: generateConfigSaveResult
 * Description: 判断用户的配置是否保存到了Flash中，把判断的结果格式化以后返回给浏览器
 * @return 向浏览器返回的判断信息
**/
char *generateConfigSaveResult(){
	memset(&board_temp,0,sizeof(board_temp));
	getInvariate(&board_temp,sizeof(board_temp)); //得到之前flash中的数据
	if(calculateChecksum(&board_temp,sizeof(board_temp),18)
		!= calculateChecksum(&board,sizeof(board),18)){ //判断两个board的检验和是否相同
		return "0|";   //没有进行保存
	}else{
		return "1|";   //已经进行了保存
	}
}
/**
 * Function: deviceRestart
 * Description: 设备从新启动的接口函数
 * 
**/
char* deviceRestart(){
	isRestartDevice = TRUE;
	NVIC_SystemReset();
	return "1|";
}
/**
 * Function: getDeviceRestartStatus
 * Description: 获取设备的重启状态，设备是否重启成功，把结果返回给浏览器
 * @return 设备重启是否成功的结果
**/
char *getDeviceRestartStatus(){
	//printf("exec status function\n\r");
	if(isRestartDevice){
		isRestartDevice = FALSE;
		return "1|";
	}else{
		return "0|";
	}
	
}
/**
 * Function: saveAllConfigToFalsh
 * Description: 保存所有的配置信息到Flash中
 * @param url 目前暂时没有使用
 * @return 1 成功保存
**/
int saveAllConfigToFalsh(char *url){
	if(updateInvariate(&board,sizeof(board))){//向Flash中写入数据
		memset(save_all_config_result,0,sizeof(save_all_config_result));
		//printf("save all config to falsh sucessful\n\r");
		strcpy((char*)save_all_config_result,"sucessful");
	} else{
		memset(save_all_config_result,0,sizeof(save_all_config_result));
		//printf("save all config to falsh failed\n\r");
		strcpy((char*)save_all_config_result,"failed");
	}
	return 1;
}
/**
 * Function: setRestartDeviceFlag
 * Description: 设置设备重新启动的标志
**/
void setRestartDeviceFlag(){
	isRestartDevice = TRUE;
}
/**
 * Function: processLogout
 * Description: 响应用户登出请求，当用户请求登出时该函数将被调用。函数对登出的处理是删除保存的用户session信息
**/
void processLogout(){
	struct session_node *node;
	char *session_key;
	struct tcp_pcb *pcb;
    char *result = NULL;
	pcb = current_http_connection->pcb;
	session_key = getSessionKey("login",
								current_http_connection->browser_info,
								current_http_connection->pcb->remote_ip.addr
								);
	node = (struct session_node *)malloc(sizeof(struct session_node));
	node->key = "login";
	node->session_key = session_key;
	/*删除session hash table数据*/
	deleteSession(session_key);
	/* 删除session的链表的数据 */
	deleteSessionNode(&session_root,node);
	node->key = NULL;
	node->session_key = NULL;
    free(session_key);
	free(node);
    session_key = NULL;
	node = NULL;
}

/**
 * Function: saveSystemImage
 * Description: 函数用于上传文件的接口函数，上传文件包括了镜像升级以及配置文件的上传
 *              用户通过网络进行系统升级或者上传配置文件时该函数将被调用。
 *
 * @param url 用户提交的上传文件的表单数据，该数据可能是镜像文件的数据也可能是配置文件的数据
**/
int saveSystemImage(char *url){
	char *file_name = NULL;
	char *file_content = NULL;
	char *boundary = NULL;
	char *begin_boundary = NULL;
	char *content_type_value = NULL;
	char *form_data = NULL;
	char *boundary_print = NULL;
	int file_size;
	int isExist = 0;
	int form_loc = 0;
	double  file_length;
	content_type_value = getHttpHeaderParameter(url,"Content-Type");
	begin_boundary = strstr(content_type_value,BOUNDARY_KWD);
	if(begin_boundary){
		begin_boundary += strlen(BOUNDARY_KWD);
	}
	boundary = (char*)malloc((strlen(begin_boundary) + 2) * sizeof(char));
	if( !boundary){
		printf("boundary malloc memory failed\n\r");
	}
	sprintf(boundary,"--%s",begin_boundary);
	freeMemory(content_type_value);
	form_loc = findSubstring(url,boundary,url_buf_length,0);
	if(form_loc < 0){
		printf("can't find form data start loc\n\r");
	}
	form_data = &url[form_loc];
	file_name = getUploadFileName(form_data,boundary);
	//printf("get file name is %s\n\r",file_name);
	//判断用户上传的文件是否是需要的镜像文件
	if( strstr(file_name,".vImage")){
		//if(isExist){
		file_content = getUploadFileContent(form_data,boundary,&file_size);
		//printf("\n\rfile size is %.2lfKB(%.2lfbytes)\n\r",(double)file_size / 1024,(double)file_size);
		memcpy(recieved_package.temp_buff,file_content,file_size);
		recieved_package.length = file_size;
		recieved_package.checksum = calculateChecksum(file_content,file_size,0);
		freeMemory(file_content);
		freeMemory(url);
		board.is_network_update = 1;  //有网络升级请求时设置该标志，更新Flash。
		if(updateInvariate(&board,sizeof(board))){  //更新Flash成功
			printf("update Flash successful:network update\n\r");
		}else{   //更新Flash失败
			printf("update Falsh failed:network update\n\r");
		}
		/* 网络升级前先保存Flash，记录网络升级标志 */
		saveNetData(&recieved_package);
		//}
	}else if( strstr(file_name,".vconfig")){
		//printf("get config file\n\r");
		file_content = getUploadFileContent(form_data,boundary,&file_size);
		printf("\n\rfile size is %.2lfKB(%.2lfbytes)\n\r",(double)file_size / 1024,(double)file_size);
		parseJSONConetnt(file_content);
		freeMemory(file_content);
		freeMemory(url);
	}
	freeMemory(file_name);
	freeMemory(boundary);
}
/**
 * Function: generateSystemUpdateStatus
 * Description: 生成系统升级后的升级结果，并把该结果返回给浏览器，以便向用户显示是否升级成功
 *         
 * @return 网络升级后的状态，升级成功返回当前的编译日期和OK标志否则返回失败的标志
**/
char* generateSystemUpdateStatus(){
	char system_compile_date[30];
	static char result[32]; 
	memset(result,0,sizeof(result));
	
	generateSystemCompileDate(system_compile_date);
	
	//printf("borad compile date is %s;system compile date is %s\n\r",board.compile_date,system_compile_date);
	if( !memcmp(pre_compile_date,system_compile_date,strlen(system_compile_date))){ //两次的版本一样，升级失败
		printf("network update failed\n\r");
		sprintf(result,"0|%s|",board.compile_date);  //升级失败返回当前的编译日期
		network_update_sucess = 0;
		return result;
	}else{  //升级成功，返回状态OK，并且更改board中编译日期
		memcpy(board.compile_date,system_compile_date,sizeof(system_compile_date));
		sprintf(result,"1|%s|",board.compile_date);  //升级成功返回当前的编译日期和OK标志
		network_update_sucess = 1;
		return result;
	}
}
