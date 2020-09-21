//functions.h
uint8_t LED_PIN = D5;

// Turn the LED on (Note that LOW is the voltage level
// but actually the LED is on; this is because
// it is active low on the ESP-01)
void turnLightOn(){
    digitalWrite(LED_BUILTIN, LOW);
    digitalWrite(LED_PIN, HIGH);
}
void turnLightOff(){
    digitalWrite(LED_BUILTIN, HIGH);
    digitalWrite(LED_PIN, LOW);
}

void blinkLight(int amt, int wait){
    for(int i = 0; i < amt; i++){
        turnLightOn();
        delay(wait);
        turnLightOff();
        delay(wait);
    }
}
