/*******************************************************
File name: httpd-cgi.c
Description: 类似于web服务器的CGI接口，根据用户的请求生成相应的数据，返回给用户
Author: dujiancheng
Version: v1.0
********************************************************/
#include "httpd.h"
#include "httpd-cgi.h"
//#include "httpd-fs.h"
#include "vlan.h"
#include "veth_common.h"
#include "ports.h"
#include <stdio.h>
#include <string.h>
#include "system_maintain.h"
#include "system_info.h"
#include "request.h"
#include "ports.h"
#include "fs.h"
#include "ip_config.h"
#include "mac.h"
#include "mac_table.h"
#include "SerialUtility.h"
#include "user.h"
#include "JSONUtility.h"
#include "LogUtility.h"
#define ISO_and 0x26
#define ISO_equal 0x3d
#define ISO_null 0x30
#define MAXPORTNUM 10
extern struct VlanLink *root;
uint8_t is_file_download;     //是否是文件下载请求
char port_mode[] = {'A','A','A','A','A','A','A','A','A','A','A','A','\0'};
char change_status[] = {'1','1','1','1','1','1','1','1','1','1','1','1','\0'};

/*---------------------------------------------------------------------------*/
/**
 * Function: generateDataFile
 * Description: 生成数据文件，如果请求的是数据文件该函数将会被调用，对于AJAX请求的话改函数将会被调用
 * @param file 服务器要发送的文件内容
 * @param url  用户请求的数据内容
**/
void generateDataFile(struct fs_file *file,char *url){
	MAC_ENTRY_T entryx;
	MAC_API_T  api_entry;
	int i = 0;
	int port_no;
	char *ptr = strchr(url,'?');
	unsigned char *status;
	char *clearParam = NULL;
	char *portNo = NULL;
	char *countType = NULL;
	char *band_port_no = NULL;
	char *nextAddr = NULL;
	char *numberEntry = NULL;
	char *startVid = NULL;
	char *nextEntry = NULL;
	char *flush = NULL;
	int startMacVid;
	char *vlanBeginId = NULL;
	char *vlanNumberEntry = NULL;
	char father[20],child[30];
	char *serial_no = NULL; 
	char *start_log_id = NULL;  //请求日志起始ID
	char *log_total_entry = NULL; //请求日志的总项数
	//char *next_log_id = NULL;      //
	char *log_request_type = NULL;   //请求的类型是获取上一页还是下一页
	//printf("url is %s;ptr is %s\n\r",url,ptr);
	if(ptr){  //请求文件名带有参数
		
		memset(father,0,sizeof(father));
		memset(child,0,sizeof(child));
		getFileName(url,father,child);
		clearParam = getParameter(ptr,"clear");  //判断是否是清除请求
		portNo = getParameter(ptr,"port");
		countType = getParameter(ptr,"count");
		band_port_no = getParameter(ptr,"port_no");
		serial_no = getParameter(ptr,"serial_no");
		if( !strcmp(father,"stat")){    //请求stat目录下的文件
			if( !strcmp(child,"ports")){   //请求ports文件相应的信息
				if( !strcmp(clearParam,"1")){       //请求posts文件中的clear选项
					Flush_ALL_counters();
					/* 清除之前存储的发送和接收的数据 */
					clearPreBandWidth();  
					status = (unsigned char*)generateCountData();
				}
			}else if( !strcmp(child,"port") ){ // 请求port文件其中一个端口信息
				if( clearParam != NULL){    //请求清除某个端口详细信息
					if( !strcmp(clearParam,"1")){
						if(portNo){
							//printf("execute clear!\n");
							//clearPortDataFile(s,portNo);
							//printf("execute clear end!\n");
							Flush_Port_counters(atoi(portNo) - 1);
							status = (unsigned char*)getPortDetailData(atoi(portNo));
						}
					}
				}else{
					if(portNo && !countType){   //调用函数生成某一个端口信息
						status = (unsigned char*)getPortDetailData(atoi(portNo));
					}else if(portNo && countType){
						changeStatsMode(atoi(countType));
						status = (unsigned char*)getPortDetailData(atoi(portNo));
						//generatePortDataFile(file,portNo);
					}
				}
			}else if( !strcmp(child,"vlan_membership_stat")){  //请求vlan的状态
				startVid = getParameter(ptr,"DynStartVid");
				numberEntry = getParameter(ptr,"DynNumberOfEntries");
				status = (unsigned char*)generateVlanStatus(atoi(startVid),atoi(numberEntry));
			 
			}else if( !strcmp(child,"bandwidth")){
				status = (unsigned char*)generateBandWidth(atoi(band_port_no) - 1);
			}else if(!strcmp(child,"system_log")){
				start_log_id = getParameter(ptr,"GetNextLogEntry");
				log_total_entry = getParameter(ptr,"DynNumberOfEntries");
				log_request_type = getParameter(ptr,"flag");
				if( !strcmp(log_request_type,"0") ){  //请求刷新
					/* 获取大于id的 log_total_entry个数据*/
					status = getSystemLogEntry(atoi(start_log_id),atoi(log_total_entry),0);
				}else if(!strcmp(log_request_type,"1")){  //请求下一页的数据
					status = getSystemLogEntry(atoi(start_log_id),atoi(log_total_entry),1);
				}else if(!strcmp(log_request_type,"2")){
					/* 获取小于id的 log_total_entry个数据*/
					status = getPreSystemLogEntry(atoi(start_log_id),atoi(log_total_entry));
				}
			}
				//end stat目录请求
		}else if( !strcmp(father,"config")){  //请求config目录下的文件
			if( !strcmp(child,"dynamic_mac_table")){  //请求动态mac表对应的信息
				flush = getParameter(ptr,"Flush");
				//printf("request dyna table!\n");
				if( !flush){   //不是清除请求
					//printf("not flush\n%file\n",ptr);
					nextAddr = getParameter(ptr,"DynGetNextAddr");
					numberEntry = getParameter(ptr,"DynNumberOfEntries");
					startVid   = getParameter(ptr,"DynStartVid");
					nextEntry  = getParameter(ptr,"GetNextEntry");
					startMacVid = atoi(startVid);
					if( !strcmp(nextEntry,"1")){
						status = (unsigned char*)getDyanMacTable(startMacVid,nextAddr,atoi(numberEntry),1);
						currentTotalPage += atoi(numberEntry);  
					}else if( !strcmp(nextEntry,"0")){
					  //处理刷新请求，把链表中动态表项删除
						//printf("process flush request\n\r");
						deleteDymMacEntry(&mac_root);
						isFirstRequest = TRUE;
						currentTotalPage = atoi(numberEntry);  
						status = (unsigned char*)getDyanMacTable(startMacVid,nextAddr,atoi(numberEntry),1);
					}else if(!strcmp(nextEntry,"2")){
						status = (unsigned char*)getPrePageDynamicMac(startMacVid,nextAddr,atoi(numberEntry));
					}
					//status = getDyanMacTable(currentTotalPage,atoi(startVid),nextAddr,atoi(numberEntry));
				}else{ //处理清除请求
					Flush_MAC_entry();
					deleteDymMacEntry(&mac_root);
					isFirstRequest = TRUE;
					//currentTotalPage = atoi(numberEntry);  
					status = (unsigned char*)getDyanMacTable(startMacVid,"00-00-00-00-00-00",20,0);
					//status = getDyanMacTable(currentTotalPage,atoi(startVid),nextAddr,atoi(numberEntry));
					
				}
			}else if( !strcmp(child,"vlan")){ //请求vlan数据文件
				initFirstVlan();
				vlanBeginId = getParameter(ptr,"DynStartVid");
				vlanNumberEntry = getParameter(ptr,"DynNumberOfEntries");
				status = (unsigned char*)generateVlanData(atoi(vlanBeginId),atoi(vlanNumberEntry));	

			}else if( !strcmp(child,"serial_config")){ //串口配置文件请求
				status = (unsigned char*)getSerialConfig((atoi(serial_no) - 1));
			} 
		}//结束config目录文件请求的处理
		
	}else{    //请求的文件不带参数，直接生成相应的文件
		memset(father,0,sizeof(father));
		memset(child,0,sizeof(child));
		getFileName(url,father,child);
		if(!strcmp(father,"config")){   //请求config目录下的文件
			if(!strcmp(child,"ports")){     //请求config/ports文件,生成该文件内容
				status = (unsigned char*)generatePortsData();
			}else if( !strcmp(child,"static_mac_table")){
				//printf("will generate mac data!\n\r");
				status = (unsigned char*)generateMacData();
			}else if( !strcmp(child,"dynamic_mac_table")){
				status = (unsigned char*)getDyanMacTable(1,"00-00-00-00-00-00",20,0);
				currentTotalPage = 20;
			}else if( !strcmp(child,"vlan_port")){
				status = (unsigned char*)generateVlanPortsStatus();
			}else if( !strcmp(child,"ip_config")){
				status = (unsigned char*)generateIPConfigData();
			}else if( !strcmp(child,"saveconfig")){  //重启之前先进行判断配置是否进行保存
				status = (unsigned char*)generateConfigSaveResult();
			}else if( !strcmp(child,"restartdevice")){
				status = (unsigned char*)getDeviceRestartStatus();
			}else if( !strcmp(child,"execresart")){
				//deviceRestart();
			}
		}else if(!strcmp(father,"stat")){    //请求stat目录下的文件
			if(!strcmp(child,"ports")){         //请求stat/ports文件，生成该文件内容
				// printf("getDataFile content!\n");
				status = (unsigned char*)generateCountData();
				//printf("data is %s\n",s->file.data);
			}else if( !strcmp(child,"portstate")){  //请求生成端口预览文件
				// printf("request port preview!\n"); 
				status = (unsigned char*)generatePreviewData();
				//file->file.data = status;
				//file->file.len = strlen(file->file.data) * sizeof(char);
			}else if( !strcmp(child,"vlan_membership_stat")){  //请求VLAN成员状态信息
				initFirstVlan();
				status = (unsigned char*)generateVlanStatus(1,20);
			}else if( !strcmp(child,"vlan_port_stat")){      //请求VLAN端口状态信息
				status = (unsigned char*)generateVlanPreviewStatus();
			}else if( !strcmp(child,"sys")){         //请求系统状态信息信息
				status = (unsigned char *)generateSystemStatusInfo();
			}else if( !strcmp(child,"update_status")){  //检查系统升级是否成功
				status = (unsigned char *)generateSystemUpdateStatus(); //获取系统升级状态
			}else if( !strcmp(child,"system_log")){
				status = getSystemLogEntry(1,20,0);
			}
			
		}//end stat目录文件的判断
		
	}//结束不带参数请求的处理
	file->data = (const char*)status;
	file->len = strlen(file->data) * sizeof(char);
	file->index = file->len;
	file->pextension = NULL;
	file->http_header_included = 0;
	freeMemory(clearParam);
	freeMemory(portNo);
	freeMemory(countType);
	freeMemory(band_port_no);
	freeMemory(startVid);
	freeMemory(numberEntry);
	freeMemory(nextAddr);
	freeMemory(nextEntry);
	freeMemory(vlanBeginId);
	freeMemory(vlanNumberEntry);
	freeMemory(serial_no);
	freeMemory(flush);
	freeMemory(start_log_id);
	freeMemory(log_total_entry);
	freeMemory(log_request_type);
}
//生成某个端口的详细信息
//获取请求文件的目录以及对应的文件名
/**
 * Function: getFileName
 * Description: 解析用户请求的文件名
 * @param url   请求的文件目录
 * @param father 存放返回的父级目录
 * @param child   存放返回的子级目录
**/
void getFileName(char *url,char *father,char *child){
	char *begin,*end;
	begin = &url[0];
	if(*begin == '/'){
		end = ++begin;
		while(*end != '/' ){  //find father dir
			if(*end == '\0'){  // father is root
				father = "/";
				strncpy(child,begin,(end - begin + 1));
				break;
			}
			end++;
		}
		strncpy(father,begin,(end - begin + 1)); //get father dir
		father[end - begin] = 0;
		begin = ++end;   //得到文件名
		while(*end && *end != ' ' && *end != '?'){   //能够处理带参数的文件请求
			end++;
		}
		strncpy(child,begin,(end - begin + 1));
		child[end - begin] = 0;
	}else{
		printf("path is error!\n");
	}
}


/**
 * Function: processPostRequest
 * Description: 处理POST请求，用于处理表单提交的内容
 * @param url   用户发送的内容。包括表单数据
 * @param s 	当前http连接状态
 * @param file_name   处理完表单以后要返回的页面名称
**/
void processPostRequest(char *url,struct http_state *s,char *file_name){
	//int begin_time;
	unsigned char is_exist;
	char *upload_file = NULL;
	char *formName = NULL;
	char *file_form = NULL;
	upload_file = getHttpHeaderParameter(url,"Content-Type");
	if(strstr(upload_file,"multipart/form-data")){  //判断是否是文件上传的请求
		//printf("file request\n\r");
		saveSystemImage(url);
	}else{
		formName = getParameter(url,"formName");
		//printf("%s\n\r;size is %d\n\r",url,strlen(url));
		if( !formName){
			//printf("form is null!\n");
			return;
		}else{
			//printf("name is %s\n",formName);
		}
		//printf("post param is %s\n",formName);
		if(strcmp(formName,"portConfigForm") == 0){//处理修改端口配置的表单请求
			modifyPortsConfig(url);
			strcpy(file_name,"/ports.shtml");
			//iconv_close(cd);
		}else if(strcmp(formName,"macForm") == 0){  //处理修改mac信息的post请求
			//printf("modify mac\n");  
			modifyMacData(url);
			strcpy(file_name,"/mac.shtml");
		}else if( !strcmp(formName,"vlanForm")){
			modifyVlanData(url);
			strcpy(file_name,"/vlan.shtml");
		}else if( !strcmp(formName,"vlanPortsForm")){
			modifyVlanPortsStatus(url);
			strcpy(file_name,"/vlan_port.shtml");
		}else if( !strcmp(formName,"loginForm")){
			is_exist = checkLogin(url);
		}else if( !strcmp(formName,"ipConfigForm")){
			modifyIPConfig(url);
			strcpy(file_name,"/system_config.shtml");
		}else if( !strcmp(formName,"recoverFactorySet")){
			recoverFactorySet();
			strcpy(file_name,"/factory.shtml");
		}else if( !strcmp(formName,"deviceRestart")){
			deviceRestart();
			strcpy(file_name,"/restart.shtml");
		}else if( !strcmp(formName,"saveAllConfigForm")){
			saveAllConfigToFalsh(url);
			strcpy(file_name,"/save_all.shtml");
		}else if(!strcmp(formName,"logoutForm")){
			processLogout();
			strcpy(file_name,"/index.shtml");
		}else if( !strcmp(formName,"serialConfigForm")){
			modifySerialConfig(url);
		}else if( !strcmp(formName,"downloadConfigFile")){ //请求下载配置文件
			is_file_download = 1;
			download_config_file_request = VETH_ALL_CONFIG_FILE;
			strcpy(file_name,"/download_config_file.shtml");
		}
		freeMemory(formName);
	}
	freeMemory(upload_file);
}

/*---------------------------------------------------------------------------*/
static char * get_function_param(char *str){
	int i = 0;
	char temp[20];
	while(1){
		if(str[i] == '\n'  || str[i] == '\t') break;
		else{
			temp[i] = str[i];
			++i;
		}
	}
	temp[i] = '\0';
	str = temp;
	str[i] = '\0';
	return str;
}
/*---------------------------------------------------------------------------*/
static unsigned short
generate_file_stats(void *arg)
{
	char *f = (char *)arg;
	//return snprintf((char *)uip_appdata, UIP_APPDATA_SIZE, "%5u", httpd_fs_count(f));
}
