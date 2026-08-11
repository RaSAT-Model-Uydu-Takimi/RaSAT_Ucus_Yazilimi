namespace _10OcakRASAT
{
    partial class Form1
    {
        
        /// Required designer variable.
        
        private System.ComponentModel.IContainer components = null;

        ///  
        /// Clean up any resources being being used.
        
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

        ///  
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.

        private void InitializeComponent()
        {
            panelHeader = new Panel();
            telemetryFlowPanel = new FlowLayoutPanel();
            gbBasinc = new GroupBox();
            lblBasincValue = new Label();
            pbBasinc = new PictureBox();
            gbYukseklik = new GroupBox();
            lblYukseklikValue = new Label();
            pbYukseklik = new PictureBox();
            gbInisHizi = new GroupBox();
            lblInisHiziValue = new Label();
            pbInisHizi = new PictureBox();
            gbSicaklik = new GroupBox();
            lblSicaklikValue = new Label();
            pbSicaklik = new PictureBox();
            gbPil = new GroupBox();
            lblPilValue = new Label();
            pbPil = new PictureBox();
            gbGpsLat = new GroupBox();
            lblGpsLatValue = new Label();
            pbGpsLat = new PictureBox();
            gbGpsLon = new GroupBox();
            lblGpsLonValue = new Label();
            pbGpsLon = new PictureBox();
            gbPitch = new GroupBox();
            lblPitchValue = new Label();
            pbPitch = new PictureBox();
            gbRoll = new GroupBox();
            lblRollValue = new Label();
            pbRoll = new PictureBox();
            gbYaw = new GroupBox();
            lblYawValue = new Label();
            pbYaw = new PictureBox();
            panelConnection = new Panel();
            lblTitle = new Label();
            lblCom = new Label();
            cbPorts = new ComboBox();
            lblBaud = new Label();
            cbBaud = new ComboBox();
            btnConnect = new Button();
            splitMain = new SplitContainer();
            panelListBox = new ListBox();
            lblPanelListHeader = new Label();
            mainGrid = new TableLayoutPanel();
            gbMainScreen = new GroupBox();
            gbCellA = new GroupBox();
            gbCellB = new GroupBox();
            gbCellC = new GroupBox();
            gbCellD = new GroupBox();
            gbCellE = new GroupBox();
            gbCellF = new GroupBox();
            gbCellG = new GroupBox();
            gbCellH = new GroupBox();
            bottomPanel = new Panel();
            btnAction2 = new Button();
            btnFilePath = new Button();
            panelHeader.SuspendLayout();
            telemetryFlowPanel.SuspendLayout();
            gbBasinc.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)pbBasinc).BeginInit();
            gbYukseklik.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)pbYukseklik).BeginInit();
            gbInisHizi.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)pbInisHizi).BeginInit();
            gbSicaklik.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)pbSicaklik).BeginInit();
            gbPil.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)pbPil).BeginInit();
            gbGpsLat.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)pbGpsLat).BeginInit();
            gbGpsLon.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)pbGpsLon).BeginInit();
            gbPitch.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)pbPitch).BeginInit();
            gbRoll.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)pbRoll).BeginInit();
            gbYaw.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)pbYaw).BeginInit();
            panelConnection.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)splitMain).BeginInit();
            splitMain.Panel1.SuspendLayout();
            splitMain.Panel2.SuspendLayout();
            splitMain.SuspendLayout();
            mainGrid.SuspendLayout();
            bottomPanel.SuspendLayout();
            SuspendLayout();
            // 
            // panelHeader
            // 
            panelHeader.BackColor = Color.FromArgb(25, 25, 25);
            panelHeader.Controls.Add(telemetryFlowPanel);
            panelHeader.Dock = DockStyle.Top;
            panelHeader.Location = new Point(0, 0);
            panelHeader.Name = "panelHeader";
            panelHeader.Size = new Size(1898, 110);
            panelHeader.TabIndex = 0;
            // 
            // telemetryFlowPanel
            // 
            telemetryFlowPanel.AutoScroll = true;
            telemetryFlowPanel.BackColor = Color.FromArgb(30, 30, 30);
            telemetryFlowPanel.Controls.Add(gbBasinc);
            telemetryFlowPanel.Controls.Add(gbYukseklik);
            telemetryFlowPanel.Controls.Add(gbInisHizi);
            telemetryFlowPanel.Controls.Add(gbSicaklik);
            telemetryFlowPanel.Controls.Add(gbPil);
            telemetryFlowPanel.Controls.Add(gbGpsLat);
            telemetryFlowPanel.Controls.Add(gbGpsLon);
            telemetryFlowPanel.Controls.Add(gbPitch);
            telemetryFlowPanel.Controls.Add(gbRoll);
            telemetryFlowPanel.Controls.Add(gbYaw);
            telemetryFlowPanel.Dock = DockStyle.Fill;
            telemetryFlowPanel.Location = new Point(391, 0);
            telemetryFlowPanel.Name = "telemetryFlowPanel";
            telemetryFlowPanel.Padding = new Padding(6);
            telemetryFlowPanel.Size = new Size(1507, 110);
            telemetryFlowPanel.TabIndex = 0;
            telemetryFlowPanel.WrapContents = false;
            // 
            // gbBasinc
            // 
            gbBasinc.BackColor = Color.FromArgb(40, 40, 40);
            gbBasinc.Controls.Add(lblBasincValue);
            gbBasinc.Controls.Add(pbBasinc);
            gbBasinc.ForeColor = Color.Gainsboro;
            gbBasinc.Location = new Point(12, 12);
            gbBasinc.Margin = new Padding(6);
            gbBasinc.Name = "gbBasinc";
            gbBasinc.Size = new Size(160, 96);
            gbBasinc.TabIndex = 0;
            gbBasinc.TabStop = false;
            gbBasinc.Text = "Basınç";
            // 
            // lblBasincValue
            // 
            lblBasincValue.AutoSize = true;
            lblBasincValue.ForeColor = Color.Gainsboro;
            lblBasincValue.Location = new Point(64, 40);
            lblBasincValue.Name = "lblBasincValue";
            lblBasincValue.Size = new Size(69, 25);
            lblBasincValue.TabIndex = 1;
            lblBasincValue.Text = "P Value";
            // 
            // pbBasinc
            // 
            pbBasinc.BackColor = Color.FromArgb(50, 50, 50);
            pbBasinc.BorderStyle = BorderStyle.FixedSingle;
            pbBasinc.Location = new Point(8, 28);
            pbBasinc.Name = "pbBasinc";
            pbBasinc.Size = new Size(48, 48);
            pbBasinc.SizeMode = PictureBoxSizeMode.Zoom;
            pbBasinc.TabIndex = 0;
            pbBasinc.TabStop = false;
            // 
            // gbYukseklik
            // 
            gbYukseklik.BackColor = Color.FromArgb(40, 40, 40);
            gbYukseklik.Controls.Add(lblYukseklikValue);
            gbYukseklik.Controls.Add(pbYukseklik);
            gbYukseklik.ForeColor = Color.Gainsboro;
            gbYukseklik.Location = new Point(184, 12);
            gbYukseklik.Margin = new Padding(6);
            gbYukseklik.Name = "gbYukseklik";
            gbYukseklik.Size = new Size(160, 96);
            gbYukseklik.TabIndex = 1;
            gbYukseklik.TabStop = false;
            gbYukseklik.Text = "Yükseklik";
            // 
            // lblYukseklikValue
            // 
            lblYukseklikValue.AutoSize = true;
            lblYukseklikValue.ForeColor = Color.Gainsboro;
            lblYukseklikValue.Location = new Point(64, 40);
            lblYukseklikValue.Name = "lblYukseklikValue";
            lblYukseklikValue.Size = new Size(81, 25);
            lblYukseklikValue.TabIndex = 1;
            lblYukseklikValue.Text = "Alt Value";
            // 
            // pbYukseklik
            // 
            pbYukseklik.BackColor = Color.FromArgb(50, 50, 50);
            pbYukseklik.BorderStyle = BorderStyle.FixedSingle;
            pbYukseklik.Location = new Point(8, 28);
            pbYukseklik.Name = "pbYukseklik";
            pbYukseklik.Size = new Size(48, 48);
            pbYukseklik.SizeMode = PictureBoxSizeMode.Zoom;
            pbYukseklik.TabIndex = 0;
            pbYukseklik.TabStop = false;
            // 
            // gbInisHizi
            // 
            gbInisHizi.BackColor = Color.FromArgb(40, 40, 40);
            gbInisHizi.Controls.Add(lblInisHiziValue);
            gbInisHizi.Controls.Add(pbInisHizi);
            gbInisHizi.ForeColor = Color.Gainsboro;
            gbInisHizi.Location = new Point(356, 12);
            gbInisHizi.Margin = new Padding(6);
            gbInisHizi.Name = "gbInisHizi";
            gbInisHizi.Size = new Size(160, 96);
            gbInisHizi.TabIndex = 2;
            gbInisHizi.TabStop = false;
            gbInisHizi.Text = "İniş Hızı";
            // 
            // lblInisHiziValue
            // 
            lblInisHiziValue.AutoSize = true;
            lblInisHiziValue.ForeColor = Color.Gainsboro;
            lblInisHiziValue.Location = new Point(64, 40);
            lblInisHiziValue.Name = "lblInisHiziValue";
            lblInisHiziValue.Size = new Size(70, 25);
            lblInisHiziValue.TabIndex = 1;
            lblInisHiziValue.Text = "V Value";
            // 
            // pbInisHizi
            // 
            pbInisHizi.BackColor = Color.FromArgb(50, 50, 50);
            pbInisHizi.BorderStyle = BorderStyle.FixedSingle;
            pbInisHizi.Location = new Point(8, 28);
            pbInisHizi.Name = "pbInisHizi";
            pbInisHizi.Size = new Size(48, 48);
            pbInisHizi.SizeMode = PictureBoxSizeMode.Zoom;
            pbInisHizi.TabIndex = 0;
            pbInisHizi.TabStop = false;
            // 
            // gbSicaklik
            // 
            gbSicaklik.BackColor = Color.FromArgb(40, 40, 40);
            gbSicaklik.Controls.Add(lblSicaklikValue);
            gbSicaklik.Controls.Add(pbSicaklik);
            gbSicaklik.ForeColor = Color.Gainsboro;
            gbSicaklik.Location = new Point(528, 12);
            gbSicaklik.Margin = new Padding(6);
            gbSicaklik.Name = "gbSicaklik";
            gbSicaklik.Size = new Size(160, 96);
            gbSicaklik.TabIndex = 3;
            gbSicaklik.TabStop = false;
            gbSicaklik.Text = "Sıcaklık";
            // 
            // lblSicaklikValue
            // 
            lblSicaklikValue.AutoSize = true;
            lblSicaklikValue.ForeColor = Color.Gainsboro;
            lblSicaklikValue.Location = new Point(64, 40);
            lblSicaklikValue.Name = "lblSicaklikValue";
            lblSicaklikValue.Size = new Size(68, 25);
            lblSicaklikValue.TabIndex = 1;
            lblSicaklikValue.Text = "T Value";
            // 
            // pbSicaklik
            // 
            pbSicaklik.BackColor = Color.FromArgb(50, 50, 50);
            pbSicaklik.BorderStyle = BorderStyle.FixedSingle;
            pbSicaklik.Location = new Point(8, 28);
            pbSicaklik.Name = "pbSicaklik";
            pbSicaklik.Size = new Size(48, 48);
            pbSicaklik.SizeMode = PictureBoxSizeMode.Zoom;
            pbSicaklik.TabIndex = 0;
            pbSicaklik.TabStop = false;
            // 
            // gbPil
            // 
            gbPil.BackColor = Color.FromArgb(40, 40, 40);
            gbPil.Controls.Add(lblPilValue);
            gbPil.Controls.Add(pbPil);
            gbPil.ForeColor = Color.Gainsboro;
            gbPil.Location = new Point(700, 12);
            gbPil.Margin = new Padding(6);
            gbPil.Name = "gbPil";
            gbPil.Size = new Size(160, 96);
            gbPil.TabIndex = 4;
            gbPil.TabStop = false;
            gbPil.Text = "Pil Gerilimi";
            // 
            // lblPilValue
            // 
            lblPilValue.AutoSize = true;
            lblPilValue.ForeColor = Color.Gainsboro;
            lblPilValue.Location = new Point(64, 40);
            lblPilValue.Name = "lblPilValue";
            lblPilValue.Size = new Size(84, 25);
            lblPilValue.TabIndex = 1;
            lblPilValue.Text = "Bat Value";
            // 
            // pbPil
            // 
            pbPil.BackColor = Color.FromArgb(50, 50, 50);
            pbPil.BorderStyle = BorderStyle.FixedSingle;
            pbPil.Location = new Point(8, 28);
            pbPil.Name = "pbPil";
            pbPil.Size = new Size(48, 48);
            pbPil.SizeMode = PictureBoxSizeMode.Zoom;
            pbPil.TabIndex = 0;
            pbPil.TabStop = false;
            // 
            // gbGpsLat
            // 
            gbGpsLat.BackColor = Color.FromArgb(40, 40, 40);
            gbGpsLat.Controls.Add(lblGpsLatValue);
            gbGpsLat.Controls.Add(pbGpsLat);
            gbGpsLat.ForeColor = Color.Gainsboro;
            gbGpsLat.Location = new Point(872, 12);
            gbGpsLat.Margin = new Padding(6);
            gbGpsLat.Name = "gbGpsLat";
            gbGpsLat.Size = new Size(160, 96);
            gbGpsLat.TabIndex = 5;
            gbGpsLat.TabStop = false;
            gbGpsLat.Text = "GPS Lat";
            // 
            // lblGpsLatValue
            // 
            lblGpsLatValue.AutoSize = true;
            lblGpsLatValue.ForeColor = Color.Gainsboro;
            lblGpsLatValue.Location = new Point(64, 40);
            lblGpsLatValue.Name = "lblGpsLatValue";
            lblGpsLatValue.Size = new Size(63, 25);
            lblGpsLatValue.TabIndex = 1;
            lblGpsLatValue.Text = "Lat Val";
            // 
            // pbGpsLat
            // 
            pbGpsLat.BackColor = Color.FromArgb(50, 50, 50);
            pbGpsLat.BorderStyle = BorderStyle.FixedSingle;
            pbGpsLat.Location = new Point(8, 28);
            pbGpsLat.Name = "pbGpsLat";
            pbGpsLat.Size = new Size(48, 48);
            pbGpsLat.SizeMode = PictureBoxSizeMode.Zoom;
            pbGpsLat.TabIndex = 0;
            pbGpsLat.TabStop = false;
            // 
            // gbGpsLon
            // 
            gbGpsLon.BackColor = Color.FromArgb(40, 40, 40);
            gbGpsLon.Controls.Add(lblGpsLonValue);
            gbGpsLon.Controls.Add(pbGpsLon);
            gbGpsLon.ForeColor = Color.Gainsboro;
            gbGpsLon.Location = new Point(1044, 12);
            gbGpsLon.Margin = new Padding(6);
            gbGpsLon.Name = "gbGpsLon";
            gbGpsLon.Size = new Size(160, 96);
            gbGpsLon.TabIndex = 6;
            gbGpsLon.TabStop = false;
            gbGpsLon.Text = "GPS Lon";
            // 
            // lblGpsLonValue
            // 
            lblGpsLonValue.AutoSize = true;
            lblGpsLonValue.ForeColor = Color.Gainsboro;
            lblGpsLonValue.Location = new Point(64, 40);
            lblGpsLonValue.Name = "lblGpsLonValue";
            lblGpsLonValue.Size = new Size(69, 25);
            lblGpsLonValue.TabIndex = 1;
            lblGpsLonValue.Text = "Lon Val";
            // 
            // pbGpsLon
            // 
            pbGpsLon.BackColor = Color.FromArgb(50, 50, 50);
            pbGpsLon.BorderStyle = BorderStyle.FixedSingle;
            pbGpsLon.Location = new Point(8, 28);
            pbGpsLon.Name = "pbGpsLon";
            pbGpsLon.Size = new Size(48, 48);
            pbGpsLon.SizeMode = PictureBoxSizeMode.Zoom;
            pbGpsLon.TabIndex = 0;
            pbGpsLon.TabStop = false;
            // 
            // gbPitch
            // 
            gbPitch.BackColor = Color.FromArgb(40, 40, 40);
            gbPitch.Controls.Add(lblPitchValue);
            gbPitch.Controls.Add(pbPitch);
            gbPitch.ForeColor = Color.Gainsboro;
            gbPitch.Location = new Point(1216, 12);
            gbPitch.Margin = new Padding(6);
            gbPitch.Name = "gbPitch";
            gbPitch.Size = new Size(160, 96);
            gbPitch.TabIndex = 7;
            gbPitch.TabStop = false;
            gbPitch.Text = "Pitch";
            // 
            // lblPitchValue
            // 
            lblPitchValue.AutoSize = true;
            lblPitchValue.ForeColor = Color.Gainsboro;
            lblPitchValue.Location = new Point(64, 40);
            lblPitchValue.Name = "lblPitchValue";
            lblPitchValue.Size = new Size(61, 25);
            lblPitchValue.TabIndex = 1;
            lblPitchValue.Text = "PitchV";
            // 
            // pbPitch
            // 
            pbPitch.BackColor = Color.FromArgb(50, 50, 50);
            pbPitch.BorderStyle = BorderStyle.FixedSingle;
            pbPitch.Location = new Point(8, 28);
            pbPitch.Name = "pbPitch";
            pbPitch.Size = new Size(48, 48);
            pbPitch.SizeMode = PictureBoxSizeMode.Zoom;
            pbPitch.TabIndex = 0;
            pbPitch.TabStop = false;
            // 
            // gbRoll
            // 
            gbRoll.BackColor = Color.FromArgb(40, 40, 40);
            gbRoll.Controls.Add(lblRollValue);
            gbRoll.Controls.Add(pbRoll);
            gbRoll.ForeColor = Color.Gainsboro;
            gbRoll.Location = new Point(1388, 12);
            gbRoll.Margin = new Padding(6);
            gbRoll.Name = "gbRoll";
            gbRoll.Size = new Size(160, 96);
            gbRoll.TabIndex = 8;
            gbRoll.TabStop = false;
            gbRoll.Text = "Roll";
            // 
            // lblRollValue
            // 
            lblRollValue.AutoSize = true;
            lblRollValue.ForeColor = Color.Gainsboro;
            lblRollValue.Location = new Point(64, 40);
            lblRollValue.Name = "lblRollValue";
            lblRollValue.Size = new Size(52, 25);
            lblRollValue.TabIndex = 1;
            lblRollValue.Text = "RollV";
            // 
            // pbRoll
            // 
            pbRoll.BackColor = Color.FromArgb(50, 50, 50);
            pbRoll.BorderStyle = BorderStyle.FixedSingle;
            pbRoll.Location = new Point(8, 28);
            pbRoll.Name = "pbRoll";
            pbRoll.Size = new Size(48, 48);
            pbRoll.SizeMode = PictureBoxSizeMode.Zoom;
            pbRoll.TabIndex = 0;
            pbRoll.TabStop = false;
            // 
            // gbYaw
            // 
            gbYaw.BackColor = Color.FromArgb(40, 40, 40);
            gbYaw.Controls.Add(lblYawValue);
            gbYaw.Controls.Add(pbYaw);
            gbYaw.ForeColor = Color.Gainsboro;
            gbYaw.Location = new Point(1560, 12);
            gbYaw.Margin = new Padding(6);
            gbYaw.Name = "gbYaw";
            gbYaw.Size = new Size(160, 96);
            gbYaw.TabIndex = 9;
            gbYaw.TabStop = false;
            gbYaw.Text = "Yaw";
            // 
            // lblYawValue
            // 
            lblYawValue.AutoSize = true;
            lblYawValue.ForeColor = Color.Gainsboro;
            lblYawValue.Location = new Point(64, 40);
            lblYawValue.Name = "lblYawValue";
            lblYawValue.Size = new Size(53, 25);
            lblYawValue.TabIndex = 1;
            lblYawValue.Text = "YawV";
            // 
            // pbYaw
            // 
            pbYaw.BackColor = Color.FromArgb(50, 50, 50);
            pbYaw.BorderStyle = BorderStyle.FixedSingle;
            pbYaw.Location = new Point(8, 28);
            pbYaw.Name = "pbYaw";
            pbYaw.Size = new Size(48, 48);
            pbYaw.SizeMode = PictureBoxSizeMode.Zoom;
            pbYaw.TabIndex = 0;
            pbYaw.TabStop = false;
            // 
            // panelConnection
            // 
            panelConnection.BackColor = Color.FromArgb(30, 30, 35);
            panelConnection.Controls.Add(lblTitle);
            panelConnection.Controls.Add(lblCom);
            panelConnection.Controls.Add(cbBaud);
            panelConnection.Controls.Add(cbPorts);
            panelConnection.Controls.Add(lblBaud);
            panelConnection.Dock = DockStyle.Left;
            panelConnection.Location = new Point(0, 0);
            panelConnection.Name = "panelConnection";
            panelConnection.Padding = new Padding(10);
            panelConnection.Size = new Size(391, 110);
            panelConnection.TabIndex = 1;
            // 
            // lblTitle
            // 
            lblTitle.AutoSize = true;
            lblTitle.Font = new Font("Segoe UI", 8F, FontStyle.Bold);
            lblTitle.ForeColor = Color.FromArgb(200, 200, 200);
            lblTitle.Location = new Point(6, 0);
            lblTitle.Name = "lblTitle";
            lblTitle.Size = new Size(174, 21);
            lblTitle.TabIndex = 0;
            lblTitle.Text = "SERİ BAĞLANTI (STM)";
            // 
            // lblCom
            // 
            lblCom.AutoSize = true;
            lblCom.ForeColor = Color.Gainsboro;
            lblCom.Location = new Point(10, 35);
            lblCom.Name = "lblCom";
            lblCom.Size = new Size(48, 25);
            lblCom.TabIndex = 1;
            lblCom.Text = "Port:";
            // 
            // cbPorts
            // 
            cbPorts.BackColor = Color.FromArgb(50, 50, 50);
            cbPorts.FlatStyle = FlatStyle.Flat;
            cbPorts.ForeColor = Color.White;
            cbPorts.FormattingEnabled = true;
            cbPorts.Items.AddRange(new object[] { "COM1", "COM2", "COM3", "COM4", "COM5" });
            cbPorts.Location = new Point(64, 35);
            cbPorts.Name = "cbPorts";
            cbPorts.Size = new Size(127, 33);
            cbPorts.TabIndex = 2;
            // 
            // lblBaud
            // 
            lblBaud.AutoSize = true;
            lblBaud.ForeColor = Color.Gainsboro;
            lblBaud.Location = new Point(197, 35);
            lblBaud.Name = "lblBaud";
            lblBaud.Size = new Size(56, 25);
            lblBaud.TabIndex = 3;
            lblBaud.Text = "Baud:";
            // 
            // cbBaud
            // 
            cbBaud.BackColor = Color.FromArgb(50, 50, 50);
            cbBaud.FlatStyle = FlatStyle.Flat;
            cbBaud.ForeColor = Color.White;
            cbBaud.FormattingEnabled = true;
            cbBaud.Items.AddRange(new object[] { "9600", "115200" });
            cbBaud.Location = new Point(259, 35);
            cbBaud.Name = "cbBaud";
            cbBaud.Size = new Size(126, 33);
            cbBaud.TabIndex = 4;
            // 
            // btnConnect
            // 
            btnConnect.Anchor = AnchorStyles.Left;
            btnConnect.BackColor = Color.FromArgb(0, 100, 150);
            btnConnect.Cursor = Cursors.Hand;
            btnConnect.FlatAppearance.BorderSize = 0;
            btnConnect.FlatStyle = FlatStyle.Flat;
            btnConnect.Font = new Font("Segoe UI", 9F, FontStyle.Bold);
            btnConnect.ForeColor = Color.White;
            btnConnect.Location = new Point(590, 30);
            btnConnect.Name = "btnConnect";
            btnConnect.Size = new Size(160, 50);
            btnConnect.TabIndex = 5;
            btnConnect.Text = "BAĞLAN";
            btnConnect.UseVisualStyleBackColor = false;
            // 
            // splitMain
            // 
            splitMain.BackColor = Color.FromArgb(20, 20, 20);
            splitMain.Dock = DockStyle.Fill;
            splitMain.FixedPanel = FixedPanel.Panel1;
            splitMain.Location = new Point(0, 110);
            splitMain.Name = "splitMain";
            // 
            // splitMain.Panel1
            // 
            splitMain.Panel1.BackColor = Color.FromArgb(25, 25, 25);
            splitMain.Panel1.Controls.Add(panelListBox);
            splitMain.Panel1.Controls.Add(lblPanelListHeader);
            splitMain.Panel1MinSize = 150;
            // 
            // splitMain.Panel2
            // 
            splitMain.Panel2.Controls.Add(mainGrid);
            splitMain.Size = new Size(1898, 1118);
            splitMain.SplitterDistance = 180;
            splitMain.SplitterWidth = 3;
            splitMain.TabIndex = 4;
            // 
            // panelListBox
            // 
            panelListBox.BackColor = Color.FromArgb(30, 30, 30);
            panelListBox.BorderStyle = BorderStyle.None;
            panelListBox.Dock = DockStyle.Fill;
            panelListBox.Font = new Font("Segoe UI", 10F);
            panelListBox.ForeColor = Color.Gainsboro;
            panelListBox.Location = new Point(0, 30);
            panelListBox.Name = "panelListBox";
            panelListBox.Size = new Size(180, 1088);
            panelListBox.TabIndex = 1;
            // 
            // lblPanelListHeader
            // 
            lblPanelListHeader.BackColor = Color.FromArgb(35, 35, 35);
            lblPanelListHeader.Dock = DockStyle.Top;
            lblPanelListHeader.Font = new Font("Segoe UI", 10F, FontStyle.Bold);
            lblPanelListHeader.ForeColor = Color.Gainsboro;
            lblPanelListHeader.Location = new Point(0, 0);
            lblPanelListHeader.Name = "lblPanelListHeader";
            lblPanelListHeader.Padding = new Padding(8, 0, 0, 0);
            lblPanelListHeader.Size = new Size(180, 30);
            lblPanelListHeader.TabIndex = 0;
            lblPanelListHeader.Text = "📦 Paneller";
            lblPanelListHeader.TextAlign = ContentAlignment.MiddleLeft;
            // 
            // mainGrid
            // 
            mainGrid.BackColor = Color.FromArgb(20, 20, 20);
            mainGrid.ColumnCount = 3;
            mainGrid.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 33.33333F));
            mainGrid.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 33.33333F));
            mainGrid.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 33.33333F));
            mainGrid.Controls.Add(gbMainScreen, 0, 0);
            mainGrid.Controls.Add(gbCellA, 2, 0);
            mainGrid.Controls.Add(gbCellB, 2, 1);
            mainGrid.Controls.Add(gbCellC, 0, 2);
            mainGrid.Controls.Add(gbCellD, 1, 2);
            mainGrid.Controls.Add(gbCellE, 2, 2);
            mainGrid.Controls.Add(gbCellF, 0, 3);
            mainGrid.Controls.Add(gbCellG, 1, 3);
            mainGrid.Controls.Add(gbCellH, 2, 3);
            mainGrid.Dock = DockStyle.Fill;
            mainGrid.Location = new Point(0, 0);
            mainGrid.Name = "mainGrid";
            mainGrid.Padding = new Padding(6);
            mainGrid.RowCount = 4;
            mainGrid.RowStyles.Add(new RowStyle(SizeType.Percent, 25F));
            mainGrid.RowStyles.Add(new RowStyle(SizeType.Percent, 25F));
            mainGrid.RowStyles.Add(new RowStyle(SizeType.Percent, 25F));
            mainGrid.RowStyles.Add(new RowStyle(SizeType.Percent, 25F));
            mainGrid.Size = new Size(1715, 1118);
            mainGrid.TabIndex = 2;
            // 
            // gbMainScreen
            // 
            gbMainScreen.BackColor = Color.FromArgb(28, 28, 28);
            mainGrid.SetColumnSpan(gbMainScreen, 2);
            gbMainScreen.Dock = DockStyle.Fill;
            gbMainScreen.ForeColor = Color.Gainsboro;
            gbMainScreen.Location = new Point(12, 12);
            gbMainScreen.Margin = new Padding(6);
            gbMainScreen.Name = "gbMainScreen";
            mainGrid.SetRowSpan(gbMainScreen, 2);
            gbMainScreen.Size = new Size(1122, 540);
            gbMainScreen.TabIndex = 0;
            gbMainScreen.TabStop = false;
            gbMainScreen.Text = "Ana Ekran";
            // 
            // gbCellA
            // 
            gbCellA.BackColor = Color.FromArgb(28, 28, 28);
            gbCellA.Dock = DockStyle.Fill;
            gbCellA.ForeColor = Color.Gainsboro;
            gbCellA.Location = new Point(1146, 12);
            gbCellA.Margin = new Padding(6);
            gbCellA.Name = "gbCellA";
            gbCellA.Size = new Size(557, 264);
            gbCellA.TabIndex = 1;
            gbCellA.TabStop = false;
            gbCellA.Text = "Panel A";
            // 
            // gbCellB
            // 
            gbCellB.BackColor = Color.FromArgb(28, 28, 28);
            gbCellB.Dock = DockStyle.Fill;
            gbCellB.ForeColor = Color.Gainsboro;
            gbCellB.Location = new Point(1146, 288);
            gbCellB.Margin = new Padding(6);
            gbCellB.Name = "gbCellB";
            gbCellB.Size = new Size(557, 264);
            gbCellB.TabIndex = 2;
            gbCellB.TabStop = false;
            gbCellB.Text = "Panel B";
            // 
            // gbCellC
            // 
            gbCellC.BackColor = Color.FromArgb(28, 28, 28);
            gbCellC.Dock = DockStyle.Fill;
            gbCellC.ForeColor = Color.Gainsboro;
            gbCellC.Location = new Point(12, 564);
            gbCellC.Margin = new Padding(6);
            gbCellC.Name = "gbCellC";
            gbCellC.Size = new Size(555, 264);
            gbCellC.TabIndex = 3;
            gbCellC.TabStop = false;
            gbCellC.Text = "Panel C";
            // 
            // gbCellD
            // 
            gbCellD.BackColor = Color.FromArgb(28, 28, 28);
            gbCellD.Dock = DockStyle.Fill;
            gbCellD.ForeColor = Color.Gainsboro;
            gbCellD.Location = new Point(579, 564);
            gbCellD.Margin = new Padding(6);
            gbCellD.Name = "gbCellD";
            gbCellD.Size = new Size(555, 264);
            gbCellD.TabIndex = 4;
            gbCellD.TabStop = false;
            gbCellD.Text = "Panel D";
            // 
            // gbCellE
            // 
            gbCellE.BackColor = Color.FromArgb(28, 28, 28);
            gbCellE.Dock = DockStyle.Fill;
            gbCellE.ForeColor = Color.Gainsboro;
            gbCellE.Location = new Point(1146, 564);
            gbCellE.Margin = new Padding(6);
            gbCellE.Name = "gbCellE";
            gbCellE.Size = new Size(557, 264);
            gbCellE.TabIndex = 5;
            gbCellE.TabStop = false;
            gbCellE.Text = "Panel E";
            // 
            // gbCellF
            // 
            gbCellF.BackColor = Color.FromArgb(28, 28, 28);
            gbCellF.Dock = DockStyle.Fill;
            gbCellF.ForeColor = Color.Gainsboro;
            gbCellF.Location = new Point(12, 840);
            gbCellF.Margin = new Padding(6);
            gbCellF.Name = "gbCellF";
            gbCellF.Size = new Size(555, 266);
            gbCellF.TabIndex = 6;
            gbCellF.TabStop = false;
            gbCellF.Text = "Panel F";
            // 
            // gbCellG
            // 
            gbCellG.BackColor = Color.FromArgb(28, 28, 28);
            gbCellG.Dock = DockStyle.Fill;
            gbCellG.ForeColor = Color.Gainsboro;
            gbCellG.Location = new Point(579, 840);
            gbCellG.Margin = new Padding(6);
            gbCellG.Name = "gbCellG";
            gbCellG.Size = new Size(555, 266);
            gbCellG.TabIndex = 7;
            gbCellG.TabStop = false;
            gbCellG.Text = "Panel G";
            // 
            // gbCellH
            // 
            gbCellH.BackColor = Color.FromArgb(28, 28, 28);
            gbCellH.Dock = DockStyle.Fill;
            gbCellH.ForeColor = Color.Gainsboro;
            gbCellH.Location = new Point(1146, 840);
            gbCellH.Margin = new Padding(6);
            gbCellH.Name = "gbCellH";
            gbCellH.Size = new Size(557, 266);
            gbCellH.TabIndex = 8;
            gbCellH.TabStop = false;
            gbCellH.Text = "Panel H";
            // 
            // bottomPanel
            // 
            bottomPanel.BackColor = Color.FromArgb(25, 25, 25);
            bottomPanel.Controls.Add(btnAction2);
            bottomPanel.Controls.Add(btnFilePath);
            bottomPanel.Controls.Add(btnConnect);
            bottomPanel.Controls.Add(panelConnection);
            bottomPanel.Dock = DockStyle.Bottom;
            bottomPanel.Location = new Point(0, 1228);
            bottomPanel.Name = "bottomPanel";
            bottomPanel.Padding = new Padding(12);
            bottomPanel.Size = new Size(1898, 110);
            bottomPanel.TabIndex = 3;
            // 
            // btnAction2
            // 
            btnAction2.Anchor = AnchorStyles.Right;
            btnAction2.BackColor = Color.FromArgb(200, 50, 50);
            btnAction2.FlatAppearance.BorderSize = 0;
            btnAction2.FlatStyle = FlatStyle.Flat;
            btnAction2.ForeColor = Color.White;
            btnAction2.Location = new Point(1723, 30);
            btnAction2.Name = "btnAction2";
            btnAction2.Size = new Size(160, 50);
            btnAction2.TabIndex = 1;
            btnAction2.Text = "🗑️ Dosya Sil";
            btnAction2.UseVisualStyleBackColor = false;
            btnAction2.Click += BtnAction2_Click;
            // 
            // btnFilePath
            // 
            btnFilePath.Anchor = AnchorStyles.Left;
            btnFilePath.BackColor = Color.FromArgb(200, 160, 0);
            btnFilePath.FlatAppearance.BorderColor = Color.FromArgb(255, 200, 0);
            btnFilePath.FlatStyle = FlatStyle.Flat;
            btnFilePath.ForeColor = Color.Black;
            btnFilePath.Location = new Point(420, 30);
            btnFilePath.Name = "btnFilePath";
            btnFilePath.Size = new Size(160, 50);
            btnFilePath.TabIndex = 6;
            btnFilePath.Text = "📁 Kayıt Yeri Seç";
            btnFilePath.UseVisualStyleBackColor = false;
            btnFilePath.Click += BtnFilePath_Click;
            // 
            // Form1
            // 
            AutoScaleDimensions = new SizeF(10F, 25F);
            AutoScaleMode = AutoScaleMode.Font;
            BackColor = Color.FromArgb(18, 18, 18);
            ClientSize = new Size(1898, 1314);
            Controls.Add(splitMain);
            Controls.Add(bottomPanel);
            Controls.Add(panelHeader);
            ForeColor = Color.Gainsboro;
            Name = "Form1";
            Text = "RASAT Yer İstasyonu";
            WindowState = FormWindowState.Maximized;
            panelHeader.ResumeLayout(false);
            telemetryFlowPanel.ResumeLayout(false);
            gbBasinc.ResumeLayout(false);
            gbBasinc.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)pbBasinc).EndInit();
            gbYukseklik.ResumeLayout(false);
            gbYukseklik.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)pbYukseklik).EndInit();
            gbInisHizi.ResumeLayout(false);
            gbInisHizi.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)pbInisHizi).EndInit();
            gbSicaklik.ResumeLayout(false);
            gbSicaklik.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)pbSicaklik).EndInit();
            gbPil.ResumeLayout(false);
            gbPil.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)pbPil).EndInit();
            gbGpsLat.ResumeLayout(false);
            gbGpsLat.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)pbGpsLat).EndInit();
            gbGpsLon.ResumeLayout(false);
            gbGpsLon.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)pbGpsLon).EndInit();
            gbPitch.ResumeLayout(false);
            gbPitch.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)pbPitch).EndInit();
            gbRoll.ResumeLayout(false);
            gbRoll.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)pbRoll).EndInit();
            gbYaw.ResumeLayout(false);
            gbYaw.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)pbYaw).EndInit();
            panelConnection.ResumeLayout(false);
            panelConnection.PerformLayout();
            splitMain.Panel1.ResumeLayout(false);
            splitMain.Panel2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)splitMain).EndInit();
            splitMain.ResumeLayout(false);
            mainGrid.ResumeLayout(false);
            bottomPanel.ResumeLayout(false);
            ResumeLayout(false);
        }

        #endregion

        private Panel panelHeader;
        private FlowLayoutPanel telemetryFlowPanel;
        private GroupBox gbBasinc;
        private PictureBox pbBasinc;
        private Label lblBasincValue;
        private GroupBox gbYukseklik;
        private PictureBox pbYukseklik;
        private Label lblYukseklikValue;
        private GroupBox gbInisHizi;
        private PictureBox pbInisHizi;
        private Label lblInisHiziValue;
        private GroupBox gbSicaklik;
        private PictureBox pbSicaklik;
        private Label lblSicaklikValue;
        private GroupBox gbPil;
        private PictureBox pbPil;
        private Label lblPilValue;
        private GroupBox gbGpsLat;
        private PictureBox pbGpsLat;
        private Label lblGpsLatValue;
        private GroupBox gbGpsLon;
        private PictureBox pbGpsLon;
        private Label lblGpsLonValue;
        private GroupBox gbPitch;
        private PictureBox pbPitch;
        private Label lblPitchValue;
        private GroupBox gbRoll;
        private PictureBox pbRoll;
        private Label lblRollValue;
        private GroupBox gbYaw;
        private PictureBox pbYaw;
        private Label lblYawValue;
        private SplitContainer splitMain;
        private ListBox panelListBox;
        private Label lblPanelListHeader;
        private TableLayoutPanel mainGrid;
        private GroupBox gbMainScreen;
        private GroupBox gbCellA;
        private GroupBox gbCellB;
        private GroupBox gbCellC;
        private GroupBox gbCellD;
        private GroupBox gbCellE;
        private GroupBox gbCellF;
        private GroupBox gbCellG;
        private GroupBox gbCellH;
        private Panel bottomPanel;
        private System.Windows.Forms.Button btnFilePath;
        private System.Windows.Forms.Button btnAction2;
        private System.Windows.Forms.Panel panelConnection;
        private System.Windows.Forms.Label lblTitle;
        private System.Windows.Forms.Label lblCom;
        private System.Windows.Forms.ComboBox cbPorts;
        private System.Windows.Forms.Label lblBaud;
        private System.Windows.Forms.ComboBox cbBaud;
        private System.Windows.Forms.Button btnConnect;
    }
}