/*
 * TM.h (ALICI - RX İÇİN)
 */
#ifndef INC_TM_H_
#define INC_TM_H_

#include "stdint.h"
#include "string.h"

#ifdef __cplusplus
extern "C" {
#endif

// 1. HEADER (BAŞLIK) TANIMLARI
#define PAKET_HEADER_TELEMETRI   0xA5
#define PAKET_HEADER_KOMUT       0xAA
#define PAKET_HEADER_IOT         0x3C  // IoT eksikti, eklendi

typedef enum {
    YER_MOD_RX = 0, // Uydu TX yaparken, İstasyon dinler
    YER_MOD_TX = 1  // Uydu RX yaparken, İstasyon gönderir
} YerIstasyonuModu_t;

// 2. STRUCT TANIMLARI (Boyut hesaplamasından ÖNCE gelmeli)
#pragma pack(push, 1)
typedef struct {
    uint8_t  header;
    uint32_t paket_no;
    uint8_t  uydu_statusu;
    uint8_t  hata_kodu[4];
    char     gonderme_saati[20];
    float    basinc;
    float    yukseklik;
    float    inis_hizi;
    float    sicaklik;
    float    pil_gerilimi;
    float    gps_latitude;
    float    gps_longitude;
    float    gps_altitude;
    float    pitch;
    float    roll;
    float    yaw;
    uint32_t takim_no;
    uint8_t  crc;
} TelemetriPaketi_t;

typedef struct {
    uint8_t  header;
    uint8_t  komut_id;
    uint8_t  parametre[3];
    uint8_t  crc;
} KomutPaketi_t;

// Hata vermemesi için basit haliyle IoT paketi eklendi
typedef struct {
    uint8_t  header;
    uint8_t  komut_id;
    uint8_t  parametre[3];
    uint8_t  crc;
} IoTPaketi_t;
#pragma pack(pop)

// 3. BOYUT HESAPLAMALARI (Structlardan SONRA olmalı)
#define TELEMETRI_PAKET_BOYUTU  sizeof(TelemetriPaketi_t)
#define KOMUT_PAKET_BOYUTU      sizeof(KomutPaketi_t)
#define IOT_PAKET_BOYUTU        sizeof(IoTPaketi_t)

// En büyük paketin boyutunu bul (Buffer dizisini oluşturmak için)
#define RF_MAX_PAKET_BOYUTU     (TELEMETRI_PAKET_BOYUTU > KOMUT_PAKET_BOYUTU ? (TELEMETRI_PAKET_BOYUTU > IOT_PAKET_BOYUTU ? TELEMETRI_PAKET_BOYUTU : IOT_PAKET_BOYUTU) : (KOMUT_PAKET_BOYUTU > IOT_PAKET_BOYUTU ? KOMUT_PAKET_BOYUTU : IOT_PAKET_BOYUTU))

// --- 4. ALICI (RX) DEĞİŞKEN BİLDİRİMLERİ ---
extern uint8_t rx_buffer[RF_MAX_PAKET_BOYUTU];
extern TelemetriPaketi_t rx_telemetri;
extern KomutPaketi_t tx_komut;

extern volatile uint16_t rx_uzunluk;
extern volatile uint8_t tx_complete;

extern volatile uint8_t senkron_mu;
extern volatile uint8_t kacan_paket_sayisi;

// --- PC HABERLEŞMESİ İÇİN (BRIDGE) DEĞİŞKENLER ---
extern uint8_t pc_rx_buffer[RF_MAX_PAKET_BOYUTU];
extern volatile uint8_t pc_veri_geldi_bayragi;
extern volatile uint16_t pc_rx_uzunluk;
extern volatile uint8_t yeni_komut_geldi;

// --- 5. ALICI (RX) FONKSİYON PROTOTİPLERİ ---
uint8_t Hesapla_CRC8(const uint8_t *data, uint16_t length);
void Telemetri_Dinlemeye_Basla(void);
void Telemetri_Isle(void);
void Komut_Gonder(void);
void Komut_Paketi_Olustur(uint8_t komut_id, uint8_t p0, uint8_t p1, uint8_t p2, uint8_t p3, uint8_t p4, uint8_t p5);

// --- PC HABERLEŞMESİ İÇİN FONKSİYON PROTOTİPLERİ ---
void PC_Dinlemeye_Basla(void);
void PC_Gelen_Veriyi_Isle(void);

#ifdef __cplusplus
}
#endif

#endif /* INC_TM_H_ */
