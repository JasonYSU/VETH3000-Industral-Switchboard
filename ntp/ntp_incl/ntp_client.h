#ifndef NTP_CLIENT_H
#define NTP_CLIENT_H
#include "board_info.h"
#include "ip_addr.h"
#include "udp.h"
#define int8 char
#define uint8 unsigned char
#define uint32 unsigned int
#define ulong32 unsigned long
#define long32 long
#define int32 int
#define long64 long long

#define debug

#define NTP_PORT               123               /*NTP专 用端口号字符串*/
#define TIME_PORT              37               /* TIME/UDP端 口号 */
#define NTP_SERVER_IP       "202.120.2.101" /*上海授时中心 IP*/
#define NTP_PORT_STR        "123"          /*NTP专用端口号字 符串*/
#define NTPV1                "NTP/V1"      /*协议及其版本号*/
#define NTPV2                "NTP/V2"
#define NTPV3                "NTP/V3"
#define NTPV4                "NTP/V4"
#define TIME                "TIME/UDP"
  
#define NTP_PCK_LEN 48
#define LI 0x00
#define VN 0x04
#define MODE 0x03
#define STRATUM 0x01
#define POLL 0x0a
#define PREC 0xfa
  
#define JAN_1970 0x83aa7e80 /* 1900年～1970年之间的时间秒数 */
#define NTPFRAC(x)     (4294 * (x) + ((1981 * (x)) >> 11))
#define USEC(x)         (((x) >> 12) - 759 * ((((x) >> 10) + 32768) >> 16))
 
#pragma pack(1)
struct ntp_packet
{
		uint8_t _flags;//Flags
        uint8_t _pcs;//Peer Clock Stratum
        uint8_t _ppt;//Peer Polling Interval
        uint8_t _pcp;//Peer Clock Precision
        uint32_t _rdy;//Root Delay
        uint32_t _rdn;//Root Dispersion
        uint32_t _rid;//Reference ID
        uint64_t _ret;//Reference Timestamp
        uint64_t _ort;//Origin Timestamp
        uint64_t _rct;//Receive Timestamp
        uint64_t _trt;//Transmit Timestamp
 
};
#pragma pack()
struct timeval{
	long tv_sec;
	long tv_usec;
};
void NTP_Init(void);
int send_packet(int usd);
void ntp_client_appcall(void);
int connectNTPServer();  //连接远程的NTP服务器
int generateSendData();
//int get_ntp_time();
void get_ntp_time(void *arg, struct udp_pcb *pcb, struct pbuf *p,
    ip_addr_t *addr, u16_t port);
#endif