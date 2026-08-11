using _10OcakRASAT.Managers;
using System.Drawing;
using Serilog;

namespace _10OcakRASAT.Panels
{
    /// <summary>
    /// Konsol benzeri TData canlı akış paneli - legend ile
    /// </summary>
    public class TDataLivePanel : IPanelContent
    {
        private readonly TelemetryManager _telemetryManager;
        private Panel? _container;
        private ListBox? _listBox;
        private Label? _legendLabel;
        private Label? _commandMessageLabel;
        private const int MaxLines = 1200; // 20 dakikalık (1 Hz) veri sınırı

        // Tablo hizalaması için standart format template'i (Sağa dayalı)
        private const string FormatTemplate = "{0,5}|{1,5}|{2,5}|{3,19}|{4,7}|{5,7}|{6,6}|{7,5}|{8,5}|{9,9}|{10,9}|{11,6}|{12,6}|{13,6}|{14,6}|{15,6}|{16,5}|{17,12}|{18,10}";
        
        // Başlıklar da veriyle aynı genişlikte formatlanıyor
        private static readonly string LegendText = string.Format(FormatTemplate, "PKT", "STS", "ERR", "SAAT", "BAR", "YÜK", "HIZ", "SIC", "PİL", "LAT", "LON", "ALT", "PIT", "ROL", "YAW", "RH", "TKM", "GİDEN KOMUT", "KOMUT SAAT");

        public string PanelName => "📡 TData Canlı";
        public string Description => "Konsol benzeri telemetri akışı";

        public TDataLivePanel(TelemetryManager telemetryManager)
        {
            _telemetryManager = telemetryManager;
        }

        public Control CreateControl()
        {
            Log.Information("TDataLivePanel oluşturuluyor...");

            _container = new Panel
            {
                Dock = DockStyle.Fill,
                BackColor = Color.FromArgb(20, 20, 20)
            };

            // Komut mesajı label'ı (en üstte)
            _commandMessageLabel = new Label
            {
                Dock = DockStyle.Top,
                Height = 25,
                BackColor = Color.FromArgb(50, 50, 60),
                ForeColor = Color.FromArgb(255, 200, 100),
                Font = new Font("Segoe UI", 9f, FontStyle.Bold),
                Text = "📡 Son Komut: 0R0G0B",
                TextAlign = ContentAlignment.MiddleLeft,
                Padding = new Padding(5, 0, 0, 0)
            };

            // Legend (en altta)
            _legendLabel = new Label
            {
                Dock = DockStyle.Bottom,
                Height = 18,
                BackColor = Color.FromArgb(40, 40, 50),
                ForeColor = Color.FromArgb(150, 200, 255),
                Font = new Font("Consolas", 6.5f, FontStyle.Bold),
                Text = LegendText,
                TextAlign = ContentAlignment.MiddleLeft,
                Padding = new Padding(2, 0, 0, 0)
            };

            _listBox = new ListBox
            {
                Dock = DockStyle.Fill,
                BackColor = Color.FromArgb(28, 28, 28),
                ForeColor = Color.FromArgb(0, 255, 100),
                Font = new Font("Consolas", 6.5f), // Daha küçük font
                BorderStyle = BorderStyle.None,
                IntegralHeight = false,
                HorizontalScrollbar = true // Yatay scroll
            };

            _container.Controls.Add(_listBox);
            _container.Controls.Add(_legendLabel);
            _container.Controls.Add(_commandMessageLabel);
            
            LoadHistory();

            _telemetryManager.OnDataReceived += OnNewData;

            // CommandStateManager'ı dinle
            _10OcakRASAT.Managers.CommandStateManager.Instance.OnCommandChanged += UpdateCommandMessage;

            return _container;
        }

        private void LoadHistory()
        {
            if (_listBox == null) return;
            
            var history = _telemetryManager.DataHistory;
            if (history.Count == 0) return;

            Log.Information($"TDataLivePanel: {history.Count} geçmiş veri yükleniyor.");
            
            _listBox.BeginUpdate(); // UI kilitlenmesini önlemek için
            
            // Eğer geçmiş veri MaxLines'dan büyükse sadece son kısmını al
            var dataToLoad = history.Count > MaxLines ? history.TakeLast(MaxLines) : history;
            
            foreach (var data in dataToLoad)
            {
                string line = FormatLine(data);
                _listBox.Items.Add(line);
            }
            
            if (_listBox.Items.Count > 0)
            {
                _listBox.TopIndex = _listBox.Items.Count - 1;
            }
            
            _listBox.EndUpdate();
        }

        private string FormatLine(TData data)
        {
            return string.Format(FormatTemplate, 
                data.PaketNumarasi, 
                data.UyduStatusu, 
                data.HataKodu, 
                data.GondermeSaati, 
                data.Basinc.ToString("F0"), 
                data.Yukseklik.ToString("F0"), 
                data.InisHizi.ToString("F1"), 
                data.Sicaklik.ToString("F0"), 
                data.PilGerilimi.ToString("F1"), 
                data.GpsLatitude.ToString("F4"), 
                data.GpsLongitude.ToString("F4"), 
                data.GpsAltitude.ToString("F0"), 
                data.Pitch.ToString("F1"), 
                data.Roll.ToString("F1"), 
                data.Yaw.ToString("F1"), 
                data.RHRHRH, 
                data.TakimNo,
                data.SentCommand,
                data.SentCommandTime);
        }

        private void OnNewData(TData data)
        {
            if (_listBox == null || _listBox.IsDisposed) return;

            string line = FormatLine(data);

            if (_listBox.InvokeRequired)
            {
                _listBox.Invoke(new Action(() => AddLine(line)));
            }
            else
            {
                AddLine(line);
            }
        }

        private void AddLine(string line)
        {
            if (_listBox == null) return;

            _listBox.Items.Add(line);

            // Sınırı aşarsa baştan silmeye başla
            // UI donmasını engellemek için toplu silme işlemi de yapılabilir ancak akışta genelde teker teker gelir.
            while (_listBox.Items.Count > MaxLines)
            {
                _listBox.Items.RemoveAt(0);
            }

            _listBox.TopIndex = _listBox.Items.Count - 1;
        }

        private void UpdateCommandMessage(string command, string message)
        {
            if (_commandMessageLabel == null) return;

            if (_commandMessageLabel.InvokeRequired)
            {
                _commandMessageLabel.Invoke(() => _commandMessageLabel.Text = $"📡 {message}");
            }
            else
            {
                _commandMessageLabel.Text = $"📡 {message}";
            }
        }

        public void Dispose()
        {
            Log.Information("TDataLivePanel kapatılıyor...");
            _telemetryManager.OnDataReceived -= OnNewData;
            _10OcakRASAT.Managers.CommandStateManager.Instance.OnCommandChanged -= UpdateCommandMessage;
            _listBox?.Dispose();
            _legendLabel?.Dispose();
            _commandMessageLabel?.Dispose();
            _container?.Dispose();
        }
    }
}
