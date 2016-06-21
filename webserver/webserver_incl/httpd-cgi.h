/**
 * \addtogroup httpd
 * @{
 */

/**
 * \file
 *         Web server script interface header file
 * \author
 *         Adam Dunkels <adam@sics.se>
 *
 */


/*
 * Copyright (c) 2001, Adam Dunkels.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote
 *    products derived from this software without specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * This file is part of the uIP TCP/IP stack.
 *
 * $Id: httpd-cgi.h,v 1.2 2006/06/11 21:46:38 adam Exp $
 *
 */

#ifndef __HTTPD_CGI_H__
#define __HTTPD_CGI_H__

//#include "port-data.h"

#include "httpd.h"
//#include "mac.h"
//#include "mac_table.h"
//#include "gbk_conv_hanzi.h"
#include "ports.h"
#include "fs.h"
//#include "web_main.h"
#define ROW_COUNT 256 
extern volatile uint64_t pre_rx_byte[PORTSNUM];
extern volatile uint64_t pre_tx_byte[PORTSNUM];
#if 0
typedef PT_THREAD((* httpd_cgifunction)(struct http_state *, char *));

httpd_cgifunction httpd_cgi(char *name);

struct httpd_cgi_call {
  const char *name;
  const httpd_cgifunction function;
};
#endif
/*
static char *upDown[]={"Down","Up"};
static char *speedType[]={"10mhdx","10mfdx","100mhdx","100mfdx","1gfdx","2.5gfdx"};
static int portsConfig[12][19] = {
	{1,12351,1,1,1,0,9600,2,0,0,0,0,0,2,0,2,0,2,0},
	{2,12351,1,0,2,1,9600,2,0,1,3,0,0,2,0,2,0,2,0},
	{3,12351,1,1,3,1,9600,2,0,0,0,0,0,2,0,2,0,2,0},
	{4,12351,1,1,2,1,9600,2,0,1,3,0,0,2,0,2,0,2,0},
	{5,12351,1,1,2,1,9600,2,0,0,0,0,0,2,0,2,0,2,0},
	{6,12351,1,1,3,1,9600,2,0,0,0,0,0,2,0,2,0,2,0},
	{7,12351,1,1,3,1,9600,2,0,0,0,0,0,2,0,2,0,2,0},
	{8,12351,1,1,3,1,9600,2,0,0,0,0,0,2,0,2,0,2,0},
	{9,9310321,1,0,2,1,9600,2,0,0,0,0,0,1,0,1,0,2,0},
	{10,9310321,1,0,2,1,9600,2,0,0,0,0,0,1,0,1,0,2,0},
	{11,9310257,1,1,2,1,9600,2,0,0,0,0,0,1,0,1,0,2,0},
	{12,9310257,1,1,3,1,9600,2,0,0,0,0,0,1,0,1,0,2,0}
};*/
static int currentTotalPage = 0;
//static char *initPorts(void);
//static char *initAllPortsStatus(void);
//static char *initClearStatus(void);
//void modifyPorts(int row,int *desPorts);
//void clearPortDataFile(struct http_state *s,char *portNo);
void generatePortDataFile(struct fs_file *file,char *portNo);
//char *generatePreviewData(void);
/**
 * \brief      HTTPD CGI function declaration
 * \param name The C variable name of the function
 * \param str  The string name of the function, used in the script file
 * \param function A pointer to the function that implements it
 *
 *             This macro is used for declaring a HTTPD CGI
 *             function. This function is then added to the list of
 *             HTTPD CGI functions with the httpd_cgi_add() function.
 *
 * \hideinitializer
 */
//#define HTTPD_CGI_CALL(name, str, function) \
//static PT_THREAD(function(struct http_state *, char *)); \
static const struct httpd_cgi_call name = {str, function}
void processPostRequest(char *url,struct http_state *s,char *file_name);
u8_t *getReferer(char *url);
//static char *getParameter(char *url,char *name);
//char *generateMacData();
//void freeMemory(char *ptr);
void getFileName(char *url,char *father,char *child);
void generateDataFile(struct fs_file *file,char *url);
void httpd_cgi_init(void);
#endif /* __HTTPD_CGI_H__ */

/** @} */
