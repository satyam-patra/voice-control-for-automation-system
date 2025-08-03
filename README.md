# Voice Controlled Home Automation System

> A smart system that allows users to control home appliances through voice commands using a custom-built web interface and ESP32 microcontroller.

---

## 🚩 Problem Statement

Many individuals, especially the elderly or physically challenged, face difficulty in controlling home appliances manually. Even for others, switching devices on/off can be repetitive or inconvenient.

---

## 💡 Our Solution

We developed a **Voice Controlled Home Automation System** that allows users to control lights, fans, door locks, alarms, and monitor environmental conditions like temperature, humidity, and soil moisture — all using natural language voice commands via a web interface and mobile support.

---

## 🔧 Components Used

- **ESP32 Dev Module**
- **RGB LED**
- **Relay Module (2-Channel)**
- **DHT11 Sensor**
- **Soil Moisture Sensor**
- **IR Sensor**
- **Buzzer**
- **Servo Motor (for door)**
- **Stepper Motor (optional)**
- **Custom-built Node.js Backend**
- **Custom Web Interface (HTML/CSS/JS)**

---

## 📊 System Architecture

### Block Diagram
```
User (Voice Command) → Web Interface → Node.js Server → ESP32 → Appliances
```

### Flowchart

1. User speaks: “Turn on the light” or “Set alarm for 7:00 AM”
2. Speech converted to text and sent to backend
3. Node.js server relays command to ESP32
4. ESP32 parses and executes the command
5. Feedback (status or sensor values) sent back to the frontend

---

## ⚙️ Features

- ✅ **Voice-Controlled Light** – On/off + RGB color change
- 🌡️ **Temperature & Humidity Monitoring**
- 🌱 **Soil Moisture Detection**
- 🔒 **Night Security Mode** – Alerts when motion is detected
- 🚨 **Emergency Help Mode** – Manual activation of alert via voice
- 🔔 **Wake-Up Alarm System** – Set alarms using voice
- 🚪 **Voice-Controlled Door Lock** – Open/close via servo
- 💨 **Fan Control via Relay** – Toggle fan through voice
- 🧠 **Multiple Natural Commands** – Example: "Turn on light", "Enable night security", "I need help"

---

## 🗣️ Sample Voice Commands

| Operation | Example Commands |
|----------|------------------|
| Light | "Turn on light", "Change color to red" |
| Fan | "Start fan", "Stop fan" |
| Door | "Open door", "Close door" |
| Alarm | "Set alarm for 7:30 AM", "Cancel alarm" |
| Emergency | "Help me", "I need help", "Cancel help" |
| Security | "Enable night security", "Disable night security" |
| Sensors | "Check temperature", "Check humidity", "Check soil moisture" |

---

## 🚀 How to Run

1. Flash the ESP32 code (`.ino`) using Arduino IDE
2. Start the **Node.js server** with:
   ```bash
   node server.js
   ```
3. Open the **web interface** in your browser or mobile (speech input enabled)
4. Connect to the same Wi-Fi network
5. Start giving voice commands!

---

## 📦 Folder Structure

## 📦 Folder Structure

```
Voice_Controled_Home_Automation_System/
├── ESP_Code_And_Connation/        # Contains ESP32 code and diagrams
│   ├── esp_code/ 
    |       |── ESP_CODE.ino    # Final ESP32 sketch (.ino)
│   ├── connection.fzz/     # Circuit diagram images
│   └── photos                # Project build and demo images
├── web/                         # Web interface and backend
│   ├── public/                  # Frontend files (index.html, style, JS)
│   ├── server.js                # Node.js backend
│   └── ...
├── README.md                    # Project documentation (this file)
└── ...
```
---

## 📌 Note

- Make sure the ESP32 is connected to Wi-Fi and the ngrok URL (or IP) is updated in both frontend and backend
- Buzzer serves for both **alarm** and **security/emergency alerts**
- Voice recognition is done via the browser's built-in `SpeechRecognizer` API (works best on Chrome/Android)

---

---

Feel free to ⭐ this repo if you like it!
