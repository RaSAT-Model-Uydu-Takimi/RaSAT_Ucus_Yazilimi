/*
 * Processed_Data.h
 *
 * Tanım:
 * Kestirim çekirdeği (EKF ve Sensör Füzyonu) sonucunda oluşturulan işlenmiş veri yapılarını,
 * sensör kalibrasyon ve filtreleme çıktılarını ve sensörlere has güven katsayılarını tanımlar.
 *
 * İşlev:
 * 1. Ham sensörlerden (Veri_Structlari.h içindeki Sensor_Verileri_t) okunan verileri SI birimlerinde
 *    ve standart formatta (SensorChannels_t) depolar.
 * 2. Sensör kalibrasyonu (bias, ölçek, termal kayma) uygulanmış verileri ile EKF ve dijital süzgeçlerden
 *    geçirilmiş verileri ayrı ayrı tutar.
 * 3. Her bir sensör grubu (İvme, Jiroskop, Manyetometre, Barometre, GPS) için hesaplanan anlık
 *    güvenilirlik katsayılarını (SensorConfidences_t) barındırır.
 * 4. 3 eksenli EKF algoritmasının çıktıları olan Kartezyen konum, hız, ivme ve hem GPS hem barometre
 *    füzyonu ile hesaplanmış nihai dikey irtifa (altitude) kestirim değerlerini depolar.
 */

#ifndef INC_PROCESSED_DATA_H_
#define INC_PROCESSED_DATA_H_

#include <stdint.h>
#include "Veri_Structlari.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Tanım: Standart Sensör Kanal Yapısı (SensorChannels_t)
 * İşlev: Farklı sensör birimlerinden gelen verilerin ortak bir SI / fiziki birim standardında
 * depolanmasını sağlar. Bu yapı hem ham (Raw), hem kalibre edilmiş (Calibrated), hem de
 * filtrelenmiş (Filtered) sensör verilerini taşımak için kullanılır.
 */
typedef struct {
    // --- İVMEÖLÇER KANALLARI ---
    float accel_x_g;         // X ekseni ivmesi [g]
    float accel_y_g;         // Y ekseni ivmesi [g]
    float accel_z_g;         // Z ekseni ivmesi [g]
    float accel_x_mps2;      // X ekseni ivmesi SI biriminde [m/s^2]
    float accel_y_mps2;      // Y ekseni ivmesi SI biriminde [m/s^2]
    float accel_z_mps2;      // Z ekseni ivmesi SI biriminde [m/s^2]

    // --- JİROSKOP KANALLARI ---
    float gyro_x_dps;        // X ekseni açısal hızı [derece/saniye]
    float gyro_y_dps;        // Y ekseni açısal hızı [derece/saniye]
    float gyro_z_dps;        // Z ekseni açısal hızı [derece/saniye]
    float gyro_x_radps;      // X ekseni açısal hızı SI biriminde [radyan/saniye]
    float gyro_y_radps;      // Y ekseni açısal hızı SI biriminde [radyan/saniye]
    float gyro_z_radps;      // Z ekseni açısal hızı SI biriminde [radyan/saniye]

    // --- MANYETOMETRE KANALLARI ---
    float mag_x_uT;          // X ekseni manyetik alan şiddeti [mikrotesla, uT]
    float mag_y_uT;          // Y ekseni manyetik alan şiddeti [mikrotesla, uT]
    float mag_z_uT;          // Z ekseni manyetik alan şiddeti [mikrotesla, uT]

    // --- BAROMETRE VE ÇEVRESEL KANALLAR ---
    float pressure_pa;       // Kompanze atmosfer basıncı [Pascal, Pa]
    float temperature_c;     // Sıcaklık [Santigrat, °C]
    float baro_altitude_m;   // Uluslararası barometrik formül ile hesaplanan irtifa [metre]

    // --- GPS KANALLARI ---
    double latitude_deg;     // GPS Enlemi [Ondalıklı derece]
    double longitude_deg;    // GPS Boylamı [Ondalıklı derece]
    float gps_altitude_m;    // GPS sensörü tarafından hesaplanan deniz seviyesi irtifası [metre]
    float gps_speed_mps;     // GPS yer hızı [metre/saniye]
    float gps_course_deg;    // GPS hareket yönü/rotası [derece]

    // --- GÜÇ VE BATARYA KANALLARI ---
    float v_bat_v;           // Batarya gerilimi [Volt]
    float i_bat_a;           // Batarya akımı [Amper]
} SensorChannels_t;


/*
 * Tanım: Sensör Güvenilirlik Katsayıları Yapısı (SensorConfidences_t)
 * İşlev: Kestirim çekirdeği (Confidence Engine) tarafından her döngüde sensör verilerinin
 * tutarlılığı, fiziksel sınırlar içinde olması, gürültü seviyesi, donma (stuck) ve sıçrama (spike)
 * durumlarına göre hesaplanan güven katsayılarını (0.0 ile 100.0 arasında) tutar.
 */
typedef struct {
    float accel_confidence;  // İvmeölçer güvenilirlik skoru [%0.0 - %100.0]
    float gyro_confidence;   // Jiroskop güvenilirlik skoru [%0.0 - %100.0]
    float mag_confidence;    // Manyetometre güvenilirlik skoru [%0.0 - %100.0]
    float baro_confidence;   // Barometre ve basınç irtifası güvenilirlik skoru [%0.0 - %100.0]
    float gps_confidence;    // GPS (kilit uydu, HDOP ve irtifa kalitesi) güvenilirlik skoru [%0.0 - %100.0]
} SensorConfidences_t;


/*
 * Tanım: EKF Durum Kestirim Yapısı (EKF_State_t)
 * İşlev: 3 eksenli Genişletilmiş Kalman Filtresi'nin (EKF) durum vektöründen (X) ve
 * sensör füzyonundan elde edilen nihai konum, hız, ivme ve koordinat kestirim değerlerini saklar.
 */
typedef struct {
    // --- YEREL KARTEZYEN KONUM VE İRTİFA (NED/ENU Referanslı) ---
    float pos_x_m;           // Yerel teğet düzleminde X ekseni konumu [metre]
    float pos_y_m;           // Yerel teğet düzleminde Y ekseni konumu [metre]
    float pos_z_m;           // Sensör füzyonu (Barometre + GPS) sonucu elde edilen nihai dikey irtifa [metre]

    // --- FİLTRELENMİŞ HIZ VEKTÖRÜ ---
    float vel_x_mps;         // X ekseni doğrusal hızı [metre/saniye]
    float vel_y_mps;         // Y ekseni doğrusal hızı [metre/saniye]
    float vel_z_mps;         // Z ekseni doğrusal hızı (dikey tırmanma/alçalma hızı) [metre/saniye]

    // --- FİLTRELENMİŞ İVME VEKTÖRÜ ---
    float acc_x_mps2;        // X ekseni doğrusal ivmesi [metre/saniye^2]
    float acc_y_mps2;        // Y ekseni doğrusal ivmesi [metre/saniye^2]
    float acc_z_mps2;        // Z ekseni doğrusal ivmesi [metre/saniye^2]

    // --- FİLTRELENMİŞ KÜRESEL KOORDİNATLAR ---
    double fused_latitude_deg;  // EKF yerel X/Y konumundan ve GPS'ten birleştirilmiş nihai enlem [derece]
    double fused_longitude_deg; // EKF yerel X/Y konumundan ve GPS'ten birleştirilmiş nihai boylam [derece]
} EKF_State_t;


/*
 * Tanım: Ana İşlenmiş Veri Yapısı (ProcessedData_t)
 * İşlev: Kestirim çekirdeğinin (Filtre Modülünün) tek bir çevrim sonucunda ürettiği
 * tüm ham, kalibre edilmiş, filtrelenmiş sensör verilerini, sensör güven puanlarını ve
 * EKF nihai durum vektörünü tek bir pakette toplayarak diğer modüllere (Lojik Modüller, PID vb.) sunar.
 */
typedef struct {
    double timestamp_s;              // Sistem çalışma zaman damgası [saniye]
    
    SensorChannels_t raw_channels;   // Ham sensör okumalarının standart birim dönüştürülmüş hali
    SensorChannels_t calib_channels; // Sıfır kayma (bias), ölçek (scale) ve termal düzeltmeli veri
    SensorChannels_t filter_channels;// Dijital alçak geçiren ve EKF süzgeçlerinden geçmiş sensör verileri
    
    EKF_State_t ekf_state;           // Genişletilmiş Kalman Filtresi nihai durum kestirimi
    SensorConfidences_t confidences; // Sensörlere özel hesaplanan güvenilirlik katsayıları
    
    // --- DURUM VE BAYRAKLAR ---
    uint8_t ekf_converged;           // EKF yakınsama bayrağı (1: Filtre oturdu/güvenilir, 0: Yakınsama sürüyor)
    uint8_t gps_altitude_fused;      // GPS irtifasının EKF Z ekseni güncellemesinde kullanılıp kullanılmadığı bayrağı
    uint32_t sensor_fault_mask;      // Sensör arıza maskesi (Bit bazlı: 0. bit İvme, 1. bit Jiroskop, 2. bit Baro vb.)
} ProcessedData_t;

#ifdef __cplusplus
}
#endif

#endif /* INC_PROCESSED_DATA_H_ */
