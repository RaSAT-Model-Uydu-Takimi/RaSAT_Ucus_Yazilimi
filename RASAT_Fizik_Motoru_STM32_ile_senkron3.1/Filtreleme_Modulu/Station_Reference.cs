using System;

namespace FilterModule
{
    public struct Station_Reference_t
    {
        public double ref_lat;
        public double ref_lon;
        public float ref_alt_msl;
        
        public float ref_baro_msl;

        public byte gps_ref_set;
        public byte baro_ref_set;
        
        // Property wrapper for convenience in C# if needed for conditionals (e.g. if (station.gps_ref_set_bool))
        public bool gps_ref_set_bool => gps_ref_set != 0;
        public bool baro_ref_set_bool => baro_ref_set != 0;
    }

    public static class Station_Reference
    {
        /* 
         * Fonksiyon: Station_Init
         * Görevi: İstasyon referans yapısını sıfırlar. 
         */
        public static void Station_Init(ref Station_Reference_t station)
        {
            station.ref_lat = 0.0;
            station.ref_lon = 0.0;
            station.ref_alt_msl = 0.0f;
            station.ref_baro_msl = 0.0f;
            station.gps_ref_set = 0;
            station.baro_ref_set = 0;
        }

        /* 
         * Fonksiyon: Station_SetReference
         * Görevi: Uçuş öncesi (veya ilk fix anında) o anki geçerli
         *         Baro ve GPS verilerini kalkış (0,0,0) noktası olarak kaydeder.
         */
        public static void Station_SetReference(ref Station_Reference_t station, ref DataCenter dc, float sea_level_pa)
        {
            /* GPS Referansı: Sadece fix varsa ve güvenliyse al */
            if (dc.gps.fixQuality > 0 && dc.gps.x.confidence > 0.2f && station.gps_ref_set == 0)
            {
                station.ref_lat = dc.gps.x.calibratedValue;
                station.ref_lon = dc.gps.y.calibratedValue;
                station.ref_alt_msl = dc.gps.z.calibratedValue;
                station.gps_ref_set = 1;
            }

            /* Barometre Referansı: Güvenliyse al */
            if (dc.baro.press.confidence > 0.2f && station.baro_ref_set == 0)
            {
                float baro_msl = 44330.0f * (1.0f - (float)Math.Pow(dc.baro.press.calibratedValue / sea_level_pa, 0.190295f));
                station.ref_baro_msl = baro_msl;
                station.baro_ref_set = 1;
            }
        }

        /* 
         * Fonksiyon: Station_ApplyReference
         * Görevi: Ham/Kalibre edilmiş sensör okumalarından istasyon
         *         referansını çıkartarak yerel AGL ve yerel X/Y'ye hazır hale getirir.
         */
        public static void Station_ApplyReference(ref Station_Reference_t station, ref DataCenter dc, float sea_level_pa)
        {
            /* 1. Barometre MSL -> AGL Çevirimi */
            if (station.baro_ref_set != 0 && dc.baro.press.confidence > 0.0f)
            {
                float current_baro_msl = 44330.0f * (1.0f - (float)Math.Pow(dc.baro.press.calibratedValue / sea_level_pa, 0.190295f));
                /* Baro irtifasını doğrudan hesaplayıp AGL'ye çevirip saklayalım (DataCenter'da baro alt için özel yer yok, M5 bunu hesaplıyor. 
                   Bu yüzden kalibre edilmiş basınç yerine, bir field açılabilir veya M5 içindeki işlemler burada hesaplanıp Z pos_z update'i M5'e bırakılabilir.
                   Fakat tasarım gereği, filtre AGL irtifa ölçümünü M5 içinde baro basıncından dönüştürüyordu. 
                   Bunu bozmamak adına `ref_baro_msl` değerini M5'in kullanabilmesi yeterlidir. 
                   Biz burada doğrudan dataC->baro_alt_agl gibi bir alan açmalıyız veya M5 modülü station'dan okumalı. */
            }

            /* 2. GPS Derece -> Metre (Local Tangent Plane X, Y, Z_AGL) Çevirimi */
            /* Bu kısımlar direkt M5 ve M6 içine gömülebilir veya dc->estimated içerisine ölçüm olarak eklenebilir. 
               Mimarinin en pürüzsüz hali: station->ref_lat / lon'u M6 içinden, station->ref_baro_msl'i M5 içinden kullanmak. */
        }
    }
}
