#ifndef PacketSerial_H
#define PacketSerial_H
#include <Arduino.h>

class PacketSerial
{
  public:
  PacketSerial(int32_t *registar, Stream & stream = Serial);
  void writeData(int32_t data, uint8_t reg);
  void readData();

  private:
  Stream * const _stream;
  int32_t* _registar;
  const uint8_t HEAD_BYTE = 0x1D;
  const uint8_t ESCAPE_BYTE = 0x1E;
  const uint8_t ESCAPE_MASK = 0x1F;
};

void serialEvent();

#endif
