using GMap.NET;
using GMap.NET.MapProviders;
using GMap.NET.WindowsForms;
using GMap.NET.WindowsForms.Markers;
using _10OcakRASAT.Managers;
using Serilog;

namespace _10OcakRASAT.Panels
{
    /// <summary>
    /// Harita görüntüleme modları
    /// </summary>
    public enum MapViewMode
    {
        /// <summary>Tüm GPS noktalarını göster ve haritayı sığdır</summary>
        FullView = 0,
        
        /// <summary>Son konumu takip et (sabit zoom)</summary>
        Follow = 1,
        
        /// <summary>Serbest gezinme (kullanıcı kontrollü)</summary>
        Inspect = 2
    }

    /// <summary>
    /// GMap.NET kullanarak gerçek harita görüntüleme paneli
    /// İlk ve son GPS konumlarını işaretler
    /// </summary>
    public class MapPanel : IPanelContent
    {
        private readonly TelemetryManager _telemetryManager;
        
        private Panel? _container;
        private GMapControl? _mapControl;
        private Label? _infoLabel;
        private Label? _modeLabel;
        private TrackBar? _modeSlider;
        private MapViewMode _currentMode = MapViewMode.FullView;
        
        // GPS koordinat geçmişi
        private List<(double lat, double lon, DateTime time)> _gpsHistory = new();
        private readonly object _lockObject = new();
        
        // Harita katmanları
        private GMapOverlay? _markersOverlay;
        private GMapOverlay? _routeOverlay;
        
        public string PanelName => "🌍 Harita";
        public string Description => "GPS konumlarını harita üzerinde gösterir";
        
        public MapPanel(TelemetryManager telemetryManager)
        {
            _telemetryManager = telemetryManager;
        }
        
        public Control CreateControl()
        {
            _container = new Panel
            {
                Dock = DockStyle.Fill,
                BackColor = Color.FromArgb(28, 28, 28)
            };
            
            // Üst bilgi paneli
            var topPanel = new Panel
            {
                Dock = DockStyle.Top,
                Height = 40,
                BackColor = Color.FromArgb(35, 35, 35),
                Padding = new Padding(8, 0, 8, 0)
            };
            
            // Mod label
            _modeLabel = new Label
            {
                Text = "🌍 Full View",
                ForeColor = Color.White,
                Font = new Font("Segoe UI", 9, FontStyle.Bold),
                Location = new Point(8, 10),
                AutoSize = true
            };
            
            // Mod slider
            _modeSlider = new TrackBar
            {
                Minimum = 0,
                Maximum = 2,
                Value = 0,
                TickStyle = TickStyle.None,
                Width = 90,
                Height = 25,
                Location = new Point(100, 7),
                BackColor = Color.FromArgb(35, 35, 35)
            };
            _modeSlider.ValueChanged += ModeSlider_ValueChanged;
            
            // Bilgi etiketi
            _infoLabel = new Label
            {
                Text = "GPS verisi bekleniyor...",
                ForeColor = Color.LightGray,
                Font = new Font("Segoe UI", 8),
                Location = new Point(200, 12),
                AutoSize = true
            };
            
            // Harita sağlayıcı seçici
            var providerCombo = new ComboBox
            {
                Dock = DockStyle.Right,
                Width = 130,
                DropDownStyle = ComboBoxStyle.DropDownList,
                BackColor = Color.FromArgb(50, 50, 50),
                ForeColor = Color.White,
                FlatStyle = FlatStyle.Flat
            };
            providerCombo.Items.AddRange(new object[] { "OpenStreetMap", "Google Map", "Google Satellite", "Bing Map" });
            providerCombo.SelectedIndex = 0;
            providerCombo.SelectedIndexChanged += ProviderCombo_SelectedIndexChanged;
            
            topPanel.Controls.Add(_modeLabel);
            topPanel.Controls.Add(_modeSlider);
            topPanel.Controls.Add(_infoLabel);
            topPanel.Controls.Add(providerCombo);
            
            // GMap kontrol
            _mapControl = new GMapControl
            {
                Dock = DockStyle.Fill,
                MinZoom = 2,
                MaxZoom = 18,
                Zoom = 10,
                DragButton = MouseButtons.Left,
                CanDragMap = true,
                ShowCenter = false,
                MapProvider = OpenStreetMapProvider.Instance
            };
            
            // GMaps.NET ayarları
            GMapProvider.UserAgent = "10OcakRASAT/1.0";
            _mapControl.Manager.Mode = AccessMode.ServerAndCache;
            
            // Başlangıç pozisyonu (Türkiye merkezi)
            _mapControl.Position = new PointLatLng(39.9334, 32.8597);
            
            // Harita katmanları oluştur
            _markersOverlay = new GMapOverlay("markers");
            _routeOverlay = new GMapOverlay("routes");
            _mapControl.Overlays.Add(_routeOverlay);
            _mapControl.Overlays.Add(_markersOverlay);
            
            _container.Controls.Add(_mapControl);
            _container.Controls.Add(topPanel);
            
            LoadHistory();

            // Telemetri event'ine abone ol
            _telemetryManager.OnDataReceived += OnNewData;
            
            return _container;
        }

        private void LoadHistory()
        {
            var history = _telemetryManager.DataHistory;
            if (history.Count == 0) return;

            Log.Information($"MapPanel: {history.Count} geçmiş veri kontrol ediliyor.");

            lock (_lockObject)
            {
                foreach (var data in history)
                {
                    if (data.GpsLatitude != 0 && data.GpsLongitude != 0)
                    {
                        _gpsHistory.Add((data.GpsLatitude, data.GpsLongitude, DateTime.Now));
                    }
                }
            }

            if (_gpsHistory.Count > 0)
            {
                Log.Information($"MapPanel: {_gpsHistory.Count} geçerli GPS noktası yüklendi.");
                UpdateMap();
            }
        }
        
        private void ModeSlider_ValueChanged(object? sender, EventArgs e)
        {
            if (_modeSlider == null || _modeLabel == null) return;
            
            _currentMode = (MapViewMode)_modeSlider.Value;
            
            _modeLabel.Text = _currentMode switch
            {
                MapViewMode.FullView => "🌍 Full View",
                MapViewMode.Follow => "🎯 Follow",
                MapViewMode.Inspect => "🔍 Inspect",
                _ => "🌍 Full View"
            };
            
            // Inspect modunda sürüklemeyi etkinleştir
            if (_mapControl != null)
            {
                _mapControl.CanDragMap = true;
            }
            
            UpdateMap();
        }
        
        private void ProviderCombo_SelectedIndexChanged(object? sender, EventArgs e)
        {
            if (_mapControl == null || sender is not ComboBox combo) return;
            
            _mapControl.MapProvider = combo.SelectedIndex switch
            {
                0 => OpenStreetMapProvider.Instance,
                1 => GoogleMapProvider.Instance,
                2 => GoogleSatelliteMapProvider.Instance,
                3 => BingMapProvider.Instance,
                _ => OpenStreetMapProvider.Instance
            };
            
            _mapControl.Refresh();
        }
        
        private void OnNewData(TData data)
        {
            if (data.GpsLatitude == 0 && data.GpsLongitude == 0) return;
            
            lock (_lockObject)
            {
                _gpsHistory.Add((data.GpsLatitude, data.GpsLongitude, DateTime.Now));
            }
            
            UpdateMap();
        }
        
        private void UpdateMap()
        {
            if (_mapControl == null || _markersOverlay == null || _routeOverlay == null) return;
            
            if (_mapControl.InvokeRequired)
            {
                _mapControl.Invoke(new Action(UpdateMapInternal));
            }
            else
            {
                UpdateMapInternal();
            }
        }
        
        private void UpdateMapInternal()
        {
            if (_mapControl == null || _markersOverlay == null || _routeOverlay == null || _infoLabel == null) return;
            
            List<(double lat, double lon, DateTime time)> history;
            
            lock (_lockObject)
            {
                if (_gpsHistory.Count == 0) return;
                history = _gpsHistory.ToList();
            }
            
            // Katmanları temizle
            _markersOverlay.Markers.Clear();
            _routeOverlay.Routes.Clear();
            
            // İlk konum (yeşil marker)
            var firstPoint = history.First();
            var startMarker = new GMarkerGoogle(
                new PointLatLng(firstPoint.lat, firstPoint.lon),
                GMarkerGoogleType.green_dot);
            startMarker.ToolTipText = $"🚀 BAŞLANGIÇ\n" +
                                      $"Enlem: {firstPoint.lat:F6}°\n" +
                                      $"Boylam: {firstPoint.lon:F6}°\n" +
                                      $"Zaman: {firstPoint.time:HH:mm:ss}";
            startMarker.ToolTipMode = MarkerTooltipMode.OnMouseOver;
            _markersOverlay.Markers.Add(startMarker);
            
            // Son konum (kırmızı marker)
            var lastPoint = history.Last();
            var endMarker = new GMarkerGoogle(
                new PointLatLng(lastPoint.lat, lastPoint.lon),
                GMarkerGoogleType.red_dot);
            endMarker.ToolTipText = $"📍 SON KONUM\n" +
                                    $"Enlem: {lastPoint.lat:F6}°\n" +
                                    $"Boylam: {lastPoint.lon:F6}°\n" +
                                    $"Zaman: {lastPoint.time:HH:mm:ss}";
            endMarker.ToolTipMode = MarkerTooltipMode.OnMouseOver;
            _markersOverlay.Markers.Add(endMarker);
            
            // Rota çizgisi
            if (history.Count >= 2)
            {
                var routePoints = history.Select(p => new PointLatLng(p.lat, p.lon)).ToList();
                var route = new GMapRoute(routePoints, "GPS Track")
                {
                    Stroke = new Pen(Color.FromArgb(200, 0, 188, 212), 3)
                };
                _routeOverlay.Routes.Add(route);
            }
            
            // Mod'a göre görüntüleme
            switch (_currentMode)
            {
                case MapViewMode.FullView:
                    // Tüm noktaları göster
                    if (history.Count >= 2)
                    {
                        double minLat = history.Min(p => p.lat);
                        double maxLat = history.Max(p => p.lat);
                        double minLon = history.Min(p => p.lon);
                        double maxLon = history.Max(p => p.lon);
                        
                        double latPadding = Math.Max((maxLat - minLat) * 0.15, 0.001);
                        double lonPadding = Math.Max((maxLon - minLon) * 0.15, 0.001);
                        
                        var bounds = new RectLatLng(
                            maxLat + latPadding,
                            minLon - lonPadding,
                            (maxLon - minLon) + 2 * lonPadding,
                            (maxLat - minLat) + 2 * latPadding);
                        
                        _mapControl.SetZoomToFitRect(bounds);
                    }
                    else
                    {
                        _mapControl.Position = new PointLatLng(lastPoint.lat, lastPoint.lon);
                    }
                    break;
                    
                case MapViewMode.Follow:
                    // Son konumu takip et (sabit zoom 15)
                    _mapControl.Position = new PointLatLng(lastPoint.lat, lastPoint.lon);
                    _mapControl.Zoom = 15;
                    break;
                    
                case MapViewMode.Inspect:
                    // Serbest mod - hiçbir şey yapma (kullanıcı kontrol eder)
                    break;
            }
            
            // Mesafe hesapla
            double totalDistance = 0;
            for (int i = 1; i < history.Count; i++)
            {
                totalDistance += CalculateDistance(
                    history[i - 1].lat, history[i - 1].lon,
                    history[i].lat, history[i].lon);
            }
            
            // Bilgi güncelle
            _infoLabel.Text = $"📍 {lastPoint.lat:F5}°, {lastPoint.lon:F5}° | " +
                              $"Nokta: {history.Count} | " +
                              $"Mesafe: {totalDistance:F2} km";
            
            _mapControl.Refresh();
        }
        
        /// Haversine formülü ile iki GPS noktası arasındaki mesafeyi hesaplar
        private double CalculateDistance(double lat1, double lon1, double lat2, double lon2)
        {
            const double R = 6371; // Dünya yarıçapı (km)
            
            double dLat = ToRadians(lat2 - lat1);
            double dLon = ToRadians(lon2 - lon1);
            
            double a = Math.Sin(dLat / 2) * Math.Sin(dLat / 2) +
                       Math.Cos(ToRadians(lat1)) * Math.Cos(ToRadians(lat2)) *
                       Math.Sin(dLon / 2) * Math.Sin(dLon / 2);
            
            double c = 2 * Math.Atan2(Math.Sqrt(a), Math.Sqrt(1 - a));
            
            return R * c;
        }
        
        private static double ToRadians(double degrees) => degrees * Math.PI / 180;
        
        public void Dispose()
        {
            Log.Information("MapPanel kapatılıyor...");
            _telemetryManager.OnDataReceived -= OnNewData;
            _mapControl?.Dispose();
            _container?.Dispose();
        }
    }
}
