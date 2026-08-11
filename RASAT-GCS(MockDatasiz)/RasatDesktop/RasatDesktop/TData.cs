using System;
using System.Globalization;

public class TData
{
    // Genel Veri
    public int PaketNumarasi { get; set; } 
    public int UyduStatusu { get; set; }  
    public string HataKodu { get; set; }   
    public string GondermeSaati { get; set; }

    // Atmosferik ve Güç Verileri
    public double Basinc { get; set; }     
    public double Yukseklik { get; set; }   
    public double InisHizi { get; set; }  
    public double Sicaklik { get; set; }    
    public double PilGerilimi { get; set; } 

    // GPS ve Konum Verileri
    public double GpsLatitude { get; set; } 
    public double GpsLongitude { get; set; }
    public double GpsAltitude { get; set; } 

    //Eksen Verileri
    public double Pitch { get; set; }   
    public double Roll { get; set; }   
    public double Yaw { get; set; }  

    // Bonus Görev ve Takım Tanımlama
    public string RHRHRH { get; set; } 
    public int TakimNo { get; set; }    

    // Gönderilen Komutlar (GCS -> Uydu)
    public string SentCommand { get; set; } = "-";
    public string SentCommandTime { get; set; } = "-";

    public static TData Parse(string rawString)
    {
        try
        {
            string[] values = rawString.Split(',');
            if (values.Length < 17) return null;

            var data = new TData
            {
                // Kimlik ve Statü 
                PaketNumarasi = int.Parse(values[0]),
                UyduStatusu = int.Parse(values[1]),
                HataKodu = values[2],
                GondermeSaati = values[3],

                // Sayısal Veriler
                Basinc = double.Parse(values[4], CultureInfo.InvariantCulture),
                Yukseklik = double.Parse(values[5], CultureInfo.InvariantCulture),
                InisHizi = double.Parse(values[6], CultureInfo.InvariantCulture),
                Sicaklik = double.Parse(values[7], CultureInfo.InvariantCulture),
                PilGerilimi = double.Parse(values[8], CultureInfo.InvariantCulture),

                // Konum ve Eksen Veriler
                GpsLatitude = double.Parse(values[9], CultureInfo.InvariantCulture),
                GpsLongitude = double.Parse(values[10], CultureInfo.InvariantCulture),
                GpsAltitude = double.Parse(values[11], CultureInfo.InvariantCulture),
                Pitch = double.Parse(values[12], CultureInfo.InvariantCulture),
                Roll = double.Parse(values[13], CultureInfo.InvariantCulture),
                Yaw = double.Parse(values[14], CultureInfo.InvariantCulture),

                // Bonus ve Takım No Ayrıştırma
                RHRHRH = values[15],
                TakimNo = int.Parse(values[16])
            };

            // Eğer yeni format (19 sütun) ise
            if (values.Length >= 19)
            {
                data.SentCommand = values[17];
                data.SentCommandTime = values[18];
            }

            return data;
        }
        catch { return null; }
    }
    public string ToCsvLine()
    {
        return string.Format(CultureInfo.InvariantCulture,
            "{0},{1},{2},{3},{4},{5},{6},{7},{8},{9},{10},{11},{12},{13},{14},{15},{16},{17},{18}",
            PaketNumarasi,
            UyduStatusu,
            HataKodu,
            GondermeSaati,
            Basinc,
            Yukseklik,
            InisHizi,
            Sicaklik,
            PilGerilimi,
            GpsLatitude,
            GpsLongitude,
            GpsAltitude,
            Pitch,
            Roll,
            Yaw,
            RHRHRH,
            TakimNo,
            SentCommand,
            SentCommandTime);
    }
}
