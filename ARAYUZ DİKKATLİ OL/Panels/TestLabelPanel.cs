namespace _10OcakRASAT.Panels
{
    /// Test amaçlı basit  panel
    
    public class TestLabelPanel : IPanelContent
    {
        private readonly string _labelText;
        private readonly Color _backgroundColor;
        private Panel? _panel;

        public string PanelName => "Test Label";
        public string Description => "Sürükle-bırak testi için basit label paneli";

        public TestLabelPanel(string labelText = "Test Panel", Color? backgroundColor = null)
        {
            _labelText = labelText;
            _backgroundColor = backgroundColor ?? Color.FromArgb(60, 60, 60);
        }

        public Control CreateControl()
        {
            _panel = new Panel
            {
                Dock = DockStyle.Fill,
                BackColor = _backgroundColor
            };

            var label = new Label
            {
                Text = _labelText,
                ForeColor = Color.White,
                Font = new Font("Segoe UI", 14, FontStyle.Bold),
                AutoSize = false,
                Dock = DockStyle.Fill,
                TextAlign = ContentAlignment.MiddleCenter
            };

            _panel.Controls.Add(label);
            return _panel;
        }

        public void Dispose()
        {
            _panel?.Dispose();
            _panel = null;
        }
    }
}
