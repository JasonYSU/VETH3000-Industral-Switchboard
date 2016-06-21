#ifndef FIND_SUBSTRING_H
#define FIND_SUBSTRING_H
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
int get_nextval(char *pattern, int next[]);
/* 使用KMP算法来实现字符串子串的查找算法,target_len是源字符串的长度，避免使用strlen来计算
**源字符串的长度，对于一些二进制的的数据来说中间可能就会存在字符串的结束符
**pos是开始查找到的起始位置 */
int findSubstring(char *target, char *pattern,int target_len,int pos);
#endif