/*******************************************************
File name: upload_file.c
Description: 提供和文件上传相关操作的接口工具
Author: dujiancheng
Version: v1.0
********************************************************/
#include "upload_file.h"
#include "system_maintain.h"
#include "FindSubstring.h"
#include "httpd.h"
#include<string.h>
#include<stdlib.h>
#define QUOTATION_MARK 0x22  //双引号
#define SEPARATOR 0x2d    //文件表单的分隔符
int http_header_length; //记录http header的长度
/**
 * Function: getUploadFileName
 * Description:获取上传文件的名称
 * @param url  上传文件的表单内容
 * @param boundary 浏览器提交的文件表单中分隔符
 * @return 获取得到的文件名
**/
char *getUploadFileName(char *url,char *boundary){
	//printf();
	char *start_loc;
	char *end_loc;
	char *ret_value;
	start_loc = strstr(url,"filename");
	start_loc = strchr(start_loc,QUOTATION_MARK);
	++start_loc;
	end_loc = strchr(start_loc,QUOTATION_MARK);
	ret_value = (char*)malloc((end_loc - start_loc + 1) * sizeof(char));
	memset(ret_value,0,(end_loc - start_loc + 1) * sizeof(char));
	strncpy(ret_value,start_loc,(end_loc - start_loc));
	ret_value[end_loc - start_loc + 1] = 0;
	//printf("filename return value is %s\n\r",ret_value);
	return ret_value;
}
/**
 * Function: getUploadFileContent
 * Description:获取上传文件的文件内容
 * @param url  上传文件的表单内容
 * @param boundary 浏览器提交的文件表单中分隔符
 * @param file_size 上传的文件的大小
 * @return 得到的文件内容
 * @ATTENTION 函数的返回值是动态分配的，调用该方法后需要手动释放返回值的内存空间
**/
char *getUploadFileContent(char *url,char *boundary,int *file_size){
	char *file_content = NULL;
	char *end_content = NULL;
	int end_loc = 0;
	int i,j;
	char *p;
	char *start_content = NULL;

	start_content = getHttpFormStartLocation(url);
	end_loc = findSubstring(start_content,boundary,url_buf_length - http_header_length,0);
	if(end_loc < 0){
		printf("can't find substring\n\r");
	}
	end_content = &start_content[end_loc];
	p = end_content;
	//printf("find end loc is %s\n\r",end_content);
	end_content = end_content - 2; //去掉协议添加的\n\r结束标志
	file_content = (char*)malloc((end_content - start_content + 1) * sizeof(char));
	if(!file_content){
		printf("file_content malloc memory failed\n\r");
	}
	memcpy(file_content,start_content,(end_content - start_content));
	file_content[end_content - start_content + 1] = 0;
	*file_size = end_content - start_content;
	return file_content;
}