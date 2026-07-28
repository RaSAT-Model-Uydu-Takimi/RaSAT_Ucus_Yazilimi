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
            components = new System.ComponentModel.Container();
            UIguncelle = new System.Windows.Forms.Timer(components);
            tabControl1 = new TabControl();
            tabIzleme = new TabPage();
            lblZaman = new Label();
            lblGiden = new Label();
            lblGelen = new Label();
            btnDurdur = new Button();
            btnBaslat = new Button();
            lblM1Name = new Label();
            lblM2Name = new Label();
            lblM3Name = new Label();
            lblM4Name = new Label();
            pbMotor1 = new ProgressBar();
            pbMotor2 = new ProgressBar();
            pbMotor3 = new ProgressBar();
            pbMotor4 = new ProgressBar();
            lblM1 = new Label();
            lblM2 = new Label();
            lblM3 = new Label();
            lblM4 = new Label();
            lblFlagSEP = new Label();
            lblFlagSGM = new Label();
            lblFlagAPAM = new Label();
            lblFlagBUZZ = new Label();
            lblFlagTextSEP = new Label();
            lblFlagTextSGM = new Label();
            lblFlagTextAPAM = new Label();
            lblFlagTextBUZZ = new Label();
            pbUyduPlan = new PictureBox();
            tabMudahale = new TabPage();
            tbRuzgarX = new TrackBar();
            lblRuzgarX = new Label();
            chkBaroKopar = new CheckBox();
            chkGpsKopar = new CheckBox();
            dgvSenaryo = new DataGridView();
            dataGridViewTextBoxColumn1 = new DataGridViewTextBoxColumn();
            dataGridViewTextBoxColumn2 = new DataGridViewTextBoxColumn();
            dataGridViewTextBoxColumn3 = new DataGridViewTextBoxColumn();
            tabAyarlar = new TabPage();
            pgAyarlar = new PropertyGrid();
            cbAyarSecim = new ComboBox();
            tabControl1.SuspendLayout();
            tabIzleme.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)pbUyduPlan).BeginInit();
            tabMudahale.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)tbRuzgarX).BeginInit();
            ((System.ComponentModel.ISupportInitialize)dgvSenaryo).BeginInit();
            tabAyarlar.SuspendLayout();
            SuspendLayout();
            // 
            // UIguncelle
            // 
            UIguncelle.Interval = 33;
            UIguncelle.Tick += UIguncelle_Tick;
            // 
            // tabControl1
            // 
            tabControl1.Controls.Add(tabIzleme);
            tabControl1.Controls.Add(tabMudahale);
            tabControl1.Controls.Add(tabAyarlar);
            tabControl1.Dock = DockStyle.Fill;
            tabControl1.Location = new Point(0, 0);
            tabControl1.Name = "tabControl1";
            tabControl1.SelectedIndex = 0;
            tabControl1.Size = new Size(1200, 700);
            tabControl1.TabIndex = 0;
            // 
            // tabIzleme
            // 
            tabIzleme.Controls.Add(lblZaman);
            tabIzleme.Controls.Add(lblGiden);
            tabIzleme.Controls.Add(lblGelen);
            tabIzleme.Controls.Add(btnDurdur);
            tabIzleme.Controls.Add(btnBaslat);
            tabIzleme.Controls.Add(lblM1Name);
            tabIzleme.Controls.Add(lblM2Name);
            tabIzleme.Controls.Add(lblM3Name);
            tabIzleme.Controls.Add(lblM4Name);
            tabIzleme.Controls.Add(pbMotor1);
            tabIzleme.Controls.Add(pbMotor2);
            tabIzleme.Controls.Add(pbMotor3);
            tabIzleme.Controls.Add(pbMotor4);
            tabIzleme.Controls.Add(lblM1);
            tabIzleme.Controls.Add(lblM2);
            tabIzleme.Controls.Add(lblM3);
            tabIzleme.Controls.Add(lblM4);
            tabIzleme.Controls.Add(lblFlagSEP);
            tabIzleme.Controls.Add(lblFlagSGM);
            tabIzleme.Controls.Add(lblFlagAPAM);
            tabIzleme.Controls.Add(lblFlagBUZZ);
            tabIzleme.Controls.Add(lblFlagTextSEP);
            tabIzleme.Controls.Add(lblFlagTextSGM);
            tabIzleme.Controls.Add(lblFlagTextAPAM);
            tabIzleme.Controls.Add(lblFlagTextBUZZ);
            tabIzleme.Controls.Add(pbUyduPlan);
            tabIzleme.Location = new Point(4, 29);
            tabIzleme.Name = "tabIzleme";
            tabIzleme.Padding = new Padding(3);
            tabIzleme.Size = new Size(1192, 667);
            tabIzleme.TabIndex = 0;
            tabIzleme.Text = "İzleme Ekranı";
            tabIzleme.UseVisualStyleBackColor = true;
            // 
            // lblZaman
            // 
            lblZaman.Font = new Font("Segoe UI", 9F, FontStyle.Bold);
            lblZaman.Location = new Point(300, 60);
            lblZaman.Name = "lblZaman";
            lblZaman.Size = new Size(200, 20);
            lblZaman.TabIndex = 5;
            lblZaman.Text = "Zaman: 0.00 sn";
            // 
            // lblGiden
            // 
            lblGiden.Location = new Point(300, 40);
            lblGiden.Name = "lblGiden";
            lblGiden.Size = new Size(200, 20);
            lblGiden.TabIndex = 3;
            lblGiden.Text = "Giden Index: 0";
            // 
            // lblGelen
            // 
            lblGelen.Location = new Point(300, 20);
            lblGelen.Name = "lblGelen";
            lblGelen.Size = new Size(200, 20);
            lblGelen.TabIndex = 2;
            lblGelen.Text = "Gelen Index: 0";
            // 
            // btnDurdur
            // 
            btnDurdur.Location = new Point(150, 20);
            btnDurdur.Name = "btnDurdur";
            btnDurdur.Size = new Size(120, 40);
            btnDurdur.TabIndex = 1;
            btnDurdur.Text = "DURDUR";
            btnDurdur.UseVisualStyleBackColor = true;
            btnDurdur.Click += btnDurdur_Click;
            // 
            // btnBaslat
            // 
            btnBaslat.Location = new Point(20, 20);
            btnBaslat.Name = "btnBaslat";
            btnBaslat.Size = new Size(120, 40);
            btnBaslat.TabIndex = 0;
            btnBaslat.Text = "BAŞLAT";
            btnBaslat.UseVisualStyleBackColor = true;
            btnBaslat.Click += btnBaslat_Click;
            // 
            // lblM1Name
            // 
            lblM1Name.Font = new Font("Segoe UI", 9F, FontStyle.Bold);
            lblM1Name.Location = new Point(20, 100);
            lblM1Name.Name = "lblM1Name";
            lblM1Name.Size = new Size(35, 25);
            lblM1Name.TabIndex = 6;
            lblM1Name.Text = "M1:";
            // 
            // lblM2Name
            // 
            lblM2Name.Font = new Font("Segoe UI", 9F, FontStyle.Bold);
            lblM2Name.Location = new Point(20, 140);
            lblM2Name.Name = "lblM2Name";
            lblM2Name.Size = new Size(35, 25);
            lblM2Name.TabIndex = 7;
            lblM2Name.Text = "M2:";
            // 
            // lblM3Name
            // 
            lblM3Name.Font = new Font("Segoe UI", 9F, FontStyle.Bold);
            lblM3Name.Location = new Point(20, 180);
            lblM3Name.Name = "lblM3Name";
            lblM3Name.Size = new Size(35, 25);
            lblM3Name.TabIndex = 8;
            lblM3Name.Text = "M3:";
            // 
            // lblM4Name
            // 
            lblM4Name.Font = new Font("Segoe UI", 9F, FontStyle.Bold);
            lblM4Name.Location = new Point(20, 220);
            lblM4Name.Name = "lblM4Name";
            lblM4Name.Size = new Size(35, 25);
            lblM4Name.TabIndex = 9;
            lblM4Name.Text = "M4:";
            // 
            // pbMotor1
            // 
            pbMotor1.Location = new Point(60, 100);
            pbMotor1.Name = "pbMotor1";
            pbMotor1.Size = new Size(210, 25);
            pbMotor1.TabIndex = 10;
            // 
            // pbMotor2
            // 
            pbMotor2.Location = new Point(60, 140);
            pbMotor2.Name = "pbMotor2";
            pbMotor2.Size = new Size(210, 25);
            pbMotor2.TabIndex = 11;
            // 
            // pbMotor3
            // 
            pbMotor3.Location = new Point(60, 180);
            pbMotor3.Name = "pbMotor3";
            pbMotor3.Size = new Size(210, 25);
            pbMotor3.TabIndex = 12;
            // 
            // pbMotor4
            // 
            pbMotor4.Location = new Point(60, 220);
            pbMotor4.Name = "pbMotor4";
            pbMotor4.Size = new Size(210, 25);
            pbMotor4.TabIndex = 13;
            // 
            // lblM1
            // 
            lblM1.Location = new Point(280, 103);
            lblM1.Name = "lblM1";
            lblM1.Size = new Size(280, 25);
            lblM1.TabIndex = 14;
            lblM1.Text = "%0.00 | 0.00 N | 0 RPM (CCW)";
            // 
            // lblM2
            // 
            lblM2.Location = new Point(280, 143);
            lblM2.Name = "lblM2";
            lblM2.Size = new Size(280, 25);
            lblM2.TabIndex = 15;
            lblM2.Text = "%0.00 | 0.00 N | 0 RPM (CW)";
            // 
            // lblM3
            // 
            lblM3.Location = new Point(280, 183);
            lblM3.Name = "lblM3";
            lblM3.Size = new Size(280, 25);
            lblM3.TabIndex = 16;
            lblM3.Text = "%0.00 | 0.00 N | 0 RPM (CCW)";
            // 
            // lblM4
            // 
            lblM4.Location = new Point(280, 223);
            lblM4.Name = "lblM4";
            lblM4.Size = new Size(280, 25);
            lblM4.TabIndex = 17;
            lblM4.Text = "%0.00 | 0.00 N | 0 RPM (CW)";
            // 
            // lblFlagSEP
            // 
            lblFlagSEP.BackColor = Color.Gray;
            lblFlagSEP.BorderStyle = BorderStyle.FixedSingle;
            lblFlagSEP.Location = new Point(20, 280);
            lblFlagSEP.Name = "lblFlagSEP";
            lblFlagSEP.Size = new Size(30, 30);
            lblFlagSEP.TabIndex = 18;
            // 
            // lblFlagSGM
            // 
            lblFlagSGM.BackColor = Color.Gray;
            lblFlagSGM.BorderStyle = BorderStyle.FixedSingle;
            lblFlagSGM.Location = new Point(140, 280);
            lblFlagSGM.Name = "lblFlagSGM";
            lblFlagSGM.Size = new Size(30, 30);
            lblFlagSGM.TabIndex = 19;
            // 
            // lblFlagAPAM
            // 
            lblFlagAPAM.BackColor = Color.Gray;
            lblFlagAPAM.BorderStyle = BorderStyle.FixedSingle;
            lblFlagAPAM.Location = new Point(260, 280);
            lblFlagAPAM.Name = "lblFlagAPAM";
            lblFlagAPAM.Size = new Size(30, 30);
            lblFlagAPAM.TabIndex = 20;
            // 
            // lblFlagBUZZ
            // 
            lblFlagBUZZ.BackColor = Color.Gray;
            lblFlagBUZZ.BorderStyle = BorderStyle.FixedSingle;
            lblFlagBUZZ.Location = new Point(380, 280);
            lblFlagBUZZ.Name = "lblFlagBUZZ";
            lblFlagBUZZ.Size = new Size(30, 30);
            lblFlagBUZZ.TabIndex = 21;
            // 
            // lblFlagTextSEP
            // 
            lblFlagTextSEP.Font = new Font("Segoe UI", 9F, FontStyle.Bold);
            lblFlagTextSEP.Location = new Point(60, 285);
            lblFlagTextSEP.Name = "lblFlagTextSEP";
            lblFlagTextSEP.Size = new Size(60, 25);
            lblFlagTextSEP.TabIndex = 22;
            lblFlagTextSEP.Text = "SEP";
            // 
            // lblFlagTextSGM
            // 
            lblFlagTextSGM.Font = new Font("Segoe UI", 9F, FontStyle.Bold);
            lblFlagTextSGM.Location = new Point(180, 285);
            lblFlagTextSGM.Name = "lblFlagTextSGM";
            lblFlagTextSGM.Size = new Size(60, 25);
            lblFlagTextSGM.TabIndex = 23;
            lblFlagTextSGM.Text = "SGM";
            // 
            // lblFlagTextAPAM
            // 
            lblFlagTextAPAM.Font = new Font("Segoe UI", 9F, FontStyle.Bold);
            lblFlagTextAPAM.Location = new Point(300, 285);
            lblFlagTextAPAM.Name = "lblFlagTextAPAM";
            lblFlagTextAPAM.Size = new Size(60, 25);
            lblFlagTextAPAM.TabIndex = 24;
            lblFlagTextAPAM.Text = "APAM";
            // 
            // lblFlagTextBUZZ
            // 
            lblFlagTextBUZZ.Font = new Font("Segoe UI", 9F, FontStyle.Bold);
            lblFlagTextBUZZ.Location = new Point(420, 285);
            lblFlagTextBUZZ.Name = "lblFlagTextBUZZ";
            lblFlagTextBUZZ.Size = new Size(60, 25);
            lblFlagTextBUZZ.TabIndex = 25;
            lblFlagTextBUZZ.Text = "BUZZ";
            // 
            // pbUyduPlan
            // 
            pbUyduPlan.BorderStyle = BorderStyle.FixedSingle;
            pbUyduPlan.Image = Properties.Resources.uydu_plan;
            pbUyduPlan.Location = new Point(20, 339);
            pbUyduPlan.Name = "pbUyduPlan";
            pbUyduPlan.Size = new Size(180, 182);
            pbUyduPlan.SizeMode = PictureBoxSizeMode.Zoom;
            pbUyduPlan.TabIndex = 26;
            pbUyduPlan.TabStop = false;
            // 
            // tabMudahale
            // 
            tabMudahale.Controls.Add(tbRuzgarX);
            tabMudahale.Controls.Add(lblRuzgarX);
            tabMudahale.Controls.Add(chkBaroKopar);
            tabMudahale.Controls.Add(chkGpsKopar);
            tabMudahale.Controls.Add(dgvSenaryo);
            tabMudahale.Location = new Point(4, 29);
            tabMudahale.Name = "tabMudahale";
            tabMudahale.Padding = new Padding(3);
            tabMudahale.Size = new Size(1192, 667);
            tabMudahale.TabIndex = 1;
            tabMudahale.Text = "Canlı Müdahale & Senaryo";
            tabMudahale.UseVisualStyleBackColor = true;
            // 
            // tbRuzgarX
            // 
            tbRuzgarX.Location = new Point(20, 130);
            tbRuzgarX.Maximum = 50;
            tbRuzgarX.Minimum = -50;
            tbRuzgarX.Name = "tbRuzgarX";
            tbRuzgarX.Size = new Size(300, 56);
            tbRuzgarX.TabIndex = 4;
            // 
            // lblRuzgarX
            // 
            lblRuzgarX.Location = new Point(20, 110);
            lblRuzgarX.Name = "lblRuzgarX";
            lblRuzgarX.Size = new Size(200, 20);
            lblRuzgarX.TabIndex = 3;
            lblRuzgarX.Text = "Rüzgar X (m/s)";
            // 
            // chkBaroKopar
            // 
            chkBaroKopar.Location = new Point(20, 60);
            chkBaroKopar.Name = "chkBaroKopar";
            chkBaroKopar.Size = new Size(200, 30);
            chkBaroKopar.TabIndex = 2;
            chkBaroKopar.Text = "Barometreyi Kopar";
            chkBaroKopar.UseVisualStyleBackColor = true;
            // 
            // chkGpsKopar
            // 
            chkGpsKopar.Location = new Point(20, 20);
            chkGpsKopar.Name = "chkGpsKopar";
            chkGpsKopar.Size = new Size(200, 30);
            chkGpsKopar.TabIndex = 1;
            chkGpsKopar.Text = "GPS Bağlantısını Kopar";
            chkGpsKopar.UseVisualStyleBackColor = true;
            // 
            // dgvSenaryo
            // 
            dgvSenaryo.ColumnHeadersHeightSizeMode = DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            dgvSenaryo.Columns.AddRange(new DataGridViewColumn[] { dataGridViewTextBoxColumn1, dataGridViewTextBoxColumn2, dataGridViewTextBoxColumn3 });
            dgvSenaryo.Location = new Point(400, 20);
            dgvSenaryo.Name = "dgvSenaryo";
            dgvSenaryo.RowHeadersWidth = 51;
            dgvSenaryo.Size = new Size(750, 600);
            dgvSenaryo.TabIndex = 4;
            // 
            // dataGridViewTextBoxColumn1
            // 
            dataGridViewTextBoxColumn1.MinimumWidth = 6;
            dataGridViewTextBoxColumn1.Name = "dataGridViewTextBoxColumn1";
            dataGridViewTextBoxColumn1.Width = 125;
            // 
            // dataGridViewTextBoxColumn2
            // 
            dataGridViewTextBoxColumn2.MinimumWidth = 6;
            dataGridViewTextBoxColumn2.Name = "dataGridViewTextBoxColumn2";
            dataGridViewTextBoxColumn2.Width = 125;
            // 
            // dataGridViewTextBoxColumn3
            // 
            dataGridViewTextBoxColumn3.MinimumWidth = 6;
            dataGridViewTextBoxColumn3.Name = "dataGridViewTextBoxColumn3";
            dataGridViewTextBoxColumn3.Width = 125;
            // 
            // tabAyarlar
            // 
            tabAyarlar.Controls.Add(pgAyarlar);
            tabAyarlar.Controls.Add(cbAyarSecim);
            tabAyarlar.Location = new Point(4, 29);
            tabAyarlar.Name = "tabAyarlar";
            tabAyarlar.Padding = new Padding(3);
            tabAyarlar.Size = new Size(1192, 667);
            tabAyarlar.TabIndex = 2;
            tabAyarlar.Text = "Gelişmiş Ayarlar";
            tabAyarlar.UseVisualStyleBackColor = true;
            // 
            // pgAyarlar
            // 
            pgAyarlar.Location = new Point(20, 60);
            pgAyarlar.Name = "pgAyarlar";
            pgAyarlar.Size = new Size(600, 580);
            pgAyarlar.TabIndex = 1;
            // 
            // cbAyarSecim
            // 
            cbAyarSecim.FormattingEnabled = true;
            cbAyarSecim.Items.AddRange(new object[] { "Simülasyon Çevre Şartları (Fizik)", "IMU Sensör Profili", "Barometre Sensör Profili", "GPS Sensör Profili", "Güç Sensörü Profili" });
            cbAyarSecim.Location = new Point(20, 20);
            cbAyarSecim.Name = "cbAyarSecim";
            cbAyarSecim.Size = new Size(300, 28);
            cbAyarSecim.TabIndex = 0;
            cbAyarSecim.SelectedIndexChanged += cbAyarSecim_SelectedIndexChanged;
            // 
            // Form1
            // 
            AutoScaleDimensions = new SizeF(8F, 20F);
            AutoScaleMode = AutoScaleMode.Font;
            ClientSize = new Size(1200, 700);
            Controls.Add(tabControl1);
            Name = "Form1";
            Text = "HIL Yer Kontrol İstasyonu v1.0";
            FormClosing += Form1_FormClosing;
            Load += Form1_Load;
            tabControl1.ResumeLayout(false);
            tabIzleme.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)pbUyduPlan).EndInit();
            tabMudahale.ResumeLayout(false);
            tabMudahale.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)tbRuzgarX).EndInit();
            ((System.ComponentModel.ISupportInitialize)dgvSenaryo).EndInit();
            tabAyarlar.ResumeLayout(false);
            ResumeLayout(false);
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
        private DataGridViewTextBoxColumn dataGridViewTextBoxColumn1;
        private DataGridViewTextBoxColumn dataGridViewTextBoxColumn2;
        private DataGridViewTextBoxColumn dataGridViewTextBoxColumn3;
    }
}
