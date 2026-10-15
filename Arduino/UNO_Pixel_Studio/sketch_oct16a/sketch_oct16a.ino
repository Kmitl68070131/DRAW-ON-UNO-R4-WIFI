#include <WiFiS3.h>
#include <MQTT.h>
#include <Arduino_LED_Matrix.h>

// ==================== Wi-Fi ====================
const char WIFI_SSID[] = "KMITL-WIFI";
const char WIFI_PASSWORD[] = "1234567890";

// ==================== MQTT ====================
const char MQTT_BROKER[] = "broker.hivemq.com";
const int MQTT_PORT = 1883;

const char TOPIC_SEND[]    = "it68070131/arduino-uno-r4/send";
const char TOPIC_RECEIVE[] = "it68070131/arduino-uno-r4/receive";
const char TOPIC_STATUS[]  = "it68070131/arduino-uno-r4/status";
const char TOPIC_STATE[]   = "it68070131/arduino-uno-r4/state";

// ==================== Objects ====================
WiFiClient net;
MQTTClient mqtt(2048);
ArduinoLEDMatrix matrix;

uint8_t frame[8][12] = {0};

// ==================== Timers ====================
unsigned long lastWiFiAttempt = 0;
unsigned long lastMQTTAttempt = 0;
unsigned long lastHeartbeat = 0;
unsigned long lastSerialBeat = 0;

const unsigned long WIFI_RETRY_INTERVAL = 10000;
const unsigned long MQTT_RETRY_INTERVAL = 5000;
const unsigned long HEARTBEAT_INTERVAL  = 1000;

// ==================== State ====================
// true เมื่อบอร์ดได้ภาพจริงแล้ว (จาก broker หรือจากเว็บ)
// ก่อนหน้านั้นจะไม่ publish state เพื่อไม่ทับภาพเก่าใน broker
bool matrixHasImage = false;
bool ledIsOn = false;

// ==================== Prototypes ====================
bool wifiReady();
void connectWiFi();
void connectMQTT();
void messageReceived(String &topic, String &payload);
bool applyPixels(const String &payload);
void publishStatus();
void publishState();
void showBlank();

// ==================== Helpers ====================
bool wifiReady() {
  return WiFi.status() == WL_CONNECTED &&
         WiFi.localIP() != IPAddress(0, 0, 0, 0);
}

void setup() {
  Serial.begin(115200);
  delay(1500);

  Serial.println();
  Serial.println("================================");
  Serial.println(" UNO R4 WiFi - Pixel Studio");
  Serial.println("================================");

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  matrix.begin();
  showBlank();

  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("ERROR: ไม่พบโมดูล Wi-Fi");
    while (true) { delay(1000); }
  }

  Serial.print("WiFi firmware: ");
  Serial.println(WiFi.firmwareVersion());
  Serial.print("Latest firmware: ");
  Serial.println(WIFI_FIRMWARE_LATEST_VERSION);

  mqtt.begin(MQTT_BROKER, MQTT_PORT, net);
  mqtt.setKeepAlive(30);
  mqtt.setTimeout(5000);
  mqtt.onMessage(messageReceived);

  mqtt.setWill(TOPIC_STATUS, "{\"online\":false}", false, 0);

  connectWiFi();
}

void loop() {
  unsigned long now = millis();

  if (!wifiReady()) {
    if (now - lastWiFiAttempt >= WIFI_RETRY_INTERVAL) {
      connectWiFi();
    }
    delay(2);
    return;
  }

  if (!mqtt.connected()) {
    if (now - lastMQTTAttempt >= MQTT_RETRY_INTERVAL) {
      connectMQTT();
    }
  } else {
    mqtt.loop();
  }

  now = millis();
  if (mqtt.connected() && now - lastHeartbeat >= HEARTBEAT_INTERVAL) {
    lastHeartbeat = now;
    publishStatus();
    publishState();   // ส่งเมื่อมีภาพจริงแล้วเท่านั้น

    if (now - lastSerialBeat >= 5000) {
      lastSerialBeat = now;
      Serial.print("Heartbeat OK, uptime = ");
      Serial.print(now / 1000);
      Serial.print(" s, image = ");
      Serial.println(matrixHasImage ? "yes" : "waiting");
    }
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

  while (!wifiReady() && millis() - start < 30000) {
    delay(250);
    Serial.print(".");
  }

  Serial.println();

  if (wifiReady()) {
    Serial.println("Wi-Fi connected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.print("Signal (RSSI): ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");
  } else {
    Serial.println("Wi-Fi/DHCP FAILED.");
    Serial.print("WiFi status code = ");
    Serial.println(WiFi.status());
    Serial.print("IP = ");
    Serial.println(WiFi.localIP());
    Serial.println("เช็ก: รหัส Wi-Fi, 2.4GHz, DHCP ของ router");
    Serial.println("Will retry automatically.");
  }
}

// ==================== MQTT ====================
void connectMQTT() {
  lastMQTTAttempt = millis();

  if (!wifiReady()) {
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

    // subscribe ทั้งสอง topic: broker จะส่งภาพ retained ล่าสุดมาให้ทันที
    // แม้ไม่มีใครเปิดเว็บค้างไว้
    bool ok1 = mqtt.subscribe(TOPIC_RECEIVE);
    bool ok2 = mqtt.subscribe(TOPIC_STATE);

    if (ok1 && ok2) {
      Serial.println("Subscribed. Pulling last image from broker...");
    } else {
      Serial.println("Subscribe failed.");
    }

    publishStatus();
    mqtt.publish(TOPIC_SEND, "online");
  } else {
    Serial.print("FAILED. lastError = ");
    Serial.print(mqtt.lastError());
    Serial.print(", returnCode = ");
    Serial.println(mqtt.returnCode());
    Serial.println("ถ้าเจอซ้ำ ๆ: เครือข่ายอาจบล็อกพอร์ต 1883 ลองใช้ hotspot มือถือ");
  }
}

// ==================== Incoming MQTT ====================
void messageReceived(String &topic, String &payload) {

  // ---- ภาพ/สถานะเก่าที่ broker เก็บไว้: ใช้กู้ภาพตอนบูตเท่านั้น ----
  if (topic == TOPIC_STATE) {
    if (matrixHasImage) return;   // มีภาพแล้ว ไม่ต้องรับจาก state อีก

    if (applyPixels(payload)) {
      if (payload.indexOf("\"led\":true") >= 0) {
        ledIsOn = true;
        digitalWrite(LED_BUILTIN, HIGH);
      } else if (payload.indexOf("\"led\":false") >= 0) {
        ledIsOn = false;
        digitalWrite(LED_BUILTIN, LOW);
      }

      Serial.println("Restored image from broker (state).");
      publishState();
    }
    return;
  }

  if (topic != TOPIC_RECEIVE) {
    return;
  }

  if (payload == "on") {
    digitalWrite(LED_BUILTIN, HIGH);
    ledIsOn = true;
    mqtt.publish(TOPIC_SEND, "LED_BUILTIN_ON");
    publishState();
    return;
  }

  if (payload == "off") {
    digitalWrite(LED_BUILTIN, LOW);
    ledIsOn = false;
    mqtt.publish(TOPIC_SEND, "LED_BUILTIN_OFF");
    publishState();
    return;
  }

  if (payload == "sync") {
    publishState();
    return;
  }

  if (payload.indexOf("\"pixels\"") >= 0) {
    bool firstImage = !matrixHasImage;

    if (applyPixels(payload)) {
      if (firstImage) {
        Serial.println("Restored image from broker (receive).");
      }
      mqtt.publish(TOPIC_SEND, "image_received");
      publishState();
    } else {
      Serial.println("Invalid image payload.");
      mqtt.publish(TOPIC_SEND, "image_error");
    }
  }
}

// ==================== Parse 96 Pixels ====================
bool applyPixels(const String &payload) {
  int keyPosition = payload.indexOf("\"pixels\"");
  if (keyPosition < 0) return false;

  int start = payload.indexOf('[', keyPosition);
  if (start < 0) return false;

  int end = payload.indexOf(']', start);
  if (end < 0) return false;

  uint8_t newFrame[8][12] = {0};

  int count = 0;
  int position = start + 1;

  while (position < end) {
    char c = payload.charAt(position);

    if (c == ' ' || c == '\n' || c == '\r' || c == '\t' || c == ',') {
      position++;
      continue;
    }

    if (c != '0' && c != '1') return false;
    if (count >= 96) return false;

    newFrame[count / 12][count % 12] = (c == '1') ? 1 : 0;

    count++;
    position++;
  }

  if (count != 96) return false;

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

// ==================== Status ====================
void publishStatus() {
  if (!mqtt.connected()) return;

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
}

// ==================== State (retained) ====================
// ถ้ายังไม่ได้ภาพจริง จะไม่ publish อะไรเลย เพื่อไม่ทับภาพเก่าที่ broker เก็บไว้
void publishState() {
  if (!mqtt.connected()) return;
  if (!matrixHasImage) return;

  char buffer[384];
  int len = snprintf(buffer, sizeof(buffer), "{\"pixels\":[");

  for (int i = 0; i < 96; i++) {
    len += snprintf(
      buffer + len,
      sizeof(buffer) - len,
      "%s%d",
      (i == 0) ? "" : ",",
      frame[i / 12][i % 12] ? 1 : 0
    );
  }

  snprintf(
    buffer + len,
    sizeof(buffer) - len,
    "],\"led\":%s}",
    ledIsOn ? "true" : "false"
  );

  mqtt.publish(TOPIC_STATE, buffer, true, 0);
}
