/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */
#include "opt.h"
#include "def.h"
#include "fs.h"
#include "fsdata.h"
#include "httpd-cgi.h"
#include "JSONUtility.h"
#include "tcp.h"
#include "Session.h"
#include <string.h>

/** Set this to 1 to include "fsdata_custom.c" instead of "fsdata.c" for the
 * file system (to prevent changing the file included in CVS) */
#ifndef HTTPD_USE_CUSTOM_FSDATA
#define HTTPD_USE_CUSTOM_FSDATA 0
#endif

#if HTTPD_USE_CUSTOM_FSDATA
#include "fsdata_custom.c"
#else /* HTTPD_USE_CUSTOM_FSDATA */
#include "fsdata.c"
#include "Session.h"
#endif /* HTTPD_USE_CUSTOM_FSDATA */

/*-----------------------------------------------------------------------------------*/

#if LWIP_HTTPD_CUSTOM_FILES
int fs_open_custom(struct fs_file *file, const char *name);
void fs_close_custom(struct fs_file *file);
#if LWIP_HTTPD_FS_ASYNC_READ
u8_t fs_canread_custom(struct fs_file *file);
u8_t fs_wait_read_custom(struct fs_file *file, fs_wait_cb callback_fn, void *callback_arg);
#endif /* LWIP_HTTPD_FS_ASYNC_READ */
#endif /* LWIP_HTTPD_CUSTOM_FILES */

/*-----------------------------------------------------------------------------------*/
/* 检测当前http连接的session是否存在 */
static uint8_t is_exist_session(){
	char *session_value = NULL;
	session_value = getSession("login");
	if(session_value){
		return 1;
	}else{
		return 0;
	}
}
err_t
fs_open(struct fs_file *file,const char *name) {
    const struct fsdata_file *f;
    char *ptr =  NULL;
	char *open_file_name = NULL;
	struct session_node *node;
	char *session_key;
	struct tcp_pcb *pcb;
    char *result = NULL;
	pcb = current_http_connection->pcb;
    if ((file == NULL) || (name == NULL)) {
        return ERR_ARG;
    }
    ptr = strchr(name,'.');
    if( !ptr) {  //数据文件请求不是网页请求
        generateDataFile(file,(char*)name);
        return ERR_OK;
    }
	/* 判断是否是文件下载请求 */
	if(is_file_download == 1){  //是文件请求
		switch(download_config_file_request){
			case VETH_ALL_CONFIG_FILE:
				final_JSON_result = generateConfigJSON();
				break;
			default:
				break;
		}
		file->data = final_JSON_result;
		file->len = strlen(file->data) * sizeof(char);
		file->index = file->len ;
		file->pextension = NULL;
		file->http_header_included = 0;
		return ERR_OK;
	}
	/* 只对html或者shtml页面进行过滤 */
	if( !is_exist_session() && strcmp(name,"/login.shtml") &&
		(!strcmp(ptr,".html") || !strcmp(ptr,".shtml"))
		){  //session 不存在并且不是登陆页面则跳转到登陆页面
		open_file_name = "/login.shtml";
	}else{
		open_file_name = (char*)name;
		/* 存在session，更新session定时器的值 */
		session_key = getSessionKey("login",
								current_http_connection->browser_info,
								current_http_connection->pcb->remote_ip.addr
								);
		node = (struct session_node *)malloc(sizeof(struct session_node));
		node->key = "login";
		node->session_key = session_key;
		updateSessionTime(session_root,node);
		node->key = NULL;
		node->session_key = NULL;
		free(session_key);
		free(node);
		session_key = NULL;
		node = NULL;
	}
#if LWIP_HTTPD_CUSTOM_FILES
    //if (fs_open_custom(file, name)) {
	if (fs_open_custom(file, open_file_name)) {
        file->is_custom_file = 1;
        return ERR_OK;
    }
    file->is_custom_file = 0;
#endif /* LWIP_HTTPD_CUSTOM_FILES */
	//printf("++++++will open file %s ++++++++\n\r",name);
    for (f = FS_ROOT; f != NULL; f = f->next) {
        //if (!strcmp(name, (char *)f->name)) {
		if (!strcmp(open_file_name, (char *)f->name)) {
            //printf("open file %s sucessful\n\r",f->name);
            file->data = (const char *)f->data;
            file->len = f->len;
            file->index = f->len;
            file->pextension = NULL;
            file->http_header_included = f->http_header_included;
#if HTTPD_PRECALCULATED_CHECKSUM
            file->chksum_count = f->chksum_count;
            file->chksum = f->chksum;
#endif /* HTTPD_PRECALCULATED_CHECKSUM */
#if LWIP_HTTPD_FILE_STATE
            file->state = fs_state_init(file, open_file_name);
#endif /* #if LWIP_HTTPD_FILE_STATE */
			//printf("file %s open sucessful\n\r",name);
            return ERR_OK;
        }
    }
    /* file not found */
    return ERR_VAL;
}

/*-----------------------------------------------------------------------------------*/
void
fs_close(struct fs_file *file) {
#if LWIP_HTTPD_CUSTOM_FILES
    if (file->is_custom_file) {
        fs_close_custom(file);
    }
#endif /* LWIP_HTTPD_CUSTOM_FILES */
#if LWIP_HTTPD_FILE_STATE
    fs_state_free(file, file->state);
#endif /* #if LWIP_HTTPD_FILE_STATE */
    LWIP_UNUSED_ARG(file);
}
/*-----------------------------------------------------------------------------------*/
#if LWIP_HTTPD_DYNAMIC_FILE_READ
#if LWIP_HTTPD_FS_ASYNC_READ
int
fs_read_async(struct fs_file *file, char *buffer, int count, fs_wait_cb callback_fn, void *callback_arg)
#else /* LWIP_HTTPD_FS_ASYNC_READ */
int
fs_read(struct fs_file *file, char *buffer, int count)
#endif /* LWIP_HTTPD_FS_ASYNC_READ */
{
    int read;

    if(file->index == file->len) {
        return FS_READ_EOF;
    }
#if LWIP_HTTPD_FS_ASYNC_READ
#if LWIP_HTTPD_CUSTOM_FILES
    if (!fs_canread_custom(file)) {
        if (fs_wait_read_custom(file, callback_fn, callback_arg)) {
            return FS_READ_DELAYED;
        }
    }
#else /* LWIP_HTTPD_CUSTOM_FILES */
    LWIP_UNUSED_ARG(callback_fn);
    LWIP_UNUSED_ARG(callback_arg);
#endif /* LWIP_HTTPD_CUSTOM_FILES */
#endif /* LWIP_HTTPD_FS_ASYNC_READ */

    read = file->len - file->index;
    if(read > count) {
        read = count;
    }

    MEMCPY(buffer, (file->data + file->index), read);
    file->index += read;

    return(read);
}
#endif /* LWIP_HTTPD_DYNAMIC_FILE_READ */
/*-----------------------------------------------------------------------------------*/
#if LWIP_HTTPD_FS_ASYNC_READ
int
fs_is_file_ready(struct fs_file *file, fs_wait_cb callback_fn, void *callback_arg) {
    if (file != NULL) {
#if LWIP_HTTPD_FS_ASYNC_READ
#if LWIP_HTTPD_CUSTOM_FILES
        if (!fs_canread_custom(file)) {
            if (fs_wait_read_custom(file, callback_fn, callback_arg)) {
                return 0;
            }
        }
#else /* LWIP_HTTPD_CUSTOM_FILES */
        LWIP_UNUSED_ARG(callback_fn);
        LWIP_UNUSED_ARG(callback_arg);
#endif /* LWIP_HTTPD_CUSTOM_FILES */
#endif /* LWIP_HTTPD_FS_ASYNC_READ */
    }
    return 1;
}
#endif /* LWIP_HTTPD_FS_ASYNC_READ */
/*-----------------------------------------------------------------------------------*/
int
fs_bytes_left(volatile struct fs_file *file) {
    return file->len - file->index;
}
