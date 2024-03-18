#include "TCSBus.h"

TCSBusWriter::TCSBusWriter(uint8_t writePin) : m_writePin(writePin), m_writing(false)
{
}

void TCSBusWriter::begin()
{
    pinMode(m_writePin, OUTPUT);
}

bool TCSBusWriter::isWriting()
{
    return m_writing;
}

void TCSBusWriter::write(uint32_t data)
{
    // this is magic from https://github.com/atc1441/TCSintercomArduino
    m_writing = true;
    int length = 16;
    byte checksm = 1;
    bool isLongMessage = false;
    if (data > 0xFFFF)
    {
        length = 32;
        isLongMessage = 1;
    }
    digitalWrite(m_writePin, HIGH);
    // delay(TCS_MSG_START_MS);
    delayMicroseconds(TCS_MSG_START_MS * 1000);
    digitalWrite(m_writePin, !digitalRead(m_writePin));
    // delay(isLongMessage ? TCS_ONE_BIT_MS : TCS_ZERO_BIT_MS);
    delayMicroseconds(isLongMessage ? TCS_ONE_BIT_MS * 1000 : TCS_ZERO_BIT_MS * 1000);
    int curBit = 0;
    for (byte i = length; i > 0; i--)
    {
        curBit = bitRead(data, i - 1);
        digitalWrite(m_writePin, !digitalRead(m_writePin));
        // delay(curBit ? TCS_ONE_BIT_MS : TCS_ZERO_BIT_MS);
        delayMicroseconds(curBit ? TCS_ONE_BIT_MS * 1000 : TCS_ZERO_BIT_MS * 1000);
        checksm ^= curBit;
    }
    digitalWrite(m_writePin, !digitalRead(m_writePin));
    // delay(checksm ? TCS_ONE_BIT_MS : TCS_ZERO_BIT_MS);
    delayMicroseconds(checksm ? TCS_ONE_BIT_MS * 1000 : TCS_ZERO_BIT_MS * 1000);
    digitalWrite(m_writePin, LOW);
    m_writing = false;
}