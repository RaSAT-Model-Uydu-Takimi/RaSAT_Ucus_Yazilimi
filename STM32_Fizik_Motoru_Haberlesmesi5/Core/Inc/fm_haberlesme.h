/*
 * fm_haberlesme.h
 *
 *  Created on: Jul 9, 2026
 *      Author: abduh
 */

#ifndef INC_FM_HABERLESME_H_
#define INC_FM_HABERLESME_H_

#include "stm32f4xx_hal.h" // UART ve DMA pointer'larını tanıyabilmesi için
#include <stdint.h>
#include <string.h>

// fm_haberlesme.h içerisine ekle:
typedef enum {
    PAKET_HATA = 0,  // Hiçbir şey gelmedi
    PAKET_TAM = 1,   // 52 byte eksiksiz geldi
    PAKET_EKSIK = 2  // Timeout oldu ama bir miktar veri geldi
} Paket_Durum_t;

// --- PAKET SENKRONİZASYON VE BİTMASK MAKROLARI ---
#define PAKET_HEADER 0xAA55U

// Durum Bayrakları Maskeleri (Bit 0 - 3)
#define BAYRAK_APAM  (1U << 0) // Bit 0
#define BAYRAK_SEP   (1U << 1) // Bit 1: Ayrılma
#define BAYRAK_SGM   (1U << 2) // Bit 2: SİGMA Kanatları
#define BAYRAK_BUZZ  (1U << 3) // Bit 3: Buzzer
//bayrakları kaldırmak için durumbayraklari kısmı ile veyalama işlemi yapacağız.
//bayrağı indirmek için ise bayrağın değilini alıp veleyeceğiz.

// Hata Kodu Maskeleri (Bit 4 - 7)
#define HATA_NACK    (0x0FU << 4) // Paket bozuk/eksik hata kodu

typedef struct __attribute__((packed)){
    uint16_t header;           // 2 Bayt: Sabit Senkronizasyon Başlığı (0xAA55)
    uint32_t index;            // 4 Bayt: Paket Sırası
    uint16_t mi1;              // 2 Bayt: Motor İtkisi 1 (Pluto 50A BLHeli)
    uint16_t mi2;              // 2 Bayt: Motor İtkisi 2
    uint16_t mi3;              // 2 Bayt: Motor İtkisi 3
    uint16_t mi4;              // 2 Bayt: Motor İtkisi 4
    uint8_t  durum_bayraklari; // 1 Bayt: Evrensel Bitmask (APAM, SEP, SGM, BUZZ ve Hatalar)
    uint8_t  checksum;         // 1 Bayt: XOR Checksum Doğrulama Baydı
} Iletim_Paketi_t;             // TOPLAM: 16 Bayt (Tam 32-bit hizalı)

typedef union {
    Iletim_Paketi_t paket;
    uint8_t buffer[sizeof(Iletim_Paketi_t)];
} Iletim_Veri_t;

typedef struct __attribute__((packed)) {
    uint16_t header;      // 2 Bayt: Sabit Senkronizasyon Başlığı (0xAA55)
    uint32_t index;       // 4 Bayt: Paket sırası (0 - 4.2 Milyar)

    // IMU Verileri (MPU9250 - 16-bit)
    int16_t acc_x, acc_y, acc_z;    // 6 Bayt (İvme: mg cinsinden. 1g = 1000)
    int16_t gyro_x, gyro_y, gyro_z; // 6 Bayt (Jiroskop: Derece/sn x 10)
    int16_t mag_x, mag_y, mag_z;    // 6 Bayt (Manyetometre: mili-Gauss ham veri)

    // Çevre Sensörleri (BMP280)
    uint32_t basinc;      // 4 Bayt: Pascal cinsinden ham veri (Örn: 101325 Pa)
    int16_t sicaklik;     // 2 Bayt: Santigrat x 100 (Örn: 25.43 C -> 2543)

    // GPS Verileri (BN-880 / Ublox M8N)
    int32_t gps_lat;      // 4 Bayt: Latitude x 10^7
    int32_t gps_lon;      // 4 Bayt: Longitude x 10^7
    int32_t gps_alt;      // 4 Bayt: Yükseklik (Milimetre)
    int16_t gps_vel;      // 2 Bayt: Dikey Hız (cm/s)

    // Güç Sistemi (INA219)
    uint16_t bat_v;       // 2 Bayt: Gerilim (Milivolt)
	int32_t  bat_a;       // 4 Bayt: Akım (Miliamper)
	uint8_t  rezerve;     // 1 Bayt: 52 bayta 4-byte align tamamlayan dolgu
	uint8_t  checksum;    // 1 Bayt: XOR Checksum (EN SONDA)

    } Alim_Paketi_t;          // TOPLAM: 52 Bayt (13 Word)

typedef union {
    Alim_Paketi_t paket;
    uint8_t buffer[sizeof(Alim_Paketi_t)];
} Alim_Veri_t;




// --- FONKSİYON PROTOTİPLERİ ---
// 1. Donanım Tanıtma (Sistemin başında 1 kez çağrılır)
void Donanim_Sec(UART_HandleTypeDef *huart, TIM_HandleTypeDef *htim);

// 2. İşlem Fonksiyonları (Artık donanım parametresi istemiyorlar)
Paket_Durum_t Alim_Paketini_Al(Alim_Veri_t *islenecek_guvenli);
uint8_t Bozuk_Paket_Mi(Alim_Veri_t *paket);
uint8_t Alarm_Paket_Mi(Alim_Veri_t *paket);
void Gonder(Iletim_Veri_t *paket);

#endif /* INC_FM_HABERLESME_H_ */
