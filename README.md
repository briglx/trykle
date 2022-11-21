# trykle
Water management controler

Create a local `conf.h` based off of `conf-example.h`

# Setup

Install PubSub Client

On arduino ide, goto tools -> Library
Search for pubsub
Installed v2.8

## Configure MQTT Broker

Create a user and ACL for the user

 Type | Pattern | Read Write 
 -----|---------|------------
 Topic  | user - #  | true /true 

## Docker image

```bash
docker build --pull --rm -f "dockerfile" -t trykle:latest "."
docker run -it --rm trykle:latest
#  docker run -it --privileged -v /dev:/dev --rm trykle:latest
#  docker run -it --privileged -v /dev/bus/usb:/dev/bus/usb --rm trykle:latest

docker run -it --privileged --device="class/DCDE6AF9-6610-4285-828F-CAAF78C424CC" --rm trykle:latest
```

## Arduino CLI

```bash
arduino-cli core install esp8266:esp8266
arduino-cli core list
arduino-cli compile --fqbn arduino:esp8266:esp8266 MyFirstSketch
```

# OpenSprinkler Analysis

Main loop code
- do_setup()
    Clear watchdog flag
    os.begin()
    os.optinos_setup()
    pd.init9) prograData init
    Real time clock sync
    Set last startup time
    Start network
    Sync ntp 
    mqtt init
    reset all stations


- do_loop()
    check flow
    Check ethernet packets
    Check State (AP mode or not)
        if inital. Set ap mode. start_server_ap
        if connecting. start_server_client, or timeout
        if connected. wifi_server->handleClient() or reboot, or timeout
    ui_state_machine()
    os.mqtt.loop()
    Check rain delayed
    Check soil sensor
    Check program Switch status (Stop all, manual start)
    Check if need to run a program based on current time
        ensure station has non-zero time and is not disabled
        Enqueue a program if match
    Check time for any running programs to stop them
    os.apply_all_station_bits() Activate / deactivate valves
    Sync time
    check_network
    check_weather





# Real Time Clock

The Real time clock runs on the I2C protocol

The device address is `0x68`

There are 13 registers
- 00h
- ...
- 13h

Real Time Clock Ic^2 signal
- Power 3.3v
- GND
- SDA Data -> GPIO04 -> D2
- SCK Clock -> GPIO14 -> D5

To read, it looks like you have to write the device address and the regsiter address, then read


The data is coded in binary-coded decimal (BCD)

# MCP23008-e/p PDIP/SOIC

- SCL/SCK Pin 1
- SDA/SI Pin 2
- A2/SO (Hardware address pin) Pin 3
- A1 (Hardware address pin) Pin 4
- A0 (Hardware address pin) Pin 5
- (-)Reset Pin 6 (Need to Keep high)
- VSS (Ground) Pin 9
- GP0 Pin 10
- GP1 Pin 11
- GP2 Pin 12
- GP3 Pin 13
- GP4 Pin 14
- GP5 Pin 15
- GP6 Pin 16
- GP7 Pin 17
- VDD (Power) Pin 18

Device Address
Based on teh A0-1 pins

A0 | A1 | A2 | Decimal | Address
--------------------------------
0  |  0 | 0  | 0       | 0x20
0  |  0 | 1  | 1       | 0x21
0  |  1 | 0  | 2       | 0x22
0  |  1 | 1  | 3       | 0x23
1  |  0 | 0  | 4       | 0x24
1  |  0 | 1  | 5       | 0x25
1  |  1 | 0  | 6       | 0x26
1  |  1 | 1  | 7       | 0x27


Register Address

 Address | Access to
---------|---------- 
00h | IODIR
01h | IPOL
02h | GPINTEN
03h | DEFVAL
04h | INTCON
05h | IOCON
06h | GPPU
07h | INTF
08h | INTCAP (Read-only)
09h | GPIO
0Ah | OLA


Opensprinkler code:


# References

- Example pubsub for mqtt on esp2688 https://github.com/knolleary/pubsubclient/blob/master/examples/mqtt_esp8266/mqtt_esp8266.ino
- Arduino CLI https://arduino.github.io/arduino-cli/latest/getting-started/
- OpenSprinkler repos https://github.com/OpenSprinkler
- PCF8523 Real Time Clock (RTC) Data Sheet https://www.nxp.com/docs/en/data-sheet/PCF8523.pdf
- Adafruit-PCF8523 RTC Breakout PCB Eagle https://github.com/adafruit/Adafruit-PCF8523-RTC-Breakout-PCB
- NTP background https://www.circuitbasics.com/using-an-arduino-ethernet-shield-for-timekeeping/
- Arduino Strings https://majenko.co.uk/blog/evils-arduino-strings
- Arduino Time library docs https://github.com/PaulStoffregen/Time
- C++ Style Guide https://google.github.io/styleguide/cppguide.html
- MCP23008-e/p https://4donline.ihs.com/images/VipMasterIC/IC/MCHP/MCHP-S-A0008004111/MCHP-S-A0008004111-1.pdf?hkey=52A5661711E402568146F3353EA87419
- MCP23008 video demo https://www.youtube.com/watch?v=Sos2ghv80wU
- Adafruit MCP230xx library https://github.com/adafruit/Adafruit-MCP23017-Arduino-Library