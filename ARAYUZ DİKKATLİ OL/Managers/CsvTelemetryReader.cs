using System;
using System.IO;
using System.Threading;

namespace _10OcakRASAT.Managers
{
    /// <summary>
    /// CSV dosyasını sürekli izler ve yeni satırları okur
    /// FileSystemWatcher kullanarak gerçek zamanlı veri akışı sağlar
    /// </summary>
    public class CsvTelemetryReader
    {
        private readonly FileSystemWatcher _watcher;
        private readonly string _csvPath;
        private int _lastReadLine = 2; // Başlık ve birim satırlarını atla
        private readonly object _lockObject = new object();
        private bool _isRunning = false;

        public bool IsRunning => _isRunning;

        /// <summary>
        /// Yeni veri okunduğunda tetiklenir
        /// </summary>
        public event Action<TData>? OnNewDataRead;

        public CsvTelemetryReader(string csvPath)
        {
            _csvPath = csvPath;

            // FileSystemWatcher kurulumu
            string? directory = Path.GetDirectoryName(_csvPath);
            string? fileName = Path.GetFileName(_csvPath);

            if (string.IsNullOrEmpty(directory) || string.IsNullOrEmpty(fileName))
            {
                throw new ArgumentException("Geçersiz CSV dosya yolu");
            }

            _watcher = new FileSystemWatcher(directory)
            {
                Filter = fileName,
                NotifyFilter = NotifyFilters.LastWrite | NotifyFilters.Size,
                EnableRaisingEvents = false
            };

            _watcher.Changed += OnFileChanged;
        }

        /// <summary>
        /// CSV izlemeyi başlatır
        /// </summary>
        public void Start()
        {
            if (_isRunning) return;

            _isRunning = true;
            _watcher.EnableRaisingEvents = true;

            // İlk okuma - dosya zaten varsa mevcut verileri oku
            if (File.Exists(_csvPath))
            {
                ReadNewLines();
            }
        }

        /// <summary>
        /// CSV izlemeyi durdurur
        /// </summary>
        public void Stop()
        {
            _isRunning = false;
            _watcher.EnableRaisingEvents = false;
        }

        /// <summary>
        /// Dosya değiştiğinde tetiklenir
        /// </summary>
        private void OnFileChanged(object sender, FileSystemEventArgs e)
        {
            if (!_isRunning) return;

            // Dosya yazımının bitmesini bekle
            Thread.Sleep(100);

            ReadNewLines();
        }

        /// <summary>
        /// Yeni satırları okur ve parse eder
        /// </summary>
        private void ReadNewLines()
        {
            lock (_lockObject)
            {
                try
                {
                    if (!File.Exists(_csvPath)) return;

                    string[] lines = File.ReadAllLines(_csvPath);

                    // Son okunan satırdan sonraki satırları oku
                    for (int i = _lastReadLine; i < lines.Length; i++)
                    {
                        string line = lines[i].Trim();
                        if (string.IsNullOrWhiteSpace(line)) continue;

                        // TData.Parse ile parse et
                        TData? data = TData.Parse(line);
                        if (data != null)
                        {
                            // Event tetikle
                            OnNewDataRead?.Invoke(data);
                        }
                    }

                    // Son okunan satırı güncelle
                    _lastReadLine = lines.Length;
                }
                catch (IOException)
                {
                    // Dosya başka bir process tarafından kullanılıyorsa (Excel vb.)
                    // Bir sonraki değişiklikte tekrar dene
                }
                catch (Exception)
                {
                    // Diğer hatalar - sessizce devam et
                }
            }
        }

        /// <summary>
        /// Okuma pozisyonunu sıfırlar (test için)
        /// </summary>
        public void Reset()
        {
            lock (_lockObject)
            {
                _lastReadLine = 2; // Başlık ve birim satırlarını atla
            }
        }

        public void Dispose()
        {
            Stop();
            _watcher?.Dispose();
        }
    }
}
