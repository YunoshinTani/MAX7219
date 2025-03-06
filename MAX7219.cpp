#include "MAX7219.hpp"

dotMatrix::dotMatrix(PinName din_pin, PinName clk_pin, PinName cs_pin, uint8_t numDevices) : _spi(din_pin, NC, clk_pin), _cs(cs_pin) {
    _numDevices = numDevices;
    // SPI設定
    _spi.format(16, 0); // 8ビットデータ, モード0
    _spi.frequency(1e7); // SPIクロック周波数 10MHz

    setting();
    send(REG_DISPLAY_TEST, 0x00, 0); // 通常動作モード
    clear();
}

// MAX7219にデータを送信
void dotMatrix::send(uint8_t reg, uint8_t data, uint8_t targetIndex) {
    _cs = 0; // チップセレクト有効
    if (targetIndex == 0) {
        for (uint8_t i=0; i<_numDevices; i++) {
            _spi.write((reg << 8) | (data));   // 目的のデバイスにデータ送信
        }
    } else {
        for (uint8_t i=1; i<=_numDevices; i++) {
            if (i == targetIndex) {
                _spi.write((reg << 8) | (data));   // 目的のデバイスにデータ送信
            } else {
                _spi.write(0x0000);  // 他のデバイスにはダミーデータを送信
            }
        }
    }
    _cs = 1; // チップセレクト無効
}

void dotMatrix::setting(uint8_t intensity, uint8_t scan_limit, uint8_t decode_mode, uint8_t shutdown, uint8_t targetIndex){
    // MAX7219の設定
    send(REG_SHUTDOWN, shutdown, targetIndex);    // 通常動作モード
    send(REG_DECODE_MODE, decode_mode, targetIndex); // デコードモード無効
    send(REG_INTENSITY, intensity, targetIndex);   // 輝度設定 (0x00〜0x0F)
    send(REG_SCAN_LIMIT, scan_limit, targetIndex);  // 8桁全表示
}

void dotMatrix::fill(uint8_t targetIndex) {
    for (uint8_t i=0; i<8; i++) {
        send(i+1, 0xFF, targetIndex);
    }
}
void dotMatrix::clear(uint8_t targetIndex) {
    for (uint8_t i=0; i<8; i++) {
        send(i+1, 0x00, targetIndex);
    }
}

void dotMatrix::test(uint8_t targetIndex)
{
    send(REG_DISPLAY_TEST, 0x01, targetIndex); // テストモード
}

void dotMatrix::drawDigit(uint8_t data[8], uint8_t targetIndex)
{
    for (int i=0; i<8; i++) {
        send(i+1, data[i], targetIndex);
    }
}

void dotMatrix::drawChar(char char_data, uint8_t targetIndex)
{
    for (int i=0; i<8; i++) {
        drawDigit(dotMatrix::EnRightFONT8x8[char_data - 32], targetIndex);
    }
}

void dotMatrix::drawText(uint8_t data[][8], uint32_t data_length, uint8_t targetIndex, uint32_t wait_ms)
{
    for (uint32_t i=0; i<data_length; i++) {
        drawDigit(data[i], targetIndex);
        ThisThread::sleep_for(chrono::milliseconds(wait_ms));
    }
}

void dotMatrix::drawText(const char *text, uint8_t targetIndex, uint32_t wait_ms)
{
    while (*text) {
        drawChar(*text++, targetIndex);
        ThisThread::sleep_for(chrono::milliseconds(wait_ms));
    }
}

void dotMatrix::slideLeftText(const char*text, uint8_t startIndex, uint8_t endIndex, uint32_t wait_ms) {
    uint8_t numEnableDevices = endIndex - startIndex + 1;
    uint32_t numWord = 0;
    uint8_t (*data)[8] = new uint8_t[1024][8];

    for (uint8_t i0=0; i0<numEnableDevices; i0++) {
        for (uint8_t i1=0; i1<8; i1++) {
            data[i0][i1] = 0x00;
        }
    }
    numWord += numEnableDevices;

    while (*text && numWord<(1024-(2*numEnableDevices))) {
        for (uint8_t i=0; i<8; i++) {
            data[numWord][i] = dotMatrix::EnLeftFONT8x8[*text - 32][i];
        }
        numWord++;
        text++;
    }

    for (uint8_t i0=0; i0<numEnableDevices; i0++) {
        for (uint8_t i1=0; i1<8; i1++) {
            data[numWord+i0][i1] = 0x00;
        }
    }
    numWord += numEnableDevices;

    for (uint32_t word=0; word<(numWord-numEnableDevices); word++) {
        for (uint8_t step=0; step<8; step++) {
            for (uint8_t digit=0; digit<8; digit++) {
                for (uint8_t index=0; index<numEnableDevices; index++) {
                    send(digit+1, (step==0) ? (data[word+index][digit] << step) & 0xFF : (((data[word+index][digit] << step) & 0xFF) | ((data[word+index+1][digit] >> (8-step)) & 0xFF)), startIndex + index);
                }
            }
            ThisThread::sleep_for(chrono::milliseconds(wait_ms));
        }
    }

    clear(0);

    delete[] data;
}

void dotMatrix::slideRightText(const char*text, uint8_t startIndex, uint8_t endIndex, uint32_t wait_ms) {
    uint8_t numEnableDevices = endIndex - startIndex + 1;
    uint32_t numWord = 0;
    uint8_t (*data)[8] = new uint8_t[1024][8];

    for (uint8_t i0=0; i0<numEnableDevices; i0++) {
        for (uint8_t i1=0; i1<8; i1++) {
            data[i0][i1] = 0x00;
        }
    }
    numWord += numEnableDevices;

    while (*text && numWord<(1024-(2*numEnableDevices))) {
        for (uint8_t i=0; i<8; i++) {
            data[numWord][i] = dotMatrix::EnRightFONT8x8[*text - 32][i];
        }
        numWord++;
        text++;
    }

    for (uint8_t i0=0; i0<numEnableDevices; i0++) {
        for (uint8_t i1=0; i1<8; i1++) {
            data[numWord+i0][i1] = 0x00;
        }
    }
    numWord += numEnableDevices;

    for (uint32_t word=numEnableDevices-1; word<(numWord-1); word++) {
        for (uint8_t step=0; step<8; step++) {
            for (uint8_t digit=0; digit<8; digit++) {
                for (int8_t index=numEnableDevices; index>=0; index--) {
                    send(digit+1, (step==0) ? (data[word-index][digit] >> step) & 0xFF : (((data[word-index][digit] >> step) & 0xFF) | ((data[word-index+1][digit] << (8-step)) & 0xFF)), startIndex + index);
                }
            }
            ThisThread::sleep_for(chrono::milliseconds(wait_ms));
        }
    }

    clear(0);

    delete[] data;
}

void dotMatrix::slideUpText(const char *text, uint8_t startIndex, uint8_t endIndex, uint32_t wait_ms) {
    uint8_t numEnableDevices = endIndex - startIndex + 1;
    uint8_t (*data)[8] = new uint8_t[1024][8]; // ヒープに確保
    uint32_t numWord = 1;

    for (uint8_t i=0; i<8; i++) {
        data[0][i] = 0x00;
    }

    while (*text && numWord<(1024-2)) {
        for (uint8_t i=0; i<8; i++) {
            data[numWord][i] = dotMatrix::EnUpFONT8x8[*text - 32][i];
        }
        numWord++;
        text++;
    }

    for (uint8_t i=0; i<8; i++) {
        data[numWord][i] = 0x00;
    }

    for (uint32_t word=0; word<(numWord); word++) {
        for (uint8_t step=0; step<8; step++) {
            for (uint8_t digit=0; digit<8; digit++) {
                for (uint8_t index=0; index<numEnableDevices; index++) {
                    send(digit+1, data[(word+digit < 8) ? word : word+1][(word+digit < 8) ? step+digit : step-(8-digit)], startIndex + index);
                }
            }
            ThisThread::sleep_for(chrono::milliseconds(wait_ms));
        }
    }

    clear(0);

    delete[] data;
}