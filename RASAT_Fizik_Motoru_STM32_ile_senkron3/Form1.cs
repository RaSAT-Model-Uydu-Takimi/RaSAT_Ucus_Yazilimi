using System;
using System.Collections.Concurrent;
using System.IO.Ports;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;
namespace RASAT_Fizik_Motoru_STM32_ile_senkron3
{
    public partial class Form1 : Form
    {

        // Kütüphane nesnelerimiz
        private FM_haberlesme haberlesme;
        private FM_sistem fizikMotoru;
        private FM_logging logging;

        // Asenkron çalýþma elemanlarý
        private CancellationTokenSource cts;
        private Task motorTask;
        private bool isRunning = false;



        public Form1()
        {
            InitializeComponent();
            
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            haberlesme = new FM_haberlesme("COM5", 921600);
            fizikMotoru = new FM_sistem();
            logging = new FM_logging();
        }




        private void btnBaslat_Click(object sender, EventArgs e)
        {
            if (isRunning) { return; }

            haberlesme.PortAc();
            Thread.Sleep(200);
            cts = new CancellationTokenSource();
            isRunning = true;
            UIguncelle.Enabled = true;
            logging.LoglamayiBaslat();
            // 400Hz (2.5ms) döngüyü baþlat
            motorTask = Task.Factory.StartNew(() => AnaFizikDongusu(cts.Token),
                                              cts.Token,
                                              TaskCreationOptions.LongRunning,
                                              TaskScheduler.Default);
            
        }




        private Iletim_Paketi_t gelenPaket;
        private Alim_Paketi_t gidenPaket = new Alim_Paketi_t();
        private readonly object _paketKilidi = new object(); // Kilit nesnesi
        private bool uiguncelle = false;
        private string zamanGelen;
        private string zamanGiden;

        private byte watchdog = 0;
        private void AnaFizikDongusu(CancellationToken token)
        {
            // Geçici paket depolarý
            Iletim_Paketi_t gelenPaketbuff;
            Alim_Paketi_t gidenPaketbuff = new Alim_Paketi_t(); // Baþlangýç paketi
            byte[] rawBuffer;


            // --- BAÞLANGIÇ ENJEKSÝYONU ---
            // 1. Fizik motorunu baþlangýç deðerleriyle (t=0) bir kez çalýþtýr
            fizikMotoru.Index_Artir();
            gidenPaketbuff = FM_sistem.BAS_PAKETI;
            fizikMotoru.Alim_Paketini_Olustur(ref gidenPaketbuff);
            // 2. Ýlk paketi gönder ki STM32 bizi duysun
            haberlesme.Gonder(gidenPaketbuff);
            // 3. Senkronize et
            lock (_paketKilidi) { gidenPaket = gidenPaketbuff; uiguncelle = true; }

            // STM32'ye giden paketin bilgisayardan çýktýðý saat
            zamanGiden = DateTime.Now.ToString("HH:mm:ss.fff");




            while (!token.IsCancellationRequested)
            {
                //lock (_paketKilidi) { watchdog = 10; uiguncelle = true; }
                // 1. Akýþ Þemasý: Ýletim Paketini Al (STM32'den)
                Paket_Durum_t sonuc = haberlesme.Iletim_Paketini_Al(out gelenPaketbuff, out rawBuffer);

                // STM32'den gelen paketin geldiði varsayýlan saat
                zamanGelen = DateTime.Now.ToString("HH:mm:ss.fff");
                // Ýþi arka plandaki kuyruða fýrlatýp hemen kendi döngümüze dönüyoruz
                logging.KuyrugaLogEkle(gidenPaketbuff, zamanGiden, gelenPaketbuff, zamanGelen);




                // 2. Akýþ Þemasý: Bozuk Paket Mi?
                if (sonuc == Paket_Durum_t.PAKET_EKSIK)
                {
                    haberlesme.Gonder(FM_sistem.ALARM_PAKETI);
                    //lock (_paketKilidi) { watchdog = 1; }
                    goto atla;
                }

                else if (sonuc == Paket_Durum_t.PAKET_HATA)
                {
                    //lock (_paketKilidi) { watchdog = 5; }
                    Thread.Sleep(500);
                    goto atla;
                }

                else if (haberlesme.Bozuk_Paket_Mi(gelenPaketbuff, rawBuffer))
                {
                    haberlesme.Gonder(FM_sistem.ALARM_PAKETI);
                    //lock (_paketKilidi) { watchdog = 2; }
                    goto atla;
                }

                else if (haberlesme.Alarm_Paketi_Mi(gelenPaketbuff))
                {
                    fizikMotoru.En_Son_Paketi_Bir_Daha_Olustur(ref gidenPaketbuff);
                    haberlesme.Gonder(gidenPaketbuff);
                    //lock (_paketKilidi) { watchdog = 3; }
                    goto atla;
                }

                else if(sonuc == Paket_Durum_t.PAKET_TAM)
                {
                    //lock (_paketKilidi) { watchdog = 50; }
                }


                
                // Normal akýþ
                //lock (_paketKilidi) { watchdog = 4; }
                fizikMotoru.Davranisi_Uygula(); // STM32 komutlarýný iþle
                fizikMotoru.Index_Artir();      // Ýndeksi artýr
                fizikMotoru.Fizigi_Calistir(ref gelenPaketbuff, ref gidenPaketbuff);
                fizikMotoru.Alim_Paketini_Olustur(ref gidenPaketbuff);
                

                // 4. Gönder (STM32'ye)
                haberlesme.Gonder(gidenPaketbuff);
               
                // STM32'ye giden paketin bilgisayardan çýktýðý saat
                zamanGiden = DateTime.Now.ToString("HH:mm:ss.fff");          

                atla:;
                lock (_paketKilidi)
                {
                    gelenPaket = gelenPaketbuff;
                    gidenPaket = gidenPaketbuff;
                    uiguncelle = true;
                }
            }
        }

        private void btnDurdur_Click(object sender, EventArgs e)
        {
            UIguncelle.Enabled = false;            
            isRunning = false;
            cts?.Cancel();
            logging.LoglamayiDurdur();            
            Thread.Sleep(200);
            haberlesme.PortKapat();



        }

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            cts?.Cancel();
            if (haberlesme != null) haberlesme.PortKapat();
        }

        private void UIguncelle_Tick(object sender, EventArgs e)
        {
            Iletim_Paketi_t yerelGelen;
            Alim_Paketi_t yerelGiden;
            byte yereldog;
            lock (_paketKilidi)
            {
                yerelGelen = gelenPaket;
                yerelGiden = gidenPaket;
                yereldog = watchdog;
            }
            if (uiguncelle)
            {
                uiguncelle = false;
                gidenindex.Text = "Giden Index: " + yerelGiden.index.ToString();
                gelenindex.Text = "Gelen Index: " + yerelGelen.index.ToString();
                mi1degeri.Text = "MI1 Degeri: " + yerelGelen.mi1.ToString();
                //watchdoglbl.Text = watchdog.ToString();
                //textBox1.AppendText(watchdog.ToString() + " : ");
            }
        }
    }
}
