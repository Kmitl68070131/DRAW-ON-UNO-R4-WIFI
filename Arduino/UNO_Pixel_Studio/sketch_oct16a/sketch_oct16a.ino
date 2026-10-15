
#include <WiFiS3.h>
#include <MQTT.h>
#include <Arduino_LED_Matrix.h>

// ==================== Wi-Fi ====================
const char WIFI_SSID[] = "KMITL-WIFI";
const char WIFI_PASSWORD[] = "1234567890";

// ==================== MQTT ====================
const char MQTT_BROKER[] = "broker.hivemq.com";
const int MQTT_PORT = 1883;

const char TOPIC_SEND[] =
  "it68070131/arduino-uno-r4/send";

const char TOPIC_RECEIVE[] =
  "it68070131/arduino-uno-r4/receive";

const char TOPIC_STATUS[] =
  "it68070131/arduino-uno-r4/status";

// ==================== Objects ====================
WiFiClient net;
MQTTClient mqtt(2048);
ArduinoLEDMatrix matrix;

// LED Matrix มี 8 แถว × 12 คอลัมน์
uint8_t frame[8][12] = {0};

// ==================== Timers ====================
unsigned long lastWiFiAttempt = 0;
unsigned long lastMQTTAttempt = 0;
unsigned long lastHeartbeat = 0;

const unsigned long WIFI_RETRY_INTERVAL = 10000;
const unsigned long MQTT_RETRY_INTERVAL = 5000;
const unsigned long HEARTBEAT_INTERVAL = 5000;

// ==================== State ====================
bool matrixHasImage = false;

// ==================== Prototypes ====================
void connectWiFi();
void connectMQTT();
void messageReceived(String &topic, String &payload);
bool applyPixels(const String &payload);
void publishStatus();
void showBlank();

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.println("================================");
  Serial.println(" UNO R4 WiFi - Pixel Studio");
  Serial.println("================================");

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  matrix.begin();
  showBlank();

  mqtt.begin(MQTT_BROKER, MQTT_PORT, net);
  mqtt.onMessage(messageReceived);

  connectWiFi();
}

void loop() {
  unsigned long now = millis();

  // ตรวจสอบ Wi-Fi และพยายามเชื่อมต่อใหม่
  if (WiFi.status() != WL_CONNECTED) {
    if (now - lastWiFiAttempt >= WIFI_RETRY_INTERVAL) {
      connectWiFi();
    }
    delay(2);
    return;
  }

  // MQTT ต้องเชื่อมต่อหลัง Wi-Fi
  if (!mqtt.connected()) {
    if (now - lastMQTTAttempt >= MQTT_RETRY_INTERVAL) {
      connectMQTT();
    }
  } else {
    mqtt.loop();
  }

  // ส่ง heartbeat เป็นระยะ
  now = millis();
  if (mqtt.connected() &&
      now - lastHeartbeat >= HEARTBEAT_INTERVAL) {
    lastHeartbeat = now;
    publishStatus();
  }

  delay(2);
}

// ==================== Wi-Fi ====================
void connectWiFi() {
  lastWiFiAttempt = millis();

  Serial.print("Connecting to Wi-Fi: ");
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  unsigned long start = millis();

  while (WiFi.status() != WL_CONNECTED &&
         millis() - start < 15000) {
    delay(250);
    Serial.print(".");
  }

  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Wi-Fi connected!");

    IPAddress ip = WiFi.localIP();
    Serial.print("IP address: ");
    Serial.println(ip);
  } else {
    Serial.println("Wi-Fi connection failed.");
    Serial.println("Will retry automatically.");
  }
}

// ==================== MQTT ====================
void connectMQTT() {
  lastMQTTAttempt = millis();

  if (WiFi.status() != WL_CONNECTED) {
    return;
  }

  char clientId[48];
  snprintf(
    clientId,
    sizeof(clientId),
    "UNO-R4-PixelStudio-%08lX",
    (unsigned long)millis()
  );

  Serial.print("Connecting to MQTT broker... ");

  if (mqtt.connect(clientId)) {
    Serial.println("connected!");

    if (mqtt.subscribe(TOPIC_RECEIVE)) {
      Serial.println("Subscribed to receive topic.");
    } else {
      Serial.println("Subscribe failed.");
    }

    // แจ้งสถานะทันที
    publishStatus();

    // บอกเว็บว่าบอร์ดพร้อมใช้งาน
    mqtt.publish(TOPIC_SEND, "online");
  } else {
    Serial.print("failed. MQTT state = ");
    Serial.println(mqtt.lastError());
  }
}

// ==================== Incoming MQTT ====================
void messageReceived(String &topic, String &payload) {
  Serial.print("Topic: ");
  Serial.println(topic);

  Serial.print("Payload: ");
  Serial.println(payload);

  if (topic != TOPIC_RECEIVE) {
    return;
  }

  // คำสั่งเปิด-ปิด LED บนบอร์ด
  if (payload == "on") {
    digitalWrite(LED_BUILTIN, HIGH);
    mqtt.publish(TOPIC_SEND, "LED_BUILTIN_ON");
    return;
  }

  if (payload == "off") {
    digitalWrite(LED_BUILTIN, LOW);
    mqtt.publish(TOPIC_SEND, "LED_BUILTIN_OFF");
    return;
  }

  // รับภาพ JSON จากเว็บ
  if (payload.indexOf("\"pixels\"") >= 0) {
    if (applyPixels(payload)) {
      Serial.println("Image rendered successfully.");
      mqtt.publish(TOPIC_SEND, "image_received");
    } else {
      Serial.println("Invalid image payload.");
      mqtt.publish(TOPIC_SEND, "image_error");
    }
  }
}

// ==================== Parse 96 Pixels ====================
bool applyPixels(const String &payload) {
  int keyPosition = payload.indexOf("\"pixels\"");
  if (keyPosition < 0) {
    return false;
  }

  int start = payload.indexOf('[', keyPosition);
  int end = payload.indexOf(']', start);

  if (start < 0 || end < 0) {
    return false;
  }

  // แปลงข้อมูลลง buffer ชั่วคราวก่อนนำไปแสดง
  uint8_t newFrame[8][12] = {0};

  int count = 0;
  int position = start + 1;

  while (position < end) {
    char c = payload.charAt(position);

    if (c == ' ' || c == '\n' ||
        c == '\r' || c == '\t' || c == ',') {
      position++;
      continue;
    }

    if (c != '0' && c != '1') {
      return false;
    }

    if (count >= 96) {
      return false;
    }

    int row = count / 12;
    int col = count % 12;

    newFrame[row][col] = (c == '1') ? 1 : 0;

    count++;
    position++;
  }

  if (count != 96) {
    return false;
  }

  // ใช้ภาพใหม่เมื่อได้รับข้อมูลครบ 96 พิกเซลเท่านั้น
  memcpy(frame, newFrame, sizeof(frame));
  matrix.renderBitmap(frame, 8, 12);

  matrixHasImage = true;
  return true;
}

// ==================== Blank Matrix ====================
void showBlank() {
  memset(frame, 0, sizeof(frame));
  matrix.renderBitmap(frame, 8, 12);
  matrixHasImage = false;
}

// ==================== Heartbeat ====================
void publishStatus() {
  if (!mqtt.connected()) {
    return;
  }

  IPAddress ip = WiFi.localIP();

  char statusPayload[160];

  snprintf(
    statusPayload,
    sizeof(statusPayload),
    "{\"online\":true,\"uptime\":%lu,\"ip\":\"%u.%u.%u.%u\"}",
    (unsigned long)(millis() / 1000),
    ip[0], ip[1], ip[2], ip[3]
  );

  mqtt.publish(TOPIC_STATUS, statusPayload);

  Serial.print("Heartbeat: ");
  Serial.println(statusPayload);
}
