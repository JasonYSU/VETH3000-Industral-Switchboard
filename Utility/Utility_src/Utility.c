/*******************************************************
File name: Utility.c
Description: 提供一些常用的工具集
Author: dujiancheng
Version: v1.0
********************************************************/
#include "Utility.h"
/**
 * Function: integerLength
 * Description:计算整数的长度，类似于strlen的功能
 * @param num  被计算的整数
 * @return 整数对应的长度
**/
int integerLength(int num){
	int temp = num;
	int length = 0;
	while(temp){
		temp = temp / 10;
		length++;
	}
	return length;
}