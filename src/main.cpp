#include <Arduino.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <Preferences.h>
#include <DHT.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

//Khai báo nguyên mẫu các hàm
void ConnectToWiFi();
void SetupProvision();
void ProvisionWithEdgeOrThings();
void Provision(const char*);
void sendProvisioningRequest(String, const char*);
void OnMessenger(const char*, byte*, unsigned int);
void SaveCredentialsAndNumberProvision();
void Checkn();

//Khai báo hằng số
#define ThingsBoardDemoHost "demo.thingsboard.io"
#define Port 1883
#define UsenameProvision "provision"
#define WifiDeviceID "SensorHumTem"
#define LedRed 12
#define LedGreen 14
#define Button 18
#define DHTPIN 4
const char* wifiSSID = "Tuan Truong";
const char* wifiPassword = "0362972868";
const char* Pass = "123";

//Khai báo cờ, biến
int n;
int EdgeIP;
int ReconnectTB;
float Tem;
float Hum;
String StatusProvisioning;
String Access;

//Khai báo đối tượng Wifi, PubSubClient, dht, lcd, preferences
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);
DHT dht(DHTPIN, DHT11);
LiquidCrystal_I2C lcd(0x27, 16, 2);
Preferences preferences;
/*------------------------------------*/

void setup() {
  Serial.begin(115200);
  pinMode(Button, INPUT_PULLUP);
  pinMode(LedRed, OUTPUT);
  pinMode(LedGreen, OUTPUT);
  
  Checkn();
  ConnectToWiFi();
  SetupProvision();
}

void loop() {
  if(StatusProvisioning == ("SUCCESS")){
    SaveCredentialsAndNumberProvision();
  }
  mqttClient.loop(); 
}


/*---------Định nghĩa hàm-----------*/

void ConnectToWiFi() {
  Serial.printf("Connecting to %s...\n", wifiSSID);
  //WiFi.begin(wifiSSID, wifiPassword);
  while (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(wifiSSID, wifiPassword);
    delay(1000);
    digitalWrite(LedRed, HIGH);
    delay(5000);
    digitalWrite(LedRed, LOW);
  }
  Serial.println("Connected to WiFi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  digitalWrite(LedGreen, HIGH);
  delay(5000);
  digitalWrite(LedGreen, LOW);
}

void SetupProvision() {
 /*Serial.println("Chuan bi cho qua trinh Provision");
 preferences.begin("Provision", false);
 n = preferences.getInt("n", 1);
 Serial.println("Gia tri cua n la: ");
 Serial.println(n);
 if (n == 1) {
  while (digitalRead(Button) == HIGH) {
    digitalWrite(LedGreen, HIGH);
    delay (1000);
    digitalWrite(LedGreen, LOW);
    delay (1000);
    digitalWrite(LedRed, HIGH);
    delay (1000);
    digitalWrite(LedRed, LOW);
    delay (1000);
    }
    EdgeOrThings();
 } else if (n == 2) {
     digitalWrite(LedGreen, HIGH);
     digitalWrite(LedRed, HIGH);
 }
 preferences.end();*/
 Serial.println("Ban muon thuc hien qua trinh Provision?");
 while (!Serial.available()) {
    // Đợi đến khi có dữ liệu từ Serial
  }
  char StartProvision = Serial.read();
  if (StartProvision == 'Y' || StartProvision == 'y') {
    Serial.println("Ban da dong y thuc hien qua trinh Provision");
    Serial.println("Nhap gia tri cua n:");
    while (!Serial.available()) {
      // Đợi đến khi có dữ liệu từ Serial
    }
    n = Serial.parseInt();
      // Kiểm tra kết thúc dòng, ấn enter là \n
    Serial.print("Gia tri cua n la: ");
    Serial.println(n);

    if (n == 1) {
      while (digitalRead(Button) == HIGH) {
        digitalWrite(LedGreen, HIGH);
        delay (1000);
        digitalWrite(LedGreen, LOW);
        delay (1000);
        digitalWrite(LedRed, HIGH);
        delay (1000);
        digitalWrite(LedRed, LOW);
        delay (1000);
      }
      ProvisionWithEdgeOrThings();
    } else if (n == 2) {
     digitalWrite(LedGreen, HIGH);
     digitalWrite(LedRed, HIGH);
     // Hàm thực hiện kết nối vs Things or Edge
    }
  } else {
    Serial.println("Ban phai thuc hien Provision ms su dung duoc san pham");
  }

}


void ProvisionWithEdgeOrThings(){
  /*if (EdgeIP == 1){
    digitalWrite(LedGreen, HIGH);
    Provision("192.168.100.6");
    digitalWrite(LedGreen, LOW);
  } else {
    digitalWrite(LedRed, HIGH);
    Provision(ThingsBoardDemoHost);
    digitalWrite(LedRed, LOW);
  }*/
 Serial.println("Nhap gia tri cua EdgeIP");
  while (!Serial.available()) {
    // Đợi đến khi có dữ liệu từ Serial
  }
  EdgeIP = Serial.parseInt();
  Serial.print("Gia tri cua EdgeIP la: ");
  Serial.println(EdgeIP);
  if (EdgeIP == 1){
    digitalWrite(LedGreen, HIGH);
    Provision("192.168.100.2");
    digitalWrite(LedGreen, LOW);
  } else {
    digitalWrite(LedRed, HIGH);
    Provision(ThingsBoardDemoHost);
    digitalWrite(LedRed, LOW);
  }
}

void Provision(const char* Host) {
  Serial.printf("Provisioning with: %s\n", Host);

  // Tạo đối tượng JSON cho Provisioning Request
  DynamicJsonDocument provisioningRequestJson(256);
  provisioningRequestJson["deviceName"] = "tuanday";
  provisioningRequestJson["provisionDeviceKey"] = "86glptvefiyfyi3s4ugk";
  provisioningRequestJson["provisionDeviceSecret"] = "khy9jstjx5tk8btza1nh";

  // Convert đối tượng JSON thành chuỗi JSON
  String provisioningRequestString;
  serializeJson(provisioningRequestJson, provisioningRequestString);

  // Gửi Provisioning Request đến ThingsBoard Demo
  sendProvisioningRequest(provisioningRequestString, Host);
  while(!(mqttClient.subscribe("/provision/response"))) {
    delay(1000);
    mqttClient.subscribe("/provision/response");
  }
  mqttClient.setCallback(OnMessenger);
}


void sendProvisioningRequest(String requestString, const char* Host) {
  Serial.printf("Connecting to %s\n", Host);
  
  // Set MQTT server and attempt to connect
  mqttClient.setServer(Host, Port);
  while (!(mqttClient.connected())) {
    mqttClient.connect(WifiDeviceID, UsenameProvision, Pass);
    Serial.println("Connecting...");
    delay(2000);
  }
  if (mqttClient.connected()) {
    Serial.printf("Connected to: %s\n",Host);
    // Publish provisioning request
    Serial.println("Sending provisioning request...");
    if (mqttClient.publish("/provision/request", requestString.c_str())) {
      Serial.println("Provisioning request sent successfully");
    } else {
      Serial.println("Failed to send provisioning request");
    }
  }
}

void OnMessenger(const char* topic, byte* payload, unsigned int length){    
 // Variable that will receive the message from the payload variable
  char MessageJson[length + 1];
 // Copy the payload bytes to the MessageJson variable
  strncpy(MessageJson, (char*)payload, length);
  Serial.println("Chuoi JSON nhan duoc la:");
  Serial.println(MessageJson);
 // Sets the string terminator to the null character '\0'
 // Identifying the end of the string.
  MessageJson[length] = '\0';
 // Create the Json data variable of size 200 bytes
  StaticJsonDocument<200> data;
 // Deserializing the message from the MessageJson variable
  DeserializationError error = deserializeJson (data, MessageJson);
 // If there is an error when deserializing, report it.
  if(error){  
    Serial.println("Error deserialize Json") ;
    return;
  } 
  StatusProvisioning = data["status"].as<String>();
  Access = data["credentialsValue"].as<String>();
}

void SaveCredentialsAndNumberProvision() {
  if(n == 1){
  Serial.println("Thong tin nhan duoc la:");
  Serial.println(StatusProvisioning);
  Serial.println(Access);
  n = 2;
  Serial.println("Bat dau luu n va Access Token vao Flash");
  preferences.begin("Provision", false);
  preferences.putInt("n", n);
  preferences.begin("Credentials", false);
  preferences.putString("Access", Access);
  preferences.end();
  Serial.println("Save Done");
  }
}

void Checkn() {
 Serial.println("Lay n tu Flash");
 preferences.begin("Provision", false);
 n = preferences.getInt("n", 1);
 preferences.end();
 Serial.println("Gia tri cua n la: ");
 Serial.println(n);
}
