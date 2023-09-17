/*
 * wsBasic.c
 *
 *  Created on: Jan 28, 2022
 *      Author: Jacob Bai
 */

#include "stdint.h"
#include "string.h"
#include "stdbool.h"
/**
@brief	CONVERT CHAR INTO HEX
@return	HEX
This function converts HEX(0-F) to a character
*/
uint8_t C2H (uint8_t c)
{
	if (c >= '0' && c <= '9')
		return c - '0';
	if (c >= 'a' && c <= 'f')
		return 10 + c -'a';
	if (c >= 'A' && c <= 'F')
		return 10 + c -'A';

	return 0;
}
/**
@brief	CONVERT STRING INTO INTEGER
@return	a integer number
*/
uint32_t ATOI(char * str)
{
	uint32_t num = 0;

	while ((*str !=0) && (*str != 0x20)) // not include the space(0x020)
			num = num * 10 + C2H(*str++);
	return num;
}

/**
 * @brief Check strings and then execute callback function by each string.
 * @param src The information of URI
 * @param s1 The start string to be researched
 * @param s2 The end string to be researched
 * @param sub The string between s1 and s2
 * @return The length value atfer working
 */
void mid(char* src, char* s1, char* s2, char* sub)
{
	char* sub1;
	char* sub2;
	uint16_t n;

	sub1=strstr((char*)src,(char*)s1);
	sub1+=strlen((char*)s1);
	sub2=strstr((char*)sub1,(char*)s2);

	n=sub2-sub1;
	if (n > 0) {
		strncpy((char*)sub,(char*)sub1,n);
		sub[n]='\0';
	}
}

// compare two string within specific length
bool STRNCMP(const char* s1, const char* s2, uint8_t n)
{
	for(uint8_t i=0; i < n; i++) {
		if (s1[i] != s2[i])
			return false;
	}
	return true;
}
