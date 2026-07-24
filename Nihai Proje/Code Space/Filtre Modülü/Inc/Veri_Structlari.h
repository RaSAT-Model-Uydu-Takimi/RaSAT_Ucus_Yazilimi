/*
 * Veri_Structlari.h
 *
 *  Created on: Jul 19, 2026
 *      Author: abduh
 */

#ifndef INC_VERI_STRUCTLARI_H_
#define INC_VERI_STRUCTLARI_H_

#include <stdint.h>

typedef struct
{
	// --- KONUM VERİLERİ (Yüksek Hassasiyet) ---
	double latitude_deg;  // Enlem. Birim: Ondalıklı Derece (Decimal Degrees). Örn: 41.0123456
						  // Hassasiyet kaybı olmaması için (metre altı sapmalar için) double kullanılmıştır.

	double longitude_deg; // Boylam. Birim: Ondalıklı Derece (Decimal Degrees). Örn: 28.9765432
						  // Hassasiyet kaybı olmaması için double kullanılmıştır.

	// --- FİZİKSEL HAREKET VE İRTİFA VERİLERİ ---
	float altitude_m;     // İrtifa/Yükseklik. Birim: Metre (m). Deniz seviyesine (MSL) göre olan yükseklik.
	float speed_mps;      // Yere Göre Hız (Speed Over Ground). Birim: Metre/Saniye (m/s).
	float course_deg;     // Yön/Rota (Course/Heading). Birim: Derece (°). 0-360 arası, Kuzeye göre gidilen gerçek yön.
	float hdop;           // Yatay Hassasiyet Çarpanı (Horizontal Dilution of Precision). Birimsizdir.
						  // Geometrik uydu dizilim kalitesini gösterir. 1.0 mükemmel, 2-3 iyi, >5 kötü demektir.

	// --- SİSTEM VE BAĞLANTI DURUM VERİLERİ ---
	uint8_t satellites;   // Kilitlenilen Uydu Sayısı. Birim: Adet. Konum bulmak için anlık kullanılan uydular.
	uint8_t fix_quality;  // Bağlantı Kalite Tipi (GGA'dan gelir).
						  // 0: Bağlantı yok, 1: Standart 3D Fix, 2: DGPS (Diferansiyel GPS) aktif.
	uint8_t fix_valid;    // Konum Geçerlilik Bayrağı (RMC'den gelir).
						  // 1: Veri geçerli (Active), 0: Veri geçersiz/hesaplanıyor (Void).

	// --- YAZILIMSAL KONTROL BAYRAĞI ---
	uint8_t new_data;     // Yeni Veri Bayrağı. UART'tan yeni bir NMEA paketi başarıyla çözümlendiğinde 1 olur.
						  // main() döngüsünde veri okunduktan sonra tekrar 0'a çekilir.

} BN_Verisi_t;





typedef struct
{
	float pressure_pa;   // Kompanze edilmiş atmosferik basınç. Birim: Pascal (Pa). (Örn: Deniz seviyesinde standart atmosferik basınç yaklaşık 101325 Pa civarındadır.)
	float temperature_c; // Kompanze edilmiş sıcaklık. Birim: Santigrat (°C). (Örn: 25.4 °C)
	uint8_t valid;       // Veri geçerlilik bayrağı (Flag). (1: Okuma başarılı, 0: Hata/Geçersiz)

} BMP_Verisi_t;




typedef struct
{
    /*
     * Accelerometer
     * Birim: g
     */
    float accel_x_g;
    float accel_y_g;
    float accel_z_g;
    /*
     * Gyroscope
     * Birim: degree per second
     */
    float gyro_x_dps;
    float gyro_y_dps;
    float gyro_z_dps;
    /*
     * Magnetometer
     * Birim: microtesla
     */
    float mag_x_uT;
    float mag_y_uT;
    float mag_z_uT;

    uint8_t valid; //accel + gyro verisi geçerli mi?
    uint8_t mag_valid; //magnetometer verisi geçerli mi?

} MPU_Verisi_t;




typedef struct{

	float v_bat;
	float i_bat; // burası henüz belli değil.

}INA_Verisi_t;



typedef struct{

	BMP_Verisi_t bmp;
	MPU_Verisi_t mpu;
	BN_Verisi_t bn;
	INA_Verisi_t ina;
    uint32_t timestamp_us; // Sensör paketinin us cinsinden zaman damgası

}Sensor_Verileri_t;


#endif /* INC_VERI_STRUCTLARI_H_ */
