using ScottPlot;
using ScottPlot.WinForms;
using _10OcakRASAT.Managers;
using Serilog;

namespace _10OcakRASAT.Panels
{  
    /// GPS konum izleme modları
    
    public enum GpsTrackMode
    {  
        /// Tüm konumlar görünür
        FullTrack = 0,

        /// Son 10 saniye (eski veriler kaldırılır) 
        Follow = 1,

        /// Hepsi görünür, serbest gezinme    
        Inspect = 2
    }
 
    /// GPS enlem/boylam izleme paneli
    /// X: Longitude, Y: Latitude
    
    public class GpsTrackPanel : IPanelContent
    {
        private readonly TelemetryManager _telemetryManager;

        private System.Windows.Forms.Panel? _container;
        private FormsPlot? _plot;
        private TrackBar? _modeSlider;
        private System.Windows.Forms.Label? _modeLabel;
        private GpsTrackMode _currentMode = GpsTrackMode.FullTrack;

        private List<(double lon, double lat, DateTime time)> _trackHistory = new();
        private readonly object _lockObject = new();

        public string PanelName => "🗺️ GPS İz";
        public string Description => "GPS konum izleme (Lat/Lon)";

        public GpsTrackPanel(TelemetryManager telemetryManager)
        {
            _telemetryManager = telemetryManager;
        }

        public Control CreateControl()
        {
            _container = new System.Windows.Forms.Panel
            {
                Dock = DockStyle.Fill,
                BackColor = System.Drawing.Color.FromArgb(28, 28, 28)
            };

            // Üst panel - mod slider
            var topPanel = new System.Windows.Forms.Panel
            {
                Dock = DockStyle.Top,
                Height = 35,
                BackColor = System.Drawing.Color.FromArgb(35, 35, 35)
            };

            _modeLabel = new System.Windows.Forms.Label
            {
                Text = "🗺️ Full Track",
                ForeColor = System.Drawing.Color.White,
                Font = new System.Drawing.Font("Segoe UI", 9, System.Drawing.FontStyle.Bold),
                Location = new System.Drawing.Point(8, 8),
                AutoSize = true
            };

            _modeSlider = new TrackBar
            {
                Minimum = 0,
                Maximum = 2,
                Value = 0,
                TickStyle = TickStyle.None,
                Width = 100,
                Height = 25,
                Location = new System.Drawing.Point(120, 5),
                BackColor = System.Drawing.Color.FromArgb(35, 35, 35)
            };
            _modeSlider.ValueChanged += ModeSlider_ValueChanged;

            topPanel.Controls.Add(_modeLabel);
            topPanel.Controls.Add(_modeSlider);

            // ScottPlot grafiği
            _plot = new FormsPlot
            {
                Dock = DockStyle.Fill
            };

            SetupPlotStyle();

            _container.Controls.Add(_plot);
            _container.Controls.Add(topPanel);

            LoadHistory();

            // Event'e abone ol
            _telemetryManager.OnDataReceived += OnNewData;

            return _container;
        }

        private void LoadHistory()
        {
            var history = _telemetryManager.DataHistory;
            if (history.Count == 0) return;

            Log.Information($"GpsTrackPanel: {history.Count} geçmiş veri kontrol ediliyor.");

            lock (_lockObject)
            {
                foreach (var data in history)
                {
                    if (data.GpsLatitude != 0 && data.GpsLongitude != 0)
                    {
                        _trackHistory.Add((data.GpsLongitude, data.GpsLatitude, DateTime.Now)); // Or data.GondermeSaati parsed if needed
                    }
                }
            }

            if (_trackHistory.Count > 0)
            {
                Log.Information($"GpsTrackPanel: {_trackHistory.Count} geçerli GPS noktası yüklendi.");
                UpdatePlot();
            }
        }

        private void SetupPlotStyle()
        {
            if (_plot == null) return;

            // Koyu tema
            _plot.Plot.FigureBackground.Color = ScottPlot.Color.FromHex("#1C1C1C");
            _plot.Plot.DataBackground.Color = ScottPlot.Color.FromHex("#1C1C1C");

            // Eksen etiketleri
            _plot.Plot.Axes.Bottom.Label.Text = "Longitude (°)";
            _plot.Plot.Axes.Left.Label.Text = "Latitude (°)";

            _plot.Plot.Axes.Bottom.Label.ForeColor = ScottPlot.Colors.White;
            _plot.Plot.Axes.Left.Label.ForeColor = ScottPlot.Colors.White;

            _plot.Plot.Axes.Bottom.TickLabelStyle.ForeColor = ScottPlot.Colors.LightGray;
            _plot.Plot.Axes.Left.TickLabelStyle.ForeColor = ScottPlot.Colors.LightGray;

            // Grid
            _plot.Plot.Grid.MajorLineColor = ScottPlot.Color.FromHex("#333333");
        }

        private void ModeSlider_ValueChanged(object? sender, EventArgs e)
        {
            if (_modeSlider == null || _modeLabel == null) return;

            _currentMode = (GpsTrackMode)_modeSlider.Value;

            _modeLabel.Text = _currentMode switch
            {
                GpsTrackMode.FullTrack => "🗺️ Full Track",
                GpsTrackMode.Follow => "🎯 Follow (10s)",
                GpsTrackMode.Inspect => "🔍 Inspect",
                _ => "🗺️ Full Track"
            };

            UpdatePlot();
        }

        private void OnNewData(TData data)
        {
            lock (_lockObject)
            {
                _trackHistory.Add((data.GpsLongitude, data.GpsLatitude, DateTime.Now));
            }

            UpdatePlot();
        }

        private void UpdatePlot()
        {
            if (_plot == null) return;

            if (_plot.InvokeRequired)
            {
                _plot.Invoke(new Action(UpdatePlotInternal));
            }
            else
            {
                UpdatePlotInternal();
            }
        }

        private void UpdatePlotInternal()
        {
            if (_plot == null) return;

            _plot.Plot.Clear();

            List<(double lon, double lat, DateTime time)> dataToShow;

            lock (_lockObject)
            {
                if (_trackHistory.Count == 0) return;

                dataToShow = _currentMode switch
                {
                    GpsTrackMode.Follow => _trackHistory
                        .Where(p => (DateTime.Now - p.time).TotalSeconds <= 10)
                        .ToList(),
                    _ => _trackHistory.ToList()
                };
            }

            if (dataToShow.Count == 0) return;

            var lons = dataToShow.Select(p => p.lon).ToArray();
            var lats = dataToShow.Select(p => p.lat).ToArray();

            // İz çizgisi
            var scatter = _plot.Plot.Add.Scatter(lons, lats);
            scatter.Color = ScottPlot.Color.FromHex("#00BCD4");
            scatter.LineWidth = 2;
            scatter.MarkerSize = 4;

            // Son konum büyük marker
            if (lons.Length > 0)
            {
                var lastMarker = _plot.Plot.Add.Scatter(
                    new[] { lons[^1] }, 
                    new[] { lats[^1] });
                lastMarker.Color = ScottPlot.Color.FromHex("#FF5722");
                lastMarker.MarkerSize = 12;
                lastMarker.LineWidth = 0;
            }

            // Axis ayarla
            if (_currentMode != GpsTrackMode.Inspect)
            {
                _plot.Plot.Axes.AutoScale();
            }

            _plot.Refresh();
        }

        public void Dispose()
        {
            Log.Information("GpsTrackPanel kapatılıyor...");
            _telemetryManager.OnDataReceived -= OnNewData;
            _plot?.Dispose();
            _container?.Dispose();
        }
    }
}
