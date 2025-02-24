#include "MAX7219.hpp"

dotMatrix::dotMatrix(PinName din_pin, PinName clk_pin, PinName cs_pin) : _spi(din_pin, NC, clk_pin), _cs(cs_pin)
{
    init();
    ThisThread::sleep_for(100ms);

    clear();
    ThisThread::sleep_for(100ms);
}

// MAX7219にデータを送信
void dotMatrix::send(uint8_t reg, uint8_t data)
{
    _cs = 0; // チップセレクト有効
    _spi.write((reg << 8) | (data));   // レジスタアドレス送信
    _cs = 1; // チップセレクト無効
}

void dotMatrix::init()
{
    // SPI設定
    _spi.format(16, 0); // 8ビットデータ, モード0
    _spi.frequency(1e7); // SPIクロック周波数 10MHz

    // MAX7219初期化
    send(REG_SHUTDOWN, 0x01);    // 通常動作モード
    send(REG_DECODE_MODE, 0x00); // デコードモード無効
    send(REG_INTENSITY, 0x0F);   // 輝度設定 (0x00〜0x0F)
    send(REG_SCAN_LIMIT, 0x07);  // 8桁全表示
    send(REG_DISPLAY_TEST, 0x00); // 通常動作モード
}

void dotMatrix::clear()
{
    for (int i=0; i<8; i++) {
        send(i+1, 0x00);
    }
}

void dotMatrix::drawDigit(uint8_t data[8])
{
    for (int i=0; i<8; i++) {
        send(i+1, data[i]);
    }
}

void dotMatrix::drawText(uint8_t data[][8], uint32_t wait_ms)
{
    for (int i=0; i<5; i++) {
        drawDigit(data[i]);
        ThisThread::sleep_for(chrono::milliseconds(wait_ms));
    }
}

void dotMatrix::drawChar(char char_data)
{
    for (int i=0; i<8; i++) {
        send(i+1, dotMatrix::FONT8x8[char_data - 32][i]);
    }
}

void dotMatrix::drawText(const char *text, uint32_t wait_ms)
{
    while (*text) {
        drawChar(*text++);
        ThisThread::sleep_for(chrono::milliseconds(wait_ms));
    }
}