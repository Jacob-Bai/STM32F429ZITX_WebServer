/*
 * fop.h
 *
 *  Created on: 29 Nov. 2021
 *      Author: Jacob Bai
 */

#ifndef FLASHIO_H_
#define FLASHIO_H_

#include "main.h"

#define IMG_START				0x20030000 // the flag of program start regconized by bootloader
#define IMG_PARTITION_SZ		0x00080000 // 512KB for each image

#define CURR_IMG_ADDR			0x08080000 // the current running image address
#define NEW_IMG_ADDR			0x08100000 // the update image address
#define BACK_IMG_ADDR			0x08180000 // the backup image address

#define CONFIG_ADDR				0x08008000 // where to save configuration

#define CURR_IMG_START_SECTOR	FLASH_SECTOR_8
#define CURR_IMG_NUM_SECTORS	4

#define NEW_IMG_START_SECTOR	FLASH_SECTOR_12
#define NEW_IMG_NUM_SECTORS		8

#define BACK_IMG_START_SECTOR	FLASH_SECTOR_20
#define BACK_IMG_NUM_SECTORS	4

#define CONFIG_START_SECTOR		FLASH_SECTOR_2
#define CONFIG_NUM_SECTORS		1

bool eraseSectors (uint32_t sector, uint32_t num);
void writeFlash(uint32_t dstFlashAddr, uint8_t* buf, uint16_t buflen);
bool unlockFlash(void);
bool lockFlash(void);

#endif /* FLASHIO_H_ */
