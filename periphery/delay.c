#include "periphery/delay.h"

#define NOP() asm volatile ("nop")

portMUX_TYPE microsMux = portMUX_INITIALIZER_UNLOCKED;

static uint16_t cpu_freq_div_ten = CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ / 10;

unsigned long IRAM_ATTR hundred_nanos() {

    static unsigned long lccount = 0;
    static unsigned long overflow = 0;
    unsigned long ccount;
    portENTER_CRITICAL_ISR(&microsMux);
    __asm__ __volatile__ ( "rsr     %0, ccount" : "=a" (ccount) );
    if (ccount < lccount) {
        overflow += UINT32_MAX / cpu_freq_div_ten;
    }
    lccount = ccount;
    portEXIT_CRITICAL_ISR(&microsMux);
    return overflow + (ccount / cpu_freq_div_ten);
}

unsigned long IRAM_ATTR micros() {

    return hundred_nanos() / 10;
}

void IRAM_ATTR delay_hundred_ns
        (uint64_t hundred_ns) {

    if (hundred_ns > 10000) {
        uint64_t ms = hundred_ns / 10000;
        hundred_ns = hundred_ns % 10000;
        delay_ms(ms);
    }

    uint64_t m = hundred_nanos();
    if (hundred_ns) {
        uint64_t e = (m + hundred_ns);
        if (m > e) { //overflow
            while (hundred_nanos() > e) {
                NOP();
            }
        }
        while (hundred_nanos() < e) {
            NOP();
        }
    }
}

void IRAM_ATTR delay_us
        (uint64_t us) {

    delay_hundred_ns(us * 10);
}

void IRAM_ATTR delay_ms
        (uint64_t ms) {

    vTaskDelay(ms / portTICK_PERIOD_MS);
}