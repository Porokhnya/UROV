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
using System.Windows.Forms.DataVisualization.Charting;

namespace UROVModbus
{
    public partial class MainForm : Form
    {
        private MbusMasterFunctions myProtocol;

        /// <summary>
        /// показывает или скрывает курсор ожидания
        /// </summary>
        /// <param name="show"></param>
        private void ShowWaitCursor(bool show)
        {
            System.Windows.Forms.Cursor.Current = show ? Cursors.WaitCursor : Cursors.Default;
            Application.UseWaitCursor = show;
            Application.DoEvents();
        }

        /// <summary>
        /// текущий тип соединения
        /// </summary>
        ConnectionMode currentConnectionMode = ConnectionMode.None;

        /// <summary>
        /// конструктор
        /// </summary>
        public MainForm()
        {
            InitializeComponent();
        }

        /// <summary>
        /// загружает названия портов в список
        /// </summary>
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

        /// <summary>
        /// Показывает информацию в поле на форме
        /// </summary>
        /// <param name="text"></param>
        private void ShowInfo(string text)
        {
            lblInfoText.Text = text;
        }


        /// <summary>
        /// Показывает или скрывает вкладку
        /// </summary>
        /// <param name="tp"></param>
        /// <param name="bShow"></param>
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

        /// <summary>
        /// Главная форма загружается
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void mainForm_Load(object sender, EventArgs e)
        {
            tsConnectionTypeInfo.Text = "";
            tsStatusMessage.Text = "";
            tsLinkStatus.Text = "";

            SetCurrentTabPageHint();

            //ShowTabPage(tpUROVSettings, false);
            ShowConnectionStatus(false);

            InitSubstitutions();

            LoadComPorts();

            UpdateControlButtons(currentConnectionMode);

            cmbBaudRate.SelectedIndex = 4; //57600 //0; // 9600
            cmbSerialProtocol.SelectedIndex = 0;
            cmbParity.SelectedIndex = 0;
            cmbStopBits.SelectedIndex = 0;
            cmbDataBits.SelectedIndex = 0;
            cmbTcpProtocol.SelectedIndex = 0;
            cmbRetry.SelectedIndex = 2;


            tmDateTime_Tick(tmDateTime, new EventArgs());
        }

        /// <summary>
        /// Клик на кнопку перезагрузки портов
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void btnReloadPorts_Click(object sender, EventArgs e)
        {
            LoadComPorts();
        }


        /// <summary>
        /// Обновляем кнопки управления соединением, в зависимости от его статуса
        /// </summary>
        /// <param name="mode"></param>
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

        /// <summary>
        /// Клик по кнопке "Открыть порт"
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
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


                string connInfo = cmbComPort.Text + ", " + baudRate.ToString() + " baud | RS-232";
                tsConnectionTypeInfo.Text = connInfo;

                currentConnectionMode = ConnectionMode.Serial;

                UpdateControlButtons(currentConnectionMode);



                //ShowConnectionStatus(true);

                //ShowTabPage(tpUROVSettings, true);
                //Polltimer1.Enabled = true;
                tmCheckConnectTimer.Enabled = true;
            }
            else
            {
                ShowConnectionStatus(false);
                //lblResult.Text = (" ошибка была: " + BusProtocolErrors.getBusProtocolErrorText(res));
                ShowInfo("Не удалось открыть протокол!  Ошибка: " + BusProtocolErrors.getBusProtocolErrorText(res));
            }
        }

        /// <summary>
        /// Клик по кнопке "Закрыть порт"
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void btnCloseSerial_Click(object sender, EventArgs e)
        {
            if ((myProtocol != null))
            {
                // Close protocol and serial port
                myProtocol.closeProtocol();
                currentConnectionMode = ConnectionMode.None;
                UpdateControlButtons(currentConnectionMode);

                ShowInfo("Протокол закрыт");

                ShowConnectionStatus(false);


                /*
                Polltimer1.Enabled = false;
                toolStripStatusLabel1.Text = "  MODBUS ЗАКРЫТ   ";
                toolStripStatusLabel1.BackColor = Color.Red;
                toolStripStatusLabel3.Text = ("");
                portFound = false;
                */
            }
        }

        /// <summary>
        /// Клик по кнопке "Открыть TCP-соединение"
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
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

                string connInfo = txtHostName.Text + ":" + tcpPort.ToString() + " | Nodbus/TCP";
                tsConnectionTypeInfo.Text = connInfo;


                currentConnectionMode = ConnectionMode.TCP;

                UpdateControlButtons(currentConnectionMode);

                //ShowConnectionStatus(true);

                //Polltimer1.Enabled = true;
                tmCheckConnectTimer.Enabled = true;
            }
            else
            {
                ShowInfo("Не удалось открыть протокол, ошибка: " + BusProtocolErrors.getBusProtocolErrorText(res));

                ShowConnectionStatus(false);
            }
        }

        /// <summary>
        /// Таймер проверки соединения
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void tmCheckConnectTimer_Tick(object sender, EventArgs e)
        {
            if(currentConnectionMode == ConnectionMode.None)
            {
                tmCheckConnectTimer.Enabled = false;
               // ShowTabPage(tpUROVSettings, false);
                return;
            }

            if(myProtocol != null)
            {
                if(!myProtocol.isOpen())
                {
                    tmCheckConnectTimer.Enabled = false;
                    currentConnectionMode = ConnectionMode.None;
                    UpdateControlButtons(currentConnectionMode);
                    // ShowTabPage(tpUROVSettings, false);

                    ShowInfo("Протокол закрыт");
                    ShowConnectionStatus(false);
                }
                else
                {
                    // протокол открыт, посылаем запрос времени с устройства
                    RequestDeviceDateTime();
                }
            }
        }

        private int modbusErrorsCount = 0;
        static bool hasModbusRequest = false;

        /// <summary>
        /// Запрашиваем дату и время с устройства
        /// </summary>
        private void RequestDeviceDateTime()
        {
            //запрашиваем только в том случае, когда соединение установлено, и нет других запросов по MODBUS.
            if(hasModbusRequest)
            {
                return; // есть другие запросы по модбас
            }

            if(myProtocol == null || !myProtocol.isOpen()) // протокол не открыт
            {
                return;
            }

            // если запрос неудачный - увеличиваем счётчик ошибок. Если ошибок больше 10 - показываем статус "Соединение разорвано".
            // читаем регистры прибора
            ushort[] readVals = new ushort[6];
            int slave;
            int startRdReg;
            int res;

            slave = Convert.ToInt32(nudModbusSlaveID.Value);
            startRdReg = 500; // 40500 регистр для года



            res = myProtocol.readMultipleRegisters(slave, startRdReg, readVals, 6);


            Debug.Write("res="); Debug.WriteLine(res);
            Debug.WriteLine(BusProtocolErrors.getBusProtocolErrorText(res));


            if ((res == BusProtocolErrors.FTALK_SUCCESS))
            {
                ShowConnectionStatus(true);

                // читаем переменные времени
                int year = readVals[0];
                int month = readVals[1];
                int day = readVals[2];
                int hour = readVals[3];
                int minute = readVals[4];
                int second = readVals[5];

                controllerDateTime = new DateTime(year, month, day, hour, minute, second);

                tbUROVDateTime.Text = controllerDateTime.ToString("dd.MM.yyyy HH:mm:ss");
            }
            else
            {
                modbusErrorsCount++;
                if (modbusErrorsCount >= 10)
                {
                    modbusErrorsCount = 0;

                    // Close protocol and serial port
                    myProtocol.closeProtocol();

                    // дальнейшее произойдёт в обработчике события таймера проверки соединения

                }
            }
        }

        /// <summary>
        /// Клик по кнопке "Закрыть TCP-соединение"
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
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
                ShowConnectionStatus(false);

                //                Polltimer1.Enabled = false;
                //                toolStripStatusLabel1.Text = "  MODBUS ЗАКРЫТ   ";
                //                toolStripStatusLabel1.BackColor = Color.Red;
            }
        }

        /// <summary>
        /// Клик по кнопке "Прочитать настройки"
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void btnReadRegisters_Click(object sender, EventArgs e)
        {
            ReadRegisters();
        }

        /// <summary>
        /// Обновляем метку процента наработки прибора
        /// </summary>
        /// <param name="currentPercents"></param>
        /// <param name="maxPercents"></param>
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

        /// <summary>
        /// Делает четырёхбайтовое целое из двух двухбайтовых
        /// </summary>
        /// <param name="reg1"></param>
        /// <param name="reg2"></param>
        /// <returns></returns>
        private UInt32 MakeUInt32(ushort reg1, ushort reg2)
        {
            UInt32 res = (UInt32) reg1;
            res <<= 16;
            res |= reg2;

            return res;
        }

        const int REGS_COUNT = 25; //TODO: КОЛИЧЕСТВО РЕГИСТРОВ К ЧТЕНИЮ


        /// <summary>
        /// Читаем регистры устройства
        /// </summary>
        private void ReadRegisters()
        {
            if(myProtocol != null && myProtocol.isOpen())
            {
                hasModbusRequest = true;

                ShowWaitCursor(true);
                ShowMessageInStatusBar(" ЧИТАЕМ РЕГИСТРЫ ", Color.Lime);


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
                    ShowMessageInStatusBar(" ПРОЧИТАНО ", Color.Lime);

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
                    ShowMessageInStatusBar(" ОШИБКА! ", Color.Red);

                    ShowWaitCursor(false);

                    // ошибка чтения регистров !!!
                    ShowProtocolError(res, "Ошибка чтения регистров: ");
                }

                hasModbusRequest = false;
            }
        }

        /// <summary>
        /// 32-х битное значение из двух слов
        /// </summary>
        private struct Reg32
        {
            public ushort reg1;
            public ushort reg2;
        }

        /// <summary>
        /// Разбивает 32-х битное значение на два слова
        /// </summary>
        /// <param name="val"></param>
        /// <returns></returns>
        private Reg32 Make2Registers(Int32 val)
        {
            Reg32 result;

            result.reg1 = (ushort)(val >> 16);
            result.reg2 = (ushort)(val & 0xffff);

            return result;
        }

        /// <summary>
        /// Пишем регистры в прибор
        /// </summary>
        private void WriteRegisters()
        {
            if (myProtocol != null && myProtocol.isOpen())
            {
                hasModbusRequest = true;

                ShowWaitCursor(true);

                ShowMessageInStatusBar(" ПИШЕМ РЕГИСТРЫ ", Color.Beige);

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

                    ShowMessageInStatusBar(" ЗАПИСАНО ", Color.Lime);

                    MessageBox.Show("Регистры успешно записаны в прибор!", "Сообщение", MessageBoxButtons.OK, MessageBoxIcon.Information);
                }
                else
                {
                    ShowWaitCursor(false);
                    ShowProtocolError(res, "Ошибка записи регистров: ");
                }

                hasModbusRequest = false;
            }
        }

        /// <summary>
        /// Клик по кнопке "Записать настройки"
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void btnCurrentCoeff_Click(object sender, EventArgs e)
        {
            WriteRegisters();
        }

        /// <summary>
        /// Форма просмотра содержимого SD
        /// </summary>
        private SDFilesForm sdFiles = null;

        /// <summary>
        /// Клик по кнопке "Список файлов"
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void btnFileList_Click(object sender, EventArgs e)
        {
            if(sdFiles == null)
            {
                sdFiles = new SDFilesForm(this);
            }

            sdFiles.plEmptySDWorkspace.BringToFront();
            sdFiles.tempSDParentNode = null;
            sdFiles.treeViewSD.Nodes.Clear();

            GetFilesList("/"); // получаем список файлов в корневой папке
        }

        /// <summary>
        /// Удаляет файл с SD
        /// </summary>
        /// <param name="fileName"></param>
        private void DoDeleteFile(string fileName)
        {
            if (myProtocol != null && myProtocol.isOpen())
            {
                hasModbusRequest = true;

                // протокол открыт, можно удалять файд
                int res; // результат запроса

                ushort func = Convert.ToUInt16(MBusFunction.DeleteFile);
                ushort dataLen = Convert.ToUInt16(fileName.Length);
                int slave = Convert.ToInt32(nudModbusSlaveID.Value);

                int numRegs = fileName.Length / 2;
                if (fileName.Length % 2 > 0)
                {
                    numRegs++;
                }

                // выделяем память под данные имени файла
                ushort[] data = new ushort[numRegs];

                // теперь записываем туда данные имени файла
                int iterator = 0;
                for (int i = 0; i < numRegs; i++)
                {
                    byte highVal = Convert.ToByte(fileName[iterator]); iterator++;
                    byte lowVal = 0;
                    if (iterator < fileName.Length)
                    {
                        lowVal = Convert.ToByte(fileName[iterator]); iterator++;
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

                if (errorsCount < 1)
                {
                    // данные записаны, файл удалён
                    if(nodeToDelete != null)
                    {
                        nodeToDelete.Remove();
                        nodeToDelete = null;
                    }
                    ShowMessageInStatusBar(" ФАЙЛ УДАЛЁН ", Color.Lime);

                }
                else
                {
                    nodeToDelete = null;
                    ShowProtocolError(res, "Ошибка записи регистров: ");

                }

                hasModbusRequest = false;
            }
        }

        /// <summary>
        /// Содержимое полученного файла
        /// </summary>
        private List<byte> fileContent = new List<byte>();
        private string fileNameToRequest = "";

        /// <summary>
        /// запрашиваем содержимое файла
        /// </summary>
        /// <param name="fileName"></param>
        private void DoRequestFile(string fileName)
        {
            if (myProtocol != null && myProtocol.isOpen())
            {
                hasModbusRequest = true;

                // протокол открыт, можно запрашивать содержимое файла
                int res; // результат запроса

                ushort func = Convert.ToUInt16(MBusFunction.FileContent);
                ushort dataLen = Convert.ToUInt16(fileName.Length);
                int slave = Convert.ToInt32(nudModbusSlaveID.Value);

                int numRegs = fileName.Length / 2;
                if (fileName.Length % 2 > 0)
                {
                    numRegs++;
                }

                // выделяем память под данные имени файла
                ushort[] data = new ushort[numRegs];

                // теперь записываем туда данные имени файла
                int iterator = 0;
                for (int i = 0; i < numRegs; i++)
                {
                    byte highVal = Convert.ToByte(fileName[iterator]); iterator++;
                    byte lowVal = 0;
                    if (iterator < fileName.Length)
                    {
                        lowVal = Convert.ToByte(fileName[iterator]); iterator++;
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

                if (errorsCount < 1)
                {
                    // данные записаны, ждём готовности данных!
                    ShowMessageInStatusBar(" ЖДЁМ ДАННЫЕ ФАЙЛА ", Color.Yellow);

                    tmFileContent.Enabled = true;

                }
                else
                {
                    hasModbusRequest = false;
                    tmFileContent.Enabled = false;
                    ShowProtocolError(res, "Ошибка записи регистров: ");

                }
            }

        }


        /// <summary>
        /// папка, с которой запрашиваем список файлов
        /// </summary>
        private string dirToList = "/";

        /// <summary>
        /// Запрашиваем список файлов указанной папки через MODBUS
        /// </summary>
        /// <param name="dir"></param>
        public void GetFilesList(string dir)
        {
            dirToList = dir;

            if (myProtocol != null && myProtocol.isOpen())
            {
                hasModbusRequest = true;

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
                    if (iterator < dir.Length)
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
                    ShowMessageInStatusBar(" ЖДЁМ СПИСКА ФАЙЛОВ ", Color.Yellow);

                    tmFileList.Enabled = true;

                }
                else
                {
                    hasModbusRequest = false;
                    tmFileList.Enabled = false;
                    ShowProtocolError(res, "Ошибка записи регистров: ");

                }

            } // if (myProtocol != null && myProtocol.isOpen())



        }

        private void ShowConnectionStatus(bool bConnected)
        {
            if (bConnected)
            {
                toolStripStatusLabel2.Text = " MODBUS ON ";
                toolStripStatusLabel2.BackColor = Color.Lime;

                tsLinkStatus.Text = " СВЯЗЬ С ПРИБОРОМ УРОВ УСТАНОВЛЕНА ";
                tsLinkStatus.BackColor = Color.Lime;
            }
            else
            {
                toolStripStatusLabel2.Text = " MODBUS OFF ";
                toolStripStatusLabel2.BackColor = Color.Red;

                tsLinkStatus.Text = " СВЯЗЬ С ПРИБОРОМ УРОВ НЕ УСТАНОВЛЕНА! ";
                tsLinkStatus.BackColor = Color.Red;

                tsConnectionTypeInfo.Text = "";

            }
        }

        /// <summary>
        /// Показывает статус в строке состояния
        /// </summary>
        /// <param name="message"></param>
        /// <param name="cl"></param>
        private void ShowMessageInStatusBar(string message, Color cl)
        {
            tsStatusMessage.Text = message;
            tsStatusMessage.BackColor = cl;

            if(sdFiles != null)
            {
                sdFiles.toolStripStatusLabel1.Text = message;
                sdFiles.toolStripStatusLabel1.BackColor = cl;
            }

            Application.DoEvents();

        }

        /// <summary>
        /// Показывает ошибку протокола
        /// </summary>
        /// <param name="res"></param>
        /// <param name="message"></param>
        private void ShowProtocolError(int res, string message)
        {
            tsStatusMessage.Text = " ОШИБКА ";
            tsStatusMessage.BackColor = Color.Red;
            Application.DoEvents();

            MessageBox.Show(message + BusProtocolErrors.getBusProtocolErrorText(res), "Ошибка", MessageBoxButtons.OK, MessageBoxIcon.Error);

        }

        /// <summary>
        /// Помещает файл в дерево файлов на SD
        /// </summary>
        /// <param name="filename"></param>
        /// <param name="fileFlags"></param>
        public void AddFileToList(string filename, int fileFlags)
        {
            Debug.Write("File name: "); Debug.WriteLine(filename);
            Debug.Write("Flags: "); Debug.WriteLine(fileFlags);
            Debug.WriteLine("");

            AddRecordToSDList(filename, fileFlags, sdFiles.tempSDParentNode);
        }

        /// <summary>
        /// Добавляет запись в дерево файлов на SD
        /// </summary>
        /// <param name="filename"></param>
        /// <param name="fileFlags"></param>
        /// <param name="parent"></param>
        private void AddRecordToSDList(string filename, int fileFlags, TreeNode parent = null)
        {
            TreeNodeCollection nodes = sdFiles.treeViewSD.Nodes;
            if (parent != null)
            {
                nodes = parent.Nodes;
                SDNodeTagHelper existingTag = (SDNodeTagHelper)parent.Tag;
                parent.Tag = new SDNodeTagHelper(SDNodeTags.TagFolderNode, existingTag.FileName, existingTag.IsDirectory); // говорим, что мы вычитали это дело
                // и удаляем заглушку...
                for (int i = 0; i < parent.Nodes.Count; i++)
                {
                    TreeNode child = parent.Nodes[i];
                    SDNodeTagHelper tg = (SDNodeTagHelper)child.Tag;
                    if (tg.Tag == SDNodeTags.TagDummyNode)
                    {
                        child.Remove();
                        break;
                    }
                }
            }
            bool isDir = fileFlags == 2;

            TreeNode node = nodes.Add(getTextFromFileName(filename));

            if (isDir)
            {
                node.ImageIndex = 0;
                node.SelectedImageIndex = 0;
                TreeNode dummy = node.Nodes.Add("вычитываем....");
                dummy.Tag = new SDNodeTagHelper(SDNodeTags.TagDummyNode, "", false); // этот узел потом удалим, при перечитывании
                dummy.ImageIndex = -1;

                node.Tag = new SDNodeTagHelper(SDNodeTags.TagFolderUninitedNode, filename, isDir); // говорим, что мы не перечитали содержимое папки ещё
            }
            else
            {
                node.ImageIndex = 2;
                node.SelectedImageIndex = node.ImageIndex;
                node.Tag = new SDNodeTagHelper(SDNodeTags.TagFileNode, filename, isDir);
            }
        }

        private TreeNode nodeToDelete = null;

        /// <summary>
        /// Удаляет файл на SD
        /// </summary>
        /// <param name="node"></param>
        public void DeleteFile(TreeNode node)
        {

            if (node == null)
                return;

            if (node.Tag == null)
                return;

            SDNodeTagHelper tg = (SDNodeTagHelper)node.Tag;
            if (tg.Tag != SDNodeTags.TagFileNode)
                return;

            string fullPathName = tg.FileName;

            nodeToDelete = node;


            TreeNode parent = node.Parent;
            while (parent != null)
            {
                SDNodeTagHelper nt = (SDNodeTagHelper)parent.Tag;
                fullPathName = nt.FileName + "/" + fullPathName;
                parent = parent.Parent;
            }

            DoDeleteFile(fullPathName);
        }

        /// <summary>
        /// Запрашивает файл, связанный с переданным узлом дерева файлов на SD
        /// </summary>
        /// <param name="node"></param>
        public void RequestFile(TreeNode node)
        {
            if (node == null)
                return;

            if (node.Tag == null)
                return;

            SDNodeTagHelper tg = (SDNodeTagHelper)node.Tag;
            if (tg.Tag != SDNodeTags.TagFileNode)
                return;

            //ShowWaitCursor(true);

            string fullPathName = tg.FileName;


            TreeNode parent = node.Parent;
            while (parent != null)
            {
                SDNodeTagHelper nt = (SDNodeTagHelper)parent.Tag;
                fullPathName = nt.FileName + "/" + fullPathName;
                parent = parent.Parent;
            }

            sdFiles.toolStripProgressBar1.Value = 0;
            sdFiles.toolStripProgressBar1.Visible = true;

            fileNameToRequest = fullPathName;
            fileContent.Clear();

            DoRequestFile(fullPathName);

            /*
            this.btnViewSDFile.Enabled = false;
            this.btnDeleteSDFile.Enabled = false;
            this.btnListSDFiles.Enabled = false;
            PushCommandToQueue(GET_PREFIX + "FILESIZE" + PARAM_DELIMITER + fullPathName, ParseAskFileSize);
            */

        }

        /// <summary>
        /// получение расшифровки из имени файла
        /// </summary>
        /// <param name="fName"></param>
        /// <returns></returns>
        string getTextFromFileName(string fName)
        {
            string fileName = fName.ToUpper();

            if (fileNamesSubstitutions.ContainsKey(fileName))
            {
                return fileNamesSubstitutions[fileName];
            }

            return fileName;
        }

        /// <summary>
        /// получение имени файла из его расшифровки
        /// </summary>
        /// <param name="text"></param>
        /// <returns></returns>
        string getFileNameFromText(string text)
        {
            if (fileNamesSubstitutions.ContainsValue(text))
            {
                string key = fileNamesSubstitutions.FirstOrDefault(x => x.Value == text).Key;
                return key;
            }

            return text;
        }

        private Dictionary<string, string> fileNamesSubstitutions = new Dictionary<string, string>();

        /// <summary>
        /// инициализация замен имён файлов их расшифровкой
        /// </summary>
        private void InitSubstitutions()
        {
            fileNamesSubstitutions.Clear();

            fileNamesSubstitutions.Add("LOG", "Лог-файлы");
            fileNamesSubstitutions.Add("ETL", "Эталоны");

            fileNamesSubstitutions.Add("ET0UP.ETL", "Канал №1, движение вверх");
            fileNamesSubstitutions.Add("ET0DWN.ETL", "Канал №1, движение вниз");

            fileNamesSubstitutions.Add("ET1UP.ETL", "Канал №2, движение вверх");
            fileNamesSubstitutions.Add("ET1DWN.ETL", "Канал №2, движение вниз");

            fileNamesSubstitutions.Add("ET2UP.ETL", "Канал №3, движение вверх");
            fileNamesSubstitutions.Add("ET2DWN.ETL", "Канал №3, движение вниз");

        }

        /// <summary>
        /// Таймер запроса списка файлов
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
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
                                        hasModbusRequest = false;
                                        // закончили запрос
                                        ShowMessageInStatusBar(" СПИСОК ФАЙЛОВ ПОЛУЧЕН ", Color.Lime);

                                        // MessageBox.Show("Список файлов получен!", "Сообщение", MessageBoxButtons.OK, MessageBoxIcon.Information);
                                        sdFiles.Show();
                                        sdFiles.BringToFront();

                                    }

                                }
                                else
                                {
                                    hasModbusRequest = false;
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
                                    hasModbusRequest = false;

                                    // закончили запрос
                                    ShowMessageInStatusBar(" СПИСОК ФАЙЛОВ ПОЛУЧЕН ", Color.Lime);

                                    sdFiles.Show();
                                    sdFiles.BringToFront();
                                    //MessageBox.Show("Список файлов получен!", "Сообщение", MessageBoxButtons.OK, MessageBoxIcon.Information);

                                }
                            }

                        } // if ((res == BusProtocolErrors.FTALK_SUCCESS))
                        else
                        {
                            hasModbusRequest = false;
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
                    hasModbusRequest = false;
                    // не прочитано
                    ShowProtocolError(res,"Ошибка чтения списка файлов: ");

                }

            }
            else
            {
                hasModbusRequest = false;
                tmFileList.Enabled = false;
                ShowMessageInStatusBar(" РАЗЪЕДИНЕНО ", Color.Red);
            }
        }


        /// <summary>
        /// Просмотр файла
        /// </summary>
        /// <param name="content">содержимое файла</param>
        private void ViewFile(List<byte> content)
        {
            /*
            ShowWaitCursor(false);
            statusProgressBar.Visible = false;
            statusProgressMessage.Visible = false;

            this.btnViewSDFile.Enabled = treeViewSD.SelectedNode != null;
            this.btnDeleteSDFile.Enabled = treeViewSD.SelectedNode != null;
            this.btnListSDFiles.Enabled = true;
            this.treeViewSD.Enabled = true;
            */
            string upStr = this.fileNameToRequest.ToUpper();

            if (upStr.EndsWith(".LOG")) // это лог-файл
            {
                ShowLogFile(content, this.sdFiles.logDataGrid, "", true, null, false);
                this.sdFiles.toolStripStatusLabel1.Text = "";
            }
            else if (upStr.EndsWith(".ETL")) // это файл эталона
            {
                CreateEthalonChart(content, this.sdFiles.ethalonChart, Convert.ToInt32(nudRodMoveLength.Value));
                this.sdFiles.plEthalonChart.BringToFront();
            }
        }

        const int customLabelsCount = 15; // сколько всего наших меток будет на оси X


        /// <summary>
        /// Устанавливает интервал для графика
        /// </summary>
        /// <param name="targetChart"></param>
        /// <param name="interval"></param>
        private void setChartInterval(System.Windows.Forms.DataVisualization.Charting.Chart targetChart, int interval)
        {
            for (int i = 0; i < targetChart.ChartAreas.Count; i++)
            {
                ChartArea area = targetChart.ChartAreas[i];
                area.AxisX.Interval = interval;
                area.AxisX.IntervalType = DateTimeIntervalType.Number; // тип интервала
                area.AxisX.ScaleView.Zoomable = true;
                area.CursorX.AutoScroll = true;

                area.CursorX.IsUserEnabled = true;
                area.CursorX.IsUserSelectionEnabled = true;
                area.CursorX.IntervalType = DateTimeIntervalType.Number;
                area.CursorX.Interval = interval;

                area.AxisX.ScaleView.SmallScrollSizeType = DateTimeIntervalType.Number;
                area.AxisX.ScaleView.SmallScrollSize = interval;
                area.AxisX.ScaleView.Zoomable = true;

                area.AxisX.ScaleView.MinSizeType = DateTimeIntervalType.Number;
                area.AxisX.ScaleView.MinSize = interval;

                area.AxisX.ScaleView.SmallScrollMinSizeType = DateTimeIntervalType.Number;
                area.AxisX.ScaleView.SmallScrollMinSize = interval;

                area.AxisY.IntervalType = DateTimeIntervalType.Number;
                area.AxisY.ScaleView.Zoomable = true;
                area.CursorY.IsUserSelectionEnabled = true;
                area.CursorY.IsUserEnabled = true;
                area.CursorY.AutoScroll = true;
            }
        }

        /// <summary>
        /// Создаёт график эталона
        /// </summary>
        /// <param name="content"></param>
        /// <param name="targetChart"></param>
        /// <param name="rodMoveLength"></param>
        private void CreateEthalonChart(List<byte> content, System.Windows.Forms.DataVisualization.Charting.Chart targetChart, int rodMoveLength)
        {

            System.Windows.Forms.DataVisualization.Charting.Series s = targetChart.Series[0];
            s.Points.Clear();




            // у нас размер одной записи - 4 байта
            int pointsCount = content.Count / 4;
            byte[] dt = new byte[4];

            // int xStep = 1;


            // подсчитываем максимальное значение по Y

            List<int> timeList = new List<int>();
            for (int i = 0; i < content.Count; i += 4)
            {
                try
                {
                    dt[0] = content[i];
                    dt[1] = content[i + 1];
                    dt[2] = content[i + 2];
                    dt[3] = content[i + 3];

                    int curVal = BitConverter.ToInt32(dt, 0);
                    timeList.Add(curVal);
                }
                catch
                {
                    break;
                }
            }

            // добавляем кол-во импульсов на график
            targetChart.Legends[0].CustomItems[0].Cells["PulsesCount"].Text = String.Format("Импульсов: {0}", timeList.Count);// * Config.Instance.SkipCounter);

            // тут добавляем кастомные метки на ось X - времена
            int maxTime = 0;
            if (timeList.Count > 0)
            {
                maxTime += timeList[timeList.Count - 1];
            }


            // получили максимальное время всего графика, в микросекундах. Теперь надо равномерно его распределить по графику в виде меток
            //            int step = maxTime / customLabelsCount;
            int neareastValue = 100000; // приближение к 100 мс
            int maxTimeDivideLabels = maxTime / customLabelsCount;
            while (maxTimeDivideLabels < neareastValue)
            {
                neareastValue /= 10;
            }

            int step = Convert.ToInt32(Math.Round(Convert.ToDouble(maxTimeDivideLabels) / neareastValue, 0)) * neareastValue;
            if (step < 1)
            {
                step = 1;
            }

            int clCount = maxTime / step;

            for (int kk = 0; kk < targetChart.ChartAreas.Count; kk++)
            {
                ChartArea area = targetChart.ChartAreas[kk];
                area.AxisX.CustomLabels.Clear();

                int startOffset = -step / 2;
                int endOffset = step / 2;
                int counter = 0;

                for (int i = 0; i < clCount; i++)
                {
                    string labelText = String.Format("{0}ms", counter / 1000);
                    CustomLabel monthLabel = new CustomLabel(startOffset, endOffset, labelText, 0, LabelMarkStyle.None);
                    area.AxisX.CustomLabels.Add(monthLabel);
                    startOffset = startOffset + step;
                    endOffset = endOffset + step;
                    counter += step;
                }
            }
            // устанавливаем интервал для меток на графике
            setChartInterval(targetChart, step);


            // получаем максимальное время импульса - это будет 100% по оси Y
            int maxPulseTime = 0;
            int minPulseTime = Int32.MaxValue;
            int fullMoveTime = 0;

            if (timeList.Count > 1)
            {
                fullMoveTime = timeList[timeList.Count - 1];
            }

            for (int i = 1; i < timeList.Count; i++)
            {
                int pulseTime = (timeList[i] - timeList[i - 1]);
                maxPulseTime = Math.Max(maxPulseTime, pulseTime);
                minPulseTime = Math.Min(minPulseTime, pulseTime);
            }

            int xCoord = 0;
            List<int> XValuesEthalon = new List<int>();
            List<double> YValuesEthalon = new List<double>();

            // теперь считаем все остальные точки
            int maxInterruptYVal = 0;
            float avgPulseTime = 0;

            for (int i = 1; i < timeList.Count; i++)
            {
                int pulseTime = timeList[i] - timeList[i - 1];

                avgPulseTime += pulseTime;

                int pulseTimePercents = (pulseTime * 100) / maxPulseTime;
                pulseTimePercents = 100 - pulseTimePercents;

                xCoord += pulseTime;
                XValuesEthalon.Add(xCoord);
                YValuesEthalon.Add(pulseTimePercents);

                maxInterruptYVal = Math.Max(maxInterruptYVal, pulseTimePercents);

            } // for

            if (timeList.Count > 1)
            {
                avgPulseTime = avgPulseTime / (timeList.Count - 1);
            }

            s.Points.DataBindXY(XValuesEthalon, YValuesEthalon);

            AddCustomSpeedLabels(targetChart.ChartAreas[0], timeList.Count, minPulseTime, avgPulseTime, fullMoveTime, maxInterruptYVal, rodMoveLength);


        }

        /// <summary>
        /// Добавляет на график метки со скоростью перемещения
        /// </summary>
        /// <param name="area"></param>
        /// <param name="timeListCount"></param>
        /// <param name="minPulseTime"></param>
        /// <param name="avgPulseTime"></param>
        /// <param name="fullMoveTime"></param>
        /// <param name="maxInterruptYVal"></param>
        /// <param name="rodMoveLength"></param>
        void AddCustomSpeedLabels(ChartArea area, int timeListCount, int minPulseTime, float avgPulseTime, int fullMoveTime, int maxInterruptYVal, int rodMoveLength)
        {
            if (timeListCount > 0 && minPulseTime != Int32.MaxValue && minPulseTime > 0 && fullMoveTime > 0 && avgPulseTime > 0 && maxInterruptYVal > 0)
            {
                // в maxInterruptYVal - у нас лежит максимальное значение по Y в условных единицах, т.е. 100% скорости перемещения


                // у нас времена перемещений - в микросекундах, чтобы получить скорость мм/с - надо умножить на миллион.
                float avgSpeed = (Convert.ToSingle(rodMoveLength) * 1000000) / fullMoveTime; // средняя скорость, мм/с

                float coeff = avgPulseTime / minPulseTime; // отношение средневзвешенной длительности импульса к минимальной
                float maxSpeed = (avgSpeed * coeff); // максимальная скорость, мм/с

                // выяснили максимальную скорость, теперь добавляем метки
                // округляем до ближайшей десятки вверх
                int roundedUpSpeed = ((int)Math.Round(maxSpeed / 10.0)) * 10;

                int divider = 10;
                int totalLabelsCount = roundedUpSpeed / divider; // получили шкалу, кратную 10

                int add = 2;
                while (totalLabelsCount > 5)
                {
                    divider = 10 * add;
                    add++;
                    totalLabelsCount = roundedUpSpeed / divider;
                }
                // эта шкала может быть очень частой, например, если у нас скорость большая

                int labelStep = maxInterruptYVal / (totalLabelsCount);

                area.AxisY.CustomLabels.Clear();
                area.AxisY.Interval = labelStep;

                area.AxisY.IntervalType = DateTimeIntervalType.Number; // тип интервала

                int startOffset = -labelStep / 2;
                int endOffset = labelStep / 2;
                int counter = 0;


                for (int i = 0; i <= totalLabelsCount; i++)
                {
                    // у нас maxSpeed = 100%
                    // maxInterruptYVal = 100% скорости
                    // labelStep*i = x% макс скорости
                    // x% = (labelStep*i*100)/maxInterruptYVal
                    // maxSpeed = 100%
                    // speedComputed = x
                    // speedComputed = (x*maxSpeed)/100;
                    float speedComputed = (((labelStep * i * 100) / maxInterruptYVal) * maxSpeed) / 100;

                    string labelText = String.Format("{0:0.00} м/с", speedComputed / 1000);

                    CustomLabel сLabel = new CustomLabel(startOffset, endOffset, labelText, 0, LabelMarkStyle.None);
                    area.AxisY.CustomLabels.Add(сLabel);

                    startOffset = startOffset + labelStep;
                    endOffset = endOffset + labelStep;
                    counter++;
                }



            }
        }

        /// <summary>
        /// Очищает таблицу
        /// </summary>
        /// <param name="targetGrid"></param>
        private void ClearInterruptsList(DataGridView targetGrid)
        {
            // Тут очистка таблицы
            targetGrid.RowCount = 0;
            try
            {
                if (gridToListCollection.ContainsKey(targetGrid))
                {
                    gridToListCollection[targetGrid].list.Clear();
                }
            }
            catch { }
        }


        /// <summary>
        /// отображение графика прерывания
        /// </summary>
        /// <param name="record"></param>
        /// <param name="stationID"></param>
        /// <param name="stationName"></param>
        /// <param name="modal"></param>
        public void ShowChart(InterruptRecord record, string stationID, string stationName, bool modal)
        {
            //  System.Diagnostics.Debug.Assert(record != null);
            if (record == null)
                return;

            ViewChartForm vcf = new ViewChartForm(record, stationID, stationName);

            vcf.setDefaultFileName(record.InterruptInfo.InterruptTime.ToString("yyyy-MM-dd HH.mm"));

            vcf.lblCaption.Text = "Срабатывание от " + record.InterruptInfo.InterruptTime.ToString("dd.MM.yyyy HH:mm:ss");

            // добавляем кол-во импульсов на график
            vcf.chart.Legends[1].CustomItems[0].Cells["PulsesCount"].Text = String.Format("Импульсов: {0}", record.InterruptData.Count * Convert.ToInt32(nudSkipCounter.Value));
            vcf.chart.Legends[1].CustomItems[0].Cells["EthalonPulses"].Text = String.Format("Эталон: {0}", record.EthalonData.Count);
            vcf.chart.Legends[1].CustomItems[0].Cells["TrigDate"].Text = "Дата: " + record.InterruptInfo.InterruptTime.ToString("dd.MM.yyyy HH:mm:ss");
            vcf.chart.Legends[1].CustomItems[0].Cells["Place"].Text = stationName;
            vcf.chart.Legends[1].CustomItems[0].Cells["PreviewCount"].Text = String.Format("Превью тока: {0} записей", record.PreviewCount);
            vcf.chart.Legends[1].CustomItems[0].Cells["tFact"].Text = String.Format("t факт: {0} ms", record.MoveTime / 1000);


            System.Windows.Forms.DataVisualization.Charting.Series ethalonSerie = vcf.chart.Series[0];
            ethalonSerie.Points.Clear();

            System.Windows.Forms.DataVisualization.Charting.Series interruptSerie = vcf.chart.Series[1];
            interruptSerie.Points.Clear();

            Series channel1Current = vcf.chart.Series[2];
            channel1Current.Points.Clear();

            Series channel2Current = vcf.chart.Series[3];
            channel2Current.Points.Clear();

            Series channel3Current = vcf.chart.Series[4];
            channel3Current.Points.Clear();

            // время наступления прерывания
            DateTime interruptTime = record.InterruptInfo.InterruptTime;

            // смещение в микросекундах от данных по току до начала списка прерываний
            int pulsesOffset = record.DataArrivedTime;


            // СПИСОК ПРИХОДИТ НОРМАЛИЗОВАННЫМ ОТНОСИТЕЛЬНО ПЕРВОЙ ЗАПИСИ!!!
            List<int> timeList = record.InterruptData;

            // получаем максимальное время импульса - это будет 100% по оси Y
            int maxPulseTime = 0;
            int minPulseTime = Int32.MaxValue;
            int fullMoveTime = 0; // полное время перемещения
            float avgPulseTime = 0; // средневзвешенная длительность импульса

            if (timeList.Count > 1)
            {
                fullMoveTime = timeList[timeList.Count - 1];
                float thisAvgPulseTime = 0;

                for (int i = 1; i < timeList.Count; i++)
                {
                    int curPulseTime = (timeList[i] - timeList[i - 1]);
                    thisAvgPulseTime += curPulseTime;

                    maxPulseTime = Math.Max(maxPulseTime, curPulseTime);
                    minPulseTime = Math.Min(minPulseTime, curPulseTime);
                }

                avgPulseTime = thisAvgPulseTime / (timeList.Count - 1);
            }


            if (record.EthalonData.Count > 1)
            {
                float thisAvgPulseTime = 0;

                for (int i = 1; i < record.EthalonData.Count; i++)
                {
                    int curPulseTime = (record.EthalonData[i] - record.EthalonData[i - 1]);
                    thisAvgPulseTime += curPulseTime;

                    maxPulseTime = Math.Max(maxPulseTime, curPulseTime);
                    minPulseTime = Math.Min(minPulseTime, curPulseTime);
                }

                fullMoveTime = Math.Max(fullMoveTime, record.EthalonData[record.EthalonData.Count - 1]);
                float avgPulseTime2 = thisAvgPulseTime / (record.EthalonData.Count - 1);

                avgPulseTime = (avgPulseTime + avgPulseTime2) / 2;
            }


            int xCoord = pulsesOffset;
            List<int> XValuesInterrupt = new List<int>();
            List<double> YValuesInterrupt = new List<double>();

            //int interruptAddedPoints = 1;

            if (record.CurrentTimes.Count > 0)
            {
                for (int z = 0; z < record.CurrentTimes.Count; z++)
                {

                    if (record.CurrentTimes[z] >= pulsesOffset)
                        break;

                    XValuesInterrupt.Add(record.CurrentTimes[z]);
                    YValuesInterrupt.Add(0);
                    //   interruptAddedPoints++;
                }
            }


            XValuesInterrupt.Add(xCoord);
            YValuesInterrupt.Add(0);

            // вот тут нам надо добавлять недостающие времена, от начала времени токов, до срабатывания защиты
            int offsetLabelIndex = XValuesInterrupt.Count - 1;

            int maxInterruptYVal = 0;


            // теперь считаем все остальные точки

            if (timeList.Count > 1)
            {

                for (int i = 1; i < timeList.Count; i++)
                {
                    int pulseTime = timeList[i] - timeList[i - 1];


                    int pulseTimePercents = (pulseTime * 100) / maxPulseTime;

                    pulseTimePercents = 100 - pulseTimePercents;

                    // абсолютное инвертированное значение от maxPulseTime
                    // maxPulseTime = 100%
                    // x = pulseTimePercents
                    // x = (pulseTimePercents*maxPulseTime)/100;

                    xCoord += pulseTime;
                    XValuesInterrupt.Add(xCoord);

                    int computedYVal = (pulseTimePercents * maxPulseTime) / 100;
                    maxInterruptYVal = Math.Max(maxInterruptYVal, computedYVal);
                    YValuesInterrupt.Add(computedYVal);


                } // for

            }


            // убираем последний пик вверх

            if (YValuesInterrupt.Count > 1)
            {
                YValuesInterrupt[YValuesInterrupt.Count - 1] = 0;
            }

            // вставляем метку окончания импульсов
            int lastInterruptIdx = XValuesInterrupt.Count - 1;

            bool canAddInterruptLabels = XValuesInterrupt.Count > 1;


            // а вот тут - добавлять следующие точки от конца прерывания до конца информации по токам
            if (record.CurrentTimes.Count > 0)
            {
                for (int z = 0; z < record.CurrentTimes.Count; z++)
                {
                    if (record.CurrentTimes[z] <= xCoord)
                        continue;

                    XValuesInterrupt.Add(record.CurrentTimes[z]);
                    YValuesInterrupt.Add(0);
                }
            }


            interruptSerie.Points.DataBindXY(XValuesInterrupt, YValuesInterrupt);
            if (offsetLabelIndex != -1 && canAddInterruptLabels)
            {
                interruptSerie.Points[offsetLabelIndex].Label = String.Format("НАЧАЛО ПРЕРЫВАНИЯ, {0} ms", pulsesOffset / 1000);
                interruptSerie.Points[offsetLabelIndex].LabelBorderDashStyle = ChartDashStyle.Solid;
                interruptSerie.Points[offsetLabelIndex].LabelBorderWidth = 1;
                interruptSerie.Points[offsetLabelIndex].LabelBorderColor = Color.Black;
                interruptSerie.Points[offsetLabelIndex].LabelBackColor = Color.Black;
                interruptSerie.Points[offsetLabelIndex].LabelForeColor = Color.White;

                interruptSerie.Points[offsetLabelIndex].MarkerColor = Color.Red;
                interruptSerie.Points[offsetLabelIndex].MarkerStyle = MarkerStyle.Circle;
                interruptSerie.Points[offsetLabelIndex].MarkerSize = 6;

            }

            // расчётная длительность импульсов прерывания
            int computedInterruptLength = -1;


            if (lastInterruptIdx != -1 && canAddInterruptLabels)
            {
                interruptSerie.Points[lastInterruptIdx].Label = String.Format("КОНЕЦ ПРЕРЫВАНИЯ, {0} ms", Convert.ToInt32(interruptSerie.Points[lastInterruptIdx].XValue / 1000));
                interruptSerie.Points[lastInterruptIdx].LabelBorderDashStyle = ChartDashStyle.Solid;
                interruptSerie.Points[lastInterruptIdx].LabelBorderWidth = 1;
                interruptSerie.Points[lastInterruptIdx].LabelBorderColor = Color.Black;
                interruptSerie.Points[lastInterruptIdx].LabelBackColor = Color.Black;
                interruptSerie.Points[lastInterruptIdx].LabelForeColor = Color.White;

                interruptSerie.Points[lastInterruptIdx].MarkerColor = Color.Red;
                interruptSerie.Points[lastInterruptIdx].MarkerStyle = MarkerStyle.Circle;
                interruptSerie.Points[lastInterruptIdx].MarkerSize = 6;

                computedInterruptLength = Convert.ToInt32(interruptSerie.Points[lastInterruptIdx].XValue) - pulsesOffset;
            }


            if (computedInterruptLength != -1)
            {
                vcf.chart.Legends[1].CustomItems[0].Cells["tComputed"].Text = String.Format("t расчёт: {0} ms", computedInterruptLength / 1000);

            }



            xCoord = pulsesOffset;
            List<int> XValuesEthalon = new List<int>();
            List<double> YValuesEthalon = new List<double>();


            XValuesEthalon.Add(xCoord);
            YValuesEthalon.Add(0);

            // считаем график эталона
            if (record.EthalonData.Count > 0)
            {
                for (int i = 1; i < record.EthalonData.Count; i++)
                {
                    int pulseTime = record.EthalonData[i] - record.EthalonData[i - 1];

                    int pulseTimePercents = (pulseTime * 100) / maxPulseTime;
                    pulseTimePercents = 100 - pulseTimePercents;

                    xCoord += pulseTime;
                    XValuesEthalon.Add(xCoord);

                    int computedYVal = (pulseTimePercents * maxPulseTime) / 100;
                    maxInterruptYVal = Math.Max(maxInterruptYVal, computedYVal);
                    YValuesEthalon.Add(computedYVal);

                } // for
            }

            // убираем последний пик вверх

            if (YValuesEthalon.Count > 1)
            {
                YValuesEthalon[YValuesEthalon.Count - 1] = 0;
            }


            ethalonSerie.Points.DataBindXY(XValuesEthalon, YValuesEthalon);

            int maxCurrentValue = 0;

            // теперь создаём графики по току
            if (record.CurrentTimes.Count > 0)
            {
                // СПИСОК СОДЕРЖИТ ВРЕМЕНА micros(), И ЯВЛЯЕТСЯ НОРМАЛИЗОВАННЫМ !!!
                List<int> XValuesOfCurrent1 = new List<int>();
                List<int> XValuesOfCurrent2 = new List<int>();
                List<int> XValuesOfCurrent3 = new List<int>();
                List<double> YValuesChannel1 = new List<double>();
                List<double> YValuesChannel2 = new List<double>();
                List<double> YValuesChannel3 = new List<double>();

                List<int> currentTimesList = record.CurrentTimes;
                xCoord = 0;

                for (int i = 1; i < currentTimesList.Count; i++)
                {
                    maxCurrentValue = Math.Max(maxCurrentValue, record.CurrentData1[i]);
                    maxCurrentValue = Math.Max(maxCurrentValue, record.CurrentData2[i]);
                    maxCurrentValue = Math.Max(maxCurrentValue, record.CurrentData3[i]);

                }


                //int phaseOffset = 10000; // пофазный сдвиг


                // теперь считаем все остальные точки
                for (int i = 1; i < currentTimesList.Count; i++)
                {
                    int pulseTime = currentTimesList[i] - currentTimesList[i - 1];
                    //pulseTime *= 100;

                    //int percents = map(record.CurrentData1[i], 0, maxCurrentValue, 0, 80);
                    YValuesChannel1.Add(record.CurrentData1[i]);// percents);

                    //percents = map(record.CurrentData2[i], 0, maxCurrentValue, 0, 78);
                    YValuesChannel2.Add(record.CurrentData2[i]);// percents);

                    //percents = map(record.CurrentData3[i], 0, maxCurrentValue, 0, 76);
                    YValuesChannel3.Add(record.CurrentData3[i]);// percents);



                    xCoord += pulseTime;

                    XValuesOfCurrent1.Add(xCoord);

                    if (i > 1)
                    {
                        XValuesOfCurrent2.Add(xCoord);// + phaseOffset);
                        XValuesOfCurrent3.Add(xCoord);// + phaseOffset * 2);
                    }
                    else
                    {
                        XValuesOfCurrent2.Add(xCoord);
                        XValuesOfCurrent3.Add(xCoord);
                    }

                } // for



                // добавляем графики тока

                channel1Current.Points.DataBindXY(XValuesOfCurrent1, YValuesChannel1);
                channel2Current.Points.DataBindXY(XValuesOfCurrent2, YValuesChannel2);
                channel3Current.Points.DataBindXY(XValuesOfCurrent3, YValuesChannel3);


                // графики тока добавлены.

            } // if(record.CurrentTimes.Count > 0)


            // тут добавляем кастомные метки на ось X - времена
            int maxTime = record.DataArrivedTime;
            if (record.InterruptData.Count > 0)
            {
                maxTime += record.InterruptData[record.InterruptData.Count - 1];
            }

            if (record.EthalonData.Count > 0)
            {
                maxTime = Math.Max(maxTime, record.DataArrivedTime + record.EthalonData[record.EthalonData.Count - 1]);
            }

            if (record.CurrentTimes.Count > 0)
            {
                maxTime = Math.Max(maxTime, record.CurrentTimes[record.CurrentTimes.Count - 1]);
            }



            // получили максимальное время всего графика, в микросекундах. Теперь надо равномерно его распределить по графику в виде меток
            //            int step = maxTime / customLabelsCount;
            int neareastValue = 100000; // приближение к 100 мс
            int maxTimeDivideLabels = maxTime / customLabelsCount;
            while (maxTimeDivideLabels < neareastValue)
            {
                neareastValue /= 10;
            }

            int step = Convert.ToInt32(Math.Round(Convert.ToDouble(maxTimeDivideLabels) / neareastValue, 0)) * neareastValue;
            if (step < 1)
            {
                step = 1;
            }

            int clCount = maxTime / step;
            for (int kk = 0; kk < vcf.chart.ChartAreas.Count; kk++)
            {
                ChartArea area = vcf.chart.ChartAreas[kk];
                area.AxisX.CustomLabels.Clear();

                int startOffset = -step / 2;
                int endOffset = step / 2;
                int counter = 0;

                for (int i = 0; i < /*customLabelsCount*/clCount; i++)
                {
                    string labelText = String.Format("{0}ms", counter / 1000);
                    CustomLabel monthLabel = new CustomLabel(startOffset, endOffset, labelText, 0, LabelMarkStyle.None);
                    area.AxisX.CustomLabels.Add(monthLabel);
                    startOffset = startOffset + step;
                    endOffset = endOffset + step;
                    counter += step;
                }
            }

            // устанавливаем интервал для меток на графике
            vcf.setInterval(step);


            // теперь пробуем для графика прерываний - переназначить метки по оси Y
            // у нас есть общее время перемещения штанги. У нас есть скорость перемещения.
            // из всего этого надо правильно сформировать метки на графике.
            // мы можем вычислить средневзвешенную скорость перемещения: v = S/t, где S - длина перемещения штанги, а t - общее время перемещения.
            // также у нас чем больше время между импульсами - тем меньше скорость на конкретном участке графика, и наоборот.
            // очевидно, что минимальная скорость - это 0. Максимальная скорость - это отношение средневзвешенной длительности импульса к минимальной длительности импульса,
            // умноженное на средневзвешенную скорость.

            int rodMoveLength = record.RodMoveLength > 0 ? record.RodMoveLength : Convert.ToInt32(nudRodMoveLength.Value); // величина перемещения штанги, мм
            AddCustomSpeedLabels(vcf.chart.ChartAreas[0], timeList.Count, minPulseTime, avgPulseTime, fullMoveTime, maxInterruptYVal, rodMoveLength);


            Color cwColor = Color.SteelBlue;
            Color ccwColor = Color.LimeGreen;

            /*
            if(record.RodPosition == RodPosition.Up) // штанга двигалась вверх
            {
                interruptSerie.Color = cwColor;
               
            } // if
            else // штанга двигалась вниз
            {
                interruptSerie.Color = ccwColor;
            } // else
            */

            if (record.InterruptData.Count > 0)
            {
                // тут раскрашиваем график направлениями движения
                RodPosition lastPos = RodPosition.Broken;

              //  System.Diagnostics.Debug.Assert(record.InterruptData.Count == record.Directions.Count);

                for (int i = 0; i < record.Directions.Count; i++)
                {
                    RodPosition pos = (RodPosition)record.Directions[i];
                    Color curSerieColor = pos == RodPosition.Up ? cwColor : ccwColor;

                    int curIdx = (i + offsetLabelIndex + 1);

                    if (interruptSerie.Points.Count > curIdx)
                    {
                        interruptSerie.Points[curIdx].Color = curSerieColor;
                        if (lastPos != pos && curIdx > 0)
                        {
                            lastPos = pos;
                            interruptSerie.Points[curIdx - 1].MarkerColor = Color.Red;
                            interruptSerie.Points[curIdx - 1].MarkerStyle = MarkerStyle.Circle;
                            interruptSerie.Points[curIdx - 1].MarkerSize = 6;
                        }
                    }

                } // for
                /*
                if (record.DirectionTimes.Count > 0)
                {

                    RodPosition initialDirection = record.RodPosition; // первоначальное движение штанги

                    // проходим по всем точкам графика, и меняем им цвет, в зависимости от направления движения штанги

                    int pointsIterator = 0;

                    Color curSerieColor = initialDirection == RodPosition.Up ? cwColor : ccwColor;

                    for (int i = 0; i < record.DirectionTimes.Count; i++)
                    {
                        int changeTime = record.DirectionTimes[i];
                        RodPosition changeTo = (RodPosition)record.Directions[i];

                        // теперь заменяем все точки, время которых меньше, чем время изменения вращения, на нужный цвет.
                        for (int k = pointsIterator; k < interruptSerie.Points.Count; k++, pointsIterator++)
                        {
                            interruptSerie.Points[k].Color = curSerieColor;

                            int changeDirectionIdx = k;

                            if (Convert.ToInt32(interruptSerie.Points[k].XValue) >= (changeTime + record.DataArrivedTime))
                            {
                                
                                // тут была проверка скорости !!!

                                interruptSerie.Points[changeDirectionIdx].MarkerColor = Color.Red;
                                interruptSerie.Points[changeDirectionIdx].MarkerStyle = MarkerStyle.Circle;
                                interruptSerie.Points[changeDirectionIdx].MarkerSize = 6;

                                pointsIterator++;
                                break;
                            }

                        } // for

                        if (changeTo == RodPosition.Up)//initialDirection)
                        {
                            curSerieColor = cwColor;
                        }
                        else
                        {
                            curSerieColor = ccwColor;
                        }


                    } // for

                    // забиваем все остальные точки
                    for (int k = pointsIterator; k < interruptSerie.Points.Count; k++, pointsIterator++)
                    {
                        interruptSerie.Points[k].Color = curSerieColor;
                    } // for

                } // record.DirectionTimes.Count > 0
                else
                {
                    // нет смены направления движения
                    RodPosition initialDirection = record.RodPosition; // первоначальное движение штанги

                    // проходим по всем точкам графика, и меняем им цвет, в зависимости от направления движения штанги
                    Color curSerieColor = initialDirection == RodPosition.Up ? cwColor : ccwColor;
                    for (int k = 0; k < interruptSerie.Points.Count; k++)
                    {
                        interruptSerie.Points[k].Color = curSerieColor;
                    }
                } // else 
                */
            } // if



            // теперь рисуем свои метки на Y осях токов
            if (record.CurrentTimes.Count > 0) // есть токи
            {

                AddCustomLabelsOfCurrent(vcf.chart.ChartAreas[1], maxCurrentValue);
                AddCustomLabelsOfCurrent(vcf.chart.ChartAreas[2], maxCurrentValue);
                AddCustomLabelsOfCurrent(vcf.chart.ChartAreas[3], maxCurrentValue);
            }

            vcf.SetChartOfCurrentAvailable(record.CurrentTimes.Count > 0);
            vcf.SetEthalonAvailable(record.EthalonData.Count > 0);

            if (modal)
            {
                vcf.ShowDialog();
            }
            else
            {
                vcf.Show();
                vcf.BringToFront();
            }

        }

        /// <summary>
        /// добавление пользовательских меток значения тока на график
        /// </summary>
        /// <param name="area"></param>
        /// <param name="maxCurrentValue"></param>
        private void AddCustomLabelsOfCurrent(ChartArea area, int maxCurrentValue)
        {
            int currentLabelsCount = 6;
            int labelStep = (maxCurrentValue) / (currentLabelsCount);
            int currentStep = (maxCurrentValue) / (currentLabelsCount);

            area.AxisY.CustomLabels.Clear();
            area.AxisY.Interval = labelStep;
            area.AxisY.IntervalType = DateTimeIntervalType.Number; // тип интервала

            float startOffset = -labelStep / 2;
            float endOffset = labelStep / 2;
            int counter = 0;

            for (int i = 0; i <= currentLabelsCount; i++)
            {
                string labelText = String.Format("{0:0.000}A", GetCurrentFromADC(currentStep * counter));
                CustomLabel cLabel = new CustomLabel(startOffset, endOffset, labelText, 0, LabelMarkStyle.None);
                area.AxisY.CustomLabels.Add(cLabel);
                startOffset = startOffset + labelStep;
                endOffset = endOffset + labelStep;
                counter++;
            }
        }

        /// <summary>
        /// получение значения тока из значения АЦП
        /// </summary>
        /// <param name="adcVAL"></param>
        /// <returns></returns>
        private float GetCurrentFromADC(float adcVAL)
        {
            float result = 0;
            float CURRENT_DIVIDER = 1000.0f;
            float COEFF_1 = 5.0f;
            float currentCoeff = Convert.ToSingle(nudCurrentCoeff.Value) / 1000;

            float intermediate = (COEFF_1 * adcVAL) / currentCoeff;

            result = (intermediate / CURRENT_DIVIDER);

            return result;
        }


        /// <summary>
        /// привязка DataGrid к лог-файлу
        /// </summary>
        public Dictionary<DataGridView, LogInfo> gridToListCollection = new Dictionary<DataGridView, LogInfo>();


        /// <summary>
        /// читает два байта
        /// </summary>
        /// <param name="content">байтовый массив</param>
        /// <param name="idx">индекс чтения в массиве</param>
        /// <returns>возвращает беззнаковое двухбайтовое целое</returns>
        private UInt16 Read16(List<byte> content, int idx)
        {
            UInt16 result = content[idx + 1];
            result <<= 8;
            result |= content[idx];

            return result;
        }

        /// <summary>
        /// читает 4 байта
        /// </summary>
        /// <param name="content">байтовый массив</param>
        /// <param name="idx">индекс чтения в массиве</param>
        /// <returns>возвращает четырёхбайтовое целое</returns>
        private int Read32(List<byte> content, int idx)
        {
            int result = (content[idx + 3] << 24) | (content[idx + 2] << 16) | (content[idx + 1] << 8) | content[idx];
            return result;
        }

        /// <summary>
        /// Добавляет запись по прерыванию в таблицу
        /// </summary>
        /// <param name="record"></param>
        /// <param name="targetGrid"></param>
        /// <param name="addToColumnName"></param>
        /// <param name="computeMotoresurcePercents"></param>
        private void AddInterruptRecordToList(InterruptRecord record, DataGridView targetGrid, string addToColumnName, bool computeMotoresurcePercents)
        {
            if (record == null)
                return;

            if (!gridToListCollection.ContainsKey(targetGrid))
            {
                LogInfo linf = new LogInfo();
                linf.addToColumnName = addToColumnName;
                linf.computeMotoresurcePercents = computeMotoresurcePercents;
                gridToListCollection[targetGrid] = linf;
            }

            //Тут добавление в список в таблицу

            gridToListCollection[targetGrid].list.Add(record);

        }

        /// <summary>
        /// Показывает лог-файл
        /// </summary>
        /// <param name="content"></param>
        /// <param name="targetGrid"></param>
        /// <param name="addToColumnName"></param>
        /// <param name="computeMotoresurcePercents"></param>
        /// <param name="callback"></param>
        /// <param name="stopAfterFirstRecord"></param>
        private void ShowLogFile(List<byte> content, DataGridView targetGrid, string addToColumnName, bool computeMotoresurcePercents, ShowInterruptInfo callback, bool stopAfterFirstRecord)
        {

            //            string myString = System.Text.Encoding.UTF8.GetString(content.ToArray());
            //            System.Diagnostics.Debug.Print(myString);


            if (targetGrid != null)
            {
                ClearInterruptsList(targetGrid);
            }

            // парсим лог-файл
            int readed = 0;
            InterruptInfo currentInterruptInfo = null;
            InterruptRecord curRecord = null;

            try
            {
                bool stopped = false;

                while (readed < content.Count && !stopped) // читаем побайтово
                {
                        string message = "Загружаем лог: {0}% из {1} байт...";
                        int percents = (readed * 100) / content.Count;
                        this.sdFiles.toolStripStatusLabel1.Text = String.Format(message, percents, content.Count);

                    Application.DoEvents();

                    byte curByte = content[readed];
                    readed++;

                    LogRecordType recType = (LogRecordType)curByte;

                    switch (recType) // смотрим тип записи
                    {
                        case LogRecordType.InterruptInfoBegin: // начало информации по прерыванию
                            {
                                currentInterruptInfo = new InterruptInfo();
                            }
                            break;

                        case LogRecordType.InterruptTime: // время прерывания
                            {
                                //System.Diagnostics.Debug.Assert(currentInterruptInfo != null);
                                if (currentInterruptInfo == null)
                                {
                                    stopped = true;
                                    break;
                                }

                                // далее идут 7 байт времени
                                byte dayOfMonth = content[readed]; readed++;
                                byte month = content[readed]; readed++;

                                UInt16 year = Read16(content, readed); readed += 2;

                                byte hour = content[readed]; readed++;
                                byte minute = content[readed]; readed++;
                                byte second = content[readed]; readed++;

                                currentInterruptInfo.InterruptTime = new DateTime(year, month, dayOfMonth, hour, minute, second);

                            }
                            break;

                        case LogRecordType.SystemTemperature: // температура системы
                            {
                                //System.Diagnostics.Debug.Assert(currentInterruptInfo != null);
                                if (currentInterruptInfo == null)
                                {
                                    stopped = true;
                                    break;
                                }

                                // далее идут два байта температуры
                                byte value = content[readed]; readed++;
                                byte fract = content[readed]; readed++;

                                float fVal = value * 100 + fract;
                                fVal /= 100;

                                currentInterruptInfo.SystemTemperature = fVal;
                            }
                            break;

                        case LogRecordType.InterruptRecordBegin: // запись по прерыванию
                            {
                                //System.Diagnostics.Debug.Assert(currentInterruptInfo != null);
                                if (currentInterruptInfo == null)
                                {
                                    stopped = true;
                                    break;
                                }

                                curRecord = new InterruptRecord();
                                curRecord.InterruptInfo = currentInterruptInfo;
                            }
                            break;

                        case LogRecordType.ChannelNumber: // номер канала
                            {
                                //System.Diagnostics.Debug.Assert(curRecord != null);
                                if (curRecord == null)
                                {
                                    stopped = true;
                                    break;
                                }

                                // далее идёт байт номера канала
                                curRecord.ChannelNumber = content[readed]; readed++;
                            }
                            break;

                        case LogRecordType.PreviewCount: // кол-во записей превью по току
                            {
                                //System.Diagnostics.Debug.Assert(curRecord != null);
                                if (curRecord == null)
                                {
                                    stopped = true;
                                    break;
                                }

                                // далее идёт байт номера канала
                                curRecord.PreviewCount = Read16(content, readed); readed += 2;
                            }
                            break;

                        case LogRecordType.RodMoveLength: // величина перемещения привода
                            {
                                //System.Diagnostics.Debug.Assert(curRecord != null);
                                if (curRecord == null)
                                {
                                    stopped = true;
                                    break;
                                }

                                // далее идёт байт номера канала
                                curRecord.RodMoveLength = Read32(content, readed); readed += 4;
                            }
                            break;


                        case LogRecordType.ChannelInductiveSensorState: // состояние индуктивных датчиков
                            {
                                //System.Diagnostics.Debug.Assert(curRecord != null);
                                if (curRecord == null)
                                {
                                    stopped = true;
                                    break;
                                }

                                // далее идёт байт номера канала
                                //DEPRECATED: curRecord.InductiveSensorState = (InductiveSensorState) content[readed]; readed++;
                                readed++;
                            }
                            break;

                        case LogRecordType.RodPosition: // позиция штанги
                            {
                                //System.Diagnostics.Debug.Assert(curRecord != null);
                                if (curRecord == null)
                                {
                                    stopped = true;
                                    break;
                                }

                                // далее идёт позиция штанги
                                curRecord.RodPosition = (RodPosition)content[readed]; readed++;
                            }
                            break;

                        case LogRecordType.MoveTime: // время перемещения штанги
                            {
                                // System.Diagnostics.Debug.Assert(curRecord != null);
                                if (curRecord == null)
                                {
                                    stopped = true;
                                    break;
                                }

                                // далее идут четыре байта времени движения штанги
                                curRecord.MoveTime = Read32(content, readed); readed += 4;

                            }
                            break;

                        case LogRecordType.RelayTriggeredTime: // время срабатывания защиты
                            {
                                // далее идут 7 байт времени
                                byte dayOfMonth = content[readed]; readed++;
                                byte month = content[readed]; readed++;

                                UInt16 year = Read16(content, readed); readed += 2;

                                byte hour = content[readed]; readed++;
                                byte minute = content[readed]; readed++;
                                byte second = content[readed]; readed++;
                            }
                            break;

                        case LogRecordType.Motoresource: // информация по моторесурсу
                            {
                                //System.Diagnostics.Debug.Assert(curRecord != null);
                                if (curRecord == null)
                                {
                                    stopped = true;
                                    break;
                                }

                                // далее идут 4 байта моторесурса
                                curRecord.Motoresource = Read32(content, readed); readed += 4;

                            }
                            break;

                        case LogRecordType.EthalonNumber: // номер эталона
                            {
                                //System.Diagnostics.Debug.Assert(curRecord != null);
                                if (curRecord == null)
                                {
                                    stopped = true;
                                    break;
                                }

                                // байт номера эталона
                                curRecord.EthalonCompareNumber = (EthalonCompareNumber)content[readed]; readed++;

                            }
                            break;

                        case LogRecordType.CompareResult: // результат сравнения с эталоном
                            {
                                //System.Diagnostics.Debug.Assert(curRecord != null);
                                if (curRecord == null)
                                {
                                    stopped = true;
                                    break;
                                }

                                // байт результатов сравнения с эталоном
                                curRecord.EthalonCompareResult = (EthalonCompareResult)content[readed]; readed++;
                            }
                            break;

                        case LogRecordType.EthalonDataFollow: // данные эталона
                            {
                                // следом идут данные эталона, с которым сравнивали
                                //System.Diagnostics.Debug.Assert(curRecord != null);
                                if (curRecord == null)
                                {
                                    stopped = true;
                                    break;
                                }

                                curRecord.EthalonData.Clear();

                                // следом идут 2 байта длины данных
                                int dataLen = Read16(content, readed); readed += 2;

                                // далее идут пачки по 4 байта записей по прерыванию
                                for (int k = 0; k < dataLen; k++)
                                {
                                    int curInterruptData = Read32(content, readed); readed += 4;
                                    curRecord.EthalonData.Add(curInterruptData);

                                } // for


                            }
                            break;

                        case LogRecordType.DataArrivedTime: // смещение от начала данных по току до начала данных по прерываниям
                            {
                                //System.Diagnostics.Debug.Assert(curRecord != null);
                                if (curRecord == null)
                                {
                                    stopped = true;
                                    break;
                                }

                                // далее идёт смещение, в миллисекундах, 4 байта
                                curRecord.DataArrivedTime = Read32(content, readed); readed += 4;
                            }
                            break;

                        case LogRecordType.OscDataFollow: // идут данные по току для канала
                            {
                                //System.Diagnostics.Debug.Assert(curRecord != null);
                                if (curRecord == null)
                                {
                                    stopped = true;
                                    break;
                                }

                                curRecord.CurrentTimes.Clear();
                                curRecord.CurrentData1.Clear();
                                curRecord.CurrentData2.Clear();
                                curRecord.CurrentData3.Clear();

                                // следом идут 2 байта длины данных
                                int dataLen = Read16(content, readed); readed += 2;

                                // далее идут пачки по 4 байта записей по времени сбора записей по току
                                for (int k = 0; k < dataLen; k++)
                                {
                                    int curData = Read32(content, readed); readed += 4;
                                    curRecord.CurrentTimes.Add(curData);

                                } // for

                                // далее идут пачки по 2 байта записей по току канала 1
                                for (int k = 0; k < dataLen; k++)
                                {
                                    int curData = Read16(content, readed); readed += 2;
                                    curRecord.CurrentData1.Add(curData);

                                } // for

                                // далее идут пачки по 2 байта записей по току канала 2
                                for (int k = 0; k < dataLen; k++)
                                {
                                    int curData = Read16(content, readed); readed += 2;
                                    curRecord.CurrentData2.Add(curData);

                                } // for

                                // далее идут пачки по 2 байта записей по току канала 3
                                for (int k = 0; k < dataLen; k++)
                                {
                                    int curData = Read16(content, readed); readed += 2;
                                    curRecord.CurrentData3.Add(curData);

                                } // for

                            }
                            break;

                        case LogRecordType.DirectionData: // идут данные по изменению направления вращения энкодера
                            {
                                //System.Diagnostics.Debug.Assert(curRecord != null);
                                if (curRecord == null)
                                {
                                    stopped = true;
                                    break;
                                }

                                //curRecord.DirectionTimes.Clear();
                                curRecord.Directions.Clear();

                                // следом идут 2 байта длины данных
                                int dataLen = Read16(content, readed); readed += 2;

                                // далее идут пачки по 1 байту записей с изменившимся направлением движения энкодера
                                for (int k = 0; k < dataLen; k++)
                                {
                                    int curData = content[readed]; readed++;
                                    curRecord.Directions.Add(curData);

                                } // for

                            }
                            break;

                        case LogRecordType.InterruptDataBegin: // данные импульсов прерывания
                            {
                                //System.Diagnostics.Debug.Assert(curRecord != null);
                                if (curRecord == null)
                                {
                                    stopped = true;
                                    break;
                                }

                                // начало данных по прерыванию
                                curRecord.InterruptData.Clear();

                                // следом идут 2 байта длины данных
                                int dataLen = Read16(content, readed); readed += 2;

                                // далее идут пачки по 4 байта записей по прерыванию
                                for (int k = 0; k < dataLen; k++)
                                {
                                    int curInterruptData = Read32(content, readed); readed += 4;
                                    curRecord.InterruptData.Add(curInterruptData);

                                } // for

                            }
                            break;

                        case LogRecordType.InterruptDataEnd: // конец данных импульсов прерывания
                            {
                                // конец данных
                            }
                            break;

                        case LogRecordType.InterruptRecordEnd: // конец записи по прерыванию
                            {
                                if (targetGrid != null)
                                {
                                    AddInterruptRecordToList(curRecord, targetGrid, addToColumnName, computeMotoresurcePercents);
                                }

                                if (stopAfterFirstRecord)
                                {
                                    stopped = true;
                                }

                                if (callback != null)
                                {
                                    //System.Diagnostics.Debug.Assert(curRecord != null);
                                    if (curRecord != null)
                                    {
                                        callback(curRecord);
                                    }
                                    else
                                    {
                                        MessageBox.Show("Неправильный формат записи срабатывания защиты!", "Ошибка", MessageBoxButtons.OK, MessageBoxIcon.Error);
                                        return;
                                    }
                                }

                            }
                            break;

                        case LogRecordType.InterruptInfoEnd: // конец информации по одной записи
                            {
                                currentInterruptInfo = null;
                            }
                            break;
                    } // switch
                } // while                
            }

            catch // ошибка разбора лог-файла
            {
                if (targetGrid != null)
                {
                    this.sdFiles.plEmptySDWorkspace.BringToFront();
                }
                MessageBox.Show("Ошибка разбора лог-файла!", "Ошибка", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

            // если попросили отобразить в таблице - отображаем
            if (targetGrid != null && gridToListCollection.ContainsKey(targetGrid) && gridToListCollection[targetGrid].list != null)
            {
                if (gridToListCollection.ContainsKey(targetGrid))
                {
                    targetGrid.RowCount = gridToListCollection[targetGrid].list.Count;
                }

                targetGrid.BringToFront();
            }
        }

        /// <summary>
        /// Таймер запроса содержимого файла
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void tmFileContent_Tick(object sender, EventArgs e)
        {
            tmFileContent.Enabled = false; // выключаем таймер, чтоб не тикал

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
                    if (regs[0] == 1)
                    {
                        // данные готовы, можно из читать
                        regs = new ushort[7]; // 7  сразу

                        // MODBUS_REG_FILE_SIZE1                     40998 // регистр размера файла 1
                        // MODBUS_REG_FILE_SIZE2                     40999 // регистр размера файла 2
                        // MODBUS_REG_FUNCTION_NUMBER                41000 // регистр для номера запрошенной мастером функции (например, выдать список файлов в директории)
                        // MODBUS_REG_READY_FLAG                     41001 // регистр флага готовности запрошенной мастером информации (например, выдано имя одного файла). Мастер проверяет
                        // MODBUS_REG_CONTINUE_FLAG                  41002 // регистр флага необходимости продолжения запроса мастером (например, когда ещё есть имена файлов в списке). Если 0 - то передача от слейва завершена.
                        // MODBUS_REG_FILE_FLAGS                     41003 // регистр флага признаков файла (0 - нет файла, 1 - обычный файл, 2 - папка)
                        // MODBUS_REG_DATA_LENGTH                    41004 // регистр длины выданных или переданных от мастера данных (например, длины имени файла)
                        res = myProtocol.readMultipleRegisters(slave, 998, regs, 7);

                        if ((res == BusProtocolErrors.FTALK_SUCCESS))
                        {
                            // регистры, кроме регистров данных - прочитаны.


                            // признак файла, в нашем случае - его полный размер
                            ushort sizeHVal = regs[0];
                            ushort sizeLVal = regs[1];
                            UInt32 fileSize = sizeHVal; fileSize <<= 16; fileSize |= sizeLVal;
                            sdFiles.toolStripProgressBar1.Maximum = Convert.ToInt32(fileSize);


                            // признак перезапроса
                            ushort continueFlag = regs[4];

                            // читаем сами данные
                            int dataLen = regs[6];
                            ushort[] data = new ushort[dataLen];

                            if (dataLen > 0) // есть имя файла или папки
                            {

                                int regsToRead = dataLen / 2;
                                if (dataLen % 2 > 0)
                                {
                                    regsToRead++;
                                }
                                // MODBUS_REG_DATA                           41005 // регистр, начиная с которого идут данные (например, имя файла)
                                res = myProtocol.readMultipleRegisters(slave, 1005, data, regsToRead);

                                if ((res == BusProtocolErrors.FTALK_SUCCESS))
                                {
                                    // размер файла есть, размер полученных данных - есть, увеличиваем прогресс-бар
                                    sdFiles.toolStripProgressBar1.Value += dataLen;


                                    // данные прочитаны, можно разбирать содержимое файла


                                    for (int k = 0; k < regsToRead; k++, dataLen -= 2)
                                    {
                                        ushort reg = data[k];
                                        byte highVal = (byte)((reg & 0xFF00) >> 8);
                                        byte lowVal = (byte)(reg & 0x00FF);

                                        if (dataLen < 2)
                                        {
                                            // в регистре валиден только старший байт
                                            fileContent.Add(highVal);
                                        }
                                        else
                                        {
                                            // оба байта в регистре валидны
                                            fileContent.Add(highVal);
                                            fileContent.Add(lowVal);
                                        }

                                    } // for


                                    // теперь смотрим - если есть флаг перезапроса - то перезапрашиваем

                                    if (continueFlag == 1) // перезапрашиваем
                                    {
                                        DoRequestFile(this.fileNameToRequest);
                                    }
                                    else
                                    {
                                        hasModbusRequest = false;
                                        // закончили запрос
                                        ShowMessageInStatusBar(" ФАЙЛ ПОЛУЧЕН ", Color.Lime);

                                        ViewFile(fileContent);

                                        // MessageBox.Show("Список файлов получен!", "Сообщение", MessageBoxButtons.OK, MessageBoxIcon.Information);

                                    }

                                }
                                else
                                {
                                    hasModbusRequest = false;
                                    ShowProtocolError(res, "Ошибка чтения регистров: ");
                                }
                            } // if (dataLen > 0)
                            else
                            {
                                // нет имени файла, проверяем, надо ли перезапрашивать?
                                if (continueFlag == 1) // перезапрашиваем
                                {
                                    DoRequestFile(this.fileNameToRequest);
                                }
                                else
                                {
                                    hasModbusRequest = false;
                                    // закончили запрос
                                    ShowMessageInStatusBar(" ФАЙЛ ПОЛУЧЕН ", Color.Lime);

                                    //MessageBox.Show("Список файлов получен!", "Сообщение", MessageBoxButtons.OK, MessageBoxIcon.Information);

                                }
                            }

                        } // if ((res == BusProtocolErrors.FTALK_SUCCESS))
                        else
                        {
                            hasModbusRequest = false;
                            ShowProtocolError(res, "Ошибка чтения регистров: ");
                        }

                    }
                    else
                    {
                        // надо перезапросить, ожидаем готовность данных
                        tmFileContent.Enabled = true;
                    }
                }
                else
                {
                    hasModbusRequest = false;
                    // не прочитано
                    ShowProtocolError(res, "Ошибка чтения списка файлов: ");

                }

            }
            else
            {
                hasModbusRequest = false;
                tmFileContent.Enabled = false;
                ShowMessageInStatusBar(" РАЗЪЕДИНЕНО ", Color.Red);
            }
        }

        /// <summary>
        /// форма закрывается
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void MainForm_FormClosed(object sender, FormClosedEventArgs e)
        {
            // сохраняем конфиг в файл
            Config.Instance.Save();
        }

        private delegate void SafeCallDelegate(string str);
        private delegate void SafeCallVoidDelegate();
        private void OnTryToConnectToPort(string portName)
        {
            if(this.InvokeRequired)
            {
                Invoke(new SafeCallDelegate(OnTryToConnectToPort), new object[] { portName });
                return;
            }

            ShowInfo("Ищем устройство на порту " + portName + "...");
        }

        private void OnFindDeviceConnectMessage(string msg)
        {
            if (this.InvokeRequired)
            {
                Invoke(new SafeCallDelegate(OnFindDeviceConnectMessage), new object[] { msg });
                return;
            }

            ShowInfo(msg);
        }

        private void OnSetSelectedPort(string portname)
        {
            if (this.InvokeRequired)
            {
                Invoke(new SafeCallDelegate(OnSetSelectedPort), new object[] { portname });
                return;
            }

            cmbComPort.SelectedIndex = cmbComPort.FindStringExact(portname);
        }

        private void OnSearchDone()
        {
            if (this.InvokeRequired)
            {
                Invoke(new SafeCallVoidDelegate(OnSearchDone));
                return;
            }

            FindSerial.Enabled = true;
            btnReloadPorts.Enabled = true;
            UpdateControlButtons(ConnectionMode.None);
            MessageBox.Show("Поиск устройства завершён.");
        }

        private long answerTimer = 0;
        private long waitAnswerTimeout = 20000;
        private int speed = 57600;
        private SerialPort port = null;
        private List<byte> COMAnswer = new List<byte>();
        private bool deviceFound = false;
        private Thread openPortThread = null;


        private void doClosePort()
        {
            if (this.port != null && this.port.IsOpen)
            {
                System.Diagnostics.Debug.WriteLine("TRANSPORT: CLOSE PORT...");

                this.port.DataReceived -= new SerialDataReceivedEventHandler(findDevice_DataReceived);

                while (!(this.port.BytesToRead == 0 && this.port.BytesToWrite == 0))
                {
                    this.port.DiscardInBuffer();
                    this.port.DiscardOutBuffer();
                }

                this.port.Close();

                System.Diagnostics.Debug.WriteLine("TRANSPORT: CLOSE PORT, BEFORE WHILE...");

                while (this.port.IsOpen) { Application.DoEvents(); }

                System.Diagnostics.Debug.WriteLine("TRANSPORT: PORT CLOSED.");
            }
        }

        /// <summary>
        /// получили данные из порта в процессе поиска контроллера
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        void findDevice_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            try
            {
                SerialPort sp = (SerialPort)sender;
                int cnt = sp.BytesToRead;
                if (cnt > 0)
                {
                    byte[] bReceived = new byte[cnt];

                    for (int i = 0; i < cnt; i++)
                        bReceived[i] = (byte)sp.ReadByte();

                    COMAnswer.AddRange(bReceived);

                    while (true)
                    {
                        int idx = Array.IndexOf(COMAnswer.ToArray(), (byte)'\n');
                        if (idx != -1)
                        {
                            string line = System.Text.Encoding.UTF8.GetString(COMAnswer.ToArray(), 0, idx);
                            COMAnswer.RemoveRange(0, idx + 1);
                            line = line.Trim();
                            if (line.StartsWith("UROV "))
                            {
                                this.deviceFound = true;
                                break;
                            }

                        }
                        else
                            break;
                    }


                    Thread.Sleep(10);
                }
            }
            catch (Exception)
            {

            }
        }

        /// <summary>
        /// начинаем искать подсоединённый контроллер
        /// </summary>
        /// <param name="o"></param>
        private void TryFindDevice(object o)
        {

            string foundPortName = "";

            string[] ports = SerialPort.GetPortNames();
            foreach (string portname in ports)
            {
                string pname = portname;

                System.Diagnostics.Debug.WriteLine("TRANSPORT: TRY TO CONNECT TO " + pname + "...");

                OnTryToConnectToPort(portname);


                // надо искать порт
                    port = new SerialPort(portname);
                    port.BaudRate = speed;
                    port.DataReceived += new SerialDataReceivedEventHandler(findDevice_DataReceived);

                    //if (this.withHandshake) // пересбрасываем порт, если надо
                    {
                        port.DtrEnable = true;
                    }

                try
                {
                    port.Open();

                    while (!port.IsOpen)
                    {
                        Thread.Sleep(100);
                    }

                    answerTimer = DateTime.Now.Ticks / TimeSpan.TicksPerMillisecond;

                    while (true)
                    {
                        if (deviceFound)
                        {
                            foundPortName = portname;
                            break;
                        }

                        if (DateTime.Now.Ticks / TimeSpan.TicksPerMillisecond - answerTimer >= waitAnswerTimeout)
                            break;
                    }

                    if (!deviceFound)
                    {
                        doClosePort();
                        port = null;
                    }
                    else
                    {
                        break;
                    }
                }
                catch (Exception)
                {
                    doClosePort();
                    this.port = null;
                }

            } // foreach

            if (deviceFound)
            {
                doClosePort();
                this.port = null;

                OnFindDeviceConnectMessage("Устройство найдено на порту " + foundPortName);
                OnSetSelectedPort(foundPortName);
            }
            else
            {
                doClosePort();
                this.port = null;
                //                DoOnConnect(false, "Can't find device!");
                OnFindDeviceConnectMessage("Устройство не найдено!");
            }

            OnSearchDone();
        }

        private void FindSerial_Click(object sender, EventArgs e)
        {
            FindSerial.Enabled = false;
            btnCloseSerial.Enabled = false;
            btnOpenSerial.Enabled = false;
            btnCloseTCP.Enabled = false;
            btnOpenTCP.Enabled = false;
            btnReloadPorts.Enabled = false;

            try
            {
                speed = int.Parse(cmbBaudRate.Text, CultureInfo.CurrentCulture);
            }
            catch (Exception)
            {
                speed = 57600;
            }

            openPortThread = new Thread(TryFindDevice);
            openPortThread.Start(null);
        }

        private DateTime controllerDateTime = DateTime.Now;

        private void tmDateTime_Tick(object sender, EventArgs e)
        {
            this.controllerDateTime = this.controllerDateTime.AddMilliseconds(tmDateTime.Interval);
            string dateTimeString = this.controllerDateTime.ToString("dd.MM.yyyy HH:mm:ss");

            tsDateTime.Text = dateTimeString;

            tbSystemTime.Text = DateTime.Now.ToString("dd.MM.yyyy HH:mm:ss");
        }


        private void SetCurrentTabPageHint()
        {
            switch (tabPages.SelectedIndex)
            {
                case 0:
                    {
                        lblCurrentTabPageHint.Text = "Файл: Работа с параметрами прибора УРОВ";
                    }
                    break;

                case 1:
                    {
                        lblCurrentTabPageHint.Text = "Файл: Дополнительные настройки прибора УРОВ";
                    }
                    break;

                case 2:
                    {
                        lblCurrentTabPageHint.Text = "Файл: Установка параметров подключения к прибору УРОВ";
                    }
                    break;
            }
        }

        private void tabPages_SelectedIndexChanged(object sender, EventArgs e)
        {
            SetCurrentTabPageHint();
        }

        private void btnSetDeviceTime_Click(object sender, EventArgs e)
        {
            SetDeviceTime();
        }

        /// <summary>
        /// устанавливаем время прибора
        /// </summary>
        private void SetDeviceTime()
        {
            if(myProtocol != null && myProtocol.isOpen())
            {
                hasModbusRequest = true;

                ushort func = Convert.ToUInt16(MBusFunction.SetDeviceTime);
                int slave = Convert.ToInt32(nudModbusSlaveID.Value);
                int numRegs = 6;
                int res; // результат запроса

                // выделяем память под данные
                ushort[] data = new ushort[numRegs];

                // формируем данные
                DateTime userDate = dtpUserTime.Value;
                data[0] = Convert.ToUInt16(userDate.Year);
                data[1] = Convert.ToUInt16(userDate.Month);
                data[2] = Convert.ToUInt16(userDate.Day);
                data[3] = Convert.ToUInt16(userDate.Hour);
                data[4] = Convert.ToUInt16(userDate.Minute);
                data[5] = Convert.ToUInt16(userDate.Second);


                // данные сформированы, записываем их в регистры устройства
                int errorsCount = 0;

                // MODBUS_REG_DATA                           41005 // регистр, начиная с которого идут данные (например, имя файла)
                res = myProtocol.writeMultipleRegisters(slave, 1005, data, numRegs);
                if (!(res == BusProtocolErrors.FTALK_SUCCESS))
                    errorsCount++;

                // MODBUS_REG_FUNCTION_NUMBER                41000 // регистр для номера запрошенной мастером функции (например, выдать список файлов в директории)
                res = myProtocol.writeSingleRegister(slave, 1000, func);
                if (!(res == BusProtocolErrors.FTALK_SUCCESS))
                    errorsCount++;

                if (errorsCount < 1)
                {
                    MessageBox.Show("Время успешно записано в прибор!", "Сообщение", MessageBoxButtons.OK, MessageBoxIcon.Information);
                }
                else
                {
                    ShowProtocolError(res, "Ошибка установки времени прибора: ");
                }

                    hasModbusRequest = false;
            }
        }
    }
}
