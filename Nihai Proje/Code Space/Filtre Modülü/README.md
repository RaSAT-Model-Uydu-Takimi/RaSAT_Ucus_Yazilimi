# RaSAT SIM - Modüler 3 Eksenli C Kestirim, Kalibrasyon ve Loglama Kütüphanesi

Bu klasör (`SensorAnalizi\C_Modular_3Axis_EKF`), **RaSAT SIM** uçuş analiz ve simülasyon sistemleri için gömülü sistem (STM32, ARM, Uçuş Bilgisayarı) uyumlu, **C99 standartlarında**, sıfır dinamik bellek ayırmalı (`zero-malloc`) modüler sensör işleme motorudur.

---

## 🏗️ Modüler Mimari (Dosya Yapısı)

Her bir işlev, karmaşıklığı önlemek ve gömülü sistemlere kolayca entegre olabilmek için ayrı `.h` / `.c` modüllerine ayrılmıştır:

| Modül Dosyası | Görev ve İçerik |
| :--- | :--- |
| **`processed_data.h`** | **17 Kanal Sensör Paketi** (`AccX..Z, GyroX..Z, MagX..Z, Basinc, Sicaklik, GpsLat, GpsLon, GpsAlt, GpsVel, BatV, BatA`) hem **`Real` (Gerçek / Referans)**, hem **`Raw` (Ham / Hatalı)**, hem de **`Filtered` (EKF Çıktısı)** olarak taşıyan ana `ProcessedData_t` yapısını barındırır. |
| **`sensor_calibration.h/c`** | **Mutlak Hata (Bias)**, **Bağıl Hata (% Scale)**, **Termal Kayma (`Drift/°C`)**, beyaz gürültü ve ani darbe şoklarını hem simüle eder hem de ters kalibrasyon denklemiyle statik düzeltme uygular. |
| **`confidence_engine.h/c`** | Sensörlerin **donma (`Stuck`)**, **ani sıçrama (`Spike/Shock`)** ve arıza durumlarını denetleyerek **%0 - %100 arası Güven Puanı (`Confidence Score`)** hesaplar ve EKF $R$ matrisini dinamik olarak cezalandırır. |
| **`ekf_3axis.h/c`** | **9 Durumlu 3 Eksenli Gelişmiş Kalman Filtresi** ($X = [x, y, z, v_x, v_y, v_z, a_x, a_y, a_z]^T$). İvmeölçer eylemsizlik tahminiyle zaman güncellemesi ve Barometre/GPS/İvmeölçer konum düzeltmesi yapar. |
| **`csv_logger.h/c`** | Üretilen tüm `Real`, `Raw`, `Filtered` ve `Confidence` verilerini **noktalı virgül (`;`) ayırıcılı** ve **56 sütunlu** olarak log dosyasına kaydeder. |
| **`main_test.c`** | 1000 adımlık (10 saniye) kinematik uçuş simülasyonunu koşturan ana test programıdır. |
| **`build_and_run.bat`** | Windows üzerinde tek tıkla C99 derlemesi ve test logu (`processed_flight_data.csv`) üretme scriptidir. |

---

## 📊 CSV Log Çıktı Formatı (`processed_flight_data.csv` - 56 Sütun)

Log dosyası **Noktalı Virgül (`;`)** ile ayrılmıştır ve her sensör grubu için **Real $\rightarrow$ Raw $\rightarrow$ Filtered** sıralamasıyla tam **56 sütundan** oluşur:

```text
Zaman;Real_AccX;Real_AccY;Real_AccZ;Raw_AccX;Raw_AccY;Raw_AccZ;Filt_AccX;Filt_AccY;Filt_AccZ;Real_GyroX;Real_GyroY;Real_GyroZ;Raw_GyroX;Raw_GyroY;Raw_GyroZ;Filt_GyroX;Filt_GyroY;Filt_GyroZ;Real_MagX;Real_MagY;Real_MagZ;Raw_MagX;Raw_MagY;Raw_MagZ;Filt_MagX;Filt_MagY;Filt_MagZ;Real_Basinc;Raw_Basinc;Filt_Basinc;Real_Sicaklik;Raw_Sicaklik;Filt_Sicaklik;Real_GpsLat;Real_GpsLon;Real_GpsAlt;Real_GpsVel;Raw_GpsLat;Raw_GpsLon;Raw_GpsAlt;Raw_GpsVel;Filt_GpsLat;Filt_GpsLon;Filt_GpsAlt;Filt_GpsVel;Real_BatV;Real_BatA;Raw_BatV;Raw_BatA;Filt_BatV;Filt_BatA;Baro_Guven;Acc_Guven;Gyro_Guven;Gps_Guven
```
