/*******************************************************
File name: Session.c
Description: web服务器的session功能的实现，session的使用可以参考字符串hash的使用
Author: dujiancheng
Version: v1.0
********************************************************/
#include "Session.h"
#include "Utility.h"
#include "httpd.h"
#include "tcp.h"
struct session_node *session_root = NULL;   //session链表的头结点
/**
 * Function: putSession
 * Description: 设置session 的key和value，用户传递的key一般比较简单，需要进行加工厂session需要的key
 * 				session的key的组成是用户传递的 key+登陆的用户名+浏览器信息+远程主机的IP地址
 * @param key  用户想要存放的key该值一般比较简单
 * @param value 用户存放的key对应的值
 * @return 1 函数正常结束
**/
unsigned char putSession(char *key,char *value){
	char *session_key;
	struct session_node *node;
	struct tcp_pcb *pcb;
	pcb = current_http_connection->pcb;
	session_key = getSessionKey(key,
								current_http_connection->browser_info,
								current_http_connection->pcb->remote_ip.addr
								);
	putStringHashValue(session_key,value);
	/* 设置session被记录的时间 */
	node = (struct session_node *)malloc(sizeof(struct session_node));
	node->key = (char*)malloc(strlen(key) + 1);
	node->session_key = (char*)malloc(strlen(session_key) + 1);
	strcpy(node->key,key);
	strcpy(node->session_key,session_key);
	node->session_time = sCount;
	addSessionNode(&session_root,node);
	free(session_key);
	session_key = NULL;
	return 1;
}

/**
 * Function: getSession
 * Description: 根据传递的key的值得到对应存储的value的值
 * @param key  用户存放的key的值
 * @return key 对应存放的value
**/
char* getSession(char *key){
	char *session_key;
	struct tcp_pcb *pcb;
    char *result = NULL;
	pcb = current_http_connection->pcb;
	session_key = getSessionKey(key,
								current_http_connection->browser_info,
								current_http_connection->pcb->remote_ip.addr
								);
	result = getStringHashValue(session_key);
	free(session_key);
	session_key = NULL;
	return result;
}
/**
 * Function: deleteSession
 * Description: 删除session时是根据session key进行删除而不是根据用户key进行删除的，通过调用deleteStringHashValue函数实现
 * @param session_key  要删除的session的key，该值是session的key是对用户的key进行加工后的key
 * @return 删除的结果
**/
unsigned char deleteSession(char *session_key){
	unsigned char result = 0;
	result = deleteStringHashValue(session_key);
	return result;
}
/**
 * Function: addSessionNode
 * Description: 添加新的session到session链中,采用尾部插入法 
 * @param session_root  指向session头结点指针的指针
 * @param node 要添加的session结点
 * @return session的头结点指针
**/
struct session_node *addSessionNode(struct session_node **session_root,struct session_node *node){
	struct session_node *temp = NULL;
	temp = *session_root;
	if( !temp){  //session链为空
		*session_root = node;
		(*session_root)->next = NULL;
	}else{
		while(temp->next){  //找到最后一个节点
			temp = temp->next;
		}
		temp->next = node;
		node->next = NULL;
	}
	return (*session_root);
}
/**
 * Function: deleteSessionNode
 * Description: 删除session节点
 * @param session_root  指向session头结点指针的指针
 * @param node 要删除的session结点
 * @return session的头结点指针
**/
struct session_node *deleteSessionNode(struct session_node **session_root,struct session_node *node){
	struct session_node **pre_node;
	struct session_node *current_node;
	pre_node = session_root;
	current_node = *session_root;
	while(current_node){
		if( !strcmp(current_node->key,node->key) && !strcmp(current_node->session_key,node->session_key)){
			break;
		}
		pre_node = &current_node->next;
		current_node = current_node->next;
	}
	if(!current_node){  //删除的节点不存在
		return (*session_root);
	}
	*pre_node = current_node->next;
	free(current_node->key);
	current_node->key = NULL;
	free(current_node->session_key);
	current_node->session_key = NULL;
	free(current_node);
	current_node = NULL;
	return *session_root;
}


/**
 * Function: isSessionTimeout
 * Description: 判断session节点是否过期
 * @param session_root  指向session头结点的指针
 * @param node 要进行判断的的session结点
 * @return 1 该节点session过期
 *         0 该节点session没有过期
**/
uint8_t isSessionTimeout(struct session_node *session_root,struct session_node *node){
	struct session_node *temp;
	temp = session_root;
	while(temp){
		if( !strcmp(temp->key,node->key) && !strcmp(temp->session_key,node->session_key)){
			break;
		}
		temp = temp->next;
	}
	if( !temp){
		return 0;
	}
	if( sCount - temp->session_time >= SESSION_TIME ){   //session 已经过期
		return 1;
	}
	return 0;
}
/**
 * Function: updateSessionTime
 * Description: 更新session的计时器的值
 * @param session_root  指向session头结点的指针
 * @param node 被更新的session结点
 * @return 0 出错
**/
uint8_t updateSessionTime(struct session_node *session_root,struct session_node *node){
	struct session_node *temp;
	temp = session_root;
	while(temp){
		if( !strcmp(temp->key,node->key) && !strcmp(temp->session_key,node->session_key)){
			break;
		}
		temp = temp->next;
	}
	if( !temp){
		return 0;
	}
	temp->session_time = sCount;
}
/**
 * Function: getSessionKey
 * Description: 根据用户传递的key的值计算得到对应的session的key
 * @param key  用户传递的key的值
 * @param browser_info 浏览器的信息
 * @param remote_ip 连接对应的远程主机IP地址
 * @return 计算得到的session的key的值
**/
char *getSessionKey(char* key,char *browser_info,uint32_t remote_ip){
	char *session_key;
	session_key = (char*)malloc(strlen(key) + 
					strlen(browser_info) +
					integerLength(remote_ip) + 1);
	sprintf(session_key,"%s%s%d",key,browser_info,remote_ip);
	return session_key;
}