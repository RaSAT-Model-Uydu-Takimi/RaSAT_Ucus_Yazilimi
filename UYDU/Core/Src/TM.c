
#include "TM.h"
#include "main.h"      // HAL UART, DMA vb. fonksiyonları tanıyabilmesi için
#include "string.h"    // memcpy, memset vb. için

// 1. DEĞİŞKENLERİ BURADA TANIMLIYORUZ (Bellekteki asıl yerleri burası olacak)
TelemetriPaketi_t telemetri;
SensorVerileri_t sensorler;
IoTPaketi_t iot_verisi;
uint8_t rx_buffer[RF_MAX_PAKET_BOYUTU];
KomutPaketi_t rx_komut;

// main.c dosyasında tanımlı olan UART ve Bayrak değişkenlerini
// bu dosyada kullanabilmek için "extern" (dışarıda ara) komutuyla çağırıyoruz.
extern UART_HandleTypeDef huart2;
extern volatile uint8_t tx_complete;
extern TIM_HandleTypeDef htim9;

//PFP
void ayrilma(void);
void apam(void);
void kilitle(void);


// --- CRC HESAPLAMA FONKSİYONU ---
uint8_t Hesapla_CRC8(const uint8_t *data, uint16_t length)
{
    uint8_t crc = 0x00;

    for (uint16_t i = 0; i < length; i++)
    {
        crc ^= data[i];
        for (uint8_t j = 0; j < 8; j++)
        {
            if (crc & 0x80)
            {
                crc = (crc << 1) ^ 0x07;
            }
            else
            {
                crc <<= 1;
            }
        }
    }
    return crc;
}

// --- SENSÖR OKUMA FONKSİYONU ---
void Sensor_Oku(void)
{
    sensorler.basinc = 1013.25f;
    sensorler.yukseklik += 0.5f;
    sensorler.inis_hizi = 0.0f;
    sensorler.sicaklik = 25.4f;
    sensorler.pil_gerilimi = 8.2f;

    sensorler.gps_latitude = 41.0082f;
    sensorler.gps_longitude = 28.9784f;
    sensorler.gps_altitude = sensorler.yukseklik;

    sensorler.pitch = 5.2f;
    sensorler.roll = -1.4f;
    sensorler.yaw = 90.0f;
}

// --- PAKET OLUŞTURMA FONKSİYONU ---
void TM_Paket_Olustur(void)
{
    telemetri.header = PAKET_HEADER_TELEMETRI;
    telemetri.paket_no++;
    telemetri.uydu_statusu = 1;
    telemetri.takim_no = 84710;

    telemetri.hata_kodu[0] = '0';
    telemetri.hata_kodu[1] = '0';
    telemetri.hata_kodu[2] = '0';
    telemetri.hata_kodu[3] = '0';

    strcpy(telemetri.gonderme_saati, "08/08/2026 12:46:00");

    telemetri.basinc       = sensorler.basinc;
    telemetri.yukseklik    = sensorler.yukseklik;
    telemetri.inis_hizi    = sensorler.inis_hizi;
    telemetri.sicaklik     = sensorler.sicaklik;
    telemetri.pil_gerilimi = sensorler.pil_gerilimi;

    telemetri.gps_latitude = sensorler.gps_latitude;
    telemetri.gps_longitude= sensorler.gps_longitude;
    telemetri.gps_altitude = sensorler.gps_altitude;

    telemetri.pitch = sensorler.pitch;
    telemetri.roll  = sensorler.roll;
    telemetri.yaw   = sensorler.yaw;

    // CRC Hesapla ve ata (En son byte hariç)
    telemetri.crc = Hesapla_CRC8((const uint8_t*)&telemetri, sizeof(TelemetriPaketi_t) - 1);
}

// --- GÖNDERME FONKSİYONU ---
void Telemetri_Gonder(void)
{
    tx_complete = 0;
    HAL_UART_Transmit_DMA(&huart2, (uint8_t*)&telemetri, sizeof(TelemetriPaketi_t));
    __HAL_DMA_DISABLE_IT(huart2.hdmatx, DMA_IT_HT);
}


void Komut_Dinlemeye_Basla(void)
{
    // Olası kilitlenmeleri önlemek için önce aktif alımı durdur
    HAL_UART_AbortReceive(&huart2);

    // Idle Line destekli DMA okumasını başlat
    HAL_UARTEx_ReceiveToIdle_DMA(&huart2, rx_buffer, RF_MAX_PAKET_BOYUTU);
    __HAL_DMA_DISABLE_IT(huart2.hdmarx, DMA_IT_HT); // Half-Transfer kesmesini kapat (Gereksiz CPU yükü)
}

void Komut_Isle(void)
{
    // 1. Boyut Kontrolü
    if (rx_uzunluk == KOMUT_PAKET_BOYUTU)
    {
        // 2. Veriyi Struct'a Kopyala
        memcpy(&rx_komut, rx_buffer, KOMUT_PAKET_BOYUTU);

        // 3. Header ve CRC Kontrolü
        if (rx_komut.header == PAKET_HEADER_KOMUT)
        {
            uint8_t beklenen_crc = Hesapla_CRC8((const uint8_t*)&rx_komut, KOMUT_PAKET_BOYUTU - 1);

            if (rx_komut.crc == beklenen_crc)
            {
                // --- GEÇERLİ KOMUT ALINDI ---
                // rx_komut.komut_id ve rx_komut.parametre[x] kullanılarak işlemler yapılır.

                switch(rx_komut.komut_id) {
                	case 0x00 :
                		// idle
                		break;
                    case 0x01:
                        ayrilma();
                        break;
                    case 0x02:
                        apam();
                        break;
                    case 0x03:
                        // sıgma fonksiyonu
                        break;
                    case 0x04:
                    	// ıot komut alındı ıot yollama fonksiyonu
                        IOT_Verisi_Gonder();
                    	break;
                    case 0x05:
                    	//motor tahrik
                    case 0x06:
                    	//motor güç kes
                    case 0x07:
                    	//kilitle
                    	kilitle();
                    default:
                        break;
                }
            }
        }
    }

    // İşlem bittikten sonra buffer'ı temizle ki eski veriler yanıltmasın
    memset(rx_buffer, 0, RF_MAX_PAKET_BOYUTU);
}

void IOT_Verisi_Gonder(void)
{
    // 1. Komut paketinden gelen verileri IOT paketine aktar
    iot_verisi.header = PAKET_HEADER_IOT;
    iot_verisi.komut_id = rx_komut.komut_id;
    iot_verisi.parametre[0] = rx_komut.parametre[0];
    iot_verisi.parametre[1] = rx_komut.parametre[1];
    iot_verisi.parametre[2] = rx_komut.parametre[2];

    // 2. IOT paketi için CRC'yi hesapla
    iot_verisi.crc = Hesapla_CRC8((const uint8_t*)&iot_verisi, sizeof(IoTPaketi_t) - 1);

    // 3. IOT paketini UART üzerinden (ve LoRa ile havaya) gönder
    // Anında gitmesi için blocking modda gönderim yapıyoruz. (RX dilimindeyken doğrudan basılır)
    HAL_UART_Transmit(&huart2, (uint8_t*)&iot_verisi, sizeof(IoTPaketi_t), 100);
}

void ayrilma(void)
{
    // Ayrılma Durumu (0 derece)
    __HAL_TIM_SET_COMPARE(&htim9, TIM_CHANNEL_1, 500);
}

void apam(void)
{
    // APAM Durumu (180 derece)
    __HAL_TIM_SET_COMPARE(&htim9, TIM_CHANNEL_1, 2500);
}

void kilitle(void)
{
    // APAM Durumu (180 derece)
    __HAL_TIM_SET_COMPARE(&htim9, TIM_CHANNEL_1, 1500);
}
