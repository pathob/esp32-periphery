#ifndef __FONT_H__
#define __FONT_H__

#include "esp_types.h"

#define CHARSET_LEN 128

typedef struct FONT_6x8_t {
    uint8_t width;                     // in pixels
    uint8_t height;                    // in pixels
    uint8_t chars[CHARSET_LEN][6];     // 6 pixels width and 1 byte height
} FONT_6x8_t;

typedef struct FONT_8x16_t {
    uint8_t width;                     // in pixels
    uint8_t height;                    // in pixels
    uint8_t chars[CHARSET_LEN][16];    // 8 pixels width and 2 bytes height
} FONT_8x16_t;

#endif
