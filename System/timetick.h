/*
 * timeTick.h
 *
 *  Created on: 1 Dec. 2021
 *      Author: Jacob Bai
 */

#ifndef TIME_H_
#define TIME_H_

#include <stdint.h>

typedef struct Sys_DateTime {
	struct {
		uint8_t year;
		uint8_t month; // 0-11
		uint8_t day; // 0-30
		uint8_t dayOfWeek; //Sun: 0 - Sat: 6
	} date;
	struct {
		uint8_t hour; //0-23
		uint8_t minute; //0-59
		uint8_t second; //0-59
		uint8_t hundredths; // 0-99
	} time;
} Sys_DateTime;


void updateSysTick(void);
uint64_t getSysTick(void);
void setSysTick(uint64_t t);
uint64_t dateTime2Tick (Sys_DateTime* dt);
void tick2DateTime (Sys_DateTime* dt, uint64_t epoch);

#endif /* TIME_H_ */
