/*
 * filter_config.h
 *
 * Tanım:
 * Filtre modülünün kalibrasyon sıfır kaymalarını, ölçek çarpanlarını, çevresel
 * referanslarını, güven motoru eşiklerini, EKF temel gürültü varyanslarını ve
 * AHRS kuaterniyon kazanç katsayılarını tek bir yerde saklayan yapılandırma dosyasıdır.
 *
 * İşlev:
 * Yazma yetkimiz olan Filtre Modülü içindeki tüm alt bileşenlerin dışarıya
 * bağımlı olmadan sabit katsayıları okumasını ve derlenmesini sağlar.
 */

#ifndef INC_FILTER_CONFIG_H_
#define INC_FILTER_CONFIG_H_

/*
 * Tanım: İvmeölçer Kalibrasyon Sabitleri
 * İşlev: İvmeölçer sensörünün statik sıfır kayma (bias) hatalarını [g] ve ölçek çarpanlarını tanımlar.
 */
#define CALIB_ACC_BIAS_X        0.0000f
#define CALIB_ACC_BIAS_Y        0.0000f
#define CALIB_ACC_BIAS_Z        0.0000f
#define CALIB_ACC_SCALE_X       1.0000f
#define CALIB_ACC_SCALE_Y       1.0000f
#define CALIB_ACC_SCALE_Z       1.0000f
#define CALIB_ACC_THERMAL_DRIFT 0.0000f

/*
 * Tanım: Jiroskop Kalibrasyon Sabitleri
 * İşlev: Jiroskop sensörünün statik sıfır kayma hatalarını [deg/s] ve ölçek çarpanlarını tanımlar.
 */
#define CALIB_GYRO_BIAS_X       0.0000f
#define CALIB_GYRO_BIAS_Y       0.0000f
#define CALIB_GYRO_BIAS_Z       0.0000f
#define CALIB_GYRO_SCALE_X      1.0000f
#define CALIB_GYRO_SCALE_Y      1.0000f
#define CALIB_GYRO_SCALE_Z      1.0000f

/*
 * Tanım: Manyetometre Kalibrasyon Sabitleri
 * İşlev: Manyetometre sensörünün sert demir (hard-iron bias [uT]) ve yumuşak demir ölçek çarpanlarını tanımlar.
 */
#define CALIB_MAG_BIAS_X        0.0000f
#define CALIB_MAG_BIAS_Y        0.0000f
#define CALIB_MAG_BIAS_Z        0.0000f
#define CALIB_MAG_SCALE_X       1.0000f
#define CALIB_MAG_SCALE_Y       1.0000f
#define CALIB_MAG_SCALE_Z       1.0000f

/*
 * Tanım: Çevresel Referans ve Rampa Kalibrasyon Sabitleri
 * İşlev: Barometrik irtifa hesabı, termal kompanzasyon ve fırlatma rampası referans kalibrasyonu adım sayısı.
 */
#define CALIB_TEMP_REF_C        25.0f
#define CALIB_SEA_LEVEL_PA      101325.0f
#define GROUND_CALIB_SAMPLES    50      // Fırlatma rampasında referans irtifa (AGL sıfırlaması) için alınacak numune sayısı

/*
 * Tanım: Güven Motoru Eşik ve Zaman Aşımı Sabitleri
 * İşlev: Sensör verilerindeki anomali tespiti için maksimum sıçrama limitlerini ve
 * donanım zaman damgası (mikrosaniye) zaman aşımı sürelerini tanımlar.
 */
#define CONFIDENCE_MAX_SPIKE_ACC_MPS2    15.0f   // İvmede bir adımda izin verilen en büyük değişim [m/s^2]
#define CONFIDENCE_MAX_SPIKE_GYRO_RADPS  0.8726f // Jiroskopta bir adımda izin verilen en büyük değişim [rad/s] (yaklaşık 50 dps)
#define CONFIDENCE_MAX_SPIKE_BARO_PA     500.0f  // Basınçta bir adımda izin verilen en büyük değişim [Pa]
#define CONFIDENCE_TIMEOUT_US            100000  // Sensör verisi donma zaman aşımı sınırı [mikrosaniye] (100 ms)

/*
 * Tanım: Durum Kestirici (EKF) ve AHRS Temel Gürültü/Kazanç Sabitleri
 * İşlev: Süreç gürültü matrisi (Q), ölçüm gürültü matrisi (R) ve Madgwick AHRS kazancını tanımlar.
 */
#define ESTIMATOR_Q_BASE           0.01f   // Kestirici temel süreç gürültüsü
#define ESTIMATOR_R_POS_BARO       2.50f   // Barometrik irtifa ölçüm gürültü varyansı [m^2]
#define ESTIMATOR_R_POS_GPS        4.00f   // GPS irtifa ve yatay konum ölçüm gürültü varyansı [m^2]
#define ESTIMATOR_R_ACC_BASE       0.25f   // İvme ölçüm gürültü varyansı [m/s^2]^2
#define ATTITUDE_BETA_BASE         0.10f   // Madgwick kuaterniyon AHRS algoritması temel düzeltme kazancı

#endif /* INC_FILTER_CONFIG_H_ */
