namespace RASAT_Fizik_Motoru_STM32_ile_senkron3._1
{
    partial class Form1
    {
        private System.ComponentModel.IContainer components = null;

        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            this.UIguncelle = new System.Windows.Forms.Timer(this.components);
            this.tabControl1 = new System.Windows.Forms.TabControl();
            this.tabIzleme = new System.Windows.Forms.TabPage();
            this.btnBaslat = new System.Windows.Forms.Button();
            this.btnDurdur = new System.Windows.Forms.Button();
            this.lblGelen = new System.Windows.Forms.Label();
            this.lblGiden = new System.Windows.Forms.Label();
            this.lblZaman = new System.Windows.Forms.Label();
            this.pgDurum = new System.Windows.Forms.PropertyGrid();
            
            this.lblM1Name = new System.Windows.Forms.Label();
            this.lblM2Name = new System.Windows.Forms.Label();
            this.lblM3Name = new System.Windows.Forms.Label();
            this.lblM4Name = new System.Windows.Forms.Label();
            
            this.pbMotor1 = new System.Windows.Forms.ProgressBar();
            this.pbMotor2 = new System.Windows.Forms.ProgressBar();
            this.pbMotor3 = new System.Windows.Forms.ProgressBar();
            this.pbMotor4 = new System.Windows.Forms.ProgressBar();
            this.lblM1 = new System.Windows.Forms.Label();
            this.lblM2 = new System.Windows.Forms.Label();
            this.lblM3 = new System.Windows.Forms.Label();
            this.lblM4 = new System.Windows.Forms.Label();
            
            this.lblFlagSEP = new System.Windows.Forms.Label();
            this.lblFlagSGM = new System.Windows.Forms.Label();
            this.lblFlagAPAM = new System.Windows.Forms.Label();
            this.lblFlagBUZZ = new System.Windows.Forms.Label();
            this.lblFlagTextSEP = new System.Windows.Forms.Label();
            this.lblFlagTextSGM = new System.Windows.Forms.Label();
            this.lblFlagTextAPAM = new System.Windows.Forms.Label();
            this.lblFlagTextBUZZ = new System.Windows.Forms.Label();
            
            this.pbUyduPlan = new System.Windows.Forms.PictureBox();
            
            this.tabMudahale = new System.Windows.Forms.TabPage();
            this.dgvSenaryo = new System.Windows.Forms.DataGridView();
            this.chkGpsKopar = new System.Windows.Forms.CheckBox();
            this.chkBaroKopar = new System.Windows.Forms.CheckBox();
            this.lblRuzgarX = new System.Windows.Forms.Label();
            this.tbRuzgarX = new System.Windows.Forms.TrackBar();
            this.tabAyarlar = new System.Windows.Forms.TabPage();
            this.cbAyarSecim = new System.Windows.Forms.ComboBox();
            this.pgAyarlar = new System.Windows.Forms.PropertyGrid();
            
            this.tabControl1.SuspendLayout();
            this.tabIzleme.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pbUyduPlan)).BeginInit();
            this.tabMudahale.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.dgvSenaryo)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.tbRuzgarX)).BeginInit();
            this.tabAyarlar.SuspendLayout();
            this.SuspendLayout();
            
            // 
            // UIguncelle
            // 
            this.UIguncelle.Interval = 33;
            this.UIguncelle.Tick += new System.EventHandler(this.UIguncelle_Tick);
            
            // 
            // tabControl1
            // 
            this.tabControl1.Controls.Add(this.tabIzleme);
            this.tabControl1.Controls.Add(this.tabMudahale);
            this.tabControl1.Controls.Add(this.tabAyarlar);
            this.tabControl1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tabControl1.Location = new System.Drawing.Point(0, 0);
            this.tabControl1.Name = "tabControl1";
            this.tabControl1.SelectedIndex = 0;
            this.tabControl1.Size = new System.Drawing.Size(1200, 700);
            this.tabControl1.TabIndex = 0;
            
            // 
            // tabIzleme
            // 
            this.tabIzleme.Controls.Add(this.pgDurum);
            this.tabIzleme.Controls.Add(this.lblZaman);
            this.tabIzleme.Controls.Add(this.lblGiden);
            this.tabIzleme.Controls.Add(this.lblGelen);
            this.tabIzleme.Controls.Add(this.btnDurdur);
            this.tabIzleme.Controls.Add(this.btnBaslat);
            
            this.tabIzleme.Controls.Add(this.lblM1Name);
            this.tabIzleme.Controls.Add(this.lblM2Name);
            this.tabIzleme.Controls.Add(this.lblM3Name);
            this.tabIzleme.Controls.Add(this.lblM4Name);
            
            this.tabIzleme.Controls.Add(this.pbMotor1);
            this.tabIzleme.Controls.Add(this.pbMotor2);
            this.tabIzleme.Controls.Add(this.pbMotor3);
            this.tabIzleme.Controls.Add(this.pbMotor4);
            this.tabIzleme.Controls.Add(this.lblM1);
            this.tabIzleme.Controls.Add(this.lblM2);
            this.tabIzleme.Controls.Add(this.lblM3);
            this.tabIzleme.Controls.Add(this.lblM4);
            
            this.tabIzleme.Controls.Add(this.lblFlagSEP);
            this.tabIzleme.Controls.Add(this.lblFlagSGM);
            this.tabIzleme.Controls.Add(this.lblFlagAPAM);
            this.tabIzleme.Controls.Add(this.lblFlagBUZZ);
            this.tabIzleme.Controls.Add(this.lblFlagTextSEP);
            this.tabIzleme.Controls.Add(this.lblFlagTextSGM);
            this.tabIzleme.Controls.Add(this.lblFlagTextAPAM);
            this.tabIzleme.Controls.Add(this.lblFlagTextBUZZ);
            
            this.tabIzleme.Controls.Add(this.pbUyduPlan);
            
            this.tabIzleme.Location = new System.Drawing.Point(4, 29);
            this.tabIzleme.Name = "tabIzleme";
            this.tabIzleme.Padding = new System.Windows.Forms.Padding(3);
            this.tabIzleme.Size = new System.Drawing.Size(1192, 667);
            this.tabIzleme.TabIndex = 0;
            this.tabIzleme.Text = "İzleme Ekranı";
            this.tabIzleme.UseVisualStyleBackColor = true;
            
            // 
            // btnBaslat
            // 
            this.btnBaslat.Location = new System.Drawing.Point(20, 20);
            this.btnBaslat.Name = "btnBaslat";
            this.btnBaslat.Size = new System.Drawing.Size(120, 40);
            this.btnBaslat.TabIndex = 0;
            this.btnBaslat.Text = "BAŞLAT";
            this.btnBaslat.UseVisualStyleBackColor = true;
            this.btnBaslat.Click += new System.EventHandler(this.btnBaslat_Click);
            
            // 
            // btnDurdur
            // 
            this.btnDurdur.Location = new System.Drawing.Point(150, 20);
            this.btnDurdur.Name = "btnDurdur";
            this.btnDurdur.Size = new System.Drawing.Size(120, 40);
            this.btnDurdur.TabIndex = 1;
            this.btnDurdur.Text = "DURDUR";
            this.btnDurdur.UseVisualStyleBackColor = true;
            this.btnDurdur.Click += new System.EventHandler(this.btnDurdur_Click);
            
            // 
            // lblGelen
            // 
            this.lblGelen.Location = new System.Drawing.Point(300, 20);
            this.lblGelen.Name = "lblGelen";
            this.lblGelen.Size = new System.Drawing.Size(200, 20);
            this.lblGelen.TabIndex = 2;
            this.lblGelen.Text = "Gelen Index: 0";
            
            // 
            // lblGiden
            // 
            this.lblGiden.Location = new System.Drawing.Point(300, 40);
            this.lblGiden.Name = "lblGiden";
            this.lblGiden.Size = new System.Drawing.Size(200, 20);
            this.lblGiden.TabIndex = 3;
            this.lblGiden.Text = "Giden Index: 0";
            
            // 
            // lblZaman
            // 
            this.lblZaman.Location = new System.Drawing.Point(300, 60);
            this.lblZaman.Name = "lblZaman";
            this.lblZaman.Size = new System.Drawing.Size(200, 20);
            this.lblZaman.TabIndex = 5;
            this.lblZaman.Text = "Zaman: 0.00 sn";
            this.lblZaman.Font = new System.Drawing.Font("Segoe UI", 9F, System.Drawing.FontStyle.Bold);
            
            // 
            // pgDurum
            // 
            this.pgDurum.HelpVisible = false;
            this.pgDurum.Location = new System.Drawing.Point(580, 20);
            this.pgDurum.Name = "pgDurum";
            this.pgDurum.Size = new System.Drawing.Size(590, 600);
            this.pgDurum.TabIndex = 4;
            this.pgDurum.ToolbarVisible = false;
            
            // MOTOR ISIMLERI
            this.lblM1Name.Location = new System.Drawing.Point(20, 100);
            this.lblM1Name.Size = new System.Drawing.Size(35, 25);
            this.lblM1Name.Text = "M1:";
            this.lblM1Name.Font = new System.Drawing.Font("Segoe UI", 9F, System.Drawing.FontStyle.Bold);
            
            this.lblM2Name.Location = new System.Drawing.Point(20, 140);
            this.lblM2Name.Size = new System.Drawing.Size(35, 25);
            this.lblM2Name.Text = "M2:";
            this.lblM2Name.Font = new System.Drawing.Font("Segoe UI", 9F, System.Drawing.FontStyle.Bold);
            
            this.lblM3Name.Location = new System.Drawing.Point(20, 180);
            this.lblM3Name.Size = new System.Drawing.Size(35, 25);
            this.lblM3Name.Text = "M3:";
            this.lblM3Name.Font = new System.Drawing.Font("Segoe UI", 9F, System.Drawing.FontStyle.Bold);
            
            this.lblM4Name.Location = new System.Drawing.Point(20, 220);
            this.lblM4Name.Size = new System.Drawing.Size(35, 25);
            this.lblM4Name.Text = "M4:";
            this.lblM4Name.Font = new System.Drawing.Font("Segoe UI", 9F, System.Drawing.FontStyle.Bold);
            
            // MOTOR PROGRESS BARS
            this.pbMotor1.Location = new System.Drawing.Point(60, 100);
            this.pbMotor1.Size = new System.Drawing.Size(210, 25);
            this.pbMotor1.Value = 0;
            
            this.pbMotor2.Location = new System.Drawing.Point(60, 140);
            this.pbMotor2.Size = new System.Drawing.Size(210, 25);
            this.pbMotor2.Value = 0;
            
            this.pbMotor3.Location = new System.Drawing.Point(60, 180);
            this.pbMotor3.Size = new System.Drawing.Size(210, 25);
            this.pbMotor3.Value = 0;
            
            this.pbMotor4.Location = new System.Drawing.Point(60, 220);
            this.pbMotor4.Size = new System.Drawing.Size(210, 25);
            this.pbMotor4.Value = 0;
            
            // MOTOR DEGER ETIKETLERI
            this.lblM1.Location = new System.Drawing.Point(280, 103);
            this.lblM1.Size = new System.Drawing.Size(280, 25);
            this.lblM1.Text = "%0.00 | 0.00 N | 0 RPM (CCW)";
            
            this.lblM2.Location = new System.Drawing.Point(280, 143);
            this.lblM2.Size = new System.Drawing.Size(280, 25);
            this.lblM2.Text = "%0.00 | 0.00 N | 0 RPM (CW)";
            
            this.lblM3.Location = new System.Drawing.Point(280, 183);
            this.lblM3.Size = new System.Drawing.Size(280, 25);
            this.lblM3.Text = "%0.00 | 0.00 N | 0 RPM (CCW)";
            
            this.lblM4.Location = new System.Drawing.Point(280, 223);
            this.lblM4.Size = new System.Drawing.Size(280, 25);
            this.lblM4.Text = "%0.00 | 0.00 N | 0 RPM (CW)";
            
            // STATUS FLAGS (BAYRAKLAR)
            this.lblFlagSEP.Location = new System.Drawing.Point(20, 280);
            this.lblFlagSEP.Size = new System.Drawing.Size(30, 30);
            this.lblFlagSEP.BackColor = System.Drawing.Color.Gray;
            this.lblFlagSEP.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            
            this.lblFlagTextSEP.Location = new System.Drawing.Point(60, 285);
            this.lblFlagTextSEP.Size = new System.Drawing.Size(60, 25);
            this.lblFlagTextSEP.Text = "SEP";
            this.lblFlagTextSEP.Font = new System.Drawing.Font("Segoe UI", 9F, System.Drawing.FontStyle.Bold);
            
            this.lblFlagSGM.Location = new System.Drawing.Point(140, 280);
            this.lblFlagSGM.Size = new System.Drawing.Size(30, 30);
            this.lblFlagSGM.BackColor = System.Drawing.Color.Gray;
            this.lblFlagSGM.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            
            this.lblFlagTextSGM.Location = new System.Drawing.Point(180, 285);
            this.lblFlagTextSGM.Size = new System.Drawing.Size(60, 25);
            this.lblFlagTextSGM.Text = "SGM";
            this.lblFlagTextSGM.Font = new System.Drawing.Font("Segoe UI", 9F, System.Drawing.FontStyle.Bold);
            
            this.lblFlagAPAM.Location = new System.Drawing.Point(260, 280);
            this.lblFlagAPAM.Size = new System.Drawing.Size(30, 30);
            this.lblFlagAPAM.BackColor = System.Drawing.Color.Gray;
            this.lblFlagAPAM.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            
            this.lblFlagTextAPAM.Location = new System.Drawing.Point(300, 285);
            this.lblFlagTextAPAM.Size = new System.Drawing.Size(60, 25);
            this.lblFlagTextAPAM.Text = "APAM";
            this.lblFlagTextAPAM.Font = new System.Drawing.Font("Segoe UI", 9F, System.Drawing.FontStyle.Bold);
            
            this.lblFlagBUZZ.Location = new System.Drawing.Point(380, 280);
            this.lblFlagBUZZ.Size = new System.Drawing.Size(30, 30);
            this.lblFlagBUZZ.BackColor = System.Drawing.Color.Gray;
            this.lblFlagBUZZ.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            
            this.lblFlagTextBUZZ.Location = new System.Drawing.Point(420, 285);
            this.lblFlagTextBUZZ.Size = new System.Drawing.Size(60, 25);
            this.lblFlagTextBUZZ.Text = "BUZZ";
            this.lblFlagTextBUZZ.Font = new System.Drawing.Font("Segoe UI", 9F, System.Drawing.FontStyle.Bold);
            
            // PICTURE BOX (UYDU PLANI)
            this.pbUyduPlan.Location = new System.Drawing.Point(90, 320);
            this.pbUyduPlan.Size = new System.Drawing.Size(350, 330);
            this.pbUyduPlan.SizeMode = System.Windows.Forms.PictureBoxSizeMode.Zoom;
            this.pbUyduPlan.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            
            // 
            // tabMudahale
            // 
            this.tabMudahale.Controls.Add(this.tbRuzgarX);
            this.tabMudahale.Controls.Add(this.lblRuzgarX);
            this.tabMudahale.Controls.Add(this.chkBaroKopar);
            this.tabMudahale.Controls.Add(this.chkGpsKopar);
            this.tabMudahale.Controls.Add(this.dgvSenaryo);
            this.tabMudahale.Location = new System.Drawing.Point(4, 29);
            this.tabMudahale.Name = "tabMudahale";
            this.tabMudahale.Padding = new System.Windows.Forms.Padding(3);
            this.tabMudahale.Size = new System.Drawing.Size(1192, 667);
            this.tabMudahale.TabIndex = 1;
            this.tabMudahale.Text = "Canlı Müdahale & Senaryo";
            this.tabMudahale.UseVisualStyleBackColor = true;
            
            // 
            // dgvSenaryo
            // 
            this.dgvSenaryo.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.dgvSenaryo.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            new System.Windows.Forms.DataGridViewTextBoxColumn() { HeaderText = "Zaman (s)", Name = "Zaman" },
            new System.Windows.Forms.DataGridViewTextBoxColumn() { HeaderText = "Etki Tipi", Name = "EtkiTipi", Width = 200 },
            new System.Windows.Forms.DataGridViewTextBoxColumn() { HeaderText = "Değer", Name = "Deger" }});
            this.dgvSenaryo.Location = new System.Drawing.Point(400, 20);
            this.dgvSenaryo.Name = "dgvSenaryo";
            this.dgvSenaryo.Size = new System.Drawing.Size(750, 600);
            this.dgvSenaryo.TabIndex = 4;
            
            // 
            // chkGpsKopar
            // 
            this.chkGpsKopar.Location = new System.Drawing.Point(20, 20);
            this.chkGpsKopar.Name = "chkGpsKopar";
            this.chkGpsKopar.Size = new System.Drawing.Size(200, 30);
            this.chkGpsKopar.TabIndex = 1;
            this.chkGpsKopar.Text = "GPS Bağlantısını Kopar";
            this.chkGpsKopar.UseVisualStyleBackColor = true;
            
            // 
            // chkBaroKopar
            // 
            this.chkBaroKopar.Location = new System.Drawing.Point(20, 60);
            this.chkBaroKopar.Name = "chkBaroKopar";
            this.chkBaroKopar.Size = new System.Drawing.Size(200, 30);
            this.chkBaroKopar.TabIndex = 2;
            this.chkBaroKopar.Text = "Barometreyi Kopar";
            this.chkBaroKopar.UseVisualStyleBackColor = true;
            
            // 
            // lblRuzgarX
            // 
            this.lblRuzgarX.Location = new System.Drawing.Point(20, 110);
            this.lblRuzgarX.Name = "lblRuzgarX";
            this.lblRuzgarX.Size = new System.Drawing.Size(200, 20);
            this.lblRuzgarX.TabIndex = 3;
            this.lblRuzgarX.Text = "Rüzgar X (m/s)";
            
            // 
            // tbRuzgarX
            // 
            this.tbRuzgarX.Location = new System.Drawing.Point(20, 130);
            this.tbRuzgarX.Maximum = 50;
            this.tbRuzgarX.Minimum = -50;
            this.tbRuzgarX.Name = "tbRuzgarX";
            this.tbRuzgarX.Size = new System.Drawing.Size(300, 56);
            this.tbRuzgarX.TabIndex = 4;
            
            // 
            // tabAyarlar
            // 
            this.tabAyarlar.Controls.Add(this.pgAyarlar);
            this.tabAyarlar.Controls.Add(this.cbAyarSecim);
            this.tabAyarlar.Location = new System.Drawing.Point(4, 29);
            this.tabAyarlar.Name = "tabAyarlar";
            this.tabAyarlar.Padding = new System.Windows.Forms.Padding(3);
            this.tabAyarlar.Size = new System.Drawing.Size(1192, 667);
            this.tabAyarlar.TabIndex = 2;
            this.tabAyarlar.Text = "Gelişmiş Ayarlar";
            this.tabAyarlar.UseVisualStyleBackColor = true;
            
            // 
            // cbAyarSecim
            // 
            this.cbAyarSecim.FormattingEnabled = true;
            this.cbAyarSecim.Items.AddRange(new object[] {
            "Simülasyon Çevre Şartları (Fizik)",
            "IMU Sensör Profili",
            "Barometre Sensör Profili",
            "GPS Sensör Profili",
            "Güç Sensörü Profili"});
            this.cbAyarSecim.Location = new System.Drawing.Point(20, 20);
            this.cbAyarSecim.Name = "cbAyarSecim";
            this.cbAyarSecim.Size = new System.Drawing.Size(300, 28);
            this.cbAyarSecim.TabIndex = 0;
            this.cbAyarSecim.SelectedIndexChanged += new System.EventHandler(this.cbAyarSecim_SelectedIndexChanged);
            
            // 
            // pgAyarlar
            // 
            this.pgAyarlar.Location = new System.Drawing.Point(20, 60);
            this.pgAyarlar.Name = "pgAyarlar";
            this.pgAyarlar.Size = new System.Drawing.Size(600, 580);
            this.pgAyarlar.TabIndex = 1;
            
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 20F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1200, 700);
            this.Controls.Add(this.tabControl1);
            this.Name = "Form1";
            this.Text = "HIL Yer Kontrol İstasyonu v1.0";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.Form1_FormClosing);
            this.Load += new System.EventHandler(this.Form1_Load);
            this.tabControl1.ResumeLayout(false);
            this.tabIzleme.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.pbUyduPlan)).EndInit();
            this.tabMudahale.ResumeLayout(false);
            this.tabMudahale.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.dgvSenaryo)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.tbRuzgarX)).EndInit();
            this.tabAyarlar.ResumeLayout(false);
            this.ResumeLayout(false);
        }

        #endregion

        private System.Windows.Forms.Timer UIguncelle;
        private System.Windows.Forms.TabControl tabControl1;
        private System.Windows.Forms.TabPage tabIzleme;
        private System.Windows.Forms.Button btnBaslat;
        private System.Windows.Forms.Button btnDurdur;
        private System.Windows.Forms.Label lblGelen;
        private System.Windows.Forms.Label lblGiden;
        private System.Windows.Forms.Label lblZaman;
        private System.Windows.Forms.PropertyGrid pgDurum;
        private System.Windows.Forms.TabPage tabMudahale;
        private System.Windows.Forms.DataGridView dgvSenaryo;
        private System.Windows.Forms.CheckBox chkGpsKopar;
        private System.Windows.Forms.CheckBox chkBaroKopar;
        private System.Windows.Forms.Label lblRuzgarX;
        private System.Windows.Forms.TrackBar tbRuzgarX;
        private System.Windows.Forms.TabPage tabAyarlar;
        private System.Windows.Forms.ComboBox cbAyarSecim;
        private System.Windows.Forms.PropertyGrid pgAyarlar;
        
        private System.Windows.Forms.Label lblM1Name;
        private System.Windows.Forms.Label lblM2Name;
        private System.Windows.Forms.Label lblM3Name;
        private System.Windows.Forms.Label lblM4Name;
        private System.Windows.Forms.ProgressBar pbMotor1;
        private System.Windows.Forms.ProgressBar pbMotor2;
        private System.Windows.Forms.ProgressBar pbMotor3;
        private System.Windows.Forms.ProgressBar pbMotor4;
        private System.Windows.Forms.Label lblM1;
        private System.Windows.Forms.Label lblM2;
        private System.Windows.Forms.Label lblM3;
        private System.Windows.Forms.Label lblM4;
        
        private System.Windows.Forms.Label lblFlagSEP;
        private System.Windows.Forms.Label lblFlagSGM;
        private System.Windows.Forms.Label lblFlagAPAM;
        private System.Windows.Forms.Label lblFlagBUZZ;
        private System.Windows.Forms.Label lblFlagTextSEP;
        private System.Windows.Forms.Label lblFlagTextSGM;
        private System.Windows.Forms.Label lblFlagTextAPAM;
        private System.Windows.Forms.Label lblFlagTextBUZZ;
        
        private System.Windows.Forms.PictureBox pbUyduPlan;
    }
}
