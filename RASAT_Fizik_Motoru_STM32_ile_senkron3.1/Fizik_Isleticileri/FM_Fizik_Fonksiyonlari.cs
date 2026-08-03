using System;

namespace RASAT_Fizik_Motoru_STM32_ile_senkron3._1
{
    
    public static class FM_Fizik_Fonksiyonlari
    {
        
        public static Vektor_t Yercekimi_Kuvveti_Hesapla(double kutle)
        {
            return new Vektor_t(0, 0, -1.0 * kutle * FM_Fizik_Sabitler.GRAVITY_M_S2, Kordinat_Sistemi_t.DUNYA_ENU);
        }

        



        public static Vektor_t[] Motor_Tekil_Itkilerini_Hesapla(ref Uydu_Dinamik_Durum_t durum, Uydu_Kontrol_Girdisi_t girdi, double v_pil, Simulasyon_Cevre_Sartlari_t cevre, double deltaT_s)
        {
            // Ham sinyalleri 0.0 - 1.0 arası gaz oranına (Throttle Ratio) çevir
            double g1_hedef = Uydu_Kontrol_Girdisi_t.HamSinyali_Gaza_Cevir(girdi.mi1_raw, girdi.Protokol);
            double g2_hedef = Uydu_Kontrol_Girdisi_t.HamSinyali_Gaza_Cevir(girdi.mi2_raw, girdi.Protokol);
            double g3_hedef = Uydu_Kontrol_Girdisi_t.HamSinyali_Gaza_Cevir(girdi.mi3_raw, girdi.Protokol);
            double g4_hedef = Uydu_Kontrol_Girdisi_t.HamSinyali_Gaza_Cevir(girdi.mi4_raw, girdi.Protokol);

            // SATÜRASYON HATASI: Motorların ulaşabileceği tavan limiti kırpması
            if (g1_hedef > cevre.Motor_1_Maks_Guc_Siniri) g1_hedef = cevre.Motor_1_Maks_Guc_Siniri;
            if (g2_hedef > cevre.Motor_2_Maks_Guc_Siniri) g2_hedef = cevre.Motor_2_Maks_Guc_Siniri;
            if (g3_hedef > cevre.Motor_3_Maks_Guc_Siniri) g3_hedef = cevre.Motor_3_Maks_Guc_Siniri;
            if (g4_hedef > cevre.Motor_4_Maks_Guc_Siniri) g4_hedef = cevre.Motor_4_Maks_Guc_Siniri;

            // LOW-PASS FILTER (PT1) - Motor Ataleti (Gecikmesi)
            double katsayi = deltaT_s / FM_Fizik_Sabitler.MOTOR_TEPKI_SURESI_TAU_S;
            if (katsayi > 1.0) katsayi = 1.0; // Matematiksel koruma

            durum.Motor_1_Gazi += (g1_hedef - durum.Motor_1_Gazi) * katsayi;
            durum.Motor_2_Gazi += (g2_hedef - durum.Motor_2_Gazi) * katsayi;
            durum.Motor_3_Gazi += (g3_hedef - durum.Motor_3_Gazi) * katsayi;
            durum.Motor_4_Gazi += (g4_hedef - durum.Motor_4_Gazi) * katsayi;

            double g1 = durum.Motor_1_Gazi;
            double g2 = durum.Motor_2_Gazi;
            double g3 = durum.Motor_3_Gazi;
            double g4 = durum.Motor_4_Gazi;

            // RPM_gercek = Gaz * V_pil * KV * 0.788 (Yük altındaki gerçek devir)
            double rpm_carpan = v_pil * FM_Fizik_Sabitler.MOTOR_KV * FM_Fizik_Sabitler.MOTOR_YUK_VERIMI;
            double rpm1 = g1 * rpm_carpan;
            double rpm2 = g2 * rpm_carpan;
            double rpm3 = g3 * rpm_carpan;
            double rpm4 = g4 * rpm_carpan;

            // F = k * RPM^2 * Verim_Carpani (Newton cinsinden kuvvetler, doğrusal güç kaybı hatasıyla)
            double f1 = FM_Fizik_Sabitler.MOTOR_ITKI_KATSAYISI * (rpm1 * rpm1) * cevre.Motor_1_Verim_Carpani;
            double f2 = FM_Fizik_Sabitler.MOTOR_ITKI_KATSAYISI * (rpm2 * rpm2) * cevre.Motor_2_Verim_Carpani;
            double f3 = FM_Fizik_Sabitler.MOTOR_ITKI_KATSAYISI * (rpm3 * rpm3) * cevre.Motor_3_Verim_Carpani;
            double f4 = FM_Fizik_Sabitler.MOTOR_ITKI_KATSAYISI * (rpm4 * rpm4) * cevre.Motor_4_Verim_Carpani;

            // 3. Tekil İtki Kuvvetlerini hesapla ve dizi olarak döndür
            return new Vektor_t[] {
                new Vektor_t(0,0,f1,Kordinat_Sistemi_t.GOVDE_BODY),
                new Vektor_t(0,0,f2,Kordinat_Sistemi_t.GOVDE_BODY),
                new Vektor_t(0,0,f3,Kordinat_Sistemi_t.GOVDE_BODY),
                new Vektor_t(0,0,f4,Kordinat_Sistemi_t.GOVDE_BODY),
            };
        }




        public static void Batarya_Durumu_Guncelle(ref Uydu_Dinamik_Durum_t durum, Uydu_Kontrol_Girdisi_t girdi, double deltaT_s)
        {
            // BATARYA VOLTAJI VE AKIM HESAPLAMALARI (CAPACITY DRAIN + VOLTAGE SAG)
            // 1. Kapasite (mAh) İntegrasyonu (Bir önceki adımın akımını kullanarak - Explicit Euler)
            durum.Tuketilen_mAh = durum.Tuketilen_mAh + (durum.Batarya_Akim_A * deltaT_s * 1000.0 / 3600.0);

            // 2. Kalan kapasite oranına göre Açık Devre Voltajı (Lineer yaklaşım)
            double kalanOran = 1.0 - (durum.Tuketilen_mAh / FM_Fizik_Sabitler.BATARYA_KAPASITE_MAH);
            if (kalanOran < 0.0) kalanOran = 0.0;
            double acik_devre_voltaji = FM_Fizik_Sabitler.BATARYA_BOS_VOLTAJ_V + (kalanOran * (FM_Fizik_Sabitler.BATARYA_DOLU_VOLTAJ_V - FM_Fizik_Sabitler.BATARYA_BOS_VOLTAJ_V));

            // 3. Batarya voltajı azaldıkça motorların çekebileceği maksimum akım da düşer (Ohm kanunu V = I*R_sargı)
            double voltaj_verim_carpani = acik_devre_voltaji / FM_Fizik_Sabitler.BATARYA_DOLU_VOLTAJ_V;
            double guncel_motor_maks_akim = FM_Fizik_Sabitler.MOTOR_MAKS_AKIM_A * voltaj_verim_carpani;

            // 4. Anlık toplam akım (Motorlar + Aviyonik)
            double g1 = Uydu_Kontrol_Girdisi_t.HamSinyali_Gaza_Cevir(girdi.mi1_raw, girdi.Protokol);
            double g2 = Uydu_Kontrol_Girdisi_t.HamSinyali_Gaza_Cevir(girdi.mi2_raw, girdi.Protokol);
            double g3 = Uydu_Kontrol_Girdisi_t.HamSinyali_Gaza_Cevir(girdi.mi3_raw, girdi.Protokol);
            double g4 = Uydu_Kontrol_Girdisi_t.HamSinyali_Gaza_Cevir(girdi.mi4_raw, girdi.Protokol);

            durum.Batarya_Akim_A = (((g1 * g1) + (g2 * g2) + (g3 * g3) + (g4 * g4)) * guncel_motor_maks_akim) + FM_Fizik_Sabitler.ELEKTRONIK_BOS_AKIM_A;

            // 5. İç dirence bağlı anlık voltaj çökmesi (Voltage Sag) eklentisi
            durum.Batarya_Voltaj_V = acik_devre_voltaji - (durum.Batarya_Akim_A * FM_Fizik_Sabitler.BATARYA_IC_DIRENC_OHM);
            if (durum.Batarya_Voltaj_V < 0.0) durum.Batarya_Voltaj_V = 0.0;
        }







        public static Vektor_t Aerodinamik_Kuvvet_Hesapla(Uydu_Dinamik_Durum_t mevcutDurum, Uydu_Mekanik_Parametreler_t sabitler, Vektor_t v_wind_dunya)
        {
            // Dünya eksenindeki hızları uydunun GÖVDE (BODY) eksenine çeviriyoruz
            Vektor_t v_body = mevcutDurum.Yonelim.DunyadanGovdeyeCevir(mevcutDurum.Hiz_m_s_dunya);
            Vektor_t v_wind_body = mevcutDurum.Yonelim.DunyadanGovdeyeCevir(v_wind_dunya);

            // Yönsel yüzey alanları
            double alan_x = sabitler.Alan_X_m2;
            double alan_y = sabitler.Alan_Y_m2;
            double alan_z = sabitler.Alan_Z_m2 + sabitler.Aktif_Parasut_Alani_m2;

            double rho = FM_Fizik_Sabitler.HAVA_YOGUNLUGU_RHO;
            double cd = FM_Fizik_Sabitler.SURTUNME_KATSAYISI_CD;

            // Gövde ekseninde bileşen bazlı sürüklenme kuvveti: F = 0.5 * rho * Cd * A * v_rel * |v_rel|
            double f_aero_x = 0.5 * rho * cd * alan_x * ((v_wind_body.X - v_body.X) * Math.Abs(v_wind_body.X - v_body.X));
            double f_aero_y = 0.5 * rho * cd * alan_y * ((v_wind_body.Y - v_body.Y) * Math.Abs(v_wind_body.Y - v_body.Y));
            double f_aero_z = 0.5 * rho * cd * alan_z * ((v_wind_body.Z - v_body.Z) * Math.Abs(v_wind_body.Z - v_body.Z));

            Vektor_t F_aero_body = new Vektor_t(f_aero_x, f_aero_y, f_aero_z, Kordinat_Sistemi_t.GOVDE_BODY);

            // Dış dünyada (F_net) ve İç dünyada (T_net) aynı veriyi kullanabilmek için
            // Dünya eksenine BURADA çevirmiyoruz! Doğrudan GOVDE (BODY) ekseninde döndürüyoruz.
            return F_aero_body;
        }








        public static Vektor_t[] Zemin_Tekil_Kuvvetlerini_Hesapla(Uydu_Dinamik_Durum_t mevcutDurum)
        {
            // 8 temas noktası için ayrı ayrı kuvvet tutacak diziyi oluşturuyoruz
            Vektor_t[] f_zemin_noktalar = new Vektor_t[FM_Fizik_Sabitler.Temas_Noktalari.Length];


            // Zemin Yay (Hooke), Sönümleme ve Sürtünme Katsayıları
            double k_zemin = FM_Fizik_Sabitler.ZEMIN_YAY_KATSAYISI;
            double c_zemin = FM_Fizik_Sabitler.ZEMIN_SONUMLEME_KATSAYISI;
            double fric_zemin = FM_Fizik_Sabitler.ZEMIN_SURTUNME_KATSAYISI;
            double zemin_rakimi = FM_Fizik_Sabitler.ZEMIN_RAKIMI_M;


            // Uydunun dönüş hızını Dünya (ENU) eksenine çeviriyoruz 
            // Neden? Çünkü V_nokta = V_merkez + W x R formülünde W (Açısal hız) Dünya ekseninde olmalıdır.
            Vektor_t acisalHiz_Dunya = mevcutDurum.Yonelim.GovdedenDunyayaCevir(mevcutDurum.Acisal_Hiz_rad_s_body);


            for (int i = 0; i < FM_Fizik_Sabitler.Temas_Noktalari.Length; i++)
            {

                // 1. Temas noktasının lokal (Gövde) konumu
                Vektor_t r_temas_lokal = FM_Fizik_Sabitler.Temas_Noktalari[i];

                // 2. Noktayı Dünya eksenine çevir ve mutlak Z (rakım) konumunu bul
                Vektor_t r_temas_dunya = mevcutDurum.Yonelim.GovdedenDunyayaCevir(r_temas_lokal);
                double noktaZ_Rakim = (mevcutDurum.Konum_m_dunya + r_temas_dunya).Z;

                // 3. Nokta zeminin altına girdi mi?
                if (noktaZ_Rakim < zemin_rakimi)
                {

                    // A) Ne kadar gömüldü? (Yay/Tepki kuvveti için x mesafesi)
                    double batma = zemin_rakimi - noktaZ_Rakim;

                    // B) Noktanın dünya eksenindeki anlık hızını bul (Sönümleme ve sürtünme için)
                    Vektor_t v_nokta_dunya = mevcutDurum.Hiz_m_s_dunya + Vektor_t.CaprazCarpim(acisalHiz_Dunya, r_temas_dunya);

                    // C) Z Ekseni Tepki Kuvvetini Hesaplanması (F_z = k*x - c*v)
                    double fZ_tepki = (k_zemin * batma) - (c_zemin * v_nokta_dunya.Z);
                    if (fZ_tepki < 0) fZ_tepki = 0; // Zemin noktayı aşağı çekemez, sadece yukarı itebilir

                    // Elde edilen 3 eksenli tepki kuvvetini diziye ekle (Sürtünme kaldırıldı, X ve Y sıfır)
                    f_zemin_noktalar[i] = new Vektor_t(0, 0, fZ_tepki, Kordinat_Sistemi_t.DUNYA_ENU);
                }

                else
                {
                    // Nokta havadaysa herhangi bir tepki kuvveti yoktur
                    f_zemin_noktalar[i] = new Vektor_t(0, 0, 0, Kordinat_Sistemi_t.DUNYA_ENU);
                }

            }

            return f_zemin_noktalar;

        }









        public static Vektor_t Aerodinamik_Tork_Hesapla(Vektor_t F_aero_body, Vektor_t Aerodinamik_Merkez_Kaymasi_m)
        {
            // Tork = r x F (Kayma vektörü ile Kuvvetin çapraz çarpımı)
            // İkisi de GOVDE_BODY ekseninde olduğu için sonuç da GOVDE_BODY eksenindedir.
            return Vektor_t.CaprazCarpim(Aerodinamik_Merkez_Kaymasi_m, F_aero_body);
        }

        





        public static Vektor_t Aerodinamik_Sonumleme_Torku_Hesapla(Vektor_t w_body)
        {
            // T_damp = -k * w * |w| (Hızın karesiyle orantılı, ancak yönü dönüş hızının tersine)
            double k = FM_Fizik_Sabitler.ROTASYONEL_SONUMLEME_KATSAYISI;
            double tx = -k * w_body.X * Math.Abs(w_body.X);
            double ty = -k * w_body.Y * Math.Abs(w_body.Y);
            double tz = -k * w_body.Z * Math.Abs(w_body.Z);

            return new Vektor_t(tx, ty, tz, Kordinat_Sistemi_t.GOVDE_BODY);
        }





        public static Vektor_t Motor_Torklarini_Hesapla(Vektor_t[] f_motorlar_body)
        {
            // 1. Tekil kuvvetleri vektöre dök (+Z yönünde)
            Vektor_t F1 = f_motorlar_body[0];
            Vektor_t F2 = f_motorlar_body[1];
            Vektor_t F3 = f_motorlar_body[2];
            Vektor_t F4 = f_motorlar_body[3];

            // 3. Roll & Pitch Torku: sum(r x F)
            // Doğrusal kaldırma kuvvetinin (F_z) kütle merkezine uzaklıkla (r) çarpılması sonucu oluşan eğilme torku.
            Vektor_t T_egilme = Vektor_t.CaprazCarpim(FM_Fizik_Sabitler.R_M1_body, F1) + Vektor_t.CaprazCarpim(FM_Fizik_Sabitler.R_M2_body, F2) +
                                Vektor_t.CaprazCarpim(FM_Fizik_Sabitler.R_M3_body, F3) + Vektor_t.CaprazCarpim(FM_Fizik_Sabitler.R_M4_body, F4);

            // 4. Yaw Torku (Pervane Geri Tepmesi - Saf Burulma Momenti)
            // Pervane sürtünmesi doğrusal bir kuvvet değil, doğrudan motor milinde bir burulma (Saf Moment / Couple) yaratır.
            // Saf momentler kütle merkezine olan uzaklıktan (Kol Uzunluğu L) bağımsız olarak aynen gövdeye iletilir.
            // Bu yüzden formülde uzaklık (L) çarpımı yoktur, sadece kuvvetten oransal tork çıkartılır.
            // CCW dönen motorlar (M1, M3) gövdeyi Saat Yönünde (CW / -Z) itmeye çalışır.
            // CW dönen motorlar (M2, M4) gövdeyi Saat Yönünün Tersinde (CCW / +Z) itmeye çalışır.
            //
            // NEDEN BÖLME YAPILIYOR?
            // İtki (F) = k_f * RPM^2  |  Yaw Torku (T) = k_yaw * RPM^2
            // İki denklemi birbirine oranlarsak: T / F = k_yaw / k_f  =>  T = (k_yaw / k_f) * F
            // Bu bölme işlemi sayesinde ağır RPM^2 işlemlerini tekrar tekrar yapmaktan kurtulup 
            // doğrudan elimizdeki F kuvveti üzerinden torku oranlayarak buluyoruz.
            double yaw_carpan = FM_Fizik_Sabitler.PERVANE_YAW_TORK_KATSAYISI / FM_Fizik_Sabitler.MOTOR_ITKI_KATSAYISI;
            
            // F1.Z, F2.Z gibi sadece skaler Z itki büyüklüklerini işleme alıyoruz (Derleme hatası çözümü)
            double yaw_net = (F2.Z + F4.Z - F1.Z - F3.Z) * yaw_carpan;

            return T_egilme + new Vektor_t(0, 0, yaw_net, Kordinat_Sistemi_t.GOVDE_BODY);
        }










        public static Vektor_t Zemin_Torklarini_Hesapla(Uydu_Dinamik_Durum_t mevcutDurum, Vektor_t[] f_zemin_noktalar_dunya)
        {
            // Gövde ekseninde net torku tutacağımız değişken
            Vektor_t net_zemin_torku_body = new Vektor_t(0, 0, 0, Kordinat_Sistemi_t.GOVDE_BODY);

            for (int i = 0; i < FM_Fizik_Sabitler.Temas_Noktalari.Length; i++)
            {

                Vektor_t kuvvet_dunya = f_zemin_noktalar_dunya[i];

                // Eğer o noktada bir kuvvet oluşmamışsa (nokta havadaysa) tork işlemi yaparak işlemciyi yormaya gerek yok
                if (kuvvet_dunya.X == 0 && kuvvet_dunya.Y == 0 && kuvvet_dunya.Z == 0)
                { continue; }

                // 1. Temas noktasının lokal (Gövde) konumu (Yani moment kolumuz olan 'r' vektörü)
                Vektor_t r_temas_lokal = FM_Fizik_Sabitler.Temas_Noktalari[i];

                // 2. Dünya eksenindeki kuvveti Gövde (Body) eksenine çevir
                // Neden? Çünkü çapraz çarpım (r x F) yapabilmemiz için 'r' ve 'F' vektörlerinin mutlaka aynı referans ekseninde (BODY) olması gerekir.
                Vektor_t kuvvet_body = mevcutDurum.Yonelim.DunyadanGovdeyeCevir(kuvvet_dunya);

                // 3. Tork (Moment) Hesabı: T = r x F
                Vektor_t tekil_tork_body = Vektor_t.CaprazCarpim(r_temas_lokal, kuvvet_body);

                // 4. Bulunan tekil torku toplam net torka ekle
                net_zemin_torku_body = net_zemin_torku_body + tekil_tork_body;

            }

            return net_zemin_torku_body;

        }









        // ========================================================================
        // İNTEGRASYON YARDIMCI FONKSİYONLARI (CEBİRSEL / YÖNTEMDEN BAĞIMSIZ)
        // ========================================================================

        public static Vektor_t Dogrusal_Ivme_Hesapla(Vektor_t F_net_dunya, double kutle)
        {
            return F_net_dunya / kutle;
        }



        public static Vektor_t Dogrusal_Hiz_Hesapla(Vektor_t v_eski, Vektor_t ivme, double deltaT_s)
        {
            return v_eski + (ivme * deltaT_s);
        }




        public static Vektor_t Dogrusal_Konum_Hesapla(Vektor_t x_eski, Vektor_t v_yeni, double deltaT_s)
        {
            return x_eski + (v_yeni * deltaT_s);
        }




        
        /// TAM ATALET TENSÖRÜ (3x3 MATRİS) İLE AÇISAL İVME HESABI
        /// -------------------------------------------------------------------------
        /// Genel formül: T_net = I * alpha + (w x I * w)
        /// Amacımız bu formülden Açısal İvmeyi (alpha) çekmektir.
        ///
        /// 1. I matrisi, uydunun asimetrik kütle dağılımını (Ixx, Iyy, Izz, Ixy, Ixz, Iyz) içerir.
        /// 2. (w x I * w) kısmı Jiroskopik Torktur (T_gyro).
        /// 3. Denklem şu hale gelir: I * alpha = T_net - T_gyro
        /// 4. alpha'yı yalnız bırakmak için I matrisinin tersini (I^-1) almamız gerekir.
        /// 
        /// Performansı düşürmemek (C#'ta Matris nesnesi yaratmamak) adına matrisin tersi 
        /// Cramer Kuralı kullanılarak tamamen cebirsel yöntemle, tek satırda hesaplanmıştır.
        public static Vektor_t Acisal_Ivme_Hesapla(Vektor_t T_net_body, Vektor_t w_body, Uydu_Mekanik_Parametreler_t sabitler)
        {
            // 1. Tensör Elemanları (J) - Matrisin Asal ve Çapraz Köşegenleri
            double J11 = sabitler.Ixx;
            double J22 = sabitler.Iyy;
            double J33 = sabitler.Izz;
            double J12 = sabitler.Ixy; // Simetrik J12 = J21
            double J13 = sabitler.Ixz; // Simetrik J13 = J31
            double J23 = sabitler.Iyz; // Simetrik J23 = J32

            double wx = w_body.X;
            double wy = w_body.Y;
            double wz = w_body.Z;

            // 2. Açısal Momentum (L = J * w)
            double Lx = J11 * wx + J12 * wy + J13 * wz;
            double Ly = J12 * wx + J22 * wy + J23 * wz;
            double Lz = J13 * wx + J23 * wy + J33 * wz;

            // 3. Jiroskopik Tork (T_gyro = w x L)
            double gyro_x = wy * Lz - wz * Ly;
            double gyro_y = wz * Lx - wx * Lz;
            double gyro_z = wx * Ly - wy * Lx;

            // Etkin Tork (M = T_net - T_gyro)
            double Mx = T_net_body.X - gyro_x;
            double My = T_net_body.Y - gyro_y;
            double Mz = T_net_body.Z - gyro_z;

            // 4. Matris Tersi (Cramer Kuralı J^-1)
            // C#'ta dizi ve matris sınıfları açıp RAM şişirmemek için cebirsel olarak çözüldü.
            double det = J11 * (J22 * J33 - J23 * J23) 
                       - J12 * (J12 * J33 - J23 * J13) 
                       + J13 * (J12 * J23 - J22 * J13);

            // Simülatör ilk açıldığında (Henüz kütle değerleri girilmemişse) NaN hatasını önlemek için ufak koruma
            if (Math.Abs(det) < 1e-12) det = 1e-12;

            // Ters matris (J^-1) elemanları (Sadece gerekli olanları hesaplıyoruz, simetriyi kullanıyoruz)
            double inv11 = (J22 * J33 - J23 * J23) / det;
            double inv12 = (J13 * J23 - J12 * J33) / det;
            double inv13 = (J12 * J23 - J13 * J22) / det;
            
            double inv22 = (J11 * J33 - J13 * J13) / det;
            double inv23 = (J12 * J13 - J11 * J23) / det;
            
            double inv33 = (J11 * J22 - J12 * J12) / det;

            // 5. Açısal İvme (alpha = J^-1 * M)
            double alpha_x = inv11 * Mx + inv12 * My + inv13 * Mz;
            double alpha_y = inv12 * Mx + inv22 * My + inv23 * Mz; // Simetriden dolayı inv21 = inv12
            double alpha_z = inv13 * Mx + inv23 * My + inv33 * Mz; // Simetriden dolayı inv31 = inv13, inv32 = inv23

            return new Vektor_t(alpha_x, alpha_y, alpha_z, Kordinat_Sistemi_t.GOVDE_BODY);
        }




        public static Vektor_t Acisal_Hiz_Hesapla(Vektor_t w_eski, Vektor_t alpha, double deltaT_s)
        {
            return w_eski + (alpha * deltaT_s);
        }

        public static Kuaterniyon_t Yonelim_Hesapla(Kuaterniyon_t q_eski, Vektor_t w_yeni, double deltaT_s)
        {
            // Kuaterniyon türevi (q_dot) = 0.5 * q * w
            double qw = q_eski.W, qx = q_eski.X, qy = q_eski.Y, qz = q_eski.Z;
            double wx = w_yeni.X, wy = w_yeni.Y, wz = w_yeni.Z;

            double qDot_w = 0.5 * (-qx * wx - qy * wy - qz * wz);
            double qDot_x = 0.5 * ( qw * wx + qy * wz - qz * wy);
            double qDot_y = 0.5 * ( qw * wy - qx * wz + qz * wx);
            double qDot_z = 0.5 * ( qw * wz + qx * wy - qy * wx);

            // İleri Euler İntegrasyonu: q_yeni = q_eski + q_dot * dt
            Kuaterniyon_t q_yeni = new Kuaterniyon_t(
                qw + qDot_w * deltaT_s,
                qx + qDot_x * deltaT_s,
                qy + qDot_y * deltaT_s,
                qz + qDot_z * deltaT_s
            );

            // Matematiksel bozulmayı (drift) önlemek için her adımda mutlaka Normalize edilmeli
            q_yeni.Normalize();
            return q_yeni;
        }









        // ========================================================================
        // 9) LCP TABANLI NOKTASAL SÜRTÜNME ÇÖZÜCÜ (Sequential Impulse / Gauss-Seidel)
        // ========================================================================
        public static void Noktasal_Surtunme_Cozucu(
            ref Vektor_t F_net_dunya, 
            ref Vektor_t T_net_body, 
            Uydu_Dinamik_Durum_t mevcutDurum, 
            Uydu_Mekanik_Parametreler_t sabitler, 
            Vektor_t[] f_zemin_noktalar, 
            double deltaT_s)
        {
            double mu = FM_Fizik_Sabitler.ZEMIN_SURTUNME_KATSAYISI;
            double m = sabitler.Kutle_kg;

            // Ters atalet katsayısı (İzotropik yaklaşım: Uzayda her yöne benzer atalet varsayımı)
            // Bu, LCP çözücünün stabil çalışması için yeterlidir.
            double I_ortalama = (sabitler.Ixx + sabitler.Iyy + sabitler.Izz) / 3.0;
            double I_inv = 1.0 / I_ortalama;

            // 1. İLERİ BAKIŞ (PREDICT): Newton-Euler integrasyonunun öncü tahmini
            Vektor_t v_cm_next = mevcutDurum.Hiz_m_s_dunya + (F_net_dunya / m) * deltaT_s;
            Vektor_t w_next_body = mevcutDurum.Acisal_Hiz_rad_s_body + Acisal_Ivme_Hesapla(T_net_body, mevcutDurum.Acisal_Hiz_rad_s_body, sabitler) * deltaT_s;
            Vektor_t w_next_dunya = mevcutDurum.Yonelim.GovdedenDunyayaCevir(w_next_body);

            int noktaSayisi = FM_Fizik_Sabitler.Temas_Noktalari.Length;
            
            // Sürtünme Kuvveti Havuzu ve Dünya Eksenindeki Noktalar (Optimizasyon)
            Vektor_t[] F_surtunme_biriken = new Vektor_t[noktaSayisi];
            Vektor_t[] r_dunya_noktalar = new Vektor_t[noktaSayisi];
            for (int i = 0; i < noktaSayisi; i++)
            {
                F_surtunme_biriken[i] = new Vektor_t(0, 0, 0, Kordinat_Sistemi_t.DUNYA_ENU);
                r_dunya_noktalar[i] = mevcutDurum.Yonelim.GovdedenDunyayaCevir(FM_Fizik_Sabitler.Temas_Noktalari[i]);
            }

            // 2. KISIT DÖNGÜSÜ (Gauss-Seidel Iteration)
            int num_iterations = 10;
            for (int iter = 0; iter < num_iterations; iter++)
            {
                for (int i = 0; i < noktaSayisi; i++)
                {
                    double N = f_zemin_noktalar[i].Z;
                    if (N <= 0.001) continue; // Yere temas etmeyen nokta için sürtünme çözülmez

                    double max_fric = mu * N;
                    Vektor_t r = r_dunya_noktalar[i];

                    // Noktanın anlık tahmini hızı: V_p = V_cm + W x R
                    Vektor_t v_nokta = v_cm_next + Vektor_t.CaprazCarpim(w_next_dunya, r);
                    Vektor_t v_yatay = new Vektor_t(v_nokta.X, v_nokta.Y, 0, Kordinat_Sistemi_t.DUNYA_ENU);

                    // --- Efektif Kütle Matrisi (K) ---
                    // K = (1/m)*I - r_cross * I_inv * r_cross (Sadece 2x2 XY bloğu hesaplanır)
                    double Kxx = (1.0 / m) + I_inv * (r.Y * r.Y + r.Z * r.Z);
                    double Kyy = (1.0 / m) + I_inv * (r.X * r.X + r.Z * r.Z);
                    double Kxy = I_inv * (-r.X * r.Y);

                    // K_inv (Matris Tersi)
                    double detK = Kxx * Kyy - Kxy * Kxy;
                    if (detK < 1e-8) detK = 1e-8; // Singularite koruması

                    double invKxx = Kyy / detK;
                    double invKyy = Kxx / detK;
                    double invKxy = -Kxy / detK;

                    // Bu noktanın yatay hızını tamamen sıfırlamak için gereken kuvvet değişimi: F = - K_inv * (V / dt)
                    double v_hedef_x = v_yatay.X / deltaT_s;
                    double v_hedef_y = v_yatay.Y / deltaT_s;

                    double dFx = -(invKxx * v_hedef_x + invKxy * v_hedef_y);
                    double dFy = -(invKxy * v_hedef_x + invKyy * v_hedef_y);

                    // Yeni toplam sürtünme kuvveti (Geçici)
                    Vektor_t F_eski = F_surtunme_biriken[i];
                    Vektor_t F_yeni = new Vektor_t(F_eski.X + dFx, F_eski.Y + dFy, 0, Kordinat_Sistemi_t.DUNYA_ENU);

                    // Friction Cone (Sürtünme Konisi) Kırpması
                    double f_yeni_kare = F_yeni.X * F_yeni.X + F_yeni.Y * F_yeni.Y;
                    if (f_yeni_kare > max_fric * max_fric)
                    {
                        double oran = max_fric / Math.Sqrt(f_yeni_kare);
                        F_yeni.X *= oran;
                        F_yeni.Y *= oran;
                    }

                    // Gerçekte bu iterasyonda uygulanabilen fark
                    Vektor_t dF_uygulanan = new Vektor_t(F_yeni.X - F_eski.X, F_yeni.Y - F_eski.Y, 0, Kordinat_Sistemi_t.DUNYA_ENU);
                    F_surtunme_biriken[i] = F_yeni; // Kuvveti havuza kaydet

                    // Tahmini hızları bu fark kadar esnet (Bir sonraki iterasyon ayak uydursun)
                    v_cm_next.X += (dF_uygulanan.X / m) * deltaT_s;
                    v_cm_next.Y += (dF_uygulanan.Y / m) * deltaT_s;

                    Vektor_t tork_dF_dunya = Vektor_t.CaprazCarpim(r, dF_uygulanan);
                    w_next_dunya.X += (tork_dF_dunya.X * I_inv) * deltaT_s;
                    w_next_dunya.Y += (tork_dF_dunya.Y * I_inv) * deltaT_s;
                    w_next_dunya.Z += (tork_dF_dunya.Z * I_inv) * deltaT_s;
                }
            }

            // 3. UYGULAMA (APPLY): İterasyonlar bitti, hiperstatik dengeler kuruldu. Toplamları Net kuvvetlere göm.
            for (int i = 0; i < noktaSayisi; i++)
            {
                if (F_surtunme_biriken[i].X == 0 && F_surtunme_biriken[i].Y == 0) continue;

                F_net_dunya = F_net_dunya + F_surtunme_biriken[i];

                Vektor_t tork_fric_dunya = Vektor_t.CaprazCarpim(r_dunya_noktalar[i], F_surtunme_biriken[i]);
                Vektor_t tork_fric_body = mevcutDurum.Yonelim.DunyadanGovdeyeCevir(tork_fric_dunya);
                T_net_body = T_net_body + tork_fric_body;
            }
        }
    }
}
