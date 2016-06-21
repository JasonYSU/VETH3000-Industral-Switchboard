#ifndef SYSTEM_MAINTAIN_H
#define SYSTEM_MAINTAIN_H
#include "board_info.h"
extern BOARD board_temp;
extern int isRestartDevice ;
extern uint8_t network_update_sucess;  //网络升级是否成功
char *recoverFactorySet();
char *generateConfigSaveResult();  //判断用户配置的信息是否保存
char* deviceRestart();
int saveAllConfigToFalsh(char *url);
char *getDeviceRestartStatus();
void processLogout(); //处理退出请求
void setRestartDeviceFlag();
int saveSystemImage(char *url);
char * transferData(char *url);
void processClient(void);
char* generateSystemUpdateStatus();
#endif