using System;

namespace RASAT_Fizik_Motoru_STM32_ile_senkron3._1
{
    // ========================================================================
    // FİZİK MOTORU ANA SINIFI (Newton-Euler İntegrasyonu)
    // ========================================================================
    /// <summary>
    /// Uydunun hareket denklemlerini (F=m*a ve T=I*alpha) çözen ana sınıftır.
    /// Sadece matematik ve fizik işlemleri barındırır, state tutmaz (Stateless).
    /// </summary>
    public class FM_Fizik_Motoru
    {
        public FM_Fizik_Motoru()
        {
            // Motor ilklendirme işlemleri
        }




        public Uydu_Dinamik_Durum_t Fizik_Adimi_Hesapla(Uydu_Dinamik_Durum_t mevcutDurum, Uydu_Mekanik_Parametreler_t sabitler, Uydu_Kontrol_Girdisi_t girdi, Simulasyon_Cevre_Sartlari_t cevreSartlari, double deltaT_s)
        {
            Uydu_Dinamik_Durum_t yeniDurum = mevcutDurum;

            //////////////////////////////////// 1. KUVVETLERİN HESAPLANMASI /////////////////////////////////////
            Vektor_t F_yercekimi_dunya = FM_Fizik_Fonksiyonlari.Yercekimi_Kuvveti_Hesapla(sabitler.Kutle_kg);            
            
            // BATARYA VOLTAJI VE AKIM HESAPLAMALARI (CAPACITY DRAIN + VOLTAGE SAG)
            FM_Fizik_Fonksiyonlari.Batarya_Durumu_Guncelle(ref yeniDurum, girdi, deltaT_s);            
            double v_pil = yeniDurum.Batarya_Voltaj_V;

            Vektor_t[] F_motorlar_body = FM_Fizik_Fonksiyonlari.Motor_Tekil_Itkilerini_Hesapla(girdi, v_pil, cevreSartlari);
            Vektor_t F_motorlar_dunya = mevcutDurum.Yonelim.GovdedenDunyayaCevir(F_motorlar_body[0] + F_motorlar_body[1] + F_motorlar_body[2] + F_motorlar_body[3]);

            Vektor_t F_aero_body = FM_Fizik_Fonksiyonlari.Aerodinamik_Kuvvet_Hesapla(mevcutDurum, sabitler, cevreSartlari.Ruzgar_Hizi_m_s_dunya);
            Vektor_t F_aero_dunya = mevcutDurum.Yonelim.GovdedenDunyayaCevir(F_aero_body);



            ///////////////////////////// 2. NET KUVVETİN TOPLANMASI (F_net_dunya) ///////////////////////////////
            // F_net = Yerçekimi + Motor İtkisi + Sürüklenme + Arayüzden Atılan Harici Tokat(Darbe)
            Vektor_t F_net_dunya = F_yercekimi_dunya + F_motorlar_dunya + F_aero_dunya + cevreSartlari.Harici_Kuvvet_N_dunya;



            //////////////////////////////////// 3. TORKLARIN HESAPLANMASI //////////////////////////////////////
            Vektor_t T_Motorlar_body = FM_Fizik_Fonksiyonlari.Motor_Torklarini_Hesapla(F_motorlar_body);
            Vektor_t T_Aero_body = FM_Fizik_Fonksiyonlari.Aerodinamik_Tork_Hesapla(F_aero_body, sabitler.Aerodinamik_Merkez_Kaymasi_m_body);
            Vektor_t T_Damp_body = FM_Fizik_Fonksiyonlari.Aerodinamik_Sonumleme_Torku_Hesapla(mevcutDurum.Acisal_Hiz_rad_s_body);
            


            ////////////////////////////// 4. NET TORKUN TOPLANMASI (T_net_body) ////////////////////////////////
            // T_net = Motorların Eğmesi + Rüzgarın Devirmesi + Rüzgarın Sönümlemesi (Fren) + Arayüzden Atılan Harici Tork (Bozucu)
            Vektor_t T_net_body = T_Motorlar_body + T_Aero_body + T_Damp_body + cevreSartlari.Harici_Tork_Nm_body;



            //////////////////////// 5. DİNAMİK DURUM GÜNCELLEMESİ (İNTEGRASYON ADIMLARI) ////////////////////////
            
            // --- DOĞRUSAL HAREKET (Öteleme) ---
            yeniDurum.Ivme_m_s2_dunya = FM_Fizik_Fonksiyonlari.Dogrusal_Ivme_Hesapla(F_net_dunya, sabitler.Kutle_kg);
            yeniDurum.Hiz_m_s_dunya = FM_Fizik_Fonksiyonlari.Dogrusal_Hiz_Hesapla(mevcutDurum.Hiz_m_s_dunya, yeniDurum.Ivme_m_s2_dunya, deltaT_s);
            yeniDurum.Konum_m_dunya = FM_Fizik_Fonksiyonlari.Dogrusal_Konum_Hesapla(mevcutDurum.Konum_m_dunya, yeniDurum.Hiz_m_s_dunya, deltaT_s);

            // --- DÖNME HAREKETİ (Rotasyon) ---
            yeniDurum.Acisal_Ivme_rad_s2_body = FM_Fizik_Fonksiyonlari.Acisal_Ivme_Hesapla(T_net_body, mevcutDurum.Acisal_Hiz_rad_s_body, sabitler);
            yeniDurum.Acisal_Hiz_rad_s_body = FM_Fizik_Fonksiyonlari.Acisal_Hiz_Hesapla(mevcutDurum.Acisal_Hiz_rad_s_body, yeniDurum.Acisal_Ivme_rad_s2_body, deltaT_s);
            yeniDurum.Yonelim = FM_Fizik_Fonksiyonlari.Yonelim_Hesapla(mevcutDurum.Yonelim, yeniDurum.Acisal_Hiz_rad_s_body, deltaT_s);

            return yeniDurum;
        }
    }
}
