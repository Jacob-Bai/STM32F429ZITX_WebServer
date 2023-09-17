/*
 * wsApi.c
 *
 *  Created on: Jan 28, 2022
 *      Author: Jacob Bai
 */

#include "wsApi.h"
#include "uifiles.h"
#include "config.h"
#include "key.h"
#include "cmsis_os.h"
#include "flashio.h"
#include "crc.h"
#include "timetick.h"

// The header contents
const HTTP_Response http_res_HTTPOK = { "HTTP/1.1 200 OK\r\n", 17 };
const HTTP_Response http_res_HTTPBad = { "HTTP/1.1 400 Bad Request\r\n", 26 };
const HTTP_Response http_res_HTTPUnauth = { "HTTP/1.1 401 Unauthorized\r\n", 27 };
const HTTP_Response http_res_HTTPForbidden = { "HTTP/1.1 403 Forbidden\r\n", 24 };
const HTTP_Response http_res_ServerName = { "Server: Embedded Web Server v2.0\r\n", 34 };
const HTTP_Response http_res_CacheDisable = { "Pragma: no-cache\r\n", 18 };
const HTTP_Response http_res_ConnectionType = { "Connection: Close\r\n", 19 };
const HTTP_Response	http_res_SetCookieA = { "Set-Cookie: KEY=", 16 };
const HTTP_Response	http_res_SetCookieB = { "; Max-Age=1800; Path=/;\r\n", 25 };
const HTTP_Response	http_res_ClearCookie = { "Set-Cookie: KEY=; Max-Age=0;\r\n", 30 };

// header for gzip html file, can save huge flash space
const HTTP_Response http_res_EncondGzip = { "Content-Encoding: gzip", 22 };
const HTTP_Response http_res_ContentHTML = { "Content-Type: text/html; charset:UTF-8\r\n", 40 };
const HTTP_Response http_res_ContentDATA = { "Content-Type: text/plain; charset:UTF-8\r\n", 41 };
const HTTP_Response http_res_ContentCSS = { "Content-Type: text/css\r\n", 24 };
const HTTP_Response http_res_ContentGif = { "Content-Type: image/gif\r\n", 25 };
const HTTP_Response http_res_ContentJS = { "Content-Type: text/javascript\r\n", 31 };

// variables that stores the settings in string
char DEV_MAC[17+1];
char DEV_SW_Ver[8+1];
char DEV_Units[1+1];
char DEV_Instance[10+1];
char DEV_Datalink[1+1];
char DEV_MSTP_MAC[3+1];
char DEV_IP_Mode[1+1];
char DEV_IP[15+1];
char DEV_NM[15+1];
char DEV_GW[15+1];
char DEV_BAC_NetNum[1+1];
char DEV_IP_Port[6+1];

// time string
char timeNow[23] = {'0'};

// config response body format
// D0|xxxx/r/n
// D1|xxxx/r/n
// D3|xxxx/r/n
//...
size_t config_contentLength = 0;
char* config_content[] = {
	"D0|", DEV_SW_Ver,
	"D1|", DEV_MAC,
	"D2|", SysConfig.device.location,
	"D3|", SysConfig.device.description,
	"D4|", DEV_Units,
	"D5|", DEV_Instance,
	"D6|", DEV_Datalink,
	"D7|", DEV_BAC_NetNum,
	"D8|", DEV_IP_Port,
	"D9|", DEV_MSTP_MAC,
	"D10|", DEV_IP_Mode,
	"D11|", DEV_IP,
	"D12|", DEV_NM,
	"D13|", DEV_GW,
	NULL, NULL,
};

#define sendHeaderOK(conn) \
	http_send_data (conn, http_res_HTTPOK.content, http_res_HTTPOK.length); \
	http_send_data (conn, http_res_CacheDisable.content, http_res_CacheDisable.length); \
	http_send_data (conn, http_res_ConnectionType.content, http_res_ConnectionType.length)

#define sendHeaderUnauth(conn) \
	http_send_data (conn, http_res_HTTPUnauth.content, http_res_HTTPUnauth.length); \
	http_send_data (conn, http_res_CacheDisable.content, http_res_CacheDisable.length); \
	http_send_data (conn, http_res_ConnectionType.content, http_res_ConnectionType.length)

#define sendHeaderBad(conn) \
	http_send_data (conn, http_res_HTTPBad.content, http_res_HTTPBad.length); \
	http_send_data (conn, http_res_CacheDisable.content, http_res_CacheDisable.length); \
	http_send_data (conn, http_res_ConnectionType.content, http_res_ConnectionType.length)

#define sendHeaderForbidden(conn) \
	http_send_data (conn, http_res_HTTPForbidden.content, http_res_HTTPForbidden.length); \
	http_send_data (conn, http_res_CacheDisable.content, http_res_CacheDisable.length); \
	http_send_data (conn, http_res_ConnectionType.content, http_res_ConnectionType.length)

// parse ip address
void parseAddr(char* s, uint8_t* addr)
{
	char tmp[4] = {0};
	uint8_t i;
	for (i=0;*s;s++) {
		if (*s == '.') {
			tmp[i] = 0;
			*addr = ATOI(tmp);
			i=0;
			addr++;
		} else {
			tmp[i] = *s;
			i++;
		}
	}
	tmp[i] = 0;
	*addr = ATOI(tmp);
}

// convert config string into system settings and write onto flash
bool setSystemConfig (void)
{
	// BACnet instance number
	SysConfig.BACnet.instanceNumber = ATOI(DEV_Instance);

	// IP or MSTP or Seg
	SysConfig.BACnet.dataLink.type = ATOI(DEV_Datalink);

	// mstp mac address
	SysConfig.BACnet.dataLink.mstp.macAdd = ATOI(DEV_MSTP_MAC);

	// DHCP or Manual
	SysConfig.OS.network.mode = ATOI(DEV_IP_Mode);

	// IP address
	parseAddr(DEV_IP, SysConfig.OS.network.ip);

	// netmask
	parseAddr(DEV_NM, SysConfig.OS.network.netmask);

	// gateway
	parseAddr(DEV_GW, SysConfig.OS.network.gateway);

	// network number
	SysConfig.BACnet.dataLink.ip.networkNumber = ATOI(DEV_BAC_NetNum);

	// bacnet port
	SysConfig.BACnet.dataLink.ip.port = ATOI(DEV_IP_Port);

	if (!eraseSectors(CONFIG_START_SECTOR, CONFIG_NUM_SECTORS))
		return false;
	unlockFlash();
	writeFlash(CONFIG_ADDR, (uint8_t*) &SysConfig, sizeof(System_Config));

	for (uint16_t i = 0; i < sizeof(System_Config); i++) {
		if (*(uint8_t*)( ((uint8_t*)&SysConfig) + i) != *(uint8_t*)(CONFIG_ADDR + i)) {
			eraseSectors(CONFIG_START_SECTOR, CONFIG_NUM_SECTORS);
			lockFlash();
			return false;
		}
	}
	lockFlash();
	return true;

}

// convert current settings into response string
void getSystemConfig(void) {

	// Set software version
	initString(DEV_SW_Ver, "%d.%d.%d", (uint8_t)(SysConfig.OS.version>>16)&0xff, (uint8_t)(SysConfig.OS.version>>8)&0xff, (uint8_t)SysConfig.OS.version&0xff);

	// Set MAC address
	for (uint8_t i = 0; i < 6; i++)
	{
		if (i > 0)
			DEV_MAC[i*3-1] = ':';
		DEV_MAC[i*3] = H2C (SysConfig.OS.network.hwMac[i]/ 16);
		DEV_MAC[i*3+1] = H2C (SysConfig.OS.network.hwMac[i]% 16);
	}

	// Metric or US
	initString(DEV_Units, "%d", 0);

	// BACnet instance number
	initString(DEV_Instance, "%lu", SysConfig.BACnet.instanceNumber);

	// IP or MSTP or Seg
	initString(DEV_Datalink, "%d", SysConfig.BACnet.dataLink.type);

	// mstp mac address
	initString(DEV_MSTP_MAC, "%d", SysConfig.BACnet.dataLink.mstp.macAdd);

	// DHCP or Manual
	initString(DEV_IP_Mode, "%d", SysConfig.OS.network.mode);

	// IP address
	initString(DEV_IP, "%d.%d.%d.%d", SysConfig.OS.network.ip[0], SysConfig.OS.network.ip[1], SysConfig.OS.network.ip[2], SysConfig.OS.network.ip[3]);

	// netmask
	initString(DEV_NM, "%d.%d.%d.%d", SysConfig.OS.network.netmask[0], SysConfig.OS.network.netmask[1], SysConfig.OS.network.netmask[2], SysConfig.OS.network.netmask[3]);

	// gateway
	initString(DEV_GW, "%d.%d.%d.%d", SysConfig.OS.network.gateway[0], SysConfig.OS.network.gateway[1], SysConfig.OS.network.gateway[2], SysConfig.OS.network.gateway[3]);

	// network number
	initString(DEV_BAC_NetNum, "%d", SysConfig.BACnet.dataLink.ip.networkNumber);

	// bacnet port
	initString(DEV_IP_Port, "%d", SysConfig.BACnet.dataLink.ip.port);

	for (uint8_t i=0; config_content[i] != NULL; i+=2) {
		config_contentLength += strlen(config_content[i]) + strlen(config_content[i+1]) + 2;
	}

}

// response with UI files
void SendUIFile (HTTP_Request* req, void* param)
{
	const UI_FILE* uifile = (UI_FILE*)param;

	char contentLenStr [11] = {'\0'};
	sprintf(contentLenStr, "%u", uifile->length);
	size_t contentLenStrSize = strlen(contentLenStr);

	sendHeaderOK(req->conn);
//	http_send_data (req->conn, http_res_ContentLength.content, http_res_ContentLength.length);
//	http_send_data (req->conn, contentLenStr, contentLenStrSize);
//	http_send_data (req->conn, "\r\n", 2);

	switch (uifile->fileType) {
	case CSS:
		http_send_data (req->conn, http_res_ContentCSS.content, http_res_ContentCSS.length);
		break;
	case JS:
		http_send_data (req->conn, http_res_ContentJS.content, http_res_ContentJS.length);
		break;
	case GIF:
		http_send_data (req->conn, http_res_ContentGif.content, http_res_ContentGif.length);
		break;
	default:
		break;
	}
	http_send_data (req->conn, "\r\n", 2);
	http_send_data (req->conn, uifile->ptr, uifile->length);
}

// send home page back
void SendHomePage (HTTP_Request* req, void* param)
{
	const UI_FILE* uifile = (UI_FILE*)param;
	sendHeaderOK(req->conn);
	// the ui bundle file is gzip format
	http_send_data (req->conn, http_res_EncondGzip.content, http_res_EncondGzip.length);
		http_send_data (req->conn, "\r\n", 2);
	http_send_data (req->conn, http_res_ContentHTML.content, http_res_ContentHTML.length);
	http_send_data (req->conn, "\r\n", 2);

	http_send_data (req->conn, uifile->ptr, uifile->length);
}

// send config or received config
void ApiConfigRes (HTTP_Request* req, void* param)
{
	if (keyExpired(req->key)) {
		sendHeaderUnauth(req->conn);
		http_send_data (req->conn, "\r\n", 2);
		return;
	}

	if (req->method == METHOD_GET) {
		char contentLenStr[11] = {'\0'};
		sprintf(contentLenStr, "%u", config_contentLength);
		size_t contentLenStrSize = strlen(contentLenStr);

		sendHeaderOK(req->conn);
		http_send_data (req->conn, contentLenStr, contentLenStrSize);
		http_send_data (req->conn, "\r\n", 2);
		http_send_data (req->conn, http_res_ContentDATA.content, http_res_ContentDATA.length);
		http_send_data (req->conn, "\r\n", 2);

		for (uint8_t i=0; config_content[i] != NULL; i+=2) {
			http_send_data (req->conn, config_content[i], strlen(config_content[i]));
			http_send_data (req->conn, config_content[i+1], strlen(config_content[i+1]));
			http_send_data (req->conn, "\r\n", 2);
		}
	} else if (req->method == METHOD_POST) {
		uint8_t i = 0;
		while(http_get_line(req)) {
			if (req->next_line[2] == '|')
				i = C2H(req->next_line[1]);
			else if (req->next_line[3] == '|')
				i = C2H(req->next_line[2]) + 10;
			else
				break;
			// parse body content into configs
			if (i <= 13) {
				memset(config_content[i*2+1], 0, strlen(config_content[i*2+1]));
				mid(req->next_line, config_content[i*2], "\r\n", config_content[i*2+1]);
			}

			req->content_len -= req->str_len;
			if (req->content_len<=0)
				// parsed all contents
				break;
		}
		if (!setSystemConfig()) {
			sendHeaderBad(req->conn);
			http_send_data (req->conn, "\r\n", 2);
		} else {
			getSystemConfig();
			sendHeaderOK(req->conn);
			http_send_data (req->conn, "\r\n", 2);
		}
	}
}

// update new image
void ApiSysUpdateRes (HTTP_Request* req, void* param)
{
	uint32_t writeAddr = NEW_IMG_ADDR;
	uint8_t erase = 0;
	uint32_t crcVal = 0;
	int payloadLen = req->content_len;
	unlockFlash();
	while (1) {
		// keep read out content from multiple packets
		http_get_content(req);
		if (!req->str_len) {
			// no more packets available but content didn't finish
			erase = 1;
			break;
		}

		// count how many byte received
		payloadLen -= req->str_len;
		if (payloadLen >= 0) {
			if (!crcVal) {
				crcVal = *((uint32_t*)req->next_line);
				// crc not available, dont write
				if (!crcVal) {
					erase = 2;
					break;
				}
				writeFlash(writeAddr, (uint8_t*)req->next_line + 4, req->str_len - 4);
				writeAddr += req->str_len - 4;
			} else {
				writeFlash(writeAddr, (uint8_t*)req->next_line, req->str_len);
				writeAddr += req->str_len;
			}

			if (payloadLen == 0) {
				// successfully received all packets
				break;
			}
		} else {
			// received more packets than supposed
			erase=1;
			break;
		}
	}

	if (!erase) {
		int len = req->content_len - 4; // without crc value
		if (len%4) // if len not divided by 4 than scale up
			len += 4 - len%4;
		len = len/4;
		uint32_t res = HAL_CRC_Calculate(&hcrc, (uint32_t*)NEW_IMG_ADDR, len);
		if (res != crcVal)
			erase = 1;
	}

	if (erase == 1)
		eraseSectors(NEW_IMG_START_SECTOR, NEW_IMG_NUM_SECTORS);

	lockFlash();

	if (erase) {
		sendHeaderBad(req->conn);
	} else {
		sendHeaderOK(req->conn);
	}

	http_send_data (req->conn, "\r\n", 2);


}

// login request
void ApiLoginRes (HTTP_Request* req, void* param)
{
	if (!strcmp(req->username, SysConfig.OS.login.name) &&
			!strcmp(req->password, SysConfig.OS.login.password)) {
		// authentication done
		// reply the token
		uint8_t key = newKey();
		char keyStr[2] = {'1',0};
		sendHeaderOK(req->conn);
		http_send_data (req->conn, http_res_SetCookieA.content, http_res_SetCookieA.length);
		if (key < 10) {
			keyStr[0] = H2C(key);
			http_send_data (req->conn, keyStr, 1);
		} else {
			keyStr[1] = H2C(key-10);
			http_send_data (req->conn, keyStr, 2);
		}
		http_send_data (req->conn, http_res_SetCookieB.content, http_res_SetCookieB.length);
	} else {
		sendHeaderUnauth(req->conn);
	}
	http_send_data (req->conn, "\r\n", 2);
}

// logout remove token
void ApiLogoutRes (HTTP_Request* req, void* param)
{
	rmKey(req->key);
	sendHeaderOK(req->conn);
	http_send_data (req->conn, http_res_ClearCookie.content, http_res_SetCookieA.length);
	http_send_data (req->conn, "\r\n", 2);
}

// set new password
void ApiPasswordRes (HTTP_Request* req, void* param)
{
	if (!strcmp(req->password, SysConfig.OS.login.password)) {
		initString(SysConfig.OS.login.name, req->username);
		initString(SysConfig.OS.login.password, req->newpass);
		sendHeaderOK(req->conn);
	} else {
		sendHeaderForbidden(req->conn);
	}
	http_send_data (req->conn, "\r\n", 2);
}

// reboot system
// TODO: need a proper reboot
void ApiSysRebootRes (HTTP_Request* req, void* param)
{
	sendHeaderOK(req->conn);
	http_send_data (req->conn, "\r\n", 2);
	osDelay(100);
	NVIC_SystemReset();
}

// time now
// body format: yyyy-mm-ddThh:mm:ss.000
// 000 can be ms
void ApiNowRes (HTTP_Request* req, void* param) {
	Sys_DateTime dt;
	// convert tick to time date
	tick2DateTime(&dt, getSysTick());

	memset(timeNow, 0, sizeof(timeNow));
	sprintf(timeNow, "%d-",2000 + dt.date.year);
	if (dt.date.month < 10)
		sprintf(timeNow+5, "0%d-", dt.date.month);
	else
		sprintf(timeNow+5, "%d-", dt.date.month);

	if (dt.date.day < 10)
		sprintf(timeNow+8, "0%dT", dt.date.day);
	else
		sprintf(timeNow+8, "%dT", dt.date.day);

	if (dt.time.hour < 10)
		sprintf(timeNow+11, "0%d:", dt.time.hour);
	else
		sprintf(timeNow+11, "%d:", dt.time.hour);

	if (dt.time.minute < 10)
		sprintf(timeNow+14, "0%d:", dt.time.minute);
	else
		sprintf(timeNow+14, "%d:", dt.time.minute);

	if (dt.time.second < 10)
		sprintf(timeNow+17, "0%d.", dt.time.second);
	else
		sprintf(timeNow+17, "%d.", dt.time.second);
	timeNow[20] = '0';
	timeNow[21] = '0';
	timeNow[22] = '0';

	sendHeaderOK(req->conn);
	http_send_data (req->conn, http_res_ContentDATA.content, http_res_ContentDATA.length);
	http_send_data (req->conn, "\r\n", 2);
	http_send_data (req->conn, timeNow, sizeof(timeNow));
}

// setup system time
void ApiTimeRes (HTTP_Request* req, void* param) {

	Sys_DateTime dt;
	char temp[5];

	http_get_content(req);
	if (req->str_len != 16) {
		sendHeaderBad(req->conn);
		http_send_data (req->conn, "\r\n", 2);
		return;
	}

	//yyyy-mm-ddThh:mm
	memset(temp, 0, sizeof(temp));
	memcpy(temp, req->next_line, 4);
	dt.date.year = (uint8_t)(ATOI(temp) - 2000);

	memset(temp, 0, sizeof(temp));
	memcpy(temp, req->next_line + 5, 2);
	dt.date.month = (uint8_t)(ATOI(temp));

	memset(temp, 0, sizeof(temp));
	memcpy(temp, req->next_line + 8, 2);
	dt.date.day = (uint8_t)(ATOI(temp));

	memset(temp, 0, sizeof(temp));
	memcpy(temp, req->next_line + 11, 2);
	dt.time.hour = (uint8_t)(ATOI(temp));

	memset(temp, 0, sizeof(temp));
	memcpy(temp, req->next_line + 14, 2);
	dt.time.minute = (uint8_t)(ATOI(temp));

	dt.time.second = 0;
	dt.time.hundredths = 0;

	// contvert time date to tick
	setSysTick(dateTime2Tick(&dt));

	sendHeaderOK(req->conn);
	http_send_data (req->conn, "\r\n", 2);

}


void ApiTestRes (HTTP_Request* req, void* param)
{
//	while ((4-strlen(testFile)%4)>0)
//		testFile[strlen(testFile)] = 1;
//	char* test = "1234567890123456";
//	uint32_t res = HAL_CRC_Calculate(&hcrc, (uint32_t*)test, strlen(test)/4);
//
//	sprintf((char*)req->content, "%lu", res);
//	char contentLenStr[11] = {'\0'};
//	sprintf(contentLenStr, "%u", strlen((char*)req->content));
//	size_t contentLenStrSize = strlen(contentLenStr);

//	http_send_data (req->conn, http_res_HTTPOK.content, http_res_HTTPOK.length);
//	http_send_data (req->conn, http_res_ServerName.content, http_res_ServerName.length);
//	http_send_data (req->conn, http_res_ContentLength.content, http_res_ContentLength.length);
//	http_send_data (req->conn, contentLenStr, contentLenStrSize);
//	http_send_data (req->conn, "\r\n", 2);
//	http_send_data (req->conn, http_res_ConnectionType.content, http_res_ConnectionType.length);
//	http_send_data (req->conn, http_res_ContentDATA.content, http_res_ContentDATA.length);
//	http_send_data (req->conn, "\r\n", 2);
//	http_send_data (req->conn, (char*)req->content, strlen((char*)req->content));
}

// API entries
const WS_Api apiList [] = {
		{ "/", SendHomePage, (void*)&ui_bundle_html},
		{ "/favicon.ico", SendUIFile, (void*)&ui_favicon_ico},
//		{ "/styles.css", SendUIFile,(void*) &ui_styles_css},
//		{ "/js/config.js", SendUIFile, (void*)&ui_js_config_js},
//		{ "/js/program.js", SendUIFile,(void*) &ui_js_program_js},
//		{ "/js/system.js", SendUIFile, (void*)&ui_js_system_js},
//		{ "/js/utils.js", SendUIFile, (void*)&ui_js_utils_js},
//		{ "/lib/css/bootstrap.min.css", SendUIFile, (void*)&ui_lib_css_bootstrap_min_css},
//		{ "/lib/js/bootstrap.bundle.min.js", SendUIFile, (void*)&ui_lib_js_bootstrap_bundle_min_js},
//		{ "/lib/js/masonry.pkgd.min.js", SendUIFile, (void*)&ui_lib_js_masonry_pkgd_min_js},
		{ "/api/config", ApiConfigRes, NULL},
		{ "/api/login", ApiLoginRes, NULL},
		{ "/api/logout", ApiLogoutRes, NULL},
		{ "/api/now", ApiNowRes, NULL},
		{ "/api/time", ApiTimeRes, NULL},
		{ "/api/password", ApiPasswordRes, NULL},
		{ "/api/system/update", ApiSysUpdateRes, NULL},
		{ "/api/system/reboot", ApiSysRebootRes, NULL},
//		{ "/", SendHomePage, NULL},
//		{ "/", SendHomePage, NULL},
//		{ "/", SendHomePage, NULL},
//		{ "/", SendHomePage, NULL},
		{ "/test", ApiTestRes, NULL},
		{ NULL, NULL, NULL},
};
