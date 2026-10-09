#ifndef PINS_H
#define PINS_H

#include <Arduino.h>

// ==============================================================================
// 1. KAMERA PINLERI (Kart uzerindeki OV3660 soketi)
// ==============================================================================
#define CAM_PWDN_GPIO_NUM     -1
#define CAM_RESET_GPIO_NUM    -1
#define CAM_XCLK_GPIO_NUM     15
#define CAM_SIOD_GPIO_NUM     4
#define CAM_SIOC_GPIO_NUM     5
#define CAM_Y9_GPIO_NUM       16
#define CAM_Y8_GPIO_NUM       17
#define CAM_Y7_GPIO_NUM       18
#define CAM_Y6_GPIO_NUM       12
#define CAM_Y5_GPIO_NUM       10
#define CAM_Y4_GPIO_NUM       8
#define CAM_Y3_GPIO_NUM       9
#define CAM_Y2_GPIO_NUM       11
#define CAM_VSYNC_GPIO_NUM    6
#define CAM_HREF_GPIO_NUM     7
#define CAM_PCLK_GPIO_NUM     13

// ==============================================================================
// 2. DAHILI MICRO SD KART PINLERI (Gelistirme karti uzerindeki yuva)
// ==============================================================================
#define SD_CLK_PIN            39
#define SD_CMD_PIN            38
#define SD_DATA_PIN           40

// ==============================================================================
// 3. TDK T5848 MIKROFON PINLERI (I2S Arayuzu)  -- PCB ICIN SABIT PIN LISTESI
// ==============================================================================
// Kullanilmayan/kacinilan pinler: 0,3,45,46 (strapping), 19,20 (USB), 35-37 (PSRAM),
// 43,44 (UART), 2 ve 48 (kart uzerinde LED var), 4-13,15-18 (kamera), 38-40 (SD).
#define MIC_SCK_PIN           42  // I2S Bit Saati (CLK)
#define MIC_WS_PIN            41  // I2S Word Select (WS)
#define MIC_SD_PIN            14  // I2S Ses Veri Girisi (DATA)  (GPIO2'de LED var, kullanma)
#define MIC_WAKE_PIN          1   // Ses aktivite kesme pini (WAKE)
#define MIC_THSEL_PIN         21  // AAD konfigurasyonu icin (su an yazilimda kullanilmiyor, PCB'de bagli olsun)
// Yedek bos pin: GPIO47

// ==============================================================================
// 4. SISTEM PARAMETRELERI
// ==============================================================================
#define FOTO_ARALIK_SN        30    // Periyodik fotograf araligi (saniye)
#define SES_ESIK_DEGERI       150   // Ses algilama RMS esigi (MIC_DEBUG_RMS ile kalibre et)
// T5848 modlari SCK frekansina gore secilir (datasheet DS-000479):
//   Low Power : 600-800 kHz  (fs = SCK/48, 16-bit veri)
//   High Qual.: 2.0-3.7 MHz  (fs = SCK/64, 24-bit veri)
// ESP32 32-bit slot kullanir -> SCK = fs x 64. 16 kHz => 1.024 MHz = HICBIR MODA GIRMEZ.
// 48 kHz => 3.072 MHz = High Quality Mode (datasheet ornegi). (Alt sinir: fs >= 31.25 kHz)
#define AUDIO_SAMPLE_RATE     48000

// --- Ses isleme ayarlari ---
// HQM'de duyarlilik -37 dBFS (94 dB SPL). 24-bit veri 32-bit slotun ustunde.
// >>16 = tam olcek (cok sessiz), >>11 = +30 dB kazanc (saturasyonlu).
// Konusma ~65 dB SPL'de RMS ~370 civari (tahmin). Cok sessiz/gurultulu ise 10-12 arasi dene.
#define MIC_SHIFT             11
// WAKE (AAD) pini yalnizca T5848 AAD modu konfigure edildiyse anlamli.
// Konfigurasyon yazilana kadar 0 kalsin (sadece RMS esigi kullanilir).
#define USE_WAKE_PIN          0
// 1 ise saniyede bir RMS degeri basilir -> SES_ESIK_DEGERI'ni kalibre etmek icin.
#define MIC_DEBUG_RMS         1
#define SES_SUSME_MS          2000   // Ses kesildikten sonra kayit suresi
#define SES_MAX_KAYIT_MS      10000  // Tek kaydin ust siniri

#endif // PINS_H
