using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO.Ports;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
namespace RASAT_Fizik_Motoru_STM32_ile_senkron3
{
    // ========================================================================
    // 1. VERİ YAPILARI (STRUCTS)
    // ========================================================================

    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public struct Iletim_Paketi_t
    {
        public ushort header;           // 2 Bayt
        public uint index;              // 4 Bayt
        public ushort mi1;              // 2 Bayt
        public ushort mi2;              // 2 Bayt
        public ushort mi3;              // 2 Bayt
        public ushort mi4;              // 2 Bayt
        public byte durum_bayraklari;   // 1 Bayt
        public byte checksum;           // 1 Bayt
    }

    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public struct Alim_Paketi_t
    {
        public ushort header;         // 2 Bayt
        public uint index;            // 4 Bayt

        public short acc_x;           // 2 Bayt
        public short acc_y;           // 2 Bayt
        public short acc_z;           // 2 Bayt

        public short gyro_x;          // 2 Bayt
        public short gyro_y;          // 2 Bayt
        public short gyro_z;          // 2 Bayt

        public short mag_x;           // 2 Bayt
        public short mag_y;           // 2 Bayt
        public short mag_z;           // 2 Bayt

        public uint basinc;           // 4 Bayt
        public short sicaklik;        // 2 Bayt

        public int gps_lat;           // 4 Bayt
        public int gps_lon;           // 4 Bayt
        public int gps_alt;           // 4 Bayt
        public short gps_vel;         // 2 Bayt

        public ushort bat_v;          // 2 Bayt
        public int bat_a;             // 4 Bayt
        public byte rezerve;          // 1 Bayt
        public byte checksum;         // 1 Bayt
    }

    public enum Paket_Durum_t
    {
        PAKET_HATA = 0,   // Hiçbir şey gelmedi
        PAKET_TAM = 1,    // 16 byte eksiksiz geldi
        PAKET_EKSIK = 2   // Timeout oldu ama bir miktar veri geldi
    }

    // --- PAKET SENKRONİZASYON VE BİTMASK MAKROLARI ---
    public static class PaketSabitleri
    {
        // C'deki 0xAA55U (unsigned) karşılığı C#'ta 'ushort' (uint16) veya 'uint' (uint32) olur.
        // Paket boyutunuz 52 byte olduğuna göre header muhtemelen 2 byte'tır (ushort).
        public const ushort PAKET_HEADER = 0xAA55;
        public const byte PAKET_NACK = 0xF0;
    }



    // ========================================================================
    // 2. HABERLEŞME SINIFI (fm_haberlesme)
    // ========================================================================
    internal class FM_haberlesme
    {
        private SerialPort _seriPort;

        // Constructor: Port ayarlarını yapıyoruz
        public FM_haberlesme(string portAdi, int baudRate)
        {
            _seriPort = new SerialPort(portAdi, baudRate, Parity.None, 8, StopBits.One);
            _seriPort.ReadTimeout = 50;  
            _seriPort.WriteTimeout = 50;
        }

        public void PortAc()
        {
            if (!_seriPort.IsOpen)
                _seriPort.Open();
        }

        public void PortKapat()
        {
            if (_seriPort.IsOpen)
                _seriPort.Close();
        }




        // --- AKIŞ ŞEMASI: İLETİM PAKETİNİ AL ---
        // STM32'den gelen paketi okur ve hem struct hem de ham byte dizisi olarak döndürür
        public Paket_Durum_t Iletim_Paketini_Al(out Iletim_Paketi_t alinanPaket, out byte[] rawBuffer)
        {
            alinanPaket = default(Iletim_Paketi_t);
            int paketBoyutu = Marshal.SizeOf(typeof(Iletim_Paketi_t));
            rawBuffer = new byte[paketBoyutu];

            int okunanToplam = 0;

            // 1. AŞAMA: İLK BAYTI SONSUZA KADAR BEKLE (Ping-Pong Dondurması)
            // STM32'den veri gelmediği sürece WinForm burayı geçemez ve fizik motoru zamanı durur.
            while (_seriPort.BytesToRead == 0)
            {
                // Sonsuz döngünün işlemci çekirdeğini %100 yormaması için 1 milisaniye nefes aldırıyoruz.
                // Bu işlem Task içinde çalıştığı için arayüzü (UI) dondurmaz.
                //Thread.Sleep(1);
                Thread.Yield();
            }

            // 2. AŞAMA: İLK BAYT GELDİ, ZAMAN AŞIMINI (TIMEOUT) BAŞLAT
            // Artık paketin geri kalanı için 100 ms kuralı devrede.
            Stopwatch sw = Stopwatch.StartNew();

            // 3. AŞAMA: PAKETİN TAMAMLANMASINI BEKLE
            while (okunanToplam < paketBoyutu)
            {
                if (_seriPort.BytesToRead > 0)
                {
                    okunanToplam += _seriPort.Read(rawBuffer, okunanToplam, paketBoyutu - okunanToplam);
                }

                // Eğer ilk bayttan sonra veri akışı kesilir ve paketin tamamlanması 100ms'yi geçerse
                if (sw.ElapsedMilliseconds > 100)
                {
                    return Paket_Durum_t.PAKET_EKSIK; // Sistem kopuk veya eksik veri geldi
                }
            }

            alinanPaket = BytesToStruct<Iletim_Paketi_t>(rawBuffer);
            return Paket_Durum_t.PAKET_TAM;

            /* ReadByte donanım seviyesinde bekler. Veri gelene kadar Thread uyur.
            int ilkBayt = _seriPort.ReadByte();
            if (ilkBayt == -1) return Paket_Durum_t.PAKET_HATA; // Port beklenmedik şekilde kapandıysa

            rawBuffer[0] = (byte)ilkBayt; // İlk baytı dizimize kaydettik

            // 2. AŞAMA: VERİ AKMAYA BAŞLADI (Artık 50ms kuralı geçerli)
            _seriPort.ReadTimeout = 500;
            int okunan = 1; // 1 baytı zaten okuduk, o yüzden 1'den başlıyoruz

            while (okunan < paketBoyutu)
            {
                // Eğer STM32 paketin yarısında çökerse, burası 50ms sonra Timeout fırlatır
                int okunanMiktar = _seriPort.Read(rawBuffer, okunan, paketBoyutu - okunan);
                okunan += okunanMiktar;
            }
            */
            // Byte dizisini Struct'a çevir
            
        }





        // --- AKIŞ ŞEMASI: BOZUK PAKET Mİ? ---
        // Header ve XOR Checksum doğrulamasını yapar
        public bool Bozuk_Paket_Mi(Iletim_Paketi_t paket, byte[] rawBuffer)
        {
            // 1. Header Kontrolü (0xAA55)
            if (paket.header != PaketSabitleri.PAKET_HEADER){return true;}

            // 2. Checksum Kontrolü (Yardımcı fonksiyon ile)
            // Son bayt (checksum baytı) hesaplamaya dahil edilmez
            byte hesaplananChk = Checksum_Hesapla(rawBuffer, rawBuffer.Length - 1);

            if (hesaplananChk != paket.checksum)
            {
                return true;
            }

            return false;
        }




        // --- AKIŞ ŞEMASI: ALARM PAKETİ Mİ? ---
        public bool Alarm_Paketi_Mi(Iletim_Paketi_t paket)
        {
            // Örnek: Durum bayrağı 0xF0 ise STM32 alarm vermiş demektir
            return paket.durum_bayraklari == PaketSabitleri.PAKET_NACK;
        }




        // --- AKIŞ ŞEMASI: GÖNDER ---
        // Oluşturulan iletim paketini byte dizisine çevirir ve SerialPort üzerinden basar
        public void Gonder(Alim_Paketi_t gonderilecekPaket)
        {
            if (!_seriPort.IsOpen) return;

            // Struct'ı byte dizisine çevir
            byte[] buffer = StructToBytes(gonderilecekPaket);

            // Hatta bas
            _seriPort.Write(buffer, 0, buffer.Length);
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
