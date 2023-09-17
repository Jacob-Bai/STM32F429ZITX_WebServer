/*
 * key.c
 *
 *  Created on: 10 Feb. 2022
 *      Author: Jacob Bai
 */

#include "System_Config.h"
#include "timetick.h"
#include "stdbool.h"
#include "string.h"

uint64_t keyStartTime[KEY_BUFF_MAX_SZ];

bool keyExpired(uint8_t key)
{
	if (!keyStartTime[key] || ((getSysTick() - keyStartTime[key]) > KEY_EXPIRED_TIME)) {
		keyStartTime[key] = 0;
		return true;
	}

	return false;
}

void updateKey(uint8_t key)
{
	keyStartTime[key] = getSysTick();
}

uint8_t newKey(void)
{
	uint8_t key;
	uint8_t keyb = 0;
	for (key = 0; key < KEY_BUFF_MAX_SZ; key++) {
		if (keyExpired(key)) {
			keyStartTime[key] = getSysTick();
			return key;
		}
		if (keyStartTime[key] < keyStartTime[keyb])
			keyb = key; // find the least active key
	}

	keyStartTime[keyb] = getSysTick();
	return keyb;
}

void rmKey(uint8_t key)
{
	keyStartTime[key] = 0;
}

void initKeyList(void)
{
	memset(keyStartTime, 0, sizeof(keyStartTime));
}
