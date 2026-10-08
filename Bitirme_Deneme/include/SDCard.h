#ifndef SDCARD_H
#define SDCARD_H

#include <Arduino.h>
#include "SD_MMC.h"

class SDCard {
public:
    static bool begin();
    static bool isReady();
    // SD'de prefix<N>ext biciminde var olan en yuksek N'yi bulur, N+1 dondurur
    static int nextFileNumber(const char *prefix, const char *ext);
    static bool savePhoto(const uint8_t *buffer, size_t size, int photoNumber);
    static File createAudioFile(int audioNumber);
    static void writeWavHeader(File &file, int totalAudioBytes);

private:
    static bool m_ready;
};

#endif // SDCARD_H
