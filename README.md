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

# Website

Build your front-end code into production ready, minified assets
Upload assets to storage
Configure backend to use assets in storage

```bash
cd /path/to/webapp
 
#Tell the terminal what application to run
export FLASK_APP=main.py

#Tell the terminal what application to run for windows
set FLASK_APP=main.py

#Run the application
flask run
```


# References

- Example pubsub for mqtt on esp2688 https://github.com/knolleary/pubsubclient/blob/master/examples/mqtt_esp8266/mqtt_esp8266.ino
- Arduino CLI https://arduino.github.io/arduino-cli/latest/getting-started/