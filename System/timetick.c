/*
 * timeTick.c
 *
 *  Created on: 1 Dec. 2021
 *      Author: Jacob Bai
 */
#include <string.h>
#include <timetick.h>

static uint64_t SysTick = 0; // msec clock, 32bits only do 49 days

static uint16_t days[4][12] =
{
    {   0,  31,  60,  91, 121, 152, 182, 213, 244, 274, 305, 335},
    { 366, 397, 425, 456, 486, 517, 547, 578, 609, 639, 670, 700},
    { 731, 762, 790, 821, 851, 882, 912, 943, 974,1004,1035,1065},
    {1096,1127,1155,1186,1216,1247,1277,1308,1339,1369,1400,1430},
};

void updateSysTick(void) {
	SysTick++;
}

uint64_t getSysTick(void) {
	return SysTick;
}

void setSysTick(uint64_t t) {
	SysTick = t;
}

// 2000-01-01 00:00:00 Sat  This is the base time
uint64_t dateTime2Tick (Sys_DateTime* dt) {
	uint64_t tick = ((((( (uint64_t)dt->date.year/4*(365*4+1) + (uint64_t)days[dt->date.year%4][dt->date.month-1] + (uint64_t)dt->date.day-1 //days
			) * 24 + (uint64_t)dt->time.hour //hours
			) * 60 + (uint64_t)dt->time.minute //minutes
			) * 60 + (uint64_t)dt->time.second //seconds
			) * 100 + (uint64_t)dt->time.hundredths //hundredth
			) * 10; //milliseconds
	return tick;
}

void tick2DateTime (Sys_DateTime* dt, uint64_t epoch) {
	dt->time.hundredths = epoch%1000 / 10;
	epoch /= 1000;
	dt->time.second = epoch%60;
	epoch /= 60;
	dt->time.minute = epoch%60;
	epoch /= 60;
	dt->time.hour   = epoch%24;
	epoch /= 24;
	dt->date.dayOfWeek = (epoch+6)%7;

	dt->date.year = epoch/(365*4+1)*4;
	epoch %= 365*4+1;

	uint8_t y;
	for (y=3; y>0; y--)
	{
		if (epoch >= days[y][0])
			break;
	}

	uint8_t m;
	for (m=11; m>0; m--)
	{
		if (epoch >= days[y][m])
			break;
	}

	dt->date.year += y;
	dt->date.month = m + 1;
	dt->date.day  = epoch - days[y][m] + 1;
}

