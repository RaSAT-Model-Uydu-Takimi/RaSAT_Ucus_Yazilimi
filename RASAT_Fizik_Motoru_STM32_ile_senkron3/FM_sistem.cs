using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Runtime.Intrinsics.X86;
using System.Text;
using System.Threading.Tasks;
using static System.Windows.Forms.VisualStyles.VisualStyleElement;
namespace RASAT_Fizik_Motoru_STM32_ile_senkron3
{
    internal class FM_sistem
    {

        // --- SİSTEM DEĞİŞKENLERİ ---
        public uint aktifIndex;
        public Alim_Paketi_t sonUretilenPaket;


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


        // Kurucu Metot (Başlangıç atamaları)
        public FM_sistem()
        {
            aktifIndex = 0;
            sonUretilenPaket = new Alim_Paketi_t();
            sonUretilenPaket.header = PaketSabitleri.PAKET_HEADER;
            sonUretilenPaket.rezerve = 0x00;
        }


        // --- AKIŞ ŞEMASI: PAKET SIRASINI ARTIR ---
        public void Index_Artir()
        {
            aktifIndex++;
        }



        // --- AKIŞ ŞEMASI: GERÇEK DEĞERLERİ HESAPLA & SANAL SENSÖR VERİLERİNİ ÜRET ---
        public void Fizigi_Calistir(ref Iletim_Paketi_t gelen_paket, ref Alim_Paketi_t giden_paket)
        {

        }


        public void Davranisi_Uygula()
        {

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
