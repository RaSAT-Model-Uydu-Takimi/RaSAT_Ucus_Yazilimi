using System;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Windows.Forms;

namespace _10OcakRASAT.Controls
{
    public class LedStateButton : Control
    {
        // 0: Off, 1: On, 2: Flash
        public int RedState { get; private set; } = 0;
        public int GreenState { get; private set; } = 0;
        public int BlueState { get; private set; } = 0;

        public event EventHandler? OnSubmit;

        private Rectangle _rectRed;
        private Rectangle _rectGreen;
        private Rectangle _rectBlue;
        private System.Windows.Forms.Timer _flashTimer;
        private bool _flashToggle;

        public LedStateButton()
        {
            this.DoubleBuffered = true;
            this.Size = new Size(200, 60);
            this.Cursor = Cursors.Hand;

            _flashTimer = new System.Windows.Forms.Timer { Interval = 500 };
            _flashTimer.Tick += (s, e) => { _flashToggle = !_flashToggle; Invalidate(); };
            _flashTimer.Start();
        }

        protected override void OnPaint(PaintEventArgs e)
        {
            e.Graphics.SmoothingMode = SmoothingMode.AntiAlias;

            // Arkaplan (Button görünümü)
            using (var brush = new SolidBrush(Color.FromArgb(50, 50, 60)))
            {
                e.Graphics.FillRectangle(brush, this.ClientRectangle);
            }
             using (var pen = new Pen(Color.FromArgb(100, 100, 100), 1))
            {
                e.Graphics.DrawRectangle(pen, 0, 0, Width - 1, Height - 1);
            }

            // LED Alanları Hesapla
            int ledSize = 30;
            int gap = 20;
            int startX = (Width - (3 * ledSize + 2 * gap)) / 2;
            int y = (Height - ledSize) / 2;

            _rectRed = new Rectangle(startX, y, ledSize, ledSize);
            _rectGreen = new Rectangle(startX + ledSize + gap, y, ledSize, ledSize);
            _rectBlue = new Rectangle(startX + 2 * (ledSize + gap), y, ledSize, ledSize);

            // LED'leri Çiz
            DrawLed(e.Graphics, _rectRed, Color.Red, RedState);
            DrawLed(e.Graphics, _rectGreen, Color.Lime, GreenState);
            DrawLed(e.Graphics, _rectBlue, Color.Blue, BlueState);
            
            // Metin (İsteğe bağlı, Submit için ipucu)
            // TextRenderer.DrawText(e.Graphics, "GÖNDER", Font, new Point(Width - 50, Height - 20), Color.White);
        }

        private void DrawLed(Graphics g, Rectangle rect, Color baseColor, int state)
        {
            Color drawColor;

            if (state == 0) // Off
            {
                drawColor = ControlPaint.Dark(baseColor, 0.5f); // Sönük
                drawColor = Color.FromArgb(100, drawColor.R, drawColor.G, drawColor.B); // Daha da sönük
            }
            else if (state == 1) // On (Sabit)
            {
                drawColor = baseColor;
            }
            else // Flash
            {
                // Yanıp sönme efekti
                drawColor = _flashToggle ? baseColor : ControlPaint.Dark(baseColor, 0.5f);
            }

            using (var brush = new SolidBrush(drawColor))
            {
                g.FillEllipse(brush, rect);
            }

            // Çerçeve
            using (var pen = new Pen(Color.White, 2))
            {
                if (state == 2) pen.DashStyle = DashStyle.Dot; // Flash modunda kesikli çizgi
                g.DrawEllipse(pen, rect);
            }
            
            // Etiket (R, G, B)
            string label = baseColor == Color.Red ? "R" : baseColor == Color.Lime ? "G" : "B";
            TextRenderer.DrawText(g, label, this.Font, rect, Color.White, TextFormatFlags.HorizontalCenter | TextFormatFlags.VerticalCenter);
        }

        protected override void OnMouseClick(MouseEventArgs e)
        {
            base.OnMouseClick(e);

            if (_rectRed.Contains(e.Location))
            {
                RedState = (RedState + 1) % 3;
                Invalidate();
            }
            else if (_rectGreen.Contains(e.Location))
            {
                GreenState = (GreenState + 1) % 3;
                Invalidate();
            }
            else if (_rectBlue.Contains(e.Location))
            {
                BlueState = (BlueState + 1) % 3;
                Invalidate();
            }
            else
            {
                // Boşluğa tıklandı -> Submit (Message Show)
                OnSubmit?.Invoke(this, EventArgs.Empty);
            }
        }

        public string GetProtocolString()
        {
            // Örnek: "0R1G2B"
            return $"{RedState}R{GreenState}G{BlueState}B";
        }
    }
}
