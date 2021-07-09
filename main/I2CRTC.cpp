#include "I2CRTC.h"
#include <ESP8266WiFi.h>
#include <Wire.h>

using namespace std;

I2CRTC::I2CRTC()
{
    Wire.begin();
}

// PUBLIC FUNCTIONS
time_t I2CRTC::get()
{
    tmElements_t tm;
    read(tm);
    // Serial.println(tm.Year);
    return(makeTime(tm));
}

void I2CRTC::set(time_t t)
{
    tmElements_t tm;
    breakTime(t, tm);
    write(tm); 
}

void I2CRTC::read( tmElements_t &tm)
{

    Wire.beginTransmission(PCF8523_ADDR);
    Wire.write((uint8_t)0x03); // Specifies which register to start reading from
    Wire.endTransmission();

    // Specify how many bytes to read
    Wire.requestFrom(PCF8523_ADDR, (uint8_t)7);

    tm.Second = bcd2dec(Wire.read() & 0x7f);
    tm.Minute = bcd2dec(Wire.read() & 0x7f);
    tm.Hour = bcd2dec(Wire.read() & 0x3f);
    tm.Day = bcd2dec(Wire.read() & 0x3f);
    tm.Wday = bcd2dec(Wire.read() & 0x07);
    tm.Month = bcd2dec(Wire.read() & 0x1f);
    tm.Year = y2kYearToTm((bcd2dec(Wire.read())));

}

void I2CRTC::write(tmElements_t &tm)
{

    Wire.beginTransmission(PCF8523_ADDR);
    Wire.write((uint8_t)0x03);
    Wire.write(dec2bcd(tm.Second) & 0x7f);
    Wire.write(dec2bcd(tm.Minute));
    Wire.write(dec2bcd(tm.Hour)); // sets 24 hour format
    Wire.write(dec2bcd(tm.Day));
    Wire.write(dec2bcd(tm.Wday));
    Wire.write(dec2bcd(tm.Month));
    Wire.write(dec2bcd(tmYearToY2k(tm.Year)));
    Wire.endTransmission();

}

// Convert Decimal to Binary Coded Decimal (BCD)
uint8_t I2CRTC::dec2bcd(uint8_t num)
{
    return ((num/10 * 16) + (num % 10));
}

// Convert Binary Coded Decimal (BCD) to Decimal
uint8_t I2CRTC::bcd2dec(uint8_t num)
{
    return ((num/16 * 10) + (num % 16));
}

I2CRTC RTC = I2CRTC();
