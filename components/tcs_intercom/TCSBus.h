#pragma once
#include <Arduino.h>

#define TCS_MSG_START_MS 6 // a new message
#define TCS_ONE_BIT_MS 4   // a 1-bit is 4ms long
#define TCS_ZERO_BIT_MS 2  // a 0-bit is 2ms long

class TCSBusReader
{
public:
    /**
     * @brief A reader class to read messages from TCS bus systems.
     *
     * @param readPin the pin that is connected to the data line of the TCS bus.
     */
    TCSBusReader(uint8_t readPin);

    /**
     * @brief Must be called once during setup() phase
     */
    void begin();

    /**
     * @brief Enables the reading of the bus via interrupts. 
     * 
     */
    void enable();

    /**
     * @brief Disables the reading of the bus via interrupts
     * 
     */
    void disable();

    /**
     * @brief Returns true if a new command has been received from the bus.
     *
     * @return true if a new command has been received.
     * @return false if no command is available.
     */
    bool hasCommand();

    /**
     * @brief Reads the last message from the bus and also resets the #hasCommand() flag.
     *
     * @return uint32_t the last command from the bus
     */
    uint32_t read();

    /**
     * @brief Injects a command into the reader that can be read from it directly 
     * without being read from the bus.
     *
     * @param cmd the command to inject
     */
    void inject(uint32_t cmd);

private:
    /**
     * @brief The interrupt method that counts the time for each high
     * or low bit and connects it to one big command.
     */
    static void IRAM_ATTR analyzeCMD();

    static volatile uint32_t s_cmd;
    static volatile uint8_t s_cmdLength;
    static volatile bool s_cmdReady;
    uint8_t m_readPin;
    bool m_enabled;
};

class TCSBusWriter
{
public:
    /**
     * @brief Allows to write to the TCS Bus.
     *
     * @param writePin the pin on which the bus cable is connected.
     */
    TCSBusWriter(uint8_t writePin);

    /**
     * @brief Must be called in the setup phase.
     *
     */
    void begin();

    /**
     * @brief Returns true if it is currently writing to the bus
     *
     * @return true if we are writing to the bus
     * @return false if we are not writing to the bus
     */
    bool isWriting();

    /**
     * @brief Writes the data to the bus. Can be a short command with 16 bits or a
     * long commands with 32.
     * Example codes:
     *   0x1100 door opener if the handset is not lifted up (short)
     *   0x1180 door opener if the handset is lifted up (short)
     *   0x1B8F9A41 specific bell code for a door (long)
     * @param data the data to write to the bus
     */
    void write(uint32_t data);

private:
    uint8_t m_writePin;
    bool m_writing;
};