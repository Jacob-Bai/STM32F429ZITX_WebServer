/*
 * memory.c
 *
 *  Created on: 1 Dec. 2021
 *      Author: Jacob Bai
 */

#include <string.h>
#include <FreeRTOS.h>

void* stm_calloc(size_t n, size_t sz) {
	void* p = pvPortMalloc(n*sz);
	memset(p, 0, n*sz);
	return p;
}
