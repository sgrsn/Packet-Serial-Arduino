#include "PacketSerial.h"

#define DELAY_TIME_WRITING  0
#define DELAY_TIME_READING  0

PacketSerial::PacketSerial(int32_t *registar, Stream & stream) : _stream(&stream)
{
  _registar = registar;
}

void PacketSerial::writeData(int32_t data, uint8_t reg)
{
    uint8_t dataBytes[4] =
    {
        (data >> 24) & 0xFF,
        (data >> 16) & 0xFF,
        (data >>  8) & 0xFF,
        (data >>  0) & 0xFF
    };

    _stream -> write(HEAD_BYTE);
    delayMicroseconds(DELAY_TIME_WRITING);
    _stream -> write(reg);
    delayMicroseconds(DELAY_TIME_WRITING);
    uint8_t checksum = 0;
    for (uint8_t i = 0; i < 4; ++i)
    {
        if ((dataBytes[i] == ESCAPE_BYTE) || (dataBytes[i] == HEAD_BYTE))
        {
            _stream -> write(ESCAPE_BYTE);
            delayMicroseconds(DELAY_TIME_WRITING);
            checksum += ESCAPE_BYTE;
            _stream -> write(dataBytes[i] ^ ESCAPE_MASK);
            delayMicroseconds(DELAY_TIME_WRITING);
            checksum += dataBytes[i] ^ ESCAPE_MASK;
        }
        else
        {
            _stream -> write(dataBytes[i]);
            delayMicroseconds(DELAY_TIME_WRITING);
            checksum += dataBytes[i];
        }
    }

    // 末尾にチェックサムを追加で送信する
    _stream -> write(checksum);
    delayMicroseconds(DELAY_TIME_WRITING);
}
void PacketSerial::readData()
{
    uint8_t bytes[4] = {0,0,0,0};
    int8_t checksum = 0;
    delayMicroseconds(DELAY_TIME_READING);
    uint8_t data = _stream -> read();
    delayMicroseconds(DELAY_TIME_READING);

    if (data == HEAD_BYTE)
    {
        uint8_t reg = _stream -> read();
        for (int i = 0; i < 4; ++i)
        {
            uint8_t d = _stream -> read();
            delayMicroseconds(DELAY_TIME_READING);
            if (d == ESCAPE_BYTE)
            {
                uint8_t nextByte = _stream -> read();
                bytes[i] = nextByte ^ ESCAPE_MASK;
                checksum += (d + nextByte);
            }
            else
            {
                bytes[i] = d;
                checksum += d;
            }
        }
        int8_t checksum_recv = _stream -> read();
        delayMicroseconds(DELAY_TIME_READING);
        int32_t DATA = 0x00;
        for(int i = 0; i < 4; i++)
        {
            DATA |= (((int32_t)bytes[i]) << (24 - (i*8)));
        }

        if (checksum == checksum_recv)
        {
            *(_registar + reg) =  DATA;
        }
        else
        {
            // data error
        }
    }
}
