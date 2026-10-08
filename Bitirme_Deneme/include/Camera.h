#ifndef CAMERA_H
#define CAMERA_H

#include <Arduino.h>
#include "esp_camera.h"

class Camera {
public:
    static bool begin();
    static bool takeAndSave(int photoNumber);

private:
    static bool m_ready;
};

#endif // CAMERA_H
