#ifndef STRING_UTILITY_H
#define STRING_UTILITY_H
#include<stdio.h>
/* 拼接字符串,把src的前num个字符拼接到dest的start_loc的后面,不考虑src中的结束符 */
char *connectNumString(char* dest,char*src,int start_loc,int num);
int compareNumString(const void * ptr1, const void * ptr2, size_t num );
#endif