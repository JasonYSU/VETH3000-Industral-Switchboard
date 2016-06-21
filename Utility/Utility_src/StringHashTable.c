/*******************************************************
File name: StringHashTable.c
Description: 实现了字符串的hash表，对字符串hash值的计算使用BKDR算法。文件中提供了一些对外的接口函数
Author: dujiancheng
Version: v1.0
********************************************************/
#include "StringHashTable.h"
struct string_hash_node *string_hash[HASH_MAX_NUM];
/*  */
/**
 * Function: initStringHashTable
 * Description:初始化字符串的hash table
**/
unsigned char initStringHashTable(){
	int i;
	for(i = 0; i < HASH_MAX_NUM; i++){
		string_hash[i] = NULL;
	}
	return 1;
}
/*  */
/**
 * Function: getStringBKDRHash
 * Description:使用BKDR算法获取字符串对应的hash值
 * @param str 需要计算的字符串的值
 * @return 计算后得到的字符串对应的hash值
**/
unsigned int getStringBKDRHash(char *str)
{
    unsigned int seed = 131; // 31 131 1313 13131 131313 etc..
    unsigned int hash = 0;

    while (*str){
		hash = hash * seed + (*str++);
    }

    return (hash & 0x7FFFFFFF);   //返回值用 hash和 0x7FFFFFFF进行与运算是为了保证返回的结果为正值
}
/* */
/**
 * Function: putStringHashValue
 * Description: 添加key - vakue键值对到字符串hash表中，可以参考下面代码使用该函数
 *				char *user_name = "xiaodu";
				putStringHashValue(user_name,"dujiancheng");
				char *value = getStringHashValue("xiaodu");
				得到的value的值是dujiancheng
 * @param key 字符串索引的关键字
 * @param value 索引对应的值
 * @return 1 函数正常结束
 *		   0 函数执行出现异常
**/
unsigned char putStringHashValue(char *key,char *value){
	unsigned int index_hash = 0;
	struct string_hash_node *node;
	node = (struct string_hash_node *)malloc(sizeof(struct string_hash_node));
	if(!node){
		printf("allocate hash node memory failed\n");
		return 0;
	}
	node->key = (char*)malloc(strlen(key) + 1);
	if(!node->key){
		printf("allocate hash node key memory failed\n");
		return 0;
	}
	node->value = (char*)malloc(strlen(value) + 1);
	if(!node->value){
		printf("allocate hash node key memory failed\n");
		return 0;
	}
	strcpy(node->key,key);
	strcpy(node->value,value);
	index_hash = getStringBKDRHash(key);
	index_hash = index_hash % HASH_MAX_NUM;
	addDataToStringHashTable(index_hash,node);
	//printf("add hash sucessful hash index is %d\n\r",index_hash);
	return 1;	
}
/**
 * Function: addDataToStringHashTable
 * Description:添加node数据到hash table中，该函数将被putStringHashValue函数调用
 * @param index 要添加节点的hash值
 * @param node 被添加的hash节点
 * @return 1 函数正常结束
 *		 
**/
unsigned char addDataToStringHashTable(int index,struct string_hash_node *node){
	struct string_hash_node *temp;
	if(string_hash[index] == NULL){ //如果该位置是否为空,直接添加,作为头结点
		string_hash[index] = node;
		string_hash[index]->next = NULL;
	}else{  //解决冲突,采用尾部插入法解决冲突
		temp = string_hash[index];
		while(temp->next){
			temp = temp->next;
		}
		temp->next = node;
		node->next = NULL;
	}
	return 1;
}
/**
 * Function: getStringHashValue
 * Description:根据key值得到存储在hash table中的对应的值
 * @param key 索引的关键字
 * @return 存放索引对应的值
 *		 
**/
char* getStringHashValue(char *key){
	unsigned int index_hash = 0;
	struct string_hash_node *temp = NULL;
	index_hash = getStringBKDRHash(key);
	index_hash = index_hash % HASH_MAX_NUM;
	temp = string_hash[index_hash];
	while(temp){
		if( !strcmp(key,temp->key) ){
			return temp->value;
		}
		temp = temp->next;
	}
	/* 执行到这的时候,表示没有找到可用的连接，返回NULL*/
	return NULL;  
}
/**
 * Function: deleteStringHashValue
 * Description: 根据hash table的key删除hash表中对应的节点
 * 			    删除时使用二阶指针进行删除,其中pre_node始终指向next指针的地址，
 *				这样可以不必考虑删除头结点的特殊情况。该函数的实现提供了一个参考，对于链表的删除操作可以不进行
 *				特殊的判断，提高了效率。
 * @param key 索引的关键字
 * @return 1 删除成功
 *         0 删除的节点不存在
 *		 
**/
unsigned char deleteStringHashValue(char *key){
	unsigned int index_hash = 0;
	struct string_hash_node *temp = NULL;
	struct string_hash_node **pre_node = NULL;
	index_hash = getStringBKDRHash(key);;
	index_hash = index_hash % HASH_MAX_NUM;
	temp = string_hash[index_hash];
	pre_node = &string_hash[index_hash];
	while(temp){
		if( !strcmp(key,temp->key) ){
			//printf("find delete node\n");
			break;
		}
		pre_node = &temp->next;
		temp = temp->next;
	}
	if(temp == NULL){ //删除的节点不存在
		//printf("delete node is not exist\n");
		return 0;
	}
	*pre_node = temp->next;
	free(temp->key);
	free(temp->value);
	free(temp);
	temp = NULL;
	//printf("hash node has been deleted\n");
	return 1;
}