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
 * This version of the file has been modified by Texas Instruments to offer
 * simple server-side-include (SSI) and Common Gateway Interface (CGI)
 * capability.
 */

#ifndef __HTTPD_H__
#define __HTTPD_H__

#include "opt.h"
#include "err.h"
#include "pbuf.h"
#include "fs.h"

/** Set this to 1 to support CGI */
#ifndef LWIP_HTTPD_CGI
#define LWIP_HTTPD_CGI            0
#endif

/** Set this to 1 to support SSI (Server-Side-Includes) */
#ifndef LWIP_HTTPD_SSI
#define LWIP_HTTPD_SSI            1
#endif

/** Set this to 1 to support HTTP POST */
#ifndef LWIP_HTTPD_SUPPORT_POST
#define LWIP_HTTPD_SUPPORT_POST   1
#endif

/** Set this to 1 to support HTTP request coming in in multiple packets/pbufs */
#ifndef LWIP_HTTPD_SUPPORT_REQUESTLIST
#define LWIP_HTTPD_SUPPORT_REQUESTLIST      1
#endif

#if LWIP_HTTPD_DYNAMIC_HEADERS
/* The number of individual strings that comprise the headers sent before each
 * requested file.
 */
#define NUM_FILE_HDR_STRINGS  3
#endif /* LWIP_HTTPD_DYNAMIC_HEADERS */

#if LWIP_HTTPD_SSI
#ifndef LWIP_HTTPD_MAX_TAG_NAME_LEN
#define LWIP_HTTPD_MAX_TAG_NAME_LEN 8
#endif

/* The maximum length of string that can be returned to replace any given tag */
#ifndef LWIP_HTTPD_MAX_TAG_INSERT_LEN
#define LWIP_HTTPD_MAX_TAG_INSERT_LEN 192
#endif

enum ssi_indexs{
	SSI_INDEX_TOP_HTML,  //top.html对应的索引号
	SSI_INDEX_LEFT_HTML,  //left.html对应的索引号
	SSI_INDEX_SAVE_CONFIG,   //配置保存的结果状态
	SSI_INDEX_LOGIN_STATUS,   //用户登录时填写的信息是否有误
};

#define HTTPD_LAST_TAG_PART 0xFFFF

enum tag_check_state {
  TAG_NONE,       /* Not processing an SSI tag */
  TAG_LEADIN,     /* Tag lead in "<!--#" being processed */
  TAG_FOUND,      /* Tag name being read, looking for lead-out start */
  TAG_LEADOUT,    /* Tag lead out "-->" being processed */
  TAG_SENDING     /* Sending tag replacement string */
};

//#pragma pack(1)
struct http_ssi_state {
  const char *parsed;     /* Pointer to the first unparsed byte in buf. */
#if !LWIP_HTTPD_SSI_INCLUDE_TAG
  const char *tag_started;/* Pointer to the first opening '<' of the tag. */
#endif /* !LWIP_HTTPD_SSI_INCLUDE_TAG */
  const char *tag_end;    /* Pointer to char after the closing '>' of the tag. */
  u32_t parse_left; /* Number of unparsed bytes in buf. */
  u16_t tag_index;   /* Counter used by tag parsing state machine */
  u16_t tag_insert_len; /* Length of insert in string tag_insert */
#if LWIP_HTTPD_SSI_MULTIPART
  u16_t tag_part; /* Counter passed to and changed by tag insertion function to insert multiple times */
#endif /* LWIP_HTTPD_SSI_MULTIPART */
  u8_t tag_name_len; /* Length of the tag name in string tag_name */
  char tag_name[LWIP_HTTPD_MAX_TAG_NAME_LEN + 1]; /* Last tag name extracted */
  char tag_insert[LWIP_HTTPD_MAX_TAG_INSERT_LEN + 1]; /* Insert string for tag_name */
  enum tag_check_state tag_state; /* State of the tag processor */
};
//#pragma pack()
#endif /* LWIP_HTTPD_SSI */

//#pragma pack(1)
struct http_state {
#if LWIP_HTTPD_KILL_OLD_ON_CONNECTIONS_EXCEEDED
  struct http_state *next;
#endif /* LWIP_HTTPD_KILL_OLD_ON_CONNECTIONS_EXCEEDED */
  struct fs_file file_handle;
  struct fs_file *handle;
  char *file;       /* Pointer to first unsent byte in buf. */

  struct tcp_pcb *pcb;
#if LWIP_HTTPD_SUPPORT_REQUESTLIST
  struct pbuf *req;
#endif /* LWIP_HTTPD_SUPPORT_REQUESTLIST */

#if LWIP_HTTPD_DYNAMIC_FILE_READ
  char *buf;        /* File read buffer. */
  int buf_len;      /* Size of file read buffer, buf. */
#endif /* LWIP_HTTPD_DYNAMIC_FILE_READ */
  u32_t left;       /* Number of unsent bytes in buf. */
  u32_t retries;
#if LWIP_HTTPD_SUPPORT_11_KEEPALIVE
  u8_t keepalive;
#endif /* LWIP_HTTPD_SUPPORT_11_KEEPALIVE */
#if LWIP_HTTPD_SSI
  struct http_ssi_state *ssi;
#endif /* LWIP_HTTPD_SSI */
#if LWIP_HTTPD_CGI
  char *params[LWIP_HTTPD_MAX_CGI_PARAMETERS]; /* Params extracted from the request URI */
  char *param_vals[LWIP_HTTPD_MAX_CGI_PARAMETERS]; /* Values for each extracted param */
#endif /* LWIP_HTTPD_CGI */
#if LWIP_HTTPD_DYNAMIC_HEADERS
  const char *hdrs[NUM_FILE_HDR_STRINGS]; /* HTTP headers to be sent. */
  u16_t hdr_pos;     /* The position of the first unsent header byte in the
                        current string */
  u16_t hdr_index;   /* The index of the hdr string currently being sent. */
#endif /* LWIP_HTTPD_DYNAMIC_HEADERS */
#if LWIP_HTTPD_TIMING
  u32_t time_started;
#endif /* LWIP_HTTPD_TIMING */
#if LWIP_HTTPD_SUPPORT_POST
  u32_t post_content_len_left;
  struct pbuf *post_buf;   //存储当前连接的post数据
  u32_t copy_loc;          //记录向post_buf拷贝数据的位置
  uint8_t is_upload_file;    //是否是文件上传对应的post
#if LWIP_HTTPD_POST_MANUAL_WND
  u32_t unrecved_bytes;
  u8_t no_auto_wnd;
  u8_t post_finished;
#endif /* LWIP_HTTPD_POST_MANUAL_WND */
#endif /* LWIP_HTTPD_SUPPORT_POST*/

/* 存放用户使用的浏览器的相关信息 */
char *browser_info;  //用户使用的浏览器的版本信息
};
extern struct http_state *current_http_connection;   //记录当前的http连接

//#pragma pack()
#if LWIP_HTTPD_CGI

/*
 * Function pointer for a CGI script handler.
 *
 * This function is called each time the HTTPD server is asked for a file
 * whose name was previously registered as a CGI function using a call to
 * http_set_cgi_handler. The iIndex parameter provides the index of the
 * CGI within the ppcURLs array passed to http_set_cgi_handler. Parameters
 * pcParam and pcValue provide access to the parameters provided along with
 * the URI. iNumParams provides a count of the entries in the pcParam and
 * pcValue arrays. Each entry in the pcParam array contains the name of a
 * parameter with the corresponding entry in the pcValue array containing the
 * value for that parameter. Note that pcParam may contain multiple elements
 * with the same name if, for example, a multi-selection list control is used
 * in the form generating the data.
 *
 * The function should return a pointer to a character string which is the
 * path and filename of the response that is to be sent to the connected
 * browser, for example "/thanks.htm" or "/response/error.ssi".
 *
 * The maximum number of parameters that will be passed to this function via
 * iNumParams is defined by LWIP_HTTPD_MAX_CGI_PARAMETERS. Any parameters in the incoming
 * HTTP request above this number will be discarded.
 *
 * Requests intended for use by this CGI mechanism must be sent using the GET
 * method (which encodes all parameters within the URI rather than in a block
 * later in the request). Attempts to use the POST method will result in the
 * request being ignored.
 *
 */
typedef const char *(*tCGIHandler)(int iIndex, int iNumParams, char *pcParam[],
                             char *pcValue[]);

/*
 * Structure defining the base filename (URL) of a CGI and the associated
 * function which is to be called when that URL is requested.
 */
typedef struct
{
    const char *pcCGIName;
    tCGIHandler pfnCGIHandler;
} tCGI;

void http_set_cgi_handlers(const tCGI *pCGIs, int iNumHandlers);


/* The maximum number of parameters that the CGI handler can be sent. */
#ifndef LWIP_HTTPD_MAX_CGI_PARAMETERS
#define LWIP_HTTPD_MAX_CGI_PARAMETERS 16
#endif

#endif /* LWIP_HTTPD_CGI */

#if LWIP_HTTPD_SSI

/** LWIP_HTTPD_SSI_MULTIPART==1: SSI handler function is called with 2 more
 * arguments indicating a counter for insert string that are too long to be
 * inserted at once: the SSI handler function must then set 'next_tag_part'
 * which will be passed back to it in the next call. */
#ifndef LWIP_HTTPD_SSI_MULTIPART
#define LWIP_HTTPD_SSI_MULTIPART    0
#endif

/*
 * Function pointer for the SSI tag handler callback.
 *
 * This function will be called each time the HTTPD server detects a tag of the
 * form <!--#name--> in a .shtml, .ssi or .shtm file where "name" appears as
 * one of the tags supplied to http_set_ssi_handler in the ppcTags array.  The
 * returned insert string, which will be appended after the the string
 * "<!--#name-->" in file sent back to the client,should be written to pointer
 * pcInsert.  iInsertLen contains the size of the buffer pointed to by
 * pcInsert.  The iIndex parameter provides the zero-based index of the tag as
 * found in the ppcTags array and identifies the tag that is to be processed.
 *
 * The handler returns the number of characters written to pcInsert excluding
 * any terminating NULL or a negative number to indicate a failure (tag not
 * recognized, for example).
 *
 * Note that the behavior of this SSI mechanism is somewhat different from the
 * "normal" SSI processing as found in, for example, the Apache web server.  In
 * this case, the inserted text is appended following the SSI tag rather than
 * replacing the tag entirely.  This allows for an implementation that does not
 * require significant additional buffering of output data yet which will still
 * offer usable SSI functionality.  One downside to this approach is when
 * attempting to use SSI within JavaScript.  The SSI tag is structured to
 * resemble an HTML comment but this syntax does not constitute a comment
 * within JavaScript and, hence, leaving the tag in place will result in
 * problems in these cases.  To work around this, any SSI tag which needs to
 * output JavaScript code must do so in an encapsulated way, sending the whole
 * HTML <script>...</script> section as a single include.
 */
typedef u16_t (*tSSIHandler)(int iIndex, char *pcInsert, int iInsertLen
#if LWIP_HTTPD_SSI_MULTIPART
                             , u16_t current_tag_part, u16_t *next_tag_part
#endif /* LWIP_HTTPD_SSI_MULTIPART */
#if LWIP_HTTPD_FILE_STATE
                             , void *connection_state
#endif /* LWIP_HTTPD_FILE_STATE */
                             );

void http_set_ssi_handler(tSSIHandler pfnSSIHandler,
                          const char **ppcTags, int iNumTags);

/* The maximum length of the string comprising the tag name */


#endif /* LWIP_HTTPD_SSI */

#if LWIP_HTTPD_SUPPORT_POST

/* These functions must be implemented by the application */

/** Called when a POST request has been received. The application can decide
 * whether to accept it or not.
 *
 * @param connection Unique connection identifier, valid until httpd_post_end
 *        is called.
 * @param uri The HTTP header URI receiving the POST request.
 * @param http_request The raw HTTP request (the first packet, normally).
 * @param http_request_len Size of 'http_request'.
 * @param content_len Content-Length from HTTP header.
 * @param response_uri Filename of response file, to be filled when denying the
 *        request
 * @param response_uri_len Size of the 'response_uri' buffer.
 * @param post_auto_wnd Set this to 0 to let the callback code handle window
 *        updates by calling 'httpd_post_data_recved' (to throttle rx speed)
 *        default is 1 (httpd handles window updates automatically)
 * @return ERR_OK: Accept the POST request, data may be passed in
 *         another err_t: Deny the POST request, send back 'bad request'.
 */
err_t httpd_post_begin(void *connection, const char *uri, const char *http_request,
                       u16_t http_request_len, int content_len, char *response_uri,
                       u16_t response_uri_len, u8_t *post_auto_wnd);

/** Called for each pbuf of data that has been received for a POST.
 * ATTENTION: The application is responsible for freeing the pbufs passed in!
 *
 * @param connection Unique connection identifier.
 * @param p Received data.
 * @return ERR_OK: Data accepted.
 *         another err_t: Data denied, http_post_get_response_uri will be called.
 */
err_t httpd_post_receive_data(void *connection, struct pbuf *p);

/** Called when all data is received or when the connection is closed.
 * The application must return the filename/URI of a file to send in response
 * to this POST request. If the response_uri buffer is untouched, a 404
 * response is returned.
 *
 * @param connection Unique connection identifier.
 * @param response_uri Filename of response file, to be filled when denying the request
 * @param response_uri_len Size of the 'response_uri' buffer.
 */
void httpd_post_finished(void *connection, char *response_uri, u16_t response_uri_len);

#ifndef LWIP_HTTPD_POST_MANUAL_WND
#define LWIP_HTTPD_POST_MANUAL_WND  0
#endif

#if LWIP_HTTPD_POST_MANUAL_WND
void httpd_post_data_recved(void *connection, u16_t recved_len);
#endif /* LWIP_HTTPD_POST_MANUAL_WND */

#endif /* LWIP_HTTPD_SUPPORT_POST */

void httpd_init(void);
extern int url_buf_length; //记录url_buf缓存中实际数据的长度
extern int http_header_length; //记录http header的长度
#endif /* __HTTPD_H__ */
