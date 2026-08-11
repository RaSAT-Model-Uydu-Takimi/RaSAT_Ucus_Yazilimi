using System;

namespace RASATMobile.Models
{
    public class TData
    {
        public int PaketNumarasi { get; set; } 
        public int UyduStatusu { get; set; }  
        public string HataKodu { get; set; }   
        public string GondermeSaati { get; set; }

        public double Basinc { get; set; }     
        public double Yukseklik { get; set; }   
        public double InisHizi { get; set; }  
        public double Sicaklik { get; set; }    
        public double PilGerilimi { get; set; } 

        public double GpsLatitude { get; set; } 
        public double GpsLongitude { get; set; }
        public double GpsAltitude { get; set; } 

        public double Pitch { get; set; }   
        public double Roll { get; set; }   
        public double Yaw { get; set; }  

        public string RHRHRH { get; set; } 
        public int TakimNo { get; set; } 
        
        // Firebase için timestamp
        public long TimeStamp { get; set; }
    }
}
