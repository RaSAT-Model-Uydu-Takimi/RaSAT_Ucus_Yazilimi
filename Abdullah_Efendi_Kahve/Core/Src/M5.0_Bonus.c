#include "M5.0_Bonus.h"
#include <string.h>

static Bonus_Telemetry_Packet_t tx_packet;
static uint16_t current_team_no = 0;
static uint32_t current_packet_no = 0;

/* Basit CRC-8 Hesaplama (Polinom: 0x07) */
static uint8_t Calculate_CRC8(uint8_t *data, uint16_t length) {
    uint8_t crc = 0x00;
    for (uint16_t i = 0; i < length; i++) {
        crc ^= data[i];
        for (uint8_t j = 0; j < 8; j++) {
            if (crc & 0x80) {
                crc = (uint8_t)((crc << 1) ^ 0x07);
            } else {
                crc = (uint8_t)(crc << 1);
            }
        }
    }
    return crc;
}

void M_Bonus_Init(uint16_t team_id) {
    current_team_no = team_id;
    current_packet_no = 0;
    memset(&tx_packet, 0, sizeof(Bonus_Telemetry_Packet_t));
}

uint8_t* M_Bonus_PackTelemetry(DataCenter *dc) {
    if (dc == NULL) {
        return (uint8_t*)&tx_packet;
    }

    // Paketin geri kalanını sıfırla (İstenmeyen veriler 0 gidecek)
    memset(&tx_packet, 0, sizeof(Bonus_Telemetry_Packet_t));

    // Sabitler ve Sayaçlar
    tx_packet.header = 0xAB;
    tx_packet.packet_no = current_packet_no++;
    tx_packet.team_no = current_team_no;
    
    // String kısımlarına varsayılan değerler yazıyoruz. 
    // Eğer null bırakırsak, C# WinForms tarafında GDI render motoru \0 karakterini görünce satırın geri kalanını çizmeyi bırakır! (Bu yüzden ekranda değerler eksik görünüyordu)
    strncpy(tx_packet.error_code, "0000", 4);
    strncpy(tx_packet.time_str, "08/08/2026 15:30:00", 19);

    // Sadece istenen değerleri doldur: Pitch, Roll, Yaw ve Yükseklik(at)
    tx_packet.pitch = dc->estimated.pitch.value;
    tx_packet.roll = dc->estimated.roll.value;
    tx_packet.yaw = dc->estimated.yaw.value;
    
    // Altitude için barometre veya EKF tahminini kullanıyoruz
    tx_packet.altitude = dc->estimated.pos_z.value;

    // CRC hesaplama (Son 1 byte CRC olduğu için sizeof - 1 kadar)
    tx_packet.crc = Calculate_CRC8((uint8_t*)&tx_packet, sizeof(Bonus_Telemetry_Packet_t) - 1);

    return (uint8_t*)&tx_packet;
}

void M_Bonus_ParseCommand(uint8_t *buffer, uint16_t length) {
    if (buffer == NULL || length < sizeof(Bonus_Command_Packet_t)) {
        return; // Geçersiz paket
    }

    Bonus_Command_Packet_t *cmd = (Bonus_Command_Packet_t *)buffer;

    // Header ve CRC Kontrolü
    if (cmd->header == 0xAA) {
        uint8_t calculated_crc = Calculate_CRC8(buffer, sizeof(Bonus_Command_Packet_t) - 1);
        
        if (calculated_crc == cmd->crc) {
            // Paket doğru bir şekilde alındı ve CRC eşleşti.
            // Kullanıcının isteği üzerine: "aldığın şeyler bir şey yapma"
            // Bu nedenle burada herhangi bir aksiyon gerçekleştirilmiyor.
            
            /* Örnek (Kullanılmayan) Aksiyon Blokları:
            if (cmd->command_id == 0x01) {
                // Ayrılma komutu
            }
            */
        }
    }
}
