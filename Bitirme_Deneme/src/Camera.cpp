#include "Camera.h"
#include "pins.h"
#include "SDCard.h"

bool Camera::m_ready = false;

bool Camera::begin() {
    Serial.println("[KAMERA] OV3660 Kamera baslatiliyor...");

    camera_config_t config = {};
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer   = LEDC_TIMER_0;
    config.pin_d0       = CAM_Y2_GPIO_NUM;
    config.pin_d1       = CAM_Y3_GPIO_NUM;
    config.pin_d2       = CAM_Y4_GPIO_NUM;
    config.pin_d3       = CAM_Y5_GPIO_NUM;
    config.pin_d4       = CAM_Y6_GPIO_NUM;
    config.pin_d5       = CAM_Y7_GPIO_NUM;
    config.pin_d6       = CAM_Y8_GPIO_NUM;
    config.pin_d7       = CAM_Y9_GPIO_NUM;
    config.pin_xclk     = CAM_XCLK_GPIO_NUM;
    config.pin_pclk     = CAM_PCLK_GPIO_NUM;
    config.pin_vsync    = CAM_VSYNC_GPIO_NUM;
    config.pin_href     = CAM_HREF_GPIO_NUM;
    config.pin_sccb_sda = CAM_SIOD_GPIO_NUM;
    config.pin_sccb_scl = CAM_SIOC_GPIO_NUM;
    config.pin_pwdn     = CAM_PWDN_GPIO_NUM;
    config.pin_reset    = CAM_RESET_GPIO_NUM;
    config.xclk_freq_hz = 12000000;
    config.pixel_format = PIXFORMAT_JPEG;
    config.frame_size   = FRAMESIZE_VGA;  // 640x480
    config.jpeg_quality = 12;             // Kalite
    config.fb_count     = 1;
    config.fb_location  = CAMERA_FB_IN_PSRAM;
    config.grab_mode    = CAMERA_GRAB_LATEST;

    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK) {
        Serial.printf("[KAMERA] HATA: Baslatilamadi: 0x%X\n", err);
        m_ready = false;
        return false;
    }

    sensor_t *s = esp_camera_sensor_get();
    if (s && s->id.PID == OV3660_PID) {
        s->set_vflip(s, 1);       // Kamerayi duzelt
        s->set_brightness(s, 1);
        s->set_saturation(s, -2);
        Serial.println("[KAMERA] OV3660 sensor basariyla ayarlandi.");
    }

    m_ready = true;
    return true;
}

bool Camera::takeAndSave(int photoNumber) {
    if (!m_ready) {
        Serial.println("[KAMERA] HATA: Kamera hazir degil!");
        return false;
    }

    Serial.println("[KAMERA] Fotograf cekiliyor...");
    camera_fb_t *fb = esp_camera_fb_get();
    if (!fb) {
        Serial.println("[KAMERA] HATA: Goruntu alinamadi!");
        return false;
    }

    bool basarili = SDCard::savePhoto(fb->buf, fb->len, photoNumber);
    esp_camera_fb_return(fb);

    return basarili;
}
