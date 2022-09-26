#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <iostream>
#include <string>

//network creds
const char* ssid = "Abhimanyu";
const char* password = "abhisingh";

//cloudmqtt auth information 
const char* mqttServer = "driver.cloudmqtt.com";
const int   mqttPort = 18763;
const char* mqttUser = "tvsqdgpg";
const char* mqttPassword = "7hNc0P-Bx048";


//Peristaltic Pump Pins for PWM
const int LiquidDispenser = 16;
const int CandyDispenser = 17;
const int statusLED = 5;

int Liquid = 0;
int Candy = 0;
int DispenseTime = 0;
char LiquidStock[8] = "0.30";
char CandyStock[8] = "0.50";

//Wifi and MQTT instantiation
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

void subscribeTopics() {
  //subscribe to all the topics for the dispenser
  client.subscribe("chomate/liquid"); //subscribes to juice dispense channel (grams of sugar INTEGER)
  client.subscribe("chomate/candy"); //subscribes to skittle dispense (grams of sugar INTEGER)
  client.subscribe("chomate/request");  //channel app side stock request (DISPENSE/STOCK)
}

//topics that I publish to
//client.publish("app/response", "SUCCESS"); //I have recieved the dispensing message 
//client.publish("app/amount/candy", "0.15"); //publish percentage of skittles in stock 15%
//client.publish("app/amount/liquid", "0.49"); //publish percentage of skittles in stock 49%

int dispenseCarbs (int skittles, int juice) {
  //dispenses correct amount of juice and skittles 
  //assuming 1 second is 1 g of sugar for pump (400ml/min flrt), using 36g sugar per 8oz juice
  DispenseTime = juice*1000; //converts amount of juice to seconds that liquid needs to be dispensed
  while (DispenseTime > 0) { // while there are still more than 0g of juice request remaining
      digitalWrite(LiquidDispenser, HIGH); //high initializes the relays to the OFF position where no current is flowing
      delay(80);
      DispenseTime -= 1000;
      Liquid--;
  }
  digitalWrite(LiquidDispenser, LOW);

  //candy dispenser
  for(int i = 0; i < skittles; i++) {
    digitalWrite(CandyDispenser, HIGH);
    delay(400);
    digitalWrite(CandyDispenser, LOW);
    delay(400);
    Candy--;
  }

  if(Liquid != 0 || Candy != 0) {
    return -1;
  }

  return 0;
}

//MQTT reconnect function
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    digitalWrite(statusLED, LOW);
    // Attempt to connect
    if (client.connect("ChomateDispenser", mqttUser, mqttPassword)) {
      Serial.println("connected");
      digitalWrite(statusLED, HIGH);
      // Subscribe to all topics again, write a function for this
      subscribeTopics(); 
      //client.publish("app/response/connec", "Dispensing System has connected!");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" trying again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

//WiFi setup
void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  //WiFi.begin(ssid, password); //wifi with password
  WiFi.mode(WIFI_STA);
  delay(1000);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

//callback function for MQTT message parsing
void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  //Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    //Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // Feel free to add more if statements to control more GPIOs with MQTT

  // If a message is received on the topic esp32/output, you check if the message is either "on" or "off". 
  // Changes the output state according to the message
  if (String(topic) == "chomate/liquid") {

    if(messageTemp.toInt() != 0) { //message is a valid integer 
      Liquid = messageTemp.toInt();
      Serial.print("Liquid Request of: ");
      Serial.print(messageTemp);
      Serial.println(" grams.");
    } else {
      Serial.println("Invalid message");
    }
  } else if (String(topic) == "chomate/candy") {

    if(messageTemp.toInt() != 0) { //message is a valid integer 
      Candy = messageTemp.toInt();
      Serial.print("Candy Request of: ");
      Serial.print(messageTemp);
      Serial.println(" grams.");
    } else {
      Serial.println("Invalid message");
    }
  } else if (String(topic) == "chomate/request") {

    if (messageTemp == "DISPENSE") {
      Serial.println("Dispense request");
      //dispense
      if (dispenseCarbs(Candy, Liquid) == 0) { //successful dispensing 
        Serial.println("Dispense Successful");
        client.publish("app/response", "SUCCESS");
      } else {
        Serial.println("Dispense Failed");
        client.publish("app/response", "FAILED");
      }
    } else if (messageTemp == "STOCK") {
      Serial.println("Stock request");
      client.publish("app/amount/candy", CandyStock);
      client.publish("app/amount/liquid", LiquidStock);
    }
  }

}


void setup() {
  // initialize serial with baud rate of 115200
  Serial.begin(115200);

  pinMode(LiquidDispenser, OUTPUT);
  pinMode(CandyDispenser, OUTPUT);
  pinMode(statusLED, OUTPUT);
  digitalWrite(LiquidDispenser, LOW); //high initializes the relays to the OFF position where no current is flowing
  digitalWrite(CandyDispenser, LOW); //high initializes the relays to the OFF position where no current is flowing

  setup_wifi();
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);

}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

}