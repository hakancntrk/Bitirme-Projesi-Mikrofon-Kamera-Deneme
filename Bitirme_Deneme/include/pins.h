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
// 3. TDK T5848 MIKROFON PINLERI (I2S Arayuzu)
// ==============================================================================
#define MIC_SCK_PIN           42  // I2S Bit Saati (CLK)
#define MIC_WS_PIN            41  // I2S Word Select (WS)
#define MIC_SD_PIN            2   // I2S Ses Veri Girisi (DATA)
#define MIC_WAKE_PIN          1   // Ses aktivite kesme pini (WAKE)

// ==============================================================================
// 4. SISTEM PARAMETRELERI
// ==============================================================================
#define FOTO_ARALIK_SN        30    // Periyodik fotograf araligi (saniye)
#define SES_ESIK_DEGERI       600   // Ses algilama RMS esigi
#define AUDIO_SAMPLE_RATE     16000 // Ses ornekleme frekansi (16 kHz)

#endif // PINS_H
