# 🇹🇷 ESP32 Web-Controlled Sound System with LCD Display & National Anthem

🎶 A Wi-Fi-based ESP32 project that plays various melodies, including the **Turkish National Anthem (İstiklal Marşı)**, controlled entirely via a browser interface. Features an LCD for status display and **amplified sound using a transistor circuit**.

---

## 🎯 Project Highlights

- ✅ ESP32-based embedded system
- 📡 Web interface over Wi-Fi Access Point
- 🔊 Plays:
  - 🇹🇷 İstiklal Marşı
  - 🎵 Melody 1
  - 🎶 Melody 2
  - 🚨 Siren
  - 🌊 Sweep
  - 🟢 ON / 🔴 OFF / 🔁 RESET sounds
- 🖥️ LCD screen for real-time status messages
- 💡 Transistor-boosted sound output
- 🎨 Clean and responsive web design





## 🔧 Hardware Components

- ✅ ESP32 Dev Board  
- ✅ LCD Display (1602 or I2C)  
- ✅ Passive Buzzer  
- ✅ NPN Transistor (e.g., 2N2222 / BC547)  
- ✅ Resistors, Breadboard, Jumper Wires  
- ✅ Optional: Power bank or adapter

---

## 🖥️ Web Interface

- HTML-based interactive control panel
- Buttons for each sound function
- Clean layout using inline CSS
- Hosted on ESP32 (no internet needed)

---

## 📐 Circuit Diagram (Buzzer Amplification with Transistor)

```text
ESP32 GPIO (e.g., D15) --> [220Ω] --> Transistor Base (B)
Transistor Collector (C) --> Buzzer (+)
Buzzer (–) --> GND
Transistor Emitter (E) --> GND
