/*
 * Data.h
 *
 * Tanım:
 * Sensorlerden Veri Okur Onları Bilrşe
 *
 * İşlev:
 * 1. Tekil float ve double hassasiyetli kanal yapılarını (Channel_t, DoubleChannel_t) tanımlar.
 * 2. TÜRKSAT Model Uydu Telemetri Formatı (79 Byte) ile tam uyumlu olacak şekilde,
 *    gereksiz çift birimleri (_g ve _dps) barındırmayan yalın SensorChannels_t yapısını kurar.
 * 3. 4 boyutlu kuaterniyon (q0, q1, q2, q3) ve 3 boyutlu Euler açılarını (Pitch, Roll, Yaw) barındırır.
 * 4. 9 durumlu EKF ve AHRS çıkışlarını kapsayan ana ProcessedData_t yapısını kurar.
 */

#ifndef INC_DATA_H_
#define INC_DATA_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
/*
 * Tanım: Standart Tekli Kanal Yapısı 
 * İşlev: Float hassasiyetindeki her bir fiziksel ölçümü (SI biriminde) ve
 * o verinin anlık güven puanını (%0.0 ile %100.0 arasında) birlikte barındırır.
 */
typedef struct {
    float rawValue;        // Fiziksel ölçüm veya kestirim değeri (SI standart biriminde)
    float calibratedValue;  // Kalibrasyon, ofset ve termal telafi uygulanmış değer
    float filteredValue;    // Filtrelenmiş Veri
    float confidence;   // Bu veriye özel anlık güven skoru [0-1]
} SensorData;

/*
 * Tanım: Yüksek Hassasiyetli Çiftli Kanal Yapısı (DoubleChannel_t)
 * İşlev: Metre altı hassasiyet gerektiren coğrafi koordinat (Enlem/Boylam) verilerini
 * ve bu koordinatların anlık güven puanlarını barındırır.
 */
typedef struct {
    double rawValue;        // Fiziksel ölçüm veya kestirim değeri (SI standart biriminde)
    double calibratedValue;  // Kalibrasyon, ofset ve termal telafi uygulanmış değer
    double filteredValue;    // Filtrelenmiş Veri
    float confidence;   // Bu veriye özel anlık güven skoru [0-1]
} DoubleSensorData;

/*
 * Tanım: SensorDatalardan üreten yapılar
 * İşlev: 
 *
 */
typedef struct {
    float value;         // Farklı sensorlerden gelen total verinin kestirimi
    float confidence;    // Bu veriye özel anlık güven skoru [0-1]
} DerivedData;

typedef struct {
    SensorData x;       // X ekseni ivmesi [g]
    SensorData y;       // Y ekseni ivmesi [g]
    SensorData z;       // Z ekseni ivmesi [g]
    uint32_t UpdateTime; // Son güncelleme zamanı (ms)
} Acc;

typedef struct {
    SensorData x;       // X ekseni açısal hızı [dps]
    SensorData y;       // Y ekseni açısal hızı [dps]
    SensorData z;       // Z ekseni açısal hızı [dps]
    uint32_t UpdateTime;   // Son güncelleme zamanı (ms)
} Gyro;

typedef struct {
    SensorData x;        // X ekseni manyetik alan [uT]
    SensorData y;        // Y ekseni manyetik alan [uT]
    SensorData z;        // Z ekseni manyetik alan [uT]
    uint32_t UpdateTime;    // Son güncelleme zamanı (ms)
} Mag;

typedef struct {
    DoubleSensorData x;       // Enlem [°]
    DoubleSensorData y;       // Boy
    SensorData  z;            // Yükseklik [m]
    SensorData course;        // Rota
    float HDOP;               // HDOP = Horizontal Dilution of Precision - Yatay Hassasiyet Çarpanı
    uint8_t satelliteCount;   // Görünen uydu sayısı
    uint8_t fixQuality;       // 0: Fix Yok, 1: GPS Fix, 2: DGPS Fix
    SensorData speed;         // Yere göre hız [m/s]
    

    uint32_t UpdateTime;      // Son güncelleme zamanı (ms)

}Gps;

typedef struct {
    SensorData press; // Basınç [Pa]
    SensorData temp; // Sıcaklık [°C]
    uint32_t UpdateTime; // Son güncelleme zamanı (ms)
} Baro;

typedef struct {
    SensorData battVolt; // Batarya gerilimi [V]
    SensorData battCurr; // Batarya akımı [A]
    uint32_t UpdateTime; // Son güncelleme zamanı (ms)
} Batt;




typedef struct {
    DerivedData pos_x; // X ekseni konum [m]
    DerivedData pos_y; // Y ekseni konum [m]  
    DerivedData pos_z; // Z ekseni konum [m] 

    DerivedData vel_x; // X ekseni hız [m/s]
    DerivedData vel_y; // Y ekseni hız [m/s]
    DerivedData vel_z; // Z ekseni hız [m/s]

    DerivedData a_x; // X ekseni ivme [m/s^2]
    DerivedData a_y; // Y ekseni ivme [m/s^2]
    DerivedData a_z; // Z ekseni ivme [m/s^2]

    // 
    DerivedData q0; // Kuaterniyon w (real)
    DerivedData q1; // Kuaterniyon x
    DerivedData q2; // Kuaterniyon y
    DerivedData q3; // Kuaterniyon z
    
    // 
    DerivedData pitch; // Pitch açısı [°]
    DerivedData roll;  // Roll açısı [°]
    DerivedData yaw;   // Yaw açısı [°]
} EstimatedDatas;


typedef struct {
    Acc acc;                   // 3 Eksenli İvmeölçer (Accelerometer) Kanalları
    Gyro gyro;                 // 3 Eksenli Jiroskop (Gyroscope) Kanalları
    Mag mag;                   // 3 Eksenli Manyetometre (Magnetometer) Kanalları
    Baro baro;                 // İrtifa Sensörü Kanalları
    Gps gps;                   // GPS ve Navigasyon Kanalları
    Batt batt;                 // Batarya Kanalları

    EstimatedDatas estimated;   // Türetilmiş Veriler
} DataCenter ;


#ifdef __cplusplus
}
#endif

#endif /* INC_DATA_H_ */

