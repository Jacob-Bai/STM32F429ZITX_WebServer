/*
 * config.h
 *
 *  Created on: 12 Jan. 2022
 *      Author: Jacob Bai
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#include "System_Config.h"
#include <string.h>
#include "stdio.h"

#define initString(dst, string, ...) \
	memset(dst, 0, sizeof(dst)); \
	sprintf(dst, string, ## __VA_ARGS__)


typedef struct _system_configuration {
	uint32_t magic;

	struct {
		uint8_t hwVersion;
		char location[CONFIG_LOCATION_SIZE + 1];
		char description[CONFIG_DESCRIPTION_SIZE + 1];
	} device;

	struct {
		uint32_t version;
		struct {
			uint8_t mode;
			uint8_t ip[4];
			uint8_t netmask[4];
			uint8_t gateway[4];
			uint8_t hwMac[6];
		} network;

		struct {
			char name[CONFIG_USERNAME_SIZE + 1];
			char password[CONFIG_PASSWORD_SIZE + 1];
		} login;
	} OS;

	struct {
		uint8_t autoId;
		uint32_t instanceNumber;
		char deviceName[CONFIG_DEVICENAME_SIZE + 1];

		struct {
			uint16_t apduTimeout;
			uint16_t apduSegmentTimeout;
			uint8_t numberOfApduRetries;
			uint8_t maxSegmentsAccepted;
			uint8_t proposedWindowSize;
		} appLayer;

		struct {
			uint8_t type; // ip or mstp
			struct {
				uint16_t port;
				uint8_t networkNumber;
				struct {
					uint8_t enabled;
					uint32_t ip;
					uint16_t port;
					uint16_t timeToLive;
				} foreignDevice;
			} ip;

			struct {
				uint8_t COMport;
				uint32_t baudrate;
				uint8_t macAdd;
				uint8_t maxMaster;
				uint8_t maxInfoFrames;
			} mstp;
		} dataLink;
	} BACnet;

} System_Config;

extern System_Config SysConfig;

void load_SystemConfig (void);

#endif /* CONFIG_H_ */
