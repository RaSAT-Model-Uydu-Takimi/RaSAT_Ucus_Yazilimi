using System;
using System.ComponentModel;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Windows.Forms;

namespace _10OcakRASAT.Controls
{
    /// <summary>
    /// Kompakt LED toggle butonu - tıklayınca mod değişir
    /// </summary>
    public class LedToggleButton : Control
    {
        private int _state = 0;
        private readonly Color _ledColor;
        private readonly string _label;
        private System.Windows.Forms.Timer _flashTimer;
        private bool _flashVisible = true;

        [DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        [Browsable(false)]
        public int State
        {
            get => _state;
            set { _state = value % 3; Invalidate(); }
        }

        public event EventHandler? StateChanged;

        public LedToggleButton(Color ledColor, string label)
        {
            _ledColor = ledColor;
            _label = label;
            
            this.DoubleBuffered = true;
            this.MinimumSize = new Size(30, 40);
            this.Cursor = Cursors.Hand;

            _flashTimer = new System.Windows.Forms.Timer { Interval = 400 };
            _flashTimer.Tick += (s, e) => { _flashVisible = !_flashVisible; if (_state == 2) Invalidate(); };
            _flashTimer.Start();
        }

        protected override void OnPaint(PaintEventArgs e)
        {
            var g = e.Graphics;
            g.SmoothingMode = SmoothingMode.AntiAlias;

            // Arkaplan
            using (var bgBrush = new SolidBrush(Color.FromArgb(40, 40, 45)))
            {
                g.FillRectangle(bgBrush, ClientRectangle);
            }

            // Çerçeve
            using (var borderPen = new Pen(Color.FromArgb(60, 60, 70), 1))
            {
                g.DrawRectangle(borderPen, 0, 0, Width - 1, Height - 1);
            }

            // LED boyutu - panele göre dinamik
            int ledDiameter = Math.Min(Width - 10, Height - 25);
            ledDiameter = Math.Max(ledDiameter, 15); // minimum 15px
            int ledX = (Width - ledDiameter) / 2;
            int ledY = 4;
            Rectangle ledRect = new Rectangle(ledX, ledY, ledDiameter, ledDiameter);

            Color currentColor;
            if (_state == 0)
            {
                currentColor = Color.FromArgb(60, _ledColor.R / 5, _ledColor.G / 5, _ledColor.B / 5);
            }
            else if (_state == 1)
            {
                currentColor = _ledColor;
            }
            else
            {
                currentColor = _flashVisible ? _ledColor : Color.FromArgb(60, _ledColor.R / 5, _ledColor.G / 5, _ledColor.B / 5);
            }

            // Glow efekti
            if (_state == 1 || (_state == 2 && _flashVisible))
            {
                using (var glowBrush = new SolidBrush(Color.FromArgb(30, _ledColor)))
                {
                    g.FillEllipse(glowBrush, ledX - 3, ledY - 3, ledDiameter + 6, ledDiameter + 6);
                }
            }

            // LED
            using (var ledBrush = new SolidBrush(currentColor))
            {
                g.FillEllipse(ledBrush, ledRect);
            }

            // LED çerçeve
            using (var ledBorderPen = new Pen(Color.FromArgb(120, 120, 120), 1))
            {
                g.DrawEllipse(ledBorderPen, ledRect);
            }

            // Etiket (R, G, B) - LED'in altında
            int labelY = ledY + ledDiameter + 2;
            int labelHeight = Height - labelY - 2;
            if (labelHeight > 8)
            {
                var labelFont = new Font("Segoe UI", Math.Min(9, labelHeight - 2), FontStyle.Bold);
                TextRenderer.DrawText(g, _label, labelFont, 
                    new Rectangle(0, labelY, Width, labelHeight), 
                    Color.White, TextFormatFlags.HorizontalCenter | TextFormatFlags.Top);
            }
        }

        protected override void OnMouseClick(MouseEventArgs e)
        {
            base.OnMouseClick(e);
            _state = (_state + 1) % 3;
            StateChanged?.Invoke(this, EventArgs.Empty);
            Invalidate();
        }

        protected override void Dispose(bool disposing)
        {
            if (disposing)
            {
                _flashTimer?.Stop();
                _flashTimer?.Dispose();
            }
            base.Dispose(disposing);
        }
    }
}
