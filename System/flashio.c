/*
 * fop.c
 *
 *  Created on: 29 Nov. 2021
 *      Author: Jacob Bai
 */
#include "stm32f4xx_hal.h"
#include "stdbool.h"

bool eraseSectors (uint32_t sector, uint32_t num)
{
	FLASH_EraseInitTypeDef flash;
	HAL_StatusTypeDef status = HAL_OK;

	status = HAL_FLASH_Unlock();
	if (status != HAL_OK)
		return false;

	// init flash type
	flash.TypeErase = FLASH_TYPEERASE_SECTORS;
	flash.Sector = sector;
	flash.VoltageRange = FLASH_VOLTAGE_RANGE_3;
	flash.NbSectors = num;

	// erase sectors
	uint32_t error = 0xFFFFFFFFU;
	status = HAL_FLASHEx_Erase(&flash, &error);
	if (status != HAL_OK) {
		HAL_FLASH_Lock();
		return false;
	}

	status = HAL_FLASH_Lock();
	if (status != HAL_OK)
		return false;

	return true;
}

bool unlockFlash(void)
{
	return HAL_FLASH_Unlock();
}
bool lockFlash(void)
{
	return HAL_FLASH_Lock();
}

// write buff data to specific address
void writeFlash(uint32_t dstFlashAddr, uint8_t* buf, uint16_t buflen)
{
	for (uint16_t i = 0; i < buflen; i++) {
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, dstFlashAddr, (uint64_t)buf[i]);
		dstFlashAddr++;
	}
}


