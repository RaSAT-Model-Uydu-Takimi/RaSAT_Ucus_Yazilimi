/*
 * TM.c (ALICI - RX İÇİN)
 */
#include "TM.h"
#include "main.h"
#include "stdio.h"

extern UART_HandleTypeDef huart1; // RF MODÜLÜ İÇİN
extern UART_HandleTypeDef huart2; // PC HABERLEŞMESİ İÇİN (BRIDGE - ST-LINK VCP)

// --- ALICI DEĞİŞKENLERİNİN BELLEKTEKİ ASIL YERLERİ ---
// main.c dosyasında değil, burada tanımlıyoruz.
TelemetriPaketi_t rx_telemetri;           
uint8_t rx_buffer[RF_MAX_PAKET_BOYUTU];   
KomutPaketi_t tx_komut;

// --- PC HABERLEŞMESİ (BRIDGE) DEĞİŞKENLERİ ---
uint8_t pc_rx_buffer[RF_MAX_PAKET_BOYUTU];
volatile uint8_t pc_veri_geldi_bayragi = 0;
volatile uint16_t pc_rx_uzunluk = 0;
volatile uint8_t yeni_komut_geldi = 0;


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

// --- YER İSTASYONU DİNLEME (RF MODÜL - USART1) ---
void Telemetri_Dinlemeye_Basla(void)
{
    // Hattı temizle ve taze bir DMA dinlemesi başlat
    HAL_UART_AbortReceive(&huart1);
    HAL_UARTEx_ReceiveToIdle_DMA(&huart1, rx_buffer, RF_MAX_PAKET_BOYUTU);
    __HAL_DMA_DISABLE_IT(huart1.hdmarx, DMA_IT_HT);
}

// --- PC DİNLEME (ST-LINK - USART2) ---
void PC_Dinlemeye_Basla(void)
{
    HAL_UART_AbortReceive(&huart2);
    HAL_UARTEx_ReceiveToIdle_DMA(&huart2, pc_rx_buffer, RF_MAX_PAKET_BOYUTU);
    __HAL_DMA_DISABLE_IT(huart2.hdmarx, DMA_IT_HT);
}

// --- YER İSTASYONU PAKET İŞLEME ---
void Telemetri_Isle(void)
{
    // Boyut Kontrolü
    if (rx_uzunluk == TELEMETRI_PAKET_BOYUTU)
    {
        memcpy(&rx_telemetri, rx_buffer, TELEMETRI_PAKET_BOYUTU);

        // Header Kontrolü
        if (rx_telemetri.header == PAKET_HEADER_TELEMETRI)
        {
            // CRC Kontrolü
            uint8_t beklenen_crc = Hesapla_CRC8((const uint8_t*)&rx_telemetri, TELEMETRI_PAKET_BOYUTU - 1);

            if (rx_telemetri.crc == beklenen_crc)
            {
                // BAŞARILI!
                // Gelen Telemetriyi PC'ye ilet (ST-Link USART2 Üzerinden Arayüze)
                // DMA yerine doğrudan (blocking) gönderim kullanarak test edelim
                HAL_UART_Transmit(&huart2, (uint8_t*)&rx_telemetri, TELEMETRI_PAKET_BOYUTU, 100);
            }
            else 
            {
                // HATA: CRC Uyuşmazlığı
                char dbg[64];
                sprintf(dbg, "HATA: CRC Yanlis! (Gelen: %d, Beklenen: %d)\r\n", rx_telemetri.crc, beklenen_crc);
                HAL_UART_Transmit(&huart2, (uint8_t*)dbg, strlen(dbg), 100);
            }
        }
        else 
        {
            // HATA: Header Yanlış
            char dbg[64];
            sprintf(dbg, "HATA: Header Yanlis! (Gelen: 0x%X)\r\n", rx_telemetri.header);
            HAL_UART_Transmit(&huart2, (uint8_t*)dbg, strlen(dbg), 100);
        }
    }
    else 
    {
        // HATA: Boyut Yanlış
        if (rx_uzunluk > 0) { // Sadece sıfırdan büyük veri geldiğinde bas
            char dbg[64];
            sprintf(dbg, "HATA: Boyut Yanlis! (Gelen: %d, Beklenen: %d)\r\n", rx_uzunluk, TELEMETRI_PAKET_BOYUTU);
            HAL_UART_Transmit(&huart2, (uint8_t*)dbg, strlen(dbg), 100);
        }
    }

    // Eski veriler yanıltmasın diye buffer'ı sıfırla
    memset(rx_buffer, 0, RF_MAX_PAKET_BOYUTU);
}


// --- PC'DEN GELEN VERİYİ İŞLEME (ARAYÜZDEN GELEN KOMUT) ---
void PC_Gelen_Veriyi_Isle(void)
{
    // Arayüzden gelen komut paketini al (Header ve Boyut kontrolü)
    if (pc_rx_uzunluk == KOMUT_PAKET_BOYUTU && pc_rx_buffer[0] == PAKET_HEADER_KOMUT)
    {
        // Direkt gelen paketi komut buffer'ına kopyala
        memcpy(&tx_komut, pc_rx_buffer, KOMUT_PAKET_BOYUTU);

        // Komutun senkron zamanında (TX dilimi) gönderilmesi için bayrağı set edelim
        yeni_komut_geldi = 1;
    }

    // Eski veriler yanıltmasın diye buffer'ı sıfırla
    memset(pc_rx_buffer, 0, RF_MAX_PAKET_BOYUTU);
}

// --- YER İSTASYONU KOMUT PAKETİ OLUŞTURMA ---
// Parametreleri arayüzden veya main içinden kolayca atamak için kullanılır.
void Komut_Paketi_Olustur(uint8_t komut_id, uint8_t p0, uint8_t p1, uint8_t p2, uint8_t p3, uint8_t p4, uint8_t p5)
{
    tx_komut.komut_id = komut_id;

    tx_komut.parametre[0] = p0;
    tx_komut.parametre[1] = p1;
    tx_komut.parametre[2] = p2;

}

// --- YER İSTASYONU KOMUT GÖNDERME (RF MODÜL - USART1) ---
void Komut_Gonder(void)
{
    tx_complete = 0;
    tx_komut.header = PAKET_HEADER_KOMUT;

    // Göndermeden hemen önce CRC ekle
    tx_komut.crc = Hesapla_CRC8((const uint8_t*)&tx_komut, KOMUT_PAKET_BOYUTU - 1);

    HAL_UART_Transmit_DMA(&huart1, (uint8_t*)&tx_komut, KOMUT_PAKET_BOYUTU);
    __HAL_DMA_DISABLE_IT(huart1.hdmatx, DMA_IT_HT); // Half transfer kapalı
}
