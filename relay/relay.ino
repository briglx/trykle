#include ".\conf.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <WiFiClient.h>
//Enables the ESP8266 to act as a server
#include <ESP8266WebServer.h>
#include <FS.h>
#include <Wire.h>

using namespace std;

ESP8266WebServer wifi_server(80);

WiFiClient espClient;
PubSubClient client(espClient);


#define PCF8523_ADDR 0x68 // i2c address for realtime clock

uint8_t DATA_PIN = D1;
uint8_t CLOCK_PIN = D2;
uint8_t LATCH_PIN = D5;
uint8_t OUTPUT_ENABLED = D6; // Low is disabled, high is enabled
uint8_t SRCLR_PIN = D3; // Low to clear, high to operate

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
      client.publish("outTopic", "Device Connected");
      // Enable output for Shift Register
      // digitalWrite(LATCH_PIN, LOW);
      // shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, 0);
      // digitalWrite(OUTPUT_ENABLED, LOW);
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
 
  // Indicate a message arrived with the built in led
  digitalWrite(LED_BUILTIN, LOW);

  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  Serial.print("Enable Output");
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  payload[length] = '\0'; // Make payload a string by NULL terminating it.
  int pwmVal = atoi((char *)payload);

  digitalWrite(OUTPUT_ENABLED, HIGH); // High Disable output, Low = Enabled
  digitalWrite(SRCLR_PIN, HIGH); // Low = Clear, High passes info through

  digitalWrite(LATCH_PIN, LOW);
  shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, pwmVal);
  digitalWrite(LATCH_PIN,HIGH);

  digitalWrite(OUTPUT_ENABLED, LOW); // High Disable output, Low = Enabled
  digitalWrite(SRCLR_PIN, LOW); // Low = Clear, High passes info through

  
  digitalWrite(LED_BUILTIN, HIGH);

}

void handle_home_page() {

  // body = "<html><head></head><body><h1>Hello</h1></body></html>"
  wifi_server.send(200, "text/html", "<html><head></head><body><h1>Hello</h1></body></html>");
}

void setup() {

  // Serial.print("Starting.");
  Serial.begin(115200);
    Serial.print("Starting..");
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(DATA_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(LATCH_PIN, OUTPUT);
  pinMode(OUTPUT_ENABLED, OUTPUT);
  pinMode(SRCLR_PIN, OUTPUT);

  digitalWrite(LED_BUILTIN, HIGH); // Active Low
  digitalWrite(CLOCK_PIN, LOW);
  digitalWrite(LATCH_PIN, LOW);
  digitalWrite(OUTPUT_ENABLED, HIGH); // High Disable output, Low = Enabled
  digitalWrite(SRCLR_PIN, LOW); // Low = Clear, High passes info through

  Serial.print("Setting up wifi..");
  //setup_wifi();
  Serial.print("Done.");


  // Start Web Server
  Serial.print("Starting Web Server..");
  wifi_server.on("/", handle_home_page);
  wifi_server.onNotFound([](){ wifi_server.send(404); });
  wifi_server.begin();
  Serial.print("The URL of the Web Server is: ");
  Serial.print("http://");
  Serial.println(WiFi.localIP());
  Serial.print("\n");
 
  // Start MQTT client
  Serial.print("Starting MQTT Server..");
  client.setServer(mqtt_server, mqttPort);
  client.setCallback(callback);
  Serial.print("Done.");

  // Once on and connected, enable write
  // digitalWrite(OUTPUT_ENABLED, LOW);
  // digitalWrite(SRCLR_PIN, LOW); // Clear Register


  Serial.print("Detecting Real Time Clock.");
  Wire.begin();
  Wire.beginTransmission(PCF8523_ADDR);
  Wire.write((uint8_t)0x00);

  uint8_t status = Wire.endTransmission();
  if (status==0) {
    Serial.print("Found it.");
  } else {
    Serial.print("Didn't find it.");
    Serial.println(status);
  }
  
}

void loop() {
  
  if (!client.connected()) {
    reconnect();
    
  }
  client.loop();
  wifi_server.handleClient();

}