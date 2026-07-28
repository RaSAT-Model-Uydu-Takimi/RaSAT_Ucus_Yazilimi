using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Runtime.Intrinsics.X86;
using System.Text;
using System.Threading.Tasks;
using static System.Windows.Forms.VisualStyles.VisualStyleElement;
namespace RASAT_Fizik_Motoru_STM32_ile_senkron3._1
{
    internal class FM_sistem
    {

        public const double TIMESTEP_S = 0.0020;




        // --- SİSTEM DEĞİŞKENLERİ ---
        public uint aktifIndex;
        public Alim_Paketi_t sonUretilenPaket;

        public FM_Fizik_Motoru fizikMotoru;
        public Uydu_Dinamik_Durum_t uyduDurumu;
        public Uydu_Mekanik_Parametreler_t uyduSabitleri;
        public Simulasyon_Cevre_Sartlari_t cevreSartlari;
        public Uydu_Kontrol_Girdisi_t aktifGirdi;


        // Sabit alarm paketi: İndex ve diğer her şey sabit.
        // Sadece rezerve 0xF0.
        public static readonly Alim_Paketi_t ALARM_PAKETI = new Alim_Paketi_t
        {
            header = PaketSabitleri.PAKET_HEADER,
            index = 0,
            acc_x = 0, acc_y = 0, acc_z = 0,
            gyro_x = 0, gyro_y = 0, gyro_z = 0,
            mag_x = 0, mag_y = 0, mag_z = 0,
            basinc = 0, sicaklik = 0,
            gps_lat = 0, gps_lon = 0, gps_alt = 0, gps_vel = 0,
            bat_v = 0, bat_a = 0,
            rezerve = 0xF0,
            checksum = 0x0F // Bu Paketin Checksum'ı direkt hesaplandı ve 0x0F bulundu
        };




        // Sabit alarm paketi: İndex ve diğer her şey sabit.
        // Sadece rezerve 0xF0.
        public static readonly Alim_Paketi_t BAS_PAKETI = new Alim_Paketi_t
        {
            header = PaketSabitleri.PAKET_HEADER,
            index = 1,
            acc_x = 0, acc_y = 0, acc_z = 0,
            gyro_x = 0, gyro_y = 0, gyro_z = 0,
            mag_x = 0, mag_y = 0, mag_z = 0,
            basinc = 0, sicaklik = 0,
            gps_lat = 0, gps_lon = 0, gps_alt = 0, gps_vel = 0,
            bat_v = 0, bat_a = 0,
            rezerve = 0,
            checksum = 0xFE // Bu Paketin Checksum'ı direkt hesaplandı ve yerine yazıldı.
        };


        public FM_sistem()
        {
            aktifIndex = 0;
            sonUretilenPaket = new Alim_Paketi_t();
            sonUretilenPaket.header = PaketSabitleri.PAKET_HEADER;
            sonUretilenPaket.rezerve = 0x00;

            fizikMotoru = new FM_Fizik_Motoru();
            uyduDurumu = new Uydu_Dinamik_Durum_t();
            uyduDurumu.Yonelim = new Kuaterniyon_t(1, 0, 0, 0); // Gimbal Lock önleyici başlangıç durumu
            
            uyduSabitleri = FM_Fizik_Sabitler.Tasiyici_Faz; // Varsayılan olarak Taşıyıcı fazında başlat
            cevreSartlari = new Simulasyon_Cevre_Sartlari_t();
            
            aktifGirdi = new Uydu_Kontrol_Girdisi_t();
            aktifGirdi.Protokol = Motor_Protokol_t.PWM_1000_2000; // Varsayılan protokol
            
            // Sensör simülatörü hata profillerini varsayılan (düşük gürültülü) değerlerle başlat
            FM_Sensor_Simulatoru.FabrikaAyarlarinaDon();
        }


        // --- AKIŞ ŞEMASI: PAKET SIRASINI ARTIR ---
        public void Index_Artir()
        {
            aktifIndex++;
        }



        // --- AKIŞ ŞEMASI: GERÇEK DEĞERLERİ HESAPLA & SANAL SENSÖR VERİLERİNİ ÜRET ---
        public void Fizigi_Calistir(ref Iletim_Paketi_t gelen_paket, ref Alim_Paketi_t giden_paket)
        {
            // ADIM 1: Gelen UDP paketini donanım davranışına / kontrol girdisine dönüştür
            Davranisi_Uygula(ref gelen_paket);

            // ADIM 2: Fizik Motorunu 1 Adım İleri Sar (Saf Euler İntegrasyonu)
            uyduDurumu = fizikMotoru.Fizik_Adimi_Hesapla(uyduDurumu, uyduSabitleri, aktifGirdi, cevreSartlari, TIMESTEP_S);

            // ADIM 3: Sensör Simülasyonu 
            FM_Sensor_Simulatoru.Paket_Icin_Veri_Uret(uyduDurumu, ref giden_paket);
        }


        public void Davranisi_Uygula(ref Iletim_Paketi_t gelen_paket)
        {
            // Gelen baytları fizik motorunun anlayacağı kontrol girdisine (Uydu_Kontrol_Girdisi_t) dönüştür
            aktifGirdi.mi1_raw = gelen_paket.mi1;
            aktifGirdi.mi2_raw = gelen_paket.mi2;
            aktifGirdi.mi3_raw = gelen_paket.mi3;
            aktifGirdi.mi4_raw = gelen_paket.mi4;

            // Durum bayraklarını ayrıştır (Bit Maskeleme: LSB'den MSB'ye)
            aktifGirdi.SEP  = (gelen_paket.durum_bayraklari & 0x01) != 0; // Bit 0
            aktifGirdi.SGM  = (gelen_paket.durum_bayraklari & 0x02) != 0; // Bit 1
            aktifGirdi.APAM = (gelen_paket.durum_bayraklari & 0x04) != 0; // Bit 2
            aktifGirdi.BUZZ = (gelen_paket.durum_bayraklari & 0x08) != 0; // Bit 3

            // Durum bayraklarına göre uydunun o anki mekanik fazını (Kütle, Kesit, Atalet) seç
            if (aktifGirdi.APAM)
            {
                uyduSabitleri = FM_Fizik_Sabitler.APAM_Acik_Faz;
            }
            else if (aktifGirdi.SGM)
            {
                uyduSabitleri = FM_Fizik_Sabitler.GorevYuku_Acik_Faz;
            }
            else if (aktifGirdi.SEP)
            {
                uyduSabitleri = FM_Fizik_Sabitler.GorevYuku_Kapali_Faz;
            }
            else
            {
                uyduSabitleri = FM_Fizik_Sabitler.Tasiyici_Faz;
            }
        }


        public void Alim_Paketini_Olustur(ref Alim_Paketi_t giden_paket)
        {
            // 1. ÖNCE değerleri güncelleyin
            giden_paket.index = aktifIndex;
            giden_paket.header = PaketSabitleri.PAKET_HEADER;

            // 2. SONRA bu güncel değerleri byte dizisine dönüştürün
            int paketBoyutu = Marshal.SizeOf<Alim_Paketi_t>();
            byte[] paketBytes = StructToBytes(giden_paket);

            // 3. Checksum'ı doğru byte dizisinden hesaplayın
            giden_paket.checksum = Checksum_Hesapla(paketBytes, paketBoyutu - 1);

            sonUretilenPaket = giden_paket;
        }

        public void En_Son_Paketi_Bir_Daha_Olustur(ref Alim_Paketi_t giden_paket)
        {
            giden_paket = sonUretilenPaket;
        }







        // ========================================================================
        // 3. YARDIMCI METODLAR (MARSHALING / AMELELİK KISMI)
        // ========================================================================
        // Sadece bu kütüphane içinde kullanılacak gizli XOR Checksum fonksiyonu
        private byte Checksum_Hesapla(byte[] veri, int uzunluk)
        {
            byte chk = 0;
            for (int i = 0; i < uzunluk; i++)
            {
                chk ^= veri[i];
            }
            return chk;
        }




        private byte[] StructToBytes<T>(T str) where T : struct
        {
            int size = Marshal.SizeOf(str);
            byte[] arr = new byte[size];
            IntPtr ptr = Marshal.AllocHGlobal(size);

            try
            {
                Marshal.StructureToPtr(str, ptr, true);
                Marshal.Copy(ptr, arr, 0, size);
            }
            finally
            {
                Marshal.FreeHGlobal(ptr);
            }
            return arr;
        }





        private T BytesToStruct<T>(byte[] arr) where T : struct
        {
            T str = default(T);
            int size = Marshal.SizeOf(str);
            IntPtr ptr = Marshal.AllocHGlobal(size);

            try
            {
                Marshal.Copy(arr, 0, ptr, size);
                str = (T)Marshal.PtrToStructure(ptr, typeof(T));
            }
            finally
            {
                Marshal.FreeHGlobal(ptr);
            }
            return str;
        }

    }
}
