/*
 * wsApi.h
 *
 *  Created on: Jan 28, 2022
 *      Author: Jacob Bai
 */

#ifndef WSAPI_H_
#define WSAPI_H_

#include "wshttp.h"

// ----------------------------------------------------------------------------
// webserver backend functions
typedef void (*apiFunc) (HTTP_Request* req, void* param);

typedef struct ws_api
{
	const char *URI;
	apiFunc func;
	void* param;
} WS_Api;

void getSystemConfig(void);

extern const WS_Api apiList[];

#endif /* WSAPI_H_ */
