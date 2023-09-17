/*
 * config.c
 *
 *  Created on: 12 Jan. 2022
 *      Author: Jacob Bai
 */
#include "stdbool.h"

#include "memory.h"
#include "config.h"
#include "flashio.h"
#include "usart.h"
#include "System_Config.h"

System_Config SysConfig;

// load system config from flash
void load_SystemConfig (void)
{
	SysConfig = *(System_Config*)CONFIG_ADDR; // read out config from flash
	if (SysConfig.magic == MAGIC) // check config is valid
		return;
	SysConfig.magic = MAGIC; // go to default settings
	SysConfig.device.hwVersion = 0;
	initString(SysConfig.device.location, "Jacob Desk");
	initString(SysConfig.device.description, "Just for Test");

	SysConfig.OS.version = SYSTEM_VERSION;
	SysConfig.OS.network.mode = 0; // 0: MAC 1: manual 2: dhcp
	SysConfig.OS.network.ip[0] = 192;
	SysConfig.OS.network.ip[1] = 168;
	SysConfig.OS.network.ip[2] = 3;
	SysConfig.OS.network.ip[3] = 23; // 192.168.1.101
	SysConfig.OS.network.netmask[0] = 255;
	SysConfig.OS.network.netmask[1] = 255;
	SysConfig.OS.network.netmask[2] = 255;
	SysConfig.OS.network.netmask[3] = 0; // 255.255.255.0
	SysConfig.OS.network.gateway[0] = 192;
	SysConfig.OS.network.gateway[1] = 168;
	SysConfig.OS.network.gateway[2] = 3;
	SysConfig.OS.network.gateway[3] = 1; // 192.168.1.1
	SysConfig.OS.network.hwMac[0] = 0x00;
	SysConfig.OS.network.hwMac[1] = 0xe1;
	SysConfig.OS.network.hwMac[2] = 0x88;
	SysConfig.OS.network.hwMac[3] = 0x01;
	SysConfig.OS.network.hwMac[4] = 0xcd;
	SysConfig.OS.network.hwMac[5] = 0x73;
	initString(SysConfig.OS.login.name, "admin");
	initString(SysConfig.OS.login.password, "admin");

	SysConfig.BACnet.autoId = false;
	SysConfig.BACnet.instanceNumber = 101;
	initString(SysConfig.BACnet.deviceName, "Jacob");

	SysConfig.BACnet.appLayer.apduTimeout = 10000;
	SysConfig.BACnet.appLayer.apduSegmentTimeout = 5000;
	SysConfig.BACnet.appLayer.numberOfApduRetries = 3;
	SysConfig.BACnet.appLayer.maxSegmentsAccepted = 8;
	SysConfig.BACnet.appLayer.proposedWindowSize = 3;

	SysConfig.BACnet.dataLink.type = 1; // 1 is dl_BIP, 0 is mstp, check bacinit.h for details
	SysConfig.BACnet.dataLink.ip.port = 0xBAC0;
	SysConfig.BACnet.dataLink.ip.networkNumber = 1;
	SysConfig.BACnet.dataLink.ip.foreignDevice.enabled = false;
	SysConfig.BACnet.dataLink.ip.foreignDevice.ip = 0;
	SysConfig.BACnet.dataLink.ip.foreignDevice.port = 0;
	SysConfig.BACnet.dataLink.ip.foreignDevice.timeToLive = 0;

	SysConfig.BACnet.dataLink.mstp.COMport = 0;
	SysConfig.BACnet.dataLink.mstp.baudrate = 115200;
	SysConfig.BACnet.dataLink.mstp.macAdd = 101;
	SysConfig.BACnet.dataLink.mstp.maxMaster = 127;
	SysConfig.BACnet.dataLink.mstp.maxInfoFrames = 1;

}
