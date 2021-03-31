﻿namespace UROVModbus
{
    partial class MainForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
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
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MainForm));
            this.statusBar = new System.Windows.Forms.StatusStrip();
            this.toolStripStatusLabel1 = new System.Windows.Forms.ToolStripStatusLabel();
            this.tabPages = new System.Windows.Forms.TabControl();
            this.tpModbusSettings = new System.Windows.Forms.TabPage();
            this.lblInfoText = new System.Windows.Forms.Label();
            this.GroupBox3 = new System.Windows.Forms.GroupBox();
            this.btnCloseTCP = new System.Windows.Forms.Button();
            this.btnOpenTCP = new System.Windows.Forms.Button();
            this.Label12 = new System.Windows.Forms.Label();
            this.cmbTcpProtocol = new System.Windows.Forms.ComboBox();
            this.Label11 = new System.Windows.Forms.Label();
            this.txtHostName = new System.Windows.Forms.TextBox();
            this.Label10 = new System.Windows.Forms.Label();
            this.txtTCPPort = new System.Windows.Forms.TextBox();
            this.GroupBox2 = new System.Windows.Forms.GroupBox();
            this.btnReloadPorts = new System.Windows.Forms.Button();
            this.FindSerial = new System.Windows.Forms.Button();
            this.btnCloseSerial = new System.Windows.Forms.Button();
            this.cmbDataBits = new System.Windows.Forms.ComboBox();
            this.cmbStopBits = new System.Windows.Forms.ComboBox();
            this.Label8 = new System.Windows.Forms.Label();
            this.Label7 = new System.Windows.Forms.Label();
            this.Label6 = new System.Windows.Forms.Label();
            this.cmbParity = new System.Windows.Forms.ComboBox();
            this.Label3 = new System.Windows.Forms.Label();
            this.Label2 = new System.Windows.Forms.Label();
            this.cmbBaudRate = new System.Windows.Forms.ComboBox();
            this.cmbComPort = new System.Windows.Forms.ComboBox();
            this.Label1 = new System.Windows.Forms.Label();
            this.btnOpenSerial = new System.Windows.Forms.Button();
            this.cmbSerialProtocol = new System.Windows.Forms.ComboBox();
            this.GroupBox1 = new System.Windows.Forms.GroupBox();
            this.cmbRetry = new System.Windows.Forms.ComboBox();
            this.Label4 = new System.Windows.Forms.Label();
            this.Label5 = new System.Windows.Forms.Label();
            this.txtTimeout = new System.Windows.Forms.TextBox();
            this.txtPollDelay = new System.Windows.Forms.TextBox();
            this.Label9 = new System.Windows.Forms.Label();
            this.tpUROVSettings = new System.Windows.Forms.TabPage();
            this.btnFileList = new System.Windows.Forms.Button();
            this.nudModbusSlaveID = new System.Windows.Forms.NumericUpDown();
            this.label29 = new System.Windows.Forms.Label();
            this.btnReadRegisters = new System.Windows.Forms.Button();
            this.panel46 = new System.Windows.Forms.Panel();
            this.panel47 = new System.Windows.Forms.Panel();
            this.cbAsuTpLine4 = new System.Windows.Forms.CheckBox();
            this.cbAsuTpLine3 = new System.Windows.Forms.CheckBox();
            this.cbAsuTpLine2 = new System.Windows.Forms.CheckBox();
            this.cbAsuTpLine1 = new System.Windows.Forms.CheckBox();
            this.label73 = new System.Windows.Forms.Label();
            this.panel48 = new System.Windows.Forms.Panel();
            this.label74 = new System.Windows.Forms.Label();
            this.btnCurrentCoeff = new System.Windows.Forms.Button();
            this.panel43 = new System.Windows.Forms.Panel();
            this.panel44 = new System.Windows.Forms.Panel();
            this.label70 = new System.Windows.Forms.Label();
            this.nudCurrentCoeff = new System.Windows.Forms.NumericUpDown();
            this.label71 = new System.Windows.Forms.Label();
            this.panel45 = new System.Windows.Forms.Panel();
            this.label72 = new System.Windows.Forms.Label();
            this.panel31 = new System.Windows.Forms.Panel();
            this.panel32 = new System.Windows.Forms.Panel();
            this.nudMaxIdleTime = new System.Windows.Forms.NumericUpDown();
            this.label28 = new System.Windows.Forms.Label();
            this.label53 = new System.Windows.Forms.Label();
            this.nudRelayDelay = new System.Windows.Forms.NumericUpDown();
            this.label55 = new System.Windows.Forms.Label();
            this.panel33 = new System.Windows.Forms.Panel();
            this.label56 = new System.Windows.Forms.Label();
            this.panel28 = new System.Windows.Forms.Panel();
            this.panel29 = new System.Windows.Forms.Panel();
            this.label49 = new System.Windows.Forms.Label();
            this.nudHighBorder = new System.Windows.Forms.NumericUpDown();
            this.nudLowBorder = new System.Windows.Forms.NumericUpDown();
            this.label50 = new System.Windows.Forms.Label();
            this.label51 = new System.Windows.Forms.Label();
            this.panel30 = new System.Windows.Forms.Panel();
            this.label52 = new System.Windows.Forms.Label();
            this.panel2 = new System.Windows.Forms.Panel();
            this.panel7 = new System.Windows.Forms.Panel();
            this.label25 = new System.Windows.Forms.Label();
            this.nudRodMoveLength = new System.Windows.Forms.NumericUpDown();
            this.label26 = new System.Windows.Forms.Label();
            this.panel8 = new System.Windows.Forms.Panel();
            this.label27 = new System.Windows.Forms.Label();
            this.plImpulses = new System.Windows.Forms.Panel();
            this.panel9 = new System.Windows.Forms.Panel();
            this.nudDelta1 = new System.Windows.Forms.NumericUpDown();
            this.label21 = new System.Windows.Forms.Label();
            this.label22 = new System.Windows.Forms.Label();
            this.nudPulses1 = new System.Windows.Forms.NumericUpDown();
            this.label23 = new System.Windows.Forms.Label();
            this.panel10 = new System.Windows.Forms.Panel();
            this.label24 = new System.Windows.Forms.Label();
            this.plMotoresourceCurrent = new System.Windows.Forms.Panel();
            this.panel5 = new System.Windows.Forms.Panel();
            this.label16 = new System.Windows.Forms.Label();
            this.lblMotoresourcePercents1 = new System.Windows.Forms.Label();
            this.nudMotoresourceMax1 = new System.Windows.Forms.NumericUpDown();
            this.label17 = new System.Windows.Forms.Label();
            this.nudMotoresourceCurrent1 = new System.Windows.Forms.NumericUpDown();
            this.label18 = new System.Windows.Forms.Label();
            this.panel6 = new System.Windows.Forms.Panel();
            this.label19 = new System.Windows.Forms.Label();
            this.plDelta = new System.Windows.Forms.Panel();
            this.panel11 = new System.Windows.Forms.Panel();
            this.nudEthalonCompareDelta = new System.Windows.Forms.NumericUpDown();
            this.label13 = new System.Windows.Forms.Label();
            this.label14 = new System.Windows.Forms.Label();
            this.nudSkipCounter = new System.Windows.Forms.NumericUpDown();
            this.label15 = new System.Windows.Forms.Label();
            this.panel12 = new System.Windows.Forms.Panel();
            this.label20 = new System.Windows.Forms.Label();
            this.tmCheckConnectTimer = new System.Windows.Forms.Timer(this.components);
            this.tmFileList = new System.Windows.Forms.Timer(this.components);
            this.tmFileContent = new System.Windows.Forms.Timer(this.components);
            this.statusBar.SuspendLayout();
            this.tabPages.SuspendLayout();
            this.tpModbusSettings.SuspendLayout();
            this.GroupBox3.SuspendLayout();
            this.GroupBox2.SuspendLayout();
            this.GroupBox1.SuspendLayout();
            this.tpUROVSettings.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.nudModbusSlaveID)).BeginInit();
            this.panel46.SuspendLayout();
            this.panel47.SuspendLayout();
            this.panel48.SuspendLayout();
            this.panel43.SuspendLayout();
            this.panel44.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.nudCurrentCoeff)).BeginInit();
            this.panel45.SuspendLayout();
            this.panel31.SuspendLayout();
            this.panel32.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.nudMaxIdleTime)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudRelayDelay)).BeginInit();
            this.panel33.SuspendLayout();
            this.panel28.SuspendLayout();
            this.panel29.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.nudHighBorder)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudLowBorder)).BeginInit();
            this.panel30.SuspendLayout();
            this.panel2.SuspendLayout();
            this.panel7.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.nudRodMoveLength)).BeginInit();
            this.panel8.SuspendLayout();
            this.plImpulses.SuspendLayout();
            this.panel9.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.nudDelta1)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudPulses1)).BeginInit();
            this.panel10.SuspendLayout();
            this.plMotoresourceCurrent.SuspendLayout();
            this.panel5.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.nudMotoresourceMax1)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudMotoresourceCurrent1)).BeginInit();
            this.panel6.SuspendLayout();
            this.plDelta.SuspendLayout();
            this.panel11.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.nudEthalonCompareDelta)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudSkipCounter)).BeginInit();
            this.panel12.SuspendLayout();
            this.SuspendLayout();
            // 
            // statusBar
            // 
            this.statusBar.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripStatusLabel1});
            this.statusBar.Location = new System.Drawing.Point(0, 613);
            this.statusBar.Name = "statusBar";
            this.statusBar.Size = new System.Drawing.Size(861, 22);
            this.statusBar.TabIndex = 0;
            this.statusBar.Text = "statusStrip1";
            // 
            // toolStripStatusLabel1
            // 
            this.toolStripStatusLabel1.Name = "toolStripStatusLabel1";
            this.toolStripStatusLabel1.Size = new System.Drawing.Size(52, 17);
            this.toolStripStatusLabel1.Text = "СТАТУС";
            // 
            // tabPages
            // 
            this.tabPages.Controls.Add(this.tpModbusSettings);
            this.tabPages.Controls.Add(this.tpUROVSettings);
            this.tabPages.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tabPages.Location = new System.Drawing.Point(0, 0);
            this.tabPages.Name = "tabPages";
            this.tabPages.SelectedIndex = 0;
            this.tabPages.Size = new System.Drawing.Size(861, 613);
            this.tabPages.TabIndex = 1;
            // 
            // tpModbusSettings
            // 
            this.tpModbusSettings.Controls.Add(this.lblInfoText);
            this.tpModbusSettings.Controls.Add(this.GroupBox3);
            this.tpModbusSettings.Controls.Add(this.GroupBox2);
            this.tpModbusSettings.Controls.Add(this.GroupBox1);
            this.tpModbusSettings.Location = new System.Drawing.Point(4, 22);
            this.tpModbusSettings.Name = "tpModbusSettings";
            this.tpModbusSettings.Padding = new System.Windows.Forms.Padding(3);
            this.tpModbusSettings.Size = new System.Drawing.Size(853, 587);
            this.tpModbusSettings.TabIndex = 0;
            this.tpModbusSettings.Text = "Соединение";
            this.tpModbusSettings.UseVisualStyleBackColor = true;
            // 
            // lblInfoText
            // 
            this.lblInfoText.BackColor = System.Drawing.SystemColors.Info;
            this.lblInfoText.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.lblInfoText.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.lblInfoText.Location = new System.Drawing.Point(28, 434);
            this.lblInfoText.Name = "lblInfoText";
            this.lblInfoText.Padding = new System.Windows.Forms.Padding(6);
            this.lblInfoText.Size = new System.Drawing.Size(798, 54);
            this.lblInfoText.TabIndex = 17;
            // 
            // GroupBox3
            // 
            this.GroupBox3.Controls.Add(this.btnCloseTCP);
            this.GroupBox3.Controls.Add(this.btnOpenTCP);
            this.GroupBox3.Controls.Add(this.Label12);
            this.GroupBox3.Controls.Add(this.cmbTcpProtocol);
            this.GroupBox3.Controls.Add(this.Label11);
            this.GroupBox3.Controls.Add(this.txtHostName);
            this.GroupBox3.Controls.Add(this.Label10);
            this.GroupBox3.Controls.Add(this.txtTCPPort);
            this.GroupBox3.Location = new System.Drawing.Point(345, 22);
            this.GroupBox3.Name = "GroupBox3";
            this.GroupBox3.Size = new System.Drawing.Size(237, 400);
            this.GroupBox3.TabIndex = 16;
            this.GroupBox3.TabStop = false;
            this.GroupBox3.Text = "MODBUS/TCP";
            // 
            // btnCloseTCP
            // 
            this.btnCloseTCP.Enabled = false;
            this.btnCloseTCP.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.btnCloseTCP.Location = new System.Drawing.Point(15, 249);
            this.btnCloseTCP.Name = "btnCloseTCP";
            this.btnCloseTCP.Size = new System.Drawing.Size(207, 40);
            this.btnCloseTCP.TabIndex = 16;
            this.btnCloseTCP.Text = "Закрыть TCP";
            this.btnCloseTCP.Click += new System.EventHandler(this.btnCloseTCP_Click);
            // 
            // btnOpenTCP
            // 
            this.btnOpenTCP.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.btnOpenTCP.Location = new System.Drawing.Point(15, 203);
            this.btnOpenTCP.Name = "btnOpenTCP";
            this.btnOpenTCP.Size = new System.Drawing.Size(207, 40);
            this.btnOpenTCP.TabIndex = 15;
            this.btnOpenTCP.Text = "Открыть TCP";
            this.btnOpenTCP.Click += new System.EventHandler(this.btnOpenTCP_Click);
            // 
            // Label12
            // 
            this.Label12.Location = new System.Drawing.Point(12, 83);
            this.Label12.Name = "Label12";
            this.Label12.Size = new System.Drawing.Size(91, 18);
            this.Label12.TabIndex = 14;
            this.Label12.Text = "Протокол:";
            // 
            // cmbTcpProtocol
            // 
            this.cmbTcpProtocol.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cmbTcpProtocol.Items.AddRange(new object[] {
            "Modbus/TCP",
            "Encapsulated RTU"});
            this.cmbTcpProtocol.Location = new System.Drawing.Point(109, 83);
            this.cmbTcpProtocol.Name = "cmbTcpProtocol";
            this.cmbTcpProtocol.Size = new System.Drawing.Size(115, 21);
            this.cmbTcpProtocol.TabIndex = 13;
            // 
            // Label11
            // 
            this.Label11.Location = new System.Drawing.Point(12, 54);
            this.Label11.Name = "Label11";
            this.Label11.Size = new System.Drawing.Size(91, 20);
            this.Label11.TabIndex = 12;
            this.Label11.Text = "IP-адрес:";
            // 
            // txtHostName
            // 
            this.txtHostName.Location = new System.Drawing.Point(109, 54);
            this.txtHostName.Name = "txtHostName";
            this.txtHostName.Size = new System.Drawing.Size(113, 20);
            this.txtHostName.TabIndex = 11;
            this.txtHostName.Text = "127.0.0.1";
            // 
            // Label10
            // 
            this.Label10.Location = new System.Drawing.Point(12, 24);
            this.Label10.Name = "Label10";
            this.Label10.Size = new System.Drawing.Size(91, 20);
            this.Label10.TabIndex = 10;
            this.Label10.Text = "TCP-порт:";
            // 
            // txtTCPPort
            // 
            this.txtTCPPort.Location = new System.Drawing.Point(109, 24);
            this.txtTCPPort.Name = "txtTCPPort";
            this.txtTCPPort.Size = new System.Drawing.Size(113, 20);
            this.txtTCPPort.TabIndex = 9;
            this.txtTCPPort.Text = "502";
            // 
            // GroupBox2
            // 
            this.GroupBox2.Controls.Add(this.btnReloadPorts);
            this.GroupBox2.Controls.Add(this.FindSerial);
            this.GroupBox2.Controls.Add(this.btnCloseSerial);
            this.GroupBox2.Controls.Add(this.cmbDataBits);
            this.GroupBox2.Controls.Add(this.cmbStopBits);
            this.GroupBox2.Controls.Add(this.Label8);
            this.GroupBox2.Controls.Add(this.Label7);
            this.GroupBox2.Controls.Add(this.Label6);
            this.GroupBox2.Controls.Add(this.cmbParity);
            this.GroupBox2.Controls.Add(this.Label3);
            this.GroupBox2.Controls.Add(this.Label2);
            this.GroupBox2.Controls.Add(this.cmbBaudRate);
            this.GroupBox2.Controls.Add(this.cmbComPort);
            this.GroupBox2.Controls.Add(this.Label1);
            this.GroupBox2.Controls.Add(this.btnOpenSerial);
            this.GroupBox2.Controls.Add(this.cmbSerialProtocol);
            this.GroupBox2.Location = new System.Drawing.Point(31, 22);
            this.GroupBox2.Name = "GroupBox2";
            this.GroupBox2.Size = new System.Drawing.Size(300, 400);
            this.GroupBox2.TabIndex = 15;
            this.GroupBox2.TabStop = false;
            this.GroupBox2.Text = "Serial Modbus";
            // 
            // btnReloadPorts
            // 
            this.btnReloadPorts.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.btnReloadPorts.Location = new System.Drawing.Point(19, 344);
            this.btnReloadPorts.Name = "btnReloadPorts";
            this.btnReloadPorts.Size = new System.Drawing.Size(265, 40);
            this.btnReloadPorts.TabIndex = 16;
            this.btnReloadPorts.Text = "Перечитать порты";
            this.btnReloadPorts.Click += new System.EventHandler(this.btnReloadPorts_Click);
            // 
            // FindSerial
            // 
            this.FindSerial.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.FindSerial.Location = new System.Drawing.Point(19, 249);
            this.FindSerial.Name = "FindSerial";
            this.FindSerial.Size = new System.Drawing.Size(265, 40);
            this.FindSerial.TabIndex = 15;
            this.FindSerial.Text = "Поиск Serial";
            this.FindSerial.Click += new System.EventHandler(this.FindSerial_Click);
            // 
            // btnCloseSerial
            // 
            this.btnCloseSerial.Enabled = false;
            this.btnCloseSerial.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.btnCloseSerial.Location = new System.Drawing.Point(154, 203);
            this.btnCloseSerial.Name = "btnCloseSerial";
            this.btnCloseSerial.Size = new System.Drawing.Size(130, 40);
            this.btnCloseSerial.TabIndex = 14;
            this.btnCloseSerial.Text = "Закрыть Serial";
            this.btnCloseSerial.Click += new System.EventHandler(this.btnCloseSerial_Click);
            // 
            // cmbDataBits
            // 
            this.cmbDataBits.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cmbDataBits.Items.AddRange(new object[] {
            "8",
            "7"});
            this.cmbDataBits.Location = new System.Drawing.Point(135, 134);
            this.cmbDataBits.Name = "cmbDataBits";
            this.cmbDataBits.Size = new System.Drawing.Size(149, 21);
            this.cmbDataBits.TabIndex = 13;
            // 
            // cmbStopBits
            // 
            this.cmbStopBits.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cmbStopBits.Items.AddRange(new object[] {
            "1",
            "2"});
            this.cmbStopBits.Location = new System.Drawing.Point(135, 161);
            this.cmbStopBits.Name = "cmbStopBits";
            this.cmbStopBits.Size = new System.Drawing.Size(149, 21);
            this.cmbStopBits.TabIndex = 12;
            // 
            // Label8
            // 
            this.Label8.Location = new System.Drawing.Point(16, 135);
            this.Label8.Name = "Label8";
            this.Label8.Size = new System.Drawing.Size(113, 18);
            this.Label8.TabIndex = 11;
            this.Label8.Text = "Биты данных:";
            // 
            // Label7
            // 
            this.Label7.Location = new System.Drawing.Point(16, 162);
            this.Label7.Name = "Label7";
            this.Label7.Size = new System.Drawing.Size(113, 18);
            this.Label7.TabIndex = 10;
            this.Label7.Text = "Стоп-биты:";
            // 
            // Label6
            // 
            this.Label6.Location = new System.Drawing.Point(16, 108);
            this.Label6.Name = "Label6";
            this.Label6.Size = new System.Drawing.Size(113, 18);
            this.Label6.TabIndex = 9;
            this.Label6.Text = "Чётность:";
            // 
            // cmbParity
            // 
            this.cmbParity.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cmbParity.Items.AddRange(new object[] {
            "None",
            "Even",
            "Odd"});
            this.cmbParity.Location = new System.Drawing.Point(135, 107);
            this.cmbParity.Name = "cmbParity";
            this.cmbParity.Size = new System.Drawing.Size(149, 21);
            this.cmbParity.TabIndex = 8;
            // 
            // Label3
            // 
            this.Label3.Location = new System.Drawing.Point(16, 81);
            this.Label3.Name = "Label3";
            this.Label3.Size = new System.Drawing.Size(113, 18);
            this.Label3.TabIndex = 7;
            this.Label3.Text = "Протокол:";
            // 
            // Label2
            // 
            this.Label2.Location = new System.Drawing.Point(16, 54);
            this.Label2.Name = "Label2";
            this.Label2.Size = new System.Drawing.Size(113, 18);
            this.Label2.TabIndex = 5;
            this.Label2.Text = "Скорость:";
            // 
            // cmbBaudRate
            // 
            this.cmbBaudRate.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cmbBaudRate.Items.AddRange(new object[] {
            "9600",
            "14400",
            "19200",
            "38400",
            "57600",
            "115200",
            "125000",
            "128000",
            "256000"});
            this.cmbBaudRate.Location = new System.Drawing.Point(135, 53);
            this.cmbBaudRate.Name = "cmbBaudRate";
            this.cmbBaudRate.Size = new System.Drawing.Size(149, 21);
            this.cmbBaudRate.TabIndex = 4;
            // 
            // cmbComPort
            // 
            this.cmbComPort.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cmbComPort.Location = new System.Drawing.Point(135, 26);
            this.cmbComPort.Name = "cmbComPort";
            this.cmbComPort.Size = new System.Drawing.Size(149, 21);
            this.cmbComPort.TabIndex = 3;
            // 
            // Label1
            // 
            this.Label1.Location = new System.Drawing.Point(16, 27);
            this.Label1.Name = "Label1";
            this.Label1.Size = new System.Drawing.Size(113, 18);
            this.Label1.TabIndex = 1;
            this.Label1.Text = "COM-порт:";
            // 
            // btnOpenSerial
            // 
            this.btnOpenSerial.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.btnOpenSerial.Location = new System.Drawing.Point(19, 203);
            this.btnOpenSerial.Name = "btnOpenSerial";
            this.btnOpenSerial.Size = new System.Drawing.Size(130, 40);
            this.btnOpenSerial.TabIndex = 2;
            this.btnOpenSerial.Text = "Открыть Serial";
            this.btnOpenSerial.Click += new System.EventHandler(this.cmdOpenSerial_Click);
            // 
            // cmbSerialProtocol
            // 
            this.cmbSerialProtocol.DisplayMember = "1";
            this.cmbSerialProtocol.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cmbSerialProtocol.Items.AddRange(new object[] {
            "RTU",
            "ASCII"});
            this.cmbSerialProtocol.Location = new System.Drawing.Point(135, 80);
            this.cmbSerialProtocol.Name = "cmbSerialProtocol";
            this.cmbSerialProtocol.Size = new System.Drawing.Size(149, 21);
            this.cmbSerialProtocol.TabIndex = 6;
            // 
            // GroupBox1
            // 
            this.GroupBox1.Controls.Add(this.cmbRetry);
            this.GroupBox1.Controls.Add(this.Label4);
            this.GroupBox1.Controls.Add(this.Label5);
            this.GroupBox1.Controls.Add(this.txtTimeout);
            this.GroupBox1.Controls.Add(this.txtPollDelay);
            this.GroupBox1.Controls.Add(this.Label9);
            this.GroupBox1.Location = new System.Drawing.Point(598, 22);
            this.GroupBox1.Name = "GroupBox1";
            this.GroupBox1.Size = new System.Drawing.Size(228, 400);
            this.GroupBox1.TabIndex = 14;
            this.GroupBox1.TabStop = false;
            this.GroupBox1.Text = "Опции MODBUS";
            // 
            // cmbRetry
            // 
            this.cmbRetry.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cmbRetry.Items.AddRange(new object[] {
            "0",
            "1",
            "2",
            "3"});
            this.cmbRetry.Location = new System.Drawing.Point(142, 56);
            this.cmbRetry.Name = "cmbRetry";
            this.cmbRetry.Size = new System.Drawing.Size(66, 21);
            this.cmbRetry.TabIndex = 9;
            // 
            // Label4
            // 
            this.Label4.Location = new System.Drawing.Point(20, 27);
            this.Label4.Name = "Label4";
            this.Label4.Size = new System.Drawing.Size(116, 20);
            this.Label4.TabIndex = 8;
            this.Label4.Text = "Таймаут:";
            // 
            // Label5
            // 
            this.Label5.Location = new System.Drawing.Point(20, 61);
            this.Label5.Name = "Label5";
            this.Label5.Size = new System.Drawing.Size(116, 20);
            this.Label5.TabIndex = 10;
            this.Label5.Text = "Кол-во попыток:";
            // 
            // txtTimeout
            // 
            this.txtTimeout.Location = new System.Drawing.Point(142, 24);
            this.txtTimeout.Name = "txtTimeout";
            this.txtTimeout.Size = new System.Drawing.Size(66, 20);
            this.txtTimeout.TabIndex = 0;
            this.txtTimeout.Text = "1000";
            // 
            // txtPollDelay
            // 
            this.txtPollDelay.Location = new System.Drawing.Point(142, 90);
            this.txtPollDelay.Name = "txtPollDelay";
            this.txtPollDelay.Size = new System.Drawing.Size(66, 20);
            this.txtPollDelay.TabIndex = 11;
            this.txtPollDelay.Text = "0";
            // 
            // Label9
            // 
            this.Label9.Location = new System.Drawing.Point(20, 93);
            this.Label9.Name = "Label9";
            this.Label9.Size = new System.Drawing.Size(116, 20);
            this.Label9.TabIndex = 12;
            this.Label9.Text = "Задержка выборки:";
            // 
            // tpUROVSettings
            // 
            this.tpUROVSettings.Controls.Add(this.btnFileList);
            this.tpUROVSettings.Controls.Add(this.nudModbusSlaveID);
            this.tpUROVSettings.Controls.Add(this.label29);
            this.tpUROVSettings.Controls.Add(this.btnReadRegisters);
            this.tpUROVSettings.Controls.Add(this.panel46);
            this.tpUROVSettings.Controls.Add(this.btnCurrentCoeff);
            this.tpUROVSettings.Controls.Add(this.panel43);
            this.tpUROVSettings.Controls.Add(this.panel31);
            this.tpUROVSettings.Controls.Add(this.panel28);
            this.tpUROVSettings.Controls.Add(this.panel2);
            this.tpUROVSettings.Controls.Add(this.plImpulses);
            this.tpUROVSettings.Controls.Add(this.plMotoresourceCurrent);
            this.tpUROVSettings.Controls.Add(this.plDelta);
            this.tpUROVSettings.Location = new System.Drawing.Point(4, 22);
            this.tpUROVSettings.Name = "tpUROVSettings";
            this.tpUROVSettings.Padding = new System.Windows.Forms.Padding(3);
            this.tpUROVSettings.Size = new System.Drawing.Size(853, 587);
            this.tpUROVSettings.TabIndex = 1;
            this.tpUROVSettings.Text = "Настройки UROV";
            this.tpUROVSettings.UseVisualStyleBackColor = true;
            // 
            // btnFileList
            // 
            this.btnFileList.BackColor = System.Drawing.SystemColors.ButtonFace;
            this.btnFileList.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnFileList.ForeColor = System.Drawing.Color.Black;
            this.btnFileList.Location = new System.Drawing.Point(221, 524);
            this.btnFileList.Name = "btnFileList";
            this.btnFileList.Size = new System.Drawing.Size(200, 46);
            this.btnFileList.TabIndex = 17;
            this.btnFileList.Text = "Список файлов";
            this.btnFileList.UseVisualStyleBackColor = false;
            this.btnFileList.Click += new System.EventHandler(this.btnFileList_Click);
            // 
            // nudModbusSlaveID
            // 
            this.nudModbusSlaveID.Location = new System.Drawing.Point(12, 539);
            this.nudModbusSlaveID.Maximum = new decimal(new int[] {
            247,
            0,
            0,
            0});
            this.nudModbusSlaveID.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.nudModbusSlaveID.Name = "nudModbusSlaveID";
            this.nudModbusSlaveID.Size = new System.Drawing.Size(98, 20);
            this.nudModbusSlaveID.TabIndex = 16;
            this.nudModbusSlaveID.Value = new decimal(new int[] {
            100,
            0,
            0,
            0});
            // 
            // label29
            // 
            this.label29.AutoSize = true;
            this.label29.ForeColor = System.Drawing.Color.Black;
            this.label29.Location = new System.Drawing.Point(9, 524);
            this.label29.Name = "label29";
            this.label29.Size = new System.Drawing.Size(83, 13);
            this.label29.TabIndex = 15;
            this.label29.Text = "Номер слейва:";
            // 
            // btnReadRegisters
            // 
            this.btnReadRegisters.BackColor = System.Drawing.Color.LightSalmon;
            this.btnReadRegisters.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnReadRegisters.ForeColor = System.Drawing.Color.Black;
            this.btnReadRegisters.Location = new System.Drawing.Point(432, 524);
            this.btnReadRegisters.Name = "btnReadRegisters";
            this.btnReadRegisters.Size = new System.Drawing.Size(200, 46);
            this.btnReadRegisters.TabIndex = 14;
            this.btnReadRegisters.Text = "Прочитать настройки";
            this.btnReadRegisters.UseVisualStyleBackColor = false;
            this.btnReadRegisters.Click += new System.EventHandler(this.btnReadRegisters_Click);
            // 
            // panel46
            // 
            this.panel46.BackColor = System.Drawing.Color.SteelBlue;
            this.panel46.Controls.Add(this.panel47);
            this.panel46.Controls.Add(this.panel48);
            this.panel46.Location = new System.Drawing.Point(640, 265);
            this.panel46.Margin = new System.Windows.Forms.Padding(10);
            this.panel46.Name = "panel46";
            this.panel46.Padding = new System.Windows.Forms.Padding(2);
            this.panel46.Size = new System.Drawing.Size(200, 248);
            this.panel46.TabIndex = 13;
            // 
            // panel47
            // 
            this.panel47.BackColor = System.Drawing.Color.White;
            this.panel47.Controls.Add(this.cbAsuTpLine4);
            this.panel47.Controls.Add(this.cbAsuTpLine3);
            this.panel47.Controls.Add(this.cbAsuTpLine2);
            this.panel47.Controls.Add(this.cbAsuTpLine1);
            this.panel47.Controls.Add(this.label73);
            this.panel47.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panel47.Location = new System.Drawing.Point(2, 26);
            this.panel47.Name = "panel47";
            this.panel47.Size = new System.Drawing.Size(196, 220);
            this.panel47.TabIndex = 2;
            // 
            // cbAsuTpLine4
            // 
            this.cbAsuTpLine4.AutoSize = true;
            this.cbAsuTpLine4.Location = new System.Drawing.Point(16, 80);
            this.cbAsuTpLine4.Name = "cbAsuTpLine4";
            this.cbAsuTpLine4.Size = new System.Drawing.Size(78, 17);
            this.cbAsuTpLine4.TabIndex = 13;
            this.cbAsuTpLine4.Text = "Линия №4";
            this.cbAsuTpLine4.UseVisualStyleBackColor = true;
            // 
            // cbAsuTpLine3
            // 
            this.cbAsuTpLine3.AutoSize = true;
            this.cbAsuTpLine3.Location = new System.Drawing.Point(16, 57);
            this.cbAsuTpLine3.Name = "cbAsuTpLine3";
            this.cbAsuTpLine3.Size = new System.Drawing.Size(78, 17);
            this.cbAsuTpLine3.TabIndex = 12;
            this.cbAsuTpLine3.Text = "Линия №3";
            this.cbAsuTpLine3.UseVisualStyleBackColor = true;
            // 
            // cbAsuTpLine2
            // 
            this.cbAsuTpLine2.AutoSize = true;
            this.cbAsuTpLine2.Location = new System.Drawing.Point(16, 34);
            this.cbAsuTpLine2.Name = "cbAsuTpLine2";
            this.cbAsuTpLine2.Size = new System.Drawing.Size(78, 17);
            this.cbAsuTpLine2.TabIndex = 11;
            this.cbAsuTpLine2.Text = "Линия №2";
            this.cbAsuTpLine2.UseVisualStyleBackColor = true;
            // 
            // cbAsuTpLine1
            // 
            this.cbAsuTpLine1.AutoSize = true;
            this.cbAsuTpLine1.Location = new System.Drawing.Point(16, 11);
            this.cbAsuTpLine1.Name = "cbAsuTpLine1";
            this.cbAsuTpLine1.Size = new System.Drawing.Size(78, 17);
            this.cbAsuTpLine1.TabIndex = 10;
            this.cbAsuTpLine1.Text = "Линия №1";
            this.cbAsuTpLine1.UseVisualStyleBackColor = true;
            // 
            // label73
            // 
            this.label73.BackColor = System.Drawing.Color.LightYellow;
            this.label73.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.label73.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Italic, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.label73.ForeColor = System.Drawing.Color.Black;
            this.label73.Location = new System.Drawing.Point(13, 120);
            this.label73.Name = "label73";
            this.label73.Padding = new System.Windows.Forms.Padding(4);
            this.label73.Size = new System.Drawing.Size(170, 82);
            this.label73.TabIndex = 9;
            this.label73.Text = "Флаги выдачи сигналов на линии АСУ ТП при срабатывании системы. Если галочка стои" +
    "т - сигнал выдаётся.";
            // 
            // panel48
            // 
            this.panel48.AutoSize = true;
            this.panel48.BackColor = System.Drawing.Color.SteelBlue;
            this.panel48.Controls.Add(this.label74);
            this.panel48.Dock = System.Windows.Forms.DockStyle.Top;
            this.panel48.Location = new System.Drawing.Point(2, 2);
            this.panel48.Margin = new System.Windows.Forms.Padding(0);
            this.panel48.Name = "panel48";
            this.panel48.Size = new System.Drawing.Size(196, 24);
            this.panel48.TabIndex = 1;
            // 
            // label74
            // 
            this.label74.AutoSize = true;
            this.label74.Dock = System.Windows.Forms.DockStyle.Top;
            this.label74.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.label74.ForeColor = System.Drawing.Color.White;
            this.label74.Location = new System.Drawing.Point(0, 0);
            this.label74.Margin = new System.Windows.Forms.Padding(0);
            this.label74.Name = "label74";
            this.label74.Padding = new System.Windows.Forms.Padding(2);
            this.label74.Size = new System.Drawing.Size(179, 24);
            this.label74.TabIndex = 0;
            this.label74.Text = "Сигналы на АСУ ТП";
            // 
            // btnCurrentCoeff
            // 
            this.btnCurrentCoeff.BackColor = System.Drawing.Color.LightGreen;
            this.btnCurrentCoeff.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnCurrentCoeff.ForeColor = System.Drawing.Color.Black;
            this.btnCurrentCoeff.Location = new System.Drawing.Point(640, 524);
            this.btnCurrentCoeff.Name = "btnCurrentCoeff";
            this.btnCurrentCoeff.Size = new System.Drawing.Size(200, 46);
            this.btnCurrentCoeff.TabIndex = 12;
            this.btnCurrentCoeff.Text = "Установить настройки";
            this.btnCurrentCoeff.UseVisualStyleBackColor = false;
            this.btnCurrentCoeff.Click += new System.EventHandler(this.btnCurrentCoeff_Click);
            // 
            // panel43
            // 
            this.panel43.BackColor = System.Drawing.Color.SteelBlue;
            this.panel43.Controls.Add(this.panel44);
            this.panel43.Controls.Add(this.panel45);
            this.panel43.Location = new System.Drawing.Point(432, 265);
            this.panel43.Margin = new System.Windows.Forms.Padding(10);
            this.panel43.Name = "panel43";
            this.panel43.Padding = new System.Windows.Forms.Padding(2);
            this.panel43.Size = new System.Drawing.Size(200, 246);
            this.panel43.TabIndex = 11;
            // 
            // panel44
            // 
            this.panel44.BackColor = System.Drawing.Color.White;
            this.panel44.Controls.Add(this.label70);
            this.panel44.Controls.Add(this.nudCurrentCoeff);
            this.panel44.Controls.Add(this.label71);
            this.panel44.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panel44.Location = new System.Drawing.Point(2, 26);
            this.panel44.Name = "panel44";
            this.panel44.Size = new System.Drawing.Size(196, 218);
            this.panel44.TabIndex = 2;
            // 
            // label70
            // 
            this.label70.BackColor = System.Drawing.Color.LightYellow;
            this.label70.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.label70.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Italic, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.label70.ForeColor = System.Drawing.Color.Black;
            this.label70.Location = new System.Drawing.Point(13, 120);
            this.label70.Name = "label70";
            this.label70.Padding = new System.Windows.Forms.Padding(4);
            this.label70.Size = new System.Drawing.Size(170, 82);
            this.label70.TabIndex = 9;
            this.label70.Text = "Коэффициент пересчёта тока, в тысячных долях (например, 1234 - это 1.234).";
            // 
            // nudCurrentCoeff
            // 
            this.nudCurrentCoeff.Location = new System.Drawing.Point(16, 27);
            this.nudCurrentCoeff.Maximum = new decimal(new int[] {
            100000,
            0,
            0,
            0});
            this.nudCurrentCoeff.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.nudCurrentCoeff.Name = "nudCurrentCoeff";
            this.nudCurrentCoeff.Size = new System.Drawing.Size(167, 20);
            this.nudCurrentCoeff.TabIndex = 5;
            this.nudCurrentCoeff.Value = new decimal(new int[] {
            1,
            0,
            0,
            0});
            // 
            // label71
            // 
            this.label71.AutoSize = true;
            this.label71.ForeColor = System.Drawing.Color.Black;
            this.label71.Location = new System.Drawing.Point(13, 11);
            this.label71.Name = "label71";
            this.label71.Size = new System.Drawing.Size(80, 13);
            this.label71.TabIndex = 0;
            this.label71.Text = "Коэффициент:";
            // 
            // panel45
            // 
            this.panel45.AutoSize = true;
            this.panel45.BackColor = System.Drawing.Color.SteelBlue;
            this.panel45.Controls.Add(this.label72);
            this.panel45.Dock = System.Windows.Forms.DockStyle.Top;
            this.panel45.Location = new System.Drawing.Point(2, 2);
            this.panel45.Margin = new System.Windows.Forms.Padding(0);
            this.panel45.Name = "panel45";
            this.panel45.Size = new System.Drawing.Size(196, 24);
            this.panel45.TabIndex = 1;
            // 
            // label72
            // 
            this.label72.AutoSize = true;
            this.label72.Dock = System.Windows.Forms.DockStyle.Top;
            this.label72.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.label72.ForeColor = System.Drawing.Color.White;
            this.label72.Location = new System.Drawing.Point(0, 0);
            this.label72.Margin = new System.Windows.Forms.Padding(0);
            this.label72.Name = "label72";
            this.label72.Padding = new System.Windows.Forms.Padding(2);
            this.label72.Size = new System.Drawing.Size(180, 24);
            this.label72.TabIndex = 0;
            this.label72.Text = "Коэффициент тока";
            // 
            // panel31
            // 
            this.panel31.BackColor = System.Drawing.Color.SteelBlue;
            this.panel31.Controls.Add(this.panel32);
            this.panel31.Controls.Add(this.panel33);
            this.panel31.Location = new System.Drawing.Point(221, 265);
            this.panel31.Margin = new System.Windows.Forms.Padding(10);
            this.panel31.Name = "panel31";
            this.panel31.Padding = new System.Windows.Forms.Padding(2);
            this.panel31.Size = new System.Drawing.Size(200, 248);
            this.panel31.TabIndex = 10;
            // 
            // panel32
            // 
            this.panel32.BackColor = System.Drawing.Color.White;
            this.panel32.Controls.Add(this.nudMaxIdleTime);
            this.panel32.Controls.Add(this.label28);
            this.panel32.Controls.Add(this.label53);
            this.panel32.Controls.Add(this.nudRelayDelay);
            this.panel32.Controls.Add(this.label55);
            this.panel32.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panel32.Location = new System.Drawing.Point(2, 26);
            this.panel32.Name = "panel32";
            this.panel32.Size = new System.Drawing.Size(196, 220);
            this.panel32.TabIndex = 2;
            // 
            // nudMaxIdleTime
            // 
            this.nudMaxIdleTime.Location = new System.Drawing.Point(16, 70);
            this.nudMaxIdleTime.Maximum = new decimal(new int[] {
            1000000000,
            0,
            0,
            0});
            this.nudMaxIdleTime.Name = "nudMaxIdleTime";
            this.nudMaxIdleTime.Size = new System.Drawing.Size(167, 20);
            this.nudMaxIdleTime.TabIndex = 14;
            // 
            // label28
            // 
            this.label28.AutoSize = true;
            this.label28.ForeColor = System.Drawing.Color.Black;
            this.label28.Location = new System.Drawing.Point(13, 54);
            this.label28.Name = "label28";
            this.label28.Size = new System.Drawing.Size(159, 13);
            this.label28.TabIndex = 13;
            this.label28.Text = "Пауза завершения ожидания:";
            // 
            // label53
            // 
            this.label53.BackColor = System.Drawing.Color.LightYellow;
            this.label53.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.label53.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Italic, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.label53.ForeColor = System.Drawing.Color.Black;
            this.label53.Location = new System.Drawing.Point(13, 120);
            this.label53.Name = "label53";
            this.label53.Padding = new System.Windows.Forms.Padding(4);
            this.label53.Size = new System.Drawing.Size(170, 82);
            this.label53.TabIndex = 12;
            this.label53.Text = "Время ожидания импульсов при превышении тока, мс. Время ожидания завершения импул" +
    "ьсов, мкс.";
            // 
            // nudRelayDelay
            // 
            this.nudRelayDelay.Location = new System.Drawing.Point(16, 27);
            this.nudRelayDelay.Maximum = new decimal(new int[] {
            10000,
            0,
            0,
            0});
            this.nudRelayDelay.Name = "nudRelayDelay";
            this.nudRelayDelay.Size = new System.Drawing.Size(167, 20);
            this.nudRelayDelay.TabIndex = 5;
            // 
            // label55
            // 
            this.label55.AutoSize = true;
            this.label55.ForeColor = System.Drawing.Color.Black;
            this.label55.Location = new System.Drawing.Point(13, 11);
            this.label55.Name = "label55";
            this.label55.Size = new System.Drawing.Size(153, 13);
            this.label55.TabIndex = 0;
            this.label55.Text = "Ожидание данных энкодера:";
            // 
            // panel33
            // 
            this.panel33.AutoSize = true;
            this.panel33.BackColor = System.Drawing.Color.SteelBlue;
            this.panel33.Controls.Add(this.label56);
            this.panel33.Dock = System.Windows.Forms.DockStyle.Top;
            this.panel33.Location = new System.Drawing.Point(2, 2);
            this.panel33.Margin = new System.Windows.Forms.Padding(0);
            this.panel33.Name = "panel33";
            this.panel33.Size = new System.Drawing.Size(196, 24);
            this.panel33.TabIndex = 1;
            // 
            // label56
            // 
            this.label56.AutoSize = true;
            this.label56.Dock = System.Windows.Forms.DockStyle.Top;
            this.label56.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.label56.ForeColor = System.Drawing.Color.White;
            this.label56.Location = new System.Drawing.Point(0, 0);
            this.label56.Margin = new System.Windows.Forms.Padding(0);
            this.label56.Name = "label56";
            this.label56.Padding = new System.Windows.Forms.Padding(2);
            this.label56.Size = new System.Drawing.Size(98, 24);
            this.label56.TabIndex = 0;
            this.label56.Text = "Задержки";
            // 
            // panel28
            // 
            this.panel28.BackColor = System.Drawing.Color.SteelBlue;
            this.panel28.Controls.Add(this.panel29);
            this.panel28.Controls.Add(this.panel30);
            this.panel28.Location = new System.Drawing.Point(10, 265);
            this.panel28.Margin = new System.Windows.Forms.Padding(10);
            this.panel28.Name = "panel28";
            this.panel28.Padding = new System.Windows.Forms.Padding(2);
            this.panel28.Size = new System.Drawing.Size(200, 248);
            this.panel28.TabIndex = 9;
            // 
            // panel29
            // 
            this.panel29.BackColor = System.Drawing.Color.White;
            this.panel29.Controls.Add(this.label49);
            this.panel29.Controls.Add(this.nudHighBorder);
            this.panel29.Controls.Add(this.nudLowBorder);
            this.panel29.Controls.Add(this.label50);
            this.panel29.Controls.Add(this.label51);
            this.panel29.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panel29.Location = new System.Drawing.Point(2, 26);
            this.panel29.Name = "panel29";
            this.panel29.Size = new System.Drawing.Size(196, 220);
            this.panel29.TabIndex = 2;
            // 
            // label49
            // 
            this.label49.BackColor = System.Drawing.Color.LightYellow;
            this.label49.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.label49.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Italic, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.label49.ForeColor = System.Drawing.Color.Black;
            this.label49.Location = new System.Drawing.Point(13, 119);
            this.label49.Name = "label49";
            this.label49.Padding = new System.Windows.Forms.Padding(4);
            this.label49.Size = new System.Drawing.Size(170, 82);
            this.label49.TabIndex = 13;
            this.label49.Text = "Верхний и нижний пороги АЦП, пока используется только верхний.";
            // 
            // nudHighBorder
            // 
            this.nudHighBorder.Location = new System.Drawing.Point(16, 26);
            this.nudHighBorder.Maximum = new decimal(new int[] {
            4096,
            0,
            0,
            0});
            this.nudHighBorder.Name = "nudHighBorder";
            this.nudHighBorder.Size = new System.Drawing.Size(167, 20);
            this.nudHighBorder.TabIndex = 6;
            // 
            // nudLowBorder
            // 
            this.nudLowBorder.Location = new System.Drawing.Point(16, 70);
            this.nudLowBorder.Maximum = new decimal(new int[] {
            4096,
            0,
            0,
            0});
            this.nudLowBorder.Name = "nudLowBorder";
            this.nudLowBorder.Size = new System.Drawing.Size(167, 20);
            this.nudLowBorder.TabIndex = 5;
            // 
            // label50
            // 
            this.label50.AutoSize = true;
            this.label50.ForeColor = System.Drawing.Color.Black;
            this.label50.Location = new System.Drawing.Point(13, 11);
            this.label50.Name = "label50";
            this.label50.Size = new System.Drawing.Size(52, 13);
            this.label50.TabIndex = 2;
            this.label50.Text = "Верхний:";
            // 
            // label51
            // 
            this.label51.AutoSize = true;
            this.label51.ForeColor = System.Drawing.Color.Black;
            this.label51.Location = new System.Drawing.Point(13, 54);
            this.label51.Name = "label51";
            this.label51.Size = new System.Drawing.Size(50, 13);
            this.label51.TabIndex = 0;
            this.label51.Text = "Нижний:";
            // 
            // panel30
            // 
            this.panel30.AutoSize = true;
            this.panel30.BackColor = System.Drawing.Color.SteelBlue;
            this.panel30.Controls.Add(this.label52);
            this.panel30.Dock = System.Windows.Forms.DockStyle.Top;
            this.panel30.Location = new System.Drawing.Point(2, 2);
            this.panel30.Margin = new System.Windows.Forms.Padding(0);
            this.panel30.Name = "panel30";
            this.panel30.Size = new System.Drawing.Size(196, 24);
            this.panel30.TabIndex = 1;
            // 
            // label52
            // 
            this.label52.AutoSize = true;
            this.label52.Dock = System.Windows.Forms.DockStyle.Top;
            this.label52.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.label52.ForeColor = System.Drawing.Color.White;
            this.label52.Location = new System.Drawing.Point(0, 0);
            this.label52.Margin = new System.Windows.Forms.Padding(0);
            this.label52.Name = "label52";
            this.label52.Padding = new System.Windows.Forms.Padding(2);
            this.label52.Size = new System.Drawing.Size(125, 24);
            this.label52.TabIndex = 0;
            this.label52.Text = "Пороги тр-ра";
            // 
            // panel2
            // 
            this.panel2.BackColor = System.Drawing.Color.SteelBlue;
            this.panel2.Controls.Add(this.panel7);
            this.panel2.Controls.Add(this.panel8);
            this.panel2.Location = new System.Drawing.Point(640, 10);
            this.panel2.Margin = new System.Windows.Forms.Padding(10);
            this.panel2.Name = "panel2";
            this.panel2.Padding = new System.Windows.Forms.Padding(2);
            this.panel2.Size = new System.Drawing.Size(200, 244);
            this.panel2.TabIndex = 8;
            // 
            // panel7
            // 
            this.panel7.BackColor = System.Drawing.Color.White;
            this.panel7.Controls.Add(this.label25);
            this.panel7.Controls.Add(this.nudRodMoveLength);
            this.panel7.Controls.Add(this.label26);
            this.panel7.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panel7.Location = new System.Drawing.Point(2, 26);
            this.panel7.Name = "panel7";
            this.panel7.Size = new System.Drawing.Size(196, 216);
            this.panel7.TabIndex = 2;
            // 
            // label25
            // 
            this.label25.BackColor = System.Drawing.Color.LightYellow;
            this.label25.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.label25.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Italic, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.label25.ForeColor = System.Drawing.Color.Black;
            this.label25.Location = new System.Drawing.Point(13, 105);
            this.label25.Name = "label25";
            this.label25.Padding = new System.Windows.Forms.Padding(4);
            this.label25.Size = new System.Drawing.Size(170, 97);
            this.label25.TabIndex = 11;
            this.label25.Text = "Величина перемещения привода штанги, мм.";
            // 
            // nudRodMoveLength
            // 
            this.nudRodMoveLength.Location = new System.Drawing.Point(16, 27);
            this.nudRodMoveLength.Maximum = new decimal(new int[] {
            1000,
            0,
            0,
            0});
            this.nudRodMoveLength.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.nudRodMoveLength.Name = "nudRodMoveLength";
            this.nudRodMoveLength.Size = new System.Drawing.Size(167, 20);
            this.nudRodMoveLength.TabIndex = 10;
            this.nudRodMoveLength.Value = new decimal(new int[] {
            1,
            0,
            0,
            0});
            // 
            // label26
            // 
            this.label26.AutoSize = true;
            this.label26.ForeColor = System.Drawing.Color.Black;
            this.label26.Location = new System.Drawing.Point(13, 11);
            this.label26.Name = "label26";
            this.label26.Size = new System.Drawing.Size(154, 13);
            this.label26.TabIndex = 9;
            this.label26.Text = "Величина перемещения, мм:";
            // 
            // panel8
            // 
            this.panel8.AutoSize = true;
            this.panel8.BackColor = System.Drawing.Color.SteelBlue;
            this.panel8.Controls.Add(this.label27);
            this.panel8.Dock = System.Windows.Forms.DockStyle.Top;
            this.panel8.Location = new System.Drawing.Point(2, 2);
            this.panel8.Margin = new System.Windows.Forms.Padding(0);
            this.panel8.Name = "panel8";
            this.panel8.Size = new System.Drawing.Size(196, 24);
            this.panel8.TabIndex = 1;
            // 
            // label27
            // 
            this.label27.AutoSize = true;
            this.label27.Dock = System.Windows.Forms.DockStyle.Top;
            this.label27.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.label27.ForeColor = System.Drawing.Color.White;
            this.label27.Location = new System.Drawing.Point(0, 0);
            this.label27.Margin = new System.Windows.Forms.Padding(0);
            this.label27.Name = "label27";
            this.label27.Padding = new System.Windows.Forms.Padding(2);
            this.label27.Size = new System.Drawing.Size(171, 24);
            this.label27.TabIndex = 0;
            this.label27.Text = "Настройки штанги";
            // 
            // plImpulses
            // 
            this.plImpulses.BackColor = System.Drawing.Color.SteelBlue;
            this.plImpulses.Controls.Add(this.panel9);
            this.plImpulses.Controls.Add(this.panel10);
            this.plImpulses.Location = new System.Drawing.Point(432, 10);
            this.plImpulses.Margin = new System.Windows.Forms.Padding(10);
            this.plImpulses.Name = "plImpulses";
            this.plImpulses.Padding = new System.Windows.Forms.Padding(2);
            this.plImpulses.Size = new System.Drawing.Size(200, 244);
            this.plImpulses.TabIndex = 7;
            // 
            // panel9
            // 
            this.panel9.BackColor = System.Drawing.Color.White;
            this.panel9.Controls.Add(this.nudDelta1);
            this.panel9.Controls.Add(this.label21);
            this.panel9.Controls.Add(this.label22);
            this.panel9.Controls.Add(this.nudPulses1);
            this.panel9.Controls.Add(this.label23);
            this.panel9.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panel9.Location = new System.Drawing.Point(2, 26);
            this.panel9.Name = "panel9";
            this.panel9.Size = new System.Drawing.Size(196, 216);
            this.panel9.TabIndex = 2;
            // 
            // nudDelta1
            // 
            this.nudDelta1.Location = new System.Drawing.Point(16, 70);
            this.nudDelta1.Maximum = new decimal(new int[] {
            255,
            0,
            0,
            0});
            this.nudDelta1.Name = "nudDelta1";
            this.nudDelta1.Size = new System.Drawing.Size(167, 20);
            this.nudDelta1.TabIndex = 11;
            // 
            // label21
            // 
            this.label21.AutoSize = true;
            this.label21.ForeColor = System.Drawing.Color.Black;
            this.label21.Location = new System.Drawing.Point(13, 54);
            this.label21.Name = "label21";
            this.label21.Size = new System.Drawing.Size(84, 13);
            this.label21.TabIndex = 10;
            this.label21.Text = "Дельта кол-ва:";
            // 
            // label22
            // 
            this.label22.BackColor = System.Drawing.Color.LightYellow;
            this.label22.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.label22.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Italic, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.label22.ForeColor = System.Drawing.Color.Black;
            this.label22.Location = new System.Drawing.Point(13, 105);
            this.label22.Name = "label22";
            this.label22.Padding = new System.Windows.Forms.Padding(4);
            this.label22.Size = new System.Drawing.Size(170, 97);
            this.label22.TabIndex = 9;
            this.label22.Text = "Образцовое количество импульсов со штанги, для сравнения с графиком эталона. Дель" +
    "та - отклонение от образца.";
            // 
            // nudPulses1
            // 
            this.nudPulses1.Location = new System.Drawing.Point(16, 27);
            this.nudPulses1.Maximum = new decimal(new int[] {
            65535,
            0,
            0,
            0});
            this.nudPulses1.Name = "nudPulses1";
            this.nudPulses1.Size = new System.Drawing.Size(167, 20);
            this.nudPulses1.TabIndex = 5;
            // 
            // label23
            // 
            this.label23.AutoSize = true;
            this.label23.ForeColor = System.Drawing.Color.Black;
            this.label23.Location = new System.Drawing.Point(13, 11);
            this.label23.Name = "label23";
            this.label23.Size = new System.Drawing.Size(100, 13);
            this.label23.TabIndex = 0;
            this.label23.Text = "Эталонное кол-во:";
            // 
            // panel10
            // 
            this.panel10.AutoSize = true;
            this.panel10.BackColor = System.Drawing.Color.SteelBlue;
            this.panel10.Controls.Add(this.label24);
            this.panel10.Dock = System.Windows.Forms.DockStyle.Top;
            this.panel10.Location = new System.Drawing.Point(2, 2);
            this.panel10.Margin = new System.Windows.Forms.Padding(0);
            this.panel10.Name = "panel10";
            this.panel10.Size = new System.Drawing.Size(196, 24);
            this.panel10.TabIndex = 1;
            // 
            // label24
            // 
            this.label24.AutoSize = true;
            this.label24.Dock = System.Windows.Forms.DockStyle.Top;
            this.label24.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.label24.ForeColor = System.Drawing.Color.White;
            this.label24.Location = new System.Drawing.Point(0, 0);
            this.label24.Margin = new System.Windows.Forms.Padding(0);
            this.label24.Name = "label24";
            this.label24.Padding = new System.Windows.Forms.Padding(2);
            this.label24.Size = new System.Drawing.Size(97, 24);
            this.label24.TabIndex = 0;
            this.label24.Text = "Импульсы";
            // 
            // plMotoresourceCurrent
            // 
            this.plMotoresourceCurrent.BackColor = System.Drawing.Color.SteelBlue;
            this.plMotoresourceCurrent.Controls.Add(this.panel5);
            this.plMotoresourceCurrent.Controls.Add(this.panel6);
            this.plMotoresourceCurrent.Location = new System.Drawing.Point(221, 10);
            this.plMotoresourceCurrent.Margin = new System.Windows.Forms.Padding(10);
            this.plMotoresourceCurrent.Name = "plMotoresourceCurrent";
            this.plMotoresourceCurrent.Padding = new System.Windows.Forms.Padding(2);
            this.plMotoresourceCurrent.Size = new System.Drawing.Size(200, 244);
            this.plMotoresourceCurrent.TabIndex = 6;
            // 
            // panel5
            // 
            this.panel5.BackColor = System.Drawing.Color.White;
            this.panel5.Controls.Add(this.label16);
            this.panel5.Controls.Add(this.lblMotoresourcePercents1);
            this.panel5.Controls.Add(this.nudMotoresourceMax1);
            this.panel5.Controls.Add(this.label17);
            this.panel5.Controls.Add(this.nudMotoresourceCurrent1);
            this.panel5.Controls.Add(this.label18);
            this.panel5.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panel5.Location = new System.Drawing.Point(2, 26);
            this.panel5.Name = "panel5";
            this.panel5.Size = new System.Drawing.Size(196, 216);
            this.panel5.TabIndex = 2;
            // 
            // label16
            // 
            this.label16.BackColor = System.Drawing.Color.LightYellow;
            this.label16.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.label16.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Italic, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.label16.ForeColor = System.Drawing.Color.Black;
            this.label16.Location = new System.Drawing.Point(13, 105);
            this.label16.Name = "label16";
            this.label16.Padding = new System.Windows.Forms.Padding(4);
            this.label16.Size = new System.Drawing.Size(170, 97);
            this.label16.TabIndex = 12;
            this.label16.Text = "Показывает, сколько прибор отработал от максимального ресурса. Его можно установи" +
    "ть на нужное значение.\r\n";
            // 
            // lblMotoresourcePercents1
            // 
            this.lblMotoresourcePercents1.BackColor = System.Drawing.Color.White;
            this.lblMotoresourcePercents1.ForeColor = System.Drawing.Color.Green;
            this.lblMotoresourcePercents1.Location = new System.Drawing.Point(108, 27);
            this.lblMotoresourcePercents1.Name = "lblMotoresourcePercents1";
            this.lblMotoresourcePercents1.Size = new System.Drawing.Size(75, 13);
            this.lblMotoresourcePercents1.TabIndex = 9;
            this.lblMotoresourcePercents1.Text = "0%";
            this.lblMotoresourcePercents1.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // nudMotoresourceMax1
            // 
            this.nudMotoresourceMax1.Location = new System.Drawing.Point(19, 70);
            this.nudMotoresourceMax1.Maximum = new decimal(new int[] {
            1410065408,
            2,
            0,
            0});
            this.nudMotoresourceMax1.Name = "nudMotoresourceMax1";
            this.nudMotoresourceMax1.Size = new System.Drawing.Size(167, 20);
            this.nudMotoresourceMax1.TabIndex = 5;
            // 
            // label17
            // 
            this.label17.AutoSize = true;
            this.label17.ForeColor = System.Drawing.Color.Black;
            this.label17.Location = new System.Drawing.Point(16, 56);
            this.label17.Name = "label17";
            this.label17.Size = new System.Drawing.Size(78, 13);
            this.label17.TabIndex = 0;
            this.label17.Text = "Макс. ресурс:";
            // 
            // nudMotoresourceCurrent1
            // 
            this.nudMotoresourceCurrent1.Location = new System.Drawing.Point(19, 25);
            this.nudMotoresourceCurrent1.Maximum = new decimal(new int[] {
            1410065408,
            2,
            0,
            0});
            this.nudMotoresourceCurrent1.Name = "nudMotoresourceCurrent1";
            this.nudMotoresourceCurrent1.Size = new System.Drawing.Size(86, 20);
            this.nudMotoresourceCurrent1.TabIndex = 5;
            // 
            // label18
            // 
            this.label18.AutoSize = true;
            this.label18.ForeColor = System.Drawing.Color.Black;
            this.label18.Location = new System.Drawing.Point(16, 11);
            this.label18.Name = "label18";
            this.label18.Size = new System.Drawing.Size(123, 13);
            this.label18.TabIndex = 0;
            this.label18.Text = "Наработанный ресурс:";
            // 
            // panel6
            // 
            this.panel6.AutoSize = true;
            this.panel6.BackColor = System.Drawing.Color.SteelBlue;
            this.panel6.Controls.Add(this.label19);
            this.panel6.Dock = System.Windows.Forms.DockStyle.Top;
            this.panel6.Location = new System.Drawing.Point(2, 2);
            this.panel6.Margin = new System.Windows.Forms.Padding(0);
            this.panel6.Name = "panel6";
            this.panel6.Size = new System.Drawing.Size(196, 24);
            this.panel6.TabIndex = 1;
            // 
            // label19
            // 
            this.label19.AutoSize = true;
            this.label19.Dock = System.Windows.Forms.DockStyle.Top;
            this.label19.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.label19.ForeColor = System.Drawing.Color.White;
            this.label19.Location = new System.Drawing.Point(0, 0);
            this.label19.Margin = new System.Windows.Forms.Padding(0);
            this.label19.Name = "label19";
            this.label19.Padding = new System.Windows.Forms.Padding(2);
            this.label19.Size = new System.Drawing.Size(113, 24);
            this.label19.TabIndex = 0;
            this.label19.Text = "Моторесурс";
            // 
            // plDelta
            // 
            this.plDelta.BackColor = System.Drawing.Color.SteelBlue;
            this.plDelta.Controls.Add(this.panel11);
            this.plDelta.Controls.Add(this.panel12);
            this.plDelta.Location = new System.Drawing.Point(10, 10);
            this.plDelta.Margin = new System.Windows.Forms.Padding(10);
            this.plDelta.Name = "plDelta";
            this.plDelta.Padding = new System.Windows.Forms.Padding(2);
            this.plDelta.Size = new System.Drawing.Size(200, 244);
            this.plDelta.TabIndex = 5;
            // 
            // panel11
            // 
            this.panel11.BackColor = System.Drawing.Color.White;
            this.panel11.Controls.Add(this.nudEthalonCompareDelta);
            this.panel11.Controls.Add(this.label13);
            this.panel11.Controls.Add(this.label14);
            this.panel11.Controls.Add(this.nudSkipCounter);
            this.panel11.Controls.Add(this.label15);
            this.panel11.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panel11.Location = new System.Drawing.Point(2, 26);
            this.panel11.Name = "panel11";
            this.panel11.Size = new System.Drawing.Size(196, 216);
            this.panel11.TabIndex = 2;
            // 
            // nudEthalonCompareDelta
            // 
            this.nudEthalonCompareDelta.Location = new System.Drawing.Point(16, 70);
            this.nudEthalonCompareDelta.Maximum = new decimal(new int[] {
            10000,
            0,
            0,
            0});
            this.nudEthalonCompareDelta.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.nudEthalonCompareDelta.Name = "nudEthalonCompareDelta";
            this.nudEthalonCompareDelta.Size = new System.Drawing.Size(167, 20);
            this.nudEthalonCompareDelta.TabIndex = 13;
            this.nudEthalonCompareDelta.Value = new decimal(new int[] {
            50,
            0,
            0,
            0});
            // 
            // label13
            // 
            this.label13.AutoSize = true;
            this.label13.ForeColor = System.Drawing.Color.Black;
            this.label13.Location = new System.Drawing.Point(13, 54);
            this.label13.Name = "label13";
            this.label13.Size = new System.Drawing.Size(152, 13);
            this.label13.TabIndex = 12;
            this.label13.Text = "Дельта времени сравнения:";
            // 
            // label14
            // 
            this.label14.BackColor = System.Drawing.Color.LightYellow;
            this.label14.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.label14.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Italic, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.label14.ForeColor = System.Drawing.Color.Black;
            this.label14.Location = new System.Drawing.Point(13, 105);
            this.label14.Name = "label14";
            this.label14.Padding = new System.Windows.Forms.Padding(4);
            this.label14.Size = new System.Drawing.Size(170, 97);
            this.label14.TabIndex = 11;
            this.label14.Text = "\"Пропускать...\" - какой по счёту импульс будет учитываться (1 - каждый, 2 - кажды" +
    "й второй, и т.п.). \"Дельта\" - отклонение в мкс при сравнении с эталоном.";
            // 
            // nudSkipCounter
            // 
            this.nudSkipCounter.Location = new System.Drawing.Point(16, 27);
            this.nudSkipCounter.Maximum = new decimal(new int[] {
            255,
            0,
            0,
            0});
            this.nudSkipCounter.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.nudSkipCounter.Name = "nudSkipCounter";
            this.nudSkipCounter.Size = new System.Drawing.Size(167, 20);
            this.nudSkipCounter.TabIndex = 10;
            this.nudSkipCounter.Value = new decimal(new int[] {
            1,
            0,
            0,
            0});
            // 
            // label15
            // 
            this.label15.AutoSize = true;
            this.label15.ForeColor = System.Drawing.Color.Black;
            this.label15.Location = new System.Drawing.Point(13, 11);
            this.label15.Name = "label15";
            this.label15.Size = new System.Drawing.Size(128, 13);
            this.label15.TabIndex = 9;
            this.label15.Text = "Пропускать импульсов:";
            // 
            // panel12
            // 
            this.panel12.AutoSize = true;
            this.panel12.BackColor = System.Drawing.Color.SteelBlue;
            this.panel12.Controls.Add(this.label20);
            this.panel12.Dock = System.Windows.Forms.DockStyle.Top;
            this.panel12.Location = new System.Drawing.Point(2, 2);
            this.panel12.Margin = new System.Windows.Forms.Padding(0);
            this.panel12.Name = "panel12";
            this.panel12.Size = new System.Drawing.Size(196, 24);
            this.panel12.TabIndex = 1;
            // 
            // label20
            // 
            this.label20.AutoSize = true;
            this.label20.Dock = System.Windows.Forms.DockStyle.Top;
            this.label20.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.label20.ForeColor = System.Drawing.Color.White;
            this.label20.Location = new System.Drawing.Point(0, 0);
            this.label20.Margin = new System.Windows.Forms.Padding(0);
            this.label20.Name = "label20";
            this.label20.Padding = new System.Windows.Forms.Padding(2);
            this.label20.Size = new System.Drawing.Size(174, 24);
            this.label20.TabIndex = 0;
            this.label20.Text = "Дельты импульсов";
            // 
            // tmCheckConnectTimer
            // 
            this.tmCheckConnectTimer.Interval = 800;
            this.tmCheckConnectTimer.Tick += new System.EventHandler(this.tmCheckConnectTimer_Tick);
            // 
            // tmFileList
            // 
            this.tmFileList.Tick += new System.EventHandler(this.tmFileList_Tick);
            // 
            // tmFileContent
            // 
            this.tmFileContent.Tick += new System.EventHandler(this.tmFileContent_Tick);
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(861, 635);
            this.Controls.Add(this.tabPages);
            this.Controls.Add(this.statusBar);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "MainForm";
            this.SizeGripStyle = System.Windows.Forms.SizeGripStyle.Hide;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "UROV через MODBUS";
            this.FormClosed += new System.Windows.Forms.FormClosedEventHandler(this.MainForm_FormClosed);
            this.Load += new System.EventHandler(this.mainForm_Load);
            this.statusBar.ResumeLayout(false);
            this.statusBar.PerformLayout();
            this.tabPages.ResumeLayout(false);
            this.tpModbusSettings.ResumeLayout(false);
            this.GroupBox3.ResumeLayout(false);
            this.GroupBox3.PerformLayout();
            this.GroupBox2.ResumeLayout(false);
            this.GroupBox1.ResumeLayout(false);
            this.GroupBox1.PerformLayout();
            this.tpUROVSettings.ResumeLayout(false);
            this.tpUROVSettings.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.nudModbusSlaveID)).EndInit();
            this.panel46.ResumeLayout(false);
            this.panel46.PerformLayout();
            this.panel47.ResumeLayout(false);
            this.panel47.PerformLayout();
            this.panel48.ResumeLayout(false);
            this.panel48.PerformLayout();
            this.panel43.ResumeLayout(false);
            this.panel43.PerformLayout();
            this.panel44.ResumeLayout(false);
            this.panel44.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.nudCurrentCoeff)).EndInit();
            this.panel45.ResumeLayout(false);
            this.panel45.PerformLayout();
            this.panel31.ResumeLayout(false);
            this.panel31.PerformLayout();
            this.panel32.ResumeLayout(false);
            this.panel32.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.nudMaxIdleTime)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudRelayDelay)).EndInit();
            this.panel33.ResumeLayout(false);
            this.panel33.PerformLayout();
            this.panel28.ResumeLayout(false);
            this.panel28.PerformLayout();
            this.panel29.ResumeLayout(false);
            this.panel29.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.nudHighBorder)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudLowBorder)).EndInit();
            this.panel30.ResumeLayout(false);
            this.panel30.PerformLayout();
            this.panel2.ResumeLayout(false);
            this.panel2.PerformLayout();
            this.panel7.ResumeLayout(false);
            this.panel7.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.nudRodMoveLength)).EndInit();
            this.panel8.ResumeLayout(false);
            this.panel8.PerformLayout();
            this.plImpulses.ResumeLayout(false);
            this.plImpulses.PerformLayout();
            this.panel9.ResumeLayout(false);
            this.panel9.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.nudDelta1)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudPulses1)).EndInit();
            this.panel10.ResumeLayout(false);
            this.panel10.PerformLayout();
            this.plMotoresourceCurrent.ResumeLayout(false);
            this.plMotoresourceCurrent.PerformLayout();
            this.panel5.ResumeLayout(false);
            this.panel5.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.nudMotoresourceMax1)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudMotoresourceCurrent1)).EndInit();
            this.panel6.ResumeLayout(false);
            this.panel6.PerformLayout();
            this.plDelta.ResumeLayout(false);
            this.plDelta.PerformLayout();
            this.panel11.ResumeLayout(false);
            this.panel11.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.nudEthalonCompareDelta)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudSkipCounter)).EndInit();
            this.panel12.ResumeLayout(false);
            this.panel12.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.StatusStrip statusBar;
        private System.Windows.Forms.TabControl tabPages;
        private System.Windows.Forms.TabPage tpModbusSettings;
        private System.Windows.Forms.TabPage tpUROVSettings;
        private System.Windows.Forms.GroupBox GroupBox3;
        private System.Windows.Forms.Button btnCloseTCP;
        private System.Windows.Forms.Button btnOpenTCP;
        private System.Windows.Forms.Label Label12;
        private System.Windows.Forms.ComboBox cmbTcpProtocol;
        private System.Windows.Forms.Label Label11;
        private System.Windows.Forms.TextBox txtHostName;
        private System.Windows.Forms.Label Label10;
        private System.Windows.Forms.TextBox txtTCPPort;
        private System.Windows.Forms.GroupBox GroupBox2;
        private System.Windows.Forms.Button FindSerial;
        private System.Windows.Forms.Button btnCloseSerial;
        private System.Windows.Forms.ComboBox cmbDataBits;
        private System.Windows.Forms.ComboBox cmbStopBits;
        private System.Windows.Forms.Label Label8;
        private System.Windows.Forms.Label Label7;
        private System.Windows.Forms.Label Label6;
        private System.Windows.Forms.ComboBox cmbParity;
        private System.Windows.Forms.Label Label3;
        private System.Windows.Forms.Label Label2;
        private System.Windows.Forms.ComboBox cmbBaudRate;
        private System.Windows.Forms.ComboBox cmbComPort;
        private System.Windows.Forms.Label Label1;
        private System.Windows.Forms.Button btnOpenSerial;
        private System.Windows.Forms.ComboBox cmbSerialProtocol;
        private System.Windows.Forms.GroupBox GroupBox1;
        private System.Windows.Forms.ComboBox cmbRetry;
        private System.Windows.Forms.Label Label4;
        private System.Windows.Forms.Label Label5;
        private System.Windows.Forms.TextBox txtTimeout;
        private System.Windows.Forms.TextBox txtPollDelay;
        private System.Windows.Forms.Label Label9;
        private System.Windows.Forms.Button btnReloadPorts;
        private System.Windows.Forms.Label lblInfoText;
        private System.Windows.Forms.Timer tmCheckConnectTimer;
        private System.Windows.Forms.Panel plMotoresourceCurrent;
        private System.Windows.Forms.Panel panel5;
        private System.Windows.Forms.Label label16;
        private System.Windows.Forms.Label lblMotoresourcePercents1;
        private System.Windows.Forms.Label label17;
        private System.Windows.Forms.NumericUpDown nudMotoresourceCurrent1;
        private System.Windows.Forms.Label label18;
        private System.Windows.Forms.Panel panel6;
        private System.Windows.Forms.Label label19;
        private System.Windows.Forms.Panel plDelta;
        private System.Windows.Forms.Panel panel11;
        private System.Windows.Forms.NumericUpDown nudEthalonCompareDelta;
        private System.Windows.Forms.Label label13;
        private System.Windows.Forms.Label label14;
        private System.Windows.Forms.NumericUpDown nudSkipCounter;
        private System.Windows.Forms.Label label15;
        private System.Windows.Forms.Panel panel12;
        private System.Windows.Forms.Label label20;
        private System.Windows.Forms.Panel plImpulses;
        private System.Windows.Forms.Panel panel9;
        private System.Windows.Forms.NumericUpDown nudDelta1;
        private System.Windows.Forms.Label label21;
        private System.Windows.Forms.Label label22;
        private System.Windows.Forms.NumericUpDown nudPulses1;
        private System.Windows.Forms.Label label23;
        private System.Windows.Forms.Panel panel10;
        private System.Windows.Forms.Label label24;
        private System.Windows.Forms.Panel panel2;
        private System.Windows.Forms.Panel panel7;
        private System.Windows.Forms.Label label25;
        private System.Windows.Forms.NumericUpDown nudRodMoveLength;
        private System.Windows.Forms.Label label26;
        private System.Windows.Forms.Panel panel8;
        private System.Windows.Forms.Label label27;
        private System.Windows.Forms.Panel panel28;
        private System.Windows.Forms.Panel panel29;
        private System.Windows.Forms.Label label49;
        private System.Windows.Forms.NumericUpDown nudHighBorder;
        private System.Windows.Forms.NumericUpDown nudLowBorder;
        private System.Windows.Forms.Label label50;
        private System.Windows.Forms.Label label51;
        private System.Windows.Forms.Panel panel30;
        private System.Windows.Forms.Label label52;
        private System.Windows.Forms.Panel panel31;
        private System.Windows.Forms.Panel panel32;
        private System.Windows.Forms.NumericUpDown nudMaxIdleTime;
        private System.Windows.Forms.Label label28;
        private System.Windows.Forms.Label label53;
        private System.Windows.Forms.NumericUpDown nudRelayDelay;
        private System.Windows.Forms.Label label55;
        private System.Windows.Forms.Panel panel33;
        private System.Windows.Forms.Label label56;
        private System.Windows.Forms.Button btnCurrentCoeff;
        private System.Windows.Forms.Panel panel43;
        private System.Windows.Forms.Panel panel44;
        private System.Windows.Forms.Label label70;
        private System.Windows.Forms.NumericUpDown nudCurrentCoeff;
        private System.Windows.Forms.Label label71;
        private System.Windows.Forms.Panel panel45;
        private System.Windows.Forms.Label label72;
        private System.Windows.Forms.Panel panel46;
        private System.Windows.Forms.Panel panel47;
        private System.Windows.Forms.CheckBox cbAsuTpLine4;
        private System.Windows.Forms.CheckBox cbAsuTpLine3;
        private System.Windows.Forms.CheckBox cbAsuTpLine2;
        private System.Windows.Forms.CheckBox cbAsuTpLine1;
        private System.Windows.Forms.Label label73;
        private System.Windows.Forms.Panel panel48;
        private System.Windows.Forms.Label label74;
        private System.Windows.Forms.Button btnReadRegisters;
        private System.Windows.Forms.NumericUpDown nudModbusSlaveID;
        private System.Windows.Forms.Label label29;
        private System.Windows.Forms.ToolStripStatusLabel toolStripStatusLabel1;
        private System.Windows.Forms.Button btnFileList;
        private System.Windows.Forms.Timer tmFileList;
        private System.Windows.Forms.Timer tmFileContent;
        public System.Windows.Forms.NumericUpDown nudMotoresourceMax1;
    }
}

