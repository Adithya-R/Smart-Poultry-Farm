#include <ESP8266WiFi.h>
#include <PubSubClient.h>
 
//-------- Customise these values -----------
const char* ssid = "kuna";
const char* password = "12345678";
//String command;
#include "DHT.h"
#define led D3
#define led D4
int count=1;

#define DHTPIN D2    // what pin we're connected to
#define DHTTYPE DHT11   // define type of sensor DH 11
DHT dht (DHTPIN, DHTTYPE);
 
#define ORG "2qp4yw"
#define DEVICE_TYPE "echo"
#define DEVICE_ID "echo"
#define TOKEN "echoecho"
//-------- Customise the above values --------
 String command;
char server[] = ORG ".messaging.internetofthings.ibmcloud.com";
char subtopic[] = "iot-2/cmd/home/fmt/String";
char pubtopic[] = "iot-2/evt/Data/fmt/json";
char authMethod[] = "use-token-auth";
char token[] = TOKEN;
char clientId[] = "d:" ORG ":" DEVICE_TYPE ":" DEVICE_ID;
 
WiFiClient wifiClient;
void callback(char* subtopic, byte* payload, unsigned int payloadLength);
PubSubClient client(server,1883, callback,wifiClient);

void setup() {
 Serial.begin(115200);
   pinMode(D3,OUTPUT);
  pinMode(D4,OUTPUT);
  wifiConnect();
  mqttConnect();
 
 Serial.println();
 dht.begin();
 Serial.print("Connecting to ");
 Serial.print(ssid);
 WiFi.begin(ssid, password);
 while (WiFi.status() != WL_CONNECTED) {
 delay(500);
 Serial.print(".");
 } 
 Serial.println("");
 
 Serial.print("WiFi connected, IP address: ");
 Serial.println(WiFi.localIP());
}
 
void loop() {
 int sensorvalue=analogRead(A0);
 //Serial.println(sensorvalue);
float h = dht.readHumidity();
float t = dht.readTemperature();
if (isnan(h) || isnan(t))
{
Serial.println("Failed to read from DHT sensor!");
delay(1000);
return;
}
PublishData(t,h,sensorvalue);
 if (!client.loop()) {
    mqttConnect();
  }
delay(100);
}
void wifiConnect() {
  Serial.print("Connecting to "); Serial.print(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("nWiFi connected, IP address: "); Serial.println(WiFi.localIP());
}
void mqttConnect() {
  if (!client.connected()) {
    Serial.print("Reconnecting MQTT client to "); Serial.println(server);
    while (!client.connect(clientId, authMethod, token)) {
      Serial.print(".");
      delay(500);
    }
    initManagedDevice();
    Serial.println();
  }
}
void initManagedDevice() {
  if (client.subscribe(subtopic)) {
    Serial.println("subscribe to cmd OK");
  } else {
    Serial.println("subscribe to cmd FAILED");
  }
}
void automatic()
{
  int sensorvalue=analogRead(A0);
if(sensorvalue==1024)
  digitalWrite(D3,HIGH);
 else
   digitalWrite(D3,LOW);
float t = dht.readTemperature();
if(t<30)
 digitalWrite(D4,LOW);
 else
 digitalWrite(D4,HIGH);
 
} 
void callback(char* subtopic, byte* payload, unsigned int payloadLength) {
  Serial.print("callback invoked for ll: "); 
  Serial.println(subtopic);

  for (int i = 0; i < payloadLength; i++) {
    //Serial.println((char)payload[i]);
    command += (char)payload[i];
  }
Serial.println(command);

if(command=="auto" && count%2!=0 )
{
  automatic();
  count++;
 }
 else if(command=="auto" && count%2==0 )
  count++;

else
{
if(command == "lighton" ){
  digitalWrite(D3,HIGH);
  Serial.println("Light is Switched ON");
}
else if(command == "lightoff"){
  digitalWrite(D3,LOW);
  Serial.println("Light is Switched OFF");
}
}
if(command == "fanon"){
  digitalWrite(D4,HIGH);
  Serial.println("Fan is Switched ON");
}
else if(command == "fanoff"){
  digitalWrite(D4,LOW);
  Serial.println("Fan is Switched OFF");
}
command ="";
}

void PublishData(float temp, float humid,int light){
 if (!!!client.connected()) {
 Serial.print("Reconnecting client to ");
 Serial.println(server);
 while (!!!client.connect(clientId, authMethod, token)) {
 Serial.print(".");
 delay(500);
 }
 Serial.println();
 }
  String payload = "{\"d\":{\"temperature\":";
  payload += temp;
  payload+="," "\"humidity\":";
  payload += humid;
  payload+="," "\"light\":";
  payload += light;
  payload += "}}";
 Serial.print("Sending payload: ");
 Serial.println(payload);
  if (client.publish(pubtopic, (char*) payload.c_str())) {
 Serial.println("Publish ok");
 } else {
 Serial.println("Publish failed");
 }
}
