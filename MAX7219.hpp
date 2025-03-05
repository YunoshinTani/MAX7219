#ifndef MAX7219_H
#define MAX7219_H

#include <stdint.h>
#include <mbed.h>

// MAX7219のレジスタアドレス
#define REG_NO_OP         0x00 // No operation
#define REG_DIGIT_0       0x01 // Digit 0
#define REG_DIGIT_1       0x02 // Digit 1
#define REG_DIGIT_2       0x03 // Digit 2
#define REG_DIGIT_3       0x04 // Digit 3
#define REG_DIGIT_4       0x05 // Digit 4
#define REG_DIGIT_5       0x06 // Digit 5
#define REG_DIGIT_6       0x07 // Digit 6
#define REG_DIGIT_7       0x08 // Digit 7
#define REG_DECODE_MODE   0x09 // Decode mode
#define REG_INTENSITY     0x0A // Intensity
#define REG_SCAN_LIMIT    0x0B // Scan limit
#define REG_SHUTDOWN      0x0C // Shutdown
#define REG_DISPLAY_TEST  0x0F // Display test

class dotMatrix
{
public:
    dotMatrix(PinName din_pin, PinName clk_pin, PinName cs_pin, uint8_t numDevices);

    void send(uint8_t reg, uint8_t data, uint8_t targetIndex = 0);

    void setting(uint8_t intensity = 0x0F, uint8_t scan_limit = 0x07, uint8_t decode_mode = 0x00, uint8_t shutdown = 0x01, uint8_t targetIndex = 0);

    void fill(uint8_t targetIndex = 0);

    void clear(uint8_t targetIndex = 0);

    void test(uint8_t targetIndex = 0);

    void drawDigit(uint8_t data[8], uint8_t targetIndex = 0);

    void drawText(uint8_t data[][8], uint32_t data_length, uint8_t targetIndex = 0, uint32_t wait_ms = 300);

    void drawText(const char *text, uint8_t targetIndex = 0, uint32_t wait_ms = 300);

    void slideText(const char *text, uint8_t startIndex = 1, uint8_t endIndex = 4, uint32_t wait_ms = 50);

    void slideTextUp(const char *text, uint8_t startIndex = 1, uint8_t endIndex = 4, uint32_t wait_ms = 50);

private:
    static uint8_t EnUpFONT8x8[97][8];
    static uint8_t EnFONT8x8[97][8];
    static uint8_t JpFONT8x8[168][8];

    SPI _spi;
    DigitalOut _cs;
    uint8_t _numDevices;

    void drawChar(char char_data, uint8_t targetIndex = 0);
};

#endif // MAX7219_H