using System;
using System.Drawing;
using System.Windows.Forms;
using _10OcakRASAT.Controls;
using _10OcakRASAT.Managers;

namespace _10OcakRASAT.Panels
{
    /// <summary>
    /// Kompakt Komut Merkezi Paneli - Kurumsal Tasarım ve ARAS Işıkları
    /// </summary>
    public class CommandPanel : IPanelContent
    {
        public string PanelName => "KOMUTA MERKEZİ";
        public string Description => "Sistem Kontrol ve ARAS Durum";

        // İsimleri buradan değiştirebilirsiniz
        private readonly string[] CommandNames = new string[] 
        {
            "KOMUT 0 (Kalp Atışı)",
            "AYRILMA KOMUTU",
            "ACİL PARAŞÜT",
            "KOMUT 3 (İsimsiz)",
            "IOT KOMUT GÖNDER",
            "KOMUT 5 (İsimsiz)",
            "KOMUT 6 (İsimsiz)",
            "KOMUT 7 (İsimsiz)"
        };

        private Panel? _container;
        private LedToggleButton? _ledRed;
        private LedToggleButton? _ledGreen;
        private LedToggleButton? _ledBlue;
        private TelemetryManager? _telemetryManager;
        
        // ARAS Işıkları
        private Panel? _light1;
        private Panel? _light2;
        private Panel? _light3;
        private Panel? _light4;

        public CommandPanel(TelemetryManager telemetryManager = null)
        {
            _telemetryManager = telemetryManager;
        }

        public Control CreateControl()
        {
            _container = new Panel
            {
                Dock = DockStyle.Fill,
                BackColor = Color.FromArgb(20, 20, 25),
                Padding = new Padding(4),
                AutoScroll = true
            };

            var mainLayout = new TableLayoutPanel
            {
                Dock = DockStyle.Fill,
                ColumnCount = 1,
                RowCount = 3,
                BackColor = Color.Transparent,
                Margin = new Padding(0)
            };

            mainLayout.RowStyles.Add(new RowStyle(SizeType.Percent, 18)); // ARAS (Kalın)
            mainLayout.RowStyles.Add(new RowStyle(SizeType.Percent, 50)); // 6 Buton (3 Satır)
            mainLayout.RowStyles.Add(new RowStyle(SizeType.Percent, 32)); // RGB + IOT

            // === ARAS IŞIKLARI (EN ÜSTTE) ===
            var arasContainer = new TableLayoutPanel
            {
                Dock = DockStyle.Fill,
                ColumnCount = 4,
                RowCount = 2,
                Margin = new Padding(2),
                BackColor = Color.FromArgb(30, 30, 35)
            };
            arasContainer.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 25f));
            arasContainer.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 25f));
            arasContainer.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 25f));
            arasContainer.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 25f));
            arasContainer.RowStyles.Add(new RowStyle(SizeType.Absolute, 15f)); // İnce label
            arasContainer.RowStyles.Add(new RowStyle(SizeType.Percent, 100f)); // Kalın ışıklar

            for (int i = 1; i <= 4; i++)
            {
                arasContainer.Controls.Add(new Label { Text = i.ToString(), ForeColor = Color.Gray, Font = new Font("Segoe UI", 7, FontStyle.Bold), TextAlign = ContentAlignment.BottomCenter, Dock = DockStyle.Fill }, i - 1, 0);
            }

            _light1 = new Panel { BackColor = Color.Lime, Margin = new Padding(2), Dock = DockStyle.Fill };
            _light2 = new Panel { BackColor = Color.Lime, Margin = new Padding(2), Dock = DockStyle.Fill };
            _light3 = new Panel { BackColor = Color.Lime, Margin = new Padding(2), Dock = DockStyle.Fill };
            _light4 = new Panel { BackColor = Color.Lime, Margin = new Padding(2), Dock = DockStyle.Fill };
            
            arasContainer.Controls.Add(_light1, 0, 1);
            arasContainer.Controls.Add(_light2, 1, 1);
            arasContainer.Controls.Add(_light3, 2, 1);
            arasContainer.Controls.Add(_light4, 3, 1);

            mainLayout.Controls.Add(arasContainer, 0, 0);

            // === 6 BUTONLUK IZGARA ===
            var btnGrid = new TableLayoutPanel
            {
                Dock = DockStyle.Fill,
                ColumnCount = 2,
                RowCount = 3,
                Margin = new Padding(0)
            };
            btnGrid.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 50f));
            btnGrid.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 50f));
            btnGrid.RowStyles.Add(new RowStyle(SizeType.Percent, 33.33f));
            btnGrid.RowStyles.Add(new RowStyle(SizeType.Percent, 33.33f));
            btnGrid.RowStyles.Add(new RowStyle(SizeType.Percent, 33.33f));

            Color btnColor = Color.FromArgb(50, 50, 55); // Renksiz, sade gri/koyu ton

            var btnCmd1 = CreateCompactButton(CommandNames[1], btnColor);
            btnCmd1.Click += (s, e) => ShowConfirmation($"{CommandNames[1]} Gönderilsin mi?", CommandNames[1], "AYRILMA"); 
            
            var btnCmd2 = CreateCompactButton(CommandNames[2], btnColor);
            btnCmd2.Click += (s, e) => ShowConfirmation($"{CommandNames[2]} Gönderilsin mi?", CommandNames[2], "PARASUT"); 

            var btnCmd3 = CreateCompactButton(CommandNames[3], btnColor);
            btnCmd3.Click += (s, e) => ShowConfirmation($"{CommandNames[3]} Gönderilsin mi?", CommandNames[3], "CMD3"); 

            var btnCmd5 = CreateCompactButton(CommandNames[5], btnColor);
            btnCmd5.Click += (s, e) => ShowConfirmation($"{CommandNames[5]} Gönderilsin mi?", CommandNames[5], "CMD5"); 

            var btnCmd6 = CreateCompactButton(CommandNames[6], btnColor);
            btnCmd6.Click += (s, e) => ShowConfirmation($"{CommandNames[6]} Gönderilsin mi?", CommandNames[6], "CMD6"); 

            var btnCmd7 = CreateCompactButton(CommandNames[7], btnColor);
            btnCmd7.Click += (s, e) => ShowConfirmation($"{CommandNames[7]} Gönderilsin mi?", CommandNames[7], "CMD7"); 

            btnGrid.Controls.Add(btnCmd1, 0, 0);
            btnGrid.Controls.Add(btnCmd2, 1, 0);
            btnGrid.Controls.Add(btnCmd3, 0, 1);
            btnGrid.Controls.Add(btnCmd5, 1, 1);
            btnGrid.Controls.Add(btnCmd6, 0, 2);
            btnGrid.Controls.Add(btnCmd7, 1, 2);

            mainLayout.Controls.Add(btnGrid, 0, 1);

            // === RGB VE IOT BUTONU (ALT SATIR) ===
            var bottomLayout = new TableLayoutPanel
            {
                Dock = DockStyle.Fill,
                ColumnCount = 2,
                RowCount = 1,
                Margin = new Padding(0)
            };
            bottomLayout.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 55f));
            bottomLayout.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 45f));

            var ledPanel = new TableLayoutPanel
            {
                Dock = DockStyle.Fill,
                ColumnCount = 3,
                RowCount = 1,
                BackColor = Color.FromArgb(35, 35, 40),
                Margin = new Padding(2)
            };
            ledPanel.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 33.33f));
            ledPanel.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 33.33f));
            ledPanel.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 33.33f));

            _ledRed = new LedToggleButton(Color.Red, "R") { Dock = DockStyle.Fill, Margin = new Padding(2) };
            _ledGreen = new LedToggleButton(Color.Lime, "G") { Dock = DockStyle.Fill, Margin = new Padding(2) };
            _ledBlue = new LedToggleButton(Color.DodgerBlue, "B") { Dock = DockStyle.Fill, Margin = new Padding(2) };

            ledPanel.Controls.Add(_ledRed, 0, 0);
            ledPanel.Controls.Add(_ledGreen, 1, 0);
            ledPanel.Controls.Add(_ledBlue, 2, 0);

            var btnSend = CreateCompactButton(CommandNames[4], btnColor); // IOT KOMUT GÖNDER
            btnSend.Click += BtnSend_Click;

            bottomLayout.Controls.Add(ledPanel, 0, 0);
            bottomLayout.Controls.Add(btnSend, 1, 0);

            mainLayout.Controls.Add(bottomLayout, 0, 2);

            _container.Controls.Add(mainLayout);

            if (_telemetryManager != null)
            {
                _telemetryManager.OnDataReceived += OnNewData;
            }

            return _container;
        }

        private void OnNewData(TData data)
        {
            if (_container == null || _container.IsDisposed) return;
            
            if (_container.InvokeRequired)
            {
                _container.Invoke(new Action(() => UpdateLights(data.HataKodu)));
            }
            else
            {
                UpdateLights(data.HataKodu);
            }
        }

        private void UpdateLights(string errorCode)
        {
            if (errorCode != null && errorCode.Length >= 4)
            {
                _light1.BackColor = (errorCode[0] == '1' || errorCode[0] == '\x01') ? Color.Red : Color.Lime;
                _light2.BackColor = (errorCode[1] == '1' || errorCode[1] == '\x01') ? Color.Red : Color.Lime;
                _light3.BackColor = (errorCode[2] == '1' || errorCode[2] == '\x01') ? Color.Red : Color.Lime;
                _light4.BackColor = (errorCode[3] == '1' || errorCode[3] == '\x01') ? Color.Red : Color.Lime;
            }
        }

        private Button CreateCompactButton(string text, Color backColor)
        {
            var btn = new Button
            {
                Text = text,
                Dock = DockStyle.Fill,
                FlatStyle = FlatStyle.Flat,
                BackColor = backColor,
                ForeColor = Color.White,
                Font = new Font("Segoe UI", 9, FontStyle.Bold),
                Margin = new Padding(2),
                Cursor = Cursors.Hand
            };
            btn.FlatAppearance.BorderSize = 0;
            return btn;
        }

        private void ShowConfirmation(string message, string title, string commandCode)
        {
            var result = MessageBox.Show(message, title, MessageBoxButtons.YesNo, MessageBoxIcon.Warning);
            if (result == DialogResult.Yes)
            {
                CommandStateManager.Instance.UpdateCommand(commandCode, $"{title} Komutu Kuyruğa Eklendi");
                MessageBox.Show($"{title} Komutu Gönderildi", "OK", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
        }

        private void BtnSend_Click(object? sender, EventArgs e)
        {
            if (_ledRed == null || _ledGreen == null || _ledBlue == null) return;
            
            string command = $"{_ledRed.State}R{_ledGreen.State}G{_ledBlue.State}B";
            string message = $"KOD: {command} - {DateTime.Now:HH:mm:ss}";
            
            CommandStateManager.Instance.UpdateCommand(command, message);
            MessageBox.Show("IOT Komutu Gönderildi", "OK", MessageBoxButtons.OK, MessageBoxIcon.Information);
        }

        public void Dispose()
        {
            if (_telemetryManager != null)
            {
                _telemetryManager.OnDataReceived -= OnNewData;
            }
            _ledRed?.Dispose();
            _ledGreen?.Dispose();
            _ledBlue?.Dispose();
            _container?.Dispose();
        }
    }
}
