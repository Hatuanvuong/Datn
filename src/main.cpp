#include <Arduino.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <PubSubClient.h>

//Khai báo nguyên mẫu các hàm
void OnMessenger(const char*, byte*, unsigned int);
//void receiveAndHandleProvisioningResponse();
void sendProvisioningRequest(String, const char*);
void provisioningWithEdge();
void provisioningWithDemo();
void receiveIP();
void connectToWiFi();
void ReconnectThingsBoardDemo();

const char* thingsBoardDemoHost = "demo.thingsboard.io";
u_int16_t Port = 1883;


const char* wifiSSID = "Tuan Truong";
const char* wifiPassword = "0362972868";
const char* usename = "provision";
const char* pass = "123";
const char* WifiDeviceIP ="DATN";
const char* EdgeIp1 = "192.168.100.15";

int App = 3;
char StartProvision ='a';
int n = 1;

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

String StatusProvisioning;
String CredentialsValueEdge;
//String Check = "SUCCESS";



void setup() {
  Serial.begin(115200);
  // Kết nối và kiểm tra kết nối WiFi 
  connectToWiFi();
  // Nhận địa chỉ IP từ App trên điện thoại
  receiveIP();
  mqttClient.setCallback(OnMessenger);
}

void loop() {
  //mqttClient.loop();
  if (((StatusProvisioning == "SUCCESS") || (StatusProvisioning == "FAILURE")) && (n == 1) )
  {
  ReconnectThingsBoardDemo();
  n = 2;
  }
  mqttClient.loop();
}


/*---------Định nghĩa hàm-----------*/

void connectToWiFi() {
  Serial.printf("Connecting to %s...\n", wifiSSID);
  WiFi.begin(wifiSSID, wifiPassword);
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.printf("\nConnected to %s\n", wifiSSID);
    //Serial.printf("LocalIP: %s\n", WiFi.localIP());
  } else {
    Serial.printf("\nFailed to connect to %s\n", wifiSSID);
  }
}

 
void receiveIP() {
  // Thực hiện logic để nhận địa chỉ IP từ App trên điện thoại
  // Ví dụ: có thể sử dụng Bluetooth, Wi-Fi Direct, hoặc giao thức tương tự
  // Kết quả được lưu vào biến edgeIPAddress
  // Dưới đây là giả định rằng đã nhận đc địa chỉ IP từ App trên điện thoại
  // edgeIPAddress = receiveIP();
  // Kiểm tra điều kiện để quyết định làm gì tiếp theo
  // Muốn thực hiện quá trình Provision nhập Y, không muốn nhập phím khác (đại diện cho nút nhấn, cho phép thực hiện Provision hay không)
  // Nếu App == 1 thực hiện Provision vs Edge, App != 0 thực hiện Provision với ThingsBoard Demo (đại diện cho thông tin có Edge hay không.
  // Nếu có Edge tức là có địa chỉ IP của Edge, nếu không có Edge thì là 1 cái j đấy khác địa chỉ IP)
  Serial.println("Ban muon thuc hien qua trinh Provision?");
  while (!Serial.available()) {
    // Đợi đến khi có dữ liệu từ Serial
  }

  char StartProvision = Serial.read();
  Serial.println("Ban da dong y thuc hien qua trinh Provision");
  
  if (StartProvision == 'Y' || StartProvision == 'y') {
    Serial.println("Nhap gia tri cua App:");
    while (!Serial.available()) {
      // Đợi đến khi có dữ liệu từ Serial
    }

    int App = Serial.parseInt();
      // Kiểm tra kết thúc dòng, ấn enter là \n
    Serial.print("Gia tri cua App la: ");
    Serial.println(App);

    if (App == 1) {
      provisioningWithEdge();
    } else if (App == 2) {
      provisioningWithDemo();
    }
  } else {
    Serial.println("Ban phai thuc hien Provision ms su dung duoc san pham");
  }
}


void provisioningWithEdge() {
  Serial.println("Provisioning with ThingsBoard Edge...");

  // Tạo đối tượng JSON cho Provisioning Request
  DynamicJsonDocument provisioningRequestJson(256);
  provisioningRequestJson["deviceName"] = "tuanday";
  provisioningRequestJson["provisionDeviceKey"] = "86glptvefiyfyi3s4ugk";
  provisioningRequestJson["provisionDeviceSecret"] = "khy9jstjx5tk8btza1nh";

  // Convert đối tượng JSON thành chuỗi JSON
  String provisioningRequestString;
  serializeJson(provisioningRequestJson, provisioningRequestString);

  // Gửi Provisioning Request đến ThingsBoard Edge
  sendProvisioningRequest(provisioningRequestString, EdgeIp1);
  mqttClient.subscribe("/provision/response");

  // Nhận và xử lý Respone từ ThingsBoard Edge
  //receiveAndHandleProvisioningResponse();
  /*if(StatusProvisioning == Check)
  { 
    Serial.println("Provisioning SUCCESS");
    if(mqttClient.unsubscribe("/provision/response")){
      Serial.println(1);
    } else{
      Serial.println(0);
    }
    mqttClient.disconnect();
    mqttClient.setServer(thingsBoardDemoHost, Port);
    delay(50);
    const char* Save = CredentialsValue.c_str();
    mqttClient.connect(WifiDeviceIP, Save, pass);
    if(mqttClient.connected() == false)
    {
    Serial.println("Connect ThingsBoard Demo FAILED");
    } else
    {
    Serial.println("Connect ThingsBoard Demo DONE");
    }
  } else
  {
    Serial.println("Provisioning FAILED");
  }*/
}


void provisioningWithDemo() {
  Serial.println("Provisioning with ThingsBoard Demo...");

  // Tạo đối tượng JSON cho Provisioning Request
  DynamicJsonDocument provisioningRequestJson(256);
  provisioningRequestJson["deviceName"] = "tuanday";
  provisioningRequestJson["provisionDeviceKey"] = "86glptvefiyfyi3s4ugk";
  provisioningRequestJson["provisionDeviceSecret"] = "khy9jstjx5tk8btza1nh";

  // Convert đối tượng JSON thành chuỗi JSON
  String provisioningRequestString;
  serializeJson(provisioningRequestJson, provisioningRequestString);

  // Gửi Provisioning Request đến ThingsBoard Demo
  sendProvisioningRequest(provisioningRequestString, thingsBoardDemoHost);
  mqttClient.subscribe("/provision/response");

  // Nhận và xử lý Respone từ ThingsBoard Demo
  //receiveAndHandleProvisioningResponse();
  /*if(StatusProvisioning == Check)
  { 
    Serial.println("Provisioning SUCCESS");
    if(mqttClient.unsubscribe("/provision/response")){
      Serial.println(1);
    } else{
      Serial.println(0);
    }
    mqttClient.disconnect();
    mqttClient.setServer(thingsBoardDemoHost, Port);
    delay(50);
    mqttClient.connect(WifiDeviceIP, CredentialsValue.c_str(), pass);
    if(mqttClient.connected() == false)
    {
    Serial.println("Connect ThingsBoard Demo FAILED");
    } else
    {
    Serial.println("Connect ThingsBoard Demo DONE");
    }
  } else
  {
    Serial.println("Provisioning FAILED");
  }*/
}


/*void sendProvisioningRequest(String requestString, const char* host) {
  Serial.printf("Connecting to MQTT broker %s...\n", host);
  mqttClient.setServer(host, Port);
  mqttClient.connect(WifiIP, usename, pass);
  Serial.println(mqttClient.connected());
    while (!mqttClient.connected())
    {
    mqttClient.connect(WifiIP);
    }
    if (mqttClient.connected()){
    Serial.println("Connecte to MQTT broker DONE");
    mqttClient.publish("/provision/request", requestString.c_str());
    } else{
    Serial.println("Connecte to MQTT broker FAILED"); 
    }
    }*/


void sendProvisioningRequest(String requestString, const char* host) {
  Serial.printf("Connecting to MQTT broker %s...\n", host);
  
  // Set MQTT server and attempt to connect
  mqttClient.setServer(host, Port);
  unsigned long startMillis = millis();
  const unsigned long timeoutMillis = 5000;  // Timeout after 5 seconds

  while (!mqttClient.connected() && millis() - startMillis < timeoutMillis) {
    if (mqttClient.connect(WifiDeviceIP, usename, pass)) {
      Serial.println("Connected to MQTT broker");
    } else {
      Serial.println("Failed to connect to MQTT broker");
      delay(1000);  // Wait for 1 second before retrying
    }
  }

  // Check if the MQTT client is connected
  if (mqttClient.connected()) {
    //Serial.println("Connected to MQTT broker");
    // Publish provisioning request
    Serial.println("Sending provisioning request...");
    if (mqttClient.publish("/provision/request", requestString.c_str())) {
      Serial.println("Provisioning request sent successfully");
    } else {
      Serial.println("Failed to send provisioning request");
    }

    // Disconnect MQTT client
    //mqttClient.disconnect();
    //Serial.println("Disconnected from MQTT broker");
  //} else {
   // Serial.println("Timed out waiting for MQTT connection");
  }
}


/*void receiveAndHandleProvisioningResponse() {
  //Serial.println("mqttClient.connected()");

  mqttClient.subscribe("/provision/response");

  // Đợi 5s để chắc chắn nhận Respone đầy đủ
  delay(3000);

  //Nhận được Payload thì xử lý thông qua hàm OnMessenger  
  mqttClient.setCallback(OnMessenger);
}*/

void OnMessenger(const char* topic, byte* payload, unsigned int length){    
 // Variable that will receive the message from the payload variable
  char message_json[length + 1];
 // Copy the payload bytes to the message_json variable
  strncpy(message_json, (char*)payload, length);
  Serial.println("Chuoi JSON nhan duoc la:");
  Serial.println(message_json);
 // Sets the string terminator to the null character '\0'
 // Identifying the end of the string.
  message_json[length] = '\0';
 // Create the Json data variable of size 200 bytes
  StaticJsonDocument<200> data;
 // Deserializing the message from the message_json variable
  DeserializationError error = deserializeJson (data, message_json);
 // If there is an error when deserializing, report it.
  if(error){  
    Serial.println("Error deserialize Json") ;
    return;
  }
 // Status do gpio
 // boolean status_gpio;
 // Receives "method" parameter
 // StatusProvisioning = String((const char*)data["status"]); 
  StatusProvisioning = data["status"].as<String>();
 // Receives "enable" parameter
 // CredentialsValue = String((const char*)data["credentialsValue"]);
  CredentialsValueEdge = data["credentialsValue"].as<String>();

  //Nếu có IP edge thì lưu Accestoken vào CredentialsValueEdge, nếu k có Ip Edge thì lưu Accesstoken vào CredentialsValueDemo
  if(StatusProvisioning == "SUCCESS"){
  Serial.println(StatusProvisioning);
  Serial.println(CredentialsValueEdge);
  }
 //boolean enabled = data["params"]["enabled"];
 // Receives "gpio" parameter
 // int pin = data["params"]["gpio"];
 // Checks if the received method is "setValue"
 /*if(StatusProvisioning.equals("SUCCESS")){  
   // If enable is true, then activates relay
   // Note that the drive is with logic level 0
    if(enabled){
      digitalWrite(ControlBulb, HIGH);
     // Update pin status
      status_gpio = true;
    }
 // Otherwise, turn off relay
    else{
      digitalWrite(ControlBulb, LOW);
      status_gpio = false;
    }
  }*/
}

void ReconnectThingsBoardDemo() {
if(StatusProvisioning == "SUCCESS")
  { 
    Serial.println("Provisioning SUCCESS");
    while (!mqttClient.unsubscribe("/provision/response")){
      Serial.println("Unsubscribing provisioning topic...");
    }
    Serial.println("Unsubscribe provisioning topic Done");
    mqttClient.disconnect();
    mqttClient.setServer(thingsBoardDemoHost, Port);
    const char* Save = CredentialsValueEdge.c_str();
    // tạo thêm 1 cái Save1 lưu CredentialsValueDemo
    // Nếu có Ip Edge thì dùng cái Save, Nếu k có Ip Edge thì dùng cái Save1
    mqttClient.connect(WifiDeviceIP, Save, pass);
    while (!mqttClient.connected())
    {
    if(mqttClient.connect(WifiDeviceIP, Save, pass)) {
    Serial.println("Connected ThingsBoard Demo DONE");
    } else {
    Serial.println("Connecting ThingsBoard Demo");
    delay(2000);
    }
    }
    Serial.println("Connected ThingsBoard Demo DONE");
  } else {
    Serial.println("Provisioning FAILED");
  }
}