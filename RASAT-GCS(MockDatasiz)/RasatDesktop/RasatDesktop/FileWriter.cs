using System;
using System.IO;
using System.Text;

public class FileWriter
{
    // Yarışma formatına uygun sabit dosya yolu
    public string FilePath { get; private set; }

    // Takım numarasını buraya girmelisin (Dosya isminde kullanılacak)
    private const string TakimNo = "6334";

    public FileWriter(string directoryPath, string? customFileName = null)
    {
        // 1. Klasör yoksa oluştur
        if (!Directory.Exists(directoryPath))
        {
            Directory.CreateDirectory(directoryPath);
        }

        // 2. Dosya ismini belirle (özel veya varsayılan)
        // Varsayılan Format: TMUY2026_TAKIMNO_TLM.csv 
        string fileName = customFileName ?? "TMUY2026_6334_TLM.csv";
        FilePath = Path.Combine(directoryPath, fileName);

        // 3. Dosya VAR MI kontrolü
        // Eğer dosya ZATEN VARSA başlık yazmıyoruz, sadece üzerine ekleyeceğiz.
        // Eğer dosya YOKSA oluşturup başlıkları yazıyoruz.
        if (!File.Exists(FilePath))
        {
            WriteHeaders();
        }
    }

    // Dosya ilk kez oluşturulurken başlık ve birimleri yazar
    private void WriteHeaders()
    {
        StringBuilder sb = new StringBuilder();

        // 1. Satır: Değişken Başlıkları
        sb.AppendLine("PAKET NUMARASI,UYDU STATÜSÜ,HATA KODU,GÖNDERME SAATİ,BASINÇ,YÜKSEKLİK,İNİŞ HIZI,SICAKLIK,PİL GERİLİMİ,GPS LATITUDE,GPS LONGITUDE,GPS ALTITUDE,PITCH,ROLL,YAW,RHRHRH,TAKIM NO");

        // 2. Satır: Değişken Birimleri (Yarışma formatına uygun)
        sb.AppendLine("-,-,-,GG/AA/YYYY SS:DD:SS,Pa,m,m/s,°C,V,°,°,m,°,°,°,-,-");

        // Dosyayı oluştur ve metni içine yaz
        File.WriteAllText(FilePath, sb.ToString(), Encoding.UTF8);
    }

    // Veri geldiğinde dosyanın SONUNA ekler (Append)
    public void WriteData(TData data)
    {
        try
        {
            string lineToAdd = data.ToCsvLine();

            // AppendAllLines: Dosyayı silmez, en son satırın altına yenisini ekler
            File.AppendAllLines(FilePath, new[] { lineToAdd }, Encoding.UTF8);
        }
        catch (Exception)
        {
            // Hata durumunda (Örn: Dosya Excel'de açıksa) burası çalışır
        }
    }
}