using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.IO.Ports;
using System.Globalization;
using System.Windows.Forms.DataVisualization.Charting;

namespace UROVConfig
{

    public partial class MainForm : Form
    {

        private ToolStripMenuItem lastSelectedPort = null;
        private ITransport currentTransport = null;
        private ConnectForm connForm = null;
        private const char PARAM_DELIMITER = '|';
        private const string GET_PREFIX = "GET=";
        private const string SET_PREFIX = "SET=";

        public MainForm()
        {
            InitializeComponent();
        }

        private void exitToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Application.Exit();
        }

        /// <summary>
        /// закрываем форму ожидания соединения
        /// </summary>
        private void EnsureCloseConnectionForm()
        {
            if (connForm != null)
            {
                connForm.DialogResult = System.Windows.Forms.DialogResult.OK;
            }
            connForm = null;
        }

        void OnTryToConnectToPort(string portname)
        {
            if (InvokeRequired)
            {
                Invoke((MethodInvoker)delegate { OnTryToConnectToPort(portname); });
                return;
            }

            if (connForm != null)
            {
                connForm.lblCurrentAction.Text = "Соединяемся с портом " + portname + "...";
            }
        }

        void OnTransportDisconnect(ITransport transport)
        {
            
            if (InvokeRequired)
            {
                Invoke((MethodInvoker)delegate { OnTransportDisconnect(transport); });
                return;
            }
            

            System.Diagnostics.Debug.WriteLine("TRANSPORT DISCONNECT EVENT!");

            this.InitAfterConnect(false);

            System.Diagnostics.Debug.WriteLine("TRANSPORT DISCONNECT EVENT DONE!");
        }

        /// <summary>
        /// Коннектимся к порту
        /// </summary>
        /// <param name="port">Имя порта</param>
        public void StartConnectToPort(string port, bool withHandshake, bool findDevice)
        {
            //System.Diagnostics.Debug.Assert(currentTransport == null);
            System.Diagnostics.Debug.WriteLine("START CONNECT, HANDLER IN MAIN FORM...");

            if (currentTransport != null)
            {
                System.Diagnostics.Debug.WriteLine("CLOSE EXISTING CONNECTION...");

                currentTransport.Disconnect();

                //TODO: тут надо дождаться, когда транспорт полностью освободится !!!!

                
                while(currentTransport.Connected())
                {
                    Application.DoEvents();
                }
                System.Diagnostics.Debug.WriteLine("EXISTING CONNECTION CLOSED!");
            }
            

            int speed = GetConnectionSpeed();

            // создаём новый транспорт
            currentTransport = new SerialPortTransport(port,speed);
            currentTransport.OnConnect = new ConnectResult(OnCOMConnect);
            currentTransport.OnDataReceived = new TransportDataReceived(OnDataFromCOMPort);
            currentTransport.OnDisconnect = new TransportDisconnect(OnTransportDisconnect);
            currentTransport.OnTryToConnectToPort = new TryToConnectToPort(OnTryToConnectToPort);

            System.Diagnostics.Debug.WriteLine("START TRANSPORT CONNECT....");
            // коннектимся
            currentTransport.Connect(withHandshake, findDevice);

        }

        /// <summary>
        /// Обрабатываем строку, пришедшую из транспорта COM-порта
        /// </summary>
        /// <param name="line"></param>
        private void OnDataFromCOMPort(byte[] data)
        {
            
            if (InvokeRequired)
            {
                Invoke((MethodInvoker)delegate { OnDataFromCOMPort(data); });
                return;
            }
            

            EnsureCloseConnectionForm(); // закрываем форму коннекта, если она ещё не закрыта


            // обрабатываем данные, полученные из порта
            //ProcessPortAnswer(line.Trim());
            ProcessPortAnswer(data);
        }


        // Буфер под ответ с SD-карты
        private List<byte> SDQueryAnswer = new List<byte>();


        /// <summary>
        /// Буфер под ответ с COM-порта
        /// </summary>
        //string COMBuffer = "";

        private void ProcessAnswerLine(string line)
        {
            System.Diagnostics.Debug.WriteLine("<= COM: " + line);

            if (line.StartsWith("UROV v.")) // нашли загрузку ядра, следовательно, можно писать данные
            {
                tbFirmwareVersion.Text = line;
                coreBootFound = true;
            }

            bool isKnownAnswer = line.StartsWith("OK=") || line.StartsWith("ER=");

            this.AddToLog(line, isKnownAnswer); // добавляем данные в лог

            if (!isKnownAnswer) // нам тут ловить нечего
            {
                return;
            }

            if (this.currentCommand.ParseFunction != null)
            {
                Answer a = new Answer(line);
                this.currentCommand.ParseFunction(a);
                this.currentCommand.ParseFunction = null; // освобождаем
                this.currentCommand.CommandToSend = "";
            }

        }

        //        private FileDownloadFlags fileDownloadFlags = FileDownloadFlags.View;

        private List<byte> logContentToShow = null;
        private void DoShowLogFile(ConnectForm frm)
        {
            System.Diagnostics.Debug.Assert(logContentToShow != null);
            ShowLogFile(logContentToShow, this.logDataGrid, "", true,frm,null,false);

            frm.DialogResult = DialogResult.OK;
        }

        private void ViewFile(List<byte> content)
        {
            ShowWaitCursor(false);
            statusProgressBar.Visible = false;
            statusProgressMessage.Visible = false;

            this.btnViewSDFile.Enabled = treeViewSD.SelectedNode != null;
            this.btnDeleteSDFile.Enabled = treeViewSD.SelectedNode != null;
            this.btnListSDFiles.Enabled = true;
            this.treeViewSD.Enabled = true;

            string upStr = this.requestedFileName.ToUpper();

            if (upStr.EndsWith(".LOG"))
            {
                ConnectForm cn = new ConnectForm(true);
                cn.OnConnectFormShown = this.DoShowLogFile;
                cn.lblCurrentAction.Text = "Загружаем лог...";
                logContentToShow = content;
                cn.ShowDialog();                

            }
            else if (upStr.EndsWith(".ETL"))
            {
                CreateChart(content, this.ethalonChart);
                this.plEthalonChart.BringToFront();
            }
        }

        private void SaveEthalonFile(List<byte> content)
        {
            SaveEthalon(requestedFileName, content);
        }

        /*
        /// <summary>
        /// Показываем содержимое файла
        /// </summary>
        /// <param name="content"></param>
        private void DealWithFileContent(List<byte> content)
        {
            switch (fileDownloadFlags)
            {
                case FileDownloadFlags.View:
                    {
                        ShowWaitCursor(false);
                        statusProgressBar.Visible = false;
                        statusProgressMessage.Visible = false;

                        this.btnViewSDFile.Enabled = treeViewSD.SelectedNode != null;
                        this.btnDeleteSDFile.Enabled = treeViewSD.SelectedNode != null;
                        this.btnListSDFiles.Enabled = true;
                        this.treeViewSD.Enabled = true;

                        string upStr = this.requestedFileName.ToUpper();

                        if (upStr.EndsWith(".LOG"))
                        {
                            ShowLogFile(content);
                            
                        }
                        else if (upStr.EndsWith(".ETL"))
                        {
                            CreateEthalonChart(content);
                            this.plEthalonChart.BringToFront();
                        }
                    }
                    break;

                case FileDownloadFlags.DownloadEthalon:
                    {
                        SaveEthalon(requestedFileName, content);
                        fileDownloadFlags = FileDownloadFlags.View;
                    }
                    break;
            } // switch
        }
        */
        private List<int> ethalon0UpData = new List<int>();
        private List<int> ethalon0DwnData = new List<int>();

        private List<int> ethalon1UpData = new List<int>();
        private List<int> ethalon1DwnData = new List<int>();

        private List<int> ethalon2UpData = new List<int>();
        private List<int> ethalon2DwnData = new List<int>();

        private void ClearEthalons()
        {
            ethalon0UpData.Clear();
            ethalon0DwnData.Clear();

            ethalon1UpData.Clear();
            ethalon1DwnData.Clear();

            ethalon2UpData.Clear();
            ethalon2DwnData.Clear();
            this.requestEthalonCounter = 0;
        }

        private UInt16 Read16(List<byte> content, int idx)
        {
            UInt16 result = content[idx + 1];
            result <<= 8;
            result |= content[idx];

            return result;
        }

        private int Read32(List<byte> content, int idx)
        {
            int result = (content[idx + 3] << 24) | (content[idx + 2] << 16) | (content[idx + 1] << 8) | content[idx];
            return result;
        }

        public class LogInfo
        {
            public List<InterruptRecord> list = new List<InterruptRecord>();
            public string addToColumnName;
            public bool computeMotoresurcePercents;
        }

        private Dictionary<DataGridView, LogInfo> gridToListCollection = new Dictionary<DataGridView, LogInfo>();

        

        private void ShowLogFile(List<byte> content, DataGridView targetGrid, string addToColumnName, bool computeMotoresurcePercents, ConnectForm frm, ShowInterruptInfo callback, bool stopAfterFirstRecord)
        {

            string myString = System.Text.Encoding.UTF8.GetString(content.ToArray());
            System.Diagnostics.Debug.Print(myString);


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

                while (readed < content.Count && !stopped)
                {
                    if (frm != null)
                    {
                        string message = "Загружаем лог: {0}% из {1} байт...";
                        int percents = (readed * 100) / content.Count;
                        frm.lblCurrentAction.Text = String.Format(message, percents, content.Count);
                    }

                    Application.DoEvents();

                    byte curByte = content[readed];
                    readed++;

                    LogRecordType recType = (LogRecordType)curByte;

                    switch (recType)
                    {
                        case LogRecordType.InterruptInfoBegin:
                            {
                                currentInterruptInfo = new InterruptInfo();
                            }
                            break;

                        case LogRecordType.InterruptTime:
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

                        case LogRecordType.SystemTemperature:
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

                        case LogRecordType.InterruptRecordBegin:
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

                        case LogRecordType.ChannelNumber:
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

                        case LogRecordType.ChannelInductiveSensorState:
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

                        case LogRecordType.RodPosition:
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

                        case LogRecordType.MoveTime:
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

                        case LogRecordType.Motoresource:
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

                        case LogRecordType.EthalonNumber:
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

                        case LogRecordType.CompareResult:
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

                        case LogRecordType.EthalonDataFollow:
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

                                /*
                                // добавляем фейковую нулевую точку, дублированием первой.
                                // между ними, таким образом, окажется нулевой промежуток времени.
                                if (curRecord.EthalonData.Count > 0)
                                {
                                    int first = curRecord.EthalonData[0];
                                    curRecord.EthalonData.Insert(0, first);
                                }
                                */

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

                        case LogRecordType.InterruptDataBegin:
                            {
                                //System.Diagnostics.Debug.Assert(curRecord != null);
                                if(curRecord == null)
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

                                /*
                                // добавляем фейковую нулевую точку, дублированием первой.
                                // между ними, таким образом, окажется нулевой промежуток времени.
                                if(curRecord.InterruptData.Count > 0)
                                {
                                    int first = curRecord.InterruptData[0];
                                    curRecord.InterruptData.Insert(0, first);
                                }
                                */
                            }
                            break;

                        case LogRecordType.InterruptDataEnd:
                            {
                                // конец данных
                            }
                            break;

                        case LogRecordType.InterruptRecordEnd:
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

                        case LogRecordType.InterruptInfoEnd:
                            {
                                currentInterruptInfo = null;
                            }
                            break;
                    } // switch
                } // while                
            }

            catch
            {
                if (targetGrid != null)
                {
                    this.plEmptySDWorkspace.BringToFront();
                }
                MessageBox.Show("Ошибка разбора лог-файла!", "Ошибка", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

            if (targetGrid != null && gridToListCollection.ContainsKey(targetGrid) && gridToListCollection[targetGrid].list != null)
            {
                if (gridToListCollection.ContainsKey(targetGrid))
                {
                    targetGrid.RowCount = gridToListCollection[targetGrid].list.Count;
                }

                targetGrid.BringToFront();
            }
        }

        private void AddInterruptRecordToList(InterruptRecord record, DataGridView targetGrid, string addToColumnName, bool computeMotoresurcePercents)
        {
            if (record == null)
                return;

            if(!gridToListCollection.ContainsKey(targetGrid))
            {
                LogInfo linf = new LogInfo();
                linf.addToColumnName = addToColumnName;
                linf.computeMotoresurcePercents = computeMotoresurcePercents;
                gridToListCollection[targetGrid] = linf;
            }

            //Тут добавление в список в таблицу

            gridToListCollection[targetGrid].list.Add(record);

        }

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

        private void SaveEthalon(string fname, List<byte> content)
        {

            byte[] dt = new byte[4];
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

            if (fname.EndsWith("ET0UP.ETL"))
                ethalon0UpData = timeList;
            else
            if (fname.EndsWith("ET0DWN.ETL"))
                ethalon0DwnData = timeList;
            else
            if (fname.EndsWith("ET1UP.ETL"))
                ethalon1UpData = timeList;
            else
            if (fname.EndsWith("ET1DWN.ETL"))
                ethalon1DwnData = timeList;
            else
            if (fname.EndsWith("ET2UP.ETL"))
                ethalon2UpData = timeList;
            else
            if (fname.EndsWith("ET2DWN.ETL"))
            {
                ethalon2DwnData = timeList;
                ethalonsRequested = true;
            }


        } 

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

        private void CreateChart(List<byte> content, System.Windows.Forms.DataVisualization.Charting.Chart targetChart)
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
            targetChart.Legends[0].CustomItems[0].Cells["PulsesCount"].Text = String.Format("Импульсов: {0}", timeList.Count);

            // тут добавляем кастомные метки на ось X - времена
            int maxTime = 0;
            if (timeList.Count > 0)
            {
                maxTime += timeList[timeList.Count - 1];
            }


            // получили максимальное время всего графика, в микросекундах. Теперь надо равномерно его распределить по графику в виде меток
            //            int step = maxTime / customLabelsCount;
            int neareastValue = 100000; // приближение к 100 мс
            int step = Convert.ToInt32(Math.Round(Convert.ToDouble(maxTime / customLabelsCount) / neareastValue, 0)) * neareastValue;

            for (int kk = 0; kk < targetChart.ChartAreas.Count; kk++)
            {
                ChartArea area = targetChart.ChartAreas[kk];
                area.AxisX.CustomLabels.Clear();

                int startOffset = -step / 2;
                int endOffset = step / 2;
                int counter = 0;

                for (int i = 0; i < customLabelsCount; i++)
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
            for (int i = 1; i < timeList.Count; i++)
            {
                maxPulseTime = Math.Max(maxPulseTime, (timeList[i] - timeList[i - 1]));
            }

            int xCoord = 0;
            List<int> XValuesEthalon = new List<int>();
            List<double> YValuesEthalon = new List<double>();

            // теперь считаем все остальные точки
            for (int i = 1; i < timeList.Count; i++)
            {
                int pulseTime = timeList[i] - timeList[i - 1];
                //pulseTime *= 100;

                int pulseTimePercents = (pulseTime*100) / maxPulseTime;
                pulseTimePercents = 100 - pulseTimePercents;


                //System.Windows.Forms.DataVisualization.Charting.DataPoint pt = new System.Windows.Forms.DataVisualization.Charting.DataPoint();
                //pt.XValue = xCoord;
                //pt.SetValueY(pulseTimePercents);

                xCoord += pulseTime;// xStep;
                XValuesEthalon.Add(xCoord);
                YValuesEthalon.Add(pulseTimePercents);

                //s.Points.Add(pt);

            } // for


            s.Points.DataBindXY(XValuesEthalon, YValuesEthalon);
        }

        /// <summary>
        /// обрабатываем ответ от контроллера
        /// </summary>
        /// <param name="dt">строка, которая пришла из порта</param>
        private List<byte> COMAnswer = new List<byte>();

        private void ProcessPortAnswer(byte[] dt)
        {
            EnsureCloseConnectionForm(); // закрываем форму коннекта, если она ещё не закрыта

            switch (answerBehaviour)
            {
                case AnswerBehaviour.Normal:
                    {

                        // нормальный режим работы

                        COMAnswer.AddRange(dt);

                        while(true)
                        {
                            int idx = Array.IndexOf(COMAnswer.ToArray(), (byte)'\n');
                            if (idx != -1)
                            {
                                string line = System.Text.Encoding.UTF8.GetString(COMAnswer.ToArray(), 0, idx);
                                COMAnswer.RemoveRange(0, idx+1);
                                line = line.Trim();
                                ProcessAnswerLine(line);

                            }
                            else
                                break;
                        }



                    }
                    break;

                case AnswerBehaviour.SDCommandFILE:
                    {
                        
                            fileReadedBytes += dt.Length;
                            int percentsReading = (fileReadedBytes * 100) / (requestedFileSize == 0 ? 1 : requestedFileSize);

                            if (percentsReading > 100)
                                percentsReading = 100;

                            fileDownloadProgressFunction?.Invoke(percentsReading, dt.Length);

                        

                        // вычитываем файл с SD. Признаком окончания файла служат байты [END]\r\n
                        for (int i = 0; i < dt.Length; i++)
                        {
                            this.SDQueryAnswer.Add(dt[i]);
                         //   System.Diagnostics.Debug.Write((char) dt[i]);
                        }

                        if(SDQueryAnswer.Count > 6)
                        {
                            // уже можно проверять на окончание пакета
                            string endOfFile = "";
                            for(int i= SDQueryAnswer.Count-7;i< SDQueryAnswer.Count;i++)
                            {
                                endOfFile += (char)SDQueryAnswer[i];
                            } // for

                            if(endOfFile == "[END]\r\n")
                            {
                                SDQueryAnswer.RemoveRange(SDQueryAnswer.Count - 7, 7);

                                //DealWithFileContent(SDQueryAnswer);
                                fileDataParseFunction?.Invoke(SDQueryAnswer);

                                SDQueryAnswer.Clear();

                                this.answerBehaviour = AnswerBehaviour.Normal;
                                this.currentCommand.ParseFunction = null; // освобождаем
                                this.currentCommand.CommandToSend = "";

                            }

                        } // if
                    }
                    break;

                case AnswerBehaviour.SDCommandLS:
                    {
                        // опрашиваем SD, команда LS
                        for (int i = 0; i < dt.Length; i++)
                            this.SDQueryAnswer.Add(dt[i]);

                        // тут разбиваем по '\n', т.к. ответ на LS - всегда текстовый

                        while(true)
                        { 
                            int newLineIdx = SDQueryAnswer.FindIndex(x => x == '\n');

                            if (newLineIdx != -1)
                            {
                                // нашли перевод строки
                                string lsLine = "";
                                for (int k = 0; k < newLineIdx; k++)
                                {
                                    lsLine += (char)SDQueryAnswer[k];
                                }

                                SDQueryAnswer.RemoveRange(0, newLineIdx + 1);

                                lsLine = lsLine.Trim();
                                System.Diagnostics.Debug.WriteLine("<= COM: " + lsLine);

                               
                                if (lsLine == "[END]") // закончили список!!!
                                {
                                    this.answerBehaviour = AnswerBehaviour.Normal;
                                    this.currentCommand.ParseFunction = null; // освобождаем
                                    this.currentCommand.CommandToSend = "";

                                    lsParseFunction?.Invoke();

                                    break;

                                }
                                else
                                {
                                    // продолжаем список
                                    lsRecordFunction?.Invoke(lsLine);
                                }
                            } // if
                            else
                                break;
                        }

                    }
                    break;
            }
            

        }


        private void AddRecordToSDList(string line, TreeNode parent = null)
        {
            TreeNodeCollection nodes = this.treeViewSD.Nodes;
            if (parent != null)
            {
                nodes = parent.Nodes;
                SDNodeTagHelper existingTag = (SDNodeTagHelper)parent.Tag;
                parent.Tag = new SDNodeTagHelper(SDNodeTags.TagFolderNode,existingTag.FileName, existingTag.IsDirectory); // говорим, что мы вычитали это дело
                // и удаляем заглушку...
                for(int i=0;i<parent.Nodes.Count;i++)
                {
                    TreeNode child = parent.Nodes[i];
                    SDNodeTagHelper tg = (SDNodeTagHelper)child.Tag;
                    if(tg.Tag == SDNodeTags.TagDummyNode)
                    {
                        child.Remove();
                        break;
                    }
                }
            }
            bool isDir = false;
            int dirIdx = line.IndexOf("<DIR>");
            if(dirIdx != -1)
            {
                isDir = true;
                line = line.Substring(0, dirIdx).Trim();
            }

            TreeNode node = nodes.Add(getTextFromFileName(line));

            if(isDir)
            {
                node.ImageIndex = 0;
                node.SelectedImageIndex = 0;
                TreeNode dummy = node.Nodes.Add("вычитываем....");
                dummy.Tag = new SDNodeTagHelper(SDNodeTags.TagDummyNode,"",false); // этот узел потом удалим, при перечитывании
                dummy.ImageIndex = -1;

                node.Tag = new SDNodeTagHelper(SDNodeTags.TagFolderUninitedNode,line, isDir); // говорим, что мы не перечитали содержимое папки ещё
            }
            else
            {
                node.ImageIndex = 2;
                node.SelectedImageIndex = node.ImageIndex;
                node.Tag = new SDNodeTagHelper(SDNodeTags.TagFileNode,line, isDir);
            }
        }

        /// <summary>
        /// Обработчик события "Пришла строка из транспортного уровня"
        /// </summary>
        /// <param name="a"></param>
        public delegate void DataParseFunction(Answer a);
        public delegate void BeforeSendFunction();
        public delegate void AfterSendFunction();
        public delegate void LSDoneFunction();
        public delegate void LSRecordFunction(string line);
        public delegate void FileDownloadProgressFunction(int percentsCompleted, int bytesReceived);
        public delegate void FileDataParseFunction(List<byte> content);

        /// <summary>
        /// Структура команды на обработку
        /// </summary>
        private struct QueuedCommand
        {
            public string CommandToSend;
            public DataParseFunction ParseFunction;
            public BeforeSendFunction BeforeSend;
            public AfterSendFunction AfterSend;
        };


        /// <summary>
        /// Помещаем команду в очередь на обработку
        /// </summary>
        /// <param name="cmd">Текстовая команда для контроллера</param>
        /// <param name="act">К какому действию команда привязана</param>
        /// <param name="func">Указатель на функцию-обработчик ответа от контроллера</param>
        public void PushCommandToQueue(string cmd, DataParseFunction func, BeforeSendFunction before = null, AfterSendFunction after = null)
        {
            QueuedCommand q = new QueuedCommand();
            q.CommandToSend = cmd;
            q.ParseFunction = func;
            q.BeforeSend = before;
            q.AfterSend = after;
            if (!commandsQueue.Contains(q))
                commandsQueue.Enqueue(q);


        }

        /// <summary>
        /// Возвращаем команду из очереди
        /// </summary>
        /// <param name="outCmd">Команда, которая получена из очереди</param>
        /// <returns>Возвращаем false, если команд в очереди нет, иначе - true</returns>
        private bool GetCommandFromQeue(ref QueuedCommand outCmd)
        {
            if (commandsQueue.Count < 1)
                return false;

            outCmd = commandsQueue.Dequeue();
            return true;
        }

        /// <summary>
        /// Очередь команд
        /// </summary>
        private Queue<QueuedCommand> commandsQueue = new Queue<QueuedCommand>();

        /// <summary>
        /// текущая команда на обработку
        /// </summary>
        private QueuedCommand currentCommand = new QueuedCommand();

        private Color currentLogItemColor = SystemColors.ControlLight;

        /// <summary>
        /// добавляем строку в лог
        /// </summary>
        /// <param name="line">строка для добавления в лог</param>
        private void AddToLog(string line, bool shouldAddCommandName)
        {

            line = line.Trim();
            if (line.Length < 1)
                return;

            lvLog.BeginUpdate();

            int cnt = this.lvLog.Items.Count;
            if (cnt > 100)
            {
                this.lvLog.Items.RemoveAt(0);
            }

            ListViewItem li = this.lvLog.Items.Add(shouldAddCommandName ? currentCommand.CommandToSend : "");
            li.ImageIndex = 0;
            if (currentLogItemColor == SystemColors.Window)
                currentLogItemColor = SystemColors.ControlLight;
            else
                currentLogItemColor = SystemColors.Window;

            string prefix = "=> ";
            if (!shouldAddCommandName)
                prefix = "<= ";

            li.BackColor = currentLogItemColor;

            li.SubItems.Add(prefix + line);
            li.EnsureVisible();

            lvLog.EndUpdate();


        }

        /// <summary>
        /// Обрабатываем результат соединения с портом
        /// </summary>
        /// <param name="succ"></param>
        /// <param name="message"></param>
        private void OnCOMConnect(bool succ, string message)
        {
            
            if (InvokeRequired)
            {
                Invoke((MethodInvoker)delegate { OnCOMConnect(succ, message); });
                return;
            }

            lastConnected = succ;

            System.Diagnostics.Debug.WriteLine("TRANSPORT CONNECT EVENT!");

            // обнуляем текущее состояние при переконнекте
            //this.currentCommand.ActionToSet = Actions.None;
            EnsureCloseConnectionForm();

            if (succ)
            {
                this.btnConnect.ImageIndex = 1;
                this.btnConnect.Text = "Соединено";
                InitAfterConnect(true);

            }
            else
            {
                InitAfterConnect(false);
                MessageBox.Show("Не удалось соединиться с устройством!", "Ошибка", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }

            /*
            if (succ && connForm != null)
            {
                connForm.lblCurrentAction.Text = "Ждём данные из порта...";
            }
            */

            System.Diagnostics.Debug.WriteLine("TRANSPORT CONNECT EVENT DONE!");
        }

        private bool ethalonsRequested = false;

        /// <summary>
        /// Инициализируем необходимое после успешного коннекта
        /// </summary>
        private void InitAfterConnect(bool isConnected)
        {
            Config.Instance.Clear();

            ethalonsRequested = false;
            inUploadFileToController = false;

            nudDelta1.Value = 0;
            //DEPRECATED: nudDelta2.Value = 0;
            //DEPRECATED: nudDelta3.Value = 0;

            nudMotoresourceCurrent1.Value = 0;
            //DEPRECATED: nudMotoresourceCurrent2.Value = 0;
            //DEPRECATED: nudMotoresourceCurrent3.Value = 0;

            nudMotoresourceMax1.Value = 0;
            //DEPRECATED: nudMotoresourceMax2.Value = 0;
            //DEPRECATED: nudMotoresourceMax3.Value = 0;

            nudPulses1.Value = 0;
            //DEPRECATED: nudPulses2.Value = 0;
            //DEPRECATED: nudPulses3.Value = 0;

            nudCurrentCoeff.Value = 1;

            nudACSDelay.Value = 0;
            nudRelayDelay.Value = 0;
            nudHighBorder.Value = 0;
            nudLowBorder.Value = 0;

            nudHighBorder.Value = 0;
            nudLowBorder.Value = 0;

            tbControllerTime.Text = "-";
            tbFirmwareVersion.Text = "";
            tbFREERAM.Text = "";

            //DEPRECATED: ResetInductiveSensors();
            ResetVoltage();

            ClearEthalons();

            btnViewSDFile.Enabled = treeViewSD.SelectedNode != null;
            btnDeleteSDFile.Enabled = btnViewSDFile.Enabled;


            if (isConnected)
            {
                this.btnConnect.ImageIndex = 1;
                this.btnConnect.Text = "Соединено";
                connectStatusMessage.Text = "Соединено.";
            }
            else
            {
               this.btnConnect.Text = "Соединить";
                this.btnConnect.ImageIndex = 0; // коннект оборвался
                connectStatusMessage.Text = "Нет соединения.";
            }

            dateTimeFromControllerReceived = false;
            inSetDateTimeToController = true;
            inSetMotoresourceToController = true;
            inSetPulsesToController = true;
            inSetCurrentCoeffToController = true;
            inSetBordersToController = true;
            inSetRelayDelayToController = true;
            inSetDeltaToController = true;
            inSetAsuTPFlagsInController = true;

            uuidRequested = false;

            // очищаем очередь
            commandsQueue.Clear();
            this.currentCommand.ParseFunction = null;
            this.currentCommand.CommandToSend = "";
            this.coreBootFound = false;

            if (isConnected)
            {
                InitTreeView();

                // добавляем нужные команды для обработки сразу после коннекта
                PushCommandToQueue(GET_PREFIX + "UUID" + PARAM_DELIMITER + GenerateUUID(), ParseAskUUID);

                PushCommandToQueue(GET_PREFIX + "DATETIME", ParseAskDatetime);
                PushCommandToQueue(GET_PREFIX + "FREERAM", ParseAskFreeram);
                PushCommandToQueue(GET_PREFIX + "RES_CUR", ParseAskMotoresurceCurrent, BeforeAskMotoresource);
                PushCommandToQueue(GET_PREFIX + "RES_MAX", ParseAskMotoresurceMax, BeforeAskMotoresource);
                PushCommandToQueue(GET_PREFIX + "PULSES", ParseAskPulses, BeforeAskPulses);
                PushCommandToQueue(GET_PREFIX + "CCOEFF", ParseAskCurrentCoeff, BeforeAskCurrentCoeff);
                PushCommandToQueue(GET_PREFIX + "TBORDERS", ParseAskBorders, BeforeAskBorders);
                PushCommandToQueue(GET_PREFIX + "RDELAY", ParseAskRelayDelay, BeforeAskRelayDelay);

                PushCommandToQueue(GET_PREFIX + "DELTA", ParseAskDelta, BeforeAskPulses);
                PushCommandToQueue(GET_PREFIX + "ECDELTA", ParseAskECDelta, BeforeAskDelta);
                PushCommandToQueue(GET_PREFIX + "SKIPC", ParseAskSkipCounter, BeforeAskDelta);
                PushCommandToQueue(GET_PREFIX + "ASUTPFLAGS", ParseAskAsuTpFlags, BeforeAskAsuTpFlags);
                
                //DEPRECATED: GetInductiveSensors();
                GetVoltage();
                RequestEthalons();
                RescanSD();

                ShowMainSettings();
                tmPeriodicCommandsTimer.Enabled = true;

            }
            else
            {
                ClearAllData();
                ShowStartPanel();

                tmPeriodicCommandsTimer.Enabled = false;
            }



        }

        private void setConnectionStatusMessage()
        {
            string cguid = Config.Instance.ControllerGUID;
            string savedName = Config.Instance.ControllerGUID;
            if (ControllerNames.Instance.Names.ContainsKey(cguid))
                savedName = ControllerNames.Instance.Names[cguid];

            if (IsConnected())
            {
                connectStatusMessage.Text = "Соединено, контроллер " + savedName;
            }
            else
                connectStatusMessage.Text = "";
        }

        private void ParseAskUUID(Answer a)
        {
            uuidRequested = true;

            if (a.IsOkAnswer)
            {
                string receivedGUID = a.Params[1];
                Config.Instance.ControllerGUID = receivedGUID;

                string savedName = receivedGUID;

                if (ControllerNames.Instance.Names.ContainsKey(receivedGUID))
                    savedName = ControllerNames.Instance.Names[receivedGUID];
                else
                    ControllerNames.Instance.Names[receivedGUID] = "";

                setConnectionStatusMessage();
            }
        }

        private string GenerateUUID()
        {
            return Guid.NewGuid().ToString("N");
        }

        private void RequestEthalons()
        {
            PushCommandToQueue(GET_PREFIX + "FILE|ETL|ET0UP.ETL", DummyAnswerReceiver, SetSDFileReadingFlagEthalon);
            PushCommandToQueue(GET_PREFIX + "FILE|ETL|ET0DWN.ETL", DummyAnswerReceiver, SetSDFileReadingFlagEthalon);

            PushCommandToQueue(GET_PREFIX + "FILE|ETL|ET1UP.ETL", DummyAnswerReceiver, SetSDFileReadingFlagEthalon);
            PushCommandToQueue(GET_PREFIX + "FILE|ETL|ET1DWN.ETL", DummyAnswerReceiver, SetSDFileReadingFlagEthalon);

            PushCommandToQueue(GET_PREFIX + "FILE|ETL|ET2UP.ETL", DummyAnswerReceiver, SetSDFileReadingFlagEthalon);
            PushCommandToQueue(GET_PREFIX + "FILE|ETL|ET2DWN.ETL", DummyAnswerReceiver, SetSDFileReadingFlagEthalon);

        }

        private void BeforeAskAsuTpFlags()
        {
            this.inSetAsuTPFlagsInController = true;
        }

        private void BeforeAskDelta()
        {
            this.inSetDeltaToController = true;
        }

        private void BeforeAskPulses()
        {
            this.inSetPulsesToController = true;
        }
        private void BeforeAskCurrentCoeff()
        {
            this.inSetCurrentCoeffToController = true;
        }
        private void BeforeAskBorders()
        {
            this.inSetBordersToController = true;
        }
        private void BeforeAskRelayDelay()
        {
            this.inSetRelayDelayToController = true;
        }

        private void BeforeAskMotoresource()
        {
            this.inSetMotoresourceToController = true;
        }


        private void ClearAllData()
        {
            this.treeView.Nodes[0].Nodes.Clear();
            this.treeViewSD.Nodes.Clear();
            this.ClearInterruptsList(this.logDataGrid);
            dateTimeFromControllerReceived = false;
            tbFirmwareVersion.Text = "";
            tbFREERAM.Text = "";
        }

        private void ParseAskDelta(Answer a)
        {
            this.inSetPulsesToController = false;
            if (a.IsOkAnswer)
            {
                try { Config.Instance.Delta1 = Convert.ToInt32(a.Params[1]); }
                catch
                {
                    Config.Instance.Delta1 = 0;
               }

            }
            else
            {
                Config.Instance.Delta1 = 0;
            }

            try
            {
                nudDelta1.Value = Config.Instance.Delta1;
            }
            catch
            {
                nudDelta1.Value = 0;
                Config.Instance.Delta1 = 0;
            }
        }

        private void ParseAskCurrentCoeff(Answer a)
        {
            this.inSetCurrentCoeffToController = false;
            if (a.IsOkAnswer)
            {
                try { Config.Instance.CurrentCoeff = Convert.ToInt32(a.Params[1]); }
                catch { Config.Instance.CurrentCoeff = 1; }

            }
            else
            {
                Config.Instance.CurrentCoeff = 1;
            }

            try
            {
                nudCurrentCoeff.Value = Config.Instance.CurrentCoeff;
            }
            catch
            {
                nudCurrentCoeff.Value = 1;
                Config.Instance.CurrentCoeff = 1;
            }
        }

        private void ParseAskPulses(Answer a)
        {
            if (a.IsOkAnswer)
            {
                try { Config.Instance.Pulses1 = Convert.ToInt32(a.Params[1]); }
                catch
                {
                    Config.Instance.Pulses1 = 0;
                    MessageBox.Show("ОШИБКА РАЗБОРА ОТВЕТА КОНТРОЛЛЕРА!");
                }

            }
            else
            {
                Config.Instance.Pulses1 = 0;
                MessageBox.Show("КОНТРОЛЛЕР ОТВЕТИЛ ОШИБКОЙ НА ЗАПРОС КОЛ-ВА ИМПУЛЬСОВ!");
            }

            try
            {
                nudPulses1.Value = Config.Instance.Pulses1;
            }
            catch
            {
                nudPulses1.Value = 0;
                Config.Instance.Pulses1 = 0;
            }
        }

        private void ParseAskBorders(Answer a)
        {
            this.inSetBordersToController = false;
            if (a.IsOkAnswer)
            {
                try { Config.Instance.LowBorder = Convert.ToInt32(a.Params[1]); } catch { Config.Instance.LowBorder = 0; }
                try { Config.Instance.HighBorder = Convert.ToInt32(a.Params[2]); } catch { Config.Instance.HighBorder = 0; }


            }
            else
            {
                Config.Instance.LowBorder = 0;
                Config.Instance.HighBorder = 0;
            }

            try
            {
                nudLowBorder.Value = Config.Instance.LowBorder;
            }
            catch
            {
                nudLowBorder.Value = 0;
                Config.Instance.LowBorder = 0;

            }

            try
            {
                nudHighBorder.Value = Config.Instance.HighBorder;
            }
            catch
            {
                nudHighBorder.Value = 0;
                Config.Instance.HighBorder = 0;
            }
        }

        private void ParseAskRelayDelay(Answer a)
        {
            this.inSetRelayDelayToController = false;
            if (a.IsOkAnswer)
            {
                try { Config.Instance.RelayDelay = Convert.ToInt32(a.Params[1]); } catch { Config.Instance.RelayDelay = 0; }
                try { Config.Instance.ACSDelay = Convert.ToInt32(a.Params[2]); } catch { Config.Instance.ACSDelay = 0; }

            }
            else
            {
                Config.Instance.RelayDelay = 0;
                Config.Instance.ACSDelay = 0;
            }

            try
            {
                nudRelayDelay.Value = Config.Instance.RelayDelay;
            }
            catch
            {
                nudRelayDelay.Value = 0;
                Config.Instance.RelayDelay = 0;
            }

            try
            {
                nudACSDelay.Value = Config.Instance.ACSDelay;
            }
            catch
            {
                nudACSDelay.Value = 0;
                Config.Instance.ACSDelay = 0;
            }
        }

        private void ParseAskAsuTpFlags(Answer a)
        {
            this.inSetAsuTPFlagsInController = false;
            if (a.IsOkAnswer)
            {
                try { Config.Instance.AsuTpFlags = Convert.ToUInt16(a.Params[1]); } catch { Config.Instance.AsuTpFlags = 0xFF; }

            }
            else
            {
                Config.Instance.AsuTpFlags = 0xFF;
            }

            cbAsuTpLine1.Checked = false;
            cbAsuTpLine2.Checked = false;
            cbAsuTpLine3.Checked = false;
            cbAsuTpLine4.Checked = false;

            if ((Config.Instance.AsuTpFlags & 1) != 0)
            {
                cbAsuTpLine1.Checked = true;
            }
            if ((Config.Instance.AsuTpFlags & 2) != 0)
            {
                cbAsuTpLine2.Checked = true;
            }
            if ((Config.Instance.AsuTpFlags & 4) != 0)
            {
                cbAsuTpLine3.Checked = true;
            }
            if ((Config.Instance.AsuTpFlags & 8) != 0)
            {
                cbAsuTpLine4.Checked = true;
            }

        }

        private void ParseAskECDelta(Answer a)
        {
            if (a.IsOkAnswer)
            {
                try { Config.Instance.EthalonCompareDelta = Convert.ToInt32(a.Params[1]); } catch { Config.Instance.EthalonCompareDelta = 50; }

            }
            else
            {
                Config.Instance.EthalonCompareDelta = 50;
            }

            try
            {
                nudEthalonCompareDelta.Value = Config.Instance.EthalonCompareDelta;
            }
            catch
            {
                nudEthalonCompareDelta.Value = 50;
                Config.Instance.EthalonCompareDelta = 50;
            }

        }

        private void ParseAskSkipCounter(Answer a)
        {
            this.inSetDeltaToController = false;
            if (a.IsOkAnswer)
            {
                try { Config.Instance.SkipCounter = Convert.ToInt32(a.Params[1]); } catch { Config.Instance.SkipCounter = 1; }

            }
            else
            {
                Config.Instance.SkipCounter = 1;
            }

            try
            {
                nudSkipCounter.Value = Config.Instance.SkipCounter;
            }
            catch
            {
                nudSkipCounter.Value = 1;
                Config.Instance.SkipCounter = 1;
            }

        }

        private void ParseAskMotoresurceCurrent(Answer a)
        {
            if (a.IsOkAnswer)
            {
                try { Config.Instance.MotoresourceCurrent1 = Convert.ToInt32(a.Params[1]);  } catch { Config.Instance.MotoresourceCurrent1 = 0; }

            }
            else
            {
                Config.Instance.MotoresourceCurrent1 = 0;
            }

            try
            {
                nudMotoresourceCurrent1.Value = Config.Instance.MotoresourceCurrent1;
            }
            catch
            {
                nudMotoresourceCurrent1.Value = 0;
                Config.Instance.MotoresourceCurrent1 = 0;
            }

            UpdateMotoresourcePercents();
        }

        private void UpdateMotoresourcePercents()
        {
            if (this.inSetMotoresourceToController)
                return;

            NumberFormatInfo nfi = new NumberFormatInfo();
            nfi.NumberDecimalSeparator = ".";

            float percents = 0;
            if(Config.Instance.MotoresourceMax1 > 0)
                percents = (100.0f * Config.Instance.MotoresourceCurrent1) / Config.Instance.MotoresourceMax1;

            Color foreColor = Color.Green;
            if (percents >= 90.0f)
                foreColor = Color.Red;

            lblMotoresourcePercents1.ForeColor = foreColor;
            lblMotoresourcePercents1.Text = percents.ToString("n1", nfi) + "%";



        }

        private void ParseAskMotoresurceMax(Answer a)
        {
            this.inSetMotoresourceToController = false;
            if (a.IsOkAnswer)
            {
                try { Config.Instance.MotoresourceMax1 = Convert.ToInt32(a.Params[1]); } catch { Config.Instance.MotoresourceMax1 = 0; }

            }
            else
            {
                Config.Instance.MotoresourceMax1 = 0;
            }

            try
            {
                nudMotoresourceMax1.Value = Config.Instance.MotoresourceMax1;
            }
            catch
            {
                nudMotoresourceMax1.Value = 0;
                Config.Instance.MotoresourceMax1 = 0;
            }

        }


        private bool dateTimeFromControllerReceived = false;
        private DateTime controllerDateTime = DateTime.MinValue;

        private void ParseAskDatetime(Answer a)
        {
            this.inSetDateTimeToController = false;
            if (a.IsOkAnswer)
            {
                try
                {
                    //пришло время из контроллера
                    this.controllerDateTime = DateTime.ParseExact(a.Params[1], "dd.MM.yyyy HH:mm:ss", null);
                    dateTimeFromControllerReceived = true;
                }
                catch { }

            }
            else
            {

            }
        }

        private void ParseAskFreeram(Answer a)
        {
            if (a.IsOkAnswer)
            {
                //пришли данные о свободной памяти
                tbFREERAM.Text = a.Params[1];
            }
            else
            {

            }
        }



        private FeaturesSettings featuresSettings = new FeaturesSettings();

        private void RecreateTreeView()
        {

            this.treeView.Nodes[0].Nodes.Clear();


            TreeNode node = this.treeView.Nodes[0].Nodes.Add("Основные настройки");
            node.Tag = TreeNodeType.MainSettingsNode;
            node.ImageIndex = 5;
            node.SelectedImageIndex = node.ImageIndex;

            if (featuresSettings.SDAvailable)
            {
                TreeNode n = this.treeView.Nodes[0].Nodes.Add("SD-карта");
                n.Tag = TreeNodeType.SDSettingsNode;
                n.ImageIndex = 8;
                n.SelectedImageIndex = n.ImageIndex;

            }

        }




        private string ByteAsHexString(byte b)
        {
            return "0x" + b.ToString("X2");
        }

 

        private void ResizeLogColumns()
        {
            this.logColumn1.Width = this.lvLog.ClientRectangle.Width / 2 - SystemInformation.VerticalScrollBarWidth / 2 - 2;
            this.logColumn2.Width = this.logColumn1.Width;
        }

        private void InitArchive()
        {
            // загружаем архив

            string path = Application.StartupPath + "\\Archive\\";
            try
            {
                System.IO.Directory.CreateDirectory(path);
            }
            catch { }

            List<string> dirs = new List<string>(System.IO.Directory.EnumerateDirectories(path));

            foreach(string dir in dirs)
            {
                string dirNameOnly = dir.Substring(dir.LastIndexOf("\\") + 1);
                AddItemToArchive(dirNameOnly);
            }
        }

        private void MainForm_Load(object sender, EventArgs e)
        {
            InitSubstitutions();

            EnumSerialPorts();

            ResizeLogColumns();

            InitArchive();

            this.toolStrip.ImageList = toolbarImages;
            this.btnConnect.ImageIndex = 0;
            this.btnSetDateTime.ImageIndex = 3;
            this.btnAbout.ImageIndex = 5;
            this.btnUploadEthalon.ImageIndex = 4;
            this.btnDisconnect.ImageIndex = 6;
            this.btnControllerName.ImageIndex = 8;
            this.btnImportSettings.ImageIndex = 9;
            this.btnRecordEthalonUp.ImageIndex = 10;
            this.btnRecordEthalonDown.ImageIndex = 11;
            this.btnSDTest.ImageIndex = 12;


            plMainSettings.Dock = DockStyle.Fill;
            this.plSDSettings.Dock = DockStyle.Fill;
            this.logDataGrid.Dock = DockStyle.Fill;
            this.plEthalonChart.Dock = DockStyle.Fill;
            this.plEmptySDWorkspace.Dock = DockStyle.Fill;
            this.plArchiveEthalonChart.Dock = DockStyle.Fill;
            this.archiveLogDataGrid.Dock = DockStyle.Fill;
            this.plEmptySDWorkspace.BringToFront();
            //TODO: тут остальные панели !!!

            ShowStartPanel();

            Application.Idle += new EventHandler(Application_Idle);

            Text += ", v." + System.Reflection.Assembly.GetExecutingAssembly().GetName().Version.ToString();
        }

        private void InitTreeView()
        {
            RecreateTreeView();

            this.treeView.Nodes[0].ExpandAll();
            

        }

        private bool uuidRequested = false;

        private bool lastConnected = false;


        /// <summary>
        /// Обработчик простоя
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        void Application_Idle(object sender, EventArgs e)
        {

            bool bConnected = IsConnected();

            if(lastConnected && !bConnected)
            {
                lastConnected = false;
                InitAfterConnect(false);
            }

            btnUploadEthalon.Enabled = bConnected && !inUploadFileToController;
            btnControllerName.Enabled = bConnected && uuidRequested;
            btnImportSettings.Enabled = bConnected;
            //btnSaveEthalons.Enabled = ethalonsRequested;
            btnSetDateTime.Enabled = bConnected && !inSetDateTimeToController;
            btnSetDateTime2.Enabled = bConnected && !inSetDateTimeToController;
            this.btnDisconnect.Enabled = bConnected && currentTransport != null;

            this.btnSetMotoresourceCurrent.Enabled = bConnected && !inSetMotoresourceToController;
            this.btnSetPulses.Enabled = bConnected && !inSetPulsesToController;
            this.btnCurrentCoeff.Enabled = bConnected && !inSetCurrentCoeffToController;
            this.btnSetDelta.Enabled = bConnected && !inSetDeltaToController;
            this.btnSetBorders.Enabled = bConnected && !inSetBordersToController;
            this.btnSetRelayDelay.Enabled = bConnected && !inSetRelayDelayToController;

            this.btnRecordEthalonUp.Enabled = bConnected && !inSetEthalonRecordToController;
            this.btnRecordEthalonDown.Enabled = bConnected && !inSetEthalonRecordToController;
            this.btnSDTest.Enabled = bConnected;

            this.btnSetAsuTpLags.Enabled = bConnected && !inSetAsuTPFlagsInController;

            if (!bConnected) // порт закрыт
            {

                if (this.lastSelectedPort != null)
                {
                    this.lastSelectedPort.Checked = false;
                }

                

            }
            else
            {

                if (lastSelectedPort != null)
                    if (!lastSelectedPort.Checked)
                        lastSelectedPort.Checked = true;

            }
        }

        /// <summary>
        /// Перечисляем COM-порты
        /// </summary>
        private void EnumSerialPorts()
        {
            miPort.DropDownItems.Clear();

            string[] ports = SerialPort.GetPortNames();
            foreach (string port in ports)
            {

                ToolStripMenuItem ti = new ToolStripMenuItem(port);

                ti.MergeIndex = 1;
                ti.AutoSize = true;
                ti.ImageScaling = ToolStripItemImageScaling.None;
                ti.Tag = port;
                ti.Click += ConnectToSelectedComPort;
                ti.CheckOnClick = false;



                miPort.DropDownItems.Add(ti);


            }
        }


        private void ConnectToSelectedComPort(object sender, EventArgs e)
        {

            ToolStripMenuItem mi = (ToolStripMenuItem)sender;

            if (mi.Checked)
                return;


            Disconnect();
            if (this.lastSelectedPort != null)
            {

                this.lastSelectedPort.Checked = false;
            }

            this.lastSelectedPort = mi;
            mi.Checked = true;

            DoConnect((string)mi.Tag, true, false);

        }

        private int GetConnectionSpeed()
        {
            foreach(ToolStripMenuItem tmi in this.portSpeedToolStripMenuItem.DropDownItems)
            {
                if(tmi.Checked)
                {
                    int speed = Convert.ToInt32(tmi.Text);
                    return speed;
                }
            }

            return 115200;
        }

        /// <summary>
        /// Начинаем коннектиться к порту
        /// </summary>
        /// <param name="port">имя порта</param>
        private void DoConnect(string port, bool withHandshake, bool findDevice)
        {
            System.Diagnostics.Debug.WriteLine("START CONNECT....");

            dateTimeFromControllerReceived = false; // не получили ещё текущее время с контроллера
            controllerDateTime = DateTime.MinValue; // устанавливаем минимальное значение даты            

            connForm = new ConnectForm(false);
            connForm.SetMainFormAndPort(this, port, withHandshake, findDevice);
            connForm.ShowDialog();
        }

        /// <summary>
        /// Отсоединяемся
        /// </summary>
        private void Disconnect() // отсоединяемся от порта
        {
            if (currentTransport != null)
                currentTransport.Disconnect();

            currentTransport = null;



        }

        /// <summary>
        /// Проверяем, соединены ли мы с контроллером
        /// </summary>
        /// <returns></returns>
        public bool IsConnected()
        {
            if (currentTransport != null)
                return currentTransport.Connected();

            return false;
        }



        /// <summary>
        /// Класс ответа от контроллера
        /// </summary>
        public class Answer
        {
            /// <summary>
            /// флаг, что ответ положительный
            /// </summary>
            public bool IsOkAnswer;
            /// <summary>
            /// список параметров
            /// </summary>
            public string[] Params;
            /// <summary>
            /// сырые данные, полученные от контроллера
            /// </summary>
            public string RawData;

            /// <summary>
            /// очищает все переменные
            /// </summary>
            private void Clear()
            {
                IsOkAnswer = false;
                Params = null;
                RawData = "";
            }

            /// <summary>
            /// конструирует параметры из строки
            /// </summary>
            /// <param name="dt"></param>
            public void Parse(string dt)
            {
                Clear();
                RawData = dt;

                int idx = dt.IndexOf("OK=");
                if (idx != -1)
                {
                    this.IsOkAnswer = true;
                    dt = dt.Substring(3).Trim();
                    this.Params = dt.Split(PARAM_DELIMITER);
                }
                idx = dt.IndexOf("ER=");
                if (idx != -1)
                {
                    this.IsOkAnswer = false;
                    dt = dt.Substring(3).Trim();
                    this.Params = dt.Split(PARAM_DELIMITER);
                }

            }
            /// <summary>
            /// кол-во параметров
            /// </summary>
            public int ParamsCount
            {
                get { return this.Params == null ? 0 : this.Params.Length; }
            }
            /// <summary>
            /// конструктор
            /// </summary>
            /// <param name="dt">строка для разбора</param>
            public Answer(string dt)
            {
                this.Clear();
                this.Parse(dt);

            }
        }

        private const int everyNTimerTicksRequestLastTrig = 5;
        private int timerTicksCounter = 0;
        private void SetLastTrigReadingFlag()
        {
            this.answerBehaviour = AnswerBehaviour.SDCommandFILE;
            this.fileDataParseFunction = this.ViewLastTrigData;
            this.SDQueryAnswer.Clear();
        }

        private void LastTrigCallback(InterruptRecord rec)
        {
            // тут показываем форму последнего срабатывания
            string stationID = Config.Instance.ControllerGUID;
            string stationName = stationID;

            if (ControllerNames.Instance.Names.ContainsKey(stationID))
                stationName = ControllerNames.Instance.Names[stationID];


            // обновляем кол-во срабатываний
            Config.Instance.MotoresourceCurrent1++;
            nudMotoresourceCurrent1.Value = Config.Instance.MotoresourceCurrent1;
            UpdateMotoresourcePercents();

            ShowChart(rec, stationID, stationName,false);
        }

        private void ViewLastTrigData(List<byte> content)
        {
            if (InvokeRequired)
            {
                Invoke((MethodInvoker)delegate { ViewLastTrigData(content); });
                return;
            }

            if (content.Count > 0) // есть информация по срабатыванию!!!
            {
                ShowLogFile(content, null, "", false, null, LastTrigCallback, true);
            }
        }

        private void tmProcessCommandsTimer_Tick(object sender, EventArgs e)
        {
            ProcessNextCommand();
        }

        private AnswerBehaviour answerBehaviour = AnswerBehaviour.Normal;

        private void ProcessNextCommand()
        {
            if (!GrantToProcess())
                return;

            timerTicksCounter++;
            if(timerTicksCounter >= everyNTimerTicksRequestLastTrig)
            {
                timerTicksCounter = 0;
                PushCommandToQueue(GET_PREFIX + "LASTTRIG", DummyAnswerReceiver, SetLastTrigReadingFlag);
            }

            if (!this.GetCommandFromQeue(ref this.currentCommand))
                return;

            System.Diagnostics.Debug.Assert(this.currentTransport != null);

            this.answerBehaviour = AnswerBehaviour.Normal;

            currentCommand.BeforeSend?.Invoke();

            this.currentTransport.WriteLine(currentCommand.CommandToSend);

            currentCommand.AfterSend?.Invoke();

        }

        private bool coreBootFound = false;


        /// <summary>
        /// Проверяем, можем ли мы работать
        /// </summary>
        /// <returns>возвращаем true, если работать можем</returns>
        private bool GrantToProcess()
        {

            if (!coreBootFound)
                return false;

            if (!this.IsConnected()) // нет коннекта
            {

                return false;
            }


            if (this.currentCommand.ParseFunction != null) // чем-то заняты
                return false;


            return true;
        }

        private void tmGetSensorsData_Tick(object sender, EventArgs e)
        {
            if (!GrantToProcess())
                return;

            PushCommandToQueue(GET_PREFIX + "FREERAM", ParseAskFreeram);

        }

        private void ShowStartPanel()
        {
            this.plStartPanel.Dock = DockStyle.Fill;
            this.plStartPanel.BringToFront();
        }

        private void ShowMainSettings()
        {
            this.plMainSettings.BringToFront();
            treeView.SelectedNode = treeView.Nodes[0].Nodes[0];
        }



        private void propertyGridSettings_PropertyValueChanged(object s, PropertyValueChangedEventArgs e)
        {

        }

        private void tmDateTime_Tick(object sender, EventArgs e)
        {
            if (!dateTimeFromControllerReceived)
            {
                tbControllerTime.Text = "-";
                return;
            }

            this.controllerDateTime = this.controllerDateTime.AddMilliseconds(tmDateTime.Interval);
            string dateTimeString = this.controllerDateTime.ToString("dd.MM.yyyy HH:mm:ss");

            tbControllerTime.Text = dateTimeString;


        }

        private DateTime dateTimeToSet = DateTime.MinValue;
        private bool inSetDateTimeToController = false;

        private void btnSetDateTime_Click(object sender, EventArgs e)
        {
            DialogResult dr = MessageBox.Show("Установить время контроллера в локальное время компьютера?", "Подтверждение", MessageBoxButtons.OKCancel, MessageBoxIcon.Question);
            if (dr != System.Windows.Forms.DialogResult.OK)
                return;

            ShowWaitCursor(true);
            dateTimeToSet = DateTime.Now;
            inSetDateTimeToController = true;

            String s = string.Format("{0,0:D2}.{1,0:D2}.{2} {3,0:D2}:{4,0:D2}:{5,0:D2}", dateTimeToSet.Day, dateTimeToSet.Month, dateTimeToSet.Year, dateTimeToSet.Hour, dateTimeToSet.Minute, dateTimeToSet.Second);
            PushCommandToQueue(SET_PREFIX + "DATETIME" + PARAM_DELIMITER + s, ParseSetDatetime);


        }

        private void ParseSetECDelta(Answer a)
        {
            if (a.IsOkAnswer)
            {
                Config.Instance.EthalonCompareDelta = Convert.ToInt32(nudEthalonCompareDelta.Value);
            }
            else
            {
                nudEthalonCompareDelta.Value = Config.Instance.EthalonCompareDelta;
            }
        }

        private void ParseSetSkipCounter(Answer a)
        {
            inSetDeltaToController = false;
            ShowWaitCursor(false);

            if (a.IsOkAnswer)
            {
                Config.Instance.SkipCounter = Convert.ToInt32(nudSkipCounter.Value);

                MessageBox.Show("Параметры обновлёны.", "Сообщение", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
            else
            {
                nudSkipCounter.Value = Config.Instance.SkipCounter;

                MessageBox.Show("Ошибка обновления параметров!", "Ошибка", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }

        }

        private void ParseSetDelta(Answer a)
        {

            if (a.IsOkAnswer)
            {
                Config.Instance.Delta1 = Convert.ToInt32(nudDelta1.Value);

            }
            else
            {
                nudDelta1.Value = Config.Instance.Delta1;
            }
                
        }


        private void ParseSetPulses(Answer a)
        {
            inSetPulsesToController = false;
            ShowWaitCursor(false);

            if (a.IsOkAnswer)
            {
                Config.Instance.Pulses1 = Convert.ToInt32(nudPulses1.Value);

                MessageBox.Show("Импульсы обновлёны.", "Сообщение", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
            else
            {
                nudPulses1.Value = Config.Instance.Pulses1;

                MessageBox.Show("Ошибка обновления импульсов!", "Ошибка", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        private void ParseSetBorders(Answer a)
        {
            inSetBordersToController = false;
            ShowWaitCursor(false);

            if (a.IsOkAnswer)
            {
                Config.Instance.LowBorder = Convert.ToInt32(nudLowBorder.Value);
                Config.Instance.HighBorder = Convert.ToInt32(nudHighBorder.Value);

                MessageBox.Show("Пороги трансформатора обновлёны.", "Сообщение", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
            else
            {
                nudLowBorder.Value = Config.Instance.LowBorder;
                nudHighBorder.Value = Config.Instance.HighBorder;

                MessageBox.Show("Ошибка обновления порогов трансформатора!", "Ошибка", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        private void ParseSetRelayDelay(Answer a)
        {
            inSetRelayDelayToController = false;
            ShowWaitCursor(false);

            if (a.IsOkAnswer)
            {
                Config.Instance.RelayDelay = Convert.ToInt32(nudRelayDelay.Value);
                Config.Instance.ACSDelay = Convert.ToInt32(nudACSDelay.Value);

                MessageBox.Show("Настройки обновлены.", "Сообщение", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
            else
            {
                nudRelayDelay.Value = Config.Instance.RelayDelay;
                nudACSDelay.Value = Config.Instance.ACSDelay;

                MessageBox.Show("Ошибка обновления настроек!", "Ошибка", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        private void ParseSetMotoresourceCurrent(Answer a)
        {
            if (a.IsOkAnswer)
            {
                Config.Instance.MotoresourceCurrent1 = Convert.ToInt32(nudMotoresourceCurrent1.Value);

            }
            else
            {
                nudMotoresourceCurrent1.Value = Config.Instance.MotoresourceCurrent1;
            }

            UpdateMotoresourcePercents();
        }

        private void ParseSetMotoresourceMax(Answer a)
        {
            inSetMotoresourceToController = false;
            ShowWaitCursor(false);

            if (a.IsOkAnswer)
            {
                Config.Instance.MotoresourceMax1 = Convert.ToInt32(nudMotoresourceMax1.Value);

                MessageBox.Show("Моторесурс обновлён.", "Сообщение", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
            else
            {
                nudMotoresourceMax1.Value = Config.Instance.MotoresourceMax1;

                MessageBox.Show("Ошибка обновления моторесурса!", "Ошибка", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }

            UpdateMotoresourcePercents();
        }

        private void ParseSetDatetime(Answer a)
        {
            inSetDateTimeToController = false;
            ShowWaitCursor(false);

            if (a.IsOkAnswer)
            {
                this.dateTimeFromControllerReceived = true;
                this.controllerDateTime = dateTimeToSet;

                MessageBox.Show("Время контроллера обновлено.", "Сообщение", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
            else
            {
                MessageBox.Show("Ошибка установки времени на контроллере!", "Ошибка", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        private void MainForm_Resize(object sender, EventArgs e)
        {
            ResizeLogColumns();
        }

        private void treeView_BeforeCollapse(object sender, TreeViewCancelEventArgs e)
        {
            //e.Cancel = true;
        }



        private void ShowWaitCursor(bool show)
        {
            System.Windows.Forms.Cursor.Current = show ? Cursors.WaitCursor : Cursors.Default;
            Application.UseWaitCursor = show;
            Application.DoEvents();

        }


        private void btnAbout_Click(object sender, EventArgs e)
        {
            AboutForm ab = new AboutForm();
            ab.ShowDialog();
        }

        private TreeNode currentSDParentNode = null;

        private void RescanSD()
        {
            btnListSDFiles.Enabled = false;
            PushCommandToQueue(GET_PREFIX + "LS", DummyAnswerReceiver, SetSDReadingFlag);

        }
        private void btnListSDFiles_Click(object sender, EventArgs e)
        {
            RescanSD();
        }

        public void DummyAnswerReceiver(Answer a)
        {

        }

        private LSDoneFunction lsParseFunction = null;
        private LSRecordFunction lsRecordFunction = null;
        private FileDownloadProgressFunction fileDownloadProgressFunction = null;
        private FileDataParseFunction fileDataParseFunction = null;

        private void ParseSDLSRecord(string line)
        {
            AddToLog(line, false);
            AddRecordToSDList(line, currentSDParentNode);

        }

        private void ParseSDLSDone()
        {
            this.btnListSDFiles.Enabled = true;

            if (currentSDParentNode != null)
            {
                // и удаляем заглушку...
                for (int i = 0; i < currentSDParentNode.Nodes.Count; i++)
                {
                    TreeNode child = currentSDParentNode.Nodes[i];
                    SDNodeTagHelper tg = (SDNodeTagHelper)child.Tag;
                    if (tg.Tag == SDNodeTags.TagDummyNode)
                    {
                        child.Remove();
                        break;
                    }
                }
            } // if
        }
        private void SetSDReadingFlag()
        {
            lsParseFunction = ParseSDLSDone;
            lsRecordFunction = ParseSDLSRecord;
            this.answerBehaviour = AnswerBehaviour.SDCommandLS;
            this.currentSDParentNode = null;
            this.SDQueryAnswer.Clear();
            this.treeViewSD.Nodes.Clear();
        }

        private void SetSDFolderReadingFlag()
        {
            lsParseFunction = ParseSDLSDone;
            lsRecordFunction = ParseSDLSRecord;
            this.answerBehaviour = AnswerBehaviour.SDCommandLS;
            this.currentSDParentNode = tempSDParentNode;
            this.SDQueryAnswer.Clear();

        }

        private Dictionary<string, string> fileNamesSubstitutions = new Dictionary<string, string>();
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

        string getFileNameFromText(string text)
        {
            if(fileNamesSubstitutions.ContainsValue(text))
            {
                string key = fileNamesSubstitutions.FirstOrDefault(x => x.Value == text).Key;
                return key;
            }

            return text;
        }

        string getTextFromFileName(string fName)
        {
            string fileName = fName.ToUpper();

            if(fileNamesSubstitutions.ContainsKey(fileName))
            {
                return fileNamesSubstitutions[fileName];
            }

            return fileName;
        }

        TreeNode tempSDParentNode = null;
        private void treeViewSD_BeforeExpand(object sender, TreeViewCancelEventArgs e)
        {
            TreeNode wantedExpand = e.Node;
            SDNodeTagHelper tg = (SDNodeTagHelper)wantedExpand.Tag;

            if (tg.Tag != SDNodeTags.TagFolderUninitedNode) // уже проинициализировали
                return;

            string folderName = tg.FileName;//getFileNameFromText(wantedExpand.Text);

            TreeNode parent = wantedExpand.Parent;
            while (parent != null)
            {
                SDNodeTagHelper nt = (SDNodeTagHelper)parent.Tag;
                folderName = /*getFileNameFromText(parent.Text)*/
            nt.FileName + PARAM_DELIMITER + folderName;
                parent = parent.Parent;
            }
            tempSDParentNode = wantedExpand;
            PushCommandToQueue(GET_PREFIX + "LS" + PARAM_DELIMITER + folderName, DummyAnswerReceiver, SetSDFolderReadingFlag);
        }

        private void SetSDFileReadingFlag()
        {
            this.answerBehaviour = AnswerBehaviour.SDCommandFILE;
            this.fileDataParseFunction = this.ViewFile;
            //this.fileDownloadFlags = FileDownloadFlags.View;
            this.SDQueryAnswer.Clear();
            ShowWaitCursor(true);
        }

        private int requestEthalonCounter = 0;
        private void SetSDFileReadingFlagEthalon()
        {
            this.answerBehaviour = AnswerBehaviour.SDCommandFILE;
            //this.fileDownloadFlags = FileDownloadFlags.DownloadEthalon;
            this.fileDataParseFunction = this.SaveEthalonFile;
            this.fileDownloadProgressFunction = null;
            this.SDQueryAnswer.Clear();

            switch(requestEthalonCounter)
            {
                case 0:
                        requestedFileName = "ET0UP.ETL";
                    break;

                case 1:
                    requestedFileName = "ET0DWN.ETL";
                    break;

                case 2:
                    requestedFileName = "ET1UP.ETL";
                    break;

                case 3:
                    requestedFileName = "ET1DWN.ETL";
                    break;

                case 4:
                    requestedFileName = "ET2UP.ETL";
                    break;

                case 5:
                    requestedFileName = "ET2DWN.ETL";
                    break;

            }

            requestEthalonCounter++;

        }


        private string requestedFileName = "";
        private int requestedFileSize = 0;
        private int fileReadedBytes = 0;

        private void ShowDownloadPercents(int percents, int bytesReceived)
        {
            this.statusProgressBar.Value = percents;
        }

        private void RequestFile(TreeNode node)
        {
            if (node == null)
                return;

            if (node.Tag == null)
                return;

            SDNodeTagHelper tg = (SDNodeTagHelper)node.Tag;
            if (tg.Tag != SDNodeTags.TagFileNode)
                return;

            //fileDownloadFlags = FileDownloadFlags.View;
            this.fileDataParseFunction = this.ViewFile;
            this.fileDownloadProgressFunction = ShowDownloadPercents;

            ShowWaitCursor(true);

            string fullPathName = tg.FileName;//getFileNameFromText(selectedNode.Text);


            TreeNode parent = node.Parent;
            while (parent != null)
            {
                SDNodeTagHelper nt = (SDNodeTagHelper)parent.Tag;
                fullPathName = /*getFileNameFromText(parent.Text)*/ nt.FileName + PARAM_DELIMITER + fullPathName;
                parent = parent.Parent;
            }

            requestedFileName = fullPathName;

            this.btnViewSDFile.Enabled = false;
            this.btnDeleteSDFile.Enabled = false;
            this.btnListSDFiles.Enabled = false;
            PushCommandToQueue(GET_PREFIX + "FILESIZE" + PARAM_DELIMITER + fullPathName, ParseAskFileSize);

        }

        private void ParseAskFileSize(Answer a)
        {
            if(!a.IsOkAnswer)
            {
                this.btnViewSDFile.Enabled = treeViewSD.SelectedNode != null;
                this.btnDeleteSDFile.Enabled = treeViewSD.SelectedNode != null;
                this.btnListSDFiles.Enabled = true;

                return;
            }

            requestedFileSize = Convert.ToInt32(a.Params[1]);
            fileReadedBytes = 0;

            statusProgressMessage.Text = "Вычитываем файл \"" + requestedFileName + "\"...";
            statusProgressMessage.Visible = true;
            statusProgressBar.Value = 0;
            statusProgressBar.Visible = true;
            PushCommandToQueue(GET_PREFIX + "FILE" + PARAM_DELIMITER + requestedFileName, DummyAnswerReceiver, SetSDFileReadingFlag);

        }

        private ArchiveImportForm archiveImportForm = null;
        private List<String> archiveLogsList = new List<string>();
        private List<String> archiveEthalonsList = new List<string>();

        private List<int> archiveLogsSizes = new List<int>();
        private List<int> archiveEthalonsSizes = new List<int>();

        private int archiveLogsIterator = 0;
        private int archiveEthalonsIterator = 0;
        private int waitForFileSizeCount = 0;
        private int waitForFileSizeDone = 0;

        private int archiveTotalFilesSize = 0;
        private int archiveWaitForListDone = 0;
        private int archiveListDoneCount = 0;

        public void StartArchive(ArchiveImportForm fm)
        {
            archiveImportForm = fm;
            archiveImportForm.DisableControls();

            DoArchive();

        }

        private void StartListLogs()
        {
            lsParseFunction = ParseLogsDone;
            lsRecordFunction = ParseLogsRecord;
            this.answerBehaviour = AnswerBehaviour.SDCommandLS;
            this.SDQueryAnswer.Clear();
        }

        private void ParseLogsDone()
        {
            archiveListDoneCount++;

            if (archiveListDoneCount >= archiveWaitForListDone)
            {
                // получили список файлов, получаем их размеры
                ArchiveRequestFileSizes();
            }
        }

        private void ParseLogsRecord(string rec)
        {
            archiveLogsList.Add(rec);
        }


        private void StartListEthalons()
        {
            lsParseFunction = ParseEthalonsDone;
            lsRecordFunction = ParseEthalonsRecord;
            this.answerBehaviour = AnswerBehaviour.SDCommandLS;
            this.SDQueryAnswer.Clear();
        }

        private void ParseEthalonsDone()
        {
            archiveListDoneCount++;

            if (archiveListDoneCount >= archiveWaitForListDone)
            {
                // получили список файлов, получаем их размеры
                ArchiveRequestFileSizes();
            }
        }

        private void ParseEthalonsRecord(string rec)
        {
            archiveEthalonsList.Add(rec);
        }

        private void ArchiveRequestFileSizes()
        {
            archiveImportForm.lblMessage.Text = "Получаем размер файлов для архивирования...";
            archiveTotalFilesSize = 0;

            if(archiveLogsList.Count < 1 && archiveEthalonsList.Count < 1)
            {
                // оба списка пустых, надо импортировать только настройки
                ArchiveImportSettings();
                return;
            }

            // тут получаем список файлов

            if (archiveEthalonsList.Count > 0)
                waitForFileSizeCount++;

            // requestedFileSize
            for (int i=0;i<archiveEthalonsList.Count;i++)
            {
                string fullPathName = "ETL" + PARAM_DELIMITER + archiveEthalonsList[i];
                PushCommandToQueue(GET_PREFIX + "FILESIZE" + PARAM_DELIMITER + fullPathName, ArchiveParseEthalonFileSize);
            }


            if (archiveLogsList.Count > 0)
                waitForFileSizeCount++;

            // requestedFileSize
            for (int i = 0; i < archiveLogsList.Count; i++)
            {
                string fullPathName = "LOG" + PARAM_DELIMITER + archiveLogsList[i];
                PushCommandToQueue(GET_PREFIX + "FILESIZE" + PARAM_DELIMITER + fullPathName, ArchiveParseLogsFileSize);
            }

        }

        private void ArchiveParseEthalonFileSize(Answer a)
        {
            archiveEthalonsIterator++;
            int fsize = 0;

            if(a.IsOkAnswer)
            {
                fsize = Convert.ToInt32(a.Params[1]);
                archiveImportForm.pbProgress.Maximum += fsize + 15;
            }

            archiveEthalonsSizes.Add(fsize);

            if(archiveEthalonsIterator >= archiveEthalonsList.Count)
            {
                // закончили получение списка эталонов
                waitForFileSizeDone++;

                if(waitForFileSizeDone >= waitForFileSizeCount)
                {
                    // закончили получение размеров для всех файлов, можно начинать скачивать
                    ArchiveDownloadFiles();
                }
            }
        }

        private void ArchiveParseLogsFileSize(Answer a)
        {
            archiveLogsIterator++;
            int fsize = 0;
            if (a.IsOkAnswer)
            {
                fsize = Convert.ToInt32(a.Params[1]);
                archiveImportForm.pbProgress.Maximum += fsize;
            }

            archiveLogsSizes.Add(fsize);

            if (archiveLogsIterator >= archiveLogsList.Count)
            {
                // закончили получение списка эталонов
                waitForFileSizeDone++;

                if (waitForFileSizeDone >= waitForFileSizeCount)
                {
                    // закончили получение размеров для всех файлов, можно начинать скачивать
                    ArchiveDownloadFiles();
                }
            }
        }

        private int waitForArchiveDownloadListCount = 0;
        private int waitForArchiveDownloadListDone = 0;

        private void ArchiveDownloadFiles()
        {
            archiveImportForm.lblMessage.Text = "Начинаем скачивать файлы...";
            waitForArchiveDownloadListDone = 0;
            waitForArchiveDownloadListCount = archiveLogsList.Count + archiveEthalonsList.Count;
            // тут начинаем скачивать файлы

            archiveEthalonsIterator = 0;
            for(int i=0;i< archiveEthalonsList.Count;i++)
            {
                string filePath = "ETL" + PARAM_DELIMITER + archiveEthalonsList[i];
                PushCommandToQueue(GET_PREFIX + "FILE" + PARAM_DELIMITER + filePath, DummyAnswerReceiver, ArchiveStartEthalonFileReading);
            }

            archiveLogsIterator = 0;
            for (int i = 0; i < archiveLogsList.Count; i++)
            {
                string filePath = "LOG" + PARAM_DELIMITER + archiveLogsList[i];
                PushCommandToQueue(GET_PREFIX + "FILE" + PARAM_DELIMITER + filePath, DummyAnswerReceiver, ArchiveStartLogFileReading);
            }

        }

        private void ArchiveStartEthalonFileReading()
        {
            archiveImportForm.lblMessage.Text = "Скачиваем файл \"" + archiveEthalonsList[archiveEthalonsIterator] + "\"...";

            requestedFileSize = archiveEthalonsSizes[archiveEthalonsIterator];
            this.answerBehaviour = AnswerBehaviour.SDCommandFILE;
            this.fileDataParseFunction = ArchiveEthalonFileReceived;
            this.fileDownloadProgressFunction = ArchiveFileDownloadProgress;
            this.SDQueryAnswer.Clear();
            archiveEthalonsIterator++;
        }

        private void ArchiveStartLogFileReading()
        {
            archiveImportForm.lblMessage.Text = "Скачиваем файл \"" + archiveLogsList[archiveLogsIterator] + "\"...";

            requestedFileSize = archiveLogsSizes[archiveLogsIterator];
            this.answerBehaviour = AnswerBehaviour.SDCommandFILE;
            this.fileDataParseFunction = ArchiveLogFileReceived;
            this.fileDownloadProgressFunction = ArchiveFileDownloadProgress;
            this.SDQueryAnswer.Clear();
            archiveLogsIterator++;
        }

        private void ArchiveImportSettings()
        {
            if (archiveImportForm.cbSettings.Checked)
            {
                // импортируем настройки
                archiveImportForm.lblMessage.Text = "Импортируем настройки...";

                ArchiveSettings aSett = new ArchiveSettings();
                aSett.ApplyFromConfig();

                string filename = Application.StartupPath + "\\Archive\\" + Config.Instance.ControllerGUID + "\\Settings\\";

                try
                {
                    System.IO.Directory.CreateDirectory(filename);
                    filename += "settings.xml";
                    aSett.Save(filename);
                }
                catch { }
            }


            //Тут заполнения дерева архива новой записью
            AddItemToArchive(Config.Instance.ControllerGUID);

            archiveImportForm.pbProgress.Value = archiveImportForm.pbProgress.Maximum;
            archiveImportForm.lblMessage.Text = "Готово.";

            MessageBox.Show("Импорт успешно завершён!");
            archiveImportForm.Done();
            archiveImportForm = null;
        }

        private void AddItemToArchive(string guid)
        {
            // ищем, есть ли такая запись. Если есть - обновляем, если нет - добавляем
            TreeNode archiveNode = treeView.Nodes[1];
            TreeNode existingNode = null;
            ArchiveTreeRootItem atri = null;

            for (int i=0;i< archiveNode.Nodes.Count;i++)
            {
                TreeNode child = archiveNode.Nodes[i];
                if(child.Tag is ArchiveTreeRootItem)
                {
                    atri = child.Tag as ArchiveTreeRootItem;
                    if(atri.GUID == guid)
                    {
                        existingNode = child;
                        if (ControllerNames.Instance.Names.ContainsKey(guid))
                            existingNode.Text = ControllerNames.Instance.Names[guid];
                        else
                            existingNode.Text = guid;

                        existingNode.Nodes.Clear();
                        break;
                    }
                }
            } // for

            if(existingNode == null)
            {
                atri = new ArchiveTreeRootItem(guid);
                existingNode = archiveNode.Nodes.Add(atri.ToString());
                existingNode.Tag = atri;
                existingNode.ImageIndex = 8;
                existingNode.SelectedImageIndex = 8;
            }

            // тут добавляем дочерние ноды - эталоны и логи
            TreeNode ethalonNode = existingNode.Nodes.Add("Эталоны");
            ethalonNode.Tag = new ArchiveTreeEthalonItem(atri);
            ethalonNode.ImageIndex = 14;
            ethalonNode.SelectedImageIndex = 14;

            FillArchiveEthalonsList(ethalonNode, ethalonNode.Tag as ArchiveTreeEthalonItem);

            TreeNode logNode = existingNode.Nodes.Add("Логи");
            logNode.Tag = new ArchiveTreeLogItem(atri);
            logNode.ImageIndex = 13;
            logNode.SelectedImageIndex = 13;

            FillArchiveLogsList(logNode, logNode.Tag as ArchiveTreeLogItem);

        }

        private void FillArchiveEthalonsList(TreeNode n, ArchiveTreeEthalonItem eti)
        {
            n.Nodes.Clear();
            string path = Application.StartupPath + "\\Archive\\" + eti.Parent.GUID + "\\ETL\\";

            try
            {
                System.IO.Directory.CreateDirectory(path);
            }
            catch { }

            string[] files = System.IO.Directory.GetFiles(path);

            foreach(string fullfilename in files)
            {
                string fileDisplayName = getTextFromFileName(System.IO.Path.GetFileName(fullfilename));
                TreeNode child = n.Nodes.Add(fileDisplayName);
                child.ImageIndex = 16;
                child.SelectedImageIndex = 16;
                ArchiveTreeEthalonItemRecord ateir = new ArchiveTreeEthalonItemRecord(eti, fullfilename);
                child.Tag = ateir;
            }
        }

        private void FillArchiveLogsList(TreeNode n, ArchiveTreeLogItem eti)
        {
            n.Nodes.Clear();
            string path = Application.StartupPath + "\\Archive\\" + eti.Parent.GUID + "\\LOG\\";

            try
            {
                System.IO.Directory.CreateDirectory(path);
            }
            catch { }

            string[] files = System.IO.Directory.GetFiles(path);

            foreach (string file in files)
            {
                string fileDisplayName = System.IO.Path.GetFileName(file);
                TreeNode child = n.Nodes.Add(fileDisplayName);
                child.ImageIndex = 15;
                child.SelectedImageIndex = 15;
                ArchiveTreeLogItemRecord ateir = new ArchiveTreeLogItemRecord(eti, file);
                child.Tag = ateir;
            }
        }

        private void SaveArchiveFile(string fileName, string dirName, List<byte> content)
        {
            String path = Application.StartupPath + "\\Archive\\" + Config.Instance.ControllerGUID + "\\" + dirName + "\\";
            try
            {
                System.IO.Directory.CreateDirectory(path);
                path += fileName;

                try
                {
                    System.IO.BinaryWriter bw = new System.IO.BinaryWriter(new System.IO.FileStream(path, System.IO.FileMode.Create));

                    for (int i = 0; i < content.Count; i++)
                        bw.Write(content[i]);

                    bw.Close();
                }
                catch
                {
                }


            }
            catch
            {

            }
        }

        private void ArchiveEthalonFileReceived(List<byte> content)
        {
            // тут получены данные файла эталона
            string fileName = archiveEthalonsList[archiveEthalonsIterator-1];

            //тут сохраняем файл на диске
            SaveArchiveFile(fileName, "ETL", content);

            waitForArchiveDownloadListDone++;
            if(waitForArchiveDownloadListDone >= waitForArchiveDownloadListCount)
            {
                // получены все файлы, можно импортировать настройки
                ArchiveImportSettings();
            }
        }

        private void ArchiveLogFileReceived(List<byte> content)
        {
            // тут получены данные файла лога
            string fileName = archiveLogsList[archiveLogsIterator - 1];

            //тут сохраняем файл на диске
            SaveArchiveFile(fileName, "LOG", content);

            waitForArchiveDownloadListDone++;
            if (waitForArchiveDownloadListDone >= waitForArchiveDownloadListCount)
            {
                // получены все файлы, можно импортировать настройки
                ArchiveImportSettings();
            }
        }

        private void ArchiveFileDownloadProgress(int percents, int bytesReceived)
        {
            try
            {
                archiveImportForm.pbProgress.Value += bytesReceived;
            }
            catch
            {

            }
        }

        private void DoArchive()
        {
            archiveLogsList.Clear();
            archiveEthalonsList.Clear();

            archiveLogsSizes.Clear();
            archiveEthalonsSizes.Clear();

            archiveLogsIterator = 0;
            archiveEthalonsIterator = 0;
            waitForFileSizeCount = 0;
            waitForFileSizeDone = 0;

            archiveTotalFilesSize = 0;
            archiveWaitForListDone = 0;
            archiveListDoneCount = 0;

            ControllerNames.Instance.Names[Config.Instance.ControllerGUID] = archiveImportForm.tbControllerName.Text.Trim();
            ControllerNames.Instance.Save();

            archiveImportForm.lblMessage.Text = "Получаем список файлов для архивирования...";
            archiveImportForm.pbProgress.Maximum = 3;
            archiveImportForm.pbProgress.Value = 1;

            if (archiveImportForm.cbEthalons.Checked)
            {
                // запрошено архивирование эталонов
                archiveWaitForListDone++;
                PushCommandToQueue(GET_PREFIX + "LS|ETL", DummyAnswerReceiver, StartListEthalons);
            }

            if (archiveImportForm.cbLogs.Checked)
            {
                // запрошено архивирование логов
                archiveWaitForListDone++;
                PushCommandToQueue(GET_PREFIX + "LS|LOG", DummyAnswerReceiver, StartListLogs);
            }


            if (archiveListDoneCount >= archiveWaitForListDone)
            {
                // получили список файлов, получаем их размеры
                ArchiveRequestFileSizes();
            }
            
        }

        private void tmEnumComPorts_Tick(object sender, EventArgs e)
        {

            string[] ports = SerialPort.GetPortNames();

            // сначала удаляем те порты, которых нет в списке текущих
            List<ToolStripMenuItem> toRemove = new List<ToolStripMenuItem>();

            foreach (ToolStripMenuItem existing in miPort.DropDownItems)
            {
                bool found = false;
                foreach (string port in ports)
                {
                    if(port == existing.Text)
                    {
                        found = true;
                        break;
                    }
                }

                if(!found)
                {
                    toRemove.Add(existing);
                }
            }

            // теперь чистим
            for(int i=0;i< toRemove.Count;i++)
            {
                miPort.DropDownItems.Remove(toRemove[i]);
            }

            foreach (string port in ports)
            {
                // ищем - есть ли такой порт уже?
                bool found = false;
                foreach(ToolStripMenuItem existing in miPort.DropDownItems)
                {
                    if(existing.Text == port)
                    {
                        found = true;
                        break;
                    }
                }

                if (found)
                    continue;

                ToolStripMenuItem ti = new ToolStripMenuItem(port);

                ti.MergeIndex = 1;
                ti.AutoSize = true;
                ti.ImageScaling = ToolStripItemImageScaling.None;
                ti.Tag = port;
                ti.Click += ConnectToSelectedComPort;
                ti.CheckOnClick = false;



                miPort.DropDownItems.Add(ti);


            }
        }


        private void ChangePortSpeed(object sender, EventArgs e)
        {
            ToolStripMenuItem selItem = sender as ToolStripMenuItem;

          foreach(ToolStripMenuItem tmi in this.portSpeedToolStripMenuItem.DropDownItems)
            {
                tmi.Checked = false;
            }
            selItem.Checked = true;
        }

        private bool inSetMotoresourceToController = true;
        private void btnSetMotoresourceCurrent_Click(object sender, EventArgs e)
        {
            inSetMotoresourceToController = true;
            ShowWaitCursor(true);

            string s = "";
            s += Convert.ToString(nudMotoresourceCurrent1.Value);//DEPRECATED:  + PARAM_DELIMITER;
            //DEPRECATED: s += Convert.ToString(nudMotoresourceCurrent2.Value) + PARAM_DELIMITER;
            //DEPRECATED: s += Convert.ToString(nudMotoresourceCurrent3.Value);

            PushCommandToQueue(SET_PREFIX + "RES_CUR" + PARAM_DELIMITER + s, ParseSetMotoresourceCurrent);

            s = "";
            s += Convert.ToString(nudMotoresourceMax1.Value);//DEPRECATED:  + PARAM_DELIMITER;
            //DEPRECATED: s += Convert.ToString(nudMotoresourceMax2.Value) + PARAM_DELIMITER;
            //DEPRECATED: s += Convert.ToString(nudMotoresourceMax3.Value);

            PushCommandToQueue(SET_PREFIX + "RES_MAX" + PARAM_DELIMITER + s, ParseSetMotoresourceMax);
        }

        private bool inSetPulsesToController = true;
        private void btnSetPulses_Click(object sender, EventArgs e)
        {
            inSetPulsesToController = true;
            ShowWaitCursor(true);

            string s = "";
            s += Convert.ToString(nudDelta1.Value);

            PushCommandToQueue(SET_PREFIX + "DELTA" + PARAM_DELIMITER + s, ParseSetDelta);


            s = "";
            s += Convert.ToString(nudPulses1.Value);


            PushCommandToQueue(SET_PREFIX + "PULSES" + PARAM_DELIMITER + s, ParseSetPulses);
        }


        private bool inSetDeltaToController = true;
        private void btnSetDelta_Click(object sender, EventArgs e)
        {
            inSetDeltaToController = true;
            ShowWaitCursor(true);

            string s = "";
            s += Convert.ToString(nudEthalonCompareDelta.Value);
            PushCommandToQueue(SET_PREFIX + "ECDELTA" + PARAM_DELIMITER + s, ParseSetECDelta);


            s = "";
            s += Convert.ToString(nudSkipCounter.Value);
            PushCommandToQueue(SET_PREFIX + "SKIPC" + PARAM_DELIMITER + s, ParseSetSkipCounter);
        }

        /*
         //DEPRECATED: 
        private void GetInductiveSensors()
        {
            PushCommandToQueue(GET_PREFIX + "IND", ParseInductiveSensors);
        }
        */

        private void GetVoltage()
        {
            PushCommandToQueue(GET_PREFIX + "VDATA", ParseVoltage);

        }


        private void tmInductiveTimer_Tick(object sender, EventArgs e)
        {
            //DEPRECATED: GetInductiveSensors();
            GetVoltage();
        }

        private void ResetVoltage()
        {
            lblVoltage1.BackColor = Color.LightGray;
            lblVoltage1.Text = "-";

            lblVoltage2.BackColor = Color.LightGray;
            lblVoltage2.Text = "-";

            lblVoltage3.BackColor = Color.LightGray;
            lblVoltage3.Text = "-";
        }

        private void ParseVoltage(Answer a)
        {
            if (a.IsOkAnswer)
            {
                int VOLTAGE_THRESHOLD = 10;
                NumberFormatInfo nfi = new NumberFormatInfo();
                nfi.NumberDecimalSeparator = ".";

                string end = "V";

                try
                {

                    int vdata = Convert.ToInt32(a.Params[1]);
                    float threshold = (3.3f / 100) * VOLTAGE_THRESHOLD;
                    float lowBorder = 3.3f - threshold;
                    float highBorder = 3.3f + threshold;

                    float currentV = vdata * (3.3f / 4096 * 2);

                    if (currentV >= lowBorder && currentV <= highBorder)
                    {
                        lblVoltage1.BackColor = Color.LightGreen;
                    }
                    else
                    {
                        lblVoltage1.BackColor = Color.LightSalmon;
                    }

                    lblVoltage1.Text = currentV.ToString("n1", nfi) + end;


                }
                catch { }

                try
                {

                    int vdata = Convert.ToInt32(a.Params[2]);
                    float threshold = (5.0f / 100) * VOLTAGE_THRESHOLD;
                    float lowBorder = 5.0f - threshold;
                    float highBorder = 5.0f + threshold;

                    float currentV = vdata * (3.3f / 4096 * 2);

                    if (currentV >= lowBorder && currentV <= highBorder)
                    {
                        lblVoltage2.BackColor = Color.LightGreen;
                    }
                    else
                    {
                        lblVoltage2.BackColor = Color.LightSalmon;
                    }

                    lblVoltage2.Text = currentV.ToString("n1", nfi) + end;


                }
                catch { }

                try
                {

                    int vdata = Convert.ToInt32(a.Params[3]);
                    float threshold = (200.0f / 100) * VOLTAGE_THRESHOLD;
                    float lowBorder = 200.0f - threshold;
                    float highBorder = 200.0f + threshold;

                    float currentV = vdata * (3.3f / 4096 * 100);

                    if (currentV >= lowBorder && currentV <= highBorder)
                    {
                        lblVoltage3.BackColor = Color.LightGreen;
                    }
                    else
                    {
                        lblVoltage3.BackColor = Color.LightSalmon;
                    }

                    lblVoltage3.Text = currentV.ToString("n1", nfi) + end;


                }
                catch { }


            }
            else
            {
                ResetVoltage();
            }
        }

        private void MotoresourceCurrentValueChanged(object sender, EventArgs e)
        {
            UpdateMotoresourcePercents();
        }

        private void treeViewSD_BeforeSelect(object sender, TreeViewCancelEventArgs e)
        {
            /*
            SDNodeTagHelper h = e.Node.Tag as SDNodeTagHelper;
            if (h == null || h.IsDirectory)
                e.Cancel = true;
                */
            }

        private void treeViewSD_AfterSelect(object sender, TreeViewEventArgs e)
        {
            btnDeleteSDFile.Enabled = false;
            btnViewSDFile.Enabled = false;

            if (treeViewSD.SelectedNode == null)
            {
                return;
            }

            SDNodeTagHelper h = e.Node.Tag as SDNodeTagHelper;
            if (h == null || h.IsDirectory)
                return;

            btnDeleteSDFile.Enabled = true;
            btnViewSDFile.Enabled = true;

        }

        private void btnDeleteSDFile_Click(object sender, EventArgs e)
        {
            if (treeViewSD.SelectedNode == null)
                return;

            SDNodeTagHelper h = treeViewSD.SelectedNode.Tag as SDNodeTagHelper;

            if (h == null || h.IsDirectory)
                return;

            string fileName = h.FileName;

            string fAsk = treeViewSD.SelectedNode.Text;
            if (fileName != fAsk)
                fAsk += " (" + fileName + ")";

            if (MessageBox.Show("Вы уверены, что хотите удалить файл \"" + fAsk + "\"?", "Подтверждение", MessageBoxButtons.YesNo, MessageBoxIcon.Question) != DialogResult.Yes)
                return;

            TreeNode parent = treeViewSD.SelectedNode.Parent;
            while (parent != null)
            {
                SDNodeTagHelper nt = (SDNodeTagHelper)parent.Tag;
                fileName =  nt.FileName + PARAM_DELIMITER + fileName;
                parent = parent.Parent;
            }


           treeViewSD.SelectedNode.Remove();
            plEmptySDWorkspace.BringToFront();

           PushCommandToQueue(SET_PREFIX + "DELFILE" + PARAM_DELIMITER + fileName, DummyAnswerReceiver);


        }

        private void btnViewSDFile_Click(object sender, EventArgs e)
        {
            RequestFile(treeViewSD.SelectedNode);
        }

        private void treeViewSD_MouseDoubleClick(object sender, MouseEventArgs e)
        {
            RequestFile(treeViewSD.SelectedNode);
        }

        const int customLabelsCount = 15; // сколько всего наших меток будет на оси X

        private void ShowChart(InterruptRecord record, string stationID, string stationName, bool modal)
        {
            //  System.Diagnostics.Debug.Assert(record != null);
            if (record == null)
                return;

            ViewChartForm vcf = new ViewChartForm(record, stationID, stationName);

            vcf.setDefaultFileName(record.InterruptInfo.InterruptTime.ToString("yyyy-MM-dd HH.mm"));

            vcf.lblCaption.Text = "Срабатывание от " + record.InterruptInfo.InterruptTime.ToString("dd.MM.yyyy HH:mm:ss");

            // добавляем кол-во импульсов на график
            vcf.chart.Legends[1].CustomItems[0].Cells["PulsesCount"].Text = String.Format("Импульсов: {0}", record.InterruptData.Count);
            vcf.chart.Legends[1].CustomItems[0].Cells["EthalonPulses"].Text = String.Format("Эталон: {0}", record.EthalonData.Count);
            vcf.chart.Legends[1].CustomItems[0].Cells["TrigDate"].Text = "Дата: " + record.InterruptInfo.InterruptTime.ToString("dd.MM.yyyy HH:mm:ss");
            vcf.chart.Legends[1].CustomItems[0].Cells["Place"].Text = stationName;


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

            //int xStep = 1;

            // СПИСОК ПРИХОДИТ НОРМАЛИЗОВАННЫМ ОТНОСИТЕЛЬНО ПЕРВОЙ ЗАПИСИ!!!
            List<int> timeList = record.InterruptData;

            // получаем максимальное время импульса - это будет 100% по оси Y
            int maxPulseTime = 0;
            for (int i = 1; i < timeList.Count; i++)
            {
                maxPulseTime = Math.Max(maxPulseTime, (timeList[i] - timeList[i - 1]));
            }

            // int endStop = timeList.Count;

            if (record.EthalonData.Count > 0)
            {
                for (int i = 1; i < record.EthalonData.Count; i++)
                {
                    maxPulseTime = Math.Max(maxPulseTime, (record.EthalonData[i] - record.EthalonData[i - 1]));
                }
            }

            //endStop = Math.Min(endStop, record.EthalonData.Count);

            //if (record.EthalonData.Count < 1)
              //  endStop = timeList.Count;


            //double xCoord = 0;
            // DateTime xCoord = interruptTime;
            //xCoord = xCoord.AddMilliseconds(pulsesOffset);
            int xCoord = pulsesOffset;
             List<int> XValuesInterrupt = new List<int>();
             List<double> YValuesInterrupt = new List<double>();

            // добавляем фейковые начальные точки
            //System.Windows.Forms.DataVisualization.Charting.DataPoint ptFake1 = new System.Windows.Forms.DataVisualization.Charting.DataPoint();
            //ptFake1.XValue = xCoord;
            //ptFake1.SetValueY(0);
            //xCoord += xStep;
            //interruptSerie.Points.Add(ptFake1);

            // вот тут нам надо добавлять недостающие времена, от начала времени токов, до срабатывания защиты
            if(record.CurrentTimes.Count > 0)
            {
                for(int z=0;z<record.CurrentTimes.Count;z++)
                {
                    if (record.CurrentTimes[z] >= pulsesOffset)
                        break;

                    XValuesInterrupt.Add(record.CurrentTimes[z]);
                    YValuesInterrupt.Add(0);
                }
            }


            XValuesInterrupt.Add(xCoord);
            YValuesInterrupt.Add(0);


            // теперь считаем все остальные точки
            for (int i = 1; i < timeList.Count; i++)
            {
                int pulseTime = timeList[i] - timeList[i - 1];
                //pulseTime *= 100;

                int pulseTimePercents = (pulseTime*100) / maxPulseTime;
                pulseTimePercents = 100 - pulseTimePercents;

                // абсолютное инвертированное значение от maxPulseTime
                // maxPulseTime = 100%
                // x = pulseTimePercents
                // x = (pulseTimePercents*maxPulseTime)/100;

                xCoord += pulseTime;
                XValuesInterrupt.Add(xCoord);
                //YValuesInterrupt.Add(pulseTimePercents);
                YValuesInterrupt.Add((pulseTimePercents * maxPulseTime) / 100);

            } // for

            
            // убираем последний пик вверх
            
            if(YValuesInterrupt.Count > 1)
            {
                YValuesInterrupt[YValuesInterrupt.Count - 1] = 0;
            }

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

            //xCoord =  interruptTime;
            xCoord = pulsesOffset;
            //xCoord = xCoord.AddMilliseconds(pulsesOffset);
            List<int> XValuesEthalon = new List<int>();
            List<double> YValuesEthalon = new List<double>();

            // добавляем фейковые начальные точки
            //System.Windows.Forms.DataVisualization.Charting.DataPoint ptFake2 = new System.Windows.Forms.DataVisualization.Charting.DataPoint();
            //ptFake2.XValue = xCoord;
            //ptFake2.SetValueY(0);
            //xCoord += xStep;
            //ethalonSerie.Points.Add(ptFake2);

            XValuesEthalon.Add(xCoord);
            YValuesEthalon.Add(0);

            // считаем график эталона
            if (record.EthalonData.Count > 0)
            {
                for (int i = 1; i < record.EthalonData.Count; i++)
                {
                    int pulseTime = record.EthalonData[i] - record.EthalonData[i - 1];
                    //pulseTime *= 100;

                    int pulseTimePercents = (pulseTime * 100) / maxPulseTime;
                    pulseTimePercents = 100 - pulseTimePercents;


                    // System.Windows.Forms.DataVisualization.Charting.DataPoint pt = new System.Windows.Forms.DataVisualization.Charting.DataPoint();
                    // pt.XValue = xCoord;
                    // pt.SetValueY(pulseTimePercents);

                    //  xCoord += xStep;

                    //  ethalonSerie.Points.Add(pt);

                    //xCoord = xCoord.AddMilliseconds(pulseTime);
                    xCoord += pulseTime;
                    XValuesEthalon.Add(xCoord);
                    //YValuesEthalon.Add(pulseTimePercents);
                    YValuesEthalon.Add((pulseTimePercents * maxPulseTime) / 100);

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
            if(record.InterruptData.Count > 0)
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
            // int step = maxTime / customLabelsCount;
            int neareastValue = 100000; // приближение к 100 мс
            int step = Convert.ToInt32(Math.Round(Convert.ToDouble(maxTime / customLabelsCount) / neareastValue, 0)) * neareastValue;

            for (int kk = 0; kk < vcf.chart.ChartAreas.Count; kk++)
            {
                ChartArea area = vcf.chart.ChartAreas[kk];
                area.AxisX.CustomLabels.Clear();

                int startOffset = -step / 2;
                int endOffset = step / 2;
                int counter = 0;

                for (int i = 0; i < customLabelsCount; i++)
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

            
            // теперь пробуем для графика прерываний - переназначить метки
            {
                int interruptLabelsCount = 6;
                step = maxPulseTime / interruptLabelsCount;

                ChartArea area = vcf.chart.ChartAreas[0];
                area.AxisY.CustomLabels.Clear();
                area.AxisY.Interval = step;
                area.AxisY.IntervalType = DateTimeIntervalType.Number; // тип интервала

                int startOffset = -step / 2;
                int endOffset = step / 2;
                int counter = 0;

                for (int i = 0; i < interruptLabelsCount; i++)
                {
                    string labelText = String.Format("{0}us", maxPulseTime - counter);
                    CustomLabel monthLabel = new CustomLabel(startOffset, endOffset, labelText, 0, LabelMarkStyle.None);
                    area.AxisY.CustomLabels.Add(monthLabel);
                    startOffset = startOffset + step;
                    endOffset = endOffset + step;
                    counter += step;
                }
            }
            

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

        private void AddCustomLabelsOfCurrent(ChartArea area, int maxCurrentValue)
        {
            int currentLabelsCount = 6;
            float step = Convert.ToSingle(maxCurrentValue) / currentLabelsCount;

            area.AxisY.CustomLabels.Clear();
            area.AxisY.Interval = step;
            area.AxisY.IntervalType = DateTimeIntervalType.Number; // тип интервала

            float startOffset = -step / 2;
            float endOffset = step / 2;
            float counter = 0;

            for (int i = 0; i < currentLabelsCount; i++)
            {
                string labelText = String.Format("{0:0.000}A", GetCurrentFromADC(counter));
                CustomLabel monthLabel = new CustomLabel(startOffset, endOffset, labelText, 0, LabelMarkStyle.None);
                area.AxisY.CustomLabels.Add(monthLabel);
                startOffset = startOffset + step;
                endOffset = endOffset + step;
                counter += step;
            }
        }

        private float GetCurrentFromADC(float adcVAL)
        {
            float result = 0;
            float CURRENT_DIVIDER = 1000.0f;
            float COEFF_1 = 5.0f;
            float currentCoeff = 3160.0f;
            currentCoeff /= 1000;

            float intermediate = (COEFF_1 * adcVAL) / currentCoeff;

            result = (intermediate / CURRENT_DIVIDER);

            return result;
        }

        private void approximate(int countPasses, ref List<double> lst)
        {
            if (lst.Count < 3)
                return;
            for (int k = 0; k < countPasses; k++)
            {

                for (int i = 1; i < lst.Count - 1; i++)
                {
                    lst[i] = (lst[i - 1] + lst[i + 1]) / 2.0;
                }
            }   
        }

        private long map(long x, long in_min, long in_max, long out_min, long out_max)
        {
            return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
        }

        private int map(int x, int in_min, int in_max, int out_min, int out_max)
        {
            return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
        }

        private void logDataGrid_CellContentClick(object sender, DataGridViewCellEventArgs e)
        {
            var senderGrid = (DataGridView)sender;

            if (senderGrid.Columns[e.ColumnIndex] is DataGridViewButtonColumn &&
                    e.RowIndex >= 0)
            {
                TreeNode child = treeView.SelectedNode;
                string stationID = "";
                string stationName = "";

                if(child != null && child.Tag is ArchiveTreeLogItemRecord)
                {
                    ArchiveTreeLogItemRecord ali = child.Tag as ArchiveTreeLogItemRecord;
                    ArchiveTreeRootItem atri = ali.Parent.Parent;
                    stationID = atri.GUID;
                    stationName = atri.GUID;
                    if (ControllerNames.Instance.Names.ContainsKey(stationID))
                        stationName = ControllerNames.Instance.Names[stationID];

                }

                ShowChart(senderGrid.Rows[e.RowIndex].Tag as InterruptRecord, stationID, stationName,true);
            }
        }
        /*

        private void SaveEthalonToDisk(string dirName, List<int> data, string filename)
        {
            string resultfname = dirName + filename;

            System.IO.BinaryWriter bw;
            try
            {
                bw = new System.IO.BinaryWriter(new System.IO.FileStream(resultfname, System.IO.FileMode.Create));

                for (int i = 0; i < data.Count; i++)
                    bw.Write(data[i]);

                bw.Close();
            }
            catch
            {
                return;
            }

        }
        */

        private void btnSaveEthalons_Click(object sender, EventArgs e)
        {
            /*
            if(folderBrowserDialog.ShowDialog() != DialogResult.OK)
            {
                return;
            }
            string dir = folderBrowserDialog.SelectedPath;

            if (!dir.EndsWith("\\"))
                dir += "\\";

            dir += Config.Instance.ControllerGUID + "\\";
            try
            {
                System.IO.Directory.CreateDirectory(dir);
            }
            catch
            {
                MessageBox.Show("Не получается создать папку \"" + dir + "\"!");
                return;
            }

            SaveEthalonToDisk(dir, this.ethalon0UpData, "Канал 1, вверх.ETL");
            SaveEthalonToDisk(dir, this.ethalon0DwnData, "Канал 1, вниз.ETL");

            SaveEthalonToDisk(dir, this.ethalon1UpData, "Канал 2, вверх.ETL");
            SaveEthalonToDisk(dir, this.ethalon1DwnData, "Канал 2, вниз.ETL");

            SaveEthalonToDisk(dir, this.ethalon2UpData, "Канал 3, вверх.ETL");
            SaveEthalonToDisk(dir, this.ethalon2DwnData, "Канал 3, вниз.ETL");

            MessageBox.Show("Эталоны сохранены по адресу \"" + dir + "\".");
            */
        }

        byte[] dataToSend = null;
        bool inUploadFileToController = false;
        private void SendEthalonData()
        {
            this.currentTransport.Write(dataToSend, dataToSend.Length);
            inUploadFileToController = false;
        }

        private void FillEthalonWithData(List<int> etl, byte[] data)
        {
            etl.Clear();
            byte[] dt = new byte[4];
            for (int i = 0; i < data.Length; i += 4)
            {
                try
                {

                    dt[0] = data[i];
                    dt[1] = data[i + 1];
                    dt[2] = data[i + 2];
                    dt[3] = data[i + 3];

                    int curVal = BitConverter.ToInt32(dt, 0);
                    etl.Add(curVal);
                }
                catch
                {
                    break;
                }
            }
        }

        private int uploadedEthalonChannel = 0;
        private int uploadedEthalonRod = 0;

        private void btnUploadEthalon_Click(object sender, EventArgs e)
        {
            UploadFileDialog ufd = new UploadFileDialog(this);
            if(ufd.ShowDialog() == DialogResult.OK)
            {
                string sourcefilename = ufd.GetSelectedFileName();
                string targetfilename = ufd.GetTargetFileName();
                if (sourcefilename.Length < 1)
                    return;



                try
                {
                    dataToSend = System.IO.File.ReadAllBytes(sourcefilename);
                    ShowWaitCursor(true);
                    uploadedEthalonChannel = Convert.ToInt32(ufd.nudChannelNumber.Value) - 1;
                    uploadedEthalonRod = ufd.cbRodMove.SelectedIndex;
                    inUploadFileToController = true;
                    PushCommandToQueue(SET_PREFIX + "UPL|" + dataToSend.Length.ToString() + "|" + targetfilename, UploadEthalonCompleted, null, SendEthalonData);
                }
                catch
                {
                    ShowWaitCursor(false);
                }

            }
        }

        private void UploadEthalonCompleted(Answer a)
        {
            ShowWaitCursor(false);
            if(a.IsOkAnswer)
            {
                
                switch(this.uploadedEthalonChannel)
                {
                    case 0:
                        {
                            if (this.uploadedEthalonRod == 0)
                                FillEthalonWithData(this.ethalon0UpData, dataToSend);
                            else
                                FillEthalonWithData(this.ethalon0DwnData, dataToSend);
                        }
                        break;

                    case 1:
                        {
                            if (this.uploadedEthalonRod == 0)
                                FillEthalonWithData(this.ethalon1UpData, dataToSend);
                            else
                                FillEthalonWithData(this.ethalon1DwnData, dataToSend);
                        }
                        break;

                    case 2:
                        {
                            if (this.uploadedEthalonRod == 0)
                                FillEthalonWithData(this.ethalon2UpData, dataToSend);
                            else
                                FillEthalonWithData(this.ethalon2DwnData, dataToSend);
                        }
                        break;
                }
                dataToSend = null;

                MessageBox.Show("Эталон загружен в контроллер!", "Сообщение", MessageBoxButtons.OK, MessageBoxIcon.Information);

            }
            else
            {
                dataToSend = null;
                MessageBox.Show("Не удалось загрузить эталон в контроллер!", "Ошибка", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        private void MainForm_FormClosed(object sender, FormClosedEventArgs e)
        {
            ControllerNames.Instance.Save();
        }

        private void btnControllerName_Click(object sender, EventArgs e)
        {
            ControllerNameForm cnf = new ControllerNameForm();
            cnf.ShowDialog();

            TreeNode archiveNode = treeView.Nodes[1];

            // переименовываем в архиве
            for (int i = 0; i < archiveNode.Nodes.Count; i++)
            {
                TreeNode child = archiveNode.Nodes[i];
                if (child.Tag is ArchiveTreeRootItem)
                {
                    ArchiveTreeRootItem atri = child.Tag as ArchiveTreeRootItem;
                    if (atri.GUID == Config.Instance.ControllerGUID)
                    {
                        TreeNode existingNode = child;
                        if (ControllerNames.Instance.Names.ContainsKey(Config.Instance.ControllerGUID))
                            existingNode.Text = ControllerNames.Instance.Names[Config.Instance.ControllerGUID];
                        else
                            existingNode.Text = Config.Instance.ControllerGUID;

                        break;
                    }
                }
            } // for

            setConnectionStatusMessage();
        }

        private void btnImportSettings_Click(object sender, EventArgs e)
        {
            ArchiveImportForm af = new ArchiveImportForm(this);
            af.ShowDialog();
        }

        private string archiveEthalonChartExportFileName;

        private void ShowArchiveEthalon(ArchiveTreeEthalonItemRecord atei)
        {
            string fname = atei.FileName;
            DateTime modification = System.IO.File.GetLastWriteTime(fname);
            archiveEthalonChartExportFileName = modification.ToString("yyyy-MM-dd HH.mm");

            try
            {
                List<byte> content = new List<byte>(System.IO.File.ReadAllBytes(fname));
                CreateChart(content, this.archiveAthalonChart);
                this.plArchiveEthalonChart.BringToFront();
            }
            catch { }
            
        }

        private async void DoReadArchiveLog(ConnectForm frm, string fname)
        {

            byte[] result;
            using (System.IO.FileStream SourceStream = System.IO.File.Open(fname, System.IO.FileMode.Open))
            {
                result = new byte[SourceStream.Length];
                await SourceStream.ReadAsync(result, 0, (int)SourceStream.Length);
            }

            //List<byte> content = new List<byte>(System.IO.File.ReadAllBytes(fname));
            List<byte> content = new List<byte>(result);
            ShowLogFile(content, this.archiveLogDataGrid, "1", false,frm,null,false);
            this.archiveLogDataGrid.BringToFront();

            frm.DialogResult = DialogResult.OK;

        }

        private ArchiveTreeLogItemRecord archiveWorkRecord = null;
        private void DoShowArchiveLog(ConnectForm frm)
        {
            System.Diagnostics.Debug.Assert(archiveWorkRecord != null);
            string fname = archiveWorkRecord.FileName;
            frm.Update();
            try
            {
                /*
                List<byte> content = new List<byte>(System.IO.File.ReadAllBytes(fname));
                ShowLogFile(content, this.archiveLogDataGrid, "1", false);
                this.archiveLogDataGrid.BringToFront();
                */
                DoReadArchiveLog(frm, fname);
            }
            catch {
                frm.DialogResult = DialogResult.OK;
            }

            
        }

        private void ShowArchiveLog(ArchiveTreeLogItemRecord atlir)
        {
            ConnectForm cn = new ConnectForm(true);
            cn.OnConnectFormShown = this.DoShowArchiveLog;
            cn.lblCurrentAction.Text = "Загружаем лог...";
            archiveWorkRecord = atlir;
            cn.ShowDialog();
            
        }

        private void treeView_MouseDoubleClick(object sender, MouseEventArgs e)
        {
            TreeNode selNode = treeView.SelectedNode;
            if (selNode == null)
                return;

            if(selNode.Tag is ArchiveTreeEthalonItemRecord)
            {
                ShowArchiveEthalon(selNode.Tag as ArchiveTreeEthalonItemRecord);
                return;
            }

            if (selNode.Tag is ArchiveTreeLogItemRecord)
            {
                ShowArchiveLog(selNode.Tag as ArchiveTreeLogItemRecord);
                return;
            }
        }

        private bool inSetBordersToController = true;
        private void btnSetBorders_Click(object sender, EventArgs e)
        {
            inSetBordersToController = true;
            ShowWaitCursor(true);

            string s = "";
            s += Convert.ToString(nudLowBorder.Value) + PARAM_DELIMITER;
            s += Convert.ToString(nudHighBorder.Value);

            PushCommandToQueue(SET_PREFIX + "TBORDERS" + PARAM_DELIMITER + s, ParseSetBorders);
        }


        private bool inSetRelayDelayToController = true;
        private void btnSetRelayDelay_Click(object sender, EventArgs e)
        {
            inSetRelayDelayToController = true;
            ShowWaitCursor(true);

            string s = "";
            s += Convert.ToString(nudRelayDelay.Value);
            s += PARAM_DELIMITER + Convert.ToString(nudACSDelay.Value);

            PushCommandToQueue(SET_PREFIX + "RDELAY" + PARAM_DELIMITER + s, ParseSetRelayDelay);
        }

        private void archiveLogDataGrid_CellValueNeeded(object sender, DataGridViewCellValueEventArgs e)
        {
            DataGridView targetGrid = sender as DataGridView;

            int rowNumber = e.RowIndex;

            if (rowNumber >= targetGrid.RowCount)
                return;

            if (!gridToListCollection.ContainsKey(targetGrid))
                return;

            LogInfo linf = gridToListCollection[targetGrid];

            if (rowNumber >= linf.list.Count)
                return;

            InterruptRecord record = linf.list[rowNumber];

            DataGridViewRow row = targetGrid.Rows[rowNumber];
            row.Tag = record;

            row.DefaultCellStyle.BackColor = rowNumber % 2 == 0 ? Color.LightGray : Color.White;

            if(targetGrid.Columns[e.ColumnIndex].Name.StartsWith("Num"))
            {
                e.Value = (rowNumber + 1).ToString();
            } 
            else if (targetGrid.Columns[e.ColumnIndex].Name.StartsWith("Time"))
            {
                e.Value = record.InterruptInfo.InterruptTime.ToString("dd.MM.yyyy HH:mm:ss");
            }
            else if (targetGrid.Columns[e.ColumnIndex].Name.StartsWith("Temp"))
            {
                e.Value = record.InterruptInfo.SystemTemperature.ToString("0.00") + " °C";
            }
            else if (targetGrid.Columns[e.ColumnIndex].Name.StartsWith("Channel"))
            {
                e.Value = (1 + record.ChannelNumber).ToString();
            }
            else if (targetGrid.Columns[e.ColumnIndex].Name.StartsWith("Rod"))
            {
                e.Value = EnumHelpers.GetEnumDescription(record.RodPosition);

                if (record.RodPosition == RodPosition.Broken)
                    row.Cells[e.ColumnIndex].Style.BackColor = Color.LightSalmon;
                else
                    row.Cells[e.ColumnIndex].Style.BackColor = Color.White;
            }
            else if (targetGrid.Columns[e.ColumnIndex].Name.StartsWith("Compare"))
            {
                e.Value = EnumHelpers.GetEnumDescription(record.EthalonCompareResult);

                if (record.EthalonCompareResult == EthalonCompareResult.MatchEthalon)
                    row.Cells[e.ColumnIndex].Style.BackColor = Color.LightGreen;
                else
                    row.Cells[e.ColumnIndex].Style.BackColor = Color.LightSalmon;

            }
            else if (targetGrid.Columns[e.ColumnIndex].Name.StartsWith("Etl"))
            {
                e.Value = EnumHelpers.GetEnumDescription(record.EthalonCompareNumber);
            }
            else if (targetGrid.Columns[e.ColumnIndex].Name.StartsWith("Motoresource"))
            {
                if(linf.computeMotoresurcePercents)
                {
                    int resMax = 1;
                    switch (record.ChannelNumber)
                    {
                        //DEPRECATED: case 0:
                        default:
                            resMax = Config.Instance.MotoresourceMax1;
                            break;

                    }

                    if (resMax < 1)
                        resMax = 1;

                    float motoPercents = (record.Motoresource * 100.0f) / resMax;
                    e.Value = record.Motoresource.ToString() + " (" + motoPercents.ToString("0.00") + "%)";
                }
                else
                {
                    e.Value = record.Motoresource.ToString(); ;
                }
            }
            else if (targetGrid.Columns[e.ColumnIndex].Name.StartsWith("Pulses"))
            {
                e.Value = record.InterruptData.Count.ToString();
            }
            else if (targetGrid.Columns[e.ColumnIndex].Name.StartsWith("Btn"))
            {
                e.Value = "Просмотр";
            }


        }

        private void btnExportEthalonToImage_Click(object sender, EventArgs e)
        {            
            saveFileDialog.FileName = archiveEthalonChartExportFileName;

            if (saveFileDialog.ShowDialog() == DialogResult.OK)
            {
                bool errors = false;
                string fname = saveFileDialog.FileName;
                switch (saveFileDialog.FilterIndex)
                {
                    case 1: // png
                        {
                            try
                            {
                                this.archiveAthalonChart.SaveImage(fname, System.Windows.Forms.DataVisualization.Charting.ChartImageFormat.Png);
                            }
                            catch
                            {
                                errors = true;
                            }
                        }
                        break;

                    case 2: // jpeg
                        {
                            try
                            {
                                this.archiveAthalonChart.SaveImage(fname, System.Windows.Forms.DataVisualization.Charting.ChartImageFormat.Jpeg);
                            }
                            catch
                            {
                                errors = true;
                            }

                        }
                        break;
                } // switch

                if (errors)
                {
                    MessageBox.Show("Ошибка экспорта!", "Сообщение", MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
                else
                {
                    MessageBox.Show("Картинка экспортирована.", "Сообщение", MessageBoxButtons.OK, MessageBoxIcon.Information);
                }
            }
        }

        private void treeView_AfterSelect(object sender, TreeViewEventArgs e)
        {
            TreeNode selectedNode = e.Node;
            if (selectedNode == null || selectedNode.Tag == null)
            {
                ShowStartPanel();
                return;
            }

            if (selectedNode.Tag is TreeNodeType)
            {
                TreeNodeType tp = (TreeNodeType)selectedNode.Tag;
                switch (tp)
                {

                    case TreeNodeType.MainSettingsNode:
                        ShowMainSettings();
                        break;

                    //TODO: Тут другие панели!!!

                    case TreeNodeType.SDSettingsNode:
                        this.plSDSettings.BringToFront();
                        break;


                } // switch
            }
        }
        private bool inSetEthalonRecordToController = false;
        private void btnRecordEthalonUp_Click(object sender, EventArgs e)
        {
            recordEthalon("UP");
        }

        private void btnRecordEthalonDown_Click(object sender, EventArgs e)
        {
            recordEthalon("DOWN");
        }

        private void recordEthalon(string dir)
        {
            if(MessageBox.Show("При записи эталона вам необходимо привести штангу в движение.\n\nПри нажатии кнопки \"Отмена\" - запись эталона производиться не будет.\n\nПосле нажатия кнопки \"ОК\" этого диалогового окна - приведите штангу в движение, и дождитесь диалогового окна с сообщением о результатах записи!", "Сообщение", MessageBoxButtons.OKCancel, MessageBoxIcon.Information) != DialogResult.OK)
            {
                return;
            }

            ShowWaitCursor(true);
            inSetEthalonRecordToController = true;
            PushCommandToQueue(GET_PREFIX + "EREC" + PARAM_DELIMITER + dir, ParseRecordEthalon);
        }

        private void ParseRecordEthalon(Answer a)
        {
            inSetEthalonRecordToController = false;
            ShowWaitCursor(false);

            if (a.IsOkAnswer)
            {
                MessageBox.Show("Эталон успешно записан.", "Сообщение", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
            else
            {
                MessageBox.Show("Ошибка записи эталона!", "Ошибка", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        private void btnDisconnect_Click(object sender, EventArgs e)
        {
            currentTransport.Disconnect();
            this.treeView.Nodes[0].Nodes.Clear();
        }

        private void btnConnect_Click(object sender, EventArgs e)
        {
            DoConnect("", false, true);
        }

        private void ParseSDTest(Answer a)
        {
            sdTestForm.DialogResult = DialogResult.OK;
            sdTestForm = null;

            if(a.IsOkAnswer && a.ParamsCount > 3)
            {
                if (a.Params[1] == "SUCC")
                {
                    String mess = "Тестирование SD-карты успешно завершено.\n\n\tСкорость записи: ";
                    mess += a.Params[2];
                    mess += " Кб/с\n\tСкорость чтения: ";
                    mess += a.Params[3];
                    mess += " Кб/с";

                    MessageBox.Show(mess,"Сообщение", MessageBoxButtons.OK, MessageBoxIcon.Information);
                }
                else
                {
                    MessageBox.Show("Тест SD-карты неуспешен!", "Ошибка", MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
            }
            else
            {
                MessageBox.Show("Тест SD-карты неуспешен!", "Ошибка", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        ConnectForm sdTestForm = null;
        private void StartSDTest(ConnectForm frm)
        {
            PushCommandToQueue(GET_PREFIX + "SDTEST", ParseSDTest);

            //frm.DialogResult = DialogResult.OK;
        }

        private void btnSDTest_Click(object sender, EventArgs e)
        {
            DialogResult dr = MessageBox.Show("Начать тест SD-карты?", "Подтверждение", MessageBoxButtons.OKCancel, MessageBoxIcon.Question);
            if (dr != System.Windows.Forms.DialogResult.OK)
                return;


            sdTestForm = new ConnectForm(true);
            sdTestForm.OnConnectFormShown = this.StartSDTest;
            sdTestForm.lblCurrentAction.Text = "Тестирование SD-карты, подождите...";
            sdTestForm.ShowDialog();

        }

        private void ParseSetCurrentAsuTPFlags(Answer a)
        {
            inSetAsuTPFlagsInController = false;
            ShowWaitCursor(false);

            if (a.IsOkAnswer)
            {
                Config.Instance.AsuTpFlags = tempAsuTPFlags;

                MessageBox.Show("Настройки выдачи сигналов обновлёны.", "Сообщение", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
            else
            {
                MessageBox.Show("Ошибка обновления флагов выдачи сигналов!", "Ошибка", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }

            cbAsuTpLine1.Checked = false;
            cbAsuTpLine2.Checked = false;
            cbAsuTpLine3.Checked = false;
            cbAsuTpLine4.Checked = false;

            if ((Config.Instance.AsuTpFlags & 1) != 0)
            {
                cbAsuTpLine1.Checked = true;
            }
            if ((Config.Instance.AsuTpFlags & 2) != 0)
            {
                cbAsuTpLine2.Checked = true;
            }
            if ((Config.Instance.AsuTpFlags & 4) != 0)
            {
                cbAsuTpLine3.Checked = true;
            }
            if ((Config.Instance.AsuTpFlags & 8) != 0)
            {
                cbAsuTpLine4.Checked = true;
            }

        }

        private void ParseSetCurrentCoeff(Answer a)
        {
            inSetCurrentCoeffToController = false;
            ShowWaitCursor(false);

            if (a.IsOkAnswer)
            {
                Config.Instance.CurrentCoeff = Convert.ToInt32(nudCurrentCoeff.Value);

                MessageBox.Show("Коэффициент тока обновлён.", "Сообщение", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
            else
            {
                nudCurrentCoeff.Value = Config.Instance.CurrentCoeff;

                MessageBox.Show("Ошибка обновления коэффициента!", "Ошибка", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        private bool inSetCurrentCoeffToController = true;
        private void btnCurrentCoeff_Click(object sender, EventArgs e)
        {
            inSetCurrentCoeffToController = true;
            ShowWaitCursor(true);

            string s = "";
            s += Convert.ToString(nudCurrentCoeff.Value);

            PushCommandToQueue(SET_PREFIX + "CCOEFF" + PARAM_DELIMITER + s, ParseSetCurrentCoeff);
        }

        private bool inSetAsuTPFlagsInController = true;
        private uint tempAsuTPFlags = 0;
        private void btnSetAsuTpLags_Click(object sender, EventArgs e)
        {
            inSetAsuTPFlagsInController = true;
            ShowWaitCursor(true);

            UInt16 val = 0;
            if(cbAsuTpLine1.Checked)
            {
                val |= 1;
            }

            if(cbAsuTpLine2.Checked)
            {
                val |= 2;
            }

            if(cbAsuTpLine3.Checked)
            {
                val |= 4;
            }

            if(cbAsuTpLine4.Checked)
            {
                val |= 8;
            }

            tempAsuTPFlags = val;
            PushCommandToQueue(SET_PREFIX + "ASUTPFLAGS" + PARAM_DELIMITER + val.ToString(), ParseSetCurrentAsuTPFlags);
        }
    }

}
