namespace RASAT_Fizik_Motoru_STM32_ile_senkron3
{
    partial class Form1
    {
        /// <summary>
        ///  Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        ///  Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        ///  Required method for Designer support - do not modify
        ///  the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            components = new System.ComponentModel.Container();
            btnBaslat = new Button();
            btnDurdur = new Button();
            UIguncelle = new System.Windows.Forms.Timer(components);
            gelenindex = new Label();
            gidenindex = new Label();
            mi1degeri = new Label();
            watchdoglbl = new Label();
            textBox1 = new TextBox();
            SuspendLayout();
            // 
            // btnBaslat
            // 
            btnBaslat.Location = new Point(32, 156);
            btnBaslat.Name = "btnBaslat";
            btnBaslat.Size = new Size(156, 53);
            btnBaslat.TabIndex = 0;
            btnBaslat.Text = "Baslat";
            btnBaslat.UseVisualStyleBackColor = true;
            btnBaslat.Click += btnBaslat_Click;
            // 
            // btnDurdur
            // 
            btnDurdur.Location = new Point(221, 158);
            btnDurdur.Name = "btnDurdur";
            btnDurdur.Size = new Size(199, 48);
            btnDurdur.TabIndex = 1;
            btnDurdur.Text = "Durdur";
            btnDurdur.UseVisualStyleBackColor = true;
            btnDurdur.Click += btnDurdur_Click;
            // 
            // UIguncelle
            // 
            UIguncelle.Interval = 16;
            UIguncelle.Tick += UIguncelle_Tick;
            // 
            // gelenindex
            // 
            gelenindex.AutoSize = true;
            gelenindex.Location = new Point(32, 29);
            gelenindex.Name = "gelenindex";
            gelenindex.Size = new Size(94, 20);
            gelenindex.TabIndex = 2;
            gelenindex.Text = "Gelen Index: ";
            // 
            // gidenindex
            // 
            gidenindex.AutoSize = true;
            gidenindex.Location = new Point(32, 9);
            gidenindex.Name = "gidenindex";
            gidenindex.Size = new Size(95, 20);
            gidenindex.TabIndex = 3;
            gidenindex.Text = "Giden Index: ";
            // 
            // mi1degeri
            // 
            mi1degeri.AutoSize = true;
            mi1degeri.Location = new Point(33, 49);
            mi1degeri.Name = "mi1degeri";
            mi1degeri.Size = new Size(90, 20);
            mi1degeri.TabIndex = 4;
            mi1degeri.Text = "MI1 Degeri: ";
            // 
            // watchdoglbl
            // 
            watchdoglbl.AutoSize = true;
            watchdoglbl.Location = new Point(21, 306);
            watchdoglbl.Name = "watchdoglbl";
            watchdoglbl.Size = new Size(79, 20);
            watchdoglbl.TabIndex = 5;
            watchdoglbl.Text = "WatchDog";
            // 
            // textBox1
            // 
            textBox1.Location = new Point(457, 19);
            textBox1.Multiline = true;
            textBox1.Name = "textBox1";
            textBox1.ScrollBars = ScrollBars.Vertical;
            textBox1.Size = new Size(417, 237);
            textBox1.TabIndex = 6;
            // 
            // Form1
            // 
            AutoScaleDimensions = new SizeF(8F, 20F);
            AutoScaleMode = AutoScaleMode.Font;
            ClientSize = new Size(919, 335);
            Controls.Add(textBox1);
            Controls.Add(watchdoglbl);
            Controls.Add(mi1degeri);
            Controls.Add(gidenindex);
            Controls.Add(gelenindex);
            Controls.Add(btnDurdur);
            Controls.Add(btnBaslat);
            Name = "Form1";
            Text = "Form1";
            FormClosing += Form1_FormClosing;
            Load += Form1_Load;
            ResumeLayout(false);
            PerformLayout();
        }

        #endregion

        private Button btnBaslat;
        private Button btnDurdur;
        private System.Windows.Forms.Timer UIguncelle;
        private Label gelenindex;
        private Label gidenindex;
        private Label mi1degeri;
        private Label watchdoglbl;
        private TextBox textBox1;
    }
}
