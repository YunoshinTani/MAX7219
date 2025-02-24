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
    dotMatrix(PinName din_pin, PinName clk_pin, PinName cs_pin);

    void send(uint8_t reg, uint8_t data);

    void init();

    void clear();

    void drawDigit(uint8_t data[8]);

    void drawText(uint8_t data[][8], uint32_t wait_ms = 300);

    void drawText(const char *text, uint32_t wait_ms = 300);

    void slide();

private:
    static uint8_t FONT8x8[97][8];

    SPI _spi;
    DigitalOut _cs;

    void drawChar(char char_data);
};

#endif // MAX7219_H