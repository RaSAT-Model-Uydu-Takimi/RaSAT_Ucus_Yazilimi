using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace RASAT_Fizik_Motoru_STM32_ile_senkron3._1
{
    






    // =========================================================================
    // 3. ANA SENSÖR SİMÜLATÖRÜ SINIFI
    // =========================================================================
    public static class FM_Sensor_Simulatoru
    {
        public static readonly Random rand = new Random();

        // Modül Hata Profillerine Dışarıdan Erişim İçin
        public static FM_Sensor_Ayarlari.IMU_Modulu IMU = new FM_Sensor_Ayarlari.IMU_Modulu();
        public static FM_Sensor_Ayarlari.Baro_Modulu Baro = new FM_Sensor_Ayarlari.Baro_Modulu();
        public static FM_Sensor_Ayarlari.GPS_Modulu GPS = new FM_Sensor_Ayarlari.GPS_Modulu();
        public static FM_Sensor_Ayarlari.Guc_Modulu Guc = new FM_Sensor_Ayarlari.Guc_Modulu();

        // Sensörlerin kopması durumunda eski değerleri hatırlamak için hafıza
        private static Alim_Paketi_t gecmis_paket = new Alim_Paketi_t();

        // LSB Dönüşüm Çarpanları (STM32 Sürücü Uyumlu)
        private const double MPU9250_ACCEL_LSB_PER_G = 2048.0;     // ±16g
        private const double MPU9250_GYRO_LSB_PER_DPS = 16.4;      // ±2000 dps
        private const double AK8963_MAG_UT_PER_LSB = 0.149938;     // 16-bit

        // ------------------------------------------------------------------
        // Gaussian Gürültü Fonksiyonu (Box-Muller)
        // ------------------------------------------------------------------
        public static double GenerateGaussianNoise(double mean, double stdDev)
        {
            if (stdDev <= 0.0) return mean;
            double u1 = 1.0 - rand.NextDouble();
            double u2 = 1.0 - rand.NextDouble();
            double randStdNormal = Math.Sqrt(-2.0 * Math.Log(u1)) * Math.Sin(2.0 * Math.PI * u2);
            return mean + stdDev * randStdNormal;
        }







        // ------------------------------------------------------------------
        // VARSAYILAN (FABRİKA) SENSÖR PROFİLLERİNİ BAŞLAT (Ayarlar dosyasından)
        // ------------------------------------------------------------------
        public static void FabrikaAyarlarinaDon()
        {
            // Tüm uzun atamalar FM_Sensor_Ayarlari.cs dosyasına taşındı.
            FM_Sensor_Ayarlari.ProfilleriYukle(ref IMU, ref Baro, ref GPS, ref Guc);
        }







        // ------------------------------------------------------------------
        // PAKET DOLDURMA ANA FONKSİYONU
        // ------------------------------------------------------------------
        public static void Paket_Icin_Veri_Uret(Uydu_Dinamik_Durum_t durum, ref Alim_Paketi_t paket)
        {
            // İvmeölçer verilerini hesapla ve gecmis_paket'e kaydet
            FM_Sensor_Fonksiyonlari.Ivme_Uret(durum, IMU, ref gecmis_paket);
            
            // Jiroskop verilerini hesapla
            FM_Sensor_Fonksiyonlari.Jiroskop_Uret(durum, IMU, ref gecmis_paket);
            
            // Manyetometre verilerini hesapla
            FM_Sensor_Fonksiyonlari.Manyetometre_Uret(durum, IMU, ref gecmis_paket);

            // Barometre verilerini hesapla
            FM_Sensor_Fonksiyonlari.Barometre_Uret(durum, Baro, ref gecmis_paket);

            // GPS verilerini hesapla
            FM_Sensor_Fonksiyonlari.GPS_Uret(durum, GPS, ref gecmis_paket);

            // Güç (Batarya) verilerini hesapla
            FM_Sensor_Fonksiyonlari.Guc_Uret(durum, Guc, ref gecmis_paket);

            // Hesaplanan gecmis_paket değerlerini asıl gönderilecek pakete aktar
            paket.acc_x = gecmis_paket.acc_x;
            paket.acc_y = gecmis_paket.acc_y;
            paket.acc_z = gecmis_paket.acc_z;
            
            paket.gyro_x = gecmis_paket.gyro_x;
            paket.gyro_y = gecmis_paket.gyro_y;
            paket.gyro_z = gecmis_paket.gyro_z;
            
            paket.mag_x = gecmis_paket.mag_x;
            paket.mag_y = gecmis_paket.mag_y;
            paket.mag_z = gecmis_paket.mag_z;

            paket.basinc = gecmis_paket.basinc;
            paket.sicaklik = gecmis_paket.sicaklik;

            paket.gps_lat = gecmis_paket.gps_lat;
            paket.gps_lon = gecmis_paket.gps_lon;
            paket.gps_alt = gecmis_paket.gps_alt;
            paket.gps_vel = gecmis_paket.gps_vel;

            paket.bat_v = gecmis_paket.bat_v;
            paket.bat_a = gecmis_paket.bat_a;
        }
    }
}
