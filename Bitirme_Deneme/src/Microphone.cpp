#include "Microphone.h"
#include "pins.h"
#include "SDCard.h"
#include <driver/i2s.h>
#include <math.h>
#include <string.h>

static const int BLOCK_SAMPLES     = 256;   // Tek seferde okunan ornek sayisi
static const int WRITE_BUF_SAMPLES = 2048;  // 4 KB: SD'ye tek seferde yazilan blok

bool  Microphone::m_ready     = false;
float Microphone::m_dcPrevIn  = 0.0f;
float Microphone::m_dcPrevOut = 0.0f;

bool Microphone::begin() {
    Serial.println("[MIKROFON] I2S mikrofon (T5848) baslatiliyor...");

#if USE_WAKE_PIN
    pinMode(MIC_WAKE_PIN, INPUT_PULLDOWN);
#endif

    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
        .sample_rate = AUDIO_SAMPLE_RATE,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT, // T5848 24-bit sesi 32-bit slot icinde gonderir
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 8,      // SD yazma takilmalarina karsi ~256 ms tampon
        .dma_buf_len = 512,
        .use_apll = false
    };

    i2s_pin_config_t pin_config = {
        .mck_io_num = I2S_PIN_NO_CHANGE,   // Baslatilmazsa 0 kalir (GPIO0'da MCLK acabilir)
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

int Microphone::convertBlock(const int32_t *raw, int16_t *out, int n) {
    if (n <= 0) return 0;

    const float R = 0.995f;          // DC blocker (~13 Hz kesim @16 kHz)
    int64_t kareToplami = 0;

    for (int i = 0; i < n; i++) {
        float x = (float)(raw[i] >> MIC_SHIFT);
        float y = x - m_dcPrevIn + R * m_dcPrevOut;
        m_dcPrevIn  = x;
        m_dcPrevOut = y;

        // Tasma (wrap) yerine saturasyon
        if (y > 32767.0f)  y = 32767.0f;
        if (y < -32768.0f) y = -32768.0f;

        int16_t s = (int16_t)y;
        out[i] = s;
        kareToplami += (int32_t)s * (int32_t)s;
    }
    return (int)sqrtf((float)(kareToplami / n));
}

void Microphone::update(int &sesSayaci) {
    if (!m_ready || !SDCard::isReady()) return;

    int32_t raw[BLOCK_SAMPLES];
    int16_t pcm[BLOCK_SAMPLES];
    size_t okunanBayt = 0;

    if (i2s_read(I2S_NUM_0, (char*)raw, sizeof(raw), &okunanBayt, pdMS_TO_TICKS(50)) != ESP_OK) return;
    if (okunanBayt == 0) return;

    int n = okunanBayt / sizeof(int32_t);
    int rms = convertBlock(raw, pcm, n);

#if MIC_DEBUG_RMS
    static unsigned long sonRmsYazdirma = 0;
    if (millis() - sonRmsYazdirma >= 1000) {
        sonRmsYazdirma = millis();
        Serial.printf("[MIKROFON] RMS: %d (esik: %d)\n", rms, SES_ESIK_DEGERI);
    }
#endif

    bool sesAlgilandi = (rms > SES_ESIK_DEGERI);
#if USE_WAKE_PIN
    sesAlgilandi = sesAlgilandi || (digitalRead(MIC_WAKE_PIN) == HIGH);
#endif
    if (!sesAlgilandi) return;

    Serial.printf("[MIKROFON] Ses algilandi! (RMS: %d) Kayit basliyor...\n", rms);

    // Sayac yalnizca dosya gercekten acildiysa artar
    File sesDosyasi = SDCard::createAudioFile(sesSayaci);
    if (!sesDosyasi) {
        Serial.println("[MIKROFON] HATA: SD karta dosya acilamadi!");
        return;
    }
    int kaydedilenNo = sesSayaci++;

    // 4 KB'lik tampon: SD'ye kucuk (512 B) yazmalar yerine buyuk bloklar yazilir
    static int16_t yazmaTamponu[WRITE_BUF_SAMPLES];
    int tamponDolu = 0;
    int toplamSesBayt = 0;

    auto tamponuBosalt = [&]() {
        if (tamponDolu > 0) {
            size_t yazilan = sesDosyasi.write((uint8_t*)yazmaTamponu, tamponDolu * sizeof(int16_t));
            toplamSesBayt += (int)yazilan;
            tamponDolu = 0;
        }
    };
    auto tampona_ekle = [&](const int16_t *src, int adet) {
        if (tamponDolu + adet > WRITE_BUF_SAMPLES) tamponuBosalt();
        memcpy(&yazmaTamponu[tamponDolu], src, adet * sizeof(int16_t));
        tamponDolu += adet;
    };

    // Kaydi tetikleyen ilk blok da kayda dahil (cumlenin basi kaybolmasin)
    tampona_ekle(pcm, n);

    unsigned long kayitBaslangic = millis();
    unsigned long sonSesZamani   = millis();

    while (millis() - sonSesZamani < SES_SUSME_MS && millis() - kayitBaslangic < SES_MAX_KAYIT_MS) {
        okunanBayt = 0;
        i2s_read(I2S_NUM_0, (char*)raw, sizeof(raw), &okunanBayt, pdMS_TO_TICKS(50));
        int m = okunanBayt / sizeof(int32_t);
        if (m <= 0) continue;

        int anlikRMS = convertBlock(raw, pcm, m);
        if (anlikRMS > SES_ESIK_DEGERI) {
            sonSesZamani = millis(); // Ses devam ediyor
        }
        tampona_ekle(pcm, m);
    }

    tamponuBosalt();

    // WAV dosyasini finalize et
    SDCard::writeWavHeader(sesDosyasi, toplamSesBayt);
    sesDosyasi.close();

    Serial.printf("[MIKROFON] Kayit tamamlandi: /ses_%d.wav (%d bayt)\n", kaydedilenNo, toplamSesBayt);
}
