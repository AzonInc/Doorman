#include "TCSBus.h"

volatile uint32_t TCSBusReader::s_cmd = 0;
volatile uint8_t TCSBusReader::s_cmdLength = 0;
volatile bool TCSBusReader::s_cmdReady = false;

TCSBusReader::TCSBusReader(uint8_t readPin) : m_readPin(readPin), m_enabled(false)
{
}

void TCSBusReader::begin()
{
    pinMode(m_readPin, INPUT);
    enable();
}

void TCSBusReader::enable()
{
    if(!m_enabled)
    {
        m_enabled = true;
        attachInterrupt(digitalPinToInterrupt(m_readPin), analyzeCMD, CHANGE);
    }
}

void TCSBusReader::disable()
{
    if(m_enabled)
    {
        m_enabled = false;
        detachInterrupt(digitalPinToInterrupt(m_readPin));
    }
}

bool TCSBusReader::hasCommand()
{
    return s_cmdReady;
}

uint32_t TCSBusReader::read()
{
    if (!s_cmdReady)
    {
        return 0;
    }
    uint32_t tmp = s_cmd;
    s_cmdReady = false;
    return tmp;
}

void TCSBusReader::inject(uint32_t cmd)
{
    s_cmdReady = 1;
    s_cmd = cmd;
}

void IRAM_ATTR TCSBusReader::analyzeCMD()
{
    // this method is magic from https://github.com/atc1441/TCSintercomArduino
    // TODO extract these to members
    static uint32_t curCMD;
    static uint32_t usLast;
    static byte curCRC;
    static byte calCRC;
    static byte curLength;
    static byte cmdIntReady;
    static byte curPos;
    uint32_t usNow = micros();
    uint32_t timeInUS = usNow - usLast;
    usLast = usNow;
    byte curBit = 4;
    if (timeInUS >= 1000 && timeInUS <= 2999)
    {
        curBit = 0;
    }
    else if (timeInUS >= 3000 && timeInUS <= 4999)
    {
        curBit = 1;
    }
    else if (timeInUS >= 5000 && timeInUS <= 6999)
    {
        curBit = 2;
    }
    else if (timeInUS >= 7000 && timeInUS <= 24000)
    {
        curBit = 3;
        curPos = 0;
    }

    if (curPos == 0)
    {
        if (curBit == 2)
        {
            curPos++;
        }
        curCMD = 0;
        curCRC = 0;
        calCRC = 1;
        curLength = 0;
    }
    else if (curBit == 0 || curBit == 1)
    {
        if (curPos == 1)
        {
            curLength = curBit;
            curPos++;
        }
        else if (curPos >= 2 && curPos <= 17)
        {
            if (curBit)
            {
                bitSet(curCMD, (curLength ? 33 : 17) - curPos);
            }
            calCRC ^= curBit;
            curPos++;
        }
        else if (curPos == 18)
        {
            if (curLength)
            {
                if (curBit)
                {
                    bitSet(curCMD, 33 - curPos);
                }
                calCRC ^= curBit;
                curPos++;
            }
            else
            {
                curCRC = curBit;
                cmdIntReady = 1;
            }
        }
        else if (curPos >= 19 && curPos <= 33)
        {
            if (curBit)
            {
                bitSet(curCMD, 33 - curPos);
            }
            calCRC ^= curBit;
            curPos++;
        }
        else if (curPos == 34)
        {
            curCRC = curBit;
            cmdIntReady = 1;
        }
    }
    else
    {
        curPos = 0;
    }
    if (cmdIntReady)
    {
        cmdIntReady = 0;
        if (curCRC == calCRC)
        {
            s_cmdReady = 1;
            s_cmdLength = curLength; // todo this variable seems to be not used anywhere
            s_cmd = curCMD;
        }
        curCMD = 0;
        curPos = 0;
    }
}