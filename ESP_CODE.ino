#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>
#include <WiFiClientSecure.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <ESP32Servo.h>

// Wi-Fi credentials
const char* ssid = "Anil";
const char* password = "g267d8xz";

// Server URL
const char* serverURL = "YOUR_SERVER_ADDRESS";

// GPIO Definitions
#define RED_PIN 23
#define GREEN_PIN 22
#define BLUE_PIN 21
#define BUZZER_PIN 19
#define IR_PIN 35
#define SERVO_PIN 27
#define RELAY_PIN 13
#define SOIL_PIN 34
#define DHTPIN 18
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);
Servo myServo;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 19800, 60000);

// State Variables
bool lightOn = false;
bool emergencyActive = false;
bool nightSecurityEnabled = false;
bool alarmActive = false;
bool alarmTriggered = false;
int alarmHour = -1;
int alarmMinute = -1;
String lastCommand = "";

void setup() {
  Serial.begin(115200);
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(IR_PIN, INPUT);
  pinMode(SOIL_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);

  digitalWrite(RED_PIN, HIGH);
  digitalWrite(GREEN_PIN, HIGH);
  digitalWrite(BLUE_PIN, HIGH);
  digitalWrite(BUZZER_PIN, HIGH);
  digitalWrite(RELAY_PIN, LOW);

  myServo.attach(SERVO_PIN);
  dht.begin();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✅ Wi-Fi Connected");
  timeClient.begin();
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(String(serverURL) + "/command");
    int httpCode = http.GET();

    if (httpCode == 200) {
      String response = http.getString();
      response.replace("{\"cmd\":\"", "");
      response.replace("\"}", "");
      response.trim();
      response.toLowerCase();

      if (response.length() > 0 && response != lastCommand) {
        Serial.println("🟢 Command: " + response);
        lastCommand = response;
        handleCommand(response);
      }
    }
    http.end();
  }

  if (emergencyActive) digitalWrite(BUZZER_PIN, LOW);

  if (nightSecurityEnabled && digitalRead(IR_PIN) == LOW) {
    digitalWrite(BUZZER_PIN, LOW);
    delay(500);
    // digitalWrite(BUZZER_PIN, HIGH);
    // delay(200);
  }

  timeClient.update();
  if (alarmActive && !alarmTriggered && timeClient.getHours() == alarmHour && timeClient.getMinutes() == alarmMinute) {
    alarmTriggered = true;
    digitalWrite(BUZZER_PIN, LOW);
    sendDataToServer("⏰ Alarm ringing!");
    sendDeviceStatus();  // optional
  }

  static unsigned long lastSend = 0;
  if (millis() - lastSend > 10000) {
    sendSensorData();
    sendDeviceStatus();
    lastSend = millis();
  }

  delay(500);
}

void handleCommand(String cmd) {
  cmd.toLowerCase();
  cmd.trim();

  if ((cmd.indexOf("turn on") >= 0 || cmd.indexOf("switch on") >= 0) && cmd.indexOf("light") >= 0) {
    setColor(LOW, LOW, LOW);
    lightOn = true;
    sendDataToServer("💡 Light turned ON");
    sendDeviceStatus();

  } else if ((cmd.indexOf("turn off") >= 0 || cmd.indexOf("switch off") >= 0) && cmd.indexOf("light") >= 0) {
    setColor(HIGH, HIGH, HIGH);
    lightOn = false;
    sendDataToServer("💡 Light turned OFF");
    sendDeviceStatus();

  } else if ((cmd.indexOf("set color to") >= 0 || cmd.indexOf("change color to") >= 0 || cmd.indexOf("set rgb to") >= 0 || cmd.indexOf("change rgb to") >= 0) && lightOn) {
    String color = cmd.substring(cmd.indexOf("to") + 3);
    color.trim();

    if (color == "red") setColor(LOW, HIGH, HIGH);
    else if (color == "green") setColor(HIGH, LOW, HIGH);
    else if (color == "blue") setColor(HIGH, HIGH, LOW);
    else if (color == "yellow") setColor(LOW, LOW, HIGH);
    else if (color == "cyan") setColor(HIGH, LOW, LOW);
    else if (color == "magenta" || color == "pink") setColor(LOW, HIGH, LOW);
    else if (color == "white") setColor(LOW, LOW, LOW);
    else if (color == "orange") setColor(LOW, LOW, HIGH);
    else if (color == "purple") setColor(LOW, HIGH, LOW);
    else if (color == "warm white") setColor(LOW, LOW, HIGH);
    else {
      sendDataToServer("⚠️ Unknown color");
      return;
    }
    sendDataToServer("🎨 Color set to " + color);

  } else if ((cmd.indexOf("set color to") >= 0 || cmd.indexOf("change color to") >= 0) && !lightOn) {
    sendDataToServer("⚠️ Light is OFF. Turn it ON before changing color.");

  } else if ((cmd.indexOf("turn on") >= 0 || cmd.indexOf("start") >= 0) && cmd.indexOf("fan") >= 0) {
    digitalWrite(RELAY_PIN, HIGH);
    sendDataToServer("🌀 Fan turned ON");
    sendDeviceStatus();

  } else if ((cmd.indexOf("turn off") >= 0 || cmd.indexOf("stop") >= 0) && cmd.indexOf("fan") >= 0) {
    digitalWrite(RELAY_PIN, LOW);
    sendDataToServer("🌀 Fan turned OFF");
    sendDeviceStatus();

  } else if (cmd.indexOf("open") >= 0 && cmd.indexOf("door") >= 0) {
    myServo.write(180);
    sendDataToServer("🚪 Door opened");

  } else if (cmd.indexOf("close") >= 0 && cmd.indexOf("door") >= 0) {
    myServo.write(90);
    sendDataToServer("🚪 Door closed");

  } else if (cmd.indexOf("emergency") >= 0 || cmd.indexOf("i need help") >= 0 || cmd.indexOf("help me") >= 0) {
    emergencyActive = true;
    sendDataToServer("🚨 Emergency alert activated");

  } else if (cmd.indexOf("cancel help") >= 0 || cmd.indexOf("stop emergency") >= 0 || cmd.indexOf("disable emergency") >= 0) {
    emergencyActive = false;
    digitalWrite(BUZZER_PIN, HIGH);
    sendDataToServer("✅ Emergency alert stopped");

  } else if ((cmd.indexOf("enable") >= 0 || cmd.indexOf("turn on") >= 0) && cmd.indexOf("security") >= 0) {
    nightSecurityEnabled = true;
    sendDataToServer("🔒 Night security mode enabled");
    sendDeviceStatus();

  } else if ((cmd.indexOf("disable") >= 0 || cmd.indexOf("turn off") >= 0) && cmd.indexOf("security") >= 0) {
    nightSecurityEnabled = false;
    digitalWrite(BUZZER_PIN, HIGH);
    sendDataToServer("🔓 Night security mode disabled");
    sendDeviceStatus();

  } else if (cmd.startsWith("set alarm for") || cmd.indexOf("wake me at") >= 0) {
    int colonIndex = cmd.indexOf(':');
    if (colonIndex > 0) {
      alarmHour = cmd.substring(14, colonIndex).toInt();
      alarmMinute = cmd.substring(colonIndex + 1).toInt();
      alarmActive = true;
      alarmTriggered = false;
      sendDataToServer("⏰ Alarm set for " + String(alarmHour) + ":" + String(alarmMinute));
    } else {
      sendDataToServer("⚠ Invalid alarm format. Use 'set alarm for HH:MM'");
    }

  } else if (cmd.indexOf("stop alarm") >= 0 || cmd.indexOf("cancel alarm") >= 0 || cmd.indexOf("turn off alarm") >= 0) {
    digitalWrite(BUZZER_PIN, HIGH);
    alarmActive = false;
    alarmTriggered = false;
    sendDataToServer("✅ Alarm stopped");

  } else if (cmd.indexOf("temperature") >= 0 || cmd.indexOf("temp") >= 0) {
    float t = dht.readTemperature();
    if (!isnan(t)) sendDataToServer("🌡️ Temp: " + String(t) + " °C");

  } else if (cmd.indexOf("humidity") >= 0) {
    float h = dht.readHumidity();
    if (!isnan(h)) sendDataToServer("💧 Humidity: " + String(h) + " %");

  } else if (cmd.indexOf("soil") >= 0 || cmd.indexOf("moisture") >= 0) {
    int soilValue = analogRead(SOIL_PIN);
    String status = "";
    if (soilValue > 3000) status = "Very dry 🌵 Water needed";
    else if (soilValue > 2000) status = "Dry Consider watering";
    else if (soilValue > 1000) status = "Moist Looks good";
    else status = "Wet 🌧️ No need to water";
    sendDataToServer("🌱 Soil: " + String(soilValue) + " " + status);

  } else {
    sendDataToServer("⚠️ Unknown command: " + cmd);
  }
}

void sendDataToServer(String msg) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(String(serverURL) + "/response");
    http.addHeader("Content-Type", "application/json");
    String body = "{\"data\":\"" + msg + "\"}";
    int code = http.POST(body);
    if (code > 0) Serial.println("📤 Sent: " + msg);
    else Serial.println("❌ Failed to send message");
    http.end();
  }
}

void sendSensorData() {
  float temp = dht.readTemperature();
  float humidity = dht.readHumidity();
  int soilRaw = analogRead(SOIL_PIN);
  int soilPercent = map(soilRaw, 1000, 3000, 100, 0);
  soilPercent = constrain(soilPercent, 0, 100);

  if (!isnan(temp) && !isnan(humidity)) {
    HTTPClient http;
    http.begin(String(serverURL) + "/sensor");
    http.addHeader("Content-Type", "application/json");

    String payload = "{\"temp\":\"" + String(temp, 1) + "\",\"humidity\":\"" + String(humidity, 1) + "\",\"soil\":\"" + String(soilPercent) + "\"}";
    int code = http.POST(payload);
    if (code > 0) Serial.println("📤 Sensor Data Sent: " + payload);
    else Serial.println("❌ Sensor POST Failed");
    http.end();
  }
}

void sendDeviceStatus() {
  String fanState = digitalRead(RELAY_PIN) == HIGH ? "ON" : "OFF";
  String lightState = lightOn ? "ON" : "OFF";
  String securityState = nightSecurityEnabled ? "ARMED" : "DISARMED";

  HTTPClient http;
  http.begin(String(serverURL) + "/devices");
  http.addHeader("Content-Type", "application/json");

  String json = "{\"fan\":\"" + fanState + "\",\"light\":\"" + lightState + "\",\"security\":\"" + securityState + "\"}";
  int code = http.POST(json);
  if (code > 0) Serial.println("📤 Device Status Sent: " + json);
  else Serial.println("❌ Device Status POST Failed");
  http.end();
}

void setColor(int r, int g, int b) {
  digitalWrite(RED_PIN, r);
  digitalWrite(GREEN_PIN, g);
  digitalWrite(BLUE_PIN, b);
}