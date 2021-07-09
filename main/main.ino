#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include <WiFiUdp.h>

#include ".\conf.h"
#include ".\function.h"
#include "I2CRTC.h"

using namespace std;
//uint8_t LED_PIN = D5;

WiFiServer server(80);

#define NTP_SYNC_INTERVAL 86413L 	// Once a day
#define RTC_SYNC_INTERVAL	3607 // once an hour

int value = 0;

static const char ntpServerName[] = "us.pool.ntp.org";
const int timeZone = -7;
WiFiUDP Udp;
unsigned int localPort = 8888;  // local port to listen for UDP packets

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

  // Check first character of payload
  if ((char)payload[0] == '1') {
     turnLightOn();
  }
  else if ((char)payload[0] == '2') {
      turnLightOff();
  }
  else if ((char)payload[0] == '3') {
      blinkLight(5,1*1000);
  }
  else if ((char)payload[0] == '4') {
      blinkLight(5,5*1000);
  }
  else if ((char)payload[0] == '5') {
      blinkLight(5,10*1000);
  }
  else {
    Serial.println("Unknown MQTT value");
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

/*-------- NTP code ----------*/

const int NTP_PACKET_SIZE = 48; // NTP time is in the first 48 bytes of message
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming & outgoing packets

time_t getNtpTime()
{
  IPAddress ntpServerIP; // NTP server's ip address

  while (Udp.parsePacket() > 0) ; // discard any previously received packets
  // get a random server from the pool
  WiFi.hostByName(ntpServerName, ntpServerIP);
  sendNTPpacket(ntpServerIP);

  ulong timeout = millis()+5000;
  while(millis() < timeout) {
    if(Udp.parsePacket()) {
      Udp.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
      unsigned long secsSince1900;
      // convert four bytes starting at location 40 to a long integer
      secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)packetBuffer[43];
      return secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR;
    }
  }
  return 0; // return 0 if unable to get the time
}

// send an NTP request to the time server at the given address
void sendNTPpacket(IPAddress &address)
{
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12] = 49;
  packetBuffer[13] = 0x4E;
  packetBuffer[14] = 49;
  packetBuffer[15] = 52;
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}

void performNtpSync(){

  Serial.println("Syncing NTP..");
  ulong t = getNtpTime();
  Serial.println(t);

  if (t>0) {

    setTime(t);
    RTC.set(t);

  }

}

void setup() {
  Serial.begin(115200);
  
  // Ensure light is turned off
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  turnLightOff();

  setup_wifi();

  // Start UDP
  Serial.println("Starting UDP");
  Udp.begin(localPort);
  Serial.print("Local port: ");
  Serial.println(Udp.localPort());
  
  // Start the server
  server.begin();
  Serial.println("Server started");
 
 //start MQTT client
  client.setServer(mqtt_server, mqttPort);
  client.setCallback(callback);

  // // Set RTC as the time source
  setSyncInterval(RTC_SYNC_INTERVAL);
  setSyncProvider(RTC.get);

  
}




void loop() {

  time_t curr_time = now();

  if (!client.connected()) {
    reconnect();
  }
  client.loop();


  if (curr_time % NTP_SYNC_INTERVAL == 0){
    performNtpSync();
  }

  Serial.println(now());
  
  delay(20000);

//   // Check if a client has connected
//   WiFiClient client = server.available();
//   if (!client) {
//     return;
//   }

//   // Wait until the client sends some data
//   Serial.println("new client");
//   while(!client.available()){
//     delay(1);
//   }

//   // Read the first line of the request
//   String request = client.readStringUntil('\r');
//   Serial.println(request);
//   client.flush();

//   // Match the request
 
//   int value = LOW;
//   char* msg;
//   if (request.indexOf("/LED=ON") != -1)  {
//     // digitalWrite(LED_BUILTIN, LOW);
//     // digitalWrite(LED_PIN, HIGH);
//     turnLightOn();
//     value = HIGH;
//     msg = "High";
//   }
//   else if (request.indexOf("/LED=OFF") != -1)  {
//     // digitalWrite(LED_BUILTIN, HIGH);
//     // digitalWrite(LED_PIN, LOW);
//     turnLightOff();
//     value = LOW;
//     msg = "Low";
//   }
//   else if (request.indexOf("/BLINK=10") != -1)  {
//     blinkLight(5,10*1000);
//     value = LOW;
//     msg = "Low";
//   }
//   else if (request.indexOf("/BLINK=5") != -1)  {
//     blinkLight(5,5*1000);
//     value = LOW;
//     msg = "Low";
//   }
//   else if (request.indexOf("/BLINK=1") != -1)  {
//     blinkLight(5,1*1000);
//     value = LOW;
//     msg = "Low";
//   }
//   else {
//     msg = "Unknown";
//   }

//   // Return the response
//   client.println("HTTP/1.1 200 OK");
//   client.println("Content-Type: text/html");
//   client.println(""); //  do not forget this one
//   client.println("<!DOCTYPE HTML>");
//   client.println("<html>");
 
//   client.print("Led pin is now: ");
//   client.print(msg);
// //  if(value == HIGH) {
// //    client.print("On");
// //  } else {
// //    client.print("Off");
// ////  }
//   client.println("<br><br>");
//   client.println("<a href=\"/LED=ON\"\"><button>Turn On</button></a>");
//   client.println("<a href=\"/LED=OFF\"\"><button>Turn Off</button></a><br />");
//   client.println("<a href=\"/BLINK=1\"\"><button>Blink 1 Sec</button></a><br />"); 
//   client.println("<a href=\"/BLINK=5\"\"><button>Blink 5 Sec</button></a><br />"); 
//   client.println("<a href=\"/BLINK=10\"\"><button>Blink 10 Sec</button></a><br />");  
//   client.println("</html>");

//   delay(1);
//   Serial.println("Client disonnected");
//   Serial.println("");
 
}
