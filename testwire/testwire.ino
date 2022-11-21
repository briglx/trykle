#include ".\conf.h"
#include <Wire.h>
#include "TimeLib.h"

#include <ESP8266WiFi.h>

#include <WiFiUdp.h>

byte errorResult;           // error code returned by I2C Wire.endTransmission()
byte i2c_addr;              // I2C address being pinged
byte lowerAddress = 0x08;   // I2C lowest valid address in range
byte upperAddress = 0x77;   // I2C highest valid address in range
byte numDevices;            // how many devices were located on I2C bus

#define PCF8523_ADDR 0x68 // i2c address for realtime clock


unsigned int localUdpPort = 4210; 
#define NTP_PACKET_SIZE 48
#define NTP_PORT 123

static byte packetBuffer[NTP_PACKET_SIZE];
WiFiUDP udp;


void setupWifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void setup() {

    
    Serial.begin(115200);

    setupWifi();
    
    Wire.begin();

    Serial.println("Starting.");
    // Serial.print("Detecting Real Time Clock.");
    
    
    // Wire.beginTransmission(PCF8523_ADDR);
    // Wire.write((uint8_t)0x00);

    // uint8_t status = Wire.endTransmission();
    // if (status==0) {
    //     Serial.print("Found it.");
    // } else {
    //     Serial.print("Didn't find it.");
    //     Serial.println(status);
    // }

    // delay(5000); // wait 5 seconds
    udp.begin(localUdpPort);
    Serial.printf("Now listening at IP %s, UDP port %d\n", WiFi.localIP().toString().c_str(), localUdpPort);

    

    Serial.println("Syncing RTC with NTP..");

    time_t cur_time = getNtpTime();
    setRTCTime(cur_time);
    



}

void scanForDevice(){

    Serial.print("Scanning I2C 7-bit address range 0x");
    if (lowerAddress < 0x10)                                // pad single digit addresses with a leading "0"
        Serial.print("0");
    Serial.print(lowerAddress, HEX);
    Serial.print(" to 0x");
    Serial.print(upperAddress, HEX);
    Serial.println(".");

    numDevices = 0;

    for (i2c_addr = lowerAddress; i2c_addr <= upperAddress; i2c_addr++ )     // loop through all valid I2C addresses
    {
        Wire.beginTransmission(i2c_addr);                     // initiate communication at current address
        errorResult = Wire.endTransmission();                 // if a device is present, it will send an ack and "0" will be returned from function

        if (errorResult == 0)                                 // "0" means a device at current address has acknowledged the serial communication
        {
            Serial.print("I2C device found at address 0x");

            if (i2c_addr < 0x10)                                // pad single digit addresses with a leading "0"
                Serial.print("0");

            Serial.println(i2c_addr, HEX);                      // display the address on the serial monitor when a device is found
            numDevices++;
        }
    }

    Serial.print("Scan complete.  Devices found: ");
    Serial.println(numDevices);
    Serial.println();

    delay(10000);   // wait 10 seconds and scan again to detect on-the-fly bus changes

}

void checkDirectory(){

    Serial.print("Detecting Real Time Clock.");

    Wire.beginTransmission(PCF8523_ADDR);
    Wire.write((uint8_t)0x00);

    uint8_t status = Wire.endTransmission();
    if (status==0) {
        Serial.print("Found it.");
    } else {
        Serial.print("Didn't find it.");
        Serial.println(status);
    }

    delay(10000); 
}


uint8_t t_sec;
uint8_t t_min;
uint8_t t_hour;
uint8_t t_day;
uint8_t t_month;
uint8_t t_year;


time_t getRtcTime() {

}

void showRtcTime(){

    Serial.print("RTC Time: ");

    Wire.beginTransmission(PCF8523_ADDR);
    Wire.write((uint8_t)0x03); // Specifies which register to start reading from
    Wire.endTransmission();

    tmElements_t tm;

    Wire.requestFrom(PCF8523_ADDR, (uint8_t)7); // 7

    t_sec = bcd2dec(Wire.read() & 0x7f);
    t_min = bcd2dec(Wire.read() & 0x7f);
    t_hour = bcd2dec(Wire.read() & 0x3f);
    t_day = bcd2dec(Wire.read() & 0x3f);
    Wire.read(); // Weekday
    t_month = bcd2dec(Wire.read() & 0x1f);
    t_year = bcd2dec(Wire.read());

    
    Serial.print('2');
    Serial.print('0');
    Serial.print(t_year); 
    Serial.print('-');
    if (t_month < 10) {
        Serial.print('0'); 
    }
    Serial.print(t_month); 
    Serial.print('-');
    if (t_day < 10) {
        Serial.print('0'); 
    }
    Serial.print( t_day); 
    Serial.print('T');
    if (t_hour < 10) {
        Serial.print('0'); 
    } 
    Serial.print(t_hour);
    Serial.print(':');
    if (t_min < 10) {
        Serial.print('0'); 
    } 
    Serial.print(t_min);
    Serial.print(':');
    if (t_sec < 10) {
        Serial.print('0'); 
    } 
    Serial.print(t_sec);
    Serial.println(' ');

}

void setRTCTime(time_t t){

    tmElements_t tm;
    breakTime(t, tm);
    Serial.print("Time is: ");
    Serial.print(tm.Year);


}

// Convert Decimal to Binary Coded Decimal (BCD)
uint8_t dec2bcd(uint8_t num)
{
	return ((num/10 * 16) + (num % 10));
}

// Convert Binary Coded Decimal (BCD) to Decimal
uint8_t bcd2dec(uint8_t num)
{
	return ((num/16 * 10) + (num % 16));
}

void showNtpTime(){

    Serial.print("NTP Time: ");

    time_t epoc = getNtpTime();

    Serial.print(epoc);
    Serial.print(" ");

    int hours = getHours(epoc);
    int minutes = getMinutes(epoc);
    int seconds = getSeconds(epoc);

    if (hours < 10) {
        Serial.print('0'); 
    }
    Serial.print(hours);
    Serial.print(":");
    if (minutes < 10) {
        Serial.print('0'); 
    }
    Serial.print(minutes);
    Serial.print(":");
    if (seconds < 10) {
        Serial.print('0'); 
    }
    Serial.println(seconds);

}


ulong getNtpTime() {

    // sendNtpPacket
	memset(packetBuffer, 0, NTP_PACKET_SIZE);   // set all bytes in the buffer to 0
    packetBuffer[0] = 0b11100011;		        // LI, Version, Mode
    packetBuffer[1] = 0;		                // Stratum, or type of clock
    packetBuffer[2] = 6;		                // Polling Interval
    packetBuffer[3] = 0xEC;                     // Peer Clock Precision
    // 8 bytes, 4 - 11, are for Root Delay and Dispersion and were set to 0 by memset
    packetBuffer[12]	= 49;  // four-byte reference ID identifying
    packetBuffer[13]	= 0x4E;
    packetBuffer[14]	= 49;
    packetBuffer[15]	= 52;

    // send the packet requesting a timestamp:
    udp.beginPacket("0.pool.ntp.org", NTP_PORT); //NTP requests are to port 123
    udp.write(packetBuffer, NTP_PACKET_SIZE);
    udp.endPacket();

    ulong timeout = millis()+5000;
    while(millis() < timeout) {
        
        if(udp.parsePacket()) {
            udp.read(packetBuffer, NTP_PACKET_SIZE);

            //the timestamp starts at byte 40, convert four bytes into a long integer
            ulong highWord = word(packetBuffer[40], packetBuffer[41]);
            ulong lowWord = word(packetBuffer[42], packetBuffer[43]);
            // this is NTP time (seconds since Jan 1 1900
            ulong secsSince1900 = highWord << 16 | lowWord;
            // Unix time starts on Jan 1 1970
            const ulong seventyYears = 2208988800UL;
            ulong epoch  = secsSince1900 - seventyYears + adjustTimeZone();

            udp.flush();
            // Ensure value is greater than 1/1/2020
            if(epoch >1577836800UL) return epoch ;
        }

        delay(10);

    }

    return 0;


}

inline int getSeconds(uint32_t UNIXTime) {
  return UNIXTime % 60;
}

inline int getMinutes(uint32_t UNIXTime) {
  return UNIXTime / 60 % 60;
}

inline int getHours(uint32_t UNIXTime) {
  return UNIXTime / 3600 % 24;
}

int adjustTimeZone(){

    return -7 * 3600;

}


void loop() {

    showNtpTime();
    showRtcTime();

    delay(10000); 

}