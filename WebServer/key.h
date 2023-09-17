/*
 * key.h
 *
 *  Created on: 10 Feb. 2022
 *      Author: Jacob Bai
 */

#ifndef KEY_H_
#define KEY_H_

#include "stdbool.h"

bool keyExpired(uint8_t key);

uint8_t newKey(void);

void rmKey(uint8_t key);

void initKeyList(void);

#endif /* KEY_H_ */
