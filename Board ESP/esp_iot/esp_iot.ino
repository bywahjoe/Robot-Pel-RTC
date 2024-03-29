#include <WiFi.h>
#include <PubSubClient.h>

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
unsigned long nows, before;

//TELEGRAM
#include "CTBot.h"
#define chatID 1308157974
CTBot myBot;
String token = "1399258400:AAGEOPpu5IOG8UE4Wq8ZaHeJ22T_g7HGOwM";
//--------

#define triggerPin  18
#define echoPin     19

#define PUB_INTERVAL 30

unsigned lastPublish = 0;

//FLEX-IOT SEND TO WIDGET
unsigned int setKosong = 16;
bool indikator = 0;

// Update these with values suitable for your network.
const char* ssid = "robotku"; // ssid WIfi
const char* password = "robot1234"; // Password Wifi

//Backend credentials
const char* mqtt_server = "mqtt.flexiot.xl.co.id";
const char* clientId = "5426471190455434";  //MAC ADDRESS [Menu Device Operation]
String DEVICE_SERIAL = "5426471190455434" ; //MAC ADRESSS [Menu Device Operation]

/* [Menu Device Onboarding (json)]*/
const char* EVENT_TOPIC = "fclean/fclean/v2/common";
String SUB_TOPIC_STRING = "+/" + DEVICE_SERIAL + "/fclean/fclean/v2/sub";
const char* mqtt_username = "fclean-fclean-v2_6092";
const char* mqtt_password = "1610679895_6092";
/*-------------------------------*/

WiFiClient espClient;
PubSubClient client(espClient);
char msg[300];

void kirim(String pesan);
long jarakAir();

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  myBot.setTelegramToken(token); //TOKEN SETT
  kirim("START TELEGRAM");
  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

//receiving a message
void callback(char* topic, byte* payload, long length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.println("] ");

  String message = "";

  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  Serial.print("Payload");
  Serial.println(message);
  //
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(clientId, mqtt_username, mqtt_password)) {
      Serial.println("connected");
      //subscribe to the topic
      const char* SUB_TOPIC = SUB_TOPIC_STRING.c_str();
      client.subscribe(SUB_TOPIC);
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

  Serial.begin(9600);
  Serial2.begin(9600);

  //LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Connecting WiFi.");

  //Ultra
  pinMode(triggerPin, OUTPUT);
  pinMode(echoPin, INPUT);

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  //Timer LCD
  nows = millis();
  before = nows;
}

void loop() {
  if (Serial2.available()) {

    char recv = Serial2.read();

    if (recv == 'n') {
      //Serial.println("OKN");
      indikator = 1;
      kirim("F-CLEAN ON");
    } else if (recv == 'f') {
      //Serial.println("OKF");
      indikator = 0;
      kirim("F-CLEAN OFF");
    } else {}

    Serial2.end();
    Serial2.begin(9600);
  }

  if (WiFi.status() != WL_CONNECTED) {
    setup_wifi();
  }
  if (WiFi.status() == WL_CONNECTED && !client.connected()) {
    reconnect();
  }

  client.loop();

  if (millis() - lastPublish >= PUB_INTERVAL * 50UL) {
    readSensor();
    lastPublish = millis();
  }

  //LCD
  nows = millis();
  if (nows - before >= 2000L) {
    viewDisplay();
    before = nows;
  }

}
void publish_message(const char* message) {
  client.publish(EVENT_TOPIC, message);
}
void readSensor() {
  //sensor calculation
  int level = jarakAir();

  //int level = 10;
  String kirim;
  //{"eventName":"amd2020","status":"<none>","level":"<levelair>","v1":"<macAddressOfDevice>"}
  kirim += "{\"eventName\":\"amd2020\",\"status\":\"<none>\",\"level\":\"";
  kirim += level; kirim += "\",\"indikator\":\"";
  kirim += indikator;
  kirim += "\",\"v2\":\"" + DEVICE_SERIAL + "\"}";
  Serial.print("Mengirim : ");
  Serial.println(kirim);
  char*msg = (char*)kirim.c_str();
  publish_message(msg);

  delay(1500);
}
long jarakAir() {
  long duration, jarak;

  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);

  duration = pulseIn(echoPin, HIGH);

  jarak = (duration / 2) / 29.1;
  jarak = setKosong - jarak;
  jarak = constrain(jarak, 1, 30);

  //  Serial.println("jarak :");
  //  Serial.print(jarak);
  //  Serial.println(" cm");

  return jarak;
}
void kirim(String pesan) {
  myBot.sendMessage(chatID, pesan);
}
void viewDisplay() {
  String robotStatus = "OFF";
  int persentase=(jarakAir()*100/setKosong);
 
  if (indikator)robotStatus = "ON";

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("F-CLEAN  : ");
  lcd.print(robotStatus);
  lcd.setCursor(0, 1);
  lcd.print("WATER LVL: ");
  lcd.print(persentase);
  lcd.print("%");
//lcd.print(jarakAir());
}
