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

        private void btnREGTest_Click(object sender, EventArgs e)
        {
            if (myProtocol == null || !myProtocol.isOpen())
                return;

            short[] readVals = new short[64];
            int startRdReg = 0; // стартовый регистр для чтения
            int numRdRegs = 24; // кол-во регистров для чтения
            int slave = 100; // номер слейва

            int res = myProtocol.readMultipleRegisters(slave, startRdReg, readVals, numRdRegs);    // 03  Считать число из регистров по адресу  40000 -49999
            ShowInfo("Результат: " + (BusProtocolErrors.getBusProtocolErrorText(res) + "\r\n"));
        }
    }
}
