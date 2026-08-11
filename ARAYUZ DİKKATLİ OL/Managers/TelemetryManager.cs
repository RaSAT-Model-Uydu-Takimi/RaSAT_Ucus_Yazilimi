using System.Windows.Forms;
using _10OcakRASAT.Panels;

namespace _10OcakRASAT.Managers
{
    ///  
    /// Telemetri verilerini yöneten sınıf.
    /// Salt okuma - gerçek uygulamada seri port veya dosyadan okur.
    
    public class TelemetryManager
    {
        private readonly Dictionary<string, Label> _labels;
        private readonly List<TData> _dataHistory;
        private readonly object _lockObject = new();

        ///  
        /// Yeni veri geldiğinde tetiklenir (grafik güncellemeleri için)
        
        public event Action<TData>? OnDataReceived;

        ///  
        /// Veri geçmişi (grafikler için)
        
        public IReadOnlyList<TData> DataHistory => _dataHistory;

        public TelemetryManager(Dictionary<string, Label> labels)
        {
            _labels = labels;
            _dataHistory = new List<TData>();
        }

        ///  
        /// Yeni veri işler (herhangi bir kaynaktan gelen)
        
        public void ProcessData(TData data)
        {
            if (data == null) return;

            lock (_lockObject)
            {
                _dataHistory.Add(data);
            }

            UpdateLabels(data);
            OnDataReceived?.Invoke(data);

            // Firebase'e gönder (Arka planda çalışır, UI'ı kilitlemez)
            _ = FirebaseManager.Instance.SendTelemetryData(data);
        }

        ///  
        /// Veri geçmişini temizler
        
        public void ClearHistory()
        {
            lock (_lockObject)
            {
                _dataHistory.Clear();
            }
        }

        ///  
        /// Belirli bir veri tipinin değerlerini döndürür (grafikler için)
        
        public (double[] paketNos, double[] values) GetDataSeries(string dataType)
        {
            lock (_lockObject)
            {
                var paketNos = _dataHistory.Select(d => (double)d.PaketNumarasi).ToArray();
                var values = dataType switch
                {
                    "Basinc" => _dataHistory.Select(d => d.Basinc).ToArray(),
                    "Yukseklik" => _dataHistory.Select(d => d.Yukseklik).ToArray(),
                    "InisHizi" => _dataHistory.Select(d => d.InisHizi).ToArray(),
                    "Sicaklik" => _dataHistory.Select(d => d.Sicaklik).ToArray(),
                    "Pil" => _dataHistory.Select(d => d.PilGerilimi).ToArray(),
                    "GpsLat" => _dataHistory.Select(d => d.GpsLatitude).ToArray(),
                    "GpsLon" => _dataHistory.Select(d => d.GpsLongitude).ToArray(),
                    "GpsAlt" => _dataHistory.Select(d => d.GpsAltitude).ToArray(),
                    "Pitch" => _dataHistory.Select(d => d.Pitch).ToArray(),
                    "Roll" => _dataHistory.Select(d => d.Roll).ToArray(),
                    "Yaw" => _dataHistory.Select(d => d.Yaw).ToArray(),
                    _ => new double[0]
                };
                return (paketNos, values);
            }
        }

        ///  
        /// Label'ları günceller
        
        private void UpdateLabels(TData data)
        {
            if (_labels.Count == 0) return;

            var firstLabel = _labels.Values.FirstOrDefault();
            if (firstLabel == null) return;

            if (firstLabel.InvokeRequired)
            {
                firstLabel.Invoke(new Action(() => UpdateLabelsInternal(data)));
            }
            else
            {
                UpdateLabelsInternal(data);
            }
        }

        private void UpdateLabelsInternal(TData data)
        {
            if (_labels.TryGetValue("Basinc", out var lblBasinc))
                lblBasinc.Text = $"{data.Basinc:F0} Pa";

            if (_labels.TryGetValue("Yukseklik", out var lblYukseklik))
                lblYukseklik.Text = $"{data.Yukseklik:F1} m";

            if (_labels.TryGetValue("InisHizi", out var lblInisHizi))
                lblInisHizi.Text = $"{data.InisHizi:F1} m/s";

            if (_labels.TryGetValue("Sicaklik", out var lblSicaklik))
                lblSicaklik.Text = $"{data.Sicaklik:F1} °C";

            if (_labels.TryGetValue("Pil", out var lblPil))
                lblPil.Text = $"{data.PilGerilimi:F2} V";

            if (_labels.TryGetValue("GpsLat", out var lblGpsLat))
                lblGpsLat.Text = $"{data.GpsLatitude:F4}°";

            if (_labels.TryGetValue("GpsLon", out var lblGpsLon))
                lblGpsLon.Text = $"{data.GpsLongitude:F4}°";

            if (_labels.TryGetValue("Pitch", out var lblPitch))
                lblPitch.Text = $"{data.Pitch:F1}°";

            if (_labels.TryGetValue("Roll", out var lblRoll))
                lblRoll.Text = $"{data.Roll:F1}°";

            if (_labels.TryGetValue("Yaw", out var lblYaw))
                lblYaw.Text = $"{data.Yaw:F1}°";
        }
    }
}
