using ScottPlot;
using ScottPlot.WinForms;
using _10OcakRASAT.Managers;

namespace _10OcakRASAT.Panels
{
    
    /// Grafik görüntüleme modları
     
    public enum ChartViewMode
    {
        /// Tüm veri görünür, veri geldikçe X ekseni daralır
        FullView = 0,

        /// Son 10 değer takibi
         
        Follow = 1,
        /// Mouse ile serbest gezinme
         
        Inspect = 2
    }

    /// Telemetri verisi için grafik paneli.
    /// 3 görüntüleme modu: FullView, Follow, Inspect
     
    public class ChartPanel : IPanelContent
    {
        private readonly string _dataType;
        private readonly string _title;
        private readonly string _unit;
        private readonly System.Drawing.Color _lineColor;
        private readonly TelemetryManager _telemetryManager;

        private Panel? _container;
        private FormsPlot? _plot;
        private TrackBar? _modeSlider;
        private System.Windows.Forms.Label? _modeLabel;
        private ChartViewMode _currentMode = ChartViewMode.FullView;

        private double[] _xData = Array.Empty<double>();
        private double[] _yData = Array.Empty<double>();

        public string PanelName => $"📊 {_title}";
        public string Description => $"{_title} grafiği ({_unit})";

        public ChartPanel(string dataType, string title, string unit, System.Drawing.Color lineColor, TelemetryManager telemetryManager)
        {
            _dataType = dataType;
            _title = title;
            _unit = unit;
            _lineColor = lineColor;
            _telemetryManager = telemetryManager;
        }

        public Control CreateControl()
        {
            _container = new Panel
            {
                Dock = DockStyle.Fill,
                BackColor = System.Drawing.Color.FromArgb(28, 28, 28)
            };

            // Mod slider ve label için üst panel
            var topPanel = new Panel
            {
                Dock = DockStyle.Top,
                Height = 35,
                BackColor = System.Drawing.Color.FromArgb(35, 35, 35)
            };

            _modeLabel = new System.Windows.Forms.Label
            {
                Text = "📊 Full View",
                ForeColor = System.Drawing.Color.White,
                Font = new System.Drawing.Font("Segoe UI", 9, System.Drawing.FontStyle.Bold),
                Location = new Point(8, 8),
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
                Location = new Point(120, 5),
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

            // Grafik stilini ayarla
            SetupPlotStyle();

            _container.Controls.Add(_plot);
            _container.Controls.Add(topPanel);

            // Telemetry event'e abone ol
            _telemetryManager.OnDataReceived += OnNewData;

            // İlk verileri yükle
            RefreshData();

            return _container;
        }

        private void SetupPlotStyle()
        {
            if (_plot == null) return;

            // Koyu tema
            _plot.Plot.FigureBackground.Color = ScottPlot.Color.FromHex("#1C1C1C");
            _plot.Plot.DataBackground.Color = ScottPlot.Color.FromHex("#1C1C1C");

            // Eksen stilleri
            _plot.Plot.Axes.Bottom.Label.Text = "Paket No";
            _plot.Plot.Axes.Left.Label.Text = $"{_title} ({_unit})";

            _plot.Plot.Axes.Bottom.Label.ForeColor = ScottPlot.Colors.White;
            _plot.Plot.Axes.Left.Label.ForeColor = ScottPlot.Colors.White;

            _plot.Plot.Axes.Bottom.TickLabelStyle.ForeColor = ScottPlot.Colors.LightGray;
            _plot.Plot.Axes.Left.TickLabelStyle.ForeColor = ScottPlot.Colors.LightGray;

            // Grid
            _plot.Plot.Grid.MajorLineColor = ScottPlot.Color.FromHex("#333333");
        }

        private void ModeSlider_ValueChanged(object? sender, EventArgs e)
        {
            if (_modeSlider == null || _modeLabel == null || _plot == null) return;

            _currentMode = (ChartViewMode)_modeSlider.Value;

            // Mod label'ını güncelle
            _modeLabel.Text = _currentMode switch
            {
                ChartViewMode.FullView => "📊 Full View",
                ChartViewMode.Follow => "🎯 Follow (10)",
                ChartViewMode.Inspect => "🔍 Inspect",
                _ => "📊 Full View"
            };

            // Grafik etkileşimini ayarla
            // ScottPlot 5'te mouse etkileşimi varsayılan olarak açık
            // Inspect modunda kullanıcı serbest hareket edebilir
            // Diğer modlarda her veri güncellemesinde axis ayarlanır

            UpdatePlot();
        }

        private void OnNewData(TData data)
        {
            RefreshData();
        }

        private void RefreshData()
        {
            var (paketNos, values) = _telemetryManager.GetDataSeries(_dataType);
            _xData = paketNos;
            _yData = values;

            UpdatePlot();
        }

        private void UpdatePlot()
        {
            if (_plot == null || _xData.Length == 0) return;

            // Thread-safe UI güncelleme
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

            if (_xData.Length == 0 || _yData.Length == 0) return;

            // Scatter plot ekle
            var scatter = _plot.Plot.Add.Scatter(_xData, _yData);
            scatter.Color = ScottPlot.Color.FromColor(_lineColor);
            scatter.LineWidth = 2;
            scatter.MarkerSize = 5;

            // Mod'a göre axis ayarla
            switch (_currentMode)
            {
                case ChartViewMode.FullView:
                    _plot.Plot.Axes.AutoScale();
                    break;

                case ChartViewMode.Follow:
                    // Son 10 değeri göster
                    if (_xData.Length > 0)
                    {
                        int count = Math.Min(10, _xData.Length);
                        double minX = _xData.Length > 10 ? _xData[^10] : _xData[0];
                        double maxX = _xData[^1] + 1;

                        var lastValues = _yData.TakeLast(count).ToArray();
                        double minY = lastValues.Length > 0 ? lastValues.Min() - 5 : 0;
                        double maxY = lastValues.Length > 0 ? lastValues.Max() + 5 : 100;

                        _plot.Plot.Axes.SetLimits(minX, maxX, minY, maxY);
                    }
                    break;

                case ChartViewMode.Inspect:
                    // Kullanıcı serbest hareket edebilir, axis değiştirme
                    break;
            }

            _plot.Refresh();
        }

        public void Dispose()
        {
            // Event'ten çık
            _telemetryManager.OnDataReceived -= OnNewData;

            _plot?.Dispose();
            _container?.Dispose();
        }
    }
}
