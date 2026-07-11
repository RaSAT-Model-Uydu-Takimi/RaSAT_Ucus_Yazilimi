using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Collections.Concurrent; // BlockingCollection için gerekli
using System.IO; // Dosya işlemleri (StreamWriter) için gerekli
using System.Threading;

namespace RASAT_Fizik_Motoru_STM32_ile_senkron3
{
    internal class FM_logging
    {

        // RAM üzerinde verileri biriktireceğimiz thread-safe (iş parçacığı güvenli) kuyruk.
        private BlockingCollection<LogTasiyici> _logKuyrugu;
        private Task _logTask; // Arka planda diske yazacak iş parçacığı
        private CancellationTokenSource _logCts;

        // Kuyrukta taşıyacağımız veri paketi kılıfı
        public struct LogTasiyici
        {
            public string ZamanGelen;
            public string ZamanGiden;
            public Iletim_Paketi_t GelenPaket;
            public Alim_Paketi_t GidenPaket;
        }

        // Simülasyon başlatıldığında (btnBaslat) çağrılacak
        public void LoglamayiBaslat()
        {
            _logKuyrugu = new BlockingCollection<LogTasiyici>();
            _logCts = new CancellationTokenSource();

            // Dosya isimleri diğer kayıtlarla karışmasın diye tarih-saat etiketiyle oluşturulur
            string zamanDamgasi = DateTime.Now.ToString("yyyyMMdd_HHmmss");
            string dosyaGelen = $"Gelen_Veriler_{zamanDamgasi}.csv";
            string dosyaGiden = $"Giden_Veriler_{zamanDamgasi}.csv";
            string dosyaBirlesik = $"Birlesik_Veriler_{zamanDamgasi}.csv";

            // Başlık (Header) Satırlarının CSV'ye yazılması
            // Excel'de hücrelere düzgün ayrılması için aralara sadece virgül (,) koyuyoruz.
            File.WriteAllText(dosyaGelen, "Saat,Header,Index,MI1,MI2,MI3,MI4,DurumBayraklari,Checksum\n");
            File.WriteAllText(dosyaGiden, "Saat,Header,Index,AccX,AccY,AccZ,GyroX,GyroY,GyroZ,MagX,MagY,MagZ,Basinc,Sicaklik,GpsLat,GpsLon,GpsAlt,GpsVel,BatV,BatA,Rezerve,Checksum\n");

            // Birleşik tablo için önce Giden, hemen yanına Gelen başlıkları
            string birlesikBaslik = "Giden Saat,Giden Header,Giden Index,AccX,AccY,AccZ,GyroX,GyroY,GyroZ,MagX,MagY,MagZ,Basinc,Sicaklik,GpsLat,GpsLon,GpsAlt,GpsVel,BatV,BatA,Rezerve,Giden Checksum," +
                                    "Gelen Saat,Gelen Header,Gelen Index,MI1,MI2,MI3,MI4,DurumBayraklari,Gelen Checksum\n";
            File.WriteAllText(dosyaBirlesik, birlesikBaslik);

            // Tüketici task'ı başlatıyoruz (Sonsuz döngüde kuyruğu dinleyecek)
            _logTask = Task.Run(() => LogYaziciTask(dosyaGelen, dosyaGiden, dosyaBirlesik, _logCts.Token));
        }

        // Form1.cs ana döngüsünden saniyede 400 kez buraya veri fırlatılacak
        public void KuyrugaLogEkle(Alim_Paketi_t giden, string zamanGiden, Iletim_Paketi_t gelen, string zamanGelen)
        {
            // Eğer sistem durdurulmadıysa veriyi RAM kuyruğuna at
            if (_logKuyrugu != null && !_logKuyrugu.IsAddingCompleted)
            {
                _logKuyrugu.Add(new LogTasiyici
                {
                    GidenPaket = giden,
                    ZamanGiden = zamanGiden,
                    GelenPaket = gelen,
                    ZamanGelen = zamanGelen
                });
            }
        }

        // SADECE ARKA PLANDA ÇALIŞAN, ANA DÖNGÜYÜ YAVAŞLATMAYAN YAZICI FONKSİYON
        private void LogYaziciTask(string pathGelen, string pathGiden, string pathBirlesik, CancellationToken token)
        {
            // StreamWriter bellek tamponu kullanarak dosyayı sürekli aç-kapat yapmadan diski rahatlatır.
            using (StreamWriter swGelen = new StreamWriter(pathGelen, true))
            using (StreamWriter swGiden = new StreamWriter(pathGiden, true))
            using (StreamWriter swBirlesik = new StreamWriter(pathBirlesik, true))
            {
                try
                {
                    // Kuyrukta veri olduğu sürece çekip diske yazar (Üretici veriyi kesene kadar döner)
                    foreach (var log in _logKuyrugu.GetConsumingEnumerable(token))
                    {
                        var gln = log.GelenPaket;
                        var gdn = log.GidenPaket;

                        // 1. GELEN VERİ SATIRI
                        string satirGelen = $"{log.ZamanGelen},{gln.header},{gln.index},{gln.mi1},{gln.mi2},{gln.mi3},{gln.mi4},{gln.durum_bayraklari},{gln.checksum}";
                        //swGelen.WriteLine(satirGelen);

                        // 2. GİDEN VERİ SATIRI
                        string satirGiden = $"{log.ZamanGiden},{gdn.header},{gdn.index},{gdn.acc_x},{gdn.acc_y},{gdn.acc_z},{gdn.gyro_x},{gdn.gyro_y},{gdn.gyro_z},{gdn.mag_x},{gdn.mag_y},{gdn.mag_z},{gdn.basinc},{gdn.sicaklik},{gdn.gps_lat},{gdn.gps_lon},{gdn.gps_alt},{gdn.gps_vel},{gdn.bat_v},{gdn.bat_a},{gdn.rezerve},{gdn.checksum}";
                        //swGiden.WriteLine(satirGiden);

                        // 3. BİRLEŞİK VERİ SATIRI
                        swBirlesik.WriteLine($"{satirGiden},{satirGelen}");
                    }
                }
                catch (OperationCanceledException)
                {
                    // Görev durdurulduğunda fırlatılan doğal istisna, sorun yok.
                }
            }
        }

        // Simülasyon durdurulduğunda (btnDurdur) çağrılacak
        public void LoglamayiDurdur()
        {
            if (_logKuyrugu != null && !_logKuyrugu.IsAddingCompleted)
            {
                _logKuyrugu.CompleteAdding(); // Üretimi durdur, kuyruğun kapağını kapat
                _logTask?.Wait(2000); // Tüketicinin elindeki son verileri diske yazması için 2 sn müsaade et
                _logCts?.Cancel(); // Task'ı güvenle sonlandır
                _logKuyrugu.Dispose();
            }
        }



    }
}
