#ifndef INC_M5_0_BONUS_H_
#define INC_M5_0_BONUS_H_

#include <stdint.h>
#include "M0.0_DataCenter.h"

#ifdef __cplusplus
extern "C" {
#endif

// 77 Byte Telemetri Paketi Yapısı
#pragma pack(push, 1)
typedef struct {
    uint8_t header;           // 0xAB
    uint32_t packet_no;       // Paket Numarası
    uint8_t status;           // Uydu Statüsü
    char error_code[4];       // Hata Kodu
    char time_str[20];        // Gönderme Saati
    float pressure;           // Basınç
    float altitude;           // Yükseklik (at)
    float descent_rate;       // İniş Hızı
    float temperature;        // Sıcaklık
    float battery_voltage;    // Pil Gerilimi
    float gps_lat;            // GPS Enlem
    float gps_lon;            // GPS Boylam
    float gps_alt;            // GPS Yükseklik
    float pitch;              // Pitch
    float roll;               // Roll
    float yaw;                // Yaw
    uint16_t team_no;         // Takım No
    uint8_t crc;              // CRC
} Bonus_Telemetry_Packet_t;

// 6 Byte Komut Paketi Yapısı
typedef struct {
    uint8_t header;           // 0xAA
    uint8_t command_id;       // Komut ID
    uint8_t param_r;          // Parametre R
    uint8_t param_g;          // Parametre G
    uint8_t param_b;          // Parametre B
    uint8_t crc;              // CRC
} Bonus_Command_Packet_t;
#pragma pack(pop)

/*
 * Fonksiyon: M_Bonus_Init
 * Görevi: Bonus modülü için ilk atamaları (ör. Takım no vb.) yapar.
 */
void M_Bonus_Init(uint16_t team_id);

/*
 * Fonksiyon: M_Bonus_PackTelemetry
 * Görevi: DataCenter üzerinden sadece Pitch, Roll, Yaw ve İrtifa (Altitude)
 *         değerlerini alır, diğer alanları sıfırlar. Paketi oluşturur
 *         ve gönderilmeye hazır bayt dizisinin (buffer) adresini döndürür.
 */
uint8_t* M_Bonus_PackTelemetry(DataCenter *dc);

/*
 * Fonksiyon: M_Bonus_ParseCommand
 * Görevi: Gelen 6 byte'lık komut paketini okur ancak herhangi bir
 *         işlem/aksiyon gerçekleştirmez. CRC kontrolü yaparak
 *         paketin doğruluğunu teyit eder.
 */
void M_Bonus_ParseCommand(uint8_t *buffer, uint16_t length);

#ifdef __cplusplus
}
#endif

#endif /* INC_M5_0_BONUS_H_ */
