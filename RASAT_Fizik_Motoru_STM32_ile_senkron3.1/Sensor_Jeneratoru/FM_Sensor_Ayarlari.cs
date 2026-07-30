using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using static System.Windows.Forms.VisualStyles.VisualStyleElement.Rebar;

namespace RASAT_Fizik_Motoru_STM32_ile_senkron3._1
{
    public static class FM_Sensor_Ayarlari
    {
        /// <summary>
        /// Bu fonksiyon, sensör simülatörünün "Ayar Dosyası" olarak görev yapar. 
        /// Değerleri istediğiniz gibi değiştirip simülasyonu test edebilirsiniz.
        /// </summary>
        public static void ProfilleriYukle(
            ref IMU_Modulu IMU, 
            ref Baro_Modulu Baro, 
            ref GPS_Modulu GPS, 
            ref Guc_Modulu Guc)
        {
            // =================================================================
            // 1. IMU (MPU9250 & AK8963) SENSÖRÜ AYARLARI
            // =================================================================
            IMU = new IMU_Modulu(); 
            IMU.IletisimKoptu = false;

            // --- İvmeölçer (Accelerometer) --- // Hatalar m/s^2 tipindeki değere uygulanır.
            IMU.AccX.BagilHataCarpani    = 1.0; 
            IMU.AccX.MutlakHata          = 0.0; 
            IMU.AccX.TermalGurultuStdDev = 0.2500; 
            IMU.AccX.DarbeIhtimaliPct    = 0.0; 
            IMU.AccX.DarbeSiddeti        = 0.0;
            
            IMU.AccY.BagilHataCarpani    = 1.0; 
            IMU.AccY.MutlakHata          = 0.0; 
            IMU.AccY.TermalGurultuStdDev = 0.2500; 
            IMU.AccY.DarbeIhtimaliPct    = 0.0; 
            IMU.AccY.DarbeSiddeti        = 0.0;
            
            IMU.AccZ.BagilHataCarpani    = 1.0; 
            IMU.AccZ.MutlakHata          = 0.00; 
            IMU.AccZ.TermalGurultuStdDev = 0.2500; 
            IMU.AccZ.DarbeIhtimaliPct    = 0.0; 
            IMU.AccZ.DarbeSiddeti        = 0.0;

            // --- Jiroskop (Gyroscope) ---  // Hatalar dps tipindeki değere uygulanır.
            IMU.GyroX.BagilHataCarpani    = 1.0; 
            IMU.GyroX.MutlakHata          = 0.01; 
            IMU.GyroX.TermalGurultuStdDev = 0.0010; 
            IMU.GyroX.DarbeIhtimaliPct    = 0.0; 
            IMU.GyroX.DarbeSiddeti        = 0.0;
            
            IMU.GyroY.BagilHataCarpani    = 1.0; 
            IMU.GyroY.MutlakHata          = 0.01; 
            IMU.GyroY.TermalGurultuStdDev = 0.0010; 
            IMU.GyroY.DarbeIhtimaliPct    = 0.0; 
            IMU.GyroY.DarbeSiddeti        = 0.0;
            
            IMU.GyroZ.BagilHataCarpani    = 1.0; 
            IMU.GyroZ.MutlakHata          = 0.01; 
            IMU.GyroZ.TermalGurultuStdDev = 0.0010; 
            IMU.GyroZ.DarbeIhtimaliPct    = 0.0; 
            IMU.GyroZ.DarbeSiddeti        = 0.0;

            // --- Manyetometre (Magnetometer) ---  // Hatalar uT tipindeki değere uygulanır.
            IMU.MagX.BagilHataCarpani    = 1.0; 
            IMU.MagX.MutlakHata          = 0.0; 
            IMU.MagX.TermalGurultuStdDev = 0.0500; 
            IMU.MagX.DarbeIhtimaliPct    = 0.0; 
            IMU.MagX.DarbeSiddeti        = 0.0;
            
            IMU.MagY.BagilHataCarpani    = 1.0; 
            IMU.MagY.MutlakHata          = 0.0; 
            IMU.MagY.TermalGurultuStdDev = 0.0500; 
            IMU.MagY.DarbeIhtimaliPct    = 0.0; 
            IMU.MagY.DarbeSiddeti        = 0.0;
            
            IMU.MagZ.BagilHataCarpani    = 1.0; 
            IMU.MagZ.MutlakHata          = 0.0; 
            IMU.MagZ.TermalGurultuStdDev = 0.0500; 
            IMU.MagZ.DarbeIhtimaliPct    = 0.0; 
            IMU.MagZ.DarbeSiddeti        = 0.0;


            // =================================================================
            // 2. BAROMETRE (BMP280) SENSÖRÜ AYARLARI
            // =================================================================
            Baro = new Baro_Modulu();
            Baro.IletisimKoptu = false;
            
            Baro.Basinc.BagilHataCarpani    = 1.0; // Hatalar Pascal tipindeki değere uygulanır.
            Baro.Basinc.MutlakHata          = 0.0; 
            Baro.Basinc.TermalGurultuStdDev = 2.0; 
            Baro.Basinc.DarbeIhtimaliPct    = 0.0; 
            Baro.Basinc.DarbeSiddeti        = 0.0;
            
            Baro.Sicaklik.BagilHataCarpani    = 1.0; // Hatalar derece_Celcius tipindeki değere uygulanır.
            Baro.Sicaklik.MutlakHata          = 0.0; 
            Baro.Sicaklik.TermalGurultuStdDev = 0.0; 
            Baro.Sicaklik.DarbeIhtimaliPct    = 0.0; 
            Baro.Sicaklik.DarbeSiddeti        = 0.0;


            // =================================================================
            // 3. GPS (NEO-M8N) SENSÖRÜ AYARLARI
            // =================================================================
            GPS = new GPS_Modulu();
            GPS.IletisimKoptu = false;

            GPS.Enlem.BagilHataCarpani    = 1.0; 
            GPS.Enlem.MutlakHata          = 0.0; 
            GPS.Enlem.TermalGurultuStdDev = 0.0100; 
            GPS.Enlem.DarbeIhtimaliPct    = 0.0; 
            GPS.Enlem.DarbeSiddeti        = 0.0;
            
            GPS.Boylam.BagilHataCarpani    = 1.0; 
            GPS.Boylam.MutlakHata          = 0.0; 
            GPS.Boylam.TermalGurultuStdDev = 0.0100; 
            GPS.Boylam.DarbeIhtimaliPct    = 0.0; 
            GPS.Boylam.DarbeSiddeti        = 0.0;
            
            GPS.Irtifa.BagilHataCarpani    = 1.0; 
            GPS.Irtifa.MutlakHata          = 0.0; 
            GPS.Irtifa.TermalGurultuStdDev = 0.0100; 
            GPS.Irtifa.DarbeIhtimaliPct    = 0.0; 
            GPS.Irtifa.DarbeSiddeti        = 0.0;
            
            GPS.Yatay_Hiz.BagilHataCarpani    = 1.0; 
            GPS.Yatay_Hiz.MutlakHata          = 0.0; // +-0.5 m/s
            GPS.Yatay_Hiz.TermalGurultuStdDev = 0.0100; 
            GPS.Yatay_Hiz.DarbeIhtimaliPct    = 0.0; 
            GPS.Yatay_Hiz.DarbeSiddeti        = 0.0;


            // =================================================================
            // 4. GÜÇ (INA219) SENSÖRÜ AYARLARI
            // =================================================================
            Guc = new Guc_Modulu();
            Guc.IletisimKoptu = false;

            Guc.Voltaj.BagilHataCarpani    = 1.0; 
            Guc.Voltaj.MutlakHata          = 0.0; 
            Guc.Voltaj.TermalGurultuStdDev = 0.05; 
            Guc.Voltaj.DarbeIhtimaliPct    = 0.0; 
            Guc.Voltaj.DarbeSiddeti        = 0.0;
            
            Guc.Akim.BagilHataCarpani    = 1.0; 
            Guc.Akim.MutlakHata          = 0.0; 
            Guc.Akim.TermalGurultuStdDev = 0.1; 
            Guc.Akim.DarbeIhtimaliPct    = 0.0; 
            Guc.Akim.DarbeSiddeti        = 0.0;
        }






        // =========================================================================
        // 1. DİNAMİK HATA PROFİLİ VERİ YAPILARI (EN ALT KATMAN)
        // =========================================================================
        public class OlcumHataProfili
        {
            public double BagilHataCarpani = 1.0;     // Ölçek (Scale Error) - 1.0 kusursuz
            public double MutlakHata = 0.0;           // Kayma (Bias / Offset)
            public double TermalGurultuStdDev = 0.0;  // Sürekli Gürültü Standart Sapması
            public double DarbeIhtimaliPct = 0.0;     // % Spike olasılığı (0.0 - 1.0)
            public double DarbeSiddeti = 0.0;         // Spike genliği

            // Gerçek değeri alıp tüm hataları uygulayarak bozan çekirdek fonksiyon
            public double DegeriBoz(double gercekDeger)
            {
                double termal = FM_Sensor_Simulatoru.GenerateGaussianNoise(0, TermalGurultuStdDev);

                double spike = 0.0;
                if (FM_Sensor_Simulatoru.rand.NextDouble() < DarbeIhtimaliPct)
                {
                    spike = (FM_Sensor_Simulatoru.rand.NextDouble() > 0.5) ? DarbeSiddeti : -DarbeSiddeti;
                }

                return (gercekDeger * BagilHataCarpani) + MutlakHata + termal + spike;
            }
        }





        // =========================================================================
        // 2. DONANIM MODÜLLERİ (ÇİP / HABERLEŞME KATMANI)
        // =========================================================================
        public class IMU_Modulu
        {
            public bool IletisimKoptu = false; // Kilitlenme / Donma durumu

            public OlcumHataProfili AccX = new OlcumHataProfili();
            public OlcumHataProfili AccY = new OlcumHataProfili();
            public OlcumHataProfili AccZ = new OlcumHataProfili();

            public OlcumHataProfili GyroX = new OlcumHataProfili();
            public OlcumHataProfili GyroY = new OlcumHataProfili();
            public OlcumHataProfili GyroZ = new OlcumHataProfili();

            public OlcumHataProfili MagX = new OlcumHataProfili();
            public OlcumHataProfili MagY = new OlcumHataProfili();
            public OlcumHataProfili MagZ = new OlcumHataProfili();
        }

        public class Baro_Modulu
        {
            public bool IletisimKoptu = false;

            public OlcumHataProfili Basinc = new OlcumHataProfili(); // Hata doğrudan sensör zarına etki eden basınca (Pascal) uygulanır
            public OlcumHataProfili Sicaklik = new OlcumHataProfili(); // Santigrat
        }

        public class GPS_Modulu
        {
            public bool IletisimKoptu = false;

            public OlcumHataProfili Enlem = new OlcumHataProfili(); // Derece
            public OlcumHataProfili Boylam = new OlcumHataProfili(); // Derece
            public OlcumHataProfili Irtifa = new OlcumHataProfili(); // Metre
            public OlcumHataProfili Yatay_Hiz = new OlcumHataProfili(); // m/s
        }

        public class Guc_Modulu
        {
            public bool IletisimKoptu = false; // INA219 vb. arızası

            public OlcumHataProfili Voltaj = new OlcumHataProfili();
            public OlcumHataProfili Akim = new OlcumHataProfili();
        }




    }
}
