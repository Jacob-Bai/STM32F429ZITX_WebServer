/*
 * webserver.c
 *
 *  Created on: 20 Jan. 2022
 *      Author: Jacob Bai
 */

#include "lwip/api.h"
#include "memory.h"


#include "log.h"
#include "crc.h"

#include "key.h"
#include "wsBasic.h"
#include "wsApi.h"

// read out API path
void ParseRequestUri (HTTP_Request* req)
{
	char* nexttok = strtok(req->next_line, " ");
	nexttok = strtok(NULL, " ");
	strcpy((char*)req->uri, nexttok);
}

// read method
void ParseRequestMethod (HTTP_Request* req)
{
	if(strstr(req->next_line, "GET") || strstr(req->next_line,"get"))
	{
		req->method = METHOD_GET;
	}
	else if (strstr(req->next_line, "HEAD") || strstr(req->next_line,"head"))
	{
		req->method = METHOD_HEAD;
	}
	else if (strstr(req->next_line, "POST") || strstr(req->next_line,"post"))
	{
		req->method = METHOD_POST;
	}
	else
	{
		req->method = METHOD_ERR;
	}
}

// parse contents in header
bool ParseRequestHeader (HTTP_Request* req)
{
	char tmp_buf[10];

	if(!http_get_line(req))
		return false;

	ParseRequestMethod(req);
	ParseRequestUri(req);

	while(1) {
		if(!http_get_line(req) ) // can not get new line
			return false;
		if (req->str_len == 2) {// this is the end of the header
			return true;
		}
		if (STRNCMP(req->next_line, "Content-Length", 14)) {
			memset(tmp_buf,0,10);
			mid(req->next_line, "Content-Length: ", "\r\n", tmp_buf); // check content length if update image request
			req->content_len = ATOI(tmp_buf);
		} else if (STRNCMP(req->next_line, "Cookie", 6)) {
			memset(tmp_buf,0,10);
			mid(req->next_line, "KEY=", "\r\n", tmp_buf); // looking for token
			req->key = (uint8_t)ATOI(tmp_buf);
		} else if (STRNCMP(req->next_line, "username", 8)) {
			mid(req->next_line, "username: ", "\r\n", (char *)req->username); // save username if login request
		} else if (STRNCMP(req->next_line, "password", 8)) {
			mid(req->next_line, "password: ", "\r\n", (char *)req->password); // save password if login request
		} else if (STRNCMP(req->next_line, "newpass", 7)) {
			mid(req->next_line, "newpass: ", "\r\n", (char *)req->newpass); // save new password if setting password request
		}
	}

	return false;
}

void ProcRequest (HTTP_Request* req) {
	uint8_t ind = 0;
	for (;;) {
		if (!apiList[ind].URI)
			return;
		if (!strcmp(req->uri, apiList[ind].URI)) {
			apiList[ind].func(req, apiList[ind].param); // looking for API entry based on the URI path
			return;
		}
		else
			ind++;
	}
}
// --------------------------------------------------------------------------

void ProcConn (HTTP_Request* req, struct netconn *conn)
{
	memset(req, 0, sizeof(HTTP_Request));
	req->conn = conn;
	netconn_set_recvtimeout(req->conn, 3);
	if (!http_get_packet(req))
	  goto end;
	if (!ParseRequestHeader(req))
	  goto end;
	ProcRequest(req);

end:
	netbuf_delete(req->netbuf);
	netconn_close (conn);

}


// --------------------------------------------------------------------------

void WebserverTask (void *pvParameters)
	{
	struct netconn *conn;
	err_t err;
	HTTP_Request* req = (HTTP_Request*) pvParameters;

	// Create a new TCP connection handle
	conn = netconn_new (NETCONN_TCP);

	if (conn != NULL)
	{
		// Bind to port 80 (HTTP) with default IP address
		err = netconn_bind (conn, NULL, 80);

		if (err == ERR_OK)
			{
			// Put the connection into LISTEN state
			netconn_listen(conn);

			while(1)
			{

				struct netconn *newconn;
				// accept any incoming connection
				err = netconn_accept(conn, &newconn);
				if (err == ERR_OK)
				{

					// proc connection
					ProcConn (req, newconn);

					// delete connection
					netconn_delete (newconn);
				}
//				osDelay(1);
			}
		}
		else
		{
			// delete connection
			netconn_delete (conn);
		}
	}
}




// --------------------------------------------------------------------------
#define WS_Stack_Size	256
HTTP_Request req;
static StackType_t WSStackBuffer[ WS_Stack_Size * 4 ];
static StaticTask_t WSTaskBuffer;


void init_Webserver (void)
{
	getSystemConfig(); // load current system config
	initKeyList(); // init key cache
	xTaskCreateStatic (WebserverTask, "WebServer", WS_Stack_Size * 4,
			(void*)&req, osPriorityRealtime, WSStackBuffer, &WSTaskBuffer);


}
