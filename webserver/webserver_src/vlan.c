/*******************************************************
File name: vlan.c
Description: 提供和vlan操作相关的接口函数，处理vlan请求
Author: dujiancheng
Version: v1.0
********************************************************/
#include "vlan.h"
#include "gbk_conv_hanzi.h"
#include "vlan_link.h"
#include "request.h"
#include "LogUtility.h"
/* 向vlan表中添加50条数据 */
void addTestDataToLink() {
    int i;
    struct VlanLink *temp;
    char name[20];
    for(i = 2; i <= 50; i++ ) {
        memset(name,0,sizeof(name));
        sprintf(name,"默认默认默认%d",i);
        temp = (struct VlanLink *)malloc(sizeof(struct VlanLink));
        temp->vlan_entry.vlan_id = board.vlan_member[i].vlan_id = i;
        temp->vlan_value = 1;
        strcpy(board.vlan_member[i].vlan_name,name);
        memset(board.vlan_member[i].port_tag,1,sizeof(board.vlan_member[i].port_tag));
        strcpy(temp->vlan_name,board.vlan_member[i].vlan_name);
        memcpy(temp->vlan_entry.ports_tag,board.vlan_member[i].port_tag,sizeof(board.vlan_member[i].port_tag));
        addVlanLinkNode(&vlan_root,temp);
    }
}
/**
 * Function: initFirstVlan
 * Description: 初始化vlan的root节点，如果root为NULL的话
 *
**/
void initFirstVlan() {
    struct VlanLink *temp;
    int i;
    uint8_t ports_tag[PORTSNUM];
    if( !vlan_root ) {
        recoverConfigEntries("config_vlan");
        for(i = 0; i < MAXVLANMEMBERNUM; i++) {
            if(board.vlan_member[i].end == 1) {
                break;
            }
            temp = (struct VlanLink *)malloc(sizeof(struct VlanLink));
			if(!temp){
				printf("init first vlan malloc failed\n\r");
				return;
			}
            temp->vlan_entry.vlan_id = board.vlan_member[i].vlan_id;
            temp->vlan_value = 1;
            strcpy(temp->vlan_name,board.vlan_member[i].vlan_name);
            memcpy(temp->vlan_entry.ports_tag,board.vlan_member[i].port_tag,sizeof(board.vlan_member[i].port_tag));
            addVlanLinkNode(&vlan_root,temp);
        }

    }

}
/**
 * Function: initVlanPorts
 * Description: 恢复断电前对vlan端口的配置
 *
**/
void initVlanPorts() {
    int i;
    if(isFirstRequestVlanPorts) {
        isFirstRequestVlanPorts = FALSE;
        recoverConfigEntries("config_vlan_ports");
    }
}
/**
 * Function: generateVlanStatus
 * Description: 生成vlan的状态信息
 * @param vid 请求的vlan ID号
 * @param total_entry
 * @return 发送给浏览器解析的格式化字符串
**/
char* generateVlanStatus(int vid,int total_entry) {
    static char vlan_status[4096];
    char header[20];
    char tail[20];
    char vlan_entry[100];
    char port_status[12];
    struct VlanLink *p;
    int i = 0;
    int j;
    int found_num = 0;
    int temp;
    memset(vlan_status,0,sizeof(vlan_status));
    memset(header,0,sizeof(header));
    memset(tail,0,sizeof(tail));
    memset(vlan_entry,0,sizeof(vlan_entry));
    sprintf(header,"#Static");

    sprintf(tail,"#%d#%d",vid,total_entry);
    strcat(vlan_status,header);
    for(p = vlan_root; p ; p = p->next) {
        if(p->vlan_entry.vlan_id >= vid) {
            memset(port_status,'0',sizeof(port_status));

            sprintf(vlan_entry,"|%d/%s/",p->vlan_entry.vlan_id,p->vlan_name);
            concatVlanPortsTagString(vlan_entry,p->vlan_entry.ports_tag);
            found_num++;
            strcat(vlan_status,vlan_entry);

        }
        if(found_num == total_entry) {
            break;
        }
    }
    strcat(vlan_status,tail);
    return vlan_status;
}
/**
 * Function: generateVlanData
 * Description: 生成vlan选项信息
 * @param vid 请求的vlan ID号
 * @param total_entry
 * @return 发送给浏览器解析的格式化字符串
**/
char* generateVlanData(int vid,int total_entry) {
    static char vlan_status[4096];
    char vlan_table[100];
    char vlan_name[1536];
    char name[100];
    char header[20];

    struct VlanLink *p;

    int i = 0;
    int found_num = 0;
    memset(vlan_status,0,sizeof(vlan_status));
    memset(vlan_name,0,sizeof(vlan_name));
    memset(vlan_table,0,sizeof(vlan_table));
    memset(name,0,sizeof(name));

    sprintf(header,"%d|%d",vid,total_entry);
    strcat(vlan_status,header);
    for( p = vlan_root; p ; p = p->next) {
        memset(vlan_table,0,sizeof(vlan_table));
        memset(name,0,sizeof(name));
        if(p->vlan_entry.vlan_id >= vid) {
            sprintf(vlan_table,"|%d,%d,%s,",
                    p->vlan_entry.vlan_id,p->vlan_value,p->vlan_name);
            concatVlanPortsTagString(vlan_table,p->vlan_entry.ports_tag);
            strcat(vlan_status,vlan_table);
            ++found_num;
        }
        sprintf(name,"$%d,%s",p->vlan_entry.vlan_id,p->vlan_name);
        strcat(vlan_name,name);
        if(found_num == total_entry) {
            break;
        }

    }
    strcat(vlan_status,vlan_name);
    vlan_status[strlen(vlan_status) ] = '$';
    vlan_status[strlen(vlan_status) + 1] = '\0';
    return vlan_status;
}
/**
 * Function: modifyVlanData
 * Description: 根据用户提交的vlan表单内容修改vlan的配置信息
 * @param url 用户提交的表单数据的值
**/
void modifyVlanData(char *url) {
    struct VlanLink *p;
    char vlan_name[20];
    char *vlan_name_value = NULL;

    char mask_name[30];
    char *mask_value = NULL;

    char delete_name[20];
    char *delete_value = NULL;

    char new_vlan_id[20];
    char *new_vlan_id_value = NULL;

    char new_vlan_name[20];
    char *new_vlan_name_value = NULL;

    char new_mask_name[30];
    char *new_mask_value = NULL;

    int sum = 0;
    int i,j;
    int isModifyName = FALSE;
    int isModifyVector = FALSE;
    uint8_t ports_tag[PORTSNUM];
    char *gbk_name;
    struct VlanLink *tempNode;
	char message[96];

    /* 先对原来的数据进行修改处理 */
    for( p = vlan_root; p ; p = p->next) {
        /* 先判断该VLAN是否需要删除 */
        sprintf(delete_name,"delete_%d",p->vlan_entry.vlan_id);
        //printf("name is %s\n",delete_name);
        delete_value = getParameter(url,delete_name);
        if( delete_value != NULL) { //如果该VLAN需要删除，则直接删除，不需进行修改工作
            //printf("delete name is %s\n",delete_name);
            //删除交换芯片里面的数据
            p->vlan_entry.add_or_delete = 0;
            if(setVlanMember(&p->vlan_entry)) { //执行删除交换芯片函数

                //删除vlan_root中的数据
                deleteVlanLinkNodeById(&vlan_root,p->vlan_entry.vlan_id,p->vlan_name);
                //删除board_info中的VLAN数据
                deleteBoardVlanMember(p->vlan_entry.vlan_id,p->vlan_name);
            }
			memset(message,0,sizeof(message));
			sprintf(message,"VLAN %d 删除成功",p->vlan_entry.vlan_id);
			addLogEntry(INFO,"vorx",message);
			printSystemLog();
            continue;
        }
        sprintf(vlan_name,"name_%d",p->vlan_entry.vlan_id);

        vlan_name_value = getParameter(url,vlan_name);

        if( !vlan_name_value) { //如果存在分页那么可能修改的只是部分vlan，vlan名可能不存在
            continue;
        }

        gbk_name = gbk_conv_hanzi(vlan_name_value);

        if( !strcmp(p->vlan_name,gbk_name)) { //判断是否需要修改VLAN名
            isModifyName = FALSE;
        } else {
            isModifyName = TRUE;
            strcpy(p->vlan_name,gbk_name);
        }
        //port_vector = 0x00;
        memset(ports_tag,0,sizeof(ports_tag));
        for(i = 0; i < ports_num; i++) {  //修改端口数据
            sprintf(mask_name,"hidden_mask_%d_%d",p->vlan_entry.vlan_id,i + 1);
            mask_value = getParameter(url,mask_name);
            if( mask_value ) {
                //printf("mask value is %s\n\r",mask_value);
                //port_vector |= (0x01 << i);
                ports_tag[i] = atoi(mask_value);
            }
            freeMemory(mask_value);
        }  //end for(i = 0; i < PORTSNUM; i++)
        if( !isPortsTagChange(p->vlan_entry.vlan_id,p->vlan_entry.ports_tag,ports_tag)) { //判断是否修改端口向量
            isModifyVector = FALSE;
        } else {
            isModifyVector = TRUE;
            memcpy(p->vlan_entry.ports_tag,ports_tag,sizeof(ports_tag));
        }

        /* 修改board结构体中对应的数据 */
        if( isModifyName || isModifyVector) {
            p->vlan_entry.add_or_delete = ADD_VLAN;
            if(setVlanMember(&p->vlan_entry)) { //修改交换芯片数据
                for(i = 0; i < MAXVLANMEMBERNUM; i++) {
                    if(board.vlan_member[i].end == 1) {
                        break;
                    }
                    if(board.vlan_member[i].vlan_id == p->vlan_entry.vlan_id) {
                        if(isModifyName) {
                            strcpy(board.vlan_member[i].vlan_name,p->vlan_name);
                        }
                        if(isModifyVector) {
                            memcpy(board.vlan_member[i].port_tag,p->vlan_entry.ports_tag,sizeof(p->vlan_entry.ports_tag));
                        }
                    }//end if(board.vlan_member...)

                }//end for(i = 0;.....)
            }//end if(setVlanMember)
        }//end if(isModifyName ...)
        freeMemory(vlan_name_value);
        //freeMemory(mask_value);
        freeMemory(delete_value);
    }//end for( p = root; p ; p = p->next)
    //printf("before data process end!\n");
    //sum = 0;
    /* 添加新的VLAN数据 */
    for( i = 0; i < 4096; i++) { //允许最大创建的VLAN数
        sprintf(new_vlan_id,"vid_new_%d",i + 1);
        new_vlan_id_value = getParameter(url,new_vlan_id);
        if( new_vlan_id_value == NULL) {
            freeMemory(new_vlan_id_value);
            break;
        } else {
            tempNode = (struct VlanLink *)malloc(sizeof(struct VlanLink));
            memset(tempNode->vlan_name,0,sizeof(tempNode->vlan_name));
        }
        sprintf(new_vlan_name,"name_new_%d",i + 1);
        new_vlan_name_value = getParameter(url,new_vlan_name);
        gbk_name = gbk_conv_hanzi(new_vlan_name_value);
        tempNode->vlan_entry.vlan_id = atoi(new_vlan_id_value);

        strcpy(tempNode->vlan_name,gbk_name);

        tempNode->vlan_value = 1;
        memset(ports_tag,0,sizeof(ports_tag));
        for( j = 0; j < ports_num; j++) {  //取得所有端口对应的值
            sprintf(new_mask_name,"hidden_mask_new_%d_%d",i + 1,j + 1);
            new_mask_value = getParameter(url,new_mask_name);
            if(new_mask_value) {
                //port_vector |= 0x01 << j;
                ports_tag[j] = atoi(new_mask_value);
            }
            freeMemory(new_mask_value);
        }//end for( j = 0; j < PORTSNUM; j++)
        memcpy(tempNode->vlan_entry.ports_tag,ports_tag,sizeof(ports_tag));
        tempNode->vlan_entry.add_or_delete = ADD_VLAN;
        if(isExistVlan(vlan_root,tempNode)) {
            free(tempNode);
            tempNode = NULL;
            freeMemory(new_vlan_id_value);
            freeMemory(new_vlan_name_value);
            continue;
        }
        /*判断新添加的vlan是否已经存在,正常情况下是不会存在新添加的项
        **是为了避免表单提交可能存在重复的现象，在此做个验证
        */
        if(setVlanMember(&tempNode->vlan_entry)) { //向交换芯片中添加数据
            //printf("add VLAN %d to switch chip sucessful\n\r",tempNode->vlan_entry.vlan_id);
            addVlanLinkNode(&vlan_root,tempNode);  //向缓存链表中添加数据
			memset(message,0,sizeof(message));
			sprintf(message,"添加VLAN %d 成功",tempNode->vlan_entry.vlan_id);
			addLogEntry(WARNING,"vorx",message);   //时间未能同步警告日志
			printSystemLog();
			/* 向board结构体中添加数据 */
			addBoardVlanMember(tempNode->vlan_entry.vlan_id,tempNode->vlan_name,tempNode->vlan_entry.ports_tag);
        }// end if(setVlanMember)
        freeMemory(new_vlan_id_value);
        freeMemory(new_vlan_name_value);
        //freeMemory(new_mask_value);
    }//end for( i = 0; i < 4096; i++)


}
/**
 * Function: addBoardVlanMember
 * Description: 向board结构体中添加新的vlan选项
 * @param vlan_id 添加的VLAN ID
 * @param vlan_name 添加的VLAN 名称
 * @param port_tag VLAN包含的端口信息
**/
int addBoardVlanMember(uint16_t vlan_id,char *vlan_name,uint8_t *ports_tag) {
    int j;
    for(j = 0; j < MAXVLANMEMBERNUM; j++) { //向board结构体重添加数据
        if(board.vlan_member[j].end == 1) {
            board.vlan_member[j].vlan_id = vlan_id;
            strcpy(board.vlan_member[j].vlan_name,vlan_name);
            memcpy(board.vlan_member[j].port_tag,ports_tag,sizeof(board.vlan_member[j].port_tag));
            board.vlan_member[j].end = 0;
            board.vlan_member[j + 1].end = 1;
            break;
        }
    }
}
/**
 * Function: isPortsTagChange
 * Description: 判断vlan端口标志是否被修改
 * @param vlan_id vlan的ID
 * @param src_tag 之前的端口标志信息
 * @param des_tag 现在的端口标志信息
 * @return 1 vlan端口标志被修改了
 *		   0 vlan端口标志没有被修改
**/
int isPortsTagChange(uint16_t vlan_id,uint8_t *src_tag,uint8_t *des_tag) {
    int i;
	uint8_t is_need_modify = 0;
	char message[96];
    for(i = 0; i < ports_num; i++) {
        if(src_tag[i] != des_tag[i]) {
			memset(message,0,sizeof(message));
			switch(des_tag[i]){
				case 0:
					sprintf(message,"VLAN %d 取消了端口 %d 成员",vlan_id,i + 1);
					addLogEntry(INFO,"vorx",message);   //
					printSystemLog();
					break;
				case 1:
					sprintf(message,"修改VLAN %d 成员端口 %d 为 \"未标记PVID\" 模式",vlan_id,(i + 1));
					addLogEntry(INFO,"vorx",message);   //
					printSystemLog();
					break;
				case 2:
					sprintf(message,"修改VLAN %d 成员端口 %d 为 \"标记所有PVID\" 模式",vlan_id,(i + 1));
					addLogEntry(INFO,"vorx",message);   //
					printSystemLog();
					break;
				case 3:
					sprintf(message,"修改VLAN %d 成员端口 %d 为 \"全不标记\" 模式",vlan_id,i + 1);
					addLogEntry(INFO,"vorx",message);   //
					printSystemLog();
					break;
			}
            
			is_need_modify = 1;
        }
    }
	if(is_need_modify == 1){
		return 1;
	}
    return 0;
}
/**
 * Function: generateVlanPortsStatus
 * Description: 生成vlan端口状态信息
 * @return 发送给浏览器解析的格式化字符串
**/
char* generateVlanPortsStatus() {
    static char vlan_ports_status[260];
    char port[20];
    int i;
    memset(vlan_ports_status,'0',sizeof(vlan_ports_status));
    strcpy(vlan_ports_status,"88a8#");
    for(i = 0; i < ports_num; i++) {
        memset(port,'0',sizeof(port));
        sprintf(port,"%d/%d/%d/%d/%d/%d|",i+1,board.vlan_port[i].port_type,
                board.vlan_port[i].filter_option,board.vlan_port[i].frame_type,
                board.vlan_port[i].port_id,board.vlan_port[i].port_mode
                //board.vlan_port[i].send_tag
               );
        strcat(vlan_ports_status,port);
    }
    return vlan_ports_status;
}
/**
 * Function: modifyVlanPortsStatus
 * Description: 修改VLAN端口的状态信息
 * @param url 用户提交的表单数据的值
**/
void modifyVlanPortsStatus(char *url) {
    char vlan_port_type[20];
    char *vlan_port_type_value = NULL;

    char vlan_port_ingress[20];
    char *vlan_port_ingress_value = NULL;

    char vlan_frame_type[20];
    char *vlan_frame_type_value = NULL;

    char vlan_port_sel[20];
    char *vlan_port_sel_value = NULL;

    char vlan_port_id[20];
    char *vlan_port_id_value = NULL;

    //char vlan_txtag_name[20];
    //char *vlan_txtag_value = NULL;
	char message[120];
	int int_type = 0;
	int int_frame_type = 0;
    int i;
    for(i = 0; i < ports_num; i++) {
	
        sprintf(vlan_port_type,"porttypev2_%d",i + 1);
        sprintf(vlan_port_ingress,"ingressflt_%d",i + 1);
        sprintf(vlan_frame_type,"frametypev2_%d",i + 1);
        sprintf(vlan_port_sel,"selpvlan_%d",i + 1);
        sprintf(vlan_port_id,"pvid_%d",i + 1);
        //sprintf(vlan_txtag_name,"tx_tag_%d",i + 1);
	
        vlan_port_type_value = getParameter(url,vlan_port_type);
        vlan_port_ingress_value = getParameter(url,vlan_port_ingress);
        vlan_frame_type_value = getParameter(url,vlan_frame_type);
        vlan_port_sel_value = getParameter(url,vlan_port_sel);
        vlan_port_id_value = getParameter(url,vlan_port_id);
        //vlan_txtag_value = getParameter(url,vlan_txtag_name);
		int_type = atoi(vlan_port_type_value);
		memset(message,0,sizeof(message));
		if(board.vlan_port[i].port_type != int_type){  //端口类型发生了改变
			board.vlan_port[i].port_type = int_type;
			switch(int_type){
				case 0:
					sprintf(message,"修改 VALN 端口 %d 为 \" 禁止 \" 模式",i+ 1);
					addLogEntry(INFO,"vorx",message);   
					printSystemLog();
					break;
				case 1:
					sprintf(message,"修改 VALN 端口 %d 为 \" 回退 \" 模式",i+ 1);
					addLogEntry(INFO,"vorx",message);   
					printSystemLog();
					break;
				case 2:
					sprintf(message,"修改 VALN 端口 %d 为 \" 检查 \" 模式",i+ 1);
					addLogEntry(INFO,"vorx",message);   
					printSystemLog();
					break;
				case 3:
					sprintf(message,"修改 VALN 端口 %d 为 \" 安全 \" 模式",i+ 1);
					addLogEntry(INFO,"vorx",message);   
					printSystemLog();
					break;
			}
			
		}
        memset(message,0,sizeof(message));
        if(vlan_port_ingress_value) {
			if(board.vlan_port[i].filter_option != 1){   //由关闭状态变为开启状态
				board.vlan_port[i].filter_option = 1;
				sprintf(message,"VALN 端口 %d 开启了 \"入口过滤\"选项",i+ 1);
				addLogEntry(INFO,"vorx",message);   
				printSystemLog();
			}
            
        } else {
			if(board.vlan_port[i].filter_option != 0){   //由开启状态变为关闭状态
				board.vlan_port[i].filter_option = 0;
				sprintf(message,"VALN 端口 %d 关闭了 \" 入口过滤 \" 选项",i+ 1);
				addLogEntry(INFO,"vorx",message);   
				printSystemLog();
			}
            
        }
		memset(message,0,sizeof(message));
		int_frame_type = atoi(vlan_frame_type_value);
		if(board.vlan_port[i].frame_type != int_frame_type){
			board.vlan_port[i].frame_type = int_frame_type;
			switch(int_frame_type){
				case 0:
					sprintf(message,"修改 VALN 端口 %d 帧类型为 \" 所有 \"  类型",i+ 1);
					addLogEntry(INFO,"vorx",message);   
					printSystemLog();
					break;
				case 1:
					sprintf(message,"修改 VALN 端口 %d 帧类型为 \" 标记 \" 类型",i+ 1);
					addLogEntry(INFO,"vorx",message);   
					printSystemLog();
					break;
				case 2:
					sprintf(message,"修改 VALN 端口 %d 帧类型为 \" 未标记 \" 类型",i+ 1);
					addLogEntry(INFO,"vorx",message);   
					printSystemLog();
					break;
			}
		}
        memset(message,0,sizeof(message));
        if(vlan_port_id_value) {
			if(board.vlan_port[i].port_mode  == 1){  //端口模式没有被修改
				if(board.vlan_port[i].port_id != atoi(vlan_port_id_value)){  //端口ID被修改
					board.vlan_port[i].port_id = atoi(vlan_port_id_value);
					sprintf(message,"修改 VALN 端口 %d 的端口ID为 %d",i+ 1,board.vlan_port[i].port_id);
					addLogEntry(INFO,"vorx",message);   
					printSystemLog();
				}
			}else{   //之前的端口模式由无变为具体模式
				board.vlan_port[i].port_mode  = 1;
				if(board.vlan_port[i].port_id != atoi(vlan_port_id_value)){  //同时端口ID被修改
					board.vlan_port[i].port_id = atoi(vlan_port_id_value);
					sprintf(message,"修改VALN 端口 %d 为 \"具体\" 模式,并且修改的端口ID为 %d",i+ 1,board.vlan_port[i].port_id);
					addLogEntry(INFO,"vorx",message);   
					printSystemLog();
				}else{   //端口ID没有被修改
					sprintf(message,"修改VALN 端口 %d 为 \"具体\" 模式",i+ 1);
					addLogEntry(INFO,"vorx",message);   
					printSystemLog();
				}
			}
			
        } else {
			if(board.vlan_port[i].port_mode != 0){   //有具体模式变为无模式
				board.vlan_port[i].port_mode = 0;
				board.vlan_port[i].port_id = 1;
				sprintf(message,"修改VALN 端口 %d 为 \" 无 \" 模式",i+ 1);
				addLogEntry(INFO,"vorx",message);   
				printSystemLog();
			}
            
        }
        //board.vlan_port[i].send_tag = atoi(vlan_txtag_value);

        freeMemory(vlan_port_type_value);
        freeMemory(vlan_port_ingress_value);
        freeMemory(vlan_frame_type_value);
        freeMemory(vlan_port_sel_value);
        freeMemory(vlan_port_id_value);
    }//end for(i = 0; i < PORTSNUM; i++)
    modifySwitchVlanPorts();//向交换芯片中写入数据
}


/**
 * Function: modifySwitchVlanPorts
 * Description: 修改交换芯片里面的VLAN端口配置信息

**/
void modifySwitchVlanPorts() {
    int i;
    for(i = 0; i < ports_num; i++) {
        /* 配置端口类型 */
        setSecurityLevel(board.vlan_port[i].port_type,i);
        /* 配置入口过滤 */
        if(board.vlan_port[i].filter_option == 1) {
            setFilterdFrames(board.vlan_port[i].frame_type,i);
        }
        /* 配置端口ID，端口VLAN模式为具体的时候配置用户设置的端口id*/
        if(board.vlan_port[i].port_mode == 1) {
            setPortVlanId(board.vlan_port[i].port_id,i);
        } else { //配置端口id为1
            setPortVlanId(1,i);
        }
        /* 配置VLAN端口发送tag */
        //setEgressType(board.vlan_port[i].send_tag,i,board.vlan_port[i].port_id);
    }
    //printf("modify VLAN ports to switch chip sucessful\n\r");
}
/**
 * Function: generateVlanPreviewStatus
 * Description: 生成VLAN的端口配置信息
 * @return 发送给浏览器解析的格式化字符串
**/
char *generateVlanPreviewStatus() {
    static char vlan_preview_status[300];
    char preview_port[30];
    int i;
    memset(vlan_preview_status,'0',sizeof(vlan_preview_status));
    strcpy(vlan_preview_status,"#Static");
    for(i = 0; i < ports_num; i++) {
        memset(preview_port,'0',sizeof(preview_port));
        sprintf(preview_port,"|%d/%d/%d/%d/%d",i + 1,board.vlan_port[i].port_id,board.vlan_port[i].port_type,
                board.vlan_port[i].filter_option,board.vlan_port[i].frame_type//,board.vlan_port[i].send_tag
               );
        strcat(vlan_preview_status,preview_port);
    }
    return vlan_preview_status;
}
/**
 * Function: deleteBoardVlanMember
 * Description: 删除board结构体中的VLAN选项
 * @param vlan_id  要删除的vlan ID
 * @param vlan_name 要删除的vlan 名称
**/
int deleteBoardVlanMember(int vlan_id,char *vlan_name) {
    int i;
    int j;
    for(i = 0; i < MAXVLANMEMBERNUM; i++) {
        if( board.vlan_member[i].vlan_id == vlan_id &&
                !strcmp(board.vlan_member[i].vlan_name,vlan_name)) {
            for(j = i; j < MAXMACADDRNUM; j++) {
                /* 判断该项的下一项是否为空，如果为空则把该项置空，结束 */
                if(board.vlan_member[j + 1].end == 1) { //
                    memset(&board.vlan_member[j],0,sizeof(board.vlan_member[j]));
                    board.vlan_member[j].end  = 1;
                    break;
                }
                board.vlan_member[j].vlan_id = board.vlan_member[j + 1].vlan_id;
                board.vlan_member[j].end = board.vlan_member[j + 1].end;
                memcpy(board.vlan_member[j].port_tag ,
                       board.vlan_member[j + 1].port_tag,sizeof(board.vlan_member[j + 1].port_tag));
                memcpy(board.vlan_member[j].vlan_name,
                       board.vlan_member[j + 1].vlan_name,sizeof(board.vlan_member[j + 1].vlan_name));
            }//end for(j = i;...);
        }//end if(!Str_cmp)
    }//end for(i = 0;...)
    return 0;
}
/**
 * Function: concatVlanPortsTagString
 * Description: 把vlan的端口成员对应的整形数组格式化成字符串数据，发送给浏览器解析
 * @param des_str  存放格式化以后字符串的数据
 * @param ports_tag 整形数组表示的vlan 端口成员数据
**/
int concatVlanPortsTagString(char *des_str,uint8_t *ports_tag) {
    char tag_str[5];
    int i;
    for(i = 0; i < ports_num; i++) {
        memset(tag_str,0,sizeof(tag_str));
        sprintf(tag_str,"%d/",ports_tag[i]);
        strcat(des_str,tag_str);
    }
}

