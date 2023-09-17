/*
 * System_Config.h
 *
 *  Created on: 24 Jan. 2022
 *      Author: Jacob Bai
 */

#ifndef SYSTEM_CONFIG_H_
#define SYSTEM_CONFIG_H_

#define SYSTEM_VERSION	0x00000104	// 0.1.2
#define MAGIC	0x50333432 

#define CONFIG_LOCATION_SIZE	32
#define CONFIG_DESCRIPTION_SIZE	32
#define CONFIG_DEVICENAME_SIZE	16
#define CONFIG_USERNAME_SIZE	16
#define CONFIG_PASSWORD_SIZE	32

// webserver
#define KEY_BUFF_MAX_SZ			16
#define KEY_EXPIRED_TIME		1800000
#endif /* SYSTEM_CONFIG_H_ */
