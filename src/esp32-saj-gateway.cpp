#include <WiFi.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <ModbusRTU.h>

#include <stdio.h>
#include <string.h>

/*
*  Configuration
*/
// ESP 32 Identification 
const char ESP32_ID[] = "ESP32_SAJ_GATEWAY";

// WIFI Configuration
char* WIFI_SSID = "";
char* WIFI_PSWD = "";

// Network configuration
// const IPAddress NET_IP_ADDR(10, 4, 4, 70);
// const IPAddress NET_GATEWAY(10, 4, 4, 1);
// const IPAddress NET_SUBNET(255, 255, 255, 0);
// const IPAddress NET_P_DNS(10, 4, 4, 4);
// const IPAddress NET_S_DNS(10, 4, 4, 4);

// MQTT Configuration
const char MQTT_HOST[] = "";
const int  MQTT_PORT   = 1883;
const char MQTT_USER[] = "";
const char MQTT_PSWD[] = "";

// MQTT List of topics to suscribe
const char MQTT_CMD[]      = "esp32pwmeter/cmd";
const char MQTT_LOG[]      = "esp32pwmeter/log";
const char MQTT_DBG[]      = "esp32pwmeter/dbg";
const char MQTT_METER[]    = "esp32pwmeter/pwmeter";
const char MQTT_INVERTER[] = "esp32pwmeter/inverter";

// MQTT CMD messages
const char MQTT_MSG_DBG_FLAG[] = "TOGGLE_DBG_FLAG";

// Debug flag
bool SERIAL_DBG_FLAG = false;
bool MQTT_DBG_FLAG   = false;

// Serial configuration
const long RTU_BAUD  = 9600;
const int  RTU_RXD   = 16;
const int  RTU_TXD   = 17;

// RTU Configuration
const int SLAVE_ID = 1;

// Interval change (10s)
const int MEAS_INTERVAL = 10000;

// Initialize the MQTT library
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

// Create Serials
HardwareSerial serialRTU(2);

// Size of responses
const int RTU_MSG_SIZE  = 142;

// Create RS485 modbusRTU
ModbusRTU clientRTU;

// Store temporal message responses
uint8_t* msgRTULong = new uint8_t[RTU_MSG_SIZE];

// Counter
unsigned long lastMeasurement = 0L;

void setupSerial() {

      Serial.begin(115200);
      delay(10); 
      Serial.println("Connected serial U0...");

}

void connectWifi() {

  // Connecting to a WiFi network
  Serial.print("Connecting to Wifi:");
  Serial.print(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PSWD);  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(5000);
  }

  Serial.println();
  Serial.printf("WiFi connected : %s\n", WiFi.localIP().toString());
  
}

void setupWifi() {

//if (!WiFi.config(NET_IP_ADDR, NET_GATEWAY, NET_SUBNET, NET_P_DNS, NET_S_DNS)) {
//  Serial.println("Problem configuring static IP address");
//}

  connectWifi();
  
}

void connectMqtt() {

  // Loop until we're reconnected
  while (!mqttClient.connected()) {
    
    if (WiFi.status() != WL_CONNECTED) {
      connectWifi();
    }
    
    Serial.print("Attempting MQTT connection...");

    // Attempt to connect
    if ( mqttClient.connect(ESP32_ID, MQTT_USER, MQTT_PSWD) ) {
                  
      // Once connected, publish an announcement...
      char outMsg[50];
      sprintf(outMsg, "Connected %s", ESP32_ID);
      mqttClient.publish(MQTT_LOG, outMsg);
      
      // ... and resubscribe
      mqttClient.subscribe(MQTT_CMD);

      Serial.println("OK");

    } else {
      
      Serial.print("KO, stat = ");
      Serial.print(mqttClient.state());
      Serial.println(". Retry in 5 seconds");

      // Wait 5 seconds before retrying
      delay(5000);

    }
  }
}

void mqttReceptCallback(char* topic, byte* payload, unsigned int length) {
  
  char msg[length+1];
  strncpy(msg, (char*) payload, length);
  msg[length] = '\0';

  char outMsg[120];
  if ( sprintf(outMsg, "Received message topic = '%s', value = '%s'", topic, msg) > 0) {
    Serial.println(outMsg);
    mqttClient.publish(MQTT_LOG, outMsg);
  }

  if ( strcmp(msg, MQTT_MSG_DBG_FLAG) == 0 ) {
    MQTT_DBG_FLAG = !MQTT_DBG_FLAG;
    sprintf(outMsg, "Set debug flag to: '%s'", MQTT_DBG_FLAG ? "T" : "F");
    mqttClient.publish(MQTT_LOG,outMsg);
  }

}

void setupMqtt() {
  
  Serial.printf("Setting MQTT - %s:%d\n", MQTT_HOST, MQTT_PORT);
  
  mqttClient.setServer(MQTT_HOST, MQTT_PORT);
  mqttClient.setCallback(mqttReceptCallback);
  mqttClient.setBufferSize(1024);
  connectMqtt();

}

Modbus::ResultCode onDataHandler(uint8_t* data, uint8_t size, void* custom) 
{

  auto src = (Modbus::frame_arg_t*) custom;
  
  if (SERIAL_DBG_FLAG) {
    Serial.printf("RTU Client: %d, Fn: %02X, len: %d\n", src->slaveId, data[0], size);
  }

  // Store results
  if (size == RTU_MSG_SIZE) {
    memcpy(msgRTULong, data, size);
  }

  // Send raw data
  if ( MQTT_DBG_FLAG ) {
    mqttClient.publish(MQTT_DBG, data, size);
  }

  return Modbus::EX_SUCCESS; // Stop procesing the frame

}

void setupRTU() {

  Serial.printf("Setting Client RTU - RX = %d / TX = %d\n", RTU_RXD, RTU_TXD);

  serialRTU.begin(RTU_BAUD, SERIAL_8N1, RTU_RXD, RTU_TXD);

  clientRTU.begin(&serialRTU);

  // Slave mode
  clientRTU.server(SLAVE_ID);
  clientRTU.onRaw(onDataHandler);
  
}

// Setup main program
void setup()
{
  setupSerial();
  setupWifi();
  setupMqtt();
  setupRTU();
}

void loopWifi() 
{

  if (WiFi.status() != WL_CONNECTED) {
    connectWifi();
  }

}

void loopMqtt() {

  if (!mqttClient.connected()) {
    connectMqtt();
  }

  mqttClient.loop();
  
}

union u_tag
{
  uint8_t b[4];
  float f;
};

float readFloat(uint8_t *data, int idx, int offset)
{
  u_tag u;
  
  int n = sizeof(float);
  u.b[0] = data[n*idx+offset + 3];
  u.b[1] = data[n*idx+offset + 2];
  u.b[2] = data[n*idx+offset + 1];
  u.b[3] = data[n*idx+offset + 0];
  
  return u.f;

}

std::string parseMeterMsg() {

  std::string outJsonMsg;

  if (msgRTULong != NULL) {

    StaticJsonDocument<1024> root;
    root["V_L12"] = readFloat(msgRTULong,  0, 2);
    root["V_L23"] = readFloat(msgRTULong,  1, 2);
    root["V_L31"] = readFloat(msgRTULong,  2, 2);
    root["V_L1"]  = readFloat(msgRTULong,  3, 2);
    root["V_L2"]  = readFloat(msgRTULong,  4, 2);
    root["V_L3"]  = readFloat(msgRTULong,  5, 2);
    root["I_L1"]  = readFloat(msgRTULong,  6, 2);
    root["I_L2"]  = readFloat(msgRTULong,  7, 2);
    root["I_L3"]  = readFloat(msgRTULong,  8, 2);
    root["P_T"]   = readFloat(msgRTULong,  9, 2);
    root["P_L1"]  = readFloat(msgRTULong, 10, 2);
    root["P_L2"]  = readFloat(msgRTULong, 11, 2);
    root["P_L3"]  = readFloat(msgRTULong, 12, 2);
    root["Q_T"]   = readFloat(msgRTULong, 13, 2);
    root["Q_L1"]  = readFloat(msgRTULong, 14, 2);
    root["Q_L2"]  = readFloat(msgRTULong, 15, 2);
    root["Q_L3"]  = readFloat(msgRTULong, 16, 2);
    root["S_T"]   = readFloat(msgRTULong, 17, 2);
    root["S_L1"]  = readFloat(msgRTULong, 18, 2);
    root["S_L2"]  = readFloat(msgRTULong, 19, 2);
    root["S_L3"]  = readFloat(msgRTULong, 20, 2);
    root["Pf_T"]  = readFloat(msgRTULong, 21, 2);
    root["Pf_L1"] = readFloat(msgRTULong, 22, 2);
    root["Pf_L2"] = readFloat(msgRTULong, 23, 2);
    root["Pf_L3"] = readFloat(msgRTULong, 24, 2);
    root["Unk5"]  = readFloat(msgRTULong, 25, 2);
    root["Unk6"]  = readFloat(msgRTULong, 26, 2);
    root["Unk7"]  = readFloat(msgRTULong, 27, 2);
    root["Unk8"]  = readFloat(msgRTULong, 28, 2);
    root["Unk9"]  = readFloat(msgRTULong, 29, 2);
    root["Unk10"] = readFloat(msgRTULong, 30, 2);
    root["Unk11"] = readFloat(msgRTULong, 31, 2);
    root["Unk12"] = readFloat(msgRTULong, 32, 2);
    root["Unk13"] = readFloat(msgRTULong, 33, 2);
    root["Freq"]  = readFloat(msgRTULong, 34, 2);
    root["Unk15"] = readFloat(msgRTULong, 35, 2);

    // Convert to JSON
    serializeJson(root, outJsonMsg);

  }

  return outJsonMsg;

}

void loopRTU() {

  clientRTU.task();
  yield();
 
}

// Main loop
void loop() {

  loopWifi();
  loopMqtt(); 
  loopRTU();

  if (millis() - lastMeasurement >= MEAS_INTERVAL) {
    lastMeasurement = millis();

    // Parse data
    std::string outJsonMsg = parseMeterMsg();

    mqttClient.publish(MQTT_METER, outJsonMsg.c_str());

  }

}