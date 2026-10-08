#ifndef MICROPHONE_H
#define MICROPHONE_H

#include <Arduino.h>

class Microphone {
public:
    static bool begin();
    static void update(int &sesSayaci);

private:
    static bool m_ready;
    static float m_dcPrevIn;   // DC blocker durumu
    static float m_dcPrevOut;

    // 32-bit ham I2S ornekleri -> DC'siz, saturasyonlu 16-bit PCM. RMS dondurur.
    static int convertBlock(const int32_t *raw, int16_t *out, int n);
};

#endif // MICROPHONE_H
