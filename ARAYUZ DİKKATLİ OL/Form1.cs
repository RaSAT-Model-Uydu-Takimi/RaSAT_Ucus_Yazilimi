using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Windows.Forms.Integration;
using _10OcakRASAT.Managers;
using _10OcakRASAT.Panels;
using System.IO;

namespace _10OcakRASAT
{
    public partial class Form1 : Form
    {
        private ElementHost? _elementHostA;
        private UserControl1? _wpfControlA;
        private DragDropPanelManager? _dragDropManager;
        private TelemetryManager? _telemetryManager;
        
        // Yeni STM32 Haberleşme ve Alarm Yöneticileri
        private SerialConnectionManager _serialManager;
        private StmTelemetryParser _telemetryParser;
        private StmCommandSender _commandSender;
        private ArasManager _arasManager;
        
        // Seçilen CSV kayıt dosya yolu
        private string _currentCsvPath = Path.Combine(
            Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments),
            "TMUY2026_6334_TLM.csv"
        );

        public Form1()
        {
            InitializeComponent();
            InitializeTelemetryManager();
            InitializeDragDropManager();
            
            // Seri port altyapısını kur
            _serialManager = new SerialConnectionManager();
            _telemetryParser = new StmTelemetryParser();
            _commandSender = new StmCommandSender(_serialManager);
            _arasManager = new ArasManager();

            // Port listesini doldur
            RefreshComPorts();
            
            // Kullanıcı port kutusuna tıkladığında listeyi otomatik güncelle
            cbPorts.DropDown += (s, e) => RefreshComPorts();
            
            cbBaud.SelectedItem = "115200";
            if (cbBaud.SelectedIndex == -1) cbBaud.SelectedIndex = 0;

            // Event bağlantıları
            _serialManager.OnDataReceived += SerialManager_OnDataReceived;
            _serialManager.OnError += (msg) => MessageBox.Show(msg, "Seri Port Hatası", MessageBoxButtons.OK, MessageBoxIcon.Error);

            _telemetryParser.OnDataParsed += TelemetryParser_OnDataParsed;
            _telemetryParser.OnCrcError += () => { _debugCrcErrors++; };
            
            // UI Button Eventleri (Tasarımcıda olmayanlar)
            btnConnect.Click += BtnConnect_Click;
            
            // Komuta Merkezi komutlarını dinle
            CommandStateManager.Instance.OnCommandChanged += CommandStateManager_OnCommandChanged;

            SetupAlarmSystem();
        }

        // DEBUG sayaçları
        private int _debugRawBytes = 0;
        private int _debugParsedPackets = 0;
        private int _debugCrcErrors = 0;

        // Telemetri yöneticisini başlatır (Read) 
        private void InitializeTelemetryManager()
        {
            var labels = new Dictionary<string, Label>
            {
                { "Basinc", lblBasincValue },
                { "Yukseklik", lblYukseklikValue },
                { "InisHizi", lblInisHiziValue },
                { "Sicaklik", lblSicaklikValue },
                { "Pil", lblPilValue },
                { "GpsLat", lblGpsLatValue },
                { "GpsLon", lblGpsLonValue },
                { "Pitch", lblPitchValue },
                { "Roll", lblRollValue },
                { "Yaw", lblYawValue }
            };

            _telemetryManager = new TelemetryManager(labels);
            PanelFactory.SetTelemetryManager(_telemetryManager);
        }

        // Seri Port'tan ham veri gelince Parser'a besle
        private void SerialManager_OnDataReceived(byte[] data, int length)
        {
            _debugRawBytes += length;
            _telemetryParser.ParseBuffer(data, 0, length);
        }

        // Parser paketi başarılı çözünce UI'ı güncelle ve PONG at
        private void RefreshComPorts()
        {
            string currentSelection = cbPorts.SelectedItem?.ToString() ?? "";
            cbPorts.Items.Clear();
            
            string[] ports = _serialManager.GetAvailablePorts();
            if (ports.Length > 0)
            {
                cbPorts.Items.AddRange(ports);
                if (!string.IsNullOrEmpty(currentSelection) && cbPorts.Items.Contains(currentSelection))
                    cbPorts.SelectedItem = currentSelection;
                else
                    cbPorts.SelectedIndex = 0;
            }
            else
            {
                cbPorts.Items.Add("Bağlantı Yok");
                cbPorts.SelectedIndex = 0;
            }
        }

        private void TelemetryParser_OnDataParsed(TData data)
        {
            // UI Thread'e geç
            if (this.InvokeRequired)
            {
                this.BeginInvoke(new Action(() => TelemetryParser_OnDataParsed(data)));
                return;
            }

            _debugParsedPackets++;

            // Bekleyen komutu veriye ekle ve sıfırla
            data.SentCommand = _pendingCommand;
            
            // Eğer özel bir komut gidiyorsa onun saati yazılır, Heartbeat gidiyorsa anlık pong saati yazılır
            data.SentCommandTime = (_pendingCommand == "HEARTBEAT") ? DateTime.Now.ToString("HH:mm:ss") : _pendingCommandTime;
            
            if (_pendingCommand != "HEARTBEAT")
            {
                _pendingCommand = "HEARTBEAT";
                _pendingCommandTime = "-";
            }

            // Gelen datayı TelemetryManager'a gönder
            _telemetryManager?.ProcessData(data);
            
            // CSV dosyasına kaydet
            try
            {
                // Dosya yoksa başlık (header) ekle
                if (!File.Exists(_currentCsvPath))
                {
                    File.WriteAllText(_currentCsvPath, "paket_numarasi,uydu_statusu,hata_kodu,gonderme_saati,basinc,yukseklik,inis_hizi,sicaklik,pil_gerilimi,gps_latitude,gps_longitude,gps_altitude,pitch,roll,yaw,rhrhrh,takim_no,giden_komut,komut_saati\n");
                }
                // Veriyi CSV formatında ekle
                File.AppendAllText(_currentCsvPath, data.ToCsvLine() + "\n");
            }
            catch (Exception ex)
            {
                // Log the exception quietly or handle it (ignoring for now to avoid freezing the UI with repeated errors)
                Serilog.Log.Error(ex, "Failed to write telemetry data to CSV file");
            }
            
            // ARAS Sistemine yolla (Hata var mı?)
            _arasManager.CheckData(data);

            // PING-PONG mantığı: Gelen her geçerli telemetriye karşılık komut (Pong) fırlat
            _commandSender.SendCurrentCommand();
            
            // DEBUG: Title bar'da durumu göster
            this.Text = $"RASAT GCS | Paket#{_debugParsedPackets} | Ham:{_debugRawBytes}B | CRC Hata:{_debugCrcErrors}";
        }

        private void BtnConnect_Click(object? sender, EventArgs e)
        {
            if (_serialManager.IsOpen)
            {
                _serialManager.Disconnect();
                btnConnect.Text = "BAĞLAN";
                btnConnect.BackColor = Color.FromArgb(0, 100, 150);
                this.Text = "RASAT GCS | Bağlantı kesildi";
            }
            else
            {
                if (cbPorts.SelectedItem == null)
                {
                    MessageBox.Show("Lütfen bir COM portu seçin!", "Uyarı", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                    return;
                }
                
                string port = cbPorts.SelectedItem.ToString() ?? "";
                string baudStr = cbBaud.SelectedItem?.ToString() ?? "115200";
                int baud = int.Parse(baudStr);

                bool result = _serialManager.Connect(port, baud);

                if (result)
                {
                    btnConnect.Text = "KES";
                    btnConnect.BackColor = Color.Red;
                    this.Text = $"RASAT GCS | {port} @ {baud} bağlandı! Veri bekleniyor...";
                }
                else
                {
                    MessageBox.Show($"❌ {port} açılamadı!\nPort başka program tarafından kullanılıyor olabilir.", "Bağlantı Başarısız", MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
            }
        }

        // Komuta Merkezinden Gelen Emirler
        private string _pendingCommand = "HEARTBEAT";
        private string _pendingCommandTime = "-";

        private void CommandStateManager_OnCommandChanged(string command, string message)
        {
            _pendingCommand = command;
            _pendingCommandTime = DateTime.Now.ToString("HH:mm:ss");

            if (command == "AYRILMA")
            {
                _commandSender.SetNextCommand(StmCommandSender.CMD_AYRILMA, new byte[] { 0, 0, 0 });
            }
            else if (command == "PARASUT")
            {
                _commandSender.SetNextCommand(StmCommandSender.CMD_PARASUT, new byte[] { 0, 0, 0 });
            }
            else if (command == "CMD3")
            {
                _commandSender.SetNextCommand(StmCommandSender.CMD_3, new byte[] { 0, 0, 0 });
            }
            else if (command == "CMD5")
            {
                _commandSender.SetNextCommand(StmCommandSender.CMD_5, new byte[] { 0, 0, 0 });
            }
            else if (command == "CMD6")
            {
                _commandSender.SetNextCommand(StmCommandSender.CMD_6, new byte[] { 0, 0, 0 });
            }
            else if (command == "CMD7")
            {
                _commandSender.SetNextCommand(StmCommandSender.CMD_7, new byte[] { 0, 0, 0 });
            }
            else
            {
                // IOT veya başka bir komut (Örn: "1R0G2B")
                byte r = 0, g = 0, b = 0;
                
                try 
                {
                    if (command.Contains("R") && command.Contains("G") && command.Contains("B"))
                    {
                        int rIndex = command.IndexOf('R');
                        int gIndex = command.IndexOf('G');
                        int bIndex = command.IndexOf('B');
                        
                        r = byte.Parse(command.Substring(rIndex - 1, 1));
                        g = byte.Parse(command.Substring(gIndex - 1, 1));
                        b = byte.Parse(command.Substring(bIndex - 1, 1));
                    }
                }
                catch { }

                _commandSender.SetNextCommand(StmCommandSender.CMD_IOT, new byte[] { r, g, b }); 
            }

            // Ayrıca komutu doğrudan ikinci CSV'ye yaz (Komut Log Dosyası)
            try
            {
                if (!string.IsNullOrEmpty(_currentCsvPath))
                {
                    string dir = Path.GetDirectoryName(_currentCsvPath) ?? "";
                    string name = Path.GetFileNameWithoutExtension(_currentCsvPath);
                    string cmdLogPath = Path.Combine(dir, $"{name}_commands.csv");

                    if (!File.Exists(cmdLogPath))
                    {
                        File.WriteAllText(cmdLogPath, "saat,komut,mesaj\n");
                    }
                    File.AppendAllText(cmdLogPath, $"{_pendingCommandTime},{command},{message}\n");
                }
            }
            catch (Exception ex)
            {
                Serilog.Log.Error(ex, "Komut log dosyasına yazılamadı.");
            }
        }

        // --- ARAS SİSTEMİ EKRAN TİTREMELERİ ---
        private System.Windows.Forms.Timer? _shakeTimer;
        private int _shakeCount = 0;
        private Color _originalColor;
        private bool _isAlarmActive = false;
        private System.Media.SoundPlayer? _alarmSound;

        private void SetupAlarmSystem()
        {
            _shakeTimer = new System.Windows.Forms.Timer { Interval = 150 }; // Flaş hızı (150ms)
            _shakeTimer.Tick += ShakeTimer_Tick;

            _arasManager.OnCriticalErrorDetected += Aras_OnCriticalErrorDetected;
            _arasManager.OnAlarmCleared += Aras_OnAlarmCleared;
            
            try {
                _alarmSound = new System.Media.SoundPlayer("Tehlike Alarm Sesi.wav");
                _alarmSound.LoadAsync();
            } catch { }
        }

        private void Aras_OnCriticalErrorDetected(object? sender, ArasManager.AlarmEventArgs e)
        {
            if (this.InvokeRequired)
            {
                this.BeginInvoke(new Action(() => Aras_OnCriticalErrorDetected(sender, e)));
                return;
            }

            if (!_isAlarmActive)
            {
                _isAlarmActive = true;
                _originalColor = this.BackColor; // Orijinal rengi kaydet
                _shakeCount = 0;
                _shakeTimer.Start();

                // Sesli Uyarıyı sürekli döngüye al
                if (_alarmSound != null) {
                    _alarmSound.PlayLooping();
                } else {
                    System.Media.SystemSounds.Exclamation.Play();
                }

                // Asenkron Popup (Telemetriyi dondurmamak için)
                Task.Run(() => 
                {
                    MessageBox.Show(
                        $"Kritik bir hata tespit edildi!\n\nDetay: {e.Message}", 
                        "⚠️ SİSTEM ALARMI", 
                        MessageBoxButtons.OK, 
                        MessageBoxIcon.Error,
                        MessageBoxDefaultButton.Button1,
                        MessageBoxOptions.DefaultDesktopOnly
                    );
                });
            }
        }

        private void Aras_OnAlarmCleared(object? sender, EventArgs e)
        {
            if (this.InvokeRequired)
            {
                this.BeginInvoke(new Action(() => Aras_OnAlarmCleared(sender, e)));
                return;
            }

            if (_isAlarmActive)
            {
                _isAlarmActive = false;
                _shakeTimer.Stop();
                this.BackColor = _originalColor;
                _alarmSound?.Stop();
            }
        }

        private void ShakeTimer_Tick(object? sender, EventArgs e)
        {
            if (!_isAlarmActive) return;
            
            _shakeCount++;
            
            // Kırmızı - Normal renk değişimi
            this.BackColor = (_shakeCount % 2 == 0) ? _originalColor : Color.DarkRed;
            
            // Alarm limitini kaldırdık, hata geldiği sürece devam edecek.
        }
        
        private void InitializeDragDropManager()
        {
            var targetContainers = new List<GroupBox>
            {
                gbMainScreen, gbCellA, gbCellB, gbCellC, gbCellD, gbCellE, gbCellF, gbCellG, gbCellH
            };
            _dragDropManager = new DragDropPanelManager(panelListBox, targetContainers);
        }

        private async void BtnAction2_Click(object? sender, EventArgs e)
        {
            var result = MessageBox.Show(
                "⚠️ Tüm veriler silinecek!\n\n" +
                $"📂 Kayıt Dosyası: {Path.GetFileName(_currentCsvPath)}\n" +
                "Tüm kayıtlar silinecek. Emin misiniz?", "Verileri Sil", MessageBoxButtons.YesNo, MessageBoxIcon.Warning);

            if (result == DialogResult.Yes)
            {
                try
                {
                    if (File.Exists(_currentCsvPath)) File.Delete(_currentCsvPath);
                    await FirebaseManager.Instance.DeleteTelemetryDataAsync();
                    _telemetryManager?.ClearHistory();
                    MessageBox.Show("✅ Silme işlemi tamamlandı!", "Başarılı", MessageBoxButtons.OK, MessageBoxIcon.Information);
                }
                catch (Exception ex)
                {
                    MessageBox.Show($"❌ Silme işlemi başarısız!\n\n{ex.Message}", "Hata", MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
            }
        }

        private void BtnFilePath_Click(object? sender, EventArgs e)
        {
            using (var saveFileDialog = new SaveFileDialog())
            {
                saveFileDialog.Title = "CSV Kayıt Yerini Seç";
                saveFileDialog.Filter = "CSV Dosyaları (*.csv)|*.csv|Tüm Dosyalar (*.*)|*.*";
                saveFileDialog.InitialDirectory = Path.GetDirectoryName(_currentCsvPath) ?? Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments);
                saveFileDialog.FileName = Path.GetFileName(_currentCsvPath);
                
                if (saveFileDialog.ShowDialog() == DialogResult.OK)
                {
                    _currentCsvPath = saveFileDialog.FileName;
                    MessageBox.Show(
                        "✅ Kayıt yeri güncellendi!\n\n" +
                        $"📂 Yeni Dosya: {Path.GetFileName(_currentCsvPath)}\n" +
                        $"📍 Konum: {Path.GetDirectoryName(_currentCsvPath)}\n\n" +
                        "💡 Porttan gelen veriler artık bu dosyaya eklenecektir.",
                        "Kayıt Yeri", MessageBoxButtons.OK, MessageBoxIcon.Information);
                }
            }
        }

        protected override void OnFormClosing(FormClosingEventArgs e)
        {
            _serialManager?.Dispose();
            base.OnFormClosing(e);
        }
    }
}
