/*******************************************************
File name: mac.c
Description: 提供处理Mac地址相关的接口函数，根据用户的不同的对Mac做相应的处理
Author: dujiancheng
Version: v1.0
********************************************************/
#include"mac.h"
#include "veth_common.h"
#include "veth_mac.h"
#include "veth_marvell_88e6097f.h"
#include "board_info.h"
#include "LogUtility.h"
#include<stdio.h>

//#define MAX_PORTS_NUM 8  //端口数
#define MAX_MAC_ENTRY 64  //最大Mac地址条数
#define RSTP_MAC
/**
 * Function: generateMacData
 * Description: 生成Mac地址的相关信息函数，生成的信息包括了老化时间、学习模式和目前用户配置的所有静态Mac地址
 *
**/
char* generateMacData() {
    static char mac_status[1024];
    char static_table[100];
    struct MacLink *p;
    int current_age_time = 300;
    int disable_age = 0;
    unsigned char i;
    unsigned char mac_address[20];
    char change_status[PORTSNUM + 1];
    //unsigned char mac[10];
    uint16_t learn_mode;
    char port_mode[PORTSNUM + 1];
    int ports_vector[PORTSNUM];
    //char vector_str[5];
//		port_mode = (char*)malloc(ports_num * sizeof(char));
//		ports_vector = (int*)malloc(ports_num * sizeof(int));
    for(i = 0; i < ports_num; i++) {
        port_mode[i] = 'A';
    }
    port_mode[PORTSNUM] = 0;
    for(i = 0; i < ports_num; i++) {
        change_status[i] = '1';
    }
    change_status[PORTSNUM] = 0;
    initRoot();//初始化root节点
    //获取老化是否可用
//		current_age_time = Get_Age_time();
//		if(current_age_time == 0){
//				disable_age = 1;
//		}else{
//
//		}
    //获取端口学习模式
    learn_mode = Get_MAC_forbid();
    for(i = 0; i < ports_num; i++) {
        if((learn_mode & (0x01 << i))) {
            port_mode[i] = 'D';
        }
    }
    //sprintf(mac_status,"%d/%d/%s/%s/-|",board.mac_address.age_time,board.mac_address.disable_age,port_mode,change_status);
    memset(static_table,0,sizeof(static_table));
    memset(mac_status,0,sizeof(mac_status));
    sprintf(mac_status,"%d/%d/%s/%s|",board.mac_address.age_time,board.mac_address.disable_age,port_mode,change_status);
    if(mac_root) {
        for( p = mac_root; p; p = p->next ) {
            //得到静态Mac地址
            //printf("dym is %d\n\r",p->mac_entry.sta_or_dym);
            if(p->mac_entry.sta_or_dym == ENTRY_DYNAMIC || !Str_cmp(p->mac_entry.new_mac,board.sets_info.The_MAC,6)
                    || !Str_cmp(p->mac_entry.new_mac,MGMT_MAC,6)) {
                continue;
            }
            memset(static_table,0,sizeof(static_table));
            memset(ports_vector,0,ports_num * sizeof(int));
            getPortsVectorArray(p->mac_entry.port_vector,ports_vector);
            memset(mac_address,0,sizeof(mac_address));
            getMacString(mac_address,p->mac_entry.new_mac);
            //printf("mac_address is %s\n\r",mac_address);
            sprintf(static_table,"%s/%d/",mac_address,p->mac_entry.vlan_id);
            /* 拼接要发送的向量字符串 */
            concatVectorString(static_table,ports_vector);
            strcat(mac_status,static_table);
        }
    }
    //printf("mac_status is %s\n\r",mac_status);
    return mac_status;
}

/**
 * Function: modifyMacData
 * Description: 修改Mac地址相关的配置信息，新的配置信息有用户通过post方法传入。新的配置信息包括老化时间、学习模式
				以及新添加的静态Mac地址选项，允许添加的最大静态Mac地址数为64条，该数目在页面中做了相应的判断
 * @param url  用户传入的新的Mac地址相关的配置
**/
void modifyMacData(char *url) {
    int i,j;
    int board_mac_index;
    int delete_num = 0;
    //int age_time;
    struct MacLink *temp;
    struct MacLink *p;

    char *disable_age_name = "DisableAgeing";
    char *disable_age_value = NULL;

    char *age_box_name = "agebox";
    char *age_box_value = NULL;

    char learn_port_name[20];
    char *learn_port_value = NULL;

    char vid_name[10];
    char *vid_value = NULL;
    uint8_t mac_vid;

    char mac_name[10];
    char *mac_value = NULL;


    char dest_name[15];
    char *dest_value = NULL;

    char delete_name[10];
    char *delete_value = NULL;

    uint32_t learn_mode = 0x00;

    unsigned char hex_mac[6];
    uint32_t port_vector = 0x00;
    unsigned char sucess = 0;
    unsigned char isExist;
    MAC_API_T mac_entry;  //删除时使用
    char message[96];
    uint8_t port_mode = 0x00;
    // printf("URL:%s\n",url);
    //获取和设置老化是否可用
    disable_age_value = getParameter(url,disable_age_name);
    if(disable_age_value) {
        if(board.mac_address.disable_age == 0) { //之前的状态为可用状态
            Set_Age_time(0);
            board.mac_address.disable_age = 1;  //修改board中老化不可用
            addLogEntry(INFO,"vorx","禁止了自动老化");
            printSystemLog();
        }

    } else {
        if(board.mac_address.disable_age == 1) { //之前的状态不可用
            board.mac_address.disable_age = 0;  //修改board中老化可用
            addLogEntry(INFO,"vorx","恢复了自动老化");
            printSystemLog();
        }

    }
    //获取和设置老化时间
    age_box_value = getParameter(url,age_box_name);
    if(age_box_value) {
        board.mac_address.disable_age = 0;  //修改board中老化可用
        age_time = Get_Age_time();
        if(atoi(age_box_value) != age_time) {
            Set_Age_time(atoi(age_box_value));
            age_time = Get_Age_time();
            board.mac_address.age_time = age_time;  //修改board中老化时间
            memset(message,0,sizeof(message));
            sprintf(message,"修改自动老化时间为 %d",age_time);
            addLogEntry(INFO,"vorx",message);
            printSystemLog();
        }
    } else {
        board.mac_address.disable_age = 1;  //修改board中老化不可用
    }
    //获取端口学习模式
    for(i = 0; i < ports_num; i++) {
        sprintf(learn_port_name,"learn_port_%d",i + 1);
        learn_port_value = getParameter(url,learn_port_name);
        //printf("port_name is %s; value is %s\n\r",learn_port_name,learn_port_value);
        //port_mode[i] = learn_port_value[6];
        port_mode = 0x00;
        if(learn_port_value[6] == 'D') {
            port_mode = board.mac_address.learn_mode & ((0x01 << i));
            learn_mode |= (0x01 << i);
            if(port_mode == 0x00) {     //某一个端口的学习模式更改了由自动模式更改为禁止模式
                memset(message,0,sizeof(message));
                sprintf(message,"端口 %d 禁用了学习模式",i + 1);
                addLogEntry(INFO,"vorx",message);
                printSystemLog();
            }

        } else {    //判断是否由禁止模式变为自动模式
            port_mode = board.mac_address.learn_mode & ((0x01 << i));
            if(port_mode != 0x00) { //之前的为禁止模式
                memset(message,0,sizeof(message));
                sprintf(message,"端口 %d 恢复了自动学习模式",i + 1);
				addLogEntry(INFO,"vorx",message);
                printSystemLog();
            }
        }
        freeMemory(learn_port_value);
    }
    //printf("learn_value is %0x\n\r",);
    //设置端口学习模式
    //printf("learn mode vector is %x\n\r",learn_mode);
    if(board.mac_address.learn_mode != learn_mode) {
        Set_Manual_learning_mode(learn_mode);
        board.mac_address.learn_mode = learn_mode;  //修改board中端口的学习模式
    }
    //获取用户新添静态Mac地址
    for( i = 0; i < MAX_MAC_ENTRY; i++) {
        port_vector = 0x00;
        sprintf(mac_name,"MAC_%d",i + 1);  //获取的mac地址
        mac_value = getParameter(url,mac_name);
        if( !mac_value) {
            freeMemory(mac_value);
            //printf("break");
            break;
        }
        //printf("need add mac address\n\r");
        getDivideMacAddress(hex_mac,mac_value);

        sprintf(vid_name,"VID_%d",i + 1);
        vid_value = getParameter(url,vid_name);
        mac_vid = atoi(vid_value);

        for(j = 0; j < ports_num; j++) {
            sprintf(dest_name,"h_Dest_%d_%d",i + 1,j + 1);
            dest_value = getParameter(url,dest_name);
            if( !strcmp(dest_value,"1")) {
                port_vector |= 0x01 << j;
            }
            freeMemory(dest_value);
        }


        sprintf(delete_name,"Delete_%d",i + 1);
        delete_value = getParameter(url,delete_name);  //获取需要删除的Vid

        if( delete_value == NULL ) { //判断该选项是否是需要删除的
            temp = (struct MacLink*)malloc(sizeof(struct MacLink));

            memcpy(temp->mac_entry.new_mac,hex_mac,sizeof(hex_mac));
            temp->mac_entry.port_vector = port_vector;
            temp->mac_entry.add_or_delet = 1;//添加标志
            temp->mac_entry.vlan_id = mac_vid;
            //添加之前先判断节点是否存在
            isExist = isExistNode(mac_root,temp,mac_value);
            if( !isExist) { //节点不存在添加到链表、board结构体和硬件表中
                sucess = Add_or_Delet_MACentry(&temp->mac_entry);
                if(sucess) {
                    temp->mac_entry.sta_or_dym = 0;
                    addMacLink(&mac_root,temp);
                    /* 向board中添加新的静态mac表项 */
                    addBoardMacAddress(mac_vid,hex_mac,port_vector);
                    //printf("add static mac sucess\n\r");
                    //printBoardInfo("config_mac");
                    memset(message,0,sizeof(message));
                    sprintf(message,"静态的MAC %s 添加成功",mac_value);
                    addLogEntry(INFO,"vorx",message);
                    printSystemLog();
                } else {
                    memset(message,0,sizeof(message));
                    sprintf(message,"静态的MAC %s 添加失败！！",mac_value);
                    addLogEntry(INFO,"vorx",message);
                    printSystemLog();
                    printf("add static mac failed\n\r");
                }
            } else if( isExist == 2) {
                /*节点存在只需要修改，需要从新添加到硬件表中，缓存中的数据的修改
                **在isExistNode方法中已经完成
                */

                /* 修改board中mac地址项信息 */
                //modifyBoardMacAddress(temp);
                sucess = Add_or_Delet_MACentry(&temp->mac_entry);
                if(sucess) {
                    //addMacLink(&root,temp);
                    //printf("modify static mac sucess\n\r");
                    /* 修改board中mac地址项信息 */
                    modifyBoardMacAddress(temp);
                    //printBoardInfo("config_mac");
                } else {
                    //printf("modify static mac failed\n\r");
                }
            }
        } else if(delete_value) {

            mac_entry.add_or_delet = 0;
            memcpy(mac_entry.new_mac,hex_mac,sizeof(hex_mac));
            mac_entry.port_vector = port_vector;
            mac_entry.vlan_id = mac_vid;
            sucess = Add_or_Delet_MACentry(&mac_entry);
            if(sucess) {
                deleteBoardMacAddress(mac_vid,hex_mac);//删除board中的mac地址项
                deleteMacLinkByVid(&mac_root,mac_vid,hex_mac);
                //printf("delete static mac sucess\n\r");
                //printBoardInfo("config_mac");
                memset(message,0,sizeof(message));
                sprintf(message,"静态的MAC %s 删除成功",mac_value);
                addLogEntry(INFO,"vorx",message);
                printSystemLog();
            } else {
                memset(message,0,sizeof(message));
                sprintf(message,"静态的MAC %s 添加失败",mac_value);
                addLogEntry(INFO,"vorx",message);
                printSystemLog();
                //printf("delete static mac failed\n\r");
            }

        }
        freeMemory(vid_value);
        freeMemory(dest_value);
        freeMemory(mac_value);
        freeMemory(delete_value);
    }//end for
    freeMemory(age_box_value);
    freeMemory(disable_age_value);
    //freeMemory(learn_port_value);
    //printf("modefy mac table finish\n\r");
}
/**
 * Function: addBoardMacAddress
 * Description: 添加静态Mac地址到board结构体中，Mac数组中
 * @param mac_vid Mac地址所属的vlan ID
 * @param hex_mac Mac地址的十六进制的数组
 * @param port_vector 静态Mac地址所包含的端口向量
**/
int addBoardMacAddress(uint8_t mac_vid,uint8_t *hex_mac,uint16_t port_vector) {
    int board_mac_index;
    for(board_mac_index = 0; board_mac_index < MAXMACADDRNUM; board_mac_index++) {
        /* 找到mac_table中最后一个项 */
        if(board.mac_address.mac_table[board_mac_index].end == 1) {
            board.mac_address.mac_table[board_mac_index].vlan_id = mac_vid;
            memcpy(board.mac_address.mac_table[board_mac_index].mac_addr,
                   hex_mac,6);
            board.mac_address.mac_table[board_mac_index].end = 0;
            board.mac_address.mac_table[board_mac_index].port_vector = port_vector;
            board.mac_address.mac_table[board_mac_index].static_dyna = ENTRY_STATIC;
            /* 设置下一项为最后一项 */
            if(board_mac_index != MAXMACADDRNUM - 1) {
                board.mac_address.mac_table[board_mac_index + 1].end = 1;
            }
            break;
        }
    }
}
/**
 * Function: modifyBoardMacAddress
 * Description: 修改board结构体中的静态Mac地址的配置
 * @param temp 静态Mac地址的结构体，存储MAC地址的相关信息
 * 
**/
int  modifyBoardMacAddress(struct MacLink *temp) {
    int i;
    for(i = 0; i < MAXMACADDRNUM; i++) {
        if(board.mac_address.mac_table[i].end == 1) {
            break;
        }
        if( !Str_cmp(board.mac_address.mac_table[i].mac_addr,temp->mac_entry.new_mac,6)
                && board.mac_address.mac_table[i].vlan_id == temp->mac_entry.vlan_id) {
            board.mac_address.mac_table[i].port_vector = temp->mac_entry.port_vector;
        }

    }
}
/**
 * Function: printfMacAddress
 * Description: 以点十六进制的格式打印Mac地址，用于调试时方便查看Mac地址的变化
 * @param mac_address 存放要打印的Mac地址的数组
 * 
**/
void printfMacAddress(uint8_t *mac_address) {
    int i;
    printf("mac address is\n\r");
    for(i = 0; i < 6; i++) {
        if(i < 5) {
            printf("%02X.",mac_address[i]);
        } else {
            printf("%02X",mac_address[i]);
        }
    }
    printf("\n\r");
}
/**
* Function: deleteBoardMacAddress
* Description: 删除board结构体中对应Mac地址的数组中Mac地址，删除办法是用后面一个的值覆盖掉前面一个的值
* 直到遇到空项为之
* @param vid 要删除的Mac地址所属的vlan ID
* @param mac_address 要删除的MAC地址
**/
int deleteBoardMacAddress(uint8_t vid,uint8_t *mac_address) {
    int i;
    int j;
    int is_delete = 0;
//	printf("deleted mac address is :\n\r");
//	printfMacAddress(mac_address);
//	printf("before delete board mac \n\r");
//	printBoardInfo("config_mac");
    for(i = 0; i < MAXMACADDRNUM; i++) {
        //printf("find board mac address\n\r");
        if( memcmp(board.mac_address.mac_table[i].mac_addr,mac_address,6) == 0
                && board.mac_address.mac_table[i].vlan_id == vid) {
            //printf("board mac address is :\n\r");
            //printfMacAddress(board.mac_address.mac_table[i].mac_addr);


            for(j = i; j < MAXMACADDRNUM - 1; j++) {
                /* 判断该项的下一项是否为空，如果为空则把该项置空，结束 */
                if(board.mac_address.mac_table[j + 1].end == 1) { //
                    memset(&board.mac_address.mac_table[j],0,sizeof(board.mac_address.mac_table[j]));
                    board.mac_address.mac_table[j].end  = 1;
                    is_delete = 1;
                    //printf("end delete\n\r");
                    break;
                }
                board.mac_address.mac_table[j].end = board.mac_address.mac_table[j + 1].end;
                board.mac_address.mac_table[j].vlan_id = board.mac_address.mac_table[j + 1].vlan_id;
                board.mac_address.mac_table[j].port_vector =
                    board.mac_address.mac_table[j + 1].port_vector;
                memcpy(board.mac_address.mac_table[j].mac_addr,
                       board.mac_address.mac_table[j + 1].mac_addr,6);

            }//end for(j = i;...);

        }//end if(!Str_cmp)
        //printf("delete board mac address sucessful\n\r");
        if(is_delete) {
            break;
        }
    }//end for(i = 0;...)
    //printf("after delete board mac \n\r");
    //printBoardInfo("config_mac");
}

/**
* Function: getDivideMacAddress
* Description: 把XX-XX-XX-XX-XX-XX字符串形式表示的MAC地址，转换成对应的十六进制数组的形式进行存储
* @param des_hex_mac 要存放十六进制MAC地址的数组
* @param src_mac_address 要转换的源字符串MAC地址
**/
int getDivideMacAddress(unsigned char *des_hex_mac,char *src_mac_address) {
    char seps[] = "-";
    char *token;
    unsigned char hight_bit;
    unsigned char low_bit;
    int i = 0 ;
    char temp[50];
    strcpy(temp,src_mac_address);
    for(token = strtok(temp,seps); token; token = strtok(NULL,seps)) {
        /* 得到高四位 */
        if(token[0] >= 'A' && token[0] <= 'Z') {
            hight_bit = (token[0] - '7') << 4;
        } else if(token[0] >= 'a' && token[0] <= 'z') {
            hight_bit = (token[0] - 'W') << 4;
        } else {
            hight_bit = (token[0] - '0') << 4;
        }
        /* 得到低四位*/
        if(token[1] >= 'A' && token[1] <= 'Z') {
            low_bit = (token[1] - '7');
        } else if(token[1] >= 'a' && token[1] <= 'z') {
            low_bit = (token[1] - 'W');
        } else {
            low_bit = (token[1] - '0');
        }
        des_hex_mac[i++] = hight_bit | low_bit;
        //printf("%s\n",token);
    }
    return 0;
}
/**
* Function: getPortsVectorArray
* Description: 把端口向量转换成数组形式
* @param ports_vector 被转换的向量，整数表示
* @param vector_array 存放转换后数据的数组
**/
int getPortsVectorArray(uint32_t ports_vector,int *vector_array) {
    int i;
    for(i = 0; i < ports_num; i++) {
        vector_array[i] = (ports_vector & (0x01 << i)) ? 1 : 0;
    }
}
/**
* Function: concatVectorString
* Description: 把端口的数组形式表示的向量拼接成要发送的字符串的形式，方便web页面进行解析
* @param des_str 要发送的端口向量字符串
* @param ports_vector 被转化的端口向量数组表示形式
* @return 正常结束返回0
**/
int concatVectorString(char*des_str,int*ports_vector) {
    int i;
    char vector_str[5];
    for(i = 0; i < ports_num; i++) {
        memset(vector_str,0,sizeof(vector_str));
        if(i != ports_num - 1) {
            sprintf(vector_str,"%d/",ports_vector[i]);
        } else {
            sprintf(vector_str,"%d|",ports_vector[i]);
        }
        strcat(des_str,vector_str);
    }
    return 0;
}

