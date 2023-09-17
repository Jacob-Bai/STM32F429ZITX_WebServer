/*
 * wshttp.h
 *
 *  Created on: 20 Jan. 2022
 *      Author: Jacob Bai
 */

#ifndef WSHTTP_H_
#define WSHTTP_H_

#include "System_Config.h"
#include <wsBasic.h>

// --------------------------------------------------------------------------
// HTTP request
enum HTTP_Method{
	METHOD_ERR,
	METHOD_GET,
	METHOD_HEAD,
	METHOD_POST
} ;

enum URI_Type{
	PAGE,
	API,
	ITEM
} ;

#define MAX_CONTENT_SIZE  1460 //not the PBUFPOOL size, relevant to the TCP_MSS
#define MAX_URI_SIZE	64

typedef struct http_request {
	struct netconn 	*conn;
	struct netbuf* 	netbuf;
	char*	ptr;
	uint16_t	buf_len;

	char	next_line[MAX_CONTENT_SIZE];
	uint16_t	str_len;
	uint16_t	cur_pos;

	uint8_t	key;
	enum HTTP_Method	method;						/**< request method(METHOD_GET...). */
	enum URI_Type	type;						/**< request type*/
	char	uri[MAX_URI_SIZE];			/**< request file name.             */
	int	content_len;
	char username[CONFIG_USERNAME_SIZE+1];
	char password[CONFIG_PASSWORD_SIZE+1];
	char newpass[CONFIG_PASSWORD_SIZE+1];
} HTTP_Request;

// --------------------------------------------------------------------------
// HTTP response
typedef struct http_response
{
	const char *content;
	size_t length;
} HTTP_Response;

bool http_get_packet(HTTP_Request* req);
bool http_get_line (HTTP_Request* req);
void http_get_content (HTTP_Request* req);
void http_send_data (struct netconn *conn, const char *data, size_t len);

#endif /* WSHTTP_H_ */
