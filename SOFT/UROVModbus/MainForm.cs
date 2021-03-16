using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.IO.Ports;
using FieldTalk.Modbus.Master;
using System.Globalization;
using System.Threading;


namespace UROVModbus
{
    public partial class MainForm : Form
    {
        private MbusMasterFunctions myProtocol;

        private enum ConnectionMode
        {
            None,
            Serial,
            TCP
        }

        ConnectionMode currentConnectionMode = ConnectionMode.None;

        public MainForm()
        {
            InitializeComponent();
        }

        private void LoadComPorts()
        {
            cmbComPort.Items.Clear();

            string[] ports = SerialPort.GetPortNames();

            foreach (string port in ports)
            {
                cmbComPort.Items.Add(port);
            }

            try
            {
                cmbComPort.SelectedIndex = 0;
            }
            catch { }
        }

        private void ShowInfo(string text)
        {
            lblInfoText.Text = text;
        }

        private void ShowTabPage(TabPage tp, bool bShow)
        {
            if (!bShow)
            {
                tabPages.TabPages.Remove(tp);
                return;
            }

            if (!tabPages.Contains(tp))
                tabPages.TabPages.Add(tp);
        }

        private void mainForm_Load(object sender, EventArgs e)
        {
            ShowTabPage(tpUROVSettings, false);

            LoadComPorts();

            UpdateControlButtons(currentConnectionMode);

            cmbBaudRate.SelectedIndex = 5;
            cmbSerialProtocol.SelectedIndex = 0;
            cmbParity.SelectedIndex = 0;
            cmbStopBits.SelectedIndex = 0;
            cmbDataBits.SelectedIndex = 0;
            cmbTcpProtocol.SelectedIndex = 0;
            cmbRetry.SelectedIndex = 2;

        }

        private void btnReloadPorts_Click(object sender, EventArgs e)
        {
            LoadComPorts();
        }


        void UpdateControlButtons(ConnectionMode mode)
        {
            switch(mode)
            {
                case ConnectionMode.None:
                    {
                        btnCloseSerial.Enabled = false;
                        btnCloseTCP.Enabled = false;
                        btnOpenSerial.Enabled = true;
                        btnOpenTCP.Enabled = true;
                    }
                    break;

                case ConnectionMode.Serial:
                    {
                        btnOpenSerial.Enabled = false;
                        btnCloseSerial.Enabled = true;
                        btnOpenTCP.Enabled = false;
                        btnCloseTCP.Enabled = false;
                    }
                    break;

                case ConnectionMode.TCP:
                    {
                        btnOpenSerial.Enabled = false;
                        btnCloseSerial.Enabled = false;
                        btnOpenTCP.Enabled = false;
                        btnCloseTCP.Enabled = true;
                    }
                    break;
            }
        }

        private void cmdOpenSerial_Click(object sender, EventArgs e)
        {
            if(cmbComPort.Items.Count < 1)
            {
                ShowInfo("Не найдено ни одного порта!");
                return;
            }

            //
            // First we must instantiate class if we haven't done so already
            //
            if ((myProtocol == null))
            {
                try
                {
                    if ((cmbSerialProtocol.SelectedIndex == 0))
                        myProtocol = new MbusRtuMasterProtocol(); // RTU
                    else
                        myProtocol = new MbusAsciiMasterProtocol(); // ASCII
                }
                catch (OutOfMemoryException ex)
                {
                    //lblResult.Text = (" Ошибка была" + ex.Message);
                    ShowInfo("Не удалось создать экземпляр класса серийного протокола!" + ex.Message);
                    return;
                }
            }
            else // already instantiated, close protocol, reinstantiate
            {
                if (myProtocol.isOpen())
                {
                    myProtocol.closeProtocol();
                }
                myProtocol = null;
                try
                {
                    if ((cmbSerialProtocol.SelectedIndex == 0))
                        myProtocol = new MbusRtuMasterProtocol(); // RTU
                    else
                        myProtocol = new MbusAsciiMasterProtocol(); // ASCII
                }
                catch (OutOfMemoryException ex)
                {
                    //lblResult.Text = (" Ошибка была" + ex.Message);
                    ShowInfo("Не удалось создать экземпляр класса серийного протокола!" + ex.Message);
                    return;
                }
            }
            //
            // Here we configure the protocol
            // Здесь мы настроим протокол
            int retryCnt;
            int pollDelay;
            int timeOut;
            int baudRate;
            int parity;
            int dataBits;
            int stopBits;
            int res;
            try
            {
                retryCnt = int.Parse(cmbRetry.Text, CultureInfo.CurrentCulture);
            }
            catch (Exception)
            {
                retryCnt = 2;
            }
            try
            {
                pollDelay = int.Parse(txtPollDelay.Text, CultureInfo.CurrentCulture);
            }
            catch (Exception)
            {
                pollDelay = 10;
            }
            try
            {
                timeOut = int.Parse(txtTimeout.Text, CultureInfo.CurrentCulture);
            }
            catch (Exception)
            {
                timeOut = 1000;
            }
            try
            {
                baudRate = int.Parse(cmbBaudRate.Text, CultureInfo.CurrentCulture);
            }
            catch (Exception)
            {
                baudRate = 57600;
            }
            switch (cmbParity.SelectedIndex)
            {
                default:
                case 0:
                    parity = MbusSerialMasterProtocol.SER_PARITY_NONE;
                    break;
                case 1:
                    parity = MbusSerialMasterProtocol.SER_PARITY_EVEN;
                    break;
                case 2:
                    parity = MbusSerialMasterProtocol.SER_PARITY_ODD;
                    break;
            }
            switch (cmbDataBits.SelectedIndex)
            {
                default:
                case 0:
                    dataBits = MbusSerialMasterProtocol.SER_DATABITS_8;
                    break;
                case 1:
                    dataBits = MbusSerialMasterProtocol.SER_DATABITS_7;
                    break;
            }
            switch (cmbStopBits.SelectedIndex)
            {
                default:
                case 0:
                    stopBits = MbusSerialMasterProtocol.SER_STOPBITS_1;
                    break;
                case 1:
                    stopBits = MbusSerialMasterProtocol.SER_STOPBITS_2;
                    break;
            }
            myProtocol.timeout = timeOut;
            myProtocol.retryCnt = retryCnt;
            myProtocol.pollDelay = pollDelay;
            // Note: The following cast is required as the myProtocol object is declared
            // as the superclass of MbusSerialMasterProtocol. That way myProtocol can
            // represent different protocol types.
            // Примечание: В следующем варианте требуется как объект myProtocol объявлен
            // Как суперкласс MbusSerialMasterProtocol. Таким образом myProtocol может
            // Представляют различные типы протоколов.

            res = ((MbusSerialMasterProtocol)(myProtocol)).openProtocol(cmbComPort.Text, baudRate, dataBits, stopBits, parity);
            if ((res == BusProtocolErrors.FTALK_SUCCESS))
            {
                ShowInfo("Последовательный порт успешно открыт с параметрами:  "
                            + (cmbComPort.Text + (", "
                            + (baudRate + (" baud, "
                            + (dataBits + (" data bits, "
                            + (stopBits + (" stop bits, parity " + parity)))))))));


                currentConnectionMode = ConnectionMode.Serial;

                UpdateControlButtons(currentConnectionMode);




                ShowTabPage(tpUROVSettings, true);
                //Polltimer1.Enabled = true;
                tmCheckConnectTimer.Enabled = true;
            }
            else
            {
                //lblResult.Text = (" ошибка была: " + BusProtocolErrors.getBusProtocolErrorText(res));
                ShowInfo("Не удалось открыть протокол!  Ошибка была: " + BusProtocolErrors.getBusProtocolErrorText(res));
            }
        }

        private void GetUROVSettings()
        {
            //TODO: ТУТ ПОЛУЧЕНИЕ НАСТРОЕК УРОВ!!!
            if (myProtocol == null)
                return;

            if (!myProtocol.isOpen())
                return;


        }

        private void btnCloseSerial_Click(object sender, EventArgs e)
        {
            if ((myProtocol != null))
            {
                // Close protocol and serial port
                myProtocol.closeProtocol();
                currentConnectionMode = ConnectionMode.None;
                UpdateControlButtons(currentConnectionMode);

                ShowInfo("Протокол закрыт");


                /*
                Polltimer1.Enabled = false;
                toolStripStatusLabel1.Text = "  MODBUS ЗАКРЫТ   ";
                toolStripStatusLabel1.BackColor = Color.Red;
                toolStripStatusLabel3.Text = ("");
                portFound = false;
                */
            }
        }

        private void btnOpenTCP_Click(object sender, EventArgs e)
        {
            //
            // First we must instantiate class if we haven't done so already
            //
            if ((myProtocol == null))
            {
                try
                {
                    if ((cmbTcpProtocol.SelectedIndex == 0))
                        myProtocol = new MbusTcpMasterProtocol();
                    else
                        myProtocol = new MbusRtuOverTcpMasterProtocol();
                }
                catch (OutOfMemoryException ex)
                {
                    ShowInfo("Не удалось создать экземпляр класса серийного протокола! Ошибка была " + ex.Message);
                    return;
                }
            }
            else // already instantiated, close protocol and reinstantiate
            {
                if (myProtocol.isOpen())
                    myProtocol.closeProtocol();
                myProtocol = null;
                try
                {
                    if ((cmbTcpProtocol.SelectedIndex == 0))
                        myProtocol = new MbusTcpMasterProtocol();
                    else
                        myProtocol = new MbusRtuOverTcpMasterProtocol();
                }
                catch (OutOfMemoryException ex)
                {
                    ShowInfo("Не удалось создать экземпляр класса серийного протокола! Ошибка была " + ex.Message);
                    return;
                }
            }
            //
            // Here we configure the protocol
            //
            int retryCnt;
            int pollDelay;
            int timeOut;
            int tcpPort;
            int res;
            try
            {
                retryCnt = int.Parse(cmbRetry.Text, CultureInfo.CurrentCulture);
            }
            catch (Exception)
            {
                retryCnt = 0;
            }
            try
            {
                pollDelay = int.Parse(txtPollDelay.Text, CultureInfo.CurrentCulture);
            }
            catch (Exception)
            {
                pollDelay = 0;
            }
            try
            {
                timeOut = int.Parse(txtTimeout.Text, CultureInfo.CurrentCulture);
            }
            catch (Exception)
            {
                timeOut = 1000;
            }
            try
            {
                tcpPort = int.Parse(txtTCPPort.Text, CultureInfo.CurrentCulture);
            }
            catch (Exception)
            {
                tcpPort = 502;
            }
            myProtocol.timeout = timeOut;
            myProtocol.retryCnt = retryCnt;
            myProtocol.pollDelay = pollDelay;
            // Note: The following cast is required as the myProtocol object is declared
            // as the superclass of MbusTcpMasterProtocol. That way myProtocol can
            // represent different protocol types.
            ((MbusTcpMasterProtocol)myProtocol).port = (short)tcpPort;
            res = ((MbusTcpMasterProtocol)myProtocol).openProtocol(txtHostName.Text);
            if ((res == BusProtocolErrors.FTALK_SUCCESS))
            {
                ShowInfo("Modbus/TCP открыт с параметрами: " + (txtHostName.Text + (", TCP port " + tcpPort)));

                currentConnectionMode = ConnectionMode.TCP;

                UpdateControlButtons(currentConnectionMode);

                //Polltimer1.Enabled = true;
                tmCheckConnectTimer.Enabled = true;
            }
            else
            {
                ShowInfo("Could not open protocol, error was: " + BusProtocolErrors.getBusProtocolErrorText(res));

                
            }
        }

        private void tmCheckConnectTimer_Tick(object sender, EventArgs e)
        {
            if(currentConnectionMode == ConnectionMode.None)
            {
                tmCheckConnectTimer.Enabled = false;
                return;
            }
            if(myProtocol != null)
            {
                if(!myProtocol.isOpen())
                {
                    tmCheckConnectTimer.Enabled = false;
                    currentConnectionMode = ConnectionMode.None;
                    UpdateControlButtons(currentConnectionMode);
                }
            }
        }

        private void btnCloseTCP_Click(object sender, EventArgs e)
        {
            if ((myProtocol != null))
            {
                // Close protocol and serial port
                myProtocol.closeProtocol();

                currentConnectionMode = ConnectionMode.None;
                UpdateControlButtons(currentConnectionMode);

                //    // Indicate result on status line
                ShowInfo("Протокол закрыт");

//                Polltimer1.Enabled = false;
//                toolStripStatusLabel1.Text = "  MODBUS ЗАКРЫТ   ";
//                toolStripStatusLabel1.BackColor = Color.Red;
            }
        }

        private void btnReadRegisters_Click(object sender, EventArgs e)
        {
            ReadRegisters();
        }

        private void UpdateMotoresourcePercents(int currentPercents, int maxPercents)
        {
            NumberFormatInfo nfi = new NumberFormatInfo();
            nfi.NumberDecimalSeparator = ".";

            float percents = 0;
            if (maxPercents > 0)
                percents = (100.0f * currentPercents) / maxPercents;

            Color foreColor = Color.Green;
            if (percents >= 90.0f)
                foreColor = Color.Red;

            lblMotoresourcePercents1.ForeColor = foreColor;
            lblMotoresourcePercents1.Text = percents.ToString("n1", nfi) + "%";



        }

        private UInt32 MakeUInt32(UInt16 reg1, UInt16 reg2)
        {
            UInt32 res = (UInt32) reg1;
            res <<= 16;
            res |= reg2;

            return res;
        }

        private void ReadRegisters()
        {
            if(myProtocol != null && myProtocol.isOpen())
            {
                // читаем регистры прибора
                const int regs_to_read = 30;  //TODO: КОЛИЧЕСТВО РЕГИСТРОВ К ЧТЕНИЮ
                UInt16[] readVals = new UInt16[regs_to_read];
                int slave;
                int startRdReg;
                int numRdRegs;
                int res;

                slave = Convert.ToInt32(nudModbusSlaveID.Value);
                startRdReg = 0; // 40000 регистр для количества импульсов
                numRdRegs = regs_to_read;

                res = myProtocol.readMultipleRegisters(slave, startRdReg, readVals, numRdRegs);
                if ((res == BusProtocolErrors.FTALK_SUCCESS))
                {
                    // регистры прочитаны, надо заполнять данные формы!!!

                    // MODBUS_REG_PULSES                         40000 // регистр для количества импульсов
                    int pulses = readVals[0];
                    nudPulses1.Value = pulses;

                    // MODBUS_REG_PULSES_DELTA                   40001 // регистр для дельты импульсов
                    int pulsesDelta = readVals[1];
                    nudDelta1.Value = pulsesDelta;

                    // 40002 // регистр 1 для дельты времени сравнения импульсов с  эталоном
                    // 40003 // регистр 2 для дельты времени сравнения импульсов с  эталоном
                    nudEthalonCompareDelta.Value = MakeUInt32(readVals[2], readVals[3]);

                    // MODBUS_REG_MOTORESOURCE1                  40004 // регистр 1 для моторесурса
                    // MODBUS_REG_MOTORESOURCE2                  40005 // регистр 2 для моторесурса
                    nudMotoresourceCurrent1.Value = MakeUInt32(readVals[4], readVals[5]);

                    // MODBUS_REG_MOTORESOURCE_MAX1              40006 // регистр 1 для максимального моторесурса
                    // MODBUS_REG_MOTORESOURCE_MAX2              40007 // регистр 2 для максимального моторесурса
                    nudMotoresourceMax1.Value = MakeUInt32(readVals[6], readVals[7]);

                    // обновляем метку процентов моторесурса
                    UpdateMotoresourcePercents(Convert.ToInt32(nudMotoresourceCurrent1.Value), Convert.ToInt32(nudMotoresourceMax1.Value));

                    // MODBUS_REG_TLOW_BORDER1                   40008 // регистр 1 для нижнего порога трансформатора
                    // MODBUS_REG_TLOW_BORDER2                   40009 // регистр 2 для нижнего порога трансформатора
                    nudLowBorder.Value = MakeUInt32(readVals[8], readVals[9]);

                    // MODBUS_REG_THIGH_BORDER1                  40010 // регистр 1 для верхнего порога трансформатора
                    // MODBUS_REG_THIGH_BORDER2                  40011 // регистр 2 для верхнего порога трансформатора
                    nudHighBorder.Value = MakeUInt32(readVals[10], readVals[11]);

                    // MODBUS_REG_RDELAY1                        40012 // регистр 1 для задержки реле
                    // MODBUS_REG_RDELAY2                        40013 // регистр 2 для задержки реле
                    nudRelayDelay.Value = MakeUInt32(readVals[12], readVals[13]);

                    // MODBUS_REG_ACSDELAY                       40014 // регистр 1 для задержки ACS
                    // не используется

                    // MODBUS_REG_SKIPC1                         40015 // регистр 1 для пропуска импульсов
                    // MODBUS_REG_SKIPC2                         40016 // регистр 2 для пропуска импульсов
                    nudSkipCounter.Value = MakeUInt32(readVals[15], readVals[16]);

                    // MODBUS_REG_CCOEFF1                        40017 // регистр 1 для коэффициента тока
                    // MODBUS_REG_CCOEFF2                        40018 // регистр 2 для коэффициента тока
                    nudCurrentCoeff.Value = MakeUInt32(readVals[17], readVals[18]);

                    // MODBUS_REG_ASUTPFLAGS                     40019 // регистр для флагов АСУ ТП
                    int asutpFlags = readVals[19];
                    cbAsuTpLine1.Checked = false;
                    cbAsuTpLine2.Checked = false;
                    cbAsuTpLine3.Checked = false;
                    cbAsuTpLine4.Checked = false;

                    if ((asutpFlags & 1) != 0)
                    {
                        cbAsuTpLine1.Checked = true;
                    }
                    if ((asutpFlags & 2) != 0)
                    {
                        cbAsuTpLine2.Checked = true;
                    }
                    if ((asutpFlags & 4) != 0)
                    {
                        cbAsuTpLine3.Checked = true;
                    }
                    if ((asutpFlags & 8) != 0)
                    {
                        cbAsuTpLine4.Checked = true;
                    }

                    // MODBUS_REG_MAXIDLETIME1                   40020 // регистр 1 для времени ожидания окончания импульсов
                    // MODBUS_REG_MAXIDLETIME2                   40021 // регистр 2 для времени ожидания окончания импульсов
                    nudMaxIdleTime.Value = MakeUInt32(readVals[20], readVals[21]);

                    // MODBUS_REG_RODMOVELEN1                    40022 // регистр 1 для величины перемещения штанги
                    // MODBUS_REG_RODMOVELEN2                    40023 // регистр 2 для величины перемещения штанги
                    nudRodMoveLength.Value = MakeUInt32(readVals[22], readVals[23]);

                }
                else
                {
                    // ошибка чтения регистров !!!
                    MessageBox.Show("Ошибка чтения регистров!", "Ошибка", MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
            }
        }
    }
}
