using System;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace RASAT_Fizik_Motoru_STM32_ile_senkron3._1
{
    public partial class Form1 : Form
    {
        private FM_haberlesme haberlesme;
        private FM_sistem fizikMotoru;
        private FM_logging logging;

        private CancellationTokenSource cts;
        private Task motorTask;
        private bool isRunning = false;

        private Iletim_Paketi_t gelenPaket;
        private Alim_Paketi_t gidenPaket = new Alim_Paketi_t();
        private readonly object _paketKilidi = new object();
        private bool uiguncelle = false;

        public Form1()
        {
            InitializeComponent();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            haberlesme = new FM_haberlesme("COM5", 921600);
            fizikMotoru = new FM_sistem();
            logging = new FM_logging();
            
            // Ayarlar penceresi icin baslangic atamalari
            pgAyarlar.SelectedObject = fizikMotoru.cevreSartlari;
            pgDurum.SelectedObject = fizikMotoru.uyduDurumu; // Izleme
            
            // Görseli Yükle
            try
            {
                pbUyduPlan.ImageLocation = System.IO.Path.Combine(Application.StartupPath, @"..\..\..\Resources\uydu_plan.jpg");
            }
            catch (Exception) { }
        }

        private void btnBaslat_Click(object sender, EventArgs e)
        {
            if (isRunning) return;

            haberlesme.PortAc();
            Thread.Sleep(200);
            cts = new CancellationTokenSource();
            isRunning = true;
            UIguncelle.Enabled = true;
            logging.LoglamayiBaslat();
            
            motorTask = Task.Factory.StartNew(() => AnaFizikDongusu(cts.Token),
                                              cts.Token,
                                              TaskCreationOptions.LongRunning,
                                              TaskScheduler.Default);
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

        private void AnaFizikDongusu(CancellationToken token)
        {
            Iletim_Paketi_t gelenPaketbuff;
            Alim_Paketi_t gidenPaketbuff = FM_sistem.BAS_PAKETI;
            byte[] rawBuffer;

            fizikMotoru.Index_Artir();
            fizikMotoru.Alim_Paketini_Olustur(ref gidenPaketbuff);
            haberlesme.Gonder(gidenPaketbuff);

            lock (_paketKilidi) { gidenPaket = gidenPaketbuff; uiguncelle = true; }

            while (!token.IsCancellationRequested)
            {
                Paket_Durum_t sonuc = haberlesme.Iletim_Paketini_Al(out gelenPaketbuff, out rawBuffer);

                if (sonuc == Paket_Durum_t.PAKET_EKSIK || haberlesme.Bozuk_Paket_Mi(gelenPaketbuff, rawBuffer))
                {
                    haberlesme.Gonder(FM_sistem.ALARM_PAKETI);
                    goto atla;
                }
                else if (sonuc == Paket_Durum_t.PAKET_HATA)
                {
                    Thread.Sleep(500);
                    goto atla;
                }
                else if (haberlesme.Alarm_Paketi_Mi(gelenPaketbuff))
                {
                    fizikMotoru.En_Son_Paketi_Bir_Daha_Olustur(ref gidenPaketbuff);
                    haberlesme.Gonder(gidenPaketbuff);
                    goto atla;
                }
                
                fizikMotoru.Index_Artir();
                fizikMotoru.Fizigi_Calistir(ref gelenPaketbuff, ref gidenPaketbuff);
                fizikMotoru.Alim_Paketini_Olustur(ref gidenPaketbuff);
                
                haberlesme.Gonder(gidenPaketbuff);

                atla:;
                lock (_paketKilidi)
                {
                    gelenPaket = gelenPaketbuff;
                    gidenPaket = gidenPaketbuff;
                    uiguncelle = true;
                }
            }
        }

        private void UIguncelle_Tick(object sender, EventArgs e)
        {
            Iletim_Paketi_t yerelGelen;
            Alim_Paketi_t yerelGiden;
            lock (_paketKilidi)
            {
                yerelGelen = gelenPaket;
                yerelGiden = gidenPaket;
            }
            if (uiguncelle)
            {
                uiguncelle = false;
                lblGiden.Text = "Giden Index: " + yerelGiden.index.ToString();
                lblGelen.Text = "Gelen Index: " + yerelGelen.index.ToString();
                
                // Durum penceresini (PropertyGrid) yenile (Sadece gorsel guncelleme, referans ayni kalir)
                pgDurum.Refresh();
            }
        }

        private void cbAyarSecim_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (cbAyarSecim.SelectedIndex == 0)
                pgAyarlar.SelectedObject = fizikMotoru.cevreSartlari;
            // Diger sekmeler henuz tam baglanmadi (Sadece gorsel tasarim gosterimi icin)
        }
    }
}
