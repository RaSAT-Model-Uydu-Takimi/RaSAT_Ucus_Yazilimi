/*
 * Processed_Data.h
 *
 * Tanım:
 * Filtre modülünün tüm alt bileşenleri tarafından kullanılan, her fiziksel
 * veriyi ve yönelim kuaterniyon/Euler açılarını kendi anlık güven skoruyla
 * birlikte taşıyan evrensel veri yapısıdır.
 *
 * İşlev:
 * 1. Tekil float ve double hassasiyetli kanal yapılarını (Channel_t, DoubleChannel_t) tanımlar.
 * 2. TÜRKSAT Model Uydu Telemetri Formatı (79 Byte) ile tam uyumlu olacak şekilde,
 *    gereksiz çift birimleri (_g ve _dps) barındırmayan yalın SensorChannels_t yapısını kurar.
 * 3. 4 boyutlu kuaterniyon (q0, q1, q2, q3) ve 3 boyutlu Euler açılarını (Pitch, Roll, Yaw) barındırır.
 * 4. 9 durumlu EKF ve AHRS çıkışlarını kapsayan ana ProcessedData_t yapısını kurar.
 */

#ifndef INC_PROCESSED_DATA_H_
#define INC_PROCESSED_DATA_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Tanım: Standart Tekli Kanal Yapısı (Channel_t)
 * İşlev: Float hassasiyetindeki her bir fiziksel ölçümü (SI biriminde) ve
 * o verinin anlık güven puanını (%0.0 ile %100.0 arasında) birlikte barındırır.
 */
typedef struct {
    float value;        // Fiziksel ölçüm veya kestirim değeri (SI standart biriminde)
    float confidence;   // Bu veriye özel anlık güven skoru [%0.0 - %100.0]
} Channel_t;

/*
 * Tanım: Yüksek Hassasiyetli Çiftli Kanal Yapısı (DoubleChannel_t)
 * İşlev: Metre altı hassasiyet gerektiren coğrafi koordinat (Enlem/Boylam) verilerini
 * ve bu koordinatların anlık güven puanlarını barındırır.
 */
typedef struct {
    double value;       // Coğrafi koordinat değeri (Ondalıklı Derece - double hassasiyetinde)
    float confidence;   // Bu koordinata özel anlık güven skoru [%0.0 - %100.0]
} DoubleChannel_t;

/*
 * Tanım: Yalınlaştırılmış Sensör Kanalları Toplu Yapısı (SensorChannels_t)
 * İşlev: TÜRKSAT Model Uydu Telemetri formatı ve EKF/AHRS matris gereksinimleriyle
 * tam uyumlu, gereksiz birim tekrarı (_g ve _dps) içermeyen fiziksel kanallar paketidir.
 */
typedef struct {
    // --- 3 EKSENLİ İVMEÖLÇER KANALLARI (SI Birimi: m/s^2) ---
    Channel_t accel_x_mps2;       // X ekseni ivmesi [m/s^2]
    Channel_t accel_y_mps2;       // Y ekseni ivmesi [m/s^2]
    Channel_t accel_z_mps2;       // Z ekseni ivmesi [m/s^2]

    // --- 3 EKSENLİ JİROSKOP KANALLARI (SI Birimi: rad/s) ---
    Channel_t gyro_x_radps;       // X ekseni açısal hızı [rad/s]
    Channel_t gyro_y_radps;       // Y ekseni açısal hızı [rad/s]
    Channel_t gyro_z_radps;       // Z ekseni açısal hızı [rad/s]

    // --- 3 EKSENLİ MANYETOMETRE KANALLARI ---
    Channel_t mag_x_uT;           // X ekseni manyetik alan [uT]
    Channel_t mag_y_uT;           // Y ekseni manyetik alan [uT]
    Channel_t mag_z_uT;           // Z ekseni manyetik alan [uT]

    // --- ÇEVRESEL VE BAROMETRİK İRTİFA/YÜKSEKLİK KANALLARI ---
    Channel_t pressure_pa;        // Atmosferik basınç [Pa] (Telemetri Formatı)
    Channel_t temperature_c;      // Ortam sıcaklığı [°C] (Telemetri Formatı)
    Channel_t baro_altitude_asl_m;// Deniz seviyesine göre mutlak barometrik irtifa [m]
    Channel_t baro_height_agl_m;  // Fırlatma rampasına göre barometrik yerden yükseklik [m] (Telemetri Yükseklik)

    // --- GPS VE NAVİGASYON KANALLARI ---
    DoubleChannel_t latitude_deg;    // GPS Enlem [derece] (Telemetri Formatı)
    DoubleChannel_t longitude_deg;   // GPS Boylam [derece] (Telemetri Formatı)
    Channel_t gps_altitude_asl_m;    // Deniz seviyesine göre mutlak GPS irtifa [m] (Telemetri Formatı)
    Channel_t gps_height_agl_m;      // Fırlatma rampasına göre GPS yerden yükseklik [m]
    Channel_t gps_speed_mps;         // GPS Yere göre hız [m/s]
    Channel_t gps_course_deg;        // GPS Rota/Yön [derece]

    // --- BATARYA VE GÜÇ KANALLARI ---
    Channel_t v_bat_v;            // Batarya gerilimi [V] (Telemetri Formatı)
    Channel_t i_bat_a;            // Batarya akımı [A]
} SensorChannels_t;

/*
 * Tanım: 9 Durumlu EKF ve Kuaterniyon AHRS Çıktı Yapısı (EKF_State_t)
 * İşlev: Durum kestiricisinin eylemsizlik tahmin denklemleri, sensör füzyonu ve
 * Madgwick kuaterniyon entegrasyonu sonucunda hesapladığı Kartezyen konum, hız, ivme,
 * 4D kuaterniyon ve 3D Euler yönelim açılarını (Pitch, Roll, Yaw) güven skorlarıyla saklar.
 */
typedef struct {
    // Yerel Kartezyen konum kestirimleri [m]
    Channel_t pos_x_m;
    Channel_t pos_y_m;
    Channel_t pos_z_agl_m;       // Fırlatma rampasına göre EKF süzülmüş net yerden yükseklik [m]
    Channel_t altitude_asl_m;    // Deniz seviyesine göre EKF süzülmüş mutlak irtifa [m]

    // Sürekli 3 eksenli hız kestirimleri [m/s]
    Channel_t vel_x_mps;
    Channel_t vel_y_mps;
    Channel_t vel_z_mps;         // Dikey tırmanma/iniş hızı [m/s] (Telemetri İniş Hızı)

    // Süzülmüş doğrusal ivme kestirimleri [m/s^2]
    Channel_t acc_x_mps2;
    Channel_t acc_y_mps2;
    Channel_t acc_z_mps2;        // Yerçekimi çıkartılmış net dikey ivme [m/s^2]

    // --- KUATERNİYON VE EULER YÖNELİM AÇILARI (AHRS) ---
    Channel_t q0;                // Kuaterniyon w (reel bileşen)
    Channel_t q1;                // Kuaterniyon x (i bileşeni)
    Channel_t q2;                // Kuaterniyon y (j bileşeni)
    Channel_t q3;                // Kuaterniyon z (k bileşeni)

    Channel_t pitch_deg;         // Yunuslama (Pitch) açısı [-90.0, +90.0 derece] (Telemetri Formatı)
    Channel_t roll_deg;          // Yatış (Roll) açısı [-180.0, +180.0 derece] (Telemetri Formatı)
    Channel_t yaw_deg;           // Sapma/Pusula (Yaw/Heading) açısı [0.0, 360.0 derece] (Telemetri Formatı)

    // Yerel Kartezyen konumdan coğrafi koordinata dönüştürülmüş küresel koordinatlar
    DoubleChannel_t fused_latitude_deg;
    DoubleChannel_t fused_longitude_deg;
} EKF_State_t;

/*
 * Tanım: Nihai İşlenmiş Veri Paketi (ProcessedData_t)
 * İşlev: Ham verilerin SI dönüştürülmüş halini, kalibrasyonlu halini, EKF durumunu,
 * yönelim açılarını ve ana uçuş bilgisayarının kullanacağı filtrelenmiş son kanalları toplar.
 */
typedef struct {
    SensorChannels_t raw_channels;    // Ham sensör verilerinin SI formundaki hali
    SensorChannels_t calib_channels;  // Kalibrasyon, ofset ve termal telafi uygulanmış hali
    EKF_State_t ekf_state;            // 9 durumlu EKF ve Kuaterniyon AHRS durum kestirimi
    SensorChannels_t filter_channels; // Uçuş kontrolcüsü ve telemetri için nihai süzülmüş kanallar

    double timestamp_s;               // Sistem çevrim zaman damgası [saniye]
    float ref_ground_altitude_m;      // Fırlatma rampasının mutlak deniz seviyesi irtifası [m]
    uint32_t sensor_fault_mask;       // Arızalı sensör veya kanalları bit bit işaretleyen maske
    uint8_t ref_ground_set;           // Rampa yüksekliği kalibrasyon bayrağı (1: Tamamlandı, 0: Ölçülüyor)
    uint8_t ekf_converged;            // EKF matrisinin yakınsama durumu bayrağı (1: Yakınsadı, 0: Bekliyor)
    uint8_t gps_altitude_fused;       // GPS dikey yükseklik versinin EKF'ye dahil edilme bayrağı
} ProcessedData_t;

#ifdef __cplusplus
}
#endif

#endif /* INC_PROCESSED_DATA_H_ */
