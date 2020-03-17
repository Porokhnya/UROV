using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO.Ports;
using System.Threading;
using System.Security.Permissions;
using System.Windows.Forms;
using System.ComponentModel;

namespace UROVConfig
{
    /// <summary>
    /// обработчик события "получена строка из порта"
    /// </summary>
    /// <param name="line"></param>
    public delegate void TransportDataReceived(byte[] data);
    /// <summary>
    /// Обработчик события подсоединения к порту
    /// </summary>
    /// <param name="success"></param>
    /// <param name="message"></param>
    public delegate void ConnectResult(bool success, string message);

    public delegate void TransportDisconnect(ITransport transport);
    public delegate void TryToConnectToPort(string portName);
     
    public abstract class ITransport
    {  
        /// <summary>
        /// соединяется с чем-либо
        /// </summary>
        public abstract void Connect(bool withHandshake, bool findDevice);
        /// <summary>
        /// состояние соединения
        /// </summary>
        /// <returns></returns>
        public abstract bool Connected();

        /// <summary>
        /// передаёт строку
        /// </summary>
        /// <param name="line"></param>
        /// <returns></returns>
        public abstract bool WriteLine(string line);

        public abstract bool Write(byte[] data, int length);

        /// <summary>
        /// отсоединяемся
        /// </summary>
        public abstract void Disconnect();

        public TransportDataReceived OnDataReceived;
        public ConnectResult OnConnect;
        public TransportDisconnect OnDisconnect;
        public TryToConnectToPort OnTryToConnectToPort;
    }

    public class SerialPortTransport : ITransport
    {
        private SerialPort port = null;
        private Thread openPortThread = null;
        private bool hasWriteError = false;

        private bool withHandshake = false;
        private bool withFindDevice = false;
        private int speed;

        private long answerTimer = 0;
        private long waitAnswerTimeout = 2000;
        private bool deviceFound = false;
        private List<byte> COMAnswer = new List<byte>();
        private bool inFindDeviceMode = false;

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
                            if(line.StartsWith("UROV v."))
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


        private void TryFindDevice(object o)
        {
            SerialPort s = (SerialPort)o;
            

            string[] ports = SerialPort.GetPortNames();
            foreach (string portname in ports)
            {
                string pname = portname;

                if (s != null) // уже передали нормальный порт, не надо искать
                {
                    pname = s.PortName;
                    if (this.withHandshake)
                    {
                        s.DtrEnable = true;
                    }
                }

                System.Diagnostics.Debug.WriteLine("TRANSPORT: TRY TO CONNECT TO " + pname + "...");

                OnTryToConnectToPort?.Invoke(portname);

                this.deviceFound = false;

                if (s == null) // надо искать порт
                {
                    this.port = new SerialPort(portname);
                    this.port.BaudRate = this.speed;
                    this.port.DataReceived += new SerialDataReceivedEventHandler(findDevice_DataReceived);

                    if (this.withHandshake) // пересбрасываем порт, если надо
                    {
                        this.port.DtrEnable = true;
                    }
                }

                try
                {
                    this.port.Open();

                    while (!this.port.IsOpen)
                    {
                        Thread.Sleep(100);
                    }

                    answerTimer = DateTime.Now.Ticks / TimeSpan.TicksPerMillisecond;
                    WriteLine("GET=VER");

                    while(true)
                    {
                        if (deviceFound)
                            break;

                        if (DateTime.Now.Ticks / TimeSpan.TicksPerMillisecond - answerTimer >= waitAnswerTimeout)
                            break;
                    }

                    if(!deviceFound)
                    {
                        doClosePort();
                        this.port = null;
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

                if(s != null)
                {
                    break; // не надо ничего больше искать, передали готовый порт. Если не получилось к нему соединиться - значит, не судьба
                }

            } // foreach

            if (deviceFound)
            {
                this.port.DataReceived -= new SerialDataReceivedEventHandler(findDevice_DataReceived);
                this.port.DataReceived += new SerialDataReceivedEventHandler(port_DataReceived);
                this.inFindDeviceMode = false;

                DoOnConnect(true, "");
                WriteLine("GET=VER");
            }
            else
            {
                doClosePort();
                this.port = null;
                DoOnConnect(false, "Can't find device!");
            }
        }

       
        private void DoOnConnect(bool succ, string message)
        {
            System.Diagnostics.Debug.WriteLine("TRANSPORT: INVOKED CONNECT EVENT...");

            openPortThread = null;
            //if (this.OnConnect != null)
            {
                this.OnConnect?.Invoke(succ, message);
            }
        }
        public override void  Connect(bool handshakeEnabled, bool findDevice)
        {
            this.withHandshake = handshakeEnabled;
            this.withFindDevice = findDevice;

            this.hasWriteError = false;
            this.inFindDeviceMode = false;

            if (withFindDevice)
            {
                this.inFindDeviceMode = true;
                openPortThread = new Thread(TryFindDevice);
                openPortThread.Start(null);
            }
            else
            {
                this.inFindDeviceMode = true;
                openPortThread = new Thread(TryFindDevice);
                openPortThread.Start(this.port);
            }
 	        
        }
       
        
        private void doClosePort()
        {
            if (this.port != null && this.port.IsOpen)
            {
                System.Diagnostics.Debug.WriteLine("TRANSPORT: CLOSE PORT...");

                this.port.DataReceived -= new SerialDataReceivedEventHandler(port_DataReceived);
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

        public override void Disconnect()
        {
            doClosePort();
            CallDisconnectEvent();
        }

        private void CallDisconnectEvent()
        {
            System.Diagnostics.Debug.WriteLine("TRANSPORT: CallDisconnectEvent BEGIN...");

            if (this.port == null || !this.port.IsOpen)
            {
                //if (this.OnDisconnect != null)
                {
                    System.Diagnostics.Debug.WriteLine("TRANSPORT: InvokedDisconnectEvent...");
                    this.OnDisconnect?.Invoke(this);
                }
            }

            System.Diagnostics.Debug.WriteLine("TRANSPORT: CallDisconnectEvent END.");
        }

        public override bool Connected()
        {
            if (this.port == null)
                return false;

            if (this.inFindDeviceMode)
                return false;

            return this.port.IsOpen && !this.hasWriteError;
        }

        public override bool Write(byte[] data, int length)
        {
            try
            {
                for(int i=0;i<length;i++)
                {
                    this.port.Write(data, i, 1);
                    Thread.Sleep(0);
                }
            }
            catch (Exception)
            {
                this.hasWriteError = true;

                doClosePort();
                CallDisconnectEvent();
                return false;
            }
            return true;
        }

        public override bool WriteLine(string line)
        {
            try
            {
                System.Diagnostics.Debug.WriteLine("=> COM: " + line);
                this.port.WriteLine(line);
            }
            catch (Exception)
            {
                this.hasWriteError = true;

                doClosePort();
                CallDisconnectEvent();
                return false; 
            }
            return true;
        }



       public SerialPortTransport(string portname, int speed)
        {
            this.speed = speed;

            if (portname.Length > 0) // если сразу вызвали для порта, то создаём порт, потому что искать его - не надо
            {
                this.port = new SerialPort(portname);
                this.port.BaudRate = speed;
                this.port.DataReceived += new SerialDataReceivedEventHandler(findDevice_DataReceived);
                
            }
        }
       

        void port_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            try
            {
                SerialPort sp = (SerialPort)sender;
                int cnt = sp.BytesToRead;
                if (cnt > 0)
                {
                    byte[] bReceived = new byte[cnt];

                    for (int i = 0; i < cnt; i++)
                        bReceived[i] = (byte) sp.ReadByte();

                    /*
                    ThreadPool.QueueUserWorkItem(
                        new WaitCallback(delegate (object state)
                        {
                           // if (this.OnDataReceived != null)
                                this.OnDataReceived?.Invoke(bReceived);

                        }), null);

    */
                    this.OnDataReceived?.Invoke(bReceived);

                    //   Thread.Sleep(10);
                }
            }
            catch (Exception)
            {
               
            }
        }
    }
}
