/*******************************************************
File name: request.c
Description: 提供一些用于处理用户请求的接口函数
Author: dujiancheng
Version: v1.0
********************************************************/
#include "request.h"
#include<stdlib.h>
#include<string.h>
#include <ctype.h>
/**
 * Function: getParameter
 * Description: 用户提交的数据中获取参数对应的值，该方法可以这样使用
 *              URL = "name=xiaodu&sex=man";
 *				char user_name = getParameter(URL,"name");
 *
 *              user_name的值为xiaodu.
 *              free(user_name);
 *				user_name = NULL;
 * @param url  浏览器提交的数据，一般为post数据
 * @param name 要获取的值对应的参数名字
 * @return 参数对应的值
 * @ATTENTION 使用该方法之后需要释放函数返回值对应的内存空间，使用方法可以参考注释给出的部分代码
**/
char *getParameter(char *url,char *name){
	char *subUrl;
	char *paraValue;
	//char paraValue[1000];
	char *equalMark;
	char *andMark;
	char *retValue;
	//static char retValue[1500];
	char ch;
	int i = 0;
	//printf("URL is %s\n",url);
    //memset(retValue,0,sizeof(retValue));
	subUrl = strstr(url,name);
	/* 查找完全匹配的子串 */
	while(subUrl){
		ch = subUrl[strlen(name)]; //判断下一个字符是否是数字，避免处理_12会按照_1来处理的情况
		//printf("%c\n",ch);
		if(isalnum(ch)){  //未找到完全匹配的子串，继续查找
			subUrl = strstr(subUrl + strlen(name),name); //从name长度出开始新的查找
			//printf("sub url is %s\n\r",subUrl);
		}else{
			break;
		}
	}
	//printf("sub str is %s\n\r",subUrl);
	if(subUrl){
		andMark = strchr(subUrl,ISO_and);
		if( !andMark){
			andMark = strchr(subUrl,'\0');
		}
		paraValue = (char*)malloc((andMark - subUrl + 1)*sizeof(char));
		if(paraValue == NULL){
			printf("malloc failed!\n\r");
			return NULL;
		}
		strncpy(paraValue,subUrl,(andMark - subUrl + 1));
		andMark = strchr(paraValue,ISO_and);
		if( !andMark ){
			andMark = strchr(paraValue,'\0');
		}else{
			*andMark = 0;
		}
		//printf("********name %s******\n",name);
		equalMark = strchr(paraValue,ISO_equal);
		//printf("******before return is %s******\n",equalMark);
		retValue = (char *)malloc(strlen(equalMark + 1) * sizeof(char));
		if(retValue == NULL){
			printf("malloc failed!\n\r");
			return NULL;
		}
		strcpy(retValue,++equalMark);
	}else{
		return NULL;
	}
	freeMemory(paraValue);
	//printf("value is %s\n\r",retValue);
	return retValue;
}
/**
 * Function: getHttpHeaderParameter
 * Description: 获取浏览器发送的http协议头中的某个字段的值，用法可以参考getParameter()
 * @param http_header  http协议头
 * @param name 要获取的值对应的参数名字
 * @return 参数对应的值
 * @ATTENTION 使用该方法之后需要释放函数返回值对应的内存空间。
**/
char *getHttpHeaderParameter(char *http_header,char *name){
	char *location_begin;
	char *location_end;
	char *return_value;
	location_begin = strstr(http_header,name);
	if(!location_begin){
		return NULL;
	}
	location_begin = strchr(location_begin,' ');
	++location_begin;
	location_end = strchr(location_begin,'\n');
	return_value = (char*)malloc((location_end - location_begin + 1) * sizeof(char));
	//pbuf_alloc(PBUF_RAW,http_request_len + content_len,PBUF_RAM)
	if(!return_value){
		printf("getHttpHeaderParameter malloc failed\n\r");
		return NULL;
	}
	strncpy(return_value,location_begin,(location_end - location_begin));
	return_value[location_end - location_begin] = 0;
	//printf("Referer is %s\n",referer_begin);
//	if(strlen(return_value) < 2){
//		return_value = "no";
//	}
	return return_value;
}
/**
 * Function: getHttpFormStartLocation
 * Description: 获取form表单的起始位置
 * @param formUrl  整个form表单的数据
 * @return form表单的起始位置
**/
char *getHttpFormStartLocation(char *formUrl){
	char *p,*q;
	int found = 0;
	p = formUrl;
	//printf("content is %s\n\r",content);
	while( !found ){
		q = p++;
		if(*p == '\r'){
			if( *q == '\n'){
				found = 1;
			}
		}
	}//end while
	return p + 2;
}
/**
 * Function: freeMemory
 * Description: 释放内存
 * @param ptr  要释放的内存指针
**/
void freeMemory(char *ptr){
	free(ptr);
	ptr = NULL;
}
