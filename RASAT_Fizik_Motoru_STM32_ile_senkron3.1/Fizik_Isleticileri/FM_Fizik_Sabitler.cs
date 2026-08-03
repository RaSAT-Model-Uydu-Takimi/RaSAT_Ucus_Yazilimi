using System;

namespace RASAT_Fizik_Motoru_STM32_ile_senkron3._1
{
    // ========================================================================
    // MEKANİK SABİTLER KÜTÜPHANESİ (C++'daki #define Mantığı)
    // ========================================================================
    /// <summary>
    /// Uydunun uçuş fazlarına göre değişen veya sabit kalan tüm fiziksel değerleri
    /// doğrudan RAM'e (Compile-time) gömülmüştür.
    /// </summary>
    public static class FM_Fizik_Sabitler
    {
        // --------------------------------------------------------------------
        // FAZ 0: DÜNYA SABİTLERİ VE DEĞİŞMEZ UYDU SABİTLERİ
        // --------------------------------------------------------------------
        public const double GRAVITY_M_S2 = 9.80084;

        public const double HAVA_YOGUNLUGU_RHO = 1.1;       // Aksaray rakımı (~900m) için ortalama hava yoğunluğu (kg/m^3)
        public const double SURTUNME_KATSAYISI_CD = 1.5;    // Çapraz (Cruciform) paraşüt ve gövde sürüklenme katsayısı
        public const double ROTASYONEL_SONUMLEME_KATSAYISI = 0.01; // Uydunun kendi etrafında fırıldak gibi dönmesini engelleyen sürtünme katsayısı

        public static Vektor_t MAG_FIELD_dunya = new Vektor_t(24.0, 2.0, 40.0, Kordinat_Sistemi_t.DUNYA_ENU); // Aksaraydaki yaklaşık manyetik alan vektörü uT cinsinden

        public const double MOTOR_KV = 1800.0;             // XING-E Pro 2207 (CDR Güncellemesi)
        public const double MOTOR_YUK_VERIMI = 0.788;      // Pervanenin havayı keserken yüke bindiğindeki devir verimi
        public const double MOTOR_ITKI_KATSAYISI = 1.5e-8; // N / RPM^2
        public const double PERVANE_YAW_TORK_KATSAYISI = 1.2e-9; // Nm / RPM^2     //Pervanenin havayı dikeyde keserken oluşturduğu yatay geri tepki momenti sabiti
        public const double MOTOR_MAKS_AKIM_A = 30.0;      // Tam gazda bir motorun çektiği maksimum akım (Amper)
        public const double MOTOR_TEPKI_SURESI_TAU_S = 0.06; // XING-E Pro 2207 (1800KV) 5 inç pervane ile %63 gaz tepki süresi (Tau = 60ms)

        public const double BATARYA_DOLU_VOLTAJ_V = 16.8;  // 4S LiPo/Li-ion Tam Dolu Voltajı
        public const double BATARYA_BOS_VOLTAJ_V = 13.0;   // 4S LiPo Kritik Boş Voltajı
        public const double BATARYA_KAPASITE_MAH = 4000.0; // 4000 mAh        
        public const double ELEKTRONIK_BOS_AKIM_A = 0.5;   // Motorlar dönmezken bile STM32 ve sensörlerin çektiği akım
        public const double BATARYA_IC_DIRENC_OHM = 0.02;  // Akım çekildikçe voltajı düşürecek olan pilin iç direnci (Voltage Sag)                                           
        
        // Quadcopter Geometrisi (3D Konumlandırma)
        public const double MOTOR_KOL_UZUNLUGU_M = 0.15036;       // Kolun gerçek uzunluğu (150.36 mm)
        public const double MOTOR_Z_KAYMASI_M = 0.12880;         // Pervane düzleminin Ağırlık Merkezine (CoM) dikey mesafesi (+Z yukarı)
        public const double d_motor = MOTOR_KOL_UZUNLUGU_M * 0.7071; // 45 derece için sin/cos izdüşümü ( sin(45 derece) = 0.7071 )
        public const double h_motor = MOTOR_Z_KAYMASI_M;
        public static Vektor_t R_M1_body = new Vektor_t(d_motor, d_motor, h_motor, Kordinat_Sistemi_t.GOVDE_BODY);   // Sağ-Ön
        public static Vektor_t R_M2_body = new Vektor_t(-d_motor, d_motor, h_motor, Kordinat_Sistemi_t.GOVDE_BODY);  // Sol-Ön
        public static Vektor_t R_M3_body = new Vektor_t(-d_motor, -d_motor, h_motor, Kordinat_Sistemi_t.GOVDE_BODY); // Sol-Arka
        public static Vektor_t R_M4_body = new Vektor_t(d_motor, -d_motor, h_motor, Kordinat_Sistemi_t.GOVDE_BODY);  // Sağ-Arka

        public const double TABAN_YARICAPI_M = 0.070;           // Görev yükünün tabanındaki dairenin yarıçapı (70 mm)
        public const double TABAN_Z_KAYMASI_M = -0.1262;        // Görev yükünün ağırlık merkezine göre görev yükünün tabanının z değeri
        public const double d_taban = TABAN_YARICAPI_M * 0.7071; // 45 derece için sin/cos izdüşümü ( sin(45 derece) = 0.7071 )
        public const double h_taban = TABAN_Z_KAYMASI_M;
        public static Vektor_t T_1_body = new Vektor_t(d_taban, d_taban, h_taban, Kordinat_Sistemi_t.GOVDE_BODY);   // Sağ-Ön
        public static Vektor_t T_2_body = new Vektor_t(-d_taban, d_taban, h_taban, Kordinat_Sistemi_t.GOVDE_BODY);  // Sol-Ön
        public static Vektor_t T_3_body = new Vektor_t(-d_taban, -d_taban, h_taban, Kordinat_Sistemi_t.GOVDE_BODY); // Sol-Arka
        public static Vektor_t T_4_body = new Vektor_t(d_taban, -d_taban, h_taban, Kordinat_Sistemi_t.GOVDE_BODY);  // Sağ-Arka

        public static Vektor_t[] Temas_Noktalari = new Vektor_t[]{T_1_body,T_2_body,T_3_body,T_4_body,R_M1_body,R_M2_body,R_M3_body,R_M4_body};


        public const double ZEMIN_RAKIMI_M = 980.0; //Aksarayın rakımını girdim buraya.
        public const double ZEMIN_YAY_KATSAYISI = 1500.0;     // K_zemin: Titremeyi engellemek için düşürüldü
        public const double ZEMIN_SONUMLEME_KATSAYISI = 45.0; // C_zemin: Explicit Euler instabilitesini engellemek için ayarlandı
        public const double ZEMIN_SURTUNME_KATSAYISI = 0.50; // Yatay eksende yerde kaymayı önleyici kinetik sürtünme


        // --------------------------------------------------------------------
        // FAZ 1: TAŞIYICI + GÖREV YÜKÜ (SİLİNDİR FORMU)
        // --------------------------------------------------------------------
        public static readonly Uydu_Mekanik_Parametreler_t Tasiyici_Faz = new Uydu_Mekanik_Parametreler_t
        {
            Kutle_kg = 1.800,
            
            Ixx = 0.0228, Iyy = 0.0228, Izz = 0.009,
            Ixy = 0.0, Ixz = 0.0, Iyz = 0.0,
            
            Alan_X_m2 = 0.07, Alan_Y_m2 = 0.07, Alan_Z_m2 = 0.0314,
            
            Aerodinamik_Merkez_Kaymasi_m_body = new Vektor_t(0, 0, 0, Kordinat_Sistemi_t.GOVDE_BODY),
            Aktif_Parasut_Alani_m2 = 0.1256
        };

        // --------------------------------------------------------------------
        // FAZ 2: GÖREV YÜKÜ YALNIZ (SİGMA KAPALI - SİLİNDİR FORMU)
        // --------------------------------------------------------------------
        public static readonly Uydu_Mekanik_Parametreler_t GorevYuku_Kapali_Faz = new Uydu_Mekanik_Parametreler_t
        {
            Kutle_kg = 1.250,
            
            Ixx = 0.0073, Iyy = 0.0073, Izz = 0.0062,
            Ixy = 0.0, Ixz = 0.0, Iyz = 0.0,
            
            Alan_X_m2 = 0.04, Alan_Y_m2 = 0.04, Alan_Z_m2 = 0.0314,
            
            Aerodinamik_Merkez_Kaymasi_m_body = new Vektor_t(0, 0, 0, Kordinat_Sistemi_t.GOVDE_BODY),
            Aktif_Parasut_Alani_m2 = 0.0
        };

        // --------------------------------------------------------------------
        // FAZ 3: GÖREV YÜKÜ YALNIZ (SİGMA AÇIK - T FORMU)
        // --------------------------------------------------------------------
        public static readonly Uydu_Mekanik_Parametreler_t GorevYuku_Acik_Faz = new Uydu_Mekanik_Parametreler_t
        {
            Kutle_kg = 1.250,
            
            Ixx = 0.0073, Iyy = 0.0073, Izz = 0.0062,
            Ixy = 0.0, Ixz = 0.0, Iyz = 0.0,
            
            Alan_X_m2 = 0.04, Alan_Y_m2 = 0.04, Alan_Z_m2 = 0.0314,
            
            Aerodinamik_Merkez_Kaymasi_m_body = new Vektor_t(0, 0, 0, Kordinat_Sistemi_t.GOVDE_BODY),
            Aktif_Parasut_Alani_m2 = 0.0
        };

        // --------------------------------------------------------------------
        // FAZ 4: APAM PARAŞÜTÜ AÇIK
        // --------------------------------------------------------------------
        public static readonly Uydu_Mekanik_Parametreler_t APAM_Acik_Faz = new Uydu_Mekanik_Parametreler_t
        {
            Kutle_kg = 1.250,
            
            Ixx = 0.0073, Iyy = 0.0073, Izz = 0.0062,
            Ixy = 0.0, Ixz = 0.0, Iyz = 0.0,
            
            Alan_X_m2 = 0.04, Alan_Y_m2 = 0.04, Alan_Z_m2 = 0.0314,
            
            Aerodinamik_Merkez_Kaymasi_m_body = new Vektor_t(0, 0, 0.80, Kordinat_Sistemi_t.GOVDE_BODY),
            Aktif_Parasut_Alani_m2 = 0.5026
        };
    }
}
