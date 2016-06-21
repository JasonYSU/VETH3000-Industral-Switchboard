#ifndef JSON_UTILITY_H
#define JSON_UTILITY_H
#include "cJSON.h"
extern char *final_JSON_result;
void parseJSONConetnt(char *content);
char* generateConfigJSON();
/* 删除之前Board里面存在的内容 */
unsigned char deleteBoardContent();
/* 重新生成board中的数据 */
unsigned char generateBoardContent();
#endif