#ifndef REQUEST_H
#define REQUEST_H
#include<stdio.h>



#define ISO_and 0x26
#define ISO_equal 0x3d
#define ISO_null 0x30

char *getParameter(char *url,char *name);
void freeMemory(char *ptr);
/* 获取HTTP协议头中的名字为name的参数的值 */
char *getHttpHeaderParameter(char *http_header,char *name);
/* 获取表单开始的位置,从开始出获取表单参数的值 */
char *getHttpFormStartLocation(char *formUrl);
#endif


