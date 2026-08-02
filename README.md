<p align="center">
  <img src="./assets/img/logo.png" width="120">
</p>

<h1 align="center">Controller IDE Source Code</h1>

<p align="center">
  <img src="https://img.shields.io/badge/C-00599C?style=for-the-badge&logo=c&logoColor=white" alt="C"/>
  <img src="https://img.shields.io/badge/C%2B%2B-00599C?style=for-the-badge&logo=cplusplus&logoColor=white" alt="C++"/>
</p>

<p align="center">
  Firmware &amp; source code untuk perangkat IoT berbasis <b>ESP32</b>, terintegrasi dengan stack
  self-hosted (MQTT, Redis, Grafana, Prometheus) untuk monitoring jaringan dan otomasi.
</p>

<p align="center">
  <img src="https://img.shields.io/badge/platform-ESP32-blue?style=flat-square" alt="Platform"/>
  <img src="https://img.shields.io/badge/framework-Arduino%20%7C%20ESP--IDF-orange?style=flat-square" alt="Framework"/>
  <img src="https://img.shields.io/badge/license-MIT-green?style=flat-square" alt="License"/>
  <img src="https://img.shields.io/badge/status-active-success?style=flat-square" alt="Status"/>
</p>

---

## 📖 Daftar Isi

- [Tentang Proyek](#-tentang-proyek)
- [Fitur](#-fitur)
- [Arsitektur Sistem](#-arsitektur-sistem)
- [Perangkat Keras](#-perangkat-keras)
- [Struktur Proyek](#-struktur-proyek)
- [Instalasi & Build](#-instalasi--build)
- [Konfigurasi](#-konfigurasi)
- [Komunikasi Data (MQTT/HTTP)](#-komunikasi-data-mqtthttp)
- [Integrasi dengan Stack Monitoring](#-integrasi-dengan-stack-monitoring)
- [Troubleshooting](#-troubleshooting)
- [Kontribusi](#-kontribusi)
- [Lisensi](#-lisensi)

---

## 📌 Tentang Proyek

Repositori ini berisi **source code firmware ESP32** yang digunakan sebagai node sensor / kontroler IoT dalam jaringan homelab/infrastruktur pribadi. Firmware ditulis dalam **C/C++** menggunakan **Arduino Framework** (atau **ESP-IDF**, tergantung modul), dan dirancang untuk:

- Mengumpulkan data sensor secara real-time
- Mengirim data ke broker MQTT / API backend
- Diawasi melalui dashboard **Grafana** dan **Prometheus**
- Dikelola dari jaringan yang diamankan dengan **WireGuard** dan router **MikroTik/Cisco**

Proyek ini merupakan bagian dari ekosistem infrastruktur self-hosted yang lebih besar (Proxmox, Docker, Nginx, dll).

---

## ✨ Fitur

- 📡 Pembacaan sensor (suhu, kelembaban, tegangan, gerak, dll — sesuaikan dengan modul yang dipakai)
- 🔄 Pengiriman data otomatis via **MQTT** dan/atau **HTTP REST API**
- 🔐 Koneksi WiFi dengan fallback **Access Point (AP) mode** untuk konfigurasi awal
- ⚙️ Konfigurasi runtime tanpa perlu re-flash (disimpan di NVS/EEPROM)
- 🔁 OTA (Over-The-Air) firmware update
- 📊 Kompatibel dengan Prometheus exporter format & Grafana dashboard
- 🧠 Watchdog timer untuk stabilitas perangkat 24/7
- 🌙 Deep sleep mode untuk efisiensi daya (opsional, untuk node berbasis baterai)

---

## 🏗️ Arsitektur Sistem

```
┌─────────────┐     WiFi / MQTT     ┌──────────────┐     Docker Network     ┌───────────────┐
│  ESP32 Node │ ──────────────────► │  MQTT Broker │ ─────────────────────► │  Node.js API   │
│  (C/C++)    │                     │ (Mosquitto)  │                        │   Backend      │
└─────────────┘                     └──────────────┘                        └───────┬───────┘
                                                                                       │
                                                                                       ▼
                                                                              ┌─────────────────┐
                                                                              │      Redis       │
                                                                              │  (cache/state)    │
                                                                              └────────┬────────┘
                                                                                       │
                                                            ┌──────────────────────────┼──────────────────────────┐
                                                            ▼                          ▼                          ▼
                                                     ┌─────────────┐          ┌──────────────┐            ┌──────────────┐
                                                     │  Prometheus │          │   Grafana    │            │    Nginx      │
                                                     │  (metrics)  │          │ (dashboard)  │            │ (reverse proxy)│
                                                     └─────────────┘          └──────────────┘            └──────────────┘
```

Seluruh traffic internal diamankan melalui **WireGuard VPN**, dengan routing dikelola oleh **MikroTik** dan **Cisco** di sisi jaringan, serta di-host di atas **Proxmox** menggunakan container **Docker**.

---

## 🔧 Perangkat Keras

| Komponen             | Keterangan                                  |
|-----------------------|----------------------------------------------|
| Board                 | ESP32 DevKit V1 / ESP32-WROOM-32              |
| Sensor                | (sesuaikan: DHT22, DS18B20, PZEM-004T, dll)   |
| Power Supply          | 5V USB / Baterai Li-Ion (opsional deep sleep) |
| Konektivitas          | WiFi 2.4GHz                                   |

> 💡 Sesuaikan tabel ini dengan daftar sensor & modul aktual yang kamu gunakan di proyek.

---

## 📂 Struktur Proyek

```
esp32-iot-project/
├── src/
│   ├── main.cpp              # Entry point program
│   ├── wifi_manager.cpp/.h    # Manajemen koneksi WiFi
│   ├── mqtt_client.cpp/.h     # Handler komunikasi MQTT
│   ├── sensors/               # Modul pembacaan sensor
│   └── config.h               # Konfigurasi default (WiFi, MQTT, dll)
├── include/                   # Header tambahan
├── lib/                        # Library eksternal/custom
├── data/                       # File untuk SPIFFS/LittleFS (jika ada)
├── platformio.ini              # Konfigurasi build PlatformIO
└── README.md
```

---

## ⚙️ Instalasi & Build

### Menggunakan PlatformIO (disarankan)

```bash
# Clone repository
git clone https://github.com/username/esp32-iot-project.git
cd esp32-iot-project

# Build firmware
pio run

# Upload ke board ESP32
pio run --target upload

# Monitor serial output
pio device monitor
```

### Menggunakan Arduino IDE

1. Install **ESP32 Board Package** melalui Board Manager.
2. Install library yang dibutuhkan (lihat `lib/` atau `platformio.ini` untuk daftar dependensi).
3. Buka `src/main.cpp`, pilih board **ESP32 Dev Module**.
4. Klik **Upload**.

---

## 🔐 Konfigurasi

Salin file contoh konfigurasi lalu sesuaikan:

```bash
cp src/config.example.h src/config.h
```

Isi parameter berikut sesuai kebutuhan:

```cpp
#define WIFI_SSID       "nama_wifi"
#define WIFI_PASSWORD   "password_wifi"
#define MQTT_HOST       "192.168.1.10"
#define MQTT_PORT       1883
#define MQTT_USER       "mqtt_user"
#define MQTT_PASSWORD   "mqtt_pass"
#define DEVICE_ID       "esp32-node-01"
```

---

## 📡 Komunikasi Data (MQTT/HTTP)

Contoh topic MQTT yang digunakan:

| Topic                          | Deskripsi                       |
|---------------------------------|----------------------------------|
| `iot/esp32-node-01/sensor`      | Data sensor (JSON payload)       |
| `iot/esp32-node-01/status`      | Status online/offline (LWT)      |
| `iot/esp32-node-01/cmd`         | Perintah dari server ke device    |

Contoh payload JSON:

```json
{
  "device_id": "esp32-node-01",
  "temperature": 28.5,
  "humidity": 65.2,
  "timestamp": 1735689600
}
```

---

## 📊 Integrasi dengan Stack Monitoring

- Data dari MQTT diteruskan oleh backend **Node.js** ke **Redis** sebagai cache state terbaru.
- **Prometheus** melakukan scraping metrik melalui exporter kustom.
- **Grafana** menampilkan dashboard visual dari data historis.
- **Nginx**/**Traefik** bertindak sebagai reverse proxy untuk dashboard & API.

---

## 🛠️ Troubleshooting

| Masalah                          | Solusi                                                        |
|-----------------------------------|----------------------------------------------------------------|
| ESP32 gagal konek WiFi             | Cek kredensial di `config.h`, pastikan sinyal 2.4GHz tersedia   |
| Data tidak sampai ke broker MQTT   | Cek firewall MikroTik/Cisco, pastikan port 1883 terbuka         |
| Board tidak terdeteksi saat upload | Cek driver USB-to-Serial (CP2102/CH340), tekan tombol BOOT      |

---

## 🤝 Kontribusi

Kontribusi sangat terbuka! Silakan buat *pull request* atau buka *issue* untuk melaporkan bug maupun mengusulkan fitur baru.

---

## 📄 Lisensi

Proyek ini dirilis di bawah lisensi **MIT** — bebas digunakan, dimodifikasi, dan didistribusikan dengan tetap mencantumkan atribusi.