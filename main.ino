#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "<changeme>";
const char* password = "<changeme>";
uint8_t LED_PIN = D5;

WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  delay(10);
  
  // Ensure light is turned off
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(LED_PIN, LOW);


  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
 
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
 
  // Start the server
  server.begin();
  Serial.println("Server started");
 
  // Print the IP address
  Serial.print("Connected, IP address: ");
  Serial.print(WiFi.localIP());
  
}

void loop() {

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
    digitalWrite(LED_BUILTIN, LOW);
    digitalWrite(LED_PIN, HIGH);
    value = HIGH;
    msg = "High";
  }
  else if (request.indexOf("/LED=OFF") != -1)  {
    digitalWrite(LED_BUILTIN, HIGH);
    digitalWrite(LED_PIN, LOW);
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