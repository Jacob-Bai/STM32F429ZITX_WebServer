/*
 * wshttp.c
 *
 *  Created on: 20 Jan. 2022
 *      Author: Jacob Bai
 */

#include "wshttp.h"
#include "lwip/api.h"

// get the next packet from this connection
bool http_get_packet(HTTP_Request* req)
{
	err_t recv_err;
	// clean the buf
	netbuf_delete(req->netbuf);
	// looking for new packet
	recv_err = netconn_recv(req->conn, &(req->netbuf));
	if (recv_err == ERR_OK && !netconn_err(req->conn) && !netbuf_data(req->netbuf, (void**)&(req->ptr), &(req->buf_len))) {
		// packet found, reset pointer to 0
		req->cur_pos = 0;
		return true;
	}

	req->cur_pos = req->buf_len;
	return false;
}

// get one line from the current packet
bool http_get_line (HTTP_Request* req)
{
	req->str_len = 0;

	if (req->cur_pos >= req->buf_len && !http_get_packet(req)) {
		// we are at the end of this packet, and there isn't new packets
		return false;
	}

	while (req->str_len < MAX_CONTENT_SIZE && req->cur_pos < req->buf_len) {
		req->next_line[req->str_len] = *(req->ptr);
		req->ptr++;
		req->str_len++;
		req->cur_pos++;
		if (req->str_len > 1 && req->next_line[req->str_len - 1] == '\n' && req->next_line[req->str_len - 2] == '\r') {
			// we found the end of this line
			if (req->str_len < MAX_CONTENT_SIZE)
				// end of this string
				req->next_line[req->str_len] = '\0';
			return true;
		}
		else if (req->cur_pos == req->buf_len) {
			// reached the end of this packet, but line didn't finish, look for new packet
			if (!http_get_packet(req))
				return false;
		}
	}
	return false;
}

// get the whole body content from the packet
void http_get_content (HTTP_Request* req)
{
	req->str_len = 0;

	if (req->cur_pos >= req->buf_len && !http_get_packet(req)) {
		// this is the end of this request
		return;
	}

	req->str_len = req->buf_len - req->cur_pos;
	memcpy(req->next_line, req->ptr, req->str_len);
	req->cur_pos += req->str_len;
	req->ptr += req->str_len;

	if (req->str_len < MAX_CONTENT_SIZE)
		req->next_line[req->str_len] = '\0';
}

// send out netconn buf
void http_send_data (struct netconn *conn, const char *data, size_t len)
{
	if (len > 0)
		netconn_write (conn, data, len, NETCONN_NOCOPY);
}


