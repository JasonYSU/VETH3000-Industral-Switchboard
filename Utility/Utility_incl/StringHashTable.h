#ifndef STRING_HASH_TABLE_H
#define STRING_HASH_TABLE_H
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#define HASH_MAX_NUM 16   
/* 定义hash表中链表的结构体 */
struct string_hash_node{
	char *key;      //hash tabke 节点对应的key
	char *value;    //Hash table 对应存储的值
	struct string_hash_node *next;
};
extern struct string_hash_node *string_hash[HASH_MAX_NUM];
/* 对hash table进行初始化 */
unsigned char initStringHashTable();
/* 根据node中的内容计算出对应的index */
unsigned char calcKeyIndex(struct string_hash_node *node);
/* 基于BKD算法实现获取字符串对应的hash数值 */
unsigned int getStringBKDRHash(char *str);

/* 添加key - value到hash table中 */
unsigned char putStringHashValue(char *key,char *value);
/* 根据key值得到存储在hash table中的值 */
char* getStringHashValue(char *key);
/* 根据hash table的key删除hash表中对应的节点 */
unsigned char deleteStringHashValue(char *key);
/* 添加node数据到hash table中 */
unsigned char addDataToStringHashTable(int index,struct string_hash_node *node);

#endif