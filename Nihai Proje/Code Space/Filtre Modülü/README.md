# RaSAT SIM - Data.h Tabanlı Evrensel ve Eksen Bağımsız Güven Skorlu Süzme Çekirdeği

Bu klasör (`Nihai Proje\Code Space\Filtre Modülü`), RaSAT uçuş bilgisayarı için geliştirilmiş, **Data.h evrensel veri merkezini (`DataCenter`)** kullanan, her verinin eksen bazlı güven puanını `[0.0 - 1.0]` ölçeğinde denetleyen havacılık standartlarında süzme ve yönelim motorudur.

---

## 1. Mimari Tasarım ve Temel Özellikler

* **Tek Evrensel Veri Merkezi (`Data.h` -> `DataCenter`):** Eski çoklu yapı biçimleri tamamen kaldırılmıştır. Sistemin tek veri modeli `DataCenter` yapısıdır.
* **4'lü Kanal Mimarisi (`SensorData` & `DoubleSensorData`):** Her eksen (`acc.x`, `baro.pressure`, `gps.x` vb.) kendi içinde `rawValue` (ham okuma), `calibratedValue` (kalibreli değer), `filteredValue` (nihai süzülmüş çıktı) ve `confidence` (0.0 ile 1.0 arasında anlık güven skoru) barındırır.
* **Eksen Bağımsız Güven Skoru (`confidence`):** Sensörün genel bir güven puanı yerine her eksenin (örneğin sadece `mag.x` veya `gps.z`) kendi bağımsız güven skoru vardır. Bir eksen arızalandığında sadece o eksen süzgeçten dışlanır.
* **Zaman Damgası Kontrolü (`UpdateTime`):** Her sensör grubunda (`Acc`, `Gyro`, `Mag`, `Baro`, `Gps`, `Batt`) ayrı `UpdateTime` bulunur. Veri donması durumunda `ConfidenceEngine_Update` bunu algılar ve güven skorunu anında `0.0f` yapar; bu nedenle ek bir `valid` bayrağı tutmaya gerek kalmaz.

---

## 2. Adım Adım Çalışma Sırası ve Fonksiyon İşlevleri

Sistem çalışırken fonksiyonlar rastgele değil, havacılık matematiğinin gerektirdiği **kesin bir sıra** ile çağrılır. 

### A. Başlatma (Kurulum) Sırası
Sistem açıldığında sadece 1 defa (`System_Init` esnasında) aşağıdan yukarıya doğru şu fonksiyonlar işletilir:

#### 1. `FilterCore_Init(FilterCore_t *pFilter)`
* **Tanım:** Filtre modülünün ana yönetim nesnesini ve tüm alt süzgeçleri kuran üst fonksiyondur.
* **İşlev:** İçerisinde sırasıyla `SensorCalib_Init`, `ConfidenceEngine_Init`, `AttitudeEstimator_Init` ve `StateEstimator_Init` fonksiyonlarını çağırır. Rampa referans irtifası sayaçlarını (`ground_calib_samples_collected`, `ground_calib_baro_sum_pa`, `ground_calib_gps_z_sum_m`) sıfırlar ve `ground_calib_done = 0` bayrağını atayarak fırlatma rampası kalibrasyonunu beklemeye alır.

#### 2. `SensorCalib_Init(SensorCalibConfig_t *pConfig)`
* **Tanım:** Statik kalibrasyon parametrelerini yükleyen başlatma fonksiyonudur.
* **İşlev:** `filter_config.h` dosyasında tanımlı olan ivmeölçer, jiroskop ve manyetometre sıfır kayması (`bias`) ile ölçek çarpanlarını (`scale`) yapıya yükler. Termal kayma katsayısını (`accel_thermal_drift`), referans sıcaklığı (`temp_ref_c`) ve deniz seviyesi basıncını (`sea_level_pa`) varsayılan sabitlerine sabitler.

#### 3. `ConfidenceEngine_Init(ConfidenceEngine_t *pEngine)`
* **Tanım:** Güven motoru bellek alanını sıfırlayan başlatma fonksiyonudur.
* **İşlev:** Sıçrama (`spike`) denetimi yapabilmek için tutulan bir önceki adım sensör okuması belleklerini (`prev_acc_cal`, `prev_gyro_cal`, `prev_mag_cal`, `prev_baro_cal`, `prev_gps_z_cal`) sıfırlar. EKF ölçüm gürültüsü ceza çarpanını (`ekf_r_penalty_factor`) varsayılan başlangıç değeri olan `1.0f` olarak ayarlar.

#### 4. `AttitudeEstimator_Init(AttitudeEstimator_t *pEstimator)`
* **Tanım:** Kuaterniyon yönelim vektörünü sıfırlayan başlatma fonksiyonudur.
* **İşlev:** Kuaterniyon vektörünü roket/model uydu rampada düz duruyormuş gibi birim vektör olan `[q0=1.0f, q1=0.0f, q2=0.0f, q3=0.0f]` durumuna getirir. Madgwick filtresi integral kazancını (`beta`) `filter_config.h` dosyasındaki `AHRS_BETA_GAIN` sabitiyle yükler.

#### 5. `StateEstimator_Init(StateEstimator_t *pEst)`
* **Tanım:** 9 durumlu Genişletilmiş Kalman Filtresi (EKF) matrislerini kuran fonksiyonudur.
* **İşlev:** Durum vektörünü (`state[9]`) sıfırlar. Hata kovaryans matrisi `P` ve süreç gürültü varyansı `Q` matrislerini diyagonal (birim matris formatında) başlatır. Barometre (`R_baro`) ve GPS (`R_gps`) taban ölçüm gürültü varyanslarını atar ve `is_converged = 0` yakınsama bayrağını sıfırlar.

---

### B. Ana Süzme Çevrimi (`System_Loop`) Sırası
Her periyodik döngüde (`System_Loop` içinde) tek bir `FilterCore_Step()` çağrısıyla şu fonksiyonlar **kesin sıra ile** koşturulur:

#### 1. `FilterCore_Step(FilterCore_t *pFilter, const Sensor_Verileri_t *pRawInput, uint32_t current_time_us)`
* **Tanım:** Ana süzme çevrimini yöneten, alt fonksiyonları doğru sırayla koşturan orkestrasyon fonksiyonudur.
* **İşlev:** İlk iş olarak `LoadRawFromSensor` çağrısını yapar. Ardından rampa kalibrasyonu tamamlanmadıysa (`!ground_calib_done`) `FilterCore_RunGroundCalibration` fonksiyonunu işletir. Devamında sırasıyla `SensorCalib_Apply`, `ConfidenceEngine_Update`, `AttitudeEstimator_Update`, `StateEstimator_Predict` ve `StateEstimator_Correct` fonksiyonlarını çağırarak süzme turunu tamamlar.

#### 2. `LoadRawFromSensor(const Sensor_Verileri_t *pRawInput, DataCenter *pDataCenter)`
* **Tanım:** Ham donanım paketini (`Sensor_Verileri_t`) alarak evrensel veri merkezi (`DataCenter`) alanlarına kopyalayan köprü fonksiyonudur.
* **İşlev:** İvmeölçer (`accel_x/y/z_g`), jiroskop (`gyro_x/y/z_dps`), manyetometre (`mag_x/y/z_uT`), barometre (`pressure_pa, temperature_c`), GPS (`latitude_deg -> gps.x, longitude_deg -> gps.y, altitude_m -> gps.z`) ve batarya ölçümlerini `DataCenter` içindeki ilgili blokların **`rawValue`** alanlarına aktarır. Ayrıca her sensör grubunun son okunduğu zamanı (`update_AG`, `update_M`, `update` vb.) ilgili blokların **`UpdateTime`** alanına kaydeder.

#### 3. `FilterCore_RunGroundCalibration(FilterCore_t *pFilter)` *(Sadece ilk 100 çevrimde çalışır)*
* **Tanım:** Fırlatma rampasının deniz seviyesine göre olan referans irtifasını belirleyen otomatik sıfırlama fonksiyonudur.
* **İşlev:** Her çevrimde barometrik basınç (`pressure.rawValue`) ve GPS mutlak irtifasını (`gps.z.rawValue`) toplar ve örnek sayacını (`ground_calib_samples_collected`) bir artırır. Sayaç `FILTER_CALIB_SAMPLES_COUNT` (`100` adet) sınırına ulaştığında aritmetik ortalama alarak `calib_config.sea_level_pa` ile `calib_config.ref_ground_altitude_m` sabitlerini sabitler ve `ground_calib_done = 1` yaparak kalibrasyonu kapatır.

#### 4. `SensorCalib_Apply(const SensorCalibConfig_t *pConfig, DataCenter *pDataCenter)`
* **Tanım:** Ham veriler üzerine statik hata, ofset ve termal telafi denklemlerini uygulayan fonksiyonudur.
* **İşlev:** `DataCenter` içindeki `rawValue` değerlerini okur. İvmeölçer için `(rawValue - bias - thermal_drift) / scale` denklemini koşturur. Jiroskop ve manyetometre için `(rawValue - bias) / scale` işlemlerini yapar. GPS mutlak irtifası (`gps.z.rawValue`) değerinden rampada ölçülen referans irtifa (`ref_ground_altitude_m`) çıkarılarak yer yüzeyine göre AGL irtifa bulunur. Tüm sonuçlar eksenlerin **`calibratedValue`** alanına yazılır ve her eksenin **`confidence`** skoru başlangıçta `1.0f` (%100 tam güven) olarak atanır.

#### 5. `ConfidenceEngine_Update(ConfidenceEngine_t *pEngine, DataCenter *pDataCenter, uint32_t current_time_us)`
* **Tanım:** Her bir eksenin anlık güven skorumunu `[0.0 - 1.0]` ölçeğinde denetleyen ve cezalandıran fonksiyonudur.
* **İşlev:**
  * **Zaman Aşımı (Timeout) Kontrolü:** Her sensör grubunun `UpdateTime` damgası ile anlık zaman (`current_time_us`) arasındaki farkı hesaplar. Fark `CONFIDENCE_TIMEOUT_US` sınırını aşıyorsa o sensörün tüm eksenlerinin `confidence` değerini anında **`0.0f`** yapar (veri donması veya kopması).
  * **Sıçrama (Spike) Kontrolü:** Bir önceki adım ile anlık adım arasındaki `calibratedValue` değişimini (`delta`) eksen bazlı inceler. İvme, açısal hız veya manyetik alanda fiziksel sınırların üzerinde ani bir sıçrama varsa, sadece o eksenin `confidence` skorumunu katsayıyla (örneğin `%50` oranında `0.5f`) çarparak azaltır.
  * **EKF Ceza Çarpanı Üretimi:** Azalan güven skorlarına paralel olarak EKF'nin ölçüm gürültüsünü artırmak için `ekf_r_penalty_factor` katsayısını (`1.0f` ile `50.0f` arasında) belirler.

#### 6. `AttitudeEstimator_Update(AttitudeEstimator_t *pEstimator, DataCenter *pDataCenter, float dt_s)`
* **Tanım:** Kuaterniyon ve Euler (Pitch, Roll, Yaw) yönelim hesaplamalarını koşturan fonksiyonudur.
* **İşlev:** İvmeölçer eksenlerinin güven skorlerini denetler (`confidence >= 0.20f`). Eğer güven yeterli değilse (örneğin roket motor yanması sırasında aşırı sarsıntı varsa), ivmeölçerden gelen yerçekimi doğrulaması iptal edilerek sadece jiroskop integrali çalıştırılır. Kuaterniyon türevleri hesaplanır, zaman adımı (`dt_s`) ile integrali alınır ve `q0..q3` vektörü normalize edilir. Ardından trigonometrik dönüşümlerle Pitch, Roll ve Yaw açıları (`[°]`) türetilerek hem kuaterniyonlar hem de açılar **`DataCenter->estimated`** altına kaydedilir.

#### 7. `StateEstimator_Predict(StateEstimator_t *pEst, DataCenter *pDataCenter, float dt_s)`
* **Tanım:** 9 durumlu Genişletilmiş Kalman Filtresinin (EKF) zaman integraliyle tahminde bulunan birinci adımıdır.
* **İşlev:** İvmeölçerin `calibratedValue` değerlerini `SensorCalib_GToMps2` ile `[m/s^2]` birimine çevirir ve durum vektöründeki (`state[6..8]`) sıfır kaymalarını (`bias`) çıkartır. `S = S0 + V*dt + 0.5*a*dt^2` ve `V = V0 + a*dt` kinematik denklemleriyle konum (`state[0..2] - AGL Yerden Yükseklik`) ve hız (`state[3..5]`) durum vektörlerini bir adım ileri taşır. Kovaryans matrisini (`P = P + Q*dt`) günceller ve ivmeölçer verilerini **`filteredValue`** alanına paslar.

#### 8. `StateEstimator_Correct(StateEstimator_t *pEst, DataCenter *pDataCenter, const ConfidenceEngine_t *pConf)`
* **Tanım:** EKF'nin barometre ve GPS ölçümlerini kullanarak tahmin edilen konumu ve irtifayı ağırlıklı düzelttiği ikinci adımıdır.
* **İşlev:**
  * Barometrik basıncın (`baro.pressure.confidence >= 0.20f`) ve GPS AGL irtifasının (`gps.z.confidence >= 0.20f`) güven skorlarını kontrol eder.
  * Güveni yeterli olan ölçümler için Kalman kazancını (`K`) hesaplar. Güven skoru düşükse veya `ekf_r_penalty_factor` yüksekse, ölçüm gürültüsü (`R`) artırıldığı için Kalman kazancı (`K`) küçülür ve EKF gürültülü sensör yerine kendi kinematik tahminine güvenir.
  * Düzeltme denklemi koşturulduktan sonra nihai AGL irtifa ile konumlar hem **`DataCenter->estimated.pos_x/y/z`** altına hem de barometre/GPS **`filteredValue`** alanlarına yazılarak süzgeç turu tamamlanır.

---

## 3. Ana Çevrimde Kullanım Örneği

```c
#include "filter_core.h"

FilterCore_t g_filter;

void System_Init(void) {
    FilterCore_Init(&g_filter);
}

void System_Loop(const Sensor_Verileri_t *pRawSensorInput, uint32_t current_time_us) {
    // Tüm süzgeçleri tek satırda koştur ve DataCenter verilerini hazırla
    FilterCore_Step(&g_filter, pRawSensorInput, current_time_us);

    // Çıktılara örnek erişim:
    float yaw_deg = g_filter.data_center.estimated.yaw.value;
    float mag_x_conf = g_filter.data_center.mag.x.confidence;
    float agl_height_m = g_filter.data_center.estimated.pos_z.value;
}
```
