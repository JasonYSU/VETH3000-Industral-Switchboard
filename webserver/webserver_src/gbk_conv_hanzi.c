/*******************************************************
File name: gbk_conv_hanzi.c
Description: 把gbk编码转换成对应的汉字
Author: dujiancheng
Version: v1.0
********************************************************/
#include"gbk_conv_hanzi.h"
/**
 * Function: gbk_conv_hanzi
 * Description: 把汉字的gbk编码转换成汉字
 * @param gbk_encode  汉字对应的gbk编码
 * @return gbk编码对应的汉字
**/
char *gbk_conv_hanzi(char *gbk_encode){
	char big_byte[5];  //存放gbk大字节编码
	char little_byte[5];//存放gbk小字节编码
	char *temp;
	char *no_encode; //指向没有gbk编码的字符串
	int group;    //存放gbk大字节编码所在的组数 其中每一组有94个汉字
	int goffset; //小字节编码在组中的偏移量 
	int offset;  //整个编码在码表中的偏移量
	int num;     //小字节编码对应的数字编号
	static char  result[50];  //存放最终得到的转码后的汉字，用两个字节存储，支持50个汉字
	no_encode = temp = gbk_encode;
	memset(big_byte,0,sizeof(big_byte));
	memset(little_byte,0,sizeof(little_byte));
	memset(result,0,sizeof(result));
	while((temp = strchr(temp,'%'))){
		++temp;
		no_encode = temp + 2;
		if( big_byte[0] == 0){
			strncpy(big_byte,temp,2);	
		}else if( little_byte[0] == 0){
			strncpy(little_byte,temp,2);
			if(big_byte[1] > '9'){    //判断大字节编码第二个十六进制编码是否大于9
				num = big_byte[1] - '7';  //十六进制大于9的编码减去字符7的编码得到十六进制对应的十进制的数
			}else{
				num = big_byte[1] - '0';  //转换成十进制的数
			}
			group = ((big_byte[0] - 'B') * 16 + num) * 94;  //根据大字节编码计算得到编码所在的组数
			/* 处理大字节结束，开始处理小字节编码 */
			if(little_byte[1] > '9'){   //同上把十六机制的编码转换成对应的十进制编码
				num = little_byte[1] - '7';
			}else{
				num = little_byte[1] - '0';
			}
			/* 根据小字节的编码计算出小字节编码在改组中的偏移量 
			**由于每一组中第一个编码不存在 因此需要减一操作
			**/
			goffset = (little_byte[0] - 'A' ) * 16 + num - 1;  //
			
			offset = group + goffset; //得到整个编码在gbk编码表中的偏移量
			//printf("big byte is %s\nlittle byte is %s\n",big_byte,little_byte);
			if(offset < 3755){
				strcat(result,*(gbk_hanzi + offset));
			}
			memset(big_byte,0,sizeof(big_byte));
			memset(little_byte,0,sizeof(little_byte));
		}
		
	}
	strcat(result,no_encode);
	return result;
}

