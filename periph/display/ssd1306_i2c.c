#include "periph/display/ssd1306_i2c.h"

static const char *TAG = "SSD1306";

static i2c_port_t _i2c_port = I2C_NUM_MAX;
static uint8_t _address = 0;

static uint8_t *_buffer;
static uint8_t _height = 64;
static uint8_t _width = 128;
uint16_t _resoltion = 0;

void SSD1306_init_config();

void SSD1306_clear();

void SSD1306_send_buffer(
    uint8_t* buffer,
    uint8_t buffer_length);

esp_err_t SSD1306_init(
        i2c_port_t i2c_port,
        uint8_t address)
{
    return SSD1306_init_with_reset(i2c_port, address, GPIO_NUM_MAX);
}

esp_err_t SSD1306_init_with_reset(
        i2c_port_t i2c_port,
        uint8_t address,
        gpio_num_t gpio_reset)
{
    esp_err_t err;

    ESP_LOGI(TAG, "Initializing");

    _i2c_port = i2c_port;
    _address = address;

    _resoltion = _height * _width;
    _buffer = (uint8_t *) malloc(_resoltion);

    // TODO: Improve condition
    if (gpio_reset != GPIO_NUM_MAX) {
        gpio_set_direction(gpio_reset, GPIO_MODE_OUTPUT);
        gpio_set_level(gpio_reset, 1);
        delay_ms(1);
        gpio_set_level(gpio_reset, 0);
        delay_ms(10);
        gpio_set_level(gpio_reset, 1);
    }

    SSD1306_init_config();
    SSD1306_clear();
    SSD1306_display();

    return ESP_OK;
}

uint8_t SSD1306_initialized()
{
    return _address != 0;
}

void SSD1306_init_config()
{
    // Init sequence

    uint8_t config[] = {
        SSD1306_REG_DISPLAY_OFF,
        SSD1306_SETDISPLAYCLOCKDIV, 0x80, // suggested ratio
        0x80, // multiplex
        SSD1306_LCDHEIGHT - 1,
        SSD1306_SETDISPLAYOFFSET, 0x00,   // no offset
        SSD1306_SETSTARTLINE | 0x0,       // line #0
        SSD1306_CHARGEPUMP, 0x14,         // SSD1306_SWITCHCAPVCC
        SSD1306_MEMORYMODE, 0x00,         // act like ks0108
        SSD1306_SEGREMAP | 0x1,
        SSD1306_COMSCANDEC,
        SSD1306_SETCOMPINS, 0x12,
        SSD1306_SETCONTRAST, 0xCF,        // SSD1306_SWITCHCAPVCC
        SSD1306_SETPRECHARGE, 0xF1,       // SSD1306_SWITCHCAPVCC
        SSD1306_SETVCOMDETECT, 0x40,
        SSD1306_DISPLAYALLON_RESUME,
        SSD1306_NORMALDISPLAY,
        SSD1306_DEACTIVATE_SCROLL,
        SSD1306_DISPLAYON
    };

    SSD1306_send_buffer(config, sizeof(config) / sizeof(uint8_t));
}

void SSD1306_clear()
{
    memset(_buffer, 0x00, _resoltion / 8);
}

void SSD1306_set_text_6x8(
    FONT_6x8_t *font,
    char *text,
    uint8_t x_start,
    uint8_t y_start)
{
    uint8_t i = 0;

    while(text[i] != '\0') {
        uint8_t c = text[i];

        for (uint8_t x = 0; x < font->width; x++) {
            uint8_t x_pos = x_start + x;

            for (uint8_t y = 0; y < font->height; y++) {
                uint8_t y_pos = y_start + y;

                uint16_t buffer_offset = (_width * (y_pos / 8)) + x_pos;
                uint16_t character_offset = (font->width * (y / 8)) + x;

                _buffer[buffer_offset] |= ((font->chars[c][character_offset] >> y) & 1) << (y_pos % 8);
            }
        }

        x_start += font->width + 1;
        i++;
    }
}

void SSD1306_fill_pixel(
    uint8_t x,
    uint8_t y)
{
    uint16_t buffer_offset = (_width * (y / 8)) + x;
    _buffer[buffer_offset] |= 1 << (y % 8);
}

void SSD1306_fill_rect(
    uint8_t x_start,
    uint8_t y_start,
    uint8_t x_end,
    uint8_t y_end)
{
    uint8_t i = 0;

    if (x_start > x_end || y_start > y_end) {
        return;
    }

    for (uint8_t x = x_start; x <= x_end; x++) {
        for (uint8_t y = y_start; y <= y_end; y++) {
            SSD1306_fill_pixel(x, y);
        }
    }
}

void SSD1306_set_bitmap(
    uint8_t *bitmap,
    uint8_t width,
    uint8_t height,
    uint8_t x_start,
    uint8_t y_start)
{
    uint16_t i = 0;
    for (uint8_t h = 0; h < height; h++) {
        for (uint8_t w = 0; w < width; w++) {
            if (bitmap[i]) {
                SSD1306_fill_pixel(x_start + w, y_start + h);
            }

            i++;
        }
    }
}

void SSD1306_display() {
    ESP_ERROR_CHECK( I2C_master_write_slave_byte(_i2c_port, _address, 0x00, SSD1306_COLUMNADDR) );
    ESP_ERROR_CHECK( I2C_master_write_slave_byte(_i2c_port, _address, 0x00, 0) ); // Column start address (0 = reset)
    ESP_ERROR_CHECK( I2C_master_write_slave_byte(_i2c_port, _address, 0x00, SSD1306_LCDWIDTH - 1) ); // Column end address (127 = reset)
    ESP_ERROR_CHECK( I2C_master_write_slave_byte(_i2c_port, _address, 0x00, SSD1306_PAGEADDR) );
    ESP_ERROR_CHECK( I2C_master_write_slave_byte(_i2c_port, _address, 0x00, 0) ); // Page start address (0 = reset)
    ESP_ERROR_CHECK( I2C_master_write_slave_byte(_i2c_port, _address, 0x00, 7) ); // Page end address LCDHEIGHT 64

    uint8_t lines = _height / 8;

    for (uint8_t i = 0; i < lines; i++) {
        ESP_ERROR_CHECK( I2C_master_write_slave_buffer(_i2c_port, _address, 0x40, _buffer + (i * _width), _width) );
    }

    SSD1306_clear();
}

void SSD1306_send_buffer(
    uint8_t* buffer,
    uint8_t buffer_length)
{
    for (uint8_t i = 0; i < buffer_length; i++) {
        ESP_ERROR_CHECK( I2C_master_write_slave_byte(_i2c_port, _address, 0x00, buffer[i]) );
    }
}
