using System.IO;

namespace _10OcakRASAT.Managers
{
    // Test amaçlı veri üretici.
    // Gerçek uygulamada kullanılmaz, sadece geliştirme/test için.
    public class TestDataWriter
    {
        private readonly GeneratorTData _generator;
        private readonly System.Windows.Forms.Timer _timer;
        private FileWriter _fileWriter;
        private bool _isRunning;
        private string _currentFilePath;

        // Yeni TData üretildiğinde tetiklenir
        public event Action<TData>? OnDataGenerated;

        public TestDataWriter(string? filePath = null)
        {
            _generator = new GeneratorTData();
            _isRunning = false;

            // Dosya yolunu belirle (varsayılan veya parametre)
            _currentFilePath = filePath ?? Path.Combine(
                Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments),
                "TMUY2026_6334_TLM.csv"
            );
            
            // FileWriter'ı başlat
            _fileWriter = new FileWriter(Path.GetDirectoryName(_currentFilePath) ?? ".", Path.GetFileName(_currentFilePath));

            _timer = new System.Windows.Forms.Timer
            {
                Interval = 1000 // 1 saniye
            };
            _timer.Tick += Timer_Tick;
        }

        /// <summary>
        /// Dosya yolunu günceller
        /// </summary>
        public void UpdateFilePath(string newFilePath)
        {
            _currentFilePath = newFilePath;
            _fileWriter = new FileWriter(Path.GetDirectoryName(newFilePath) ?? ".", Path.GetFileName(newFilePath));
        }

        
        /// Test veri üretimini başlatır
        
        public void Start()
        {
            if (_isRunning) return;
            _isRunning = true;
            _timer.Start();
        }

        
        // Test veri üretimini durdurur
        
        public void Stop()
        {
            _isRunning = false;
            _timer.Stop();
        }

        public bool IsRunning => _isRunning;


        // Tek bir veri üretir (manuel test için)
        
        public TData GenerateSingle()
        {
            return _generator.GenerateBasicTData();
        }

        private void Timer_Tick(object? sender, EventArgs e)
        {
            var data = _generator.GenerateBasicTData();
            
            // CSV'ye yaz (yarışma formatı)
            _fileWriter.WriteData(data);
            
            // Event tetikle (paneller için - eski davranış)
            OnDataGenerated?.Invoke(data);
        }

        // Interval'i değiştirir (ms)
        public void SetInterval(int milliseconds)
        {
            _timer.Interval = milliseconds;
        }
    }
}
