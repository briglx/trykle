#include "gpio.h"

#include <Wire.h>


IOEXP::IOEXP()
{
    Wire.begin();
}

// PUBLIC FUNCTIONS
uint16_t IOEXP::read(uint8_t reg)
{
    Wire.beginTransmission(MCP23008_ADDR);
    Wire.write((uint8_t)0x09); // Specifies which register to start reading from
    Wire.endTransmission();


    // Specify how many bytes to read
    Wire.requestFrom(MCP23008_ADDR, (uint8_t)7);
    uint8_t value = bcd2dec(Wire.read() & 0x7f);

}


IOEXP IO = IOEXP();