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
using System.Diagnostics;

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

        private enum MBusFunction
        {
            None = 0,     // нет функции
            ListFiles,    // запрошен список файлов в директории
        }

        private void ShowWaitCursor(bool show)
        {
            System.Windows.Forms.Cursor.Current = show ? Cursors.WaitCursor : Cursors.Default;
            Application.UseWaitCursor = show;
            Application.DoEvents();
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

            cmbBaudRate.SelectedIndex = 0; // 9600
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
                    ShowInfo("Не удалось создать экземпляр класса протокола! Ошибка: " + ex.Message);
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
                    ShowInfo("Не удалось создать экземпляр класса протокола! Ошибка: " + ex.Message);
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
                ShowInfo("Не удалось открыть протокол!  Ошибка: " + BusProtocolErrors.getBusProtocolErrorText(res));
            }
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
                    ShowInfo("Не удалось создать экземпляр класса протокола! Ошибка: " + ex.Message);
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
                    ShowInfo("Не удалось создать экземпляр класса протокола! Ошибка: " + ex.Message);
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
                ShowInfo("Не удалось открыть протокол, ошибка: " + BusProtocolErrors.getBusProtocolErrorText(res));

                
            }
        }

        private void tmCheckConnectTimer_Tick(object sender, EventArgs e)
        {
            if(currentConnectionMode == ConnectionMode.None)
            {
                tmCheckConnectTimer.Enabled = false;
                ShowTabPage(tpUROVSettings, false);
                return;
            }
            if(myProtocol != null)
            {
                if(!myProtocol.isOpen())
                {
                    tmCheckConnectTimer.Enabled = false;
                    currentConnectionMode = ConnectionMode.None;
                    UpdateControlButtons(currentConnectionMode);
                    ShowTabPage(tpUROVSettings, false);
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

        private UInt32 MakeUInt32(ushort reg1, ushort reg2)
        {
            UInt32 res = (UInt32) reg1;
            res <<= 16;
            res |= reg2;

            return res;
        }

        const int REGS_COUNT = 25; //TODO: КОЛИЧЕСТВО РЕГИСТРОВ К ЧТЕНИЮ

        private void ReadRegisters()
        {
            if(myProtocol != null && myProtocol.isOpen())
            {
                ShowWaitCursor(true);

                toolStripStatusLabel1.Text = " ЧИТАЕМ... ";
                toolStripStatusLabel1.BackColor = Color.Beige;
                Application.DoEvents();

                // читаем регистры прибора
                ushort[] readVals = new ushort[REGS_COUNT];
                int slave;
                int startRdReg;
                int res;

                slave = Convert.ToInt32(nudModbusSlaveID.Value);
                startRdReg = 1; // 40001 регистр для количества импульсов



                res = myProtocol.readMultipleRegisters(slave, startRdReg, readVals, REGS_COUNT);


                Debug.Write("res="); Debug.WriteLine(res);
                Debug.WriteLine(BusProtocolErrors.getBusProtocolErrorText(res));


                if ((res == BusProtocolErrors.FTALK_SUCCESS))
                {
                    // регистры прочитаны, надо заполнять данные формы!!!
                    toolStripStatusLabel1.Text = " ПРОЧИТАНО ";
                    toolStripStatusLabel1.BackColor = Color.Lime;
                    Application.DoEvents();

                    // MODBUS_REG_PULSES                         40001 // регистр для количества импульсов
                    int pulses = readVals[0];
                    nudPulses1.Value = pulses;

                    // MODBUS_REG_PULSES_DELTA                   40002 // регистр для дельты импульсов
                    int pulsesDelta = readVals[1];
                    nudDelta1.Value = pulsesDelta;

                    // MODBUS_REG_ETHALON_PULSES_DELTA1          40003 // регистр 1 для дельты времени сравнения импульсов с  эталоном
                    // MODBUS_REG_ETHALON_PULSES_DELTA2          40004 // регистр 2 для дельты времени сравнения импульсов с  эталоном
                    nudEthalonCompareDelta.Value = MakeUInt32(readVals[2], readVals[3]);

                    // MODBUS_REG_MOTORESOURCE1                  40005 // регистр 1 для моторесурса
                    // MODBUS_REG_MOTORESOURCE2                  40006 // регистр 2 для моторесурса
                    nudMotoresourceCurrent1.Value = MakeUInt32(readVals[4], readVals[5]);

                    // MODBUS_REG_MOTORESOURCE_MAX1              40007 // регистр 1 для максимального моторесурса
                    // MODBUS_REG_MOTORESOURCE_MAX2              40008 // регистр 2 для максимального моторесурса
                    nudMotoresourceMax1.Value = MakeUInt32(readVals[6], readVals[7]);

                    // обновляем метку процентов моторесурса
                    UpdateMotoresourcePercents(Convert.ToInt32(nudMotoresourceCurrent1.Value), Convert.ToInt32(nudMotoresourceMax1.Value));

                    // MODBUS_REG_TLOW_BORDER1                   40009 // регистр 1 для нижнего порога трансформатора
                    // MODBUS_REG_TLOW_BORDER2                   40010 // регистр 2 для нижнего порога трансформатора
                    nudLowBorder.Value = MakeUInt32(readVals[8], readVals[9]);

                    // MODBUS_REG_THIGH_BORDER1                  40011 // регистр 1 для верхнего порога трансформатора
                    // MODBUS_REG_THIGH_BORDER2                  40012 // регистр 2 для верхнего порога трансформатора
                    nudHighBorder.Value = MakeUInt32(readVals[10], readVals[11]);

                    // MODBUS_REG_RDELAY1                        40013 // регистр 1 для задержки реле
                    // MODBUS_REG_RDELAY2                        40014 // регистр 2 для задержки реле
                    nudRelayDelay.Value = MakeUInt32(readVals[12], readVals[13])/1000;

                    // MODBUS_REG_ACSDELAY                       40015 // регистр 1 для задержки ACS
                    // не используется

                    // MODBUS_REG_SKIPC1                         40016 // регистр 1 для пропуска импульсов
                    // MODBUS_REG_SKIPC2                         40017 // регистр 2 для пропуска импульсов
                    nudSkipCounter.Value = MakeUInt32(readVals[15], readVals[16]);

                    // MODBUS_REG_CCOEFF1                        40018 // регистр 1 для коэффициента тока
                    // MODBUS_REG_CCOEFF2                        40019 // регистр 2 для коэффициента тока
                    nudCurrentCoeff.Value = MakeUInt32(readVals[17], readVals[18]);

                    // MODBUS_REG_ASUTPFLAGS                     40020 // регистр для флагов АСУ ТП
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

                    // MODBUS_REG_MAXIDLETIME1                   40021 // регистр 1 для времени ожидания окончания импульсов
                    // MODBUS_REG_MAXIDLETIME2                   40022 // регистр 2 для времени ожидания окончания импульсов
                    nudMaxIdleTime.Value = MakeUInt32(readVals[20], readVals[21]);

                    // MODBUS_REG_RODMOVELEN1                    40023 // регистр 1 для величины перемещения штанги
                    // MODBUS_REG_RODMOVELEN2                    40024 // регистр 2 для величины перемещения штанги
                    nudRodMoveLength.Value = MakeUInt32(readVals[22], readVals[23]);

                    ShowWaitCursor(false);
                }
                else
                {
                    toolStripStatusLabel1.Text = " ОШИБКА ";
                    toolStripStatusLabel1.BackColor = Color.Red;
                    Application.DoEvents();

                    ShowWaitCursor(false);

                    // ошибка чтения регистров !!!
                    ShowProtocolError(res, "Ошибка чтения регистров: ");
                }
            }
        }

        private struct Reg32
        {
            public ushort reg1;
            public ushort reg2;
        }

        private Reg32 Make2Registers(Int32 val)
        {
            Reg32 result;

            result.reg1 = (ushort)(val >> 16);
            result.reg2 = (ushort)(val & 0xffff);

            return result;
        }

        private void WriteRegisters()
        {
            if (myProtocol != null && myProtocol.isOpen())
            {
                ShowWaitCursor(true);

                toolStripStatusLabel1.Text = " ПИШЕМ... ";
                toolStripStatusLabel1.BackColor = Color.Beige;
                Application.DoEvents();


                // пишем регистры в прибор
                ushort[] writeVals = new ushort[REGS_COUNT];
                int slave;
                int startWrReg;
                int res;

                slave = Convert.ToInt32(nudModbusSlaveID.Value);
                startWrReg = 1; // 40001 регистр для количества импульсов

                // тут заполняем регистры из формы

                // MODBUS_REG_PULSES                         40001 // регистр для количества импульсов
                ushort pulses = Convert.ToUInt16(nudPulses1.Value);
                writeVals[0] = pulses;

                // MODBUS_REG_PULSES_DELTA                   40002 // регистр для дельты импульсов
                ushort pulsesDelta = Convert.ToUInt16(nudDelta1.Value);
                writeVals[1] = pulsesDelta;

                // MODBUS_REG_ETHALON_PULSES_DELTA1          40003 // регистр 1 для дельты времени сравнения импульсов с  эталоном
                // MODBUS_REG_ETHALON_PULSES_DELTA2          40004 // регистр 2 для дельты времени сравнения импульсов с  эталоном
                Reg32 dReg = Make2Registers(Convert.ToInt32(nudEthalonCompareDelta.Value));
                writeVals[2] = dReg.reg1;
                writeVals[3] = dReg.reg2;


                // MODBUS_REG_MOTORESOURCE1                  40005 // регистр 1 для моторесурса
                // MODBUS_REG_MOTORESOURCE2                  40006 // регистр 2 для моторесурса
                dReg = Make2Registers(Convert.ToInt32(nudMotoresourceCurrent1.Value));
                writeVals[4] = dReg.reg1;
                writeVals[5] = dReg.reg2;

                // MODBUS_REG_MOTORESOURCE_MAX1              40007 // регистр 1 для максимального моторесурса
                // MODBUS_REG_MOTORESOURCE_MAX2              40008 // регистр 2 для максимального моторесурса
                dReg = Make2Registers(Convert.ToInt32(nudMotoresourceMax1.Value));
                writeVals[6] = dReg.reg1;
                writeVals[7] = dReg.reg2;


                // MODBUS_REG_TLOW_BORDER1                   40009 // регистр 1 для нижнего порога трансформатора
                // MODBUS_REG_TLOW_BORDER2                   40010 // регистр 2 для нижнего порога трансформатора
                dReg = Make2Registers(Convert.ToInt32(nudLowBorder.Value));
                writeVals[8] = dReg.reg1;
                writeVals[9] = dReg.reg2;

                // MODBUS_REG_THIGH_BORDER1                  40011 // регистр 1 для верхнего порога трансформатора
                // MODBUS_REG_THIGH_BORDER2                  40012 // регистр 2 для верхнего порога трансформатора
                dReg = Make2Registers(Convert.ToInt32(nudHighBorder.Value));
                writeVals[10] = dReg.reg1;
                writeVals[11] = dReg.reg2;

                // MODBUS_REG_RDELAY1                        40013 // регистр 1 для задержки реле
                // MODBUS_REG_RDELAY2                        40014 // регистр 2 для задержки реле
                dReg = Make2Registers(Convert.ToInt32(nudRelayDelay.Value)*1000);
                writeVals[12] = dReg.reg1;
                writeVals[13] = dReg.reg2;

                // MODBUS_REG_ACSDELAY                       40015 // регистр 1 для задержки ACS
                // не используется

                // MODBUS_REG_SKIPC1                         40016 // регистр 1 для пропуска импульсов
                // MODBUS_REG_SKIPC2                         40017 // регистр 2 для пропуска импульсов
                dReg = Make2Registers(Convert.ToInt32(nudSkipCounter.Value));
                writeVals[15] = dReg.reg1;
                writeVals[16] = dReg.reg2;

                // MODBUS_REG_CCOEFF1                        40018 // регистр 1 для коэффициента тока
                // MODBUS_REG_CCOEFF2                        40019 // регистр 2 для коэффициента тока
                dReg = Make2Registers(Convert.ToInt32(nudCurrentCoeff.Value));
                writeVals[17] = dReg.reg1;
                writeVals[18] = dReg.reg2;

                // MODBUS_REG_ASUTPFLAGS                     40020 // регистр для флагов АСУ ТП
                ushort asutpFlags = 0;

                if (cbAsuTpLine1.Checked)
                {
                    asutpFlags |= 1;
                }
                if (cbAsuTpLine2.Checked)
                {
                    asutpFlags |= 2;
                }
                if (cbAsuTpLine3.Checked)
                {
                    asutpFlags |= 4;
                }
                if (cbAsuTpLine4.Checked)
                {
                    asutpFlags |= 8;
                }

                writeVals[19] = asutpFlags;

                // MODBUS_REG_MAXIDLETIME1                   40021 // регистр 1 для времени ожидания окончания импульсов
                // MODBUS_REG_MAXIDLETIME2                   40022 // регистр 2 для времени ожидания окончания импульсов
                dReg = Make2Registers(Convert.ToInt32(nudMaxIdleTime.Value));
                writeVals[20] = dReg.reg1;
                writeVals[21] = dReg.reg2;

                // MODBUS_REG_RODMOVELEN1                    40023 // регистр 1 для величины перемещения штанги
                // MODBUS_REG_RODMOVELEN2                    40024 // регистр 2 для величины перемещения штанги
                dReg = Make2Registers(Convert.ToInt32(nudRodMoveLength.Value));
                writeVals[22] = dReg.reg1;
                writeVals[23] = dReg.reg2;


                //MODBUS_REG_SAVECHANGES                    40025 // регистр для флага сохранения настроек, ДОЛЖЕН БЫТЬ ПОСЛЕДНИМ ПО НОМЕРУ !!!
                //writeVals[24] = 1; // устанавливаем флаг, что надо сохранить настройки

                res = myProtocol.writeMultipleRegisters(slave, startWrReg, writeVals, REGS_COUNT);


                Debug.Write("res="); Debug.WriteLine(res);
                Debug.WriteLine(BusProtocolErrors.getBusProtocolErrorText(res));


                if ((res == BusProtocolErrors.FTALK_SUCCESS))
                {
                    //MODBUS_REG_SAVECHANGES                    40025 // регистр для флага сохранения настроек, ДОЛЖЕН БЫТЬ ПОСЛЕДНИМ ПО НОМЕРУ !!!
                    myProtocol.writeSingleRegister(slave, 25, 1);

                    ShowWaitCursor(false);

                    toolStripStatusLabel1.Text = " ЗАПИСАНО ";
                    toolStripStatusLabel1.BackColor = Color.Lime;
                    Application.DoEvents();


                    MessageBox.Show("Регистры успешно записаны в прибор!", "Сообщение", MessageBoxButtons.OK, MessageBoxIcon.Information);
                }
                else
                {
                    ShowWaitCursor(false);
                    ShowProtocolError(res, "Ошибка записи регистров: ");
                }
            }
        }

        private void btnCurrentCoeff_Click(object sender, EventArgs e)
        {
            WriteRegisters();
        }

        private void btnFileList_Click(object sender, EventArgs e)
        {
            GetFilesList("/"); // получаем список файлов в корневой папке
        }

        private string dirToList = "/"; // папка, с которой запрашиваем список файлов

        /// <summary>
        /// Запрашиваем список файлов указанной папки через MODBUS
        /// </summary>
        /// <param name="dir"></param>
        private void GetFilesList(string dir)
        {
            dirToList = dir;

            if (myProtocol != null && myProtocol.isOpen())
            {
                int res; // результат запроса

                // формируем регистры
                ushort func = Convert.ToUInt16(MBusFunction.ListFiles);
                ushort dataLen = Convert.ToUInt16(dir.Length);
                int slave = Convert.ToInt32(nudModbusSlaveID.Value);

                int numRegs = dir.Length / 2;
                if (dir.Length % 2 > 0)
                {
                    numRegs++;
                }

                // выделяем память под данные имени папки
                ushort[] data = new ushort[numRegs];

                // теперь записываем туда данные имени папки
                int iterator = 0;
                for (int i = 0; i < numRegs; i++)
                {
                    byte highVal = Convert.ToByte(dir[iterator]); iterator++;
                    byte lowVal = 0;
                    if (iterator < dir.Length - 1)
                    {
                        lowVal = Convert.ToByte(dir[iterator]); iterator++;
                    }
                    else
                    {
                        // только последний байт остался, младший
                        lowVal = highVal;
                        highVal = 0;

                    }

                    // формируем данные регистра
                    ushort reg = highVal; reg <<= 8; reg |= lowVal;
                    data[i] = reg;

                } // for

                // данные сформированы, записываем их в регистры устройства, и ждём ответа
                int errorsCount = 0;

                // MODBUS_REG_DATA_LENGTH                    41004 // регистр длины выданных или переданных от мастера данных (например, длины имени файла)
                res = myProtocol.writeSingleRegister(slave, 1004, dataLen);
                if (!(res == BusProtocolErrors.FTALK_SUCCESS))
                    errorsCount++;

                // MODBUS_REG_DATA                           41005 // регистр, начиная с которого идут данные (например, имя файла)
                res = myProtocol.writeMultipleRegisters(slave, 1005, data, numRegs);
                if (!(res == BusProtocolErrors.FTALK_SUCCESS))
                    errorsCount++;


                // MODBUS_REG_FUNCTION_NUMBER                41000 // регистр для номера запрошенной мастером функции (например, выдать список файлов в директории)
                res = myProtocol.writeSingleRegister(slave, 1000, func);
                if (!(res == BusProtocolErrors.FTALK_SUCCESS))
                    errorsCount++;

                if(errorsCount < 1)
                {
                    // данные записаны, ждём готовности данных!
                    toolStripStatusLabel1.Text = " ЖДЁМ СПИСКА ФАЙЛОВ... ";
                    toolStripStatusLabel1.BackColor = Color.Lime;
                    Application.DoEvents();

                    tmFileList.Enabled = true;

                }
                else
                {
                    tmFileList.Enabled = false;
                    ShowProtocolError(res, "Ошибка записи регистров: ");

                }

            } // if (myProtocol != null && myProtocol.isOpen())



        }

        private void ShowProtocolError(int res, string message)
        {
            toolStripStatusLabel1.Text = " ОШИБКА ";
            toolStripStatusLabel1.BackColor = Color.Red;
            Application.DoEvents();

            MessageBox.Show(message + BusProtocolErrors.getBusProtocolErrorText(res), "Ошибка", MessageBoxButtons.OK, MessageBoxIcon.Error);

        }

        /// <summary>
        /// Помещает файл в дерево файлов на SD
        /// </summary>
        /// <param name="filename"></param>
        /// <param name="fileFlags"></param>
        private void AddFileToList(string filename, int fileFlags)
        {
            Debug.Write("File name: "); Debug.WriteLine(filename);
            Debug.Write("Flags: "); Debug.WriteLine(fileFlags);
            Debug.WriteLine("");
        }

        private void tmFileList_Tick(object sender, EventArgs e)
        {
            tmFileList.Enabled = false; // выключаем таймер, чтоб не тикал

            if (myProtocol != null && myProtocol.isOpen())
            {

                // ждём готовности данных от устройства, при запросе списка файлов
                int slave = Convert.ToInt32(nudModbusSlaveID.Value);

                ushort[] regs = new ushort[1];

                //MODBUS_REG_READY_FLAG                     41001 // регистр флага готовности запрошенной мастером информации (например, выдано имя одного файла). Мастер проверяет
                int res = myProtocol.readMultipleRegisters(slave, 1001, regs, 1);

                if ((res == BusProtocolErrors.FTALK_SUCCESS))
                {
                    // прочитано успешно
                    if(regs[0] == 1)
                    {
                        // данные готовы, можно из читать
                        regs = new ushort[3]; // три регистра сразу

                        // MODBUS_REG_CONTINUE_FLAG                  41002 // регистр флага необходимости продолжения запроса мастером (например, когда ещё есть имена файлов в списке). Если 0 - то передача от слейва завершена.
                        // MODBUS_REG_FILE_FLAGS                     41003 // регистр флага признаков файла (0 - нет файла, 1 - обычный файл, 2 - папка)
                        // MODBUS_REG_DATA_LENGTH                    41004 // регистр длины выданных или переданных от мастера данных (например, длины имени файла)
                        res = myProtocol.readMultipleRegisters(slave, 1002, regs, 3);

                        if ((res == BusProtocolErrors.FTALK_SUCCESS))
                        {
                            // регистры, кроме регистров данных - прочитаны.

                            // признак перезапроса
                            ushort continueFlag = regs[0];

                            // признак файла
                            ushort fileFlags = regs[1];


                            // читаем сами данные
                            int dataLen = regs[2];
                            ushort[] data = new ushort[dataLen];

                            if (dataLen > 0) // есть имя файла или папки
                            {

                                // MODBUS_REG_DATA                           41005 // регистр, начиная с которого идут данные (например, имя файла)
                                res = myProtocol.readMultipleRegisters(slave, 1005, data, regs[2]);

                                if ((res == BusProtocolErrors.FTALK_SUCCESS))
                                {
                                    // данные прочитаны, можно разбирать имя папки
                                    string filename = "";

                                    int regsToRead = dataLen / 2;
                                    if(dataLen % 2 > 0)
                                    {
                                        regsToRead++;
                                    }

                                    for(int k=0;k< regsToRead;k++, dataLen-=2)
                                    {
                                        ushort reg = data[k];
                                        char highVal = (char)((reg & 0xFF00) >> 8);
                                        char lowVal = (char)(reg & 0x00FF);

                                        if (dataLen < 2)
                                        {
                                            // в регистре валиден только младший байт
                                            filename += lowVal;
                                        }
                                        else
                                        {
                                            // оба байта в регистре валидны
                                            filename += highVal;
                                            filename += lowVal;
                                        }

                                    } // for

                                    // получили имя файла, можно помещать его в список
                                    AddFileToList(filename, fileFlags);

                                    // теперь смотрим - если есть флаг перезапроса - то перезапрашиваем

                                    if(continueFlag == 1) // перезапрашиваем
                                    {
                                        GetFilesList(this.dirToList);
                                    }
                                    else
                                    {
                                        // закончили запрос
                                        toolStripStatusLabel1.Text = " СПИСОК ФАЙЛОВ ПОЛУЧЕН ";
                                        toolStripStatusLabel1.BackColor = Color.Lime;
                                        Application.DoEvents();


                                        MessageBox.Show("Список файлов получен!", "Сообщение", MessageBoxButtons.OK, MessageBoxIcon.Information);

                                    }

                                }
                                else
                                {
                                    ShowProtocolError(res, "Ошибка чтения регистров: ");
                                }
                            } // if (dataLen > 0)
                            else
                            {
                                // нет имени файла, проверяем, надо ли перезапрашивать?
                                if (continueFlag == 1) // перезапрашиваем
                                {
                                    GetFilesList(this.dirToList);
                                }
                                else
                                {
                                    // закончили запрос
                                    toolStripStatusLabel1.Text = " СПИСОК ФАЙЛОВ ПОЛУЧЕН ";
                                    toolStripStatusLabel1.BackColor = Color.Lime;
                                    Application.DoEvents();


                                    MessageBox.Show("Список файлов получен!", "Сообщение", MessageBoxButtons.OK, MessageBoxIcon.Information);

                                }
                            }

                        } // if ((res == BusProtocolErrors.FTALK_SUCCESS))
                        else
                        {
                            ShowProtocolError(res, "Ошибка чтения регистров: ");
                        }

                    }
                    else
                    {
                        // надо перезапросить, ожидаем готовность данных
                        tmFileList.Enabled = true;
                    }
                }
                else
                {
                    // не прочитано
                    ShowProtocolError(res,"Ошибка чтения списка файлов: ");

                }

            }
            else
            {
                tmFileList.Enabled = false;
                toolStripStatusLabel1.Text = " РАЗЪЕДИНЕНО ";
                toolStripStatusLabel1.BackColor = Color.Red;
                Application.DoEvents();
            }
        }
    }
}
