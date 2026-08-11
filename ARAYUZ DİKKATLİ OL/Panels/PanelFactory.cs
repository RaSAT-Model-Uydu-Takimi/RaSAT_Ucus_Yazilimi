using _10OcakRASAT.Managers;

namespace _10OcakRASAT.Panels
{
    ///  
    /// Panel türlerine göre IPanelContent nesneleri oluşturur
    
    public static class PanelFactory
    {
        private static TelemetryManager? _telemetryManager;

        /// TelemetryManager'ı ayarlar (grafik panelleri için gerekli)
        
        public static void SetTelemetryManager(TelemetryManager manager)
        {
            _telemetryManager = manager;
        }

        /// Mevcut panel türleri
        
        public enum PanelType
        {
            ChartBasinc,
            ChartYukseklik,
            ChartInisHizi,
            ChartSicaklik,
            ChartPil,
            ChartGpsLat,
            ChartGpsLon,
            ChartPitch,
            ChartRoll,
            ChartYaw,
            GpsTrack,
            Model3D,
            TDataLive,
            CommandPanel,
            MapPanel,
            VideoPanel
        }

        // Panel türüne göre IPanelContent oluşturur    
        public static IPanelContent CreatePanel(PanelType type)
        {
            if (_telemetryManager == null)
                throw new InvalidOperationException("TelemetryManager ayarlanmamış!");

            return type switch
            {
                
                // Grafik Panelleri
                PanelType.ChartBasinc => new ChartPanel("Basinc", "Basınç", "Pa", 
                    Color.FromArgb(33, 150, 243), _telemetryManager),
                
                PanelType.ChartYukseklik => new ChartPanel("Yukseklik", "Yükseklik", "m", 
                    Color.FromArgb(255, 152, 0), _telemetryManager),
                
                PanelType.ChartInisHizi => new ChartPanel("InisHizi", "İniş Hızı", "m/s", 
                    Color.FromArgb(156, 39, 176), _telemetryManager),
                
                PanelType.ChartSicaklik => new ChartPanel("Sicaklik", "Sıcaklık", "°C", 
                    Color.FromArgb(244, 67, 54), _telemetryManager),
                
                PanelType.ChartPil => new ChartPanel("Pil", "Pil Gerilimi", "V", 
                    Color.FromArgb(76, 175, 80), _telemetryManager),
                
                PanelType.ChartGpsLat => new ChartPanel("GpsLat", "GPS Latitude", "°", 
                    Color.FromArgb(0, 188, 212), _telemetryManager),
                
                PanelType.ChartGpsLon => new ChartPanel("GpsLon", "GPS Longitude", "°", 
                    Color.FromArgb(0, 150, 136), _telemetryManager),
                
                PanelType.ChartPitch => new ChartPanel("Pitch", "Pitch", "°", 
                    Color.FromArgb(233, 30, 99), _telemetryManager),
                
                PanelType.ChartRoll => new ChartPanel("Roll", "Roll", "°", 
                    Color.FromArgb(103, 58, 183), _telemetryManager),
                
                PanelType.ChartYaw => new ChartPanel("Yaw", "Yaw", "°", 
                    Color.FromArgb(63, 81, 181), _telemetryManager),
                
                // Özel Paneller
                PanelType.GpsTrack => new GpsTrackPanel(_telemetryManager),
                PanelType.Model3D => new Model3DPanel(_telemetryManager),
                PanelType.TDataLive => new TDataLivePanel(_telemetryManager),
                PanelType.CommandPanel => new CommandPanel(_telemetryManager),
                PanelType.MapPanel => new MapPanel(_telemetryManager),
                PanelType.VideoPanel => new VideoPanel(),
                
                _ => new TestLabelPanel("Bilinmeyen Panel")
            };
        }

        ///  
        /// Tüm panel türlerinin listesini döndürür
        
        public static List<PanelTypeInfo> GetAvailablePanels()
        {
            return new List<PanelTypeInfo>
            {
                // Grafik Panelleri
                new(PanelType.ChartBasinc, "📊 Basınç", "Basınç grafiği"),
                new(PanelType.ChartYukseklik, "📊 Yükseklik", "Yükseklik grafiği"),
                new(PanelType.ChartInisHizi, "📊 İniş Hızı", "İniş hızı grafiği"),
                new(PanelType.ChartSicaklik, "📊 Sıcaklık", "Sıcaklık grafiği"),
                new(PanelType.ChartPil, "📊 Pil Gerilimi", "Pil gerilimi grafiği"),
                new(PanelType.ChartGpsLat, "📊 GPS Lat", "GPS Latitude grafiği"),
                new(PanelType.ChartGpsLon, "📊 GPS Lon", "GPS Longitude grafiği"),
                new(PanelType.ChartPitch, "📊 Pitch", "Pitch açısı grafiği"),
                new(PanelType.ChartRoll, "📊 Roll", "Roll açısı grafiği"),
                new(PanelType.ChartYaw, "📊 Yaw", "Yaw açısı grafiği"),
                
                // Özel Paneller
                new(PanelType.GpsTrack, "🗺️ GPS İz", "GPS konum izleme (Lat/Lon)"),
                new(PanelType.Model3D, "🛰️ 3D Model", "Uydu 3D silindir modeli"),
                new(PanelType.TDataLive, "📡 TData Canlı", "Tüm telemetri verilerini anlık gösterir"),
                new(PanelType.CommandPanel, "🎮 Komut Merkezi", "Manuel komutlar ve LED kontrolü"),
                new(PanelType.MapPanel, "🌍 Harita", "GPS konumlarını gerçek harita üzerinde gösterir"),
                new(PanelType.VideoPanel, "📹 Video", "Canlı video akışı (OBS Virtual Camera)"),
            };
        }
    }

    /// Panel türü bilgisi (listede gösterim için)
    
    public record PanelTypeInfo(PanelFactory.PanelType Type, string Name, string Description);
}
