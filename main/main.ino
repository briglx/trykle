#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include ".\conf.h"
#include ".\function.h"

//uint8_t LED_PIN = D5;

WiFiServer server(80);

WiFiClient espClient;
PubSubClient client(espClient);
int value = 0;

void setup_wifi() {

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

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
     turnLightOn();
    // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is active low on the ESP-01)
  } else {
      turnLightOff();
      // Turn the LED off by making the voltage HIGH
  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect("ESP8266", mqttUser, mqttPassword )) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  
  // Ensure light is turned off
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  turnLightOff();

  setup_wifi();
  
  // Start the server
  server.begin();
  Serial.println("Server started");
 
 //start MQTT client
  client.setServer(mqtt_server, mqttPort);
  client.setCallback(callback);
  
}




void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  // Wait until the client sends some data
  Serial.println("new client");
  while(!client.available()){
    delay(1);
  }

  // Read the first line of the request
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();

  // Match the request
 
  int value = LOW;
  char* msg;
  if (request.indexOf("/LED=ON") != -1)  {
    // digitalWrite(LED_BUILTIN, LOW);
    // digitalWrite(LED_PIN, HIGH);
    turnLightOn();
    value = HIGH;
    msg = "High";
  }
  else if (request.indexOf("/LED=OFF") != -1)  {
    // digitalWrite(LED_BUILTIN, HIGH);
    // digitalWrite(LED_PIN, LOW);
    turnLightOff();
    value = LOW;
    msg = "Low";
  }
  else if (request.indexOf("/BLINK") != -1)  {
    blinkLight(10,500);
   value = LOW;
    msg = "Low";
  }
  else {
    msg = "Unknown";
  }

  // Return the response
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); //  do not forget this one
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
 
  client.print("Led pin is now: ");
  client.print(msg);
//  if(value == HIGH) {
//    client.print("On");
//  } else {
//    client.print("Off");
////  }
  client.println("<br><br>");
  client.println("<a href=\"/LED=ON\"\"><button>Turn On </button></a>");
  client.println("<a href=\"/LED=OFF\"\"><button>Turn Off </button></a><br />");  
  client.println("</html>");

  delay(1);
  Serial.println("Client disonnected");
  Serial.println("");
 
}
