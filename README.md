# 🎨 UNO R4 WiFi Pixel Studio

### Version 2.0.0 — Real-time Drawing & Shared State

เว็บแอปพลิเคชันวาดภาพพิกเซลและควบคุม LED Matrix บน **Arduino UNO R4 WiFi** แบบ Real-time ผ่าน MQTT รองรับการซิงก์ภาพระหว่างผู้ใช้หลายคน และการเรียกคืนภาพล่าสุดเมื่อเปิดหน้าเว็บใหม่ผ่านระบบ Shared State

[![Arduino](https://img.shields.io/badge/Arduino-UNO%20R4%20WiFi-00979D?logo=arduino&logoColor=white)](https://docs.arduino.cc/hardware/uno-r4-wifi/)
[![Version](https://img.shields.io/badge/Version-2.0.0-blue)](https://github.com/Kmitl68070131/DRAW-ON-UNO-R4-WIFI)
[![MQTT](https://img.shields.io/badge/Protocol-MQTT-purple)](https://mqtt.org/)
[![Web](https://img.shields.io/badge/Web-HTML%20%7C%20CSS%20%7C%20JavaScript-orange)](https://developer.mozilla.org/)
[![License](https://img.shields.io/badge/License-See%20Repository-lightgrey)](https://github.com/Kmitl68070131/DRAW-ON-UNO-R4-WIFI)

> 🖥️ วาดภาพบนเว็บ → ส่งข้อมูลผ่าน MQTT → แสดงผลบน LED Matrix → ซิงก์ภาพล่าสุดกลับไปยังผู้ใช้ทุกคน

🔗 **GitHub Repository:** https://github.com/Kmitl68070131/DRAW-ON-UNO-R4-WIFI

---

## 📑 สารบัญ

- [✨ What's New in Version 2.0](#-whats-new-in-version-20)
- [🎯 Features](#-features)
- [🕹️ วิธีใช้งาน](#️-วิธีใช้งาน)
- [🧠 System Architecture](#-system-architecture)
- [🔄 Shared State และ MQTT Retained Message](#-shared-state-และ-mqtt-retained-message)
- [🧰 อุปกรณ์ที่ต้องใช้](#-อุปกรณ์ที่ต้องใช้)
- [📚 ไลบรารีและเครื่องมือ](#-ไลบรารีและเครื่องมือ)
- [📂 โครงสร้างโปรเจกต์](#-โครงสร้างโปรเจกต์)
- [🚀 Installation](#-installation)
- [🔌 ตั้งค่า Arduino UNO R4 WiFi](#-ตั้งค่า-arduino-uno-r4-wifi)
- [⚙️ MQTT Configuration](#️-mqtt-configuration)
- [🌐 Deploy เว็บไซต์](#-deploy-เว็บไซต์)
- [🧪 Testing Checklist](#-testing-checklist)
- [🛠️ Troubleshooting](#️-troubleshooting)
- [🔐 Security](#-security)
- [❓ FAQ](#-faq)
- [🗺️ Roadmap](#️-roadmap)
- [📖 References](#-references)

---

## ✨ What's New in Version 2.0

Version 2.0 มุ่งเน้นการเปลี่ยนจากเว็บวาดภาพที่ส่งข้อมูลไปยังบอร์ดเป็นครั้ง ๆ ให้เป็นระบบวาดภาพที่สามารถซิงก์สถานะล่าสุดร่วมกันได้

| Feature | Version 1.x | Version 2.0 |
|---|---|---|
| 🎨 Pixel Editor | วาดภาพบนเว็บ | ยังคงรองรับ |
| 📡 MQTT | ส่งภาพผ่าน MQTT | ใช้ MQTT เป็นช่องทางสื่อสารหลัก |
| ⚡ Real-time | ส่งภาพเมื่อมีการเปลี่ยนแปลง | รองรับการอัปเดตภาพต่อเนื่อง |
| 👥 Multi-user | แชร์ผ่าน Topic เดียวกัน | ออกแบบให้ผู้ใช้เห็นสถานะภาพร่วมกัน |
| 🔄 Shared State | ขึ้นอยู่กับการเชื่อมต่อ | ใช้ retained state เพื่อเรียกคืนภาพล่าสุด |
| 🌐 Reconnect | ต้องจัดการข้อมูลใหม่ตามการเชื่อมต่อ | ออกแบบให้ซิงก์สถานะล่าสุดหลังเชื่อมต่อใหม่ |
| 💡 LED Matrix | แสดงภาพบนบอร์ด | แสดงภาพตามสถานะที่ได้รับ |

**หมายเหตุ:** ตารางนี้อธิบายแนวทางการพัฒนาของ Version 2.0 การทำงานของแต่ละฟังก์ชันขึ้นอยู่กับว่าโค้ดเว็บและโค้ด Arduino เวอร์ชันที่นำไปใช้งานได้ติดตั้งกลไกดังกล่าวครบถ้วนแล้วหรือไม่

### เป้าหมายหลัก

- ให้ผู้ใช้หลายคนเปิดเว็บแล้วเห็นภาพเดียวกัน
- ให้ผู้ใช้ที่เปิดเว็บภายหลังสามารถรับภาพล่าสุดได้
- ลดความจำเป็นในการรีสตาร์ตบอร์ดเพื่ออัปเดตภาพ
- รองรับการเชื่อมต่อใหม่เมื่อเครือข่ายขาดช่วง
- แยกการวาดภาพ การส่งข้อมูล และการแสดงสถานะการเชื่อมต่อออกจากกัน

---

## 🎯 Features

### 🎨 1. Pixel Editor

สร้างภาพพิกเซลผ่านเว็บเบราว์เซอร์ โดยใช้พื้นที่วาดขนาด 12 × 8 พิกเซล

- 🖱️ คลิกพิกเซลเพื่อเปลี่ยนสถานะ
- 🧩 สร้างภาพจาก LED จำนวน 96 จุด
- 🧹 ล้างภาพและเริ่มวาดใหม่
- ⚡ ส่งข้อมูลภาพเมื่อมีการเปลี่ยนแปลงตามกลไกที่หน้าเว็บกำหนด

### 💡 2. Arduino LED Matrix

แสดงภาพบน LED Matrix ที่ติดตั้งมาใน Arduino UNO R4 WiFi โดยไม่ต้องต่อจอ LED ภายนอกสำหรับการใช้งานพื้นฐาน

### 📡 3. MQTT Communication

ใช้ MQTT Broker เป็นตัวกลางรับส่งข้อมูลระหว่างเว็บและบอร์ด

- เว็บเชื่อมต่อผ่าน MQTT over WebSocket
- บอร์ดเชื่อมต่อผ่าน MQTT บนเครือข่าย Wi-Fi
- ทั้งสองฝั่งใช้ Topic ที่กำหนดร่วมกัน

### ⚡ 4. Real-time Synchronization

เมื่อมีการเปลี่ยนแปลงภาพ ระบบสามารถส่งสถานะใหม่ไปยัง Broker เพื่อกระจายให้ผู้ใช้และบอร์ดที่ Subscribe Topic ที่เกี่ยวข้อง

### 🔄 5. Shared State

ใช้ MQTT Retained Message สำหรับเก็บข้อความล่าสุดของ Topic ที่กำหนด ทำให้ผู้ใช้ที่ Subscribe ภายหลังสามารถได้รับสถานะล่าสุดที่ Broker เก็บไว้ได้

การทำงานนี้ต้องมีการ Publish ข้อความโดยเปิด Retain และ Subscribe Topic ให้ถูกต้อง

### 🌐 6. Online Web Application

เว็บสามารถโฮสต์บน GitHub Pages, Vercel หรือ Static Hosting อื่นที่รองรับ HTTPS ได้ โดย Broker ต้องรองรับ WebSocket ที่เว็บใช้ด้วย

---

## 🕹️ วิธีใช้งาน

### Step 1 — เปิดเว็บไซต์

เปิดหน้าเว็บ Pixel Studio ที่เผยแพร่แล้ว หรือเปิดไฟล์ `index.html` เพื่อทดสอบส่วนติดต่อผู้ใช้

หากเปิดจากเครื่องโดยตรง ให้ตรวจสอบว่าไลบรารี JavaScript และ MQTT WebSocket เชื่อมต่อได้จากสภาพแวดล้อมที่ใช้งาน

### Step 2 — วาดภาพ

1. คลิกพิกเซลที่ต้องการเปลี่ยน
2. ปรับภาพตามต้องการ
3. รอให้ระบบส่งข้อมูล หรือกดปุ่มส่งภาพหากหน้าเว็บกำหนดให้ส่งด้วยตนเอง
4. ตรวจสอบผลลัพธ์บน LED Matrix

### Step 3 — เปิดบอร์ด

1. ต่อ Arduino UNO R4 WiFi เข้ากับ USB-C
2. จ่ายไฟให้บอร์ด
3. ตรวจสอบการเชื่อมต่อ Wi-Fi
4. ตรวจสอบการเชื่อมต่อ MQTT
5. รอให้บอร์ดรับสถานะภาพล่าสุด

### Step 4 — ทดสอบหลายผู้ใช้

1. เปิดหน้าเว็บในเบราว์เซอร์แรก
2. วาดภาพและรอให้ระบบซิงก์
3. เปิดเว็บไซต์เดียวกันในหน้าต่างไม่ระบุตัวตนหรืออุปกรณ์อีกเครื่อง
4. ตรวจสอบว่าภาพล่าสุดปรากฏในหน้าที่เปิดใหม่
5. ทดลองวาดจากอีกหน้าต่าง แล้วตรวจสอบผลลัพธ์บนทุกหน้าต่างและบนบอร์ด

**เงื่อนไข:** การซิงก์ระหว่างผู้ใช้ต้องมีการ Subscribe สถานะร่วมกันและมีการส่งสถานะล่าสุดอย่างถูกต้อง การเปิดเว็บหลายหน้าต่างเพียงอย่างเดียวไม่ได้ทำให้ข้อมูลซิงก์กันโดยอัตโนมัติ

---

## 🧠 System Architecture

```text
┌─────────────────────────────┐
│        Web Client A         │
│      Pixel Editor           │
└──────────────┬──────────────┘
               │
               │ MQTT over WebSocket
               ▼
┌─────────────────────────────┐
│         MQTT Broker         │
│                             │
│  /send   /state   /status   │
│                             │
│  Retained State (latest)    │
└───────────┬─────────────────┘
            │
            ├─────────────────────┐
            │                     │
            ▼                     ▼
┌───────────────────┐  ┌─────────────────────┐
│    Web Client B   │  │ Arduino UNO R4 WiFi │
│ Shared Pixel View │  │ MQTT + LED Matrix   │
└───────────────────┘  └──────────┬──────────┘
                                  │
                                  ▼
                         ┌─────────────────┐
                         │  LED Matrix     │
                         │    12 × 8       │
                         └─────────────────┘
```

### ลำดับการทำงาน

1. ผู้ใช้วาดภาพบนเว็บ
2. เว็บแปลงภาพเป็น Payload ตามรูปแบบที่กำหนด
3. เว็บ Publish ข้อมูลไปยัง Topic สำหรับรับคำสั่ง
4. Arduino รับข้อมูลและแสดงผล
5. ระบบ Publish สถานะภาพล่าสุดไปยัง Topic สำหรับ Shared State
6. เว็บทุกหน้าที่ Subscribe สถานะจะอัปเดตตามข้อความที่ได้รับ
7. ผู้ใช้ที่เปิดเว็บใหม่จะได้รับ Retained Message หาก Broker ยังเก็บข้อความไว้และ Topic ตรงกัน

> MQTT เป็นระบบส่งข้อความ ไม่ใช่ฐานข้อมูลถาวร Retained Message โดยทั่วไปเก็บข้อความล่าสุดต่อ Topic เท่านั้น และข้อมูลอาจหายเมื่อมีการลบ Retained Message หรือ Broker ล้างข้อมูล

---

## 🔄 Shared State และ MQTT Retained Message

### Retained Message คืออะไร?

MQTT Retained Message คือข้อความที่ Publisher ส่งให้ Broker โดยตั้งค่า `retain = true` เพื่อให้ Broker เก็บข้อความล่าสุดของ Topic นั้นไว้

เมื่อ Subscriber รายใหม่ Subscribe Topic ดังกล่าว Broker สามารถส่งข้อความที่เก็บไว้นั้นกลับมาได้ทันที

ตัวอย่างเช่น หากเก็บภาพล่าสุดไว้ใน Topic

```text
it68070131/arduino-uno-r4/state
```

เมื่อเว็บเปิดใหม่และ Subscribe Topic นี้ เว็บจะได้รับสถานะล่าสุดที่ Broker เก็บไว้ หากยังมี Retained Message อยู่

### Topic ที่ใช้ในโปรเจกต์

| Topic | หน้าที่ |
|---|---|
| `it68070131/arduino-uno-r4/send` | เว็บส่งภาพหรือคำสั่งไปยังบอร์ด |
| `it68070131/arduino-uno-r4/receive` | ช่องทางส่งข้อมูลกลับจากบอร์ดหรือข้อมูลตอบกลับ ตามที่โค้ดกำหนด |
| `it68070131/arduino-uno-r4/status` | สถานะการทำงานหรือการเชื่อมต่อ |
| `it68070131/arduino-uno-r4/state` | สถานะภาพล่าสุดสำหรับ Shared State |

Topic ข้างต้นเป็นผังที่แนะนำสำหรับ Version 2.0 ต้องตรวจสอบให้ตรงกับค่าที่กำหนดจริงในไฟล์เว็บและไฟล์ Arduino

### แนวทางการทำงานของ Shared State

**ฝั่งเว็บ**

- Subscribe Topic สถานะภาพ
- นำ Payload ที่ได้รับมาอัปเดต Pixel Editor
- Publish ภาพที่เปลี่ยนแปลงตามรูปแบบข้อมูลที่ระบบกำหนด
- เมื่อเชื่อมต่อใหม่ ให้ Subscribe สถานะอีกครั้ง

**ฝั่ง Arduino**

- Subscribe Topic รับภาพ
- ตรวจสอบรูปแบบและจำนวนพิกเซล
- แสดงภาพบน LED Matrix
- Publish สถานะล่าสุดเมื่อภาพเปลี่ยนแปลง
- ส่งสถานะล่าสุดอีกครั้งหลังเชื่อมต่อใหม่ตามที่โค้ดกำหนด

### ตัวอย่าง Payload

ตัวอย่าง JSON สำหรับภาพขนาด 12 × 8 ที่แต่ละพิกเซลแทนค่าเปิดหรือปิด:

```json
{
  "pixels": [
    0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0
  ]
}
```

ตัวอย่างนี้แสดงเฉพาะ 12 ค่าแรกเพื่อให้อ่านง่าย ส่วน Payload ที่ใช้งานจริงต้องมีข้อมูลครบ 96 พิกเซลตามลำดับที่โค้ดกำหนด

### ข้อควรระวังเกี่ยวกับหลายผู้ใช้

Shared State ทำให้ผู้ใช้เห็นสถานะร่วมกันได้ แต่หากมีหลายคนวาดพร้อมกัน ข้อความอาจมาถึงต่างลำดับกัน

หากต้องการให้ผลลัพธ์มีความแน่นอน ควรกำหนดกติกาเพิ่มเติม เช่น

- ใช้ภาพที่ได้รับล่าสุดเป็นสถานะปัจจุบัน
- ใช้ Timestamp หรือ Revision Number เพื่อช่วยจัดลำดับ
- จำกัดความถี่ในการ Publish
- ตรวจสอบ Payload ก่อนนำไปแสดงผล
- กำหนดให้ Arduino เป็นผู้ยืนยันสถานะภาพกลาง หากต้องการให้บอร์ดเป็นแหล่งข้อมูลหลัก

**ข้อจำกัด:** MQTT Retained Message ไม่ได้แก้ปัญหาการเขียนทับข้อมูลพร้อมกันโดยอัตโนมัติ และไม่ได้รับประกันว่าทุก Client จะมีภาพเหมือนกันในทุกช่วงเวลาโดยไม่มีการออกแบบเพิ่มเติม

---

## 🧰 อุปกรณ์ที่ต้องใช้

| อุปกรณ์ | จำนวน | รายละเอียด |
|---|---:|---|
| Arduino UNO R4 WiFi | 1 | บอร์ดไมโครคอนโทรลเลอร์พร้อม Wi-Fi |
| สาย USB-C | 1 | จ่ายไฟและอัปโหลดโค้ด |
| คอมพิวเตอร์ | 1 | ใช้พัฒนาและอัปโหลดโปรแกรม |
| เครือข่าย Wi-Fi 2.4 GHz | 1 | ใช้เชื่อมต่อ MQTT Broker |
| เว็บเบราว์เซอร์ | 1 | ใช้เปิด Pixel Studio |
| MQTT Broker | 1 | ใช้เป็นตัวกลางรับส่งข้อความ |

ไม่จำเป็นต้องต่อ LED Matrix เพิ่มสำหรับการใช้จอ LED ที่ติดตั้งมากับบอร์ด

---

## 📚 ไลบรารีและเครื่องมือ

### 1. Arduino IDE

ดาวน์โหลดจาก https://www.arduino.cc/en/software

ใช้แก้ไข คอมไพล์ และอัปโหลดไฟล์ `.ino`

### 2. Arduino UNO R4 Boards

ติดตั้งผ่าน Boards Manager ใน Arduino IDE แล้วเลือกบอร์ด `Arduino UNO R4 WiFi`

### 3. WiFiS3

```cpp
#include <WiFiS3.h>
```

ใช้เชื่อมต่อ Wi-Fi ของ Arduino UNO R4 WiFi โดยไลบรารีนี้มากับแพ็กเกจบอร์ดที่เกี่ยวข้อง

### 4. MQTTClient

```cpp
#include <MQTTClient.h>
```

เลือกไลบรารี MQTT ที่มี API ตรงกับโค้ด โดย `MQTT` ของ 256dpi เป็นตัวเลือกหนึ่งที่ใช้ Header นี้

### 5. Arduino_LED_Matrix

```cpp
#include "Arduino_LED_Matrix.h"
```

ใช้ควบคุม LED Matrix ในตัวบอร์ด หากโค้ดเรียกใช้ไลบรารีนี้

### 6. MQTT.js

หากหน้าเว็บเชื่อมต่อ MQTT โดยตรงผ่าน WebSocket สามารถใช้ MQTT.js ได้

เอกสาร: https://github.com/mqttjs/MQTT.js

ตรวจสอบเวอร์ชันและ URL ของไลบรารีที่หน้าเว็บใช้งานจริงก่อนเผยแพร่

---

## 📂 โครงสร้างโปรเจกต์

โครงสร้างที่อ้างอิงจากตำแหน่งไฟล์ที่ใช้พัฒนาโปรเจกต์:

```text
DRAW-ON-UNO-R4-WIFI/
│
├── README.md
│
└── Arduino/
    └── UNO_Pixel_Studio/
        ├── index.html
        └── sketch_oct16a.ino
```

หาก Repository มีไฟล์ CSS, JavaScript หรือไฟล์ประกอบอื่นแยกต่างหาก ให้เพิ่มชื่อไฟล์เหล่านั้นตามโครงสร้างจริง

| ไฟล์ | หน้าที่ |
|---|---|
| `README.md` | เอกสารแนะนำและวิธีติดตั้ง |
| `index.html` | หน้าเว็บ Pixel Studio |
| `sketch_oct16a.ino` | โค้ด Arduino สำหรับ Wi-Fi, MQTT และ LED Matrix |

ชื่อไฟล์และตำแหน่งอาจเปลี่ยนแปลงตามการจัดระเบียบ Repository

---

## 🚀 Installation

### Step 1 — Clone Repository

เปิด Terminal หรือ Command Prompt:

```bash
git clone https://github.com/Kmitl68070131/DRAW-ON-UNO-R4-WIFI.git
cd DRAW-ON-UNO-R4-WIFI
```

### Step 2 — เปิดหน้าเว็บ

เปิดไฟล์ `Arduino/UNO_Pixel_Studio/index.html` ในเบราว์เซอร์ หรือเผยแพร่ไฟล์เว็บผ่าน Static Hosting

หากหน้าเว็บเชื่อมต่อ MQTT Broker ผ่าน WebSocket ต้องตรวจสอบ URL, Port และ Path ให้ตรงกับ Broker ที่เลือก

### Step 3 — ติดตั้ง Arduino IDE และไลบรารี

1. ติดตั้ง Arduino IDE
2. ติดตั้ง Arduino UNO R4 Boards
3. ติดตั้งไลบรารี MQTT ที่ตรงกับ Header ในโค้ด
4. ตรวจสอบไลบรารี LED Matrix
5. เลือกบอร์ด `Arduino UNO R4 WiFi`

### Step 4 — ตั้งค่าเครือข่าย

แก้ SSID และ Password ในไฟล์ Arduino ให้ตรงกับเครือข่ายที่จะใช้

```cpp
const char WIFI_SSID[] = "YOUR_WIFI_NAME";
const char WIFI_PASSWORD[] = "YOUR_WIFI_PASSWORD";
```

อย่าใส่ข้อมูล Wi-Fi จริงใน Repository สาธารณะ

### Step 5 — Upload

1. ต่อบอร์ดด้วย USB-C
2. เลือกพอร์ต COM ให้ถูกต้อง
3. กด Verify
4. กด Upload
5. เปิด Serial Monitor
6. ตรวจสอบการเชื่อมต่อ Wi-Fi และ MQTT

หากโค้ดกำหนด `Serial.begin(115200);` ให้ตั้ง Baud Rate ใน Serial Monitor เป็น `115200` ด้วย

---

## 🔌 ตั้งค่า Arduino UNO R4 WiFi

### Wi-Fi Configuration

```cpp
const char WIFI_SSID[] = "YOUR_WIFI_NAME";
const char WIFI_PASSWORD[] = "YOUR_WIFI_PASSWORD";
```

แก้เฉพาะค่าที่อยู่ภายในเครื่องหมายคำพูด โดยไม่ต้องแก้ชื่อ Constant หากโค้ดใช้ชื่อนี้อยู่แล้ว

บอร์ด UNO R4 WiFi รองรับ Wi-Fi ย่าน 2.4 GHz เครือข่ายที่ต้องล็อกอินผ่าน Captive Portal หรือใช้ระบบยืนยันตัวตนเฉพาะองค์กรอาจไม่เหมาะกับการเชื่อมต่อของบอร์ด

### MQTT Broker Configuration

ตัวอย่าง Host ที่ใช้ในแนวทางการพัฒนา:

```text
broker.hivemq.com
```

เว็บอาจใช้ WebSocket Endpoint:

```text
wss://broker.hivemq.com:8884/mqtt
```

ส่วน Arduino ใช้การเชื่อมต่อ MQTT ผ่าน TCP ตาม Port ที่ Broker ให้บริการ

ตรวจสอบการตั้งค่าปัจจุบันของ Broker ก่อนใช้งานจริง เนื่องจาก Port, TLS และการยืนยันตัวตนอาจแตกต่างกันตามบริการ

### LED Matrix

Arduino UNO R4 WiFi มี LED Matrix ในตัวขนาด 12 × 8 พิกเซล รวม 96 จุด

```text
Width  = 12 pixels
Height = 8 pixels
Total  = 96 pixels
```

หากเปลี่ยนขนาดภาพบนเว็บ ต้องปรับการแปลงข้อมูลและการแสดงผลให้สอดคล้องกับขนาดจอและ API ที่ใช้

---

## ⚙️ MQTT Configuration

### Topic Configuration

ตัวอย่างการกำหนดชื่อ Topic:

```cpp
const char TOPIC_SEND[] =
    "it68070131/arduino-uno-r4/send";

const char TOPIC_RECEIVE[] =
    "it68070131/arduino-uno-r4/receive";

const char TOPIC_STATUS[] =
    "it68070131/arduino-uno-r4/status";

const char TOPIC_STATE[] =
    "it68070131/arduino-uno-r4/state";
```

ตัวอย่างนี้เป็นแนวทางการตั้งชื่อ หากโค้ดเดิมใช้ชื่อ Constant หรือ Topic แตกต่างกัน ให้ปรับให้ตรงกับโค้ดจริง

### MQTT Retain

การ Publish สถานะภาพล่าสุดควรเปิด Retain:

```cpp
mqtt.publish(
    "it68070131/arduino-uno-r4/state",
    payload,
    true,
    1
);
```

ตัวอย่างนี้เป็นรูปแบบ API ที่ใช้ได้กับไลบรารี MQTTClient บางเวอร์ชัน โปรดตรวจสอบ Signature ของไลบรารีที่ติดตั้งก่อนนำไปใช้

ความหมายของพารามิเตอร์ในตัวอย่าง:

- Topic: ช่องทางจัดเก็บสถานะภาพ
- Payload: ข้อมูลภาพล่าสุด
- Retain: `true`
- QoS: `1`

การเปิด Retain ไม่ได้หมายความว่าข้อมูลจะถูกบันทึกลงฐานข้อมูลถาวร และการรับประกันการส่งข้อความขึ้นอยู่กับ QoS, Broker และการเชื่อมต่อด้วย

### WebSocket

เว็บเบราว์เซอร์ไม่สามารถเชื่อมต่อ MQTT ผ่าน TCP Socket ปกติโดยตรง ต้องใช้ MQTT over WebSocket

หากเว็บไซต์ให้บริการผ่าน HTTPS ควรใช้ `wss://` สำหรับ WebSocket ที่รองรับ TLS

---

## 🌐 Deploy เว็บไซต์

### GitHub Pages

1. เปิด Repository
2. เข้า Settings → Pages
3. เลือก Source และ Branch ตามโครงสร้างโปรเจกต์
4. เลือกโฟลเดอร์ที่เก็บไฟล์เว็บ
5. กด Save
6. เปิด URL ที่ GitHub Pages แจ้ง

เอกสาร: https://docs.github.com/pages

**ข้อควรระวัง:** หากหน้าเว็บอยู่ในโฟลเดอร์ `Arduino/UNO_Pixel_Studio/` ต้องตั้งค่าแหล่งเผยแพร่หรือพาธให้ถูกต้อง ไม่ใช่สมมติว่า `index.html` อยู่ที่ Root ของ Repository

### Vercel หรือ Static Hosting อื่น

สามารถนำไฟล์ HTML, CSS และ JavaScript ไปเผยแพร่ผ่านบริการ Static Hosting ได้

ก่อนใช้งานให้ตรวจสอบว่า:

- หน้าเว็บเปิดผ่าน HTTPS ได้
- MQTT WebSocket Endpoint เข้าถึงได้
- Topic ตรงกับโค้ด Arduino
- Broker อนุญาตการเชื่อมต่อจาก Client ที่ต้องการ
- ไม่มีรหัสผ่านหรือ Secret ฝังอยู่ในไฟล์เว็บสาธารณะ

การเผยแพร่หน้าเว็บไม่ได้ทำให้บอร์ดออนไลน์เอง บอร์ดต้องเปิดเครื่อง เชื่อมต่อ Wi-Fi และเชื่อมต่อ Broker สำเร็จ

---

## 🧪 Testing Checklist

### Hardware

- [ ] บอร์ดอัปโหลดโปรแกรมได้
- [ ] Wi-Fi เชื่อมต่อสำเร็จ
- [ ] MQTT เชื่อมต่อสำเร็จ
- [ ] LED Matrix แสดงภาพได้
- [ ] บอร์ดรับภาพใหม่ได้โดยไม่ต้องรีสตาร์ต

### Web

- [ ] หน้าเว็บเปิดได้
- [ ] Pixel Editor ทำงาน
- [ ] เว็บเชื่อมต่อ MQTT ผ่าน WebSocket ได้
- [ ] การส่งภาพใช้ Topic ที่ถูกต้อง
- [ ] เว็บรับสถานะภาพจาก Broker ได้

### Shared State

- [ ] สถานะภาพล่าสุดถูก Publish ด้วย Retain
- [ ] เว็บที่เปิดใหม่ได้รับ Retained Message
- [ ] ผู้ใช้หลายหน้าต่างเห็นภาพที่ซิงก์กัน
- [ ] บอร์ดแสดงภาพตามสถานะที่ระบบกำหนด
- [ ] เมื่อเชื่อมต่อใหม่ ระบบสามารถเรียกคืนภาพล่าสุดได้
- [ ] การส่งข้อมูลผิดรูปแบบไม่ทำให้ระบบแสดงภาพผิดพลาด

### Network Recovery

- [ ] เมื่อตัด Wi-Fi มีการแสดงสถานะการเชื่อมต่อที่เหมาะสม
- [ ] เมื่อ Broker กลับมาใช้งาน ระบบเชื่อมต่อใหม่ได้
- [ ] หลังเชื่อมต่อใหม่มีการ Subscribe Topic อีกครั้ง
- [ ] ภาพล่าสุดยังถูกต้องหลังการเชื่อมต่อกลับมา

---

## 🛠️ Troubleshooting

### ❌ Wi-Fi ไม่เชื่อมต่อ

ตรวจสอบ:

1. SSID และ Password
2. เครือข่ายรองรับ 2.4 GHz หรือไม่
3. เครือข่ายต้องล็อกอินผ่านเว็บหรือไม่
4. สถานะจาก `WiFi.status()`
5. การตั้งค่า Baud Rate ใน Serial Monitor

### ❌ MQTT Error

ตรวจสอบ:

1. Wi-Fi เชื่อมต่อสำเร็จหรือยัง
2. Host และ Port ถูกต้องหรือไม่
3. Broker รองรับ TCP หรือ WebSocket ตามที่ Client ใช้หรือไม่
4. ต้องใช้ TLS หรือ Authentication หรือไม่
5. Topic ตรงกันทั้งเว็บและบอร์ดหรือไม่

รหัส Error มีความหมายแตกต่างกันตามไลบรารี จึงควรตรวจสอบเอกสารของไลบรารีที่ใช้ประกอบ

### ❌ เว็บเปิดได้ แต่บอร์ดไม่เปลี่ยนภาพ

ตรวจสอบ:

- เว็บ Publish ไปยัง Topic ที่บอร์ด Subscribe หรือไม่
- Payload มีข้อมูลครบ 96 พิกเซลหรือไม่
- รูปแบบ JSON ตรงกับโค้ด Arduino หรือไม่
- Callback MQTT ถูกเรียกหรือไม่
- โค้ดเรียก API LED Matrix ถูกต้องหรือไม่

### ❌ ผู้ใช้ใหม่ไม่เห็นภาพล่าสุด

ตรวจสอบ:

- เว็บ Subscribe Topic `/state` หรือ Topic ที่กำหนดไว้จริงหรือไม่
- Broker มี Retained Message อยู่หรือไม่
- Publisher ตั้งค่า Retain เป็น `true` หรือไม่
- Payload ที่เก็บไว้ตรงกับรูปแบบที่เว็บอ่านหรือไม่
- มีการลบ Retained Message โดยไม่ตั้งใจหรือไม่

### ❌ ผู้ใช้หลายคนเห็นภาพไม่ตรงกัน

ตรวจสอบ:

- ทุก Client ใช้ Broker เดียวกันหรือไม่
- ทุก Client Subscribe Topic สถานะเดียวกันหรือไม่
- มีการ Publish ภาพจากหลาย Client พร้อมกันหรือไม่
- ระบบมีการจัดลำดับข้อความหรือจัดการสถานะที่ขัดแย้งกันหรือไม่
- เว็บอัปเดตภาพจากสถานะที่ได้รับจริง หรืออัปเดตจากภาพในเครื่องตัวเองอย่างเดียว

### ❌ HTTPS แต่ MQTT WebSocket เชื่อมต่อไม่ได้

ตรวจสอบว่า URL ใช้ `wss://` และ Broker รองรับ WebSocket Endpoint นั้นจริง รวมถึง Port และ Path ถูกต้อง

---

## 🔐 Security

### Public MQTT Broker

การใช้ Public Broker เหมาะสำหรับการทดลองและต้นแบบ แต่ไม่ควรถือว่า Topic เป็นส่วนตัว

บุคคลอื่นอาจเข้าถึง Topic ที่คาดเดาได้และส่งข้อความเข้ามาเขียนทับสถานะภาพได้ ขึ้นอยู่กับนโยบายของ Broker

ข้อแนะนำ:

- ใช้ Broker ที่รองรับ Authentication และ ACL สำหรับการใช้งานจริง
- ใช้ TLS เมื่อรองรับ
- แยก Topic ตามอุปกรณ์หรือห้องเมื่อจำเป็น
- ตรวจสอบขนาดและรูปแบบ Payload
- จำกัดความถี่ในการ Publish
- ห้ามฝัง Secret ที่ต้องปกปิดไว้ใน JavaScript สาธารณะ

### Wi-Fi Credentials

อย่าเผยแพร่ Wi-Fi Password จริงใน Repository

หากข้อมูลรับรองถูกเผยแพร่ไปแล้ว ควรเปลี่ยนรหัสผ่าน และตรวจสอบ Git History เพราะการลบออกจากไฟล์ล่าสุดไม่ได้ทำให้ข้อมูลหายจากประวัติเวอร์ชันเดิม

---

## ❓ FAQ

### ต้องใช้ Arduino UNO R4 WiFi หรือไม่?

โค้ดที่ใช้ `WiFiS3` และ LED Matrix ในตัวออกแบบมาสำหรับ UNO R4 WiFi หากใช้บอร์ดอื่นต้องปรับโค้ดตามฮาร์ดแวร์

### ต้องต่อ LED Matrix เพิ่มหรือไม่?

ไม่จำเป็น เพราะ UNO R4 WiFi มี LED Matrix ขนาด 12 × 8 อยู่แล้ว

### เปิดเว็บจากหลายเครื่องได้หรือไม่?

ได้ หากเว็บสามารถเข้าถึง MQTT Broker เดียวกัน และมีการ Subscribe/Publish Topic ตรงกัน

### เปิดเว็บใหม่แล้วภาพจะกลับมาเองหรือไม่?

ได้เมื่อมี Retained Message ของสถานะภาพล่าสุดอยู่ใน Broker และเว็บ Subscribe Topic นั้นได้ถูกต้อง หากไม่มี Retained Message ระบบต้องมีกลไกเรียกคืนสถานะอื่นเพิ่มเติม

### ภาพจะหายเมื่อปิดบอร์ดหรือไม่?

Retained Message ที่ Broker เก็บไว้สามารถคงอยู่ได้แม้บอร์ดปิด แต่ขึ้นอยู่กับการตั้งค่าและการเก็บข้อมูลของ Broker ทั้งนี้ Retained Message ไม่ใช่ฐานข้อมูลถาวรที่รับประกันการเก็บข้อมูล

### ใช้งานผ่านอินเทอร์เน็ตได้หรือไม่?

ได้ หากเว็บไซต์และ MQTT Broker เข้าถึงได้จากอินเทอร์เน็ต และบอร์ดสามารถเชื่อมต่อ Broker ได้สำเร็จ

### ถ้ามีคนวาดพร้อมกันจะเกิดอะไรขึ้น?

ระบบอาจมีการเขียนทับสถานะภาพ หากไม่ได้กำหนดการจัดลำดับหรือการจัดการความขัดแย้งไว้

---

## 🗺️ Roadmap

แนวทางที่สามารถพัฒนาต่อได้:

- [ ] 🖼️ บันทึกและโหลดภาพหลายชุด
- [ ] ↩️ Undo / Redo
- [ ] 🎨 เพิ่มสี เครื่องมือ และชุดภาพตัวอย่าง
- [ ] 👥 ระบบห้องวาดภาพหลายห้อง
- [ ] 🏷️ Revision Number สำหรับจัดลำดับสถานะ
- [ ] 📶 Dashboard แสดงสถานะออนไลน์และเวลา Heartbeat ล่าสุด
- [ ] 🔐 ระบบสิทธิ์การควบคุมและ Authentication
- [ ] 📱 ปรับปรุงการใช้งานบนโทรศัพท์
- [ ] 🧪 เพิ่มระบบทดสอบ Payload และ Network Recovery

---

## 📖 References

- Arduino UNO R4 WiFi: https://docs.arduino.cc/hardware/uno-r4-wifi/
- Arduino IDE: https://www.arduino.cc/en/software
- Arduino Libraries: https://docs.arduino.cc/libraries/
- MQTT: https://mqtt.org/
- MQTT.js: https://github.com/mqttjs/MQTT.js
- GitHub Pages: https://docs.github.com/pages
- GitHub Repository: https://github.com/Kmitl68070131/DRAW-ON-UNO-R4-WIFI

---

## 📄 License

โปรดตรวจสอบไฟล์ License ที่มีอยู่ใน Repository ก่อนนำโค้ดไปเผยแพร่หรือใช้งานต่อ หากยังไม่มี License ไม่ควรระบุว่าโครงการใช้ MIT License หรือ License อื่นโดยไม่มีการกำหนดสิทธิ์อย่างชัดเจน

---

⭐ หากโปรเจกต์นี้เป็นประโยชน์ สามารถกด Star ให้ Repository และร่วมพัฒนาต่อได้

**Made with ❤️ using Arduino UNO R4 WiFi, MQTT, and Web Technologies**

*UNO R4 WiFi Pixel Studio — Version 2.0.0*
