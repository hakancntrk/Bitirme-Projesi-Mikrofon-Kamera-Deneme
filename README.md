# Bitirme-Projesi-Mikrofon-Kamera-Deneme

---

##  Donanım Bileşenleri

* **Mikrodenetleyici:** ESP32-S3-N16R8 (16MB Flash, 8MB PSRAM)
* **Kamera:** OV3660 (VGA 640x480, JPEG formatında)
* **Mikrofon:** TDK InvenSense T5848 (I2S, 48 kHz Mono, High Quality Mode; AAD destekli ama henüz kullanılmıyor)
* **Depolama:** Kart üzerindeki dahili MicroSD kart yuvası (SDMMC 1-Bit modu)

---

##  Pin Bağlantı Tablosu

### 1. Dahili MicroSD Kart (Geliştirme Kartı Üzerinde)
Kart üzerindeki dahili soket doğrudan bağlıdır, harici kablo gerekmez:
* `SD_CLK` ➡️ `GPIO 39`
* `SD_CMD` ➡️ `GPIO 38`
* `SD_DATA` ➡️ `GPIO 40`

### 2. TDK T5848 I2S Mikrofon
* `SCK / CLK` (Saat) ➡️ `GPIO 42`
* `WS / LRCLK` (Word Select) ➡️ `GPIO 41`
* `SD / DATA` (Veri Çıkışı) ➡️ `GPIO 2`
* `WAKE` (Ses Algılama Kesmesi) ➡️ `GPIO 1` (AAD konfigüre edilmedikçe kullanılmıyor)
* `LR` (Kanal Seçimi) ➡️ `GND` (Sol kanal)

### 3. OV3660 Kamera
Geliştirme kartının FPC kamera konnektörüne doğrudan takılıdır.

---

##  Çalışma Mantığı

1. **Fotoğraf:** Her 30 saniyede bir kamera tetiklenir ve görüntü `/foto_1.jpg`, `/foto_2.jpg` ... olarak SD karta yazılır.
2. **Ses Algılama (VAD):** Mikrofon ortamı sürekli dinler. Ses enerjisi (RMS) belirlenen eşiği aştığında kayıt başlar ve `/ses_1.wav`, `/ses_2.wav` ... olarak kaydedilir. Ses kesildikten 2 saniye sonra dosya otomatik kapatılır.
3. **Format:** Sesler standart 44-byte WAV başlığıyla kaydedildiği için doğrudan herhangi bir medya oynatıcıda dinlenebilir.

---

##  Kurulum ve Yükleme

1. Projeyi **PlatformIO** (VS Code) ile açın.
2. Geliştirme kartınızı USB üzerinden bilgisayara bağlayın.
3. PlatformIO panelinden **Build** ve ardından **Upload** butonuna tıklayın.
4. Çıktıları izlemek için Serial Monitor'ü **115200 baud** hızında açın.
