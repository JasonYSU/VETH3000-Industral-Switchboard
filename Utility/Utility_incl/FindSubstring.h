#ifndef FIND_SUBSTRING_H
#define FIND_SUBSTRING_H
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
int get_nextval(char *pattern, int next[]);
/* ʹ��KMP�㷨��ʵ���ַ����Ӵ��Ĳ����㷨,target_len��Դ�ַ����ĳ��ȣ�����ʹ��strlen������
**Դ�ַ����ĳ��ȣ�����һЩ�����Ƶĵ�������˵�м���ܾͻ�����ַ����Ľ�����
**pos�ǿ�ʼ���ҵ�����ʼλ�� */
int findSubstring(char *target, char *pattern,int target_len,int pos);
#endif