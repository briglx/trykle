#include ".\conf.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

WiFiClient espClient;
PubSubClient client(espClient);
WiFiServer server(80);

uint8_t DATA_PIN = D1;
uint8_t CLOCK_PIN = D2;
uint8_t LATCH_PIN = D5;

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

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

    if ((char)payload[0] == '1') {
      digitalWrite(LATCH_PIN, LOW);
      shiftOut(DATA_PIN,CLOCK_PIN,LSBFIRST,payload[0]);
      digitalWrite(LATCH_PIN,HIGH);


      // delay(1000);
      // digitalWrite(LATCH_PIN,LOW);
      // shiftOut(DATA_PIN,CLOCK_PIN,LSBFIRST,0);
      // digitalWrite(LATCH_PIN, HIGH);
      // delay(1000);
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(DATA_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(LATCH_PIN, OUTPUT);
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
}