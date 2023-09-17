/*
 * log.h
 *
 *  Created on: 13 Dec. 2021
 *      Author: Jacob Bai
 */

#ifndef LOG_H_
#define LOG_H_

#include <stdio.h>
#include <string.h>
#include <main.h>
#include "usart.h"

#define bufSz 100

char logData[bufSz];

#define send_logS(s) memset(logData, 0, bufSz); \
	sprintf(logData, s); \
	HAL_UART_Transmit_IT(&huart6, (uint8_t*)logData, strlen(logData))
#define send_logSD(s,d) memset(logData, 0, bufSz); \
	sprintf(logData, s, d); \
	HAL_UART_Transmit_IT(&huart6, (uint8_t*)logData, strlen(logData))

#endif /* LOG_H_ */
