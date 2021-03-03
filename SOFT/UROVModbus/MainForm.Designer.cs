namespace UROVModbus
{
    partial class Form1
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
            this.statusBar = new System.Windows.Forms.StatusStrip();
            this.tabPages = new System.Windows.Forms.TabControl();
            this.tpModbusSettings = new System.Windows.Forms.TabPage();
            this.tpUROVSettings = new System.Windows.Forms.TabPage();
            this.GroupBox3 = new System.Windows.Forms.GroupBox();
            this.button6 = new System.Windows.Forms.Button();
            this.cmdOpenTCP = new System.Windows.Forms.Button();
            this.Label12 = new System.Windows.Forms.Label();
            this.cmbTcpProtocol = new System.Windows.Forms.ComboBox();
            this.Label11 = new System.Windows.Forms.Label();
            this.txtHostName = new System.Windows.Forms.TextBox();
            this.Label10 = new System.Windows.Forms.Label();
            this.txtTCPPort = new System.Windows.Forms.TextBox();
            this.GroupBox2 = new System.Windows.Forms.GroupBox();
            this.FindSerial = new System.Windows.Forms.Button();
            this.button5 = new System.Windows.Forms.Button();
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
            this.cmdOpenSerial = new System.Windows.Forms.Button();
            this.cmbSerialProtocol = new System.Windows.Forms.ComboBox();
            this.GroupBox1 = new System.Windows.Forms.GroupBox();
            this.cmbRetry = new System.Windows.Forms.ComboBox();
            this.Label4 = new System.Windows.Forms.Label();
            this.Label5 = new System.Windows.Forms.Label();
            this.txtTimeout = new System.Windows.Forms.TextBox();
            this.txtPollDelay = new System.Windows.Forms.TextBox();
            this.Label9 = new System.Windows.Forms.Label();
            this.tabPages.SuspendLayout();
            this.tpModbusSettings.SuspendLayout();
            this.GroupBox3.SuspendLayout();
            this.GroupBox2.SuspendLayout();
            this.GroupBox1.SuspendLayout();
            this.SuspendLayout();
            // 
            // statusBar
            // 
            this.statusBar.Location = new System.Drawing.Point(0, 535);
            this.statusBar.Name = "statusBar";
            this.statusBar.Size = new System.Drawing.Size(888, 22);
            this.statusBar.TabIndex = 0;
            this.statusBar.Text = "statusStrip1";
            // 
            // tabPages
            // 
            this.tabPages.Controls.Add(this.tpModbusSettings);
            this.tabPages.Controls.Add(this.tpUROVSettings);
            this.tabPages.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tabPages.Location = new System.Drawing.Point(0, 0);
            this.tabPages.Name = "tabPages";
            this.tabPages.SelectedIndex = 0;
            this.tabPages.Size = new System.Drawing.Size(888, 535);
            this.tabPages.TabIndex = 1;
            // 
            // tpModbusSettings
            // 
            this.tpModbusSettings.Controls.Add(this.GroupBox3);
            this.tpModbusSettings.Controls.Add(this.GroupBox2);
            this.tpModbusSettings.Controls.Add(this.GroupBox1);
            this.tpModbusSettings.Location = new System.Drawing.Point(4, 22);
            this.tpModbusSettings.Name = "tpModbusSettings";
            this.tpModbusSettings.Padding = new System.Windows.Forms.Padding(3);
            this.tpModbusSettings.Size = new System.Drawing.Size(880, 509);
            this.tpModbusSettings.TabIndex = 0;
            this.tpModbusSettings.Text = "Соединение";
            this.tpModbusSettings.UseVisualStyleBackColor = true;
            // 
            // tpUROVSettings
            // 
            this.tpUROVSettings.Location = new System.Drawing.Point(4, 22);
            this.tpUROVSettings.Name = "tpUROVSettings";
            this.tpUROVSettings.Padding = new System.Windows.Forms.Padding(3);
            this.tpUROVSettings.Size = new System.Drawing.Size(880, 474);
            this.tpUROVSettings.TabIndex = 1;
            this.tpUROVSettings.Text = "Настройки UROV";
            this.tpUROVSettings.UseVisualStyleBackColor = true;
            // 
            // GroupBox3
            // 
            this.GroupBox3.Controls.Add(this.button6);
            this.GroupBox3.Controls.Add(this.cmdOpenTCP);
            this.GroupBox3.Controls.Add(this.Label12);
            this.GroupBox3.Controls.Add(this.cmbTcpProtocol);
            this.GroupBox3.Controls.Add(this.Label11);
            this.GroupBox3.Controls.Add(this.txtHostName);
            this.GroupBox3.Controls.Add(this.Label10);
            this.GroupBox3.Controls.Add(this.txtTCPPort);
            this.GroupBox3.Location = new System.Drawing.Point(351, 22);
            this.GroupBox3.Name = "GroupBox3";
            this.GroupBox3.Size = new System.Drawing.Size(237, 310);
            this.GroupBox3.TabIndex = 16;
            this.GroupBox3.TabStop = false;
            this.GroupBox3.Text = "MODBUS/TCP";
            // 
            // button6
            // 
            this.button6.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.button6.Location = new System.Drawing.Point(13, 249);
            this.button6.Name = "button6";
            this.button6.Size = new System.Drawing.Size(207, 40);
            this.button6.TabIndex = 16;
            this.button6.Text = "Закрыть TCP";
            // 
            // cmdOpenTCP
            // 
            this.cmdOpenTCP.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.cmdOpenTCP.Location = new System.Drawing.Point(13, 203);
            this.cmdOpenTCP.Name = "cmdOpenTCP";
            this.cmdOpenTCP.Size = new System.Drawing.Size(207, 40);
            this.cmdOpenTCP.TabIndex = 15;
            this.cmdOpenTCP.Text = "Открыть TCP";
            // 
            // Label12
            // 
            this.Label12.Location = new System.Drawing.Point(10, 83);
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
            this.cmbTcpProtocol.Location = new System.Drawing.Point(107, 83);
            this.cmbTcpProtocol.Name = "cmbTcpProtocol";
            this.cmbTcpProtocol.Size = new System.Drawing.Size(115, 21);
            this.cmbTcpProtocol.TabIndex = 13;
            // 
            // Label11
            // 
            this.Label11.Location = new System.Drawing.Point(10, 54);
            this.Label11.Name = "Label11";
            this.Label11.Size = new System.Drawing.Size(91, 20);
            this.Label11.TabIndex = 12;
            this.Label11.Text = "IP-адрес:";
            // 
            // txtHostName
            // 
            this.txtHostName.Location = new System.Drawing.Point(107, 54);
            this.txtHostName.Name = "txtHostName";
            this.txtHostName.Size = new System.Drawing.Size(113, 20);
            this.txtHostName.TabIndex = 11;
            this.txtHostName.Text = "127.0.0.1";
            // 
            // Label10
            // 
            this.Label10.Location = new System.Drawing.Point(10, 24);
            this.Label10.Name = "Label10";
            this.Label10.Size = new System.Drawing.Size(91, 20);
            this.Label10.TabIndex = 10;
            this.Label10.Text = "TCP-порт:";
            // 
            // txtTCPPort
            // 
            this.txtTCPPort.Location = new System.Drawing.Point(107, 24);
            this.txtTCPPort.Name = "txtTCPPort";
            this.txtTCPPort.Size = new System.Drawing.Size(113, 20);
            this.txtTCPPort.TabIndex = 9;
            this.txtTCPPort.Text = "502";
            // 
            // GroupBox2
            // 
            this.GroupBox2.Controls.Add(this.FindSerial);
            this.GroupBox2.Controls.Add(this.button5);
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
            this.GroupBox2.Controls.Add(this.cmdOpenSerial);
            this.GroupBox2.Controls.Add(this.cmbSerialProtocol);
            this.GroupBox2.Location = new System.Drawing.Point(25, 22);
            this.GroupBox2.Name = "GroupBox2";
            this.GroupBox2.Size = new System.Drawing.Size(300, 310);
            this.GroupBox2.TabIndex = 15;
            this.GroupBox2.TabStop = false;
            this.GroupBox2.Text = "Serial Modbus";
            // 
            // FindSerial
            // 
            this.FindSerial.Enabled = false;
            this.FindSerial.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.FindSerial.Location = new System.Drawing.Point(13, 249);
            this.FindSerial.Name = "FindSerial";
            this.FindSerial.Size = new System.Drawing.Size(265, 40);
            this.FindSerial.TabIndex = 15;
            this.FindSerial.Text = "Поиск Serial";
            // 
            // button5
            // 
            this.button5.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.button5.Location = new System.Drawing.Point(148, 203);
            this.button5.Name = "button5";
            this.button5.Size = new System.Drawing.Size(130, 40);
            this.button5.TabIndex = 14;
            this.button5.Text = "Закрыть Serial";
            // 
            // cmbDataBits
            // 
            this.cmbDataBits.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cmbDataBits.Items.AddRange(new object[] {
            "8",
            "7"});
            this.cmbDataBits.Location = new System.Drawing.Point(129, 134);
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
            this.cmbStopBits.Location = new System.Drawing.Point(129, 161);
            this.cmbStopBits.Name = "cmbStopBits";
            this.cmbStopBits.Size = new System.Drawing.Size(149, 21);
            this.cmbStopBits.TabIndex = 12;
            // 
            // Label8
            // 
            this.Label8.Location = new System.Drawing.Point(10, 135);
            this.Label8.Name = "Label8";
            this.Label8.Size = new System.Drawing.Size(113, 18);
            this.Label8.TabIndex = 11;
            this.Label8.Text = "Биты данных:";
            // 
            // Label7
            // 
            this.Label7.Location = new System.Drawing.Point(10, 162);
            this.Label7.Name = "Label7";
            this.Label7.Size = new System.Drawing.Size(113, 18);
            this.Label7.TabIndex = 10;
            this.Label7.Text = "Стоп-биты:";
            // 
            // Label6
            // 
            this.Label6.Location = new System.Drawing.Point(10, 108);
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
            this.cmbParity.Location = new System.Drawing.Point(129, 107);
            this.cmbParity.Name = "cmbParity";
            this.cmbParity.Size = new System.Drawing.Size(149, 21);
            this.cmbParity.TabIndex = 8;
            // 
            // Label3
            // 
            this.Label3.Location = new System.Drawing.Point(10, 81);
            this.Label3.Name = "Label3";
            this.Label3.Size = new System.Drawing.Size(113, 18);
            this.Label3.TabIndex = 7;
            this.Label3.Text = "Протокол:";
            // 
            // Label2
            // 
            this.Label2.Location = new System.Drawing.Point(10, 54);
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
            "56000",
            "57600",
            "115200",
            "125000",
            "128000",
            "256000"});
            this.cmbBaudRate.Location = new System.Drawing.Point(129, 53);
            this.cmbBaudRate.Name = "cmbBaudRate";
            this.cmbBaudRate.Size = new System.Drawing.Size(149, 21);
            this.cmbBaudRate.TabIndex = 4;
            // 
            // cmbComPort
            // 
            this.cmbComPort.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cmbComPort.Location = new System.Drawing.Point(129, 26);
            this.cmbComPort.Name = "cmbComPort";
            this.cmbComPort.Size = new System.Drawing.Size(149, 21);
            this.cmbComPort.TabIndex = 3;
            // 
            // Label1
            // 
            this.Label1.Location = new System.Drawing.Point(10, 27);
            this.Label1.Name = "Label1";
            this.Label1.Size = new System.Drawing.Size(113, 18);
            this.Label1.TabIndex = 1;
            this.Label1.Text = "COM-порт:";
            // 
            // cmdOpenSerial
            // 
            this.cmdOpenSerial.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.cmdOpenSerial.Location = new System.Drawing.Point(13, 203);
            this.cmdOpenSerial.Name = "cmdOpenSerial";
            this.cmdOpenSerial.Size = new System.Drawing.Size(130, 40);
            this.cmdOpenSerial.TabIndex = 2;
            this.cmdOpenSerial.Text = "Открыть Serial";
            // 
            // cmbSerialProtocol
            // 
            this.cmbSerialProtocol.DisplayMember = "1";
            this.cmbSerialProtocol.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cmbSerialProtocol.Items.AddRange(new object[] {
            "RTU",
            "ASCII"});
            this.cmbSerialProtocol.Location = new System.Drawing.Point(129, 80);
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
            this.GroupBox1.Location = new System.Drawing.Point(619, 22);
            this.GroupBox1.Name = "GroupBox1";
            this.GroupBox1.Size = new System.Drawing.Size(228, 310);
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
            this.cmbRetry.Location = new System.Drawing.Point(132, 56);
            this.cmbRetry.Name = "cmbRetry";
            this.cmbRetry.Size = new System.Drawing.Size(66, 21);
            this.cmbRetry.TabIndex = 9;
            // 
            // Label4
            // 
            this.Label4.Location = new System.Drawing.Point(10, 27);
            this.Label4.Name = "Label4";
            this.Label4.Size = new System.Drawing.Size(116, 20);
            this.Label4.TabIndex = 8;
            this.Label4.Text = "Таймаут:";
            // 
            // Label5
            // 
            this.Label5.Location = new System.Drawing.Point(10, 61);
            this.Label5.Name = "Label5";
            this.Label5.Size = new System.Drawing.Size(116, 20);
            this.Label5.TabIndex = 10;
            this.Label5.Text = "Кол-во попыток:";
            // 
            // txtTimeout
            // 
            this.txtTimeout.Location = new System.Drawing.Point(132, 24);
            this.txtTimeout.Name = "txtTimeout";
            this.txtTimeout.Size = new System.Drawing.Size(66, 20);
            this.txtTimeout.TabIndex = 0;
            this.txtTimeout.Text = "1000";
            // 
            // txtPollDelay
            // 
            this.txtPollDelay.Location = new System.Drawing.Point(132, 90);
            this.txtPollDelay.Name = "txtPollDelay";
            this.txtPollDelay.Size = new System.Drawing.Size(66, 20);
            this.txtPollDelay.TabIndex = 11;
            this.txtPollDelay.Text = "0";
            // 
            // Label9
            // 
            this.Label9.Location = new System.Drawing.Point(10, 93);
            this.Label9.Name = "Label9";
            this.Label9.Size = new System.Drawing.Size(116, 20);
            this.Label9.TabIndex = 12;
            this.Label9.Text = "Задержка выборки:";
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(888, 557);
            this.Controls.Add(this.tabPages);
            this.Controls.Add(this.statusBar);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.Name = "Form1";
            this.SizeGripStyle = System.Windows.Forms.SizeGripStyle.Hide;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "UROV через MODBUS";
            this.tabPages.ResumeLayout(false);
            this.tpModbusSettings.ResumeLayout(false);
            this.GroupBox3.ResumeLayout(false);
            this.GroupBox3.PerformLayout();
            this.GroupBox2.ResumeLayout(false);
            this.GroupBox1.ResumeLayout(false);
            this.GroupBox1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.StatusStrip statusBar;
        private System.Windows.Forms.TabControl tabPages;
        private System.Windows.Forms.TabPage tpModbusSettings;
        private System.Windows.Forms.TabPage tpUROVSettings;
        private System.Windows.Forms.GroupBox GroupBox3;
        private System.Windows.Forms.Button button6;
        private System.Windows.Forms.Button cmdOpenTCP;
        private System.Windows.Forms.Label Label12;
        private System.Windows.Forms.ComboBox cmbTcpProtocol;
        private System.Windows.Forms.Label Label11;
        private System.Windows.Forms.TextBox txtHostName;
        private System.Windows.Forms.Label Label10;
        private System.Windows.Forms.TextBox txtTCPPort;
        private System.Windows.Forms.GroupBox GroupBox2;
        private System.Windows.Forms.Button FindSerial;
        private System.Windows.Forms.Button button5;
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
        private System.Windows.Forms.Button cmdOpenSerial;
        private System.Windows.Forms.ComboBox cmbSerialProtocol;
        private System.Windows.Forms.GroupBox GroupBox1;
        private System.Windows.Forms.ComboBox cmbRetry;
        private System.Windows.Forms.Label Label4;
        private System.Windows.Forms.Label Label5;
        private System.Windows.Forms.TextBox txtTimeout;
        private System.Windows.Forms.TextBox txtPollDelay;
        private System.Windows.Forms.Label Label9;
    }
}

