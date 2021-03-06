/**
****************************************************************************************
@File    : D:\ProgramFiles\KEIL\hal\incl\veth_mac.h
@Author  : Duke Lee
@Date    : 18-Oct-2014
@Version : V1.0.00
@Breif   : CGI接口：MAC表项的头文件 MAC琛ㄩ」鐨勫ご鏂囦欢
****************************************************************************************
**/

#ifndef _VETH_MAC_H__
#define _VETH_MAC_H__ 

#include <stdint.h>
#include "veth_common.h"

#define ENTRY_STATIC    0
#define ENTRY_DYNAMIC   1

typedef struct mac_api_t {
	uint8_t  add_or_delet;      /* '0' 涓哄垹闄� ,'1'  涓烘坊鍔� */ //          
	uint8_t  new_mac[6];        /* 瀛樻斁鏂版坊鍔犲垹闄ゆ垨鑰呰鍙栧埌鐨別ntry鐨凪AC鍦板潃*/ 
	uint16_t port_vector;       /* 瀛樻斁娣诲姞鎴栬�呰鍙栧埌鐨勭鍙ｅ悜閲�*/ 
	uint8_t  sta_or_dym;        /* 瀛樻斁闈欐�佸姩鎬佹爣蹇�*/  //ENTRY_STATIC for static entrys ENTRY_DYNAMIC for dynamic entrys   //只在状态栏里面用到
	uint8_t  vlan_id;           /* 瀛樻斁vlanid*/
}MAC_API_T;


/***********************************************************************/
//鍑芥暟鍚�   :  Set_Aging_time(uint16_t age_time)
//鍔熻兘     :  璁剧疆鑰佸寲鏃堕棿
//鍙傛暟     :  Age_time(Input)__鑰佸寲鏃堕棿鍙栧�艰寖鍥村簲璇ヤ负15鐨勫�嶆暟锛�0~3825锛夛紝鍚﹀垯璁剧疆鐨勮�佸寲鏃堕棿鍙兘璺熸墍杈撳叆鐨勬椂闂存湁鍑哄叆
//                          PS锛氬皢鑰佸寲鏃堕棿璁剧疆涓�0琛ㄧず绂佹鑰佸寲
//杩斿洖鍊�   锛� None
/***********************************************************************/
void Set_Age_time(uint16_t age_time);

/***********************************************************************/
//鍑芥暟鍚�     :  Get_Aging_time()
//鍔熻兘       :  鑾峰彇MAC琛ㄦ牸鐨勫綋鍓嶈缃椂闂�
//鍙傛暟       锛� None
//杩斿洖鍊�     锛� uint16_t 鑾峰彇鐨凪ACentry鑰佸寲鏃堕棿
/***********************************************************************/
uint16_t Get_Age_time(void);

/***********************************************************************/
//鍑芥暟鍚�  锛�  Add_or_Delet_MACentry(MAC_API_T *entry)
//鍔熻兘    锛�  娣诲姞鎴栬�呭垹闄や竴鏉￠潤鎬丮AC鍦板潃椤�
//鍙傛暟    锛�  entry锛屾寚鍚戜竴涓狹AC_API_T ,鐢ㄦ潵浼犻�掗渶瑕佹坊鍔犳垨鑰呭垹闄ょ殑MAC entry 
//杩斿洖鍊�  锛�  uint8_t '1' 娣诲姞鎴栬�呭垹闄ゆ垚鍔�,'0' 娣诲姞/鍒犻櫎澶辫触 
/***********************************************************************/
uint8_t Add_or_Delet_MACentry(MAC_API_T *entry);

/***********************************************************************/
//鍑芥暟鍚�    锛� Display_All_MACentry(MAC_API_T *entry,MAC_ENTRY_T *mac_entry)
//鍔熻兘      锛� 鏌ユ壘鎵�MAC琛ㄤ腑鐨凪AC entry 浣跨敤鍓嶉渶濉叆MAC_ENTRY_T 涓殑鍒濆MAC鍊�        
//鍙傛暟      锛� MAC_API_T *entry鐢ㄦ潵瀛樻斁璇诲彇鍒扮殑MAC_entry淇℃伅,鎵�闇�淇℃伅瑙丮AC_ENTRY_T 缁撴瀯浣撳畾涔�
//             MAC_ENTRY_T *mac_entry鐢ㄦ潵瀛樻斁姣忔璁块棶MAC琛ㄧ殑鍒濆MAC鍦板潃锛屼篃灏辨槸锛�
//	  for(loop = 0 ;loop < 6; loop++ )
//	   {
//	      mac_entry->mac_entry_MAC[loop] = 0xff;  //MAC{0xff,0xff,0xff,0xff,0xff,0xff} used to get the first ATU entry
//		 }	
/***********************************************************************/	 
uint8_t Display_ALL_MACentry(MAC_API_T *entry,MAC_ENTRY_T *mac_entry,uint16_t vlan_id);

/***********************************************************************/
//鍑芥暟鍚�    : Flush_MAC_entry()
//鍔熻兘      锛氭竻闄ゆ帀MAC琛ㄤ腑鐨勬墍鏈夐潪闈欐�佺殑MAC entrys
//鍙傛暟      锛歂one
//杩斿洖鍊�    锛歂one
/***********************************************************************/
void Flush_MAC_entry(void);

/***********************************************************************/
//鍑芥暟鍚�        : Set_MAC_forbid(uint16_t port_vector)
//鍔熻兘          锛氳缃鍙AC瀛︿範绂佹妯″紡
//鍙傛暟          锛歱ort_vector : 绔彛鍚戦噺(0~9)
//杩斿洖鍊�        锛歂one
/***********************************************************************/	
void Set_MAC_forbid(uint16_t port_vector);

/***********************************************************************/
//鍑芥暟鍚�        : Get_MAC_forbid()
//鍔熻兘          锛氳幏鍙栫鍙AC瀛︿範绂佹妯″紡鐨勭鍙ｅ悜閲�
//鍙傛暟          锛歂one
//杩斿洖鍊�        锛氱姝㈡ā寮忕殑绔彛鍚戦噺
/***********************************************************************/	
uint16_t Get_MAC_forbid(void);

/***********************************************************************/	
//鍑芥暟鍚�      	:	Dispaly_sta_MAC_entry(MAC_API_T *entry,MAC_ENTRY_T *mac_entry)
//鍔熻兘          : 鑾峰彇闈欐�� ATU(:MAC entry table)椤�
//鍙傛暟          : MAC_API_T  : 瀛樻斁鑾峰緱鐨凪AC entry鐩稿叧淇℃伅
//                MAC_ENTRY_T: 瀛樻斁褰撳墠瀛樻斁鐨凪AC鍊硷紝鍒濇璋冪敤闇�瀵� MAC_ENTRY_T.mac_entry_MAC濉叆鍒濆�納0xff,0xff,0xff,0xff,0xff,0xff}
//                vlan_id    : MAC entry 瀵瑰簲鐨剉lan id
//杩斿洖鍊�        : '0' 琛ㄧず鍒拌揪浜咥TU(:MAC entry table)缁撳熬 ,鈥�1鈥欒〃绀鸿繕鍙兘鏈夋湭璇诲彇鍒扮殑闈欐�丮AC entry 
/***********************************************************************/
uint8_t Display_sta_MAC_entry(MAC_API_T *entry,MAC_ENTRY_T *mac_entry,uint16_t vlan_id);

/***********************************************************************/
//鍑芥暟鍚�   : MAC_demo()
//鍔熻兘     锛氭祴璇曟墍鏈夌紪鍐欑殑MAC琛ㄩ」鎺ュ彛鍑芥暟
//鍙傛暟     锛歂one
//杩斿洖鍊�   锛歂one
/***********************************************************************/
void MAC_demo(void);

#endif

