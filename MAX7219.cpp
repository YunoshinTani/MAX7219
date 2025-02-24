#include "MAX7219.hpp"

dotMatrix::dotMatrix(PinName din_pin, PinName clk_pin, PinName cs_pin) : _spi(din_pin, NC, clk_pin), _cs(cs_pin)
{
    init();
    ThisThread::sleep_for(100ms);

    clear();
    ThisThread::sleep_for(100ms);
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

// MAX7219にデータを送信
void dotMatrix::send(uint8_t reg, uint8_t data)
{
    _cs = 0; // チップセレクト有効
    _spi.write((reg << 8) | (data));   // レジスタアドレス送信
    _cs = 1; // チップセレクト無効
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

void dotMatrix::drawChar(char char_data)
{
    for (int i=0; i<8; i++) {
        drawDigit(dotMatrix::FONT8x8[char_data - 32]);
    }
}

void dotMatrix::drawText(uint8_t data[][8], uint32_t data_length, uint32_t wait_ms)
{
    for (uint32_t i=0; i<data_length; i++) {
        drawDigit(data[i]);
        ThisThread::sleep_for(chrono::milliseconds(wait_ms));
    }
}

void dotMatrix::drawText(const char *text, uint32_t wait_ms)
{
    while (*text) {
        drawChar(*text++);
        ThisThread::sleep_for(chrono::milliseconds(wait_ms));
    }
}

void dotMatrix::slideText(const char *text, uint32_t wait_ms)
{
    uint8_t (*data)[8] = new uint8_t[1024][8]; // ヒープに確保
    uint32_t word = 1;

    for (uint8_t i=0; i<8; i++) {
        data[word-1][i] = 0x00;
    }

    while (*text && word<(1024-2)) {
        for (uint8_t i=0; i<8; i++) {
            data[word][i] = dotMatrix::FONT8x8[*text - 32][i];
        }
        word++;
        *text++;
    }

    for (uint8_t i=0; i<8; i++) {
        data[word][i] = 0x00;
    }

    for (uint32_t i=0; i<word; i++) {
        for (uint8_t j=0; j<8; j++) {
            printf("i : %ld   j : %d\n", i, j);
            send(1, data[(j+0 < 8) ? i : i+1][(j+0 < 8) ? j+0 : j-8]);
            send(2, data[(j+1 < 8) ? i : i+1][(j+1 < 8) ? j+1 : j-7]);
            send(3, data[(j+2 < 8) ? i : i+1][(j+2 < 8) ? j+2 : j-6]);
            send(4, data[(j+3 < 8) ? i : i+1][(j+3 < 8) ? j+3 : j-5]);
            send(5, data[(j+4 < 8) ? i : i+1][(j+4 < 8) ? j+4 : j-4]);
            send(6, data[(j+5 < 8) ? i : i+1][(j+5 < 8) ? j+5 : j-3]);
            send(7, data[(j+6 < 8) ? i : i+1][(j+6 < 8) ? j+6 : j-2]);
            send(8, data[(j+7 < 8) ? i : i+1][(j+7 < 8) ? j+7 : j-1]);
            ThisThread::sleep_for(chrono::milliseconds(wait_ms));
        }
    }

    delete[] data;
}