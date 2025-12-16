#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// ---------- WiFi and MQTT Setup ----------
const char* ssid = "hitech.lk -2.4G";
const char* password = "0113059058";
const char* mqtt_server = "192.168.1.100"; // IP of machine running Node-RED / Mosquitto

WiFiClient espClient;
PubSubClient client(espClient);

// ---------- Fake sensor variables (replace with real sensor readings) ----------
float voltage = 230.5;
float current = 5.2;
float power = 1100.0;
float energy = 2.45;
float pf = 0.96;
float frequency = 50.0;
float energyTemp = 36.5;

float thermoTemp = 65.2;
int productionCount = 0;
float speed = 120.5;

// ---------- WiFi Connect Function ----------
void setup_wifi() {
  delay(10);
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected!");
}

// ---------- Reconnect to MQTT ----------
void reconnect() {
  while (!client.connected()) {
    Serial.print("Connecting to MQTT...");
    if (client.connect("ESP32_Machine1")) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
}

void loop() {
  if (!client.connected()) reconnect();
  client.loop();

  // ---------- Publish Energy Meter Data ----------
  StaticJsonDocument<256> energyDoc;
  energyDoc["voltage"] = voltage;
  energyDoc["current"] = current;
  energyDoc["power"] = power;
  energyDoc["energy"] = energy;
  energyDoc["pf"] = pf;
  energyDoc["frequency"] = frequency;
  energyDoc["temperature"] = energyTemp;

  char energyBuffer[256];
  serializeJson(energyDoc, energyBuffer);
  client.publish("factory/machine1/energy", energyBuffer);

  // ---------- Publish Thermocouple Data ----------
  StaticJsonDocument<128> tempDoc;
  tempDoc["temperature"] = thermoTemp;
  char tempBuffer[128];
  serializeJson(tempDoc, tempBuffer);
  client.publish("factory/machine1/temperature", tempBuffer);

  // ---------- Publish Production Count ----------
  productionCount++; // example increment
  StaticJsonDocument<128> prodDoc;
  prodDoc["count"] = productionCount;
  char prodBuffer[128];
  serializeJson(prodDoc, prodBuffer);
  client.publish("factory/machine1/production", prodBuffer);

  // ---------- Publish Production Speed ----------
  StaticJsonDocument<128> speedDoc;
  speedDoc["speed"] = speed;
  char speedBuffer[128];
  serializeJson(speedDoc, speedBuffer);
  client.publish("factory/machine1/speed", speedBuffer);

  Serial.println("All data published to MQTT");
  delay(5000); // Publish every 5 seconds
}
