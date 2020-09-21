//functions.h
uint8_t LED_PIN = D5;
// if (int wait = 0 && request.indexOf("/LED=ON") != -1){
//   turn_light_on();
// }
// else if (int wait = 0 && request.indexOf("/LED=OFF") != -1){
//   turn_light_off();
// }
// else if (int wait > 0){
//   blink_light(int wait);
// }
// else{
//   msg = "Unknown";
// }
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
