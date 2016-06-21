/*******************************************************
File name: LogInfo.c
Description: 日志系统数据结构的实现，文件不提供对外的接口，文件实现的所有方法只供日志系统内部调用
Author: dujiancheng
Version: v1.0
********************************************************/
#include "LogInfo.h"
uint32_t log_num = 0;    //当前日志的总数
uint32_t log_use_total_memory = 0;    //日志系统使用的总的内存
struct system_log *log_root = NULL;      //日志系统的根节点


/**
 * Function: addLogNode
 * Description: 添加日志节点到日志系统中，采用尾部插入法，生成的日志一次往后插入到日志系统中
 *				插入之前应该先判断日志系统是否还有可用的空间
 * @param root  指向日志头部节点指针的指针
 * @param node  新添加的节点
 * @return 日志系统的首节点
**/
struct system_log* addLogNode(struct system_log **root,struct system_log *node){
	struct system_log* temp;
	if( !(*root)){  //链表为空
 		*root = node;
		(*root)->next = NULL;
	}else{
		temp = (*root);
		while(temp->next){  //查找最后一个节点位置
			temp = temp->next;
		}
		temp->next = node;
		node->next = NULL;
	}
	return *root;
}
/*  */
/**
 * Function: deleteLogNode
 * Description: 删除日志的节点，删除头部节点，头部的日志是最老的日志
 * @param root  指向日志头部节点指针的指针
 * @return 日志系统的首节点
**/
struct system_log* deleteLogNode(struct system_log **root){
	struct system_log* temp;
	temp = (*root);
	*root = (*root)->next;
	free(temp->log_content);
	temp->log_content = NULL;
	free(temp);
	temp = NULL;
	return (*root);
}

/*  */
/**
 * Function: emptyLogSystem
 * Description: 清空当前的日志系统所有日志信息
 * @param root  指向日志头部节点指针的指针
 * @return 1 函数正常执行
**/
uint8_t emptyLogSystem(struct system_log **root){
	struct system_log* temp;
	while((*root)){
		temp = (*root);
		(*root) = (*root)->next;
		free(temp->log_content);
		temp->log_content = NULL;
		free(temp);
		temp = NULL;
	}
	log_num = 0;
	return 1; 
}

/**
 * Function: emptyLogSystem
 * Description: 管理系统日志使用的内存，释放老的日志占用的空间，根据第二个参数来删除root中的数据
 * @param root  指向日志头部节点指针的指针
 * @param need_memory 需要释放的内存数
 * @return 0 函数正常执行
**/
uint32_t freeLogSystemMemory(struct system_log **root,uint32_t need_memory){
	uint32_t free_memory_size = 0;  //释放的内存大小
	struct system_log* temp;
	while(free_memory_size < need_memory){  //释放的空间没有达到所需要的空间的大小，继续释放
		temp = *root;
		free_memory_size += sizeof(*temp) + strlen(temp->log_content) + 1;
		//printf("free memory size is %d\n\r",free_memory_size);
		deleteLogNode(root);
		temp = NULL;
	}
	log_use_total_memory -= free_memory_size;
	//printf("after free use memory total is %d\n\r",log_use_total_memory);
	return 0;
}