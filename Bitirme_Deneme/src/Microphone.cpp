#include "Microphone.h"
#include "pins.h"
#include "SDCard.h"
#include <driver/i2s.h>
#include <math.h>

bool Microphone::m_ready = false;

bool Microphone::begin() {
    Serial.println("[MIKROFON] I2S mikrofon (T5848) baslatiliyor...");

    pinMode(MIC_WAKE_PIN, INPUT_PULLDOWN);

    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
        .sample_rate = AUDIO_SAMPLE_RATE,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT, // T5848 24-bit sesi 32-bit slot icinde gonderir
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 4,
        .dma_buf_len = 512,
        .use_apll = false
    };

    i2s_pin_config_t pin_config = {
        .bck_io_num = MIC_SCK_PIN,
        .ws_io_num = MIC_WS_PIN,
        .data_out_num = I2S_PIN_NO_CHANGE,
        .data_in_num = MIC_SD_PIN
    };

    if (i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL) != ESP_OK) {
        Serial.println("[MIKROFON] HATA: I2S surucusu baslatilamadi!");
        m_ready = false;
        return false;
    }

    if (i2s_set_pin(I2S_NUM_0, &pin_config) != ESP_OK) {
        Serial.println("[MIKROFON] HATA: I2S pinleri ayarlanamadi!");
        m_ready = false;
        return false;
    }

    m_ready = true;
    Serial.println("[MIKROFON] I2S basariyla hazirlandi.");
    return true;
}

int Microphone::hesaplaRMS(const int32_t *rawSamples, int ornekSayisi) {
    if (ornekSayisi <= 0) return 0;
    int64_t kareToplami = 0;

    for (int i = 0; i < ornekSayisi; i++) {
        int16_t sample16 = (int16_t)(rawSamples[i] >> 14); // 16-bit PCM genlige cevir
        kareToplami += ((int32_t)sample16 * (int32_t)sample16);
    }
    return (int)sqrt(kareToplami / ornekSayisi);
}

void Microphone::update(int &sesSayaci) {
    if (!m_ready) return;

    int32_t raw_samples[256];
    size_t okunanBayt = 0;

    i2s_read(I2S_NUM_0, (char*)raw_samples, sizeof(raw_samples), &okunanBayt, pdMS_TO_TICKS(50));
    if (okunanBayt == 0) return;

    int ornekSayisi = okunanBayt / sizeof(int32_t);
    int rms = hesaplaRMS(raw_samples, ornekSayisi);

    // Donanimsal WAKE pini aktif mi VEYA yazilimsal ses esigi asildi mi?
    bool sesAlgilandi = (rms > SES_ESIK_DEGERI) || (digitalRead(MIC_WAKE_PIN) == HIGH);

    if (sesAlgilandi) {
        Serial.printf("[MIKROFON] Ses algilandi! (RMS: %d) Kayit basliyor...\n", rms);

        int kaydedilenNo = sesSayaci++;
        File sesDosyasi = SDCard::createAudioFile(kaydedilenNo);
        if (!sesDosyasi) {
            Serial.println("[MIKROFON] HATA: SD karta dosya acilamadi!");
            return;
        }

        int toplamSesBayt = 0;
        unsigned long kayitBaslangic = millis();
        unsigned long sonSesZamani = millis();

        // Konusma devam ettigi surece (veya max 10 saniye) kaydet
        while (millis() - sonSesZamani < 2000 && millis() - kayitBaslangic < 10000) {
            i2s_read(I2S_NUM_0, (char*)raw_samples, sizeof(raw_samples), &okunanBayt, pdMS_TO_TICKS(50));
            int n = okunanBayt / sizeof(int32_t);

            int16_t pcm16[256];
            for (int i = 0; i < n; i++) {
                pcm16[i] = (int16_t)(raw_samples[i] >> 14);
            }

            int anlikRMS = hesaplaRMS(raw_samples, n);
            if (anlikRMS > SES_ESIK_DEGERI) {
                sonSesZamani = millis(); // Ses devam ediyor
            }

            int yazilacakBayt = n * sizeof(int16_t);
            sesDosyasi.write((uint8_t*)pcm16, yazilacakBayt);
            toplamSesBayt += yazilacakBayt;
        }

        // WAV dosyasini finalize et
        SDCard::writeWavHeader(sesDosyasi, toplamSesBayt);
        sesDosyasi.close();

        Serial.printf("[MIKROFON] Kayit tamamlandi: /ses_%d.wav (%d bayt)\n", kaydedilenNo, toplamSesBayt);
    }
}
