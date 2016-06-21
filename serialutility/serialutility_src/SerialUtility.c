/*******************************************************
File name: SerialUtility.c
Description: 提供了操作串口服务器相关的对外接口函数
Author: dujiancheng
Version: v1.0
********************************************************/
#include "SerialUtility.h"
#include "SerialClient.h"
#include "SerialServer.h"
#include "SerialServerUdp.h"
#include "veth_COM.h"
#include "LogUtility.h"
#include "ip_config.h"
uint32_t server_queue_all_length = 0; //添加到串口服务器队列的数据总长度
uint32_t serial_use_memory[MAX_SERIAL_NUM];  //串口使用的内存值,数组下标是对应的串口号
uint32_t serial_server_queue_total[4];
/**
 * Function: initAllQueueArray
 * Description: 初始化串口队列数组，把所有串口收数据队列和发数据队列分别放到两个数组中
**/
void initAllQueueArray() {
    int i;
    /* 把所有的队列初始为空值 */
    for(i = 0; i < MAX_QUEUE_NUM; i++) {
        //initQueue(array_queue[i].queue);
        server_array_queue[i].queue = NULL;
        server_array_queue[i].empty_queue = 1;
        server_array_queue[i].serial_no = 200;
        /* 结束对串口服务器数组的初始化 */

        client_array_queue[i].queue = NULL;
        memset(client_array_queue[i].remote_ip,0,sizeof(client_array_queue[i].remote_ip));
        client_array_queue[i].empty_queue = 1;
        client_array_queue[i].serial_no = 200;
        client_array_queue[i].remote_port = 0;
    }
	for(i = 0; i < MAX_SERIAL_NUM;i++){
		serial_use_memory[i] = 0;
		serial_server_queue_total[i] = 0;
	}
}
/**
 * Function: addDataToServerQueue
 * Description: 向串口服务器队列中添加数据，添加原则：先从所有队列中查找，是否存在的对应的串口队列
 *				如果存在则之间添加，如果不存在，则从所有队列中找到一个可用的队列，添加。
 * @param data  要添加的数据
 * @param data_len 添加的数据长度
 * @param serial_no 队列队列的串口号
**/
void addDataToServerQueue(uint8_t *data,uint32_t data_len,uint8_t serial_no) {
    int i;
    int queue_index = -1;
    struct queue_node *temp;
	temp = (struct queue_node *)malloc(sizeof(struct queue_node));
	if( !temp){
		printf("add server queue malloc failed\n\r");
		return;
	}
    temp->content_length = data_len;
	temp->content_buf = (uint8_t*)malloc(data_len * sizeof(char));
    if( temp->content_buf == NULL) {
        printf("server queue malloc failed\n\r");
		return;
    }
    memset(temp->content_buf,0,data_len*sizeof(char));
    memcpy(temp->content_buf,data,data_len);
	serial_server_queue_total[serial_no] += data_len;
    /* 首先遍历所有的队列查看对应的串口队列是否存在 */
    for(i = 0; i < MAX_QUEUE_NUM; i++) {
        if(server_array_queue[i].empty_queue == 1) {
            continue;
        }
        if(server_array_queue[i].serial_no== serial_no) { //找到了串口对应的队列
            queue_index = i;
            break;
        }
    }
    /* 对应的串口队列不存在,在所有的队列数组中找到可用的位置添加进去 */
    if(queue_index == -1) {
        for(i = 0; i < MAX_QUEUE_NUM; i++ ) {
            if(server_array_queue[i].empty_queue == 1) { //找到可用的队列位置
                server_array_queue[i].empty_queue = 0;
                /* 创建一个新的队列 */
				server_array_queue[i].queue = (struct serial_queue*)malloc(sizeof(struct serial_queue));
				if(server_array_queue[i].queue == NULL){
					printf("server queue malloc failed\n\r");
					return;
				}
                server_array_queue[i].queue->head = NULL;
                server_array_queue[i].queue->tail = NULL;
                server_array_queue[i].serial_no = serial_no;
                queue_index = i;
                break;
            }
        }
    }

    if(queue_index == -1) {
        printf("queue is full,no avaliable queue\n\r");
        return;
    }
    /* 添加数据到队列 */
    addQueueNode(server_array_queue[queue_index].queue,temp);
    server_queue_all_length += temp->content_length;
}
/**
 * Function: addDataToClientQueue
 * Description: 向串口客户端队列中添加数据，添加原则：先从所有队列中查找，是否存在的对应的串口队列
 *				如果存在则之间添加，如果不存在，则从所有队列中找到一个可用的队列，添加。
 * @param data  要添加的数据
 * @param data_len 添加的数据长度
 * @param serial_no 队列队列的串口号
**/
void addDataToClientQueue(uint8_t *data,uint32_t data_len,uint8_t serial_no) {
    int i;
    int queue_index = -1;
    struct queue_node *temp;
	if(serial_use_memory[serial_no] >= SERIAL_QUEUE_MAX_MEMORY){
		printf("serial %d use memory is %d beyond max limit,data will drop\n\r",serial_no + 1,serial_use_memory[serial_no]);
		return;
	}
	temp = (struct queue_node *)malloc(sizeof(struct queue_node));
	if( !temp ){
		printf("add client queue malloc failed\n\r");
		return;
	}
    temp->content_length = data_len;
	temp->content_buf = (uint8_t*)malloc(data_len);
    memset(temp->content_buf,0,data_len);
    memcpy(temp->content_buf,data,data_len);
	/* 记录该串口使用的内存总量 */
	serial_use_memory[serial_no] += data_len;
    /* 首先遍历所有的队列查看对应的串口队列是否存在 */
    for(i = 0; i < MAX_QUEUE_NUM; i++) {
        if(client_array_queue[i].empty_queue == 1) {
            continue;
        }
        if(client_array_queue[i].serial_no== serial_no) { //找到了串口对应的队列
            queue_index = i;
            break;
        }
    }
    /* 对应的串口队列不存在,在所有的队列数组中找到可用的位置添加进去 */
    if(queue_index == -1) {
        for(i = 0; i < MAX_QUEUE_NUM; i++ ) {
            if(client_array_queue[i].empty_queue == 1) { //找到可用的队列位置
                client_array_queue[i].empty_queue = 0;
                /* 创建一个新的队列 */
                //client_array_queue[i].queue = (struct serial_queue*)vethMalloc(sizeof(struct serial_queue));
				client_array_queue[i].queue = (struct serial_queue*)malloc(sizeof(struct serial_queue));
				if(client_array_queue[i].queue == NULL){
					printf("client queue malloc failed\n\r");
					return;
				}
                client_array_queue[i].queue->head = NULL;
                client_array_queue[i].queue->tail = NULL;
                client_array_queue[i].serial_no = serial_no;
                queue_index = i;
                break;
            }
        }
    }

    if(queue_index == -1) {
        printf("queue is full,no avaliable queue\n\r");
        return;
    }
    /* 添加数据到队列 */
    addQueueNode(client_array_queue[queue_index].queue,temp);
}
/**
 * Function: getServerSerialData
 * Description: 从串口服务器队列中读取数据
 * @param serial_no  对应的串口号
 * @param data_length  存放读取的实际长度
 * @param current_total_length 目前已经读取的总长度
 * @param max_length 需要读取的最大长度
 * @return 读取到的队列里面的数据
 * @ATTENTION 返回值使用的空间是动态生成的，调用者需要手动释放返回值空间
**/
char* getServerSerialData(uint8_t serial_no,uint32_t *data_length,uint32_t current_total_length,uint32_t max_length) {
    int i;
    int queue_index = -1;
    char *get_content;
    for(i = 0; i < MAX_QUEUE_NUM; i++) {
        if(server_array_queue[i].empty_queue == 1) {
            continue;
        }
        if(server_array_queue[i].serial_no == serial_no) {
            queue_index = i;
            break;
        }
    }
    if(queue_index == -1) {
        get_content = NULL;
        return NULL;
    }
    /* 判断该队列中的数据是否已经取完,如果取完则把对应的队列置空,循环利用 */
    if(server_array_queue[queue_index].queue->head == NULL) {
        server_array_queue[queue_index].empty_queue = 1;
		free(server_array_queue[queue_index].queue);
        server_array_queue[queue_index].queue = NULL;

        get_content = NULL;
        return NULL;
    }
    get_content = getQueueData(server_array_queue[queue_index].queue,data_length,current_total_length,max_length);
    return get_content;
}
/**
 * Function: getClientSerialData
 * Description: 从串口客户端队列中读取数据
 * @param serial_no  对应的串口号
 * @param data_length  存放读取的实际长度
 * @param current_total_length 目前已经读取的总长度
 * @param max_length 需要读取的最大长度
 * @return 读取到的队列里面的数据
 * @ATTENTION 返回值使用的空间是动态生成的，调用者需要手动释放返回值空间
**/
char* getClientSerialData(uint8_t serial_no,uint32_t *data_length,uint32_t current_total_length,uint32_t max_length) {
    int i;
    int queue_index = -1;
    char *get_content;
    for(i = 0; i < MAX_QUEUE_NUM; i++) {
        if(client_array_queue[i].empty_queue == 1) {
            continue;
        }
        if(client_array_queue[i].serial_no == serial_no) {
            queue_index = i;
            break;
        }
    }
    if(queue_index == -1) {
        printf("串口 %d 客户端中对应的队列不存在\n\r",serial_no);
        return NULL;
    }

    /* 判断该队列中的数据是否已经取完,如果取完则把对应的队列置空,循环利用 */
    if(client_array_queue[queue_index].queue->head == NULL) {
		free(client_array_queue[queue_index].queue);
        client_array_queue[queue_index].queue = NULL;
        client_array_queue[queue_index].empty_queue = 1;
        get_content = NULL;
        *data_length = 0;
        return NULL;
    }
    get_content = getQueueData(client_array_queue[queue_index].queue,data_length,current_total_length,max_length);
	/* 读取串口数据,修改对应的内存使用量 */
	serial_use_memory[serial_no] -= *data_length;
    return get_content;
}
/**
 * Function: getSerialConfig
 * Description: 生成串口配置信息
 * @param serial_no 生成的信息的串口号
 * @return 格式化以后要返回给浏览器的数据
**/
char *getSerialConfig(uint8_t serial_no) {
    int i;
    static char serial_config_data[1324];
    char serial_ports[300];
    char temp_serial[15];
    char remote_config[1024];
    char temp_remote[50];

    char local_serial[100];

    memset(serial_config_data,0,sizeof(serial_config_data));
    memset(serial_ports,0,sizeof(serial_ports));
    memset(remote_config,0,sizeof(remote_config));
    /* 生成串口占用的端口号 */
    for(i = 0; i < MAX_SERIAL_NUM; i++) {
        memset(temp_serial,0,sizeof(temp_serial));
        sprintf(temp_serial,"%u/",ntohs(board.serial_config[i].serial_listen_port));
        strcat(serial_ports,temp_serial);
    }

    /* 生成某个串口对应的本地配置参数 */
    sprintf(local_serial,"%d/%d/%d/%d/%d/%.1f/%d/",board.serial_mode.com1_2_mode,board.serial_mode.com3_4_mode,
            board.serial_config[serial_no].serial_bound_rate,board.serial_config[serial_no].serial_data_bit,
            board.serial_config[serial_no].serial_parity,board.serial_config[serial_no].serial_stop_bit,
            board.serial_config[serial_no].serial_listen_port);
    strcat(serial_ports,local_serial);
	sprintf(temp_remote,"%s/%d/%d|",board.serial_remote_host[serial_no].remote_ip,
					board.serial_remote_host[serial_no].remote_port,board.serial_remote_host[serial_no].is_use_udp);
	strcat(remote_config,temp_remote);
    serial_ports[strlen(serial_ports) - 1] = '|'; //添加|分隔符
    strcat(serial_config_data,serial_ports);
    strcat(serial_config_data,remote_config);
    //printf("generate data is %s\n\r",serial_config_data);
    return serial_config_data;
}
/**
 * Function: isSerialListenPort
 * Description: 判断端口号是否是串口监听的串口号
 * @param port 要判断的端口号
 * @return 1 是串口监听的端口号
 * 		   0 不是串口监听的端口号
**/
int isSerialListenPort(uint16_t port) {
    int i;
    for(i = 0; i < MAX_SERIAL_NUM; i++) {
        if(board.serial_config[i].serial_listen_port == port) {
            return 1;
        }
    }
    return 0;
}
/**
 * Function: closeSerialClientConnection
 * Description: 关闭串口客户端的连接，如果串口对应的连接存在的话
 * @param serial_no 要关闭的连接对应的串口号
**/
static void closeSerialClientConnection(uint8_t serial_no){
	int i;
	for(i = 0; i < MAX_SERIAL_NUM;i++){
		if(serial_client.serial_obj[i].serial_no == serial_no && serial_client.serial_obj[i].pcb){
			tcp_close(serial_client.serial_obj[i].pcb);
			serial_client.serial_obj[i].pcb = NULL;
			return;
		}
	}
}

/**
 * Function: modifySerialConfig
 * Description: 根据用户提交的串口配置表单，修改串口配置信息
 * @param form_content 用户提交的串口配置的表单数据
**/
int modifySerialConfig(char *form_content) {
    int i;
    int add_ret;
    int modify_result;
    char *form_content_start = NULL;  //得到表单数据开始的位置
    char *isConfigLocalServer = NULL;  //判断用户是否需要配置本地serial
    char *serial_no_value = NULL;    //本地的串口号
    char *serial_1_2_mode_value = NULL;    //串口1和2的模式
	char *serial_3_4_mode_value = NULL;    //串口3和4的模式
    char *serial_bound_rate = NULL;     //串口波特率
    char *serial_data_bit = NULL;      //串口数据位
    char *serial_parity = NULL;        //串口的校验
    char *serial_stop_bit = NULL;      //串口的停止位
    char *serial_listen_port = NULL;    //串口的监听端口号

    //char delete_name[15];     //删除的参数名称
    char *delete_value = NULL;       //删除选项是否选择
    //char remote_ip_name[20];  //远端IP地址的参数名称
    char *remote_ip_value = NULL;    //远端IP数值
    //char remote_port_name[20];  //远端的端口参数名称
    char *remote_port_value = NULL;   //远端的端口的数值
	char *is_use_udp = NULL;          //是否使用UDP协议进行数据传输
	uint8_t use_udp = 0;               //对应的UDP开启或者关闭的数值
   // char remote_serial_no_name[20];   //要和远端通信的串口号对应的参数名称
    //char *remote_serial_no_value = NULL;     //要和远端通信的串口号对应的数值
	char message[96];
    form_content_start = getHttpFormStartLocation(form_content);
	serial_no_value = getParameter(form_content_start,"ableSerialNo"); //读取串口号
    /* 得到本地串口配置相关的数据 */
    isConfigLocalServer = getParameter(form_content_start,"isConfigSerial");
    if(atoi(isConfigLocalServer) == 1) { //需要配置本地串口参数
        
        serial_1_2_mode_value = getParameter(form_content_start,"COM1_2_serial_mode");
		serial_3_4_mode_value = getParameter(form_content_start,"COM3_4_serial_mode");
        serial_bound_rate = getParameter(form_content_start,"bound_rate");
        serial_data_bit = getParameter(form_content_start,"data_bit");
        serial_parity = getParameter(form_content_start,"parity");
        serial_stop_bit = getParameter(form_content_start,"stop_bit");
        serial_listen_port = getParameter(form_content_start,"listen_serial_no");
        /* 更新本地串口参数的配置 */
        modify_result = modifyLocalSerialConfig((atoi(serial_no_value) - 1),atoi(serial_1_2_mode_value),atoi(serial_3_4_mode_value),atoi(serial_bound_rate),
                                                atoi(serial_data_bit),atoi(serial_parity),atof(serial_stop_bit),
                                                atoi(serial_listen_port));

        if(modify_result) {
            //printf("modify local serial config sucessful\n\r");
        }
        /*释放动态开辟的空间*/
        
        freeMemory(serial_1_2_mode_value);
		freeMemory(serial_3_4_mode_value);
        freeMemory(serial_bound_rate);
        freeMemory(serial_data_bit);
        freeMemory(serial_parity);
        freeMemory(serial_stop_bit);
        freeMemory(serial_listen_port);
    }
	freeMemory(isConfigLocalServer);
    /* 得到远端配置的信息 */
        delete_value = getParameter(form_content_start,"delete_entry");
        //sprintf(remote_serial_no_name,"serial_no_%d",(i + 1));
        //remote_serial_no_value = getParameter(form_content_start,remote_serial_no_name);
        if(!strcmp(delete_value,"1") ) { //删除当前配置
			closeSerialClientConnection((atoi(serial_no_value) - 1));//删除对应的连接，如果连接存在的话
            deleteSerialRemoteEntry(remote_ip_value,atoi(remote_port_value),(atoi(serial_no_value) - 1));
            freeMemory(delete_value);
			memset(message,0,sizeof(message));
			sprintf(message,"删除串口 %d 的远程IP %s",atoi(serial_no_value),remote_ip_value);
			addLogEntry(INFO,"vorx",message);  
			printSystemLog();
			return 0;
        }
		//sprintf(remote_ip_name,"remote_ip",(i + 1));
        remote_ip_value = getParameter(form_content_start,"remote_ip");
		//sprintf(remote_port_name,"remote_port",(i + 1));
        remote_port_value = getParameter(form_content_start,"remote_port");
		is_use_udp = getParameter(form_content_start,"use_udp");
		if(!strcmp(is_use_udp,"1")){
			use_udp = 1;
		}else{
			use_udp = 0;
		}
        //不存在该配置，则添加到数组中
        add_ret = modifyRemoteConfig(remote_ip_value,atoi(remote_port_value),use_udp,(atoi(serial_no_value) - 1));
		if(!add_ret) {
            printf("添加失败，没有可用的空间");
        }
        freeMemory(delete_value);
        freeMemory(remote_ip_value);
        freeMemory(remote_port_value);
        //freeMemory(remote_serial_no_value);
   // }
	freeMemory(serial_no_value);
    return 0;
}

/**
 * Function: deleteSerialRemoteEntry
 * Description: 删除board结构体中某一个串口配置的远程主机
 * @param remote_ip 远程主机的IP地址
 * @param remote_port 远程主机的端口号
 * @param serial_no 远程主机对应的串口号
 * @return 1 函数正常执行结束
**/
int deleteSerialRemoteEntry(char *remote_ip,uint16_t remote_port,uint8_t serial_no) {
    int i,j;
    //int isDelete = 0;
	/* 需要删除对应的keep alive，如果对应的keep alive存在的话 */
	//isNeedDeleteKeepAlive(remote_ip,remote_port);
	/* 一个串口对应一个远程配置的删除方式 */
	memset(board.serial_remote_host[serial_no].remote_ip,0,sizeof(board.serial_remote_host[serial_no].remote_ip));
	board.serial_remote_host[serial_no].remote_port = 0;
	board.serial_remote_host[serial_no].is_use_udp = 0;
	/* 之前的删除多个选项的方法 */
    return 1;
}
/**
 * Function: deleteSerialRemoteEntry
 * Description: 判断某一个串口是否配置了远程主机的信息
 * @param remote_ip 远程主机的IP地址
 * @param remote_port 远程主机的端口号
 * @param serial_no 远程主机对应的串口号
 * @return 1 远程主机的配置存在
 *         0 对应的远程主机的配置不存在
**/
int isExistRemoteConfig(char *remote_ip,uint16_t remote_port,uint8_t serial_no) {
    int i;
    for(i = 0; i < MAX_SERIAL_REMOTE_NUM; i++) {
        if(board.serial_remote_host[i].end == 1) { //该条件已经说明不存在该配置,可以直接返回
            break;
        }
        if( !strcmp(board.serial_remote_host[i].remote_ip,remote_ip)
                && board.serial_remote_host[i].remote_port == remote_port
                && board.serial_remote_host[i].serial_no == serial_no
          ) {   //找到相同的选项
            return 1;
        }

    }
    return 0;
}
/**
 * Function: modifyRemoteConfig
 * Description: 修改某一个串口的远程主机的配置
 * @param remote_ip 远程主机的IP地址
 * @param remote_port 远程主机的端口号
 * @param use_udp 是否使用了UDP协议进行传输
 * @param serial_no 远程主机对应的串口号
 * @return 1 函数正常执行 
**/
int modifyRemoteConfig(char *remote_ip,uint16_t remote_port,uint8_t use_udp,uint8_t serial_no){
	/* 串口号和数组的下标对应 */
	char message[96];
	if( strcmp(board.serial_remote_host[serial_no].remote_ip,remote_ip)){  //远程IP被修改或者添加
		/* 远程的IP地址被修改，需要删除对应的keep alive
		  如果对应的keep alive存在的话 */
		//isNeedDeleteKeepAlive(remote_ip,remote_port);
		closeSerialClientConnection(serial_no);   //删除存在的连接
		strcpy(board.serial_remote_host[serial_no].remote_ip,remote_ip);
		memset(message,0,sizeof(message));
		sprintf(message,"修改了串口 %d 的远程IP %s",serial_no + 1,remote_ip);
		addLogEntry(INFO,"vorx",message);  
		printSystemLog();
	}
	if(board.serial_remote_host[serial_no].remote_port != remote_port){
		/* 远程的port号被修改，需要删除对应的keep alive，如果对应的keep alive存在的话 */
		//isNeedDeleteKeepAlive(remote_ip,remote_port);
		closeSerialClientConnection(serial_no);  //删除对应的连接
		board.serial_remote_host[serial_no].remote_port = remote_port;
		memset(message,0,sizeof(message));
		sprintf(message,"修改了串口 %d 的远程端口号 %d",serial_no + 1,remote_port);
		addLogEntry(INFO,"vorx",message);  
		printSystemLog();
	}
	if(board.serial_remote_host[serial_no].is_use_udp != use_udp){
		board.serial_remote_host[serial_no].is_use_udp = use_udp;
		if(use_udp == 1){  //打开了UDP选项
			memset(message,0,sizeof(message));
			sprintf(message,"串口 %d 开启了UDP传输模式",serial_no + 1);
			addLogEntry(INFO,"vorx",message);  
			printSystemLog();
		}else{  //关闭了UDP开关
			memset(message,0,sizeof(message));
			sprintf(message,"串口 %d 关闭了UDP传输模式",serial_no + 1);
			addLogEntry(INFO,"vorx",message);  
			printSystemLog();
		}
	}
	
	return 1;
}
/**
 * Function: modifyLocalSerialConfig
 * Description: 修改某一个串口的配置参数信息
 * @param serial_no 远程主机对应的串口号
 * @param serial_mode_1_2 串口1~2口的串口模式
 * @param serial_mode_3_4 串口3~4口的串口模式
 * @param serial_bound_rate 串口的波特率
 * @param serial_data_bit   串口的数据位
 * @param serial_parity     串口的校验位
 * @param serial_stop_bit   串口的停止位
 * @param serial_listen_port  串口监听的端口号
 * @return 1 函数正常执行 
**/
int modifyLocalSerialConfig(uint8_t serial_no,uint16_t serial_mode_1_2,uint16_t serial_mode_3_4,uint32_t serial_bound_rate,
                            uint8_t serial_data_bit,uint8_t serial_parity,float serial_stop_bit,
                            uint16_t serial_listen_port) {
    //printf("modify serial no is %d\n\r",serial_no);
	int retValue;
	char message[96];
	char parity_str[10];
    if(board.serial_mode.com1_2_mode != serial_mode_1_2) {
        board.serial_mode.com1_2_mode = serial_mode_1_2;
		if(board.serial_mode.com1_2_mode == 232){
			memset(message,0,sizeof(message));
			sprintf(message,"修改串口1~2为RS232模式");
			addLogEntry(INFO,"vorx",message);  
			printSystemLog();
			setChipMode(0,RS232_MODE);
		}else{
			memset(message,0,sizeof(message));
			sprintf(message,"修改串口1~2为RS485模式");
			addLogEntry(INFO,"vorx",message);  
			printSystemLog();
			setChipMode(0,RS485_MODE);
		}
    }
	if(board.serial_mode.com3_4_mode != serial_mode_3_4) {
        board.serial_mode.com3_4_mode = serial_mode_3_4;
		if(board.serial_mode.com3_4_mode == 232){
			memset(message,0,sizeof(message));
			sprintf(message,"修改串口3~4为RS232模式");
			addLogEntry(INFO,"vorx",message);  
			printSystemLog();
			setChipMode(1,RS232_MODE);
		}else{
			memset(message,0,sizeof(message));
			sprintf(message,"修改串口3~4为RS485模式");
			addLogEntry(INFO,"vorx",message);  
			printSystemLog();
			setChipMode(1,RS485_MODE);
		}
    }
	
    if(board.serial_config[serial_no].serial_bound_rate != serial_bound_rate) {
        board.serial_config[serial_no].serial_bound_rate = serial_bound_rate;
		memset(message,0,sizeof(message));
		sprintf(message,"修改串口 %d 的波特率为 %d",serial_no + 1,serial_bound_rate);
		addLogEntry(INFO,"vorx",message);  
		printSystemLog();
    }
    if(board.serial_config[serial_no].serial_data_bit != serial_data_bit) {
        board.serial_config[serial_no].serial_data_bit = serial_data_bit;
		memset(message,0,sizeof(message));
		sprintf(message,"修改串口 %d 的数据位为 %d",serial_no + 1,serial_data_bit);
		addLogEntry(INFO,"vorx",message);  
		printSystemLog();
    }

    if(board.serial_config[serial_no].serial_parity != serial_parity) {
        board.serial_config[serial_no].serial_parity = serial_parity;
		memset(parity_str,0,sizeof(parity_str));
		if(serial_parity == 0){
			strcpy(parity_str,"无校验");
		}else if(serial_parity == 1){
			strcpy(parity_str,"奇校验");
		}else{
			strcpy(parity_str,"偶校验");
		}
		memset(message,0,sizeof(message));
		sprintf(message,"修改串口 %d 的校验为 %s",serial_no + 1,parity_str);
		addLogEntry(INFO,"vorx",message);  
		printSystemLog();
    }

    if(board.serial_config[serial_no].serial_stop_bit != serial_stop_bit) {
        board.serial_config[serial_no].serial_stop_bit = serial_stop_bit;
		memset(message,0,sizeof(message));
		sprintf(message,"修改串口 %d 的停止位为 %f",serial_no + 1,serial_stop_bit);
		addLogEntry(INFO,"vorx",message);  
		printSystemLog();
    }
    if(board.serial_config[serial_no].serial_listen_port != serial_listen_port) {
        /* 如果之前监听过该端口应该先把之前的监听取消 */
        if(board.serial_config[serial_no].serial_listen_port != 0) {
            deleteSerialServerListen(board.serial_config[serial_no].serial_listen_port);
			unlistenUdpSerialPort(board.serial_config[serial_no].serial_listen_port,serial_no);
        }
        board.serial_config[serial_no].serial_listen_port = serial_listen_port;
        /* 把新的监听端口添加到监听队列 */
		addSerialServerListen(serial_listen_port,serial_no + 1);
		addUdpSerialListenPort(serial_listen_port,serial_no);  //修改udp服务器监听的端口号
		memset(message,0,sizeof(message));
		sprintf(message,"修改串口 %d 的监听端口号为 %d",serial_no + 1, board.serial_config[serial_no].serial_listen_port);
		addLogEntry(INFO,"vorx",message);  
		printSystemLog();
    }
	retValue = setComProperty(&board.serial_config[serial_no]);
	if(retValue){
		//printf("serial modify sucessful\n\r");
	}else{
		//printf("serial modify ");
	}
    return 1;
}

/**
 * Function: readClientQueueToBuf
 * Description: 读取串口客户端的数据到缓冲buf中
 * @param buf 接收数据的buf
 * @param length 需要读取的数据长度
 * @param serial_no 数据对应的串口号
 * @return 实际读取到的数据长度 
**/
int readClientQueueToBuf(char *buf,int length,uint8_t serial_no) {
    char *read_loc = NULL;  //位置指针
    int read_length = 0; //读取的长度
    int isQueueEmpty = 0;
    char *serial_client_data = NULL;  //存放读到的数据
    uint32_t data_length = 0;  //读到队列中的数据长度
    do {
        read_loc = &buf[read_length];
        serial_client_data = getClientSerialData(serial_no,&data_length,read_length,length);
        if( !serial_client_data) {
            isQueueEmpty = 1;
        }else{
			memcpy(read_loc,serial_client_data,data_length);
			read_length += data_length;
		}
		free(serial_client_data);
        serial_client_data = NULL;
        data_length = 0;
    } while( !isQueueEmpty && read_length < length);;
    return read_length;
}

/**
 * Function: checkLocalSerialTransmit
 * Description: 检测串口的数据是否要发回到本机
 * @param remote_ip 远程主机的IP地址
 * @return 1 数据是本机到本机的环回
 * 		   0 数据不是环回数据
**/
uint8_t checkLocalSerialTransmit(char *remote_ip){
	char backup_remote_ip[40];
	ip_addr_t remote_addr;
	uint8_t format_ip[4];
	memset(backup_remote_ip,0,sizeof(backup_remote_ip));
	strcpy(backup_remote_ip,remote_ip);
	getFormatAddress(format_ip,backup_remote_ip);
    IP4_ADDR(&remote_addr,format_ip[0],format_ip[1],format_ip[2],format_ip[3]);
	if(ip_addr_cmp(&remote_addr, &veth_netif.ip_addr)){  //远程主机是本机，
		return 1;
	}
	return 0;
}
/* 
*/
/**
 * Function: transmitLocalSerial
 * Description: 本地串口之间的数据转发,根据串口监听的端口号实现本地串口数据之间的转发 
 * 				读取串口客户端队列里面的数据，模拟网络的数据发送，每次往串口服务器队列中
 * 				加载的数据最大为2 * MSS这和网络上收到的数据最大长度保持了一致
 * 				每次从串口客户端队列中读取的数据长度为CLIENT_DATA_BUF_LENGTH
 * @param listen_port 监听的端口号
 * @return 1 程序正常执行
 * 		  
**/
uint8_t transmitLocalSerial(uint16_t listen_port){
	uint8_t *serial_client_buf = NULL;
	uint8_t send_buf[2 * TCP_MSS];
	int i;
	int read_length = 0;
	int loc = 0;
	int left = 0;  //剩余的长度
	serial_client_buf = (uint8_t *)malloc(CLIENT_DATA_BUF_LENGTH * sizeof(uint8_t));
	for(i = 0; i < MAX_SERIAL_NUM; i++){
		if(board.serial_config[i].serial_listen_port == listen_port){   //找到该监听端口对应的串口号
			break;
		}
	}
	read_length = readClientQueueToBuf((char*)serial_client_buf,CLIENT_DATA_BUF_LENGTH,i);
	left = read_length;
	loc = 0;
	while(left > 0){
		memset(send_buf,0,sizeof(send_buf));
		if(left >= 2 * TCP_MSS){   //一次最多只挂载2 * MSS
			memcpy(send_buf,(serial_client_buf + loc),2 * TCP_MSS);
			left -= 2 * TCP_MSS;
			loc += 2 * TCP_MSS;
			addDataToServerQueue(send_buf,2 * TCP_MSS,i);
		}else{
			memcpy(send_buf,(serial_client_buf + loc),left);
			loc += left;
			addDataToServerQueue(send_buf,left,i);
			left = 0;
		}
		
	}
	free(serial_client_buf);
	serial_client_buf = NULL;
	return 1;
}