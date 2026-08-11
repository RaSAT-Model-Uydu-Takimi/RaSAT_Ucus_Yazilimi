using OpenCvSharp;
using OpenCvSharp.Extensions;

namespace _10OcakRASAT.Panels
{
    /// <summary>
    /// OpenCvSharp kullanarak webcam/OBS Virtual Camera video akışı paneli
    /// </summary>
    public class VideoPanel : IPanelContent
    {
        private Panel? _container;
        private PictureBox? _videoDisplay;
        private ComboBox? _cameraSelector;
        private Button? _startButton;
        private Button? _stopButton;
        private Label? _statusLabel;
        private Label? _fpsLabel;
        private Label? _resolutionLabel;
        
        private System.Windows.Forms.Timer? _captureTimer;
        private OpenCvSharp.VideoCapture? _capture;
        private Mat? _frame;
        private bool _isRunning = false;
        private int _frameCount = 0;
        private DateTime _lastFpsUpdate = DateTime.Now;
        
        public string PanelName => "📹 Video";
        public string Description => "Canlı video akışı (OBS Virtual Camera)";
        
        public Control CreateControl()
        {
            _container = new Panel
            {
                Dock = DockStyle.Fill,
                BackColor = System.Drawing.Color.FromArgb(28, 28, 28)
            };
            
            // Üst kontrol paneli
            var topPanel = new Panel
            {
                Dock = DockStyle.Top,
                Height = 50,
                BackColor = System.Drawing.Color.FromArgb(35, 35, 35),
                Padding = new Padding(8, 5, 8, 5)
            };
            
            // Kamera seçici etiketi
            var cameraLabel = new Label
            {
                Text = "📹 Kamera:",
                ForeColor = System.Drawing.Color.White,
                Font = new System.Drawing.Font("Segoe UI", 9, System.Drawing.FontStyle.Bold),
                Location = new System.Drawing.Point(8, 15),
                AutoSize = true
            };
            
            _cameraSelector = new ComboBox
            {
                Location = new System.Drawing.Point(85, 11),
                Width = 180,
                DropDownStyle = ComboBoxStyle.DropDownList,
                BackColor = System.Drawing.Color.FromArgb(50, 50, 50),
                ForeColor = System.Drawing.Color.White,
                FlatStyle = FlatStyle.Flat
            };
            
            // Başlat butonu
            _startButton = new Button
            {
                Text = "▶ Başlat",
                Location = new System.Drawing.Point(275, 8),
                Width = 85,
                Height = 30,
                FlatStyle = FlatStyle.Flat,
                BackColor = System.Drawing.Color.FromArgb(76, 175, 80),
                ForeColor = System.Drawing.Color.White,
                Font = new System.Drawing.Font("Segoe UI", 9, System.Drawing.FontStyle.Bold),
                Cursor = Cursors.Hand
            };
            _startButton.FlatAppearance.BorderSize = 0;
            _startButton.Click += StartButton_Click;
            
            // Durdur butonu
            _stopButton = new Button
            {
                Text = "⏹ Durdur",
                Location = new System.Drawing.Point(365, 8),
                Width = 85,
                Height = 30,
                FlatStyle = FlatStyle.Flat,
                BackColor = System.Drawing.Color.FromArgb(244, 67, 54),
                ForeColor = System.Drawing.Color.White,
                Font = new System.Drawing.Font("Segoe UI", 9, System.Drawing.FontStyle.Bold),
                Enabled = false,
                Cursor = Cursors.Hand
            };
            _stopButton.FlatAppearance.BorderSize = 0;
            _stopButton.Click += StopButton_Click;
            
            // Yenile butonu
            var refreshButton = new Button
            {
                Text = "🔄",
                Location = new System.Drawing.Point(455, 8),
                Width = 35,
                Height = 30,
                FlatStyle = FlatStyle.Flat,
                BackColor = System.Drawing.Color.FromArgb(63, 81, 181),
                ForeColor = System.Drawing.Color.White,
                Font = new System.Drawing.Font("Segoe UI", 11),
                Cursor = Cursors.Hand
            };
            refreshButton.FlatAppearance.BorderSize = 0;
            refreshButton.Click += (s, e) => RefreshCameraList();
            
            // Durum label
            _statusLabel = new Label
            {
                Text = "⚪ Hazır",
                ForeColor = System.Drawing.Color.LightGray,
                Font = new System.Drawing.Font("Segoe UI", 9),
                Location = new System.Drawing.Point(500, 15),
                AutoSize = true
            };
            
            // Çözünürlük label
            _resolutionLabel = new Label
            {
                Text = "",
                ForeColor = System.Drawing.Color.Cyan,
                Font = new System.Drawing.Font("Segoe UI", 8),
                Location = new System.Drawing.Point(600, 15),
                AutoSize = true
            };
            
            // FPS label
            _fpsLabel = new Label
            {
                Text = "",
                ForeColor = System.Drawing.Color.LimeGreen,
                Font = new System.Drawing.Font("Segoe UI", 10, System.Drawing.FontStyle.Bold),
                Dock = DockStyle.Right,
                TextAlign = ContentAlignment.MiddleRight,
                Width = 80
            };
            
            topPanel.Controls.Add(cameraLabel);
            topPanel.Controls.Add(_cameraSelector);
            topPanel.Controls.Add(_startButton);
            topPanel.Controls.Add(_stopButton);
            topPanel.Controls.Add(refreshButton);
            topPanel.Controls.Add(_statusLabel);
            topPanel.Controls.Add(_resolutionLabel);
            topPanel.Controls.Add(_fpsLabel);
            
            // Video görüntü alanı
            _videoDisplay = new PictureBox
            {
                Dock = DockStyle.Fill,
                BackColor = System.Drawing.Color.Black,
                SizeMode = PictureBoxSizeMode.Zoom
            };
            
            // Başlangıç mesajı
            _videoDisplay.Paint += VideoDisplay_Paint;
            
            _container.Controls.Add(_videoDisplay);
            _container.Controls.Add(topPanel);
            
            // Kamera listesini yükle
            RefreshCameraList();
            
            // Capture timer (30 FPS hedefi)
            _captureTimer = new System.Windows.Forms.Timer
            {
                Interval = 33
            };
            _captureTimer.Tick += CaptureTimer_Tick;
            
            // Frame için Mat oluştur
            _frame = new Mat();
            
            return _container;
        }
        
        private void VideoDisplay_Paint(object? sender, PaintEventArgs e)
        {
            if (!_isRunning && _videoDisplay?.Image == null)
            {
                var text = "📹 VİDEO AKIŞI\n\n" +
                           "Video başlatmak için:\n" +
                           "1. Listeden kamera seçin\n" +
                           "2. '▶ Başlat' butonuna tıklayın\n\n" +
                           "━━━━━━━━━━━━━━━━━━━━━━━\n\n";
                
                using var font = new System.Drawing.Font("Segoe UI", 11, System.Drawing.FontStyle.Regular);
                using var brush = new SolidBrush(System.Drawing.Color.FromArgb(150, 150, 150));
                
                if (_videoDisplay == null) return;
                
                var size = e.Graphics.MeasureString(text, font, _videoDisplay.Width - 60);
                var x = (_videoDisplay.Width - size.Width) / 2;
                var y = (_videoDisplay.Height - size.Height) / 2;
                
                e.Graphics.DrawString(text, font, brush, new RectangleF(x, y, size.Width + 10, size.Height));
            }
        }
        
        private void RefreshCameraList()
        {
            if (_cameraSelector == null) return;
            
            _cameraSelector.Items.Clear();
            
            // OpenCV ile kameraları test et (0-9 arası)
            for (int i = 0; i < 10; i++)
            {
                try
                {
                    using var testCapture = new OpenCvSharp.VideoCapture(i);
                    if (testCapture.IsOpened())
                    {
                        // Kamera adını al veya index kullan
                        string cameraName = i switch
                        {
                            0 => "Kamera 0 (Varsayılan)",
                            1 => "Kamera 1 (OBS Virtual Cam?)",
                            2 => "Kamera 2",
                            _ => $"Kamera {i}"
                        };
                        _cameraSelector.Items.Add(new CameraItem(i, cameraName));
                    }
                }
                catch
                {
                    // Bu index'te kamera yok
                }
            }
            
            if (_cameraSelector.Items.Count == 0)
            {
                _cameraSelector.Items.Add(new CameraItem(-1, "Kamera bulunamadı"));
            }
            
            _cameraSelector.SelectedIndex = 0;
            _cameraSelector.DisplayMember = "Name";
        }
        
        private void StartButton_Click(object? sender, EventArgs e)
        {
            if (_cameraSelector == null || _cameraSelector.SelectedItem == null) return;
            
            var selectedCamera = _cameraSelector.SelectedItem as CameraItem;
            if (selectedCamera == null || selectedCamera.Index < 0)
            {
                UpdateStatus("🔴 Kamera seçin!", System.Drawing.Color.Red);
                return;
            }
            
            try
            {
                // Kamerayı aç
                _capture = new OpenCvSharp.VideoCapture(selectedCamera.Index);
                
                if (!_capture.IsOpened())
                {
                    UpdateStatus("🔴 Kamera açılamadı!", System.Drawing.Color.Red);
                    return;
                }
                
                // Yüksek çözünürlük ayarla (1280x720)
                _capture.Set(VideoCaptureProperties.FrameWidth, 1280);
                _capture.Set(VideoCaptureProperties.FrameHeight, 720);
                _capture.Set(VideoCaptureProperties.Fps, 30);
                
                // Gerçek çözünürlüğü al
                int width = (int)_capture.Get(VideoCaptureProperties.FrameWidth);
                int height = (int)_capture.Get(VideoCaptureProperties.FrameHeight);
                
                if (_resolutionLabel != null)
                {
                    _resolutionLabel.Text = $"{width}x{height}";
                }
                
                _isRunning = true;
                _frameCount = 0;
                _lastFpsUpdate = DateTime.Now;
                _captureTimer?.Start();
                
                UpdateStatus("🟢 Yayın Aktif", System.Drawing.Color.LimeGreen);
                
                if (_startButton != null) _startButton.Enabled = false;
                if (_stopButton != null) _stopButton.Enabled = true;
                if (_cameraSelector != null) _cameraSelector.Enabled = false;
            }
            catch (Exception ex)
            {
                UpdateStatus($"🔴 Hata: {ex.Message}", System.Drawing.Color.Red);
            }
        }
        
        private void StopButton_Click(object? sender, EventArgs e)
        {
            StopCapture();
        }
        
        private void StopCapture()
        {
            _isRunning = false;
            _captureTimer?.Stop();
            
            _capture?.Release();
            _capture?.Dispose();
            _capture = null;
            
            if (_videoDisplay != null)
            {
                var oldImage = _videoDisplay.Image;
                _videoDisplay.Image = null;
                oldImage?.Dispose();
            }
            
            UpdateStatus("⚪ Durduruldu", System.Drawing.Color.Gray);
            
            if (_startButton != null) _startButton.Enabled = true;
            if (_stopButton != null) _stopButton.Enabled = false;
            if (_cameraSelector != null) _cameraSelector.Enabled = true;
            if (_fpsLabel != null) _fpsLabel.Text = "";
            if (_resolutionLabel != null) _resolutionLabel.Text = "";
            
            _videoDisplay?.Invalidate();
        }
        
        private void CaptureTimer_Tick(object? sender, EventArgs e)
        {
            if (!_isRunning || _capture == null || _videoDisplay == null || _frame == null) return;
            
            try
            {
                // Frame yakala
                if (_capture.Read(_frame) && !_frame.Empty())
                {
                    // Mat -> Bitmap dönüşümü
                    var bitmap = BitmapConverter.ToBitmap(_frame);
                    
                    // Eski frame'i dispose et
                    var oldImage = _videoDisplay.Image;
                    _videoDisplay.Image = bitmap;
                    oldImage?.Dispose();
                    
                    // FPS hesapla
                    _frameCount++;
                    var elapsed = (DateTime.Now - _lastFpsUpdate).TotalSeconds;
                    if (elapsed >= 1.0)
                    {
                        if (_fpsLabel != null)
                        {
                            int fps = (int)(_frameCount / elapsed);
                            _fpsLabel.Text = $"{fps} FPS";
                            _fpsLabel.ForeColor = fps >= 25 
                                ? System.Drawing.Color.LimeGreen 
                                : fps >= 15 
                                    ? System.Drawing.Color.Orange 
                                    : System.Drawing.Color.Red;
                        }
                        _frameCount = 0;
                        _lastFpsUpdate = DateTime.Now;
                    }
                }
            }
            catch (Exception ex)
            {
                UpdateStatus($"🔴 Frame hatası: {ex.Message}", System.Drawing.Color.Red);
                StopCapture();
            }
        }
        
        private void UpdateStatus(string text, System.Drawing.Color color)
        {
            if (_statusLabel == null) return;
            
            if (_statusLabel.InvokeRequired)
            {
                _statusLabel.Invoke(new Action(() =>
                {
                    _statusLabel.Text = text;
                    _statusLabel.ForeColor = color;
                }));
            }
            else
            {
                _statusLabel.Text = text;
                _statusLabel.ForeColor = color;
            }
        }
        
        public void Dispose()
        {
            StopCapture();
            _frame?.Dispose();
            _captureTimer?.Dispose();
            _videoDisplay?.Dispose();
            _container?.Dispose();
        }
    }
    
    /// Kamera bilgisi için yardımcı sınıf
    internal class CameraItem
    {
        public int Index { get; }
        public string Name { get; }
        
        public CameraItem(int index, string name)
        {
            Index = index;
            Name = name;
        }
        
        public override string ToString() => Name;
    }
}
