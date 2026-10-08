#include <Arduino.h>
#include "pins.h"
#include "SDCard.h"
#include "Camera.h"
#include "Microphone.h"

// Sayaclar ve zaman takibi
static int fotoSayaci = 1;   // setup()'ta SD'deki mevcut dosyalara gore ayarlanir
static int sesSayaci  = 1;
static unsigned long sonFotoZamani = 0;

void setup() {
    Serial.begin(115200);
    delay(1500);

    Serial.println("\n=============================================");
    Serial.println("  WhatHaveIDone: IoT Wearable Lifelogger");
    Serial.println("=============================================\n");

    // Modulleri baslat
    SDCard::begin();
    // Reset sonrasi eski dosyalarin uzerine yazmamak icin numaralandirmaya kaldigi yerden devam et
    fotoSayaci = SDCard::nextFileNumber("foto_", ".jpg");
    sesSayaci  = SDCard::nextFileNumber("ses_", ".wav");
    Serial.printf("[SISTEM] Siradaki dosyalar: foto_%d.jpg, ses_%d.wav\n", fotoSayaci, sesSayaci);
    Camera::begin();
    Microphone::begin();

    Serial.println("\n[SISTEM] Tum birimler hazir, dongu basliyor...\n");
    sonFotoZamani = millis();
}

void loop() {
    // 1. Mikrofonu dinle (Ses algilarsa otomatik SD karta kaydeder)
    Microphone::update(sesSayaci);

    // 2. Belirli araliklarla (30 saniyede bir) fotograf cek ve SD karta kaydet
    if (millis() - sonFotoZamani >= (FOTO_ARALIK_SN * 1000UL)) {
        sonFotoZamani = millis();
        Camera::takeAndSave(fotoSayaci++);
    }

    yield(); // CPU rahatlatmasi
}
