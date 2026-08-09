/*
 * M4.0_Telemetry.h
 *
 * Yer istasyonuna (Rasat-GCS) UART üzerinden 77-byte BİNARY telemetri paketi gönderen modül.
 */

#ifndef INC_M4_0_TELEMETRY_H_
#define INC_M4_0_TELEMETRY_H_

#include "M0.0_DataCenter.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(push, 1) // Hafızada boşluk bırakılmasını engelle (Packed Struct)
typedef struct {
    uint8_t  header;          // 0xAB
    uint32_t paketNumarasi;
    uint8_t  uyduStatusu;
    char     hataKodu[4];
    char     gondermeSaati[20];
    float    basinc;
    float    yukseklik;
    float    inisHizi;
    float    sicaklik;
    float    pilGerilimi;
    float    gpsLatitude;
    float    gpsLongitude;
    float    gpsAltitude;
    float    pitch;
    float    roll;
    float    yaw;
    uint16_t takimNo;
    uint8_t  crc;
} TelemetryPacket;
#pragma pack(pop)

void Telemetry_Init(void);
void Telemetry_Run(DataCenter *dc);

#ifdef __cplusplus
}
#endif

#endif /* INC_M4_0_TELEMETRY_H_ */
