/*
 * wsBasic.h
 *
 *  Created on: Jan 28, 2022
 *      Author: Jacob Bai
 */

#ifndef WSBASIC_H_
#define WSBASIC_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#define H2C(i) ((i) < 10) ? ((i) + '0') : (((i) - 10) + 'A')

uint8_t C2H (uint8_t c);
uint32_t ATOI(char * str);
void mid(char* src, char* s1, char* s2, char* sub);
bool STRNCMP(const char* s1, const char* s2, uint8_t n);

#endif /* WSBASIC_H_ */
