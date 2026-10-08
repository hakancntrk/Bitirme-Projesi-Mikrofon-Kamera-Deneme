#ifndef MICROPHONE_H
#define MICROPHONE_H

#include <Arduino.h>

class Microphone {
public:
    static bool begin();
    static void update(int &sesSayaci);

private:
    static bool m_ready;
    static int hesaplaRMS(const int32_t *rawSamples, int ornekSayisi);
};

#endif // MICROPHONE_H
