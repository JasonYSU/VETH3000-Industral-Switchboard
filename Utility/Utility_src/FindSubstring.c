/*******************************************************
File name: FindSubstring.c
Description: 使用KMP算法实现字符串子串的查找
Author: dujiancheng
Version: v1.0
********************************************************/
#include"FindSubstring.h"
/**
 * Function: get_nextval
 * Description: 得到KMP算法中的next数值
 * @param pattern 要查找的字符串子串
 * @param next 返回计算后得到的next的数值的值
**/
int get_nextval(char *pattern, int next[])
{
    //get the next value of the pattern 
    int i = 0, j = -1;
    int patlen = strlen(pattern);
	next[0] = -1;
    while ( i < patlen - 1){
        if ( j == -1 || pattern[i] == pattern[j]){
            ++i;
            ++j;
            if (pattern[i] != pattern[j])
                next[i] = j;
            else
                next[i] = next[j];
        }
        else
            j = next[j];
        }

    return(0);
}
/**
 * Function: findSubstring
 * Description: 在源字符串中查找子串
 * @param target 源字符串
 * @param pattern 要查找的子串
 * @param target_len 源字符串的长度
 * @param pos 查找开始的起始位置
 * @return 如果子串存在，返回子串在源字符串首次出现的下标位置
 *		   -1 子字符串不存在
**/
int findSubstring(char *target, char *pattern,int target_len, int pos)
{
    int tari = pos, pati = 0; 
    int tarlen = target_len;
	int patlen = strlen(pattern);
    int *next = NULL;
	next = (int *)malloc(patlen * sizeof(int));
    get_nextval(pattern, next);
    while ( tari < tarlen && pati < patlen ){
        if (pati == -1 ||target[tari] == pattern[pati]){
            ++tari;
            ++pati;
        }else{
            pati = next[pati];
        }
    }    
	if(next != NULL) 
		free(next);
	next = NULL;
	if (pati == patlen)
		return tari - pati;
	else
		return -1;
}