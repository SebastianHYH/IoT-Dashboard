#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>

// --- WiFi & MQTT Setup ---
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
const char* mqtt_server = "192.168.X.X"; // your PC's IP

WiFiClient espClient;
PubSubClient client(espClient);

// --- DHT22 Setup ---
#define DHTPIN 4
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// --- MPU6050 Setup ---
Adafruit_MPU6050 mpu;

// --- CD74HC4067 Setup ---
#define SIG_PIN 34
int s0 = 16, s1 = 17, s2 = 5, s3 = 18;

// Pulse Sensor (simulated)
int spo2Pin = 35;

// --- Setup WiFi ---
void setup_wifi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);
}

// --- Reconnect to MQTT ---
void reconnect() {
  while (!client.connected()) {
    client.connect("ESP32Client");
  }
}

// --- Set multiplexer channel ---
void setMuxChannel(int channel) {
  digitalWrite(s0, channel & 0x01);
  digitalWrite(s1, channel & 0x02);
  digitalWrite(s2, channel & 0x04);
  digitalWrite(s3, channel & 0x08);
}

// --- Setup ---
void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);

  dht.begin();

  Wire.begin();
  if (!mpu.begin()) {
    Serial.println("MPU6050 not found!");
    while (1);
  }

  pinMode(s0, OUTPUT);
  pinMode(s1, OUTPUT);
  pinMode(s2, OUTPUT);
  pinMode(s3, OUTPUT);
}

// --- Main Loop ---
void loop() {
  if (!client.connected()) reconnect();
  client.loop();

  // Read DHT22
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();

  // Read MPU6050
  sensors_event_t a, g, tempEvent;
  mpu.getEvent(&a, &g, &tempEvent);

  // Detect posture (basic logic)
  String posture = "Unknown";
  if (abs(a.acceleration.x) < 2 && abs(a.acceleration.z) > 8) {
    posture = "Supine";
  } else if (abs(a.acceleration.x) > 8) {
    posture = "Upright";
  }

  // Read SPO2 (simulate)
  int spo2 = analogRead(spo2Pin) % 20 + 95; // simulated: 95–114

  // Read pressure sensors (channels 0–15)
  String pressureJson = "{";
  for (int i = 0; i < 16; i++) {
    setMuxChannel(i);
    delay(5);
    int pressureVal = analogRead(SIG_PIN);
    pressureJson += "\"p" + String(i) + "\":" + String(pressureVal);
    if (i != 15) pressureJson += ",";
  }
  pressureJson += "}";

  // Construct payload
  String payload = "{";
  payload += "\"temperature\":" + String(temp) + ",";
  payload += "\"humidity\":" + String(hum) + ",";
  payload += "\"posture\":\"" + posture + "\",";
  payload += "\"spo2\":" + String(spo2) + ",";
  payload += "\"pressure\":" + pressureJson;
  payload += "}";

  // Publish to MQTT
  client.publish("iot/health", payload.c_str());

  Serial.println(payload);
  delay(5000); // every 5 seconds
}
