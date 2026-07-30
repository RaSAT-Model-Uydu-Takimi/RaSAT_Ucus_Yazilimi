using System;

namespace RASAT_Fizik_Motoru_STM32_ile_senkron3._1
{
    // ========================================================================
    // 1. REFERANS SİSTEMLERİ (COORDINATE FRAMES)
    // ========================================================================
    public enum Kordinat_Sistemi_t
    {
        DUNYA_ENU = 0,
        GOVDE_BODY = 1
    }

    // ========================================================================
    // 2. AKILLI 3D VEKTÖR YAPISI (SMART VECTOR STRUCT)
    // ========================================================================
    public struct Vektor_t
    {
        public double X;
        public double Y;
        public double Z;
        public Kordinat_Sistemi_t ReferansSistemi;

        public Vektor_t(double x, double y, double z, Kordinat_Sistemi_t refSistemi)
        {
            X = x; Y = y; Z = z; ReferansSistemi = refSistemi;
        }

        public static Vektor_t operator +(Vektor_t v1, Vektor_t v2)
        {
            if (v1.ReferansSistemi != v2.ReferansSistemi)
                throw new InvalidOperationException($"Fizik Hatası: Eksen uyumsuzluğu ({v1.ReferansSistemi} vs {v2.ReferansSistemi})!");
            return new Vektor_t(v1.X + v2.X, v1.Y + v2.Y, v1.Z + v2.Z, v1.ReferansSistemi);
        }

        public static Vektor_t operator -(Vektor_t v1, Vektor_t v2)
        {
            if (v1.ReferansSistemi != v2.ReferansSistemi)
                throw new InvalidOperationException("Fizik Hatası: Eksen uyumsuzluğu!");
            return new Vektor_t(v1.X - v2.X, v1.Y - v2.Y, v1.Z - v2.Z, v1.ReferansSistemi);
        }

        public static Vektor_t operator *(Vektor_t v, double skaler) => new Vektor_t(v.X * skaler, v.Y * skaler, v.Z * skaler, v.ReferansSistemi);
        public static Vektor_t operator *(double skaler, Vektor_t v) => v * skaler;
        
        public static Vektor_t operator /(Vektor_t v, double skaler)
        {
            if (skaler == 0) throw new DivideByZeroException("Fizik Hatası: Sıfıra bölme!");
            return new Vektor_t(v.X / skaler, v.Y / skaler, v.Z / skaler, v.ReferansSistemi);
        }

        public static Vektor_t operator -(Vektor_t v) => new Vektor_t(-v.X, -v.Y, -v.Z, v.ReferansSistemi);

        public double Buyukluk() => Math.Sqrt(X * X + Y * Y + Z * Z);

        public Vektor_t Normalize()
        {
            double mag = Buyukluk();
            if (mag < 1e-9) return new Vektor_t(0, 0, 0, ReferansSistemi);
            return this / mag;
        }

        public static double NoktaCarpim(Vektor_t v1, Vektor_t v2)
        {
            if (v1.ReferansSistemi != v2.ReferansSistemi) throw new InvalidOperationException("Eksen uyumsuzluğu!");
            return (v1.X * v2.X) + (v1.Y * v2.Y) + (v1.Z * v2.Z);
        }

        public static Vektor_t CaprazCarpim(Vektor_t v1, Vektor_t v2)
        {
            if (v1.ReferansSistemi != v2.ReferansSistemi) throw new InvalidOperationException("Eksen uyumsuzluğu!");
            return new Vektor_t(
                (v1.Y * v2.Z) - (v1.Z * v2.Y),
                (v1.Z * v2.X) - (v1.X * v2.Z),
                (v1.X * v2.Y) - (v1.Y * v2.X),
                v1.ReferansSistemi);
        }
    }

    // ========================================================================
    // 3. YÖNELİM VE ROTASYON YAPISI (QUATERNION STRUCT)
    // ========================================================================
    public struct Kuaterniyon_t
    {
        public double W; public double X; public double Y; public double Z;

        public Kuaterniyon_t(double w, double x, double y, double z) { W = w; X = x; Y = y; Z = z; }

        public static Kuaterniyon_t BirimKuaterniyon => new Kuaterniyon_t(1.0, 0.0, 0.0, 0.0);

        public void Normalize()
        {
            double mag = Math.Sqrt(W * W + X * X + Y * Y + Z * Z);
            if (mag > 1e-9) { W /= mag; X /= mag; Y /= mag; Z /= mag; } else { this = BirimKuaterniyon; }
        }

        public Vektor_t GovdedenDunyayaCevir(Vektor_t govdeVektoru)
        {
            if (govdeVektoru.ReferansSistemi != Kordinat_Sistemi_t.GOVDE_BODY)
                throw new ArgumentException("Sadece GOVDE_BODY vektörlerini çevirebilir!");

            double num = X * 2.0; double num2 = Y * 2.0; double num3 = Z * 2.0;
            double num4 = X * num; double num5 = Y * num2; double num6 = Z * num3;
            double num7 = X * num2; double num8 = X * num3; double num9 = Y * num3;
            double num10 = W * num; double num11 = W * num2; double num12 = W * num3;

            double rx = (1.0 - (num5 + num6)) * govdeVektoru.X + (num7 - num12) * govdeVektoru.Y + (num8 + num11) * govdeVektoru.Z;
            double ry = (num7 + num12) * govdeVektoru.X + (1.0 - (num4 + num6)) * govdeVektoru.Y + (num9 - num10) * govdeVektoru.Z;
            double rz = (num8 - num11) * govdeVektoru.X + (num9 + num10) * govdeVektoru.Y + (1.0 - (num4 + num5)) * govdeVektoru.Z;

            return new Vektor_t(rx, ry, rz, Kordinat_Sistemi_t.DUNYA_ENU);
        }

        public Vektor_t DunyadanGovdeyeCevir(Vektor_t dunyaVektoru)
        {
            if (dunyaVektoru.ReferansSistemi != Kordinat_Sistemi_t.DUNYA_ENU)
                throw new ArgumentException("Sadece DUNYA_ENU vektörlerini çevirebilir!");

            double num = -X * 2.0; double num2 = -Y * 2.0; double num3 = -Z * 2.0;
            double num4 = (-X) * num; double num5 = (-Y) * num2; double num6 = (-Z) * num3;
            double num7 = (-X) * num2; double num8 = (-X) * num3; double num9 = (-Y) * num3;
            double num10 = W * num; double num11 = W * num2; double num12 = W * num3;

            double rx = (1.0 - (num5 + num6)) * dunyaVektoru.X + (num7 - num12) * dunyaVektoru.Y + (num8 + num11) * dunyaVektoru.Z;
            double ry = (num7 + num12) * dunyaVektoru.X + (1.0 - (num4 + num6)) * dunyaVektoru.Y + (num9 - num10) * dunyaVektoru.Z;
            double rz = (num8 - num11) * dunyaVektoru.X + (num9 + num10) * dunyaVektoru.Y + (1.0 - (num4 + num5)) * dunyaVektoru.Z;

            return new Vektor_t(rx, ry, rz, Kordinat_Sistemi_t.GOVDE_BODY);
        }

        public void ToEulerDerece(out double roll, out double pitch, out double yaw)
        {
            double sinr_cosp = 2 * (W * X + Y * Z);
            double cosr_cosp = 1 - 2 * (X * X + Y * Y);
            roll = Math.Atan2(sinr_cosp, cosr_cosp) * (180.0 / Math.PI);

            double sinp = 2 * (W * Y - Z * X);
            if (Math.Abs(sinp) >= 1) pitch = Math.CopySign(Math.PI / 2, sinp) * (180.0 / Math.PI);
            else pitch = Math.Asin(sinp) * (180.0 / Math.PI);

            double siny_cosp = 2 * (W * Z + X * Y);
            double cosy_cosp = 1 - 2 * (Y * Y + Z * Z);
            yaw = Math.Atan2(siny_cosp, cosy_cosp) * (180.0 / Math.PI);
        }
    }




    // ========================================================================
    // 4. UYDU MEKANİK PARAMETRELERİ (STATİK CONFİG TİPİ)
    // ========================================================================
    public struct Uydu_Mekanik_Parametreler_t
    {
        public double Kutle_kg;
        public double Ixx; public double Iyy; public double Izz;
        public double Ixy; public double Ixz; public double Iyz;
        public double Alan_X_m2; public double Alan_Y_m2; public double Alan_Z_m2;
        public Vektor_t Aerodinamik_Merkez_Kaymasi_m_body; 
        public double Aktif_Parasut_Alani_m2;
    }




    // ========================================================================
    // 5. UYDU DİNAMİK DURUMU (KİNEMATİK STATE)
    // ========================================================================
    public struct Uydu_Dinamik_Durum_t
    {
        public Vektor_t Konum_m_dunya;
        public Vektor_t Hiz_m_s_dunya;
        public Vektor_t Ivme_m_s2_dunya;
        public Kuaterniyon_t Yonelim;
        public Vektor_t Acisal_Hiz_rad_s_body;
        public Vektor_t Acisal_Ivme_rad_s2_body;
        public double Batarya_Voltaj_V;
        public double Batarya_Akim_A;
        public double Tuketilen_mAh;

        public double Motor_1_Gazi; //stm32den gelen motor sinyalini direkt olarak motora yansıtmıyoruz, low pass filterdan geçirerek bir gecikme yaratıyoruz.
        public double Motor_2_Gazi; //LPF için de eski değerin hafızada tutulması gerektiğinden uydunun dinamik durumu içine motorun anlık çalışma yüzdeliğini (gazını) dahil ediyoruz.
        public double Motor_3_Gazi; //Motor gazı demek motor rpm'inin 0 1 arasında normalize edilmiş halidir. Motor kuvvetinin normalize edilmiş hali değildir. ingilizcesi throttle.
        public double Motor_4_Gazi;

        public static Uydu_Dinamik_Durum_t VarsayilanOlustur()
        {
            return new Uydu_Dinamik_Durum_t
            {
                Konum_m_dunya = new Vektor_t(0, 0, FM_Fizik_Sabitler.ZEMIN_RAKIMI_M + 1000, Kordinat_Sistemi_t.DUNYA_ENU),
                Hiz_m_s_dunya = new Vektor_t(0, 0, 0, Kordinat_Sistemi_t.DUNYA_ENU),
                Ivme_m_s2_dunya = new Vektor_t(0, 0, 0, Kordinat_Sistemi_t.DUNYA_ENU),
                Yonelim = Kuaterniyon_t.BirimKuaterniyon,
                Acisal_Hiz_rad_s_body = new Vektor_t(0, 0, 0, Kordinat_Sistemi_t.GOVDE_BODY),
                Acisal_Ivme_rad_s2_body = new Vektor_t(0, 0, 0, Kordinat_Sistemi_t.GOVDE_BODY),
                Batarya_Voltaj_V = FM_Fizik_Sabitler.BATARYA_DOLU_VOLTAJ_V,
                Batarya_Akim_A = 0,
                Tuketilen_mAh = 0
            };
        }
    }



    // ========================================================================
    // 6. KONTROL GİRDİLERİ VE PROTOKOL SEÇİMİ
    // ========================================================================
    public enum Motor_Protokol_t
    {
        PWM_1000_2000 = 0, // Standart Analog PWM (1000=Dur, 2000=Tam Gaz)
        DSHOT_11BIT = 1    // Dijital DShot (48=Min Gaz, 2047=Tam Gaz, 0-47=Dur/Telemetri)
    }

    public struct Uydu_Kontrol_Girdisi_t
    {
        public ushort mi1_raw; // 1. Motor ham komutu
        public ushort mi2_raw; // 2. Motor ham komutu
        public ushort mi3_raw; // 3. Motor ham komutu
        public ushort mi4_raw; // 4. Motor ham komutu
        public Motor_Protokol_t Protokol; // Arayüzden seçilen protokol (PWM veya DShot)       
        public bool SEP;   // Ayrılma komutu verildi mi?
        public bool SGM;   // SİGMA kanatları açıldı mı?
        public bool APAM;  // APAM acil durum kapağı patlatıldı mı?
        public bool BUZZ;  // BUZZER ötüyor mu?

        // ---> ÇÖZÜM: Fonksiyonu struct'ın içine aldık ve 'public static' yaptık! <---
        /// <summary>
        /// Ham motor sinyalini (PWM veya DShot) 0.0 (Durdu) ile 1.0 (%100 Tam Gaz) aralığına normalize eder.
        /// </summary>
        public static double HamSinyali_Gaza_Cevir(ushort ham_deger, Motor_Protokol_t protokol)
        {
            if (protokol == Motor_Protokol_t.PWM_1000_2000)
            {
                if (ham_deger <= 1000) return 0.0;
                if (ham_deger >= 2000) return 1.0;
                return (ham_deger - 1000.0) / 1000.0;
            }
            else // DSHOT_11BIT
            {
                if (ham_deger <= 47) return 0.0;
                if (ham_deger >= 2047) return 1.0;
                return (ham_deger - 48.0) / 1999.0;
            }
        }
    }

    // ========================================================================
    // 7. SİMÜLASYON ÇEVRE ŞARTLARI VE HATA ENJEKSİYONU (FAULT INJECTION)
    // ========================================================================
    public struct Simulasyon_Cevre_Sartlari_t
    {
        // --- 1. Çevresel Koşullar ---
        public Vektor_t Ruzgar_Hizi_m_s_dunya;      // [DUNYA_ENU] - Sürekli esen rüzgar

        // --- 2. Fiziksel Sarsıntılar ve Darbeler ---
        public Vektor_t Harici_Kuvvet_N_dunya;      // [DUNYA_ENU] - Anlık tokatlama/itme
        public Vektor_t Harici_Tork_Nm_body;       // [GOVDE_BODY] - Anlık devirici tork

        // --- 3. Aktüatör (Motor) Hataları ---
        // Oransal güç kaybı (Örn: Sargı yanması, ESC direnci) -> 1.0 = Normal, 0.5 = Yarı Güç
        public double Motor_1_Verim_Carpani;
        public double Motor_2_Verim_Carpani;
        public double Motor_3_Verim_Carpani;
        public double Motor_4_Verim_Carpani;

        // Tavan sınırı / Satürasyon -> 1.0 = Sınır Yok, 0.5 = Gelen sinyal %50'yi aşamaz
        public double Motor_1_Maks_Guc_Siniri;
        public double Motor_2_Maks_Guc_Siniri;
        public double Motor_3_Maks_Guc_Siniri;
        public double Motor_4_Maks_Guc_Siniri;

        // Varsayılan (Hatası Sağlam) değerlerle yapıyı oluşturmak için yardımcı metod
        public static Simulasyon_Cevre_Sartlari_t VarsayilanOlustur()
        {
            return new Simulasyon_Cevre_Sartlari_t
            {
                Ruzgar_Hizi_m_s_dunya = new Vektor_t(0, 0, 0, Kordinat_Sistemi_t.DUNYA_ENU),
                Harici_Kuvvet_N_dunya = new Vektor_t(0, 0, 0, Kordinat_Sistemi_t.DUNYA_ENU),
                Harici_Tork_Nm_body = new Vektor_t(0, 0, 0, Kordinat_Sistemi_t.GOVDE_BODY),
                
                Motor_1_Verim_Carpani = 1.0, Motor_2_Verim_Carpani = 1.0, 
                Motor_3_Verim_Carpani = 1.0, Motor_4_Verim_Carpani = 1.0,
                
                Motor_1_Maks_Guc_Siniri = 1.0, Motor_2_Maks_Guc_Siniri = 1.0,
                Motor_3_Maks_Guc_Siniri = 1.0, Motor_4_Maks_Guc_Siniri = 1.0
            };
        }
    }


}
