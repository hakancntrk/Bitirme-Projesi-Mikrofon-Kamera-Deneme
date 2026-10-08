#ifndef SDCARD_H
#define SDCARD_H

#include <Arduino.h>
#include "SD_MMC.h"

class SDCard {
public:
    static bool begin();
    static bool isReady();
    static bool savePhoto(const uint8_t *buffer, size_t size, int photoNumber);
    static File createAudioFile(int audioNumber);
    static void writeWavHeader(File &file, int totalAudioBytes);

private:
    static bool m_ready;
};

#endif // SDCARD_H
