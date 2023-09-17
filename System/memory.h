/*
 * memory.h
 *
 *  Created on: 1 Dec. 2021
 *      Author: Jacob Bai
 */

#ifndef MEMORY_H_
#define MEMORY_H_

#include <FreeRTOS.h>

// Macro to use CCM (Core Coupled Memory) in STM32F4
#ifndef CCM_RAM
#define CCM_RAM __attribute__((section(".ccmram")))
#endif

#define stm_malloc(sz) pvPortMalloc(sz)
#define stm_free(p) vPortFree(p)

void* stm_calloc(size_t, size_t);

#endif /* MEMORY_H_ */
