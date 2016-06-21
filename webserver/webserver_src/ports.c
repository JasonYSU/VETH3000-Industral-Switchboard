/*******************************************************
File name: ports.c
Description: 实现了对端口操作的各种操作接口函数
Author: dujiancheng
Version: v1.0
********************************************************/
#include "ports.h"
#include "LogUtility.h"
int band_width_refresh_time; //记录刷新带宽利用率经过的时间
char *upDown[]= {"Down","Up"};
char *speedType[]= {"","10M 半双工","10M 全双工","100M 半双工","100M 全双工","1G 全双工","2.5G 全双工"};
char *link_down[]= {"down","link"};
//char *port_nick[12]={"默认","默认","默认","默认","默认","默认","默认","默认","默认","默认","默认","默认"};
char *top_bottom[]= {"top","bottom"};
volatile uint64_t pre_rx_byte[PORTSNUM];
volatile uint64_t pre_tx_byte[PORTSNUM];
volatile PORTSBANDWIDTH ports_pre_band_width[PORTSNUM];
static int current_light_num;
static int is_first_light_port = 0;  //记录是否是第一个光口
/**
 * Function: initPorts
 * Description: 初始化端口配置信息，系统启动的时候该函数被调用一次
 * 
**/
void initPorts() {
    if(isFirstRequestPorts) {
        isFirstRequestPorts = FALSE;
        recoverConfigEntries("config_ports");
    }
}
/**
 * Function: initPreBandWidth
 * Description: 初始化记录上一次端口带宽的数组
 * 
**/
int initPreBandWidth() {
    memset((uint64_t*)pre_rx_byte,0,sizeof(pre_rx_byte));
    memset((uint64_t *)pre_tx_byte,0,sizeof(pre_tx_byte));
}
/**
 * Function: updatePreBandWidth
 * Description: 更新记录的上一次的端口带宽的数组
 * 
**/
int updatePreBandWidth() {
    int port_no;
    volatile API_ROUGH_COUNTER_T count;
    memset((uint64_t *)pre_rx_byte,0,sizeof(pre_rx_byte));
    memset((uint64_t *)pre_tx_byte,0,sizeof(pre_tx_byte));
    for(port_no = 0; port_no < ports_num; port_no++) {
        Get_Port_Counters_rou(&count,port_no);
        pre_rx_byte[port_no] = count.RxGoodOctets;
        pre_tx_byte[port_no] = count.TxGoodOctets;
        ports_pre_band_width[port_no].pre_curve_rx_byte = count.RxGoodOctets;
        ports_pre_band_width[port_no].pre_curve_tx_byte = count.TxGoodOctets;
        ports_pre_band_width[port_no].band_width_curve_refresh_time = msCount;
    }
}
/**
 * Function: clearPreBandWidth
 * Description: 清除记录的上一次的端口带宽的数组
 * 
**/
int clearPreBandWidth() {
    memset((uint64_t *)pre_rx_byte,0,ports_num * sizeof(uint64_t));
    memset((uint64_t *)pre_tx_byte,0,ports_num * sizeof(uint64_t));
    return 0;
}
char *generatePortsData() {
    static char portsState[PORTS_SIZE];
    char rowData[800];
    int i;
    /* 定义端口的各个状态值 */
    unsigned char port_type; //端口是电口还是光口
    unsigned char port_fc = 2; //端口流控的收发状态以及开启选项是否可见，默认是2

    unsigned char port_link; //端口的link状态是up还是down
    unsigned char port_rxpause; //端口流控的发送状态
    unsigned char port_txpause; //端口流控的接收状态
    unsigned char port_fiber_speed = 0; //端口光纤速率，默认0
    char link_speed[20];  //端口链接的速率


    VETH_TYPE_T  device;
    initPorts();
    getSetType(&device);   //获取设备信息
    memset(portsState,0,sizeof(portsState));
    portsState[0] = 0;
    for(i = 0; i < ports_num; i++) {
        port_type = getPortType(i);

        if(port_type) { //光口
            board.ports_config.port_auto[i] = 0;
        }

        port_link = Get_link_status(i);
        
        if(port_link && is_open_delay_config_ports == 0) {   //不是端口延迟配置模式
            board.ports_config.port_speed[i] = Get_speed_status(i);
            board.ports_config.port_fdx[i] = Get_duplex_status(i);
			port_txpause = port_rxpause = Get_flow_status(i);
        }else{
			port_txpause = port_rxpause = board.ports_config.port_flow[i];
		}

        memset(link_speed,0,sizeof(link_speed));
        sprintf(link_speed,"%d%s%s",(board.ports_config.port_speed[i] == 1000) ? 1 : board.ports_config.port_speed[i],
                (board.ports_config.port_speed[i] == 1000) ? "G" :"M",board.ports_config.port_fdx[i] ? "全双工" : "半双工");
        sprintf(rowData,"%d/%d/%d/%d/%d/%d/%d/%d/%s/%s/%d/%d/%d/%s/%d|",
                i + 1,(port_type == 0) ? ELECT_2_6_CAPABILITY : LIGHT_2_8_CAPABILITY,board.ports_config.speed_select_enable[i],board.ports_config.port_auto[i],board.ports_config.port_speed[i],
                board.ports_config.port_fdx[i],port_fc,board.ports_config.port_flow[i],upDown[port_link],port_link ? link_speed : upDown[port_link],port_rxpause,
                port_txpause,port_fiber_speed,board.ports_config.port_nick[i],port_type);
        strcat(portsState,rowData);
    }
    return portsState;
}
/**
 * Function: generateCountData
 * Description: 生成所有端口粗略的数据信息，包括每个口的带宽利用率的数据。带宽利用率的计算是
 *				根据用户两次刷新的时间间隔和两次的收发字节数进行计算
 *  @return 格式化以后的要发送给客户端的数据
**/
char *generateCountData() {
    volatile API_ROUGH_COUNTER_T count;
    static char all_count[200];
    char one_count[50];
    int port_speed;
    double time;
    int i;
    volatile double Rx_band_width_effective;  //接收的带宽利用率
    volatile double Tx_band_width_effective;  //发送的带宽利用率
    memset(all_count,0,sizeof(all_count));
    all_count[0] = 0;
    time = msCount - band_width_refresh_time;  //计算刷新该页面请求经过的时间
    if(time <= 0) {
        time = 1;
    }
    time = (double)time / 1000;
    for(i = 0; i < ports_num; i++) {
        memset(one_count,0,sizeof(one_count));
        port_speed =  Get_speed_status(i);
        Rx_band_width_effective = 0;
        Tx_band_width_effective = 0;
        Get_Port_Counters_rou(&count,i);
        if(Get_link_status(i)) {
            Rx_band_width_effective = (double)((count.RxGoodOctets  -  pre_rx_byte[i])* 8) / (port_speed * 1000000);
            Tx_band_width_effective = (double)((count.TxGoodOctets  - pre_tx_byte[i])* 8) / (port_speed * 1000000);
            Rx_band_width_effective = (Rx_band_width_effective / time) * 100;
            Tx_band_width_effective = (Tx_band_width_effective / time) * 100;
        } else {
            Rx_band_width_effective = 0;
            Tx_band_width_effective = 0;
        }
        pre_rx_byte[i] = count.RxGoodOctets;
        pre_tx_byte[i] = count.TxGoodOctets;

        sprintf(one_count,"%d/%s/%lld/%lld/%d/%d/%d/%d/%.2lf/%.2lf|",i + 1,board.ports_config.port_nick[i],
                count.RxGoodOctets,count.TxGoodOctets,
                count.RxbadOctets,count.RxDiscard,
                count.RxFiltered,count.TxFiltered,
                Rx_band_width_effective ,Tx_band_width_effective );
        strcat(all_count,one_count);
    }
    band_width_refresh_time = msCount;
    return all_count;
}
/**
 * Function: generatePreviewData
 * Description: 生成端口的预览状态，预览状态包括端口的连接状态，端口的连接速率以及端口类型。是目前页面首页显示的信息
 * @return 格式化以后的要发送给客户端的数据
**/
char *generatePreviewData() {
    int i;
    static char head_str[750];
    char megr_str[700];
    char port_img[50];
    char img[20];
    char port_status[100];
    char center_str[100];
    unsigned char port_link; //端口的link状态是up还是down
    int port_speed; //端口对应的速率
    unsigned char port_fdx; //端口是否是全双工模式
    char link_speed[20];  //端口链接的速率
    unsigned char port_type; //端口是电口还是光口
    int img_location[5];

    VETH_TYPE_T  device;
    getSetType(&device);   //获取设备信息

    memset(head_str,0,sizeof(head_str));
    memset(megr_str,0,sizeof(megr_str));
    strcpy(head_str,"1|switch_small.png|");
	current_light_num = 0;   //从新计数
    for(i = 0; i < device.max_portNo; i++) {
        port_type = getPortType(i);
        port_speed = Get_speed_status(i);
        port_fdx = Get_duplex_status(i);
        port_link = Get_link_status(i);
        memset(link_speed,0,sizeof(link_speed));
        memset(img_location,0,sizeof(img_location));
        getImgLocation(i + 1,img_location);
        sprintf(link_speed,"%d%s%s",(port_speed == 1000) ? 1 : port_speed,(port_speed == 1000) ? "G" :"M",port_fdx ? "全双工" : "半双工");
        if(port_type == 0) {
            strcpy(port_img,"jack_copper_");
            sprintf(img,"%s_%s.png",link_down[port_link],top_bottom[ (i + 1) % 2 ] );
        } else {
            strcpy(port_img,"jack_sfp_");
            sprintf(img,"%s_bottom.png",link_down[port_link]);
        }
        strcat(port_img,img);
        sprintf(port_status,"%s %d: %s",port_type ? "光口" : "电口",(i + 1),port_link ? link_speed :"未连接");
        if(port_type == 0) {
            sprintf(center_str,"%d/%s/%s/%d/%d/%d/%d/%d|",
                    i + 1,port_img,port_status,img_location[0],img_location[1],img_location[2],
                    img_location[3],img_location[4]);
        } else {
            sprintf(center_str,"%d/%s/%s/%d/%d/%d/%d/%d|",
                    i + 1,port_img,port_status,img_location[0],img_location[1],img_location[2],
                    img_location[3],img_location[4]);
        }
        strcat(megr_str,center_str);
    }
    strcat(head_str,megr_str);
    //printf("generate file is %s\n",head_str);
    return head_str;
}
/**
 * Function: modifyPortsConfig
 * Description: 根据用户提交的端口数据，修改端口的配置
 * @param url 用户提交的表单数据，包含了用户设置的端口信息
**/
void modifyPortsConfig(char *url) {
    PORT_STAS ports_status;
    int desPorts[9];
    char speed[20],FlowRate[20],portNick[50];
    char *speedValue;
    char *flowValue;
    char *portNickValue;
    char *nick;
    int input_len;
    int output_len;
    int i;
    int j = 0;
    int k = 0;
    int new_speed;  //用户提交的速度
    int new_speed_mode;  //用户提交的速率模式
    char message[96];
    VETH_TYPE_T  device;
    getSetType(&device);   //获取设备信息
    for(i = 1; i <= device.max_portNo; i++) {
        j = 0;
        k = 0;
        sprintf(speed,"speedSelect_%d",i);
        sprintf(FlowRate,"openFlowRate_%d",i);
        sprintf(portNick,"setPortNick_%d",i);
        speedValue = getParameter(url,speed);
        flowValue = getParameter(url,FlowRate);
        portNickValue = getParameter(url,portNick);
        if(portNickValue) {
            output_len = 50;
            nick  = gbk_conv_hanzi(portNickValue);
            memcpy(board.ports_config.port_nick[i - 1],nick,strlen(nick));
        }
        while(speedValue[j]) {
            if(speedValue[j] >= '0' && speedValue[j] <= '9') {
                desPorts[k++] = speedValue[j] - '0' ;
            }
            j++;
        }
        if(!flowValue) {
            desPorts[k++] = 0;
        } else {
            desPorts[k++] = 1;
        }
        if(board.ports_config.speed_select_enable[i - 1] != desPorts[0]) { //判断用户是否进行了修改
            board.ports_config.speed_select_enable[i - 1] = desPorts[0];  //修改速率选择是否可用

            if( !board.ports_config.speed_select_enable[i - 1]) {
                if(is_modify_port[i - 1] == 0) {
                    is_modify_port[i - 1] = 1;   //置端口被修改标志
                    if(is_open_delay_config_ports == 0) {
                        is_open_delay_config_ports = 1;   //开启延迟配置模式
                    }
                }
                memset(message,0,sizeof(message));
                sprintf(message,"端口 %d 禁用了速率选择模式",i);
                addLogEntry(INFO,"vorx",message);   //禁用端口速率选择模式
                printSystemLog();
            }else{
				board.ports_config.speed_select_enable[i - 1] = 1;
			}
        }
        if(board.ports_config.port_auto[i - 1] != desPorts[1]) {
            board.ports_config.port_auto[i - 1] = desPorts[1];  //修改速率模式是否为auto
            if(board.ports_config.port_auto[i - 1]) {
                //Set_port_status(i - 1 ,AUTO);
                if(is_modify_port[i - 1] == 0) {
                    is_modify_port[i - 1] = 1;   //置端口被修改标志
                    if(is_open_delay_config_ports == 0) {
                        is_open_delay_config_ports = 1;   //开启延迟配置模式
                    }
                }
                memset(message,0,sizeof(message));
                //printf(message,"修改端口 %d 速率为自动模式",i);
                addLogEntry(INFO,"vorx",message);   //禁用端口速率选择模式
                printSystemLog();
            }else{
				board.ports_config.port_auto[i - 1] = 0;
			}
        }
        /* 读取用户提交的速率数值 */
        if(desPorts[2] == 1) {
            new_speed = 10;
        } else if(desPorts[2] == 2) {
            new_speed = 100;
        } else if(desPorts[2] == 3) { //1G全双工
            new_speed = 1000;
        }
        /* 读取用户提交的端口速率模式 */
        if(desPorts[3] == 1) {
            new_speed_mode = 1;  //全双工模式
        } else {
            new_speed_mode = 0;  //半双工模式
        }
        /* 两种情况下会去修改端口的速率
        ** 1.速率被修改时修改
        ** 2.速率没被修改但是模式被修改了
        */
        if(board.ports_config.port_speed[i - 1] != new_speed ||
                board.ports_config.port_fdx[i - 1] != new_speed_mode) {
            if(is_modify_port[i - 1] == 0) {
                is_modify_port[i - 1] = 1;   //置端口被修改标志
                if(is_open_delay_config_ports == 0) {
                    is_open_delay_config_ports = 1;   //开启延迟配置模式
                }
            }

            if(desPorts[2] == 1) {
                board.ports_config.port_speed[i - 1] = 10;
                if(desPorts[3] == 1) { //10M全双工
                    board.ports_config.port_fdx[i - 1] = 1;
                    memset(message,0,sizeof(message));
                    sprintf(message,"修改端口 %d 速率为10M全双工",i);
                    addLogEntry(INFO,"vorx",message);   //禁用端口速率选择模式
                    printSystemLog();
                } else { //10M半双工
                    board.ports_config.port_fdx[i - 1] = 0;
                    memset(message,0,sizeof(message));
                    sprintf(message,"修改端口 %d 速率为10M半双工",i);
                    addLogEntry(INFO,"vorx",message);   //禁用端口速率选择模式
                    printSystemLog();
                }
            } else if(desPorts[2] == 2) {
                board.ports_config.port_speed[i - 1] = 100;
                if(desPorts[3] == 1) { //100M全双工
                    board.ports_config.port_fdx[i - 1] = 1;
                    memset(message,0,sizeof(message));
                    sprintf(message,"修改端口 %d 速率为100M全双工",i);
                    addLogEntry(INFO,"vorx",message);   //禁用端口速率选择模式
                    printSystemLog();
                } else { //100M半双工
                    board.ports_config.port_fdx[i - 1] = 0;
                    memset(message,0,sizeof(message));
                    sprintf(message,"修改端口 %d 速率为100M半双工",i);
                    addLogEntry(INFO,"vorx",message);   //禁用端口速率选择模式
                    printSystemLog();
                }

            } else if(desPorts[2] == 3) { //1G全双工
                board.ports_config.port_speed[i - 1] = 1000;
                board.ports_config.port_fdx[i - 1] = 1;
                memset(message,0,sizeof(message));
                sprintf(message,"修改端口 %d 速率为1G全双工",i);
                addLogEntry(INFO,"vorx",message);   //禁用端口速率选择模式
                printSystemLog();
            }

        }
        if(board.ports_config.port_flow[i - 1] != desPorts[5]) {
            board.ports_config.port_flow[i - 1] = desPorts[5];
            if(is_modify_port[i - 1] == 0) {
                is_modify_port[i - 1] = 1;   //置端口被修改标志
                if(is_open_delay_config_ports == 0) {
                    is_open_delay_config_ports = 1;   //开启延迟配置模式
                }
            }
            if(desPorts[5] == 1) { //开启了流控
                memset(message,0,sizeof(message));
                sprintf(message,"端口 %d 开启了流控",i);
                addLogEntry(INFO,"vorx",message);   //禁用端口速率选择模式
                printSystemLog();
            } else {
                memset(message,0,sizeof(message));
                sprintf(message,"端口 %d 关闭了流控",i);
                addLogEntry(INFO,"vorx",message);   //禁用端口速率选择模式
                printSystemLog();
            }
        }
        if(is_open_delay_config_ports == 1) {
            delay_config_ports_time = sCount;   //更新计时器的值
        }
        freeMemory(speedValue);
        freeMemory(flowValue);
        freeMemory(portNickValue);
    }
}
/**
 * Function: modifySwitchChipPortsConfig
 * Description: 修改底层芯片对应的串口配置信息
 * @param url 用户提交的表单数据，包含了用户设置的端口信息
**/
uint8_t modifySwitchChipPortsConfig(){
	int i;
	for(i = 0; i < ports_num;i++){
		if(is_modify_port[i] == 1){  //该端口的配置被修改，修改芯片中对应的配置
			if( !board.ports_config.speed_select_enable[i]) {   //端口被禁用
                Set_port_status(i,DISABLED);
			}else if(board.ports_config.port_auto[i]){  //端口模式被设置成自动模式
				Set_port_status(i,AUTO);
			}else{  //手动选择速率
				if(board.ports_config.port_speed[i] == 10){   //10M速率
					if(board.ports_config.port_fdx[i] == 0){    //10M半双工
						Set_port_status(i,SPEED10_HALFDPX);
					}else{   //10M全双工
						Set_port_status(i,SPEED10_FULLDPX);
					}
				}else if(board.ports_config.port_speed[i] == 100){     //100M速率
					if(board.ports_config.port_fdx[i] == 0){     //100M半双工
						Set_port_status(i,SPEED100_HALFDPX);
					}else{      //100M全双工
						Set_port_status(i,SPEED100_FULLDPX);
					}
					
				}else if(board.ports_config.port_speed[i] == 1000){   //1G全双工
					Set_port_status(i,SPEED1000_FULLDPX);
				}
			}// 结束对速率修改的配置
			
			//修改对流控的配置
			Set_flow_control(i,board.ports_config.port_flow[i]);
			
			is_modify_port[i] = 0;
		}
	}
	is_open_delay_config_ports = 0;
}
/**
 * Function: getPortDetailData
 * Description: 生成某一个端口的详细数据信息
 * @param port_no 端口号
 * @return 格式化以后的要发送给客户端的数据
**/
char *getPortDetailData(int port_no) {
    static char portBuf[200];
    API_DETAIL_COUNTER_T detail_count;
    memset(portBuf,0,sizeof(portBuf));
    Get_Port_Counters_det(&detail_count,port_no - 1);
    sprintf(portBuf,"%d,%d,1/%lld/%lld/%lld/%lld/%d/%d/%d/%d|2/%d/%d/%d/%d/%d/%d|3/%d/%d|4/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d|5/%d/%d/%d/%d/%d/%d|8/%d/%d",
            port_no,ports_num,detail_count.RxFrames,detail_count.TxFrames,
            detail_count.RxGoodOctets,detail_count.TxGoodOctets,
            detail_count.RxFiltered,detail_count.TxFiltered,
            detail_count.RxErr,detail_count.TxLate,//结束第一组

            detail_count.RxUnicast,detail_count.TxUnicast,
            detail_count.TxMulticasts,detail_count.TxMulticasts,
            detail_count.RxBroadcasts,detail_count.TxBroadcasts, //结束第二组

            detail_count.RxPause,detail_count.TxPause,  //结束第三组

            detail_count.Octets_64,detail_count.Octets_65to127,
            detail_count.Octets_128to255,detail_count.Octets_256to511,
            detail_count.Octets_512to1023,detail_count.Octets_1024toMAX,
            detail_count.RxUndersize,detail_count.RxOversize,
            detail_count.RxFragments,detail_count.RxJabber, //结束第四组

            detail_count.TxDeferred,detail_count.TxSingle,
            detail_count.TxMultiple,detail_count.TxExcessive,
            detail_count.TxCollisions,detail_count.TxFCSErr,//结束第五组

            detail_count.RxDiscard,detail_count.TxFCSErr
           );
    return portBuf;
}
/**
 * Function: generateBandWidth
 * Description: 生成某一个端口的带宽利用率，包括端口的收数据和发数据的利用率
 * @param port_no 端口号
 * @return 格式化以后的要发送给客户端的收发带宽利用率的值
**/
char *generateBandWidth(int port_no) {
    API_ROUGH_COUNTER_T count;
    static char rx_tx_band_width[100];
    int port_speed;
    int i;
    double time;
    volatile double Rx_band_width_effective;  //接收的带宽利用率
    volatile double Tx_band_width_effective;  //发送的带宽利用率
    uint64_t pre_rx_byte;
    uint64_t pre_tx_byte;
    memset(rx_tx_band_width,0,sizeof(rx_tx_band_width));
    time = msCount - ports_pre_band_width[port_no].band_width_curve_refresh_time;  //计算刷新该页面请求经过的时间
    if(time <= 0) {
        time = 1;
    }
    time = (double)time / 1000;
    pre_rx_byte = ports_pre_band_width[port_no].pre_curve_rx_byte;
    pre_tx_byte = ports_pre_band_width[port_no].pre_curve_tx_byte;
    port_speed =  Get_speed_status(port_no);
    Get_Port_Counters_rou(&count,port_no);
    if(Get_link_status(port_no)) {
        Rx_band_width_effective = (double)((count.RxGoodOctets  -  pre_rx_byte)* 8) / (port_speed * 1000000);
        Tx_band_width_effective = (double)((count.TxGoodOctets  - pre_tx_byte)* 8) / (port_speed * 1000000);
        Rx_band_width_effective = (Rx_band_width_effective / time) * 100;
        Tx_band_width_effective = (Tx_band_width_effective / time) * 100;
        //printf("rx_band_width is %lf;tx_bane_width is %lf\n\r",Rx_band_width_effective,
        //													Tx_band_width_effective);
    } else {
        Rx_band_width_effective = 0;
        Tx_band_width_effective = 0;
    }
    sprintf(rx_tx_band_width,"%.2lf|%.2lf|0",Tx_band_width_effective,Rx_band_width_effective);
    ports_pre_band_width[port_no].band_width_curve_refresh_time = msCount;
    ports_pre_band_width[port_no].pre_curve_rx_byte = count.RxGoodOctets;
    ports_pre_band_width[port_no].pre_curve_tx_byte = count.TxGoodOctets;
    return rx_tx_band_width;
}
/**
 * Function: getImgLocation
 * Description: 计算端口图片应该在的位置信息，在generatePreviewData函数中被调用
 * @param port_no 端口号
 * @array 存放位置信息的数组
 * 
**/
int getImgLocation(int port_no,int *array) {
	int port_type;
	port_type = getPortType(port_no - 1);
	/* 计算得到光口图片的位置 */
	if(port_type){
		if(current_light_num == 0 ){
			current_light_num = port_no / 2 + 1;
			//current_light_num = 1;
			is_first_light_port = 1;
		}else{
			current_light_num += 1;
			is_first_light_port = 0;
		}
		if(is_first_light_port == 1){
			array[0] = (current_light_num * 32 + 15);
		}else{
			array[0] = (current_light_num - 1) * (32 + 15);
		}
		array[1] = 46;
		array[4] = 1;
				
	}else{   //计算电口的图片位置
	  if(port_no % 2 == 0 ){ //端口为偶数
			array[0] = ((port_no - 2) / 2) * 32 + 32;
			array[1] = 23;
			array[4] = -1;
		}else{
			array[0] = ((port_no - 1) / 2) * 32 + 32;
			array[1] = 46;
			array[4] = 1;
		}
	}
	array[2] = 32;
	array[3] = 23;
}





