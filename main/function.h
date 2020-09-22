//functions.h
uint8_t LED_PIN = D5;
WiFiClient espClient;
PubSubClient client(espClient);
// Turn the LED on (Note that LOW is the voltage level
// but actually the LED is on; this is because
// it is active low on the ESP-01)
void turnLightOn(){
    digitalWrite(LED_BUILTIN, LOW);
    digitalWrite(LED_PIN, HIGH);
    client.publish("outTopic", "Light is On");
}
void turnLightOff(){
    digitalWrite(LED_BUILTIN, HIGH);
    digitalWrite(LED_PIN, LOW);
    client.publish("outTopic", "Light is Off");
}

void blinkLight(int amt, int wait){
    
        char waitTime[10];
        client.publish("outTopic", "Light is Blinking Every");
        client.publish("outTopic", itoa(wait/1000, waitTime,10));
        client.publish("outTopic", " Seconds");
    
    for(int i = 0; i < amt; i++){
        turnLightOn();
        delay(wait);
        turnLightOff();
        delay(wait);
    }
}
