/*******************************************************
File name: user.c
Description: 提供检查用户登录是否合法的函数
Author: dujiancheng
Version: v1.0
********************************************************/
#include"user.h"
#include"request.h"
#include"httpd.h"
#include "session.h"
uint8_t login_status[32];  //存放用户登录时填写信息是否有误
/**
 * Function: checkLogin
 * Description: 检测用户登录信息是否合法
 * @param url 用户提交的登录表单数据
 * @return TRUE 用户合法
 *         FALSE 用户非法登录
**/
int checkLogin(char *url){
	char *user_name;
	char *password;
	char *p;
	p = strrchr(url,'\n');
	p = getHttpFormStartLocation(url);
	//printf("form content is %s\n\r",p);
	user_name = getParameter(p,"username");
	password = getParameter(p,"password");
	//printf("login_name is %s;pwd is %s\n\r",user_name,password);
	if( !strncmp(user_name,"vorx",4) && !strncmp(password,"vorx",4)){
		//session = getSessionId(url); //获取session
		//setSession(url); //登录成功设置session
		if( !getSession("login")){  //先判断session是否存在
			putSession("login","sucessful");  //不存在时设置session
			
		}
		free(user_name);
		user_name = NULL;
		free(password);
		password = NULL;
		return TRUE;
	}else{
		if(strncmp(user_name,"vorx",4)){  //用户名错误
			memset(login_status,0,sizeof(login_status));
			strcpy((char*)login_status,"user_name_failed");
		}else if(strncmp(password,"vorx",4)){    //密码错误
			memset(login_status,0,sizeof(login_status));
			strcpy((char*)login_status,"password_failed");
		}
	}
	free(user_name);
	user_name = NULL;
	free(password);
	password = NULL;
	return FALSE;
}