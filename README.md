# trykle
Water management controler

Create a local conf.h based off of conf-example.h

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


# References

- Example pubsub for mqtt on esp2688 https://github.com/knolleary/pubsubclient/blob/master/examples/mqtt_esp8266/mqtt_esp8266.ino