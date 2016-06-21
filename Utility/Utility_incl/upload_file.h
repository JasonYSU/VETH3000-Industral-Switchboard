#ifndef UPLOAD_FILE_H
#define UPLOAD_FILE_H
#include "request.h"
#define  BOUNDARY_KWD  "boundary="  //定义边界开始字符串
/* 获取上传的文件名 */
char *getUploadFileName(char *url,char *boundary);
/* 获取上传的文件内容
**返回值是获取的文件内容,参数file_size存储获取的文件的大小
 */
char *getUploadFileContent(char *url,char *boundary,int *file_size);
#endif