#include "SDCard.h"
#include "pins.h"

bool SDCard::m_ready = false;

bool SDCard::begin() {
    Serial.println("[SD] Dahili MicroSD kart baslatiliyor...");
    SD_MMC.setPins(SD_CLK_PIN, SD_CMD_PIN, SD_DATA_PIN);

    // true = 1-bit mod (kartin baglanti sekli)
    if (!SD_MMC.begin("/sdcard", true)) {
        Serial.println("[SD] HATA: SD kart takili degil veya baslatilamadi!");
        m_ready = false;
        return false;
    }

    uint64_t kartBoyutu = SD_MMC.cardSize() / (1024 * 1024);
    Serial.printf("[SD] Basarili! Kart Boyutu: %llu MB\n", kartBoyutu);
    m_ready = true;
    return true;
}

bool SDCard::isReady() {
    return m_ready;
}

int SDCard::nextFileNumber(const char *prefix, const char *ext) {
    if (!m_ready) return 1;

    File root = SD_MMC.open("/");
    if (!root || !root.isDirectory()) return 1;

    int maxNo = 0;
    size_t plen = strlen(prefix);
    size_t elen = strlen(ext);

    File f = root.openNextFile();
    while (f) {
        if (!f.isDirectory()) {
            const char *ad = f.name();
            if (ad[0] == '/') ad++;   // bazi surumler basta '/' dondurur
            size_t len = strlen(ad);
            if (len > plen + elen &&
                strncmp(ad, prefix, plen) == 0 &&
                strcmp(ad + len - elen, ext) == 0) {
                int no = atoi(ad + plen);
                if (no > maxNo) maxNo = no;
            }
        }
        f.close();
        f = root.openNextFile();
    }
    root.close();
    return maxNo + 1;
}

bool SDCard::savePhoto(const uint8_t *buffer, size_t size, int photoNumber) {
    if (!m_ready || !buffer || size == 0) return false;

    char dosyaAdi[32];
    snprintf(dosyaAdi, sizeof(dosyaAdi), "/foto_%d.jpg", photoNumber);

    File file = SD_MMC.open(dosyaAdi, FILE_WRITE);
    if (!file) {
        Serial.printf("[SD] HATA: %s dosyasi acilamadi!\n", dosyaAdi);
        return false;
    }

    size_t yazilan = file.write(buffer, size);
    file.close();

    if (yazilan == size) {
        Serial.printf("[SD] Fotograf kaydedildi: %s (%u bayt)\n", dosyaAdi, size);
        return true;
    }
    return false;
}

File SDCard::createAudioFile(int audioNumber) {
    if (!m_ready) return File();

    char dosyaAdi[32];
    snprintf(dosyaAdi, sizeof(dosyaAdi), "/ses_%d.wav", audioNumber);

    File file = SD_MMC.open(dosyaAdi, FILE_WRITE);
    if (file) {
        // Ilk basta 44 bayt bos header yaz (kayit bitince guncellenecek)
        byte dummyHeader[44] = {0};
        file.write(dummyHeader, 44);
    }
    return file;
}

void SDCard::writeWavHeader(File &file, int totalAudioBytes) {
    if (!file) return;

    byte header[44];
    int toplamDosya = 36 + totalAudioBytes;
    int byteRate = AUDIO_SAMPLE_RATE * 1 * 2; // 16kHz, mono, 16-bit (2 bayt)

    // RIFF basligi
    header[0] = 'R'; header[1] = 'I'; header[2] = 'F'; header[3] = 'F';
    header[4] = (byte)(toplamDosya & 0xff);
    header[5] = (byte)((toplamDosya >> 8) & 0xff);
    header[6] = (byte)((toplamDosya >> 16) & 0xff);
    header[7] = (byte)((toplamDosya >> 24) & 0xff);
    header[8] = 'W'; header[9] = 'A'; header[10] = 'V'; header[11] = 'E';

    // fmt alt basligi
    header[12] = 'f'; header[13] = 'm'; header[14] = 't'; header[15] = ' ';
    header[16] = 16; header[17] = 0; header[18] = 0; header[19] = 0;
    header[20] = 1;  header[21] = 0; // PCM format
    header[22] = 1;  header[23] = 0; // 1 kanal (Mono)
    header[24] = (byte)(AUDIO_SAMPLE_RATE & 0xff);
    header[25] = (byte)((AUDIO_SAMPLE_RATE >> 8) & 0xff);
    header[26] = (byte)((AUDIO_SAMPLE_RATE >> 16) & 0xff);
    header[27] = (byte)((AUDIO_SAMPLE_RATE >> 24) & 0xff);
    header[28] = (byte)(byteRate & 0xff);
    header[29] = (byte)((byteRate >> 8) & 0xff);
    header[30] = (byte)((byteRate >> 16) & 0xff);
    header[31] = (byte)((byteRate >> 24) & 0xff);
    header[32] = 2;  header[33] = 0; // Block align
    header[34] = 16; header[35] = 0; // 16-bit

    // data basligi
    header[36] = 'd'; header[37] = 'a'; header[38] = 't'; header[39] = 'a';
    header[40] = (byte)(totalAudioBytes & 0xff);
    header[41] = (byte)((totalAudioBytes >> 8) & 0xff);
    header[42] = (byte)((totalAudioBytes >> 16) & 0xff);
    header[43] = (byte)((totalAudioBytes >> 24) & 0xff);

    file.seek(0);
    file.write(header, 44);
}
