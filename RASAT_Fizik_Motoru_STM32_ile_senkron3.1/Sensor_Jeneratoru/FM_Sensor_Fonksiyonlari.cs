using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace RASAT_Fizik_Motoru_STM32_ile_senkron3._1
{
    public static class FM_Sensor_Fonksiyonlari
    {
        // Sabitler
        private const double MPU9250_ACCEL_LSB_PER_G = 2048.0;     // ±16g aralığı için LSB çarpanı
        private const double MPU9250_GYRO_LSB_PER_DPS = 16.4;      // ±2000 dps aralığı için LSB çarpanı
        private const double AK8963_MAG_UT_PER_LSB = 0.15;         // AK8963 16-bit çözünürlük çarpanı (uT/LSB)




        public static void Ivme_Uret(Uydu_Dinamik_Durum_t durum, FM_Sensor_Ayarlari.IMU_Modulu imu, ref Alim_Paketi_t gecmis_paket)
        {
            // Eğer iletişim koptuysa işlem yapma, geçmiş paketteki eski değerler kalsın (Donma etkisi)
            if (imu.IletisimKoptu)
            {
                return; 
            }

            // 1. Dünya referanslı ivmeyi al ve Yerçekimini (Gravity) ekle
            Vektor_t ivme_dunya = durum.Ivme_m_s2_dunya;
            ivme_dunya = ivme_dunya + new Vektor_t(0,0,FM_Fizik_Sabitler.GRAVITY_M_S2,Kordinat_Sistemi_t.DUNYA_ENU); 

            // 2. Dünya eksenindeki bu ivmeyi, Uydunun o anki yönelimine (Kuaterniyon) göre Gövde (Body) eksenine çevir
            Vektor_t ivme_body = durum.Yonelim.DunyadanGovdeyeCevir(ivme_dunya);

            // 3. Değerleri G cinsine çevirmek için yerçekimi sabitine böl
            double g_bolucu = FM_Fizik_Sabitler.GRAVITY_M_S2;

            // 4. Her bir eksene kendi hata profilini uygulayıp, ardından STM32'nin okuyacağı (short) LSB değerine dönüştür
            gecmis_paket.acc_x = (short)((imu.AccX.DegeriBoz(ivme_body.X) / g_bolucu) * MPU9250_ACCEL_LSB_PER_G);
            gecmis_paket.acc_y = (short)((imu.AccY.DegeriBoz(ivme_body.Y) / g_bolucu) * MPU9250_ACCEL_LSB_PER_G);
            gecmis_paket.acc_z = (short)((imu.AccZ.DegeriBoz(ivme_body.Z) / g_bolucu) * MPU9250_ACCEL_LSB_PER_G);
        }

        



        public static void Jiroskop_Uret(Uydu_Dinamik_Durum_t durum, FM_Sensor_Ayarlari.IMU_Modulu imu, ref Alim_Paketi_t gecmis_paket)
        {
            if (imu.IletisimKoptu)
            {
                return;
            }

            // 1. Açısal hız gövde (body) ekseninde radyan/saniye cinsindendir.
            Vektor_t gyro_body_rad = durum.Acisal_Hiz_rad_s_body;

            // 2. Radyanı dereceye (degrees per second - dps) çevir
            double rad_to_deg = 180.0 / Math.PI;
            double dps_x = gyro_body_rad.X * rad_to_deg;
            double dps_y = gyro_body_rad.Y * rad_to_deg;
            double dps_z = gyro_body_rad.Z * rad_to_deg;

            // 3. Hata profilini dps üzerinden uygula ve LSB'ye dönüştür
            gecmis_paket.gyro_x = (short)(imu.GyroX.DegeriBoz(dps_x) * MPU9250_GYRO_LSB_PER_DPS);
            gecmis_paket.gyro_y = (short)(imu.GyroY.DegeriBoz(dps_y) * MPU9250_GYRO_LSB_PER_DPS);
            gecmis_paket.gyro_z = (short)(imu.GyroZ.DegeriBoz(dps_z) * MPU9250_GYRO_LSB_PER_DPS);
        }

       



        
        public static void Manyetometre_Uret(Uydu_Dinamik_Durum_t durum, FM_Sensor_Ayarlari.IMU_Modulu imu, ref Alim_Paketi_t gecmis_paket)
        {
            if (imu.IletisimKoptu)
            {
                return;
            }

            // 1. Dünyanın Sabit Manyetik Alanı (Örnek referans: 24uT, 2uT, 40uT)
            Vektor_t mag_earth = FM_Fizik_Sabitler.MAG_FIELD_dunya;
            
            // 2. Dünyadan Gövdeye (Body) Çevir
            Vektor_t mag_body = durum.Yonelim.DunyadanGovdeyeCevir(mag_earth);
            
            // 3. Eksen Çaprazlaması ve Hata Uygulaması (AK8963 Donanım Taklidi: X=Y, Y=X, Z=-Z)
            double mag_x_raw = imu.MagY.DegeriBoz(mag_body.Y); 
            double mag_y_raw = imu.MagX.DegeriBoz(mag_body.X); 
            double mag_z_raw = imu.MagZ.DegeriBoz(-mag_body.Z); 
            
            // 4. LSB'ye Dönüştür (0.15 uT per LSB)
            gecmis_paket.mag_x = (short)(mag_x_raw / AK8963_MAG_UT_PER_LSB);
            gecmis_paket.mag_y = (short)(mag_y_raw / AK8963_MAG_UT_PER_LSB);
            gecmis_paket.mag_z = (short)(mag_z_raw / AK8963_MAG_UT_PER_LSB);
        }

        




        public static void Barometre_Uret(Uydu_Dinamik_Durum_t durum, FM_Sensor_Ayarlari.Baro_Modulu baro, ref Alim_Paketi_t gecmis_paket)
        {
            if (baro.IletisimKoptu)
            {
                return;
            }

            // 1. Gerçek irtifayı fizik motorundan (Z ekseni konumu) al
            double gercek_irtifa = durum.Konum_m_dunya.Z;
            
            // Gerçek sıcaklığı standart atmosfer modeline göre hesapla (Deniz seviyesi 15°C, düşüş 6.5°C/km)
            double gercek_sicaklik = 15.0 - (0.0065 * gercek_irtifa); 

            // 2. Gerçek irtifayı Standart Barometrik Formül ile Kusursuz Basınca (Pascal) çevir
            // Formül: P = P0 * (1 - (L * h) / T0) ^ (g * M / (R * L))
            double kusursuz_basinc = 101325.0 * Math.Pow(1.0 - (0.0065 * gercek_irtifa) / 288.15, 5.25588);

            // 3. Sensör hata profilini DOĞRUDAN ölçülen fiziksel büyüklüğe (Basınç) uygula
            double bozuk_basinc = baro.Basinc.DegeriBoz(kusursuz_basinc);

            // Güvenlik: Alt sınır koruması
            if (bozuk_basinc < 0.0 || double.IsNaN(bozuk_basinc)) bozuk_basinc = 0.0;
            gecmis_paket.basinc = (uint)bozuk_basinc;

            // 4. Sıcaklık (°C) hata profilini uygula ve x100 (virgülden sonraki 2 haneyi koruma) ile short'a atama
            double bozuk_sicaklik = baro.Sicaklik.DegeriBoz(gercek_sicaklik);
            gecmis_paket.sicaklik = (short)(bozuk_sicaklik * 100.0);
        }







        public static void GPS_Uret(Uydu_Dinamik_Durum_t durum, FM_Sensor_Ayarlari.GPS_Modulu gps, ref Alim_Paketi_t gecmis_paket)
        {
            if (gps.IletisimKoptu) return;

            // 1. Referans Noktası (Aksaray Atış Alanı)
            double ref_enlem = 38.8300000;
            double ref_boylam = 33.2900000;

            // 2. ENU Metre -> Enlem/Boylam Derece (Dünya yariçapi = ~111.139 km/derece)
            double dunya_yari_cap_m = 111139.0;
            double gercek_enlem = ref_enlem + (durum.Konum_m_dunya.Y / dunya_yari_cap_m);
            double cos_enlem = Math.Cos(ref_enlem * Math.PI / 180.0);
            double gercek_boylam = ref_boylam + (durum.Konum_m_dunya.X / (dunya_yari_cap_m * cos_enlem));

            // 3. Konum Hatalarını Uygula
            double bozuk_enlem = gps.Enlem.DegeriBoz(gercek_enlem);
            double bozuk_boylam = gps.Boylam.DegeriBoz(gercek_boylam);
            
            // 4. LSB Formatına (x 10^7) dönüştür
            gecmis_paket.gps_lat = (int)(bozuk_enlem * 10000000.0);
            gecmis_paket.gps_lon = (int)(bozuk_boylam * 10000000.0);

            // 5. İrtifa (Metre -> Milimetre)
            double gercek_irtifa = durum.Konum_m_dunya.Z;
            double bozuk_irtifa = gps.Irtifa.DegeriBoz(gercek_irtifa);
            gecmis_paket.gps_alt = (int)(bozuk_irtifa * 1000.0);

            // 6. YATAY HIZ (Speed Over Ground) Hesaplama ve Ekleme
            // NMEA $GPRMC mesajı yere göre hızı verir. (X ve Y hızlarının bileşkesi)
            double gercek_yatay_hiz = Math.Sqrt((durum.Hiz_m_s_dunya.X * durum.Hiz_m_s_dunya.X) + 
                                                (durum.Hiz_m_s_dunya.Y * durum.Hiz_m_s_dunya.Y));
            
            double bozuk_yatay_hiz = gps.Yatay_Hiz.DegeriBoz(gercek_yatay_hiz);

            // NMEA gibi sadece pozitif hız üretiriz. cm/s (x100) formatında STM32'ye yollarız.
            if (bozuk_yatay_hiz < 0.0) { bozuk_yatay_hiz = 0.0; }
            gecmis_paket.gps_vel = (short)(bozuk_yatay_hiz * 100.0);
        }






        public static void Guc_Uret(Uydu_Dinamik_Durum_t durum, FM_Sensor_Ayarlari.Guc_Modulu guc, ref Alim_Paketi_t gecmis_paket)
        {
            if (guc.IletisimKoptu) return;

            // 1. Fiziksel kusursuz değerleri oku
            double gercek_voltaj = durum.Batarya_Voltaj_V;
            double gercek_akim = durum.Batarya_Akim_A;

            // 2. Sensör gürültüsü / kalibrasyon hatalarını uygula
            double bozuk_voltaj = guc.Voltaj.DegeriBoz(gercek_voltaj);
            double bozuk_akim = guc.Akim.DegeriBoz(gercek_akim);

            // Alt sınır koruması
            if (bozuk_voltaj < 0.0) bozuk_voltaj = 0.0;
            if (bozuk_akim < 0.0) bozuk_akim = 0.0;

            // 3. Donanım yapısına dönüştür (mV ve mA birimlerinde)
            // Örn: 16.8V -> 16800, 45.2A -> 45200
            gecmis_paket.bat_v = (ushort)(bozuk_voltaj * 1000.0);
            gecmis_paket.bat_a = (int)(bozuk_akim * 1000.0);
        }
    }
}
