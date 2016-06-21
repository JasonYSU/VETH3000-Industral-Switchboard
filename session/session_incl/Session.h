#ifndef SESSION_H
#define SESSION_H
#include "StringHashTable.h"
#include "board_info.h"
#define SESSION_TIME     30 * 60      //定义session过期时间为30分钟,30分钟之内用户无操作则session将会过期
//extern uint32_t session_time_out;   //session过期定时器
/* 定义session节点结构体 */
struct session_node{
	char *key;   //用户存储的key
	char *session_key; //存放session实际使用的key
	uint32_t session_time;   //session计时器
	struct session_node *next;
};
/* 设置session key value */
unsigned char putSession(char *key,char *value);
/* 获取session的value */
char* getSession(char *key);
/* 删除Session */
unsigned char deleteSession(char *key);

/* 添加新的session到session链中 */
struct session_node *addSessionNode(struct session_node **session_root,struct session_node *node);

/*删除session节点*/
struct session_node *deleteSessionNode(struct session_node **session_root,struct session_node *node);

/* 判断session节点是否过期 */
uint8_t isSessionTimeout(struct session_node *session_root,struct session_node *node);

/* 刷新session的计时器的值 */
uint8_t updateSessionTime(struct session_node *session_root,struct session_node *node);
/* 计算session使用的key session_key,函数内部使用动态分配空间的方式实现，调用该方法后需要手动释放返回值空间 */
char *getSessionKey(char* key,char *browser_info,uint32_t remote_ip);

extern struct session_node *session_root;   //session链表的头结点
#endif