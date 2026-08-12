
#ifndef INC_TM_H_
#define INC_TM_H_

#include "stdint.h"
#include "string.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PAKET_HEADER_TELEMETRI   0xA5
#define PAKET_HEADER_KOMUT       0xAA
#define PAKET_HEADER_IOT         0x3C


#pragma pack(push, 1)
typedef struct {
    uint8_t  header;           // PAKET_HEADER_TELEMETRI
	uint32_t paket_no;         // 1'den başlar, resette kaldığı yerden sürer (Flash'tan okunur)
    uint8_t  uydu_statusu;     // 0..5
    uint8_t  hata_kodu[4];     // (örn: '0','0','0','0')
    char     gonderme_saati[20]; // "GG/AA/YYYY SS:DD:SS" formatı, "\0" VAR
    float    basinc;           // Pa
    float    yukseklik;        // m
    float    inis_hizi;        // m/s
    float    sicaklik;         // °C
    float    pil_gerilimi;     // V
    float    gps_latitude;
    float    gps_longitude;
    float    gps_altitude;     // m
    float    pitch;            // derece
    float    roll;             // derece
    float    yaw;              // derece
    uint32_t takim_no;
    uint8_t  crc;                // CRC8 byte'ı (Paketin en sonu)
} TelemetriPaketi_t;

typedef struct {
    uint8_t  header;         // PAKET_HEADER_KOMUT
    uint8_t  komut_id;
    uint8_t  parametre[3];
    uint8_t  crc;                // CRC8 byte'ı (Paketin en sonu)
} KomutPaketi_t;

typedef struct {
    uint8_t  header;         // PAKET_HEADER_IOT
    uint8_t  komut_id;       // İlgili komutun ID'si (Hangi komutun parametresi olduğunu bilmek için)
    uint8_t  parametre[3];   // Komut paketinden kopyalanan yönlendirilecek parametre
    uint8_t  crc;            // CRC8 byte'ı
} IoTPaketi_t;

#pragma pack(pop)


#define TELEMETRI_PAKET_BOYUTU  sizeof(TelemetriPaketi_t)
#define KOMUT_PAKET_BOYUTU      sizeof(KomutPaketi_t)
#define IOT_PAKET_BOYUTU        sizeof(IoTPaketi_t)
#define RF_MAX_PAKET_BOYUTU     (TELEMETRI_PAKET_BOYUTU > KOMUT_PAKET_BOYUTU ? (TELEMETRI_PAKET_BOYUTU > IOT_PAKET_BOYUTU ? TELEMETRI_PAKET_BOYUTU : IOT_PAKET_BOYUTU) : (KOMUT_PAKET_BOYUTU > IOT_PAKET_BOYUTU ? KOMUT_PAKET_BOYUTU : IOT_PAKET_BOYUTU))


typedef struct {
    float basinc;
    float yukseklik;
    float inis_hizi;
    float sicaklik;
    float pil_gerilimi;
    float gps_latitude;
    float gps_longitude;
    float gps_altitude;
    float pitch;
    float roll;
    float yaw;
} SensorVerileri_t;

typedef enum {
	SISTEM_MOD_TX = 0,
	SISTEM_MOD_RX = 1
} SistemModu_t;

// --- FONKSİYON PROTOTİPLERİ ---
uint8_t Hesapla_CRC8(const uint8_t *data, uint16_t length);
void Sensor_Oku(void);
void TM_Paket_Olustur(void);
void Telemetri_Gonder(void);
void Komut_Dinlemeye_Basla(void);
void Komut_Isle(void);
void IOT_Verisi_Gonder(void);

extern TelemetriPaketi_t telemetri;
extern SensorVerileri_t sensorler;
extern IoTPaketi_t iot_verisi;
extern uint8_t rx_buffer[RF_MAX_PAKET_BOYUTU];
extern KomutPaketi_t rx_komut;
extern volatile uint16_t rx_uzunluk;

#endif /* INC_TM_H_ */
