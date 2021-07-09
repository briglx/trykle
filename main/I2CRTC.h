/*
 * I2CRTC.h - library for common I2C RTCs
 * This library is intended to be uses with Arduino Time.h library functions
 */


#ifndef I2CRTC_h
#define I2CRTC_h

#include <TimeLib.h>

#define PCF8523_ADDR 0x68


class I2CRTC
{
    // user-accessible "public" interface
    public:
    I2CRTC();
    static time_t get();
    static void set(time_t t);
    static void read(tmElements_t &tm);
    static void write(tmElements_t &tm);
    
    private:
    static uint8_t dec2bcd(uint8_t num);
    static uint8_t bcd2dec(uint8_t num);

};

extern I2CRTC RTC;

#endif
