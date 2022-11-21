#ifndef GPIO_H
#define GPIO_H

#define MCP23008_ADDR 0x20

class IOEXP
{
    // user-accessible "public" interface
    public:
    IOEXP();
    uint8_t digitalRead(uint8_t p);
    bool digitalWrite(uint8_t p, uint8_t d);
}


#endif // GPIO_H