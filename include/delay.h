#ifndef __TIME_H__
#define __TIME_H__

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/portmacro.h"

#include "esp_attr.h"
#include "esp_log.h"

unsigned long IRAM_ATTR hundred_ns();

unsigned long IRAM_ATTR micros();

void IRAM_ATTR delay_hundred_ns
        (uint64_t hundred_ns);

void IRAM_ATTR delay_us
        (uint64_t us);

#endif