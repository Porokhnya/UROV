﻿using System;
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

        private ToolStripMenuItem lastSelectedPort = null; // последний выбранный порт
        private ITransport currentTransport = null; // текущий транспорт
        private ConnectForm connForm = null; // форма соединения
        private const char PARAM_DELIMITER = '|'; // разделитель параметров
        private const string GET_PREFIX = "GET="; // префикс команды получения свойств
        private const string SET_PREFIX = "SET="; // префикс команды установки свойств

        public MainForm()
        {
            InitializeComponent();
        }

        private void exitToolStripMenuItem_Click(object sender, EventArgs e)
        {
            // по клику на меню "Выход" - завершаем приложение
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
        /// <summary>
        /// Пробуем коннектиться к порту
        /// </summary>
        /// <param name="portname">имя порта</param>
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
        /// <summary>
        /// Событие отсоединения транспорта от связи
        /// </summary>
        /// <param name="transport">транспорт, с которым разорвана связь</param>
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
            

            int speed = GetConnectionSpeed(); // получаем скорость соединения

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
        /// Обрабатываем данные, пришедшие из транспорта COM-порта
        /// </summary>
        /// <param name="data">массив данных</param>
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


        /// <summary>
        /// Буфер под ответ с SD-карты
        /// </summary>
        private List<byte> SDQueryAnswer = new List<byte>();


        /// <summary>
        /// Обрабатываем строку ответа контроллера
        /// </summary>
        /// <param name="line">строка ответа</param>
        private void ProcessAnswerLine(string line)
        {
            System.Diagnostics.Debug.WriteLine("<= COM: " + line);

            if (line.StartsWith("UROV v.")) // нашли загрузку ядра, следовательно, можно писать данные
            {
                tbFirmwareVersion.Text = line;
                coreBootFound = true;
            }

            bool isKnownAnswer = line.StartsWith("OK=") || line.StartsWith("ER="); // проверяем на известные типы ответов

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


        /// <summary>
        /// Данные для показа лог-файла
        /// </summary>
        private List<byte> logContentToShow = null;

        /// <summary>
        /// Показывает лог-файл
        /// </summary>
        /// <param name="frm">форма загрузки</param>
        private void DoShowLogFile(ConnectForm frm)
        {
            System.Diagnostics.Debug.Assert(logContentToShow != null);
            ShowLogFile(logContentToShow, this.logDataGrid, "", true,frm,null,false);

            frm.DialogResult = DialogResult.OK;
        }

        /// <summary>
        /// Просмотр файла
        /// </summary>
        /// <param name="content">содержимое файла</param>
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

            if (upStr.EndsWith(".LOG")) // это лог-файл
            {
                ConnectForm cn = new ConnectForm(true);
                cn.OnConnectFormShown = this.DoShowLogFile;
                cn.lblCurrentAction.Text = "Загружаем лог...";
                logContentToShow = content;
                cn.ShowDialog();                

            }
            else if (upStr.EndsWith(".ETL")) // это файл эталона
            {
                CreateEthalonChart(content, this.ethalonChart, Config.Instance.RodMoveLength);
                this.plEthalonChart.BringToFront();
            }
        }

        /// <summary>
        /// Сохраняет файл эталона
        /// </summary>
        /// <param name="content">содержимое файла</param>
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

        /// <summary>
        /// Очищает все эталоны
        /// </summary>
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
        /// Информация с лог-файла
        /// </summary>
        public class LogInfo
        {
            /// <summary>
            /// Список записей в лог-файле
            /// </summary>
            public List<InterruptRecord> list = new List<InterruptRecord>();
            public string addToColumnName;
            public bool computeMotoresurcePercents;
        }

        /// <summary>
        /// привязка DataGrid к лог-файлу
        /// </summary>
        private Dictionary<DataGridView, LogInfo> gridToListCollection = new Dictionary<DataGridView, LogInfo>();

        

        /// <summary>
        /// Показывает лог-файл
        /// </summary>
        /// <param name="content"></param>
        /// <param name="targetGrid"></param>
        /// <param name="addToColumnName"></param>
        /// <param name="computeMotoresurcePercents"></param>
        /// <param name="frm"></param>
        /// <param name="callback"></param>
        /// <param name="stopAfterFirstRecord"></param>
        private void ShowLogFile(List<byte> content, DataGridView targetGrid, string addToColumnName, bool computeMotoresurcePercents, ConnectForm frm, ShowInterruptInfo callback, bool stopAfterFirstRecord)
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
                    this.plEmptySDWorkspace.BringToFront();
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
        /// Сохраняет эталон
        /// </summary>
        /// <param name="fname"></param>
        /// <param name="content"></param>
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
            while(maxTimeDivideLabels < neareastValue)
            {
                neareastValue /= 10;
            }

            int step = Convert.ToInt32(Math.Round(Convert.ToDouble(maxTimeDivideLabels) / neareastValue, 0)) * neareastValue;
            if(step < 1)
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

            if(timeList.Count > 1)
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

                int pulseTimePercents = (pulseTime*100) / maxPulseTime;
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

                case AnswerBehaviour.SDCommandFILE: // загрузка файла
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

                case AnswerBehaviour.SDCommandLS: // получение списка файлов
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

        /// <summary>
        /// Добавляет запись в дерево файлов на SD
        /// </summary>
        /// <param name="line"></param>
        /// <param name="parent"></param>
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
                PushCommandToQueue(GET_PREFIX + "RLENGTH", ParseAskRodMoveLength, BeforeAskRodMoveLength);
                PushCommandToQueue(GET_PREFIX + "MBUSID", ParseAskMBUSID, BeforeAskMBUSID);

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

        /// <summary>
        /// Устанавливает надпись в строке статуса
        /// </summary>
        private void setConnectionStatusMessage()
        {
            string cguid = Config.Instance.ControllerGUID;
            string savedName = Config.Instance.ControllerGUID;
            if (ControllerNames.Instance.Names.ContainsKey(cguid))
            {
                savedName = ControllerNames.Instance.Names[cguid];
            }

            if(savedName.Length < 1)
            {
                savedName = "<без имени>";
            }

            if (IsConnected())
            {
                connectStatusMessage.Text = "Соединено, контроллер \"" + savedName + "\"";
            }
            else
            {
                connectStatusMessage.Text = "";
            }
        }

        /// <summary>
        /// Разбор результатов команды получения уникального ID контроллера
        /// </summary>
        /// <param name="a"></param>
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

        /// <summary>
        /// Создание уникального ID контроллера
        /// </summary>
        /// <returns></returns>
        private string GenerateUUID()
        {
            return Guid.NewGuid().ToString("N");
        }

        /// <summary>
        /// Запрос эталонов с контроллера
        /// </summary>
        private void RequestEthalons()
        {
            PushCommandToQueue(GET_PREFIX + "FILE|ETL|ET0UP.ETL", DummyAnswerReceiver, SetSDFileReadingFlagEthalon);
            PushCommandToQueue(GET_PREFIX + "FILE|ETL|ET0DWN.ETL", DummyAnswerReceiver, SetSDFileReadingFlagEthalon);

            PushCommandToQueue(GET_PREFIX + "FILE|ETL|ET1UP.ETL", DummyAnswerReceiver, SetSDFileReadingFlagEthalon);
            PushCommandToQueue(GET_PREFIX + "FILE|ETL|ET1DWN.ETL", DummyAnswerReceiver, SetSDFileReadingFlagEthalon);

            PushCommandToQueue(GET_PREFIX + "FILE|ETL|ET2UP.ETL", DummyAnswerReceiver, SetSDFileReadingFlagEthalon);
            PushCommandToQueue(GET_PREFIX + "FILE|ETL|ET2DWN.ETL", DummyAnswerReceiver, SetSDFileReadingFlagEthalon);

        }

        /// <summary>
        /// Обработчик перед запросом величины перемещения штанги
        /// </summary>
        private void BeforeAskRodMoveLength()
        {
            this.inSetRodSettingsToController = true;
        }


        /// <summary>
        /// Обработчик перед запросом ID слейва modbus
        /// </summary>
        private void BeforeAskMBUSID()
        {
            this.inSetMBUSIDToController = true;
        }

        /// <summary>
        /// Обработчик перед запросом флагов АСУ ТП
        /// </summary>
        private void BeforeAskAsuTpFlags()
        {
            this.inSetAsuTPFlagsInController = true;
        }

        /// <summary>
        /// Обработчик перед запросом дельты импульсов
        /// </summary>
        private void BeforeAskDelta()
        {
            this.inSetDeltaToController = true;
        }

        /// <summary>
        /// Обработчик перед запросом кол-ва импульсов
        /// </summary>
        private void BeforeAskPulses()
        {
            this.inSetPulsesToController = true;
        }

        /// <summary>
        /// Обработчик перед запросом коэффициента по току
        /// </summary>
        private void BeforeAskCurrentCoeff()
        {
            this.inSetCurrentCoeffToController = true;
        }

        /// <summary>
        /// Обработчик перед запросом порогов трансформатора
        /// </summary>
        private void BeforeAskBorders()
        {
            this.inSetBordersToController = true;
        }

        /// <summary>
        /// Обработчик перед запросом задержки реле
        /// </summary>
        private void BeforeAskRelayDelay()
        {
            this.inSetRelayDelayToController = true;
        }

        /// <summary>
        /// Обработчик перед запросом моторесурса
        /// </summary>
        private void BeforeAskMotoresource()
        {
            this.inSetMotoresourceToController = true;
        }


        /// <summary>
        /// Очищает все данные, связанные с подсоединённым контроллером
        /// </summary>
        private void ClearAllData()
        {
            this.treeView.Nodes[0].Nodes.Clear();
            this.treeViewSD.Nodes.Clear();
            this.ClearInterruptsList(this.logDataGrid);
            dateTimeFromControllerReceived = false;
            tbFirmwareVersion.Text = "";
            tbFREERAM.Text = "";
        }

        /// <summary>
        /// Разбор ответа с дельтой импульсов
        /// </summary>
        /// <param name="a"></param>
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

        /// <summary>
        /// разбор ответа с коэффициентом по току
        /// </summary>
        /// <param name="a"></param>
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

        /// <summary>
        /// разбор ответа с количеством импульсов
        /// </summary>
        /// <param name="a"></param>
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

        /// <summary>
        /// разбор ответа с порогами трансформатора
        /// </summary>
        /// <param name="a"></param>
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

        /// <summary>
        /// разбор ответа с задержкой реле
        /// </summary>
        /// <param name="a"></param>
        private void ParseAskRelayDelay(Answer a)
        {
            this.inSetRelayDelayToController = false;
            if (a.IsOkAnswer)
            {
                try { Config.Instance.RelayDelay = Convert.ToInt32(a.Params[1]); } catch { Config.Instance.RelayDelay = 0; }
                try { Config.Instance.ACSDelay = Convert.ToInt32(a.Params[2]); } catch { Config.Instance.ACSDelay = 0; }
                try { Config.Instance.MaxIdleTime = Convert.ToInt32(a.Params[3]); } catch { Config.Instance.MaxIdleTime = 0; }

            }
            else
            {
                Config.Instance.RelayDelay = 0;
                Config.Instance.ACSDelay = 0;
                Config.Instance.MaxIdleTime = 0;
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

            try
            {
                nudMaxIdleTime.Value = Config.Instance.MaxIdleTime;
            }
            catch
            {
                nudMaxIdleTime.Value = 0;
                Config.Instance.MaxIdleTime = 0;
            }
        }

        /// <summary>
        /// разбор ответа с флагами АСУ ТП
        /// </summary>
        /// <param name="a"></param>
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

        /// <summary>
        /// разбор ответа с дельтой времени при сравнении с эталоном
        /// </summary>
        /// <param name="a"></param>
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

        /// <summary>
        /// разбор ответа пропуска импульсов
        /// </summary>
        /// <param name="a"></param>
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

        /// <summary>
        /// разбор ответа с величиной перемещения привода
        /// </summary>
        /// <param name="a"></param>
        private void ParseAskRodMoveLength(Answer a)
        {
            this.inSetRodSettingsToController = false;

            if (a.IsOkAnswer)
            {
                try { Config.Instance.RodMoveLength = Convert.ToInt32(a.Params[1]); } catch { Config.Instance.RodMoveLength = 1; }

            }
            else
            {
                Config.Instance.RodMoveLength = 1;
            }

            try
            {
                nudRodMoveLength.Value = Config.Instance.RodMoveLength;
            }
            catch
            {
                nudRodMoveLength.Value = 1;
                Config.Instance.RodMoveLength = 1;
            }

        }

        /// <summary>
        /// разбор ответа с ID слейва модбас
        /// </summary>
        /// <param name="a"></param>
        private void ParseAskMBUSID(Answer a)
        {
            this.inSetMBUSIDToController = false;

            if (a.IsOkAnswer)
            {
                try { Config.Instance.ModbusSlaveID = Convert.ToInt32(a.Params[1]); } catch { Config.Instance.ModbusSlaveID = 1; }

            }
            else
            {
                Config.Instance.ModbusSlaveID = 1;
            }

            try
            {
                nudModbusSlaveID.Value = Config.Instance.ModbusSlaveID;
            }
            catch
            {
                nudModbusSlaveID.Value = 1;
                Config.Instance.ModbusSlaveID = 1;
            }
        }

        /// <summary>
        /// разбор ответа с текущим моторесурсом
        /// </summary>
        /// <param name="a"></param>
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

        /// <summary>
        /// обновляет моторесурс, выводимый на экране
        /// </summary>
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

        /// <summary>
        /// разбор ответа с максимальным моторесурсом
        /// </summary>
        /// <param name="a"></param>
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

        /// <summary>
        /// разбор ответа с временем контроллера
        /// </summary>
        /// <param name="a"></param>
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

        /// <summary>
        /// разбор ответа с величиной свободной памяти контроллера
        /// </summary>
        /// <param name="a"></param>
        private void ParseAskFreeram(Answer a)
        {
            if (a.IsOkAnswer)
            {
                //пришли данные о свободной памяти
                tbFREERAM.Text = a.Params[1] + " байт";
            }
            else
            {

            }
        }



        private FeaturesSettings featuresSettings = new FeaturesSettings();

        /// <summary>
        /// пересоздание дерева настроек
        /// </summary>
        private void RecreateTreeView()
        {

            this.treeView.Nodes[0].Nodes.Clear();


            TreeNode node = this.treeView.Nodes[0].Nodes.Add("Основные настройки");
            node.Tag = TreeNodeType.MainSettingsNode;
            node.ImageIndex = 5;
            node.SelectedImageIndex = node.ImageIndex;

            node = this.treeView.Nodes[0].Nodes.Add("Дополнительные настройки");
            node.Tag = TreeNodeType.AdditionalSettingsNode;
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


        /// <summary>
        /// представление байта в виде строки в шестнадцатеричном формате
        /// </summary>
        /// <param name="b"></param>
        /// <returns></returns>
        private string ByteAsHexString(byte b)
        {
            return "0x" + b.ToString("X2");
        }

 
        /// <summary>
        /// перераспределение ширины столбцов в таблице логов
        /// </summary>
        private void ResizeLogColumns()
        {
            this.logColumn1.Width = this.lvLog.ClientRectangle.Width / 2 - SystemInformation.VerticalScrollBarWidth / 2 - 2;
            this.logColumn2.Width = this.logColumn1.Width;
        }

        /// <summary>
        /// инициализация архивных записей
        /// </summary>
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

        /// <summary>
        /// загрузка главной формы приложения
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
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
            plAdditionalSettings.Dock = DockStyle.Fill;
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
            tbSoftwareVersion.Text = "v." + System.Reflection.Assembly.GetExecutingAssembly().GetName().Version.ToString() + " от " +
                Properties.Resources.BuildDate;
            
        }

        /// <summary>
        /// инициализация дерева настроек
        /// </summary>
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
            this.btnSetRodSettings.Enabled = bConnected && !inSetRodSettingsToController;
            this.btnSetModbusSlaveID.Enabled = bConnected && !inSetMBUSIDToController;

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


        /// <summary>
        /// клик на пункте меню с выбранным для соединения портом
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
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

        /// <summary>
        /// возвращает выбранную скорость соединения с портом
        /// </summary>
        /// <returns></returns>
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

        /// <summary>
        /// обработчик перед чтением последнего срабатывания
        /// </summary>
        private void SetLastTrigReadingFlag()
        {
            this.answerBehaviour = AnswerBehaviour.SDCommandFILE;
            this.fileDataParseFunction = this.ViewLastTrigData;
            this.SDQueryAnswer.Clear();
        }

        /// <summary>
        /// обратный вызов при чтении последнего срабатывания
        /// </summary>
        /// <param name="rec"></param>
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

        /// <summary>
        /// просмотр данных последнего срабатывания
        /// </summary>
        /// <param name="content"></param>
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

        /// <summary>
        /// таймер обработки команд
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void tmProcessCommandsTimer_Tick(object sender, EventArgs e)
        {
            ProcessNextCommand();
        }

        private AnswerBehaviour answerBehaviour = AnswerBehaviour.Normal;

        /// <summary>
        /// обработка следующей команды
        /// </summary>
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

        /// <summary>
        /// таймер получения информации по свободной памяти контроллера
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void tmGetSensorsData_Tick(object sender, EventArgs e)
        {
            if (!GrantToProcess())
                return;

            PushCommandToQueue(GET_PREFIX + "FREERAM", ParseAskFreeram);

        }

        /// <summary>
        /// показывает стартовую панель
        /// </summary>
        private void ShowStartPanel()
        {
            this.plStartPanel.Dock = DockStyle.Fill;
            this.plStartPanel.BringToFront();
        }

        /// <summary>
        /// показывает основные настройки
        /// </summary>
        private void ShowMainSettings()
        {
            this.plMainSettings.BringToFront();
            treeView.SelectedNode = treeView.Nodes[0].Nodes[0];
        }

        /// <summary>
        /// показывает дополнительные настройки
        /// </summary>
        private void ShowAdditionalSettings()
        {
            this.plAdditionalSettings.BringToFront();
            treeView.SelectedNode = treeView.Nodes[0].Nodes[1];

        }

        private void propertyGridSettings_PropertyValueChanged(object s, PropertyValueChangedEventArgs e)
        {

        }

        /// <summary>
        /// таймер времени контроллера
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
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

        /// <summary>
        /// установка времени контроллера
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
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

        /// <summary>
        /// обработчик результатов установки дельты времени при сравнении с эталоном
        /// </summary>
        /// <param name="a"></param>
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

        /// <summary>
        /// обработчик результатов установки величины перемещения привода
        /// </summary>
        /// <param name="a"></param>
        private void ParseSetRodMoveLength(Answer a)
        {
            inSetRodSettingsToController = false;
            ShowWaitCursor(false);

            if (a.IsOkAnswer)
            {
                Config.Instance.RodMoveLength = Convert.ToInt32(nudRodMoveLength.Value);

                MessageBox.Show("Параметры обновлёны.", "Сообщение", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
            else
            {
                nudRodMoveLength.Value = Config.Instance.RodMoveLength;

                MessageBox.Show("Ошибка обновления параметров!", "Ошибка", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }

        }

        /// <summary>
        /// обработчик результатов установки ID слейва modbus
        /// </summary>
        /// <param name="a"></param>
        private void ParseSetMBUSID(Answer a)
        {
            inSetMBUSIDToController = false;
            ShowWaitCursor(false);

            if (a.IsOkAnswer)
            {
                Config.Instance.ModbusSlaveID = Convert.ToInt32(nudModbusSlaveID.Value);

                MessageBox.Show("Параметры обновлёны.", "Сообщение", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
            else
            {
                nudModbusSlaveID.Value = Config.Instance.ModbusSlaveID;

                MessageBox.Show("Ошибка обновления параметров!", "Ошибка", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }

        }

        /// <summary>
        /// обработчик результатов установки пропуска импульсов
        /// </summary>
        /// <param name="a"></param>
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

        /// <summary>
        /// обработчик результатов установки дельты импульсов
        /// </summary>
        /// <param name="a"></param>
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

        /// <summary>
        /// обработчик результатов установки кол-ва импульсов
        /// </summary>
        /// <param name="a"></param>
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

        /// <summary>
        /// обработчик результатов установки порогов трансформатора
        /// </summary>
        /// <param name="a"></param>
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

        /// <summary>
        /// обработчик результатов установки задержки реле
        /// </summary>
        /// <param name="a"></param>
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

        /// <summary>
        /// обработчик результатов установки текущего моторесурса
        /// </summary>
        /// <param name="a"></param>
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

        /// <summary>
        /// обработчик результатов установки максимального моторесурса
        /// </summary>
        /// <param name="a"></param>
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

        /// <summary>
        /// обработчик результатов установки времени
        /// </summary>
        /// <param name="a"></param>
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

        /// <summary>
        /// изменение размеров формы
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void MainForm_Resize(object sender, EventArgs e)
        {
            ResizeLogColumns();
        }

        private void treeView_BeforeCollapse(object sender, TreeViewCancelEventArgs e)
        {
            //e.Cancel = true;
        }


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
        /// показ окна "О программе"
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void btnAbout_Click(object sender, EventArgs e)
        {
            AboutForm ab = new AboutForm();
            ab.ShowDialog();
        }

        private TreeNode currentSDParentNode = null;

        /// <summary>
        /// сканирование SD
        /// </summary>
        private void RescanSD()
        {
            btnListSDFiles.Enabled = false;
            PushCommandToQueue(GET_PREFIX + "LS", DummyAnswerReceiver, SetSDReadingFlag);

        }

        /// <summary>
        /// получение списка файлов SD
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void btnListSDFiles_Click(object sender, EventArgs e)
        {
            RescanSD();
        }

        /// <summary>
        /// пустой обработчик ответа
        /// </summary>
        /// <param name="a"></param>
        public void DummyAnswerReceiver(Answer a)
        {

        }

        private LSDoneFunction lsParseFunction = null;
        private LSRecordFunction lsRecordFunction = null;
        private FileDownloadProgressFunction fileDownloadProgressFunction = null;
        private FileDataParseFunction fileDataParseFunction = null;

        /// <summary>
        /// разбор одной записи файловой системы на SD
        /// </summary>
        /// <param name="line"></param>
        private void ParseSDLSRecord(string line)
        {
            AddToLog(line, false);
            AddRecordToSDList(line, currentSDParentNode);

        }

        /// <summary>
        /// окончание разбора списка файлов на SD
        /// </summary>
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

        /// <summary>
        /// начало получения списка файлов на SD
        /// </summary>
        private void SetSDReadingFlag()
        {
            lsParseFunction = ParseSDLSDone;
            lsRecordFunction = ParseSDLSRecord;
            this.answerBehaviour = AnswerBehaviour.SDCommandLS;
            this.currentSDParentNode = null;
            this.SDQueryAnswer.Clear();
            this.treeViewSD.Nodes.Clear();
        }

        /// <summary>
        /// начало получения списка файлов в папке
        /// </summary>
        private void SetSDFolderReadingFlag()
        {
            lsParseFunction = ParseSDLSDone;
            lsRecordFunction = ParseSDLSRecord;
            this.answerBehaviour = AnswerBehaviour.SDCommandLS;
            this.currentSDParentNode = tempSDParentNode;
            this.SDQueryAnswer.Clear();

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
        /// получение имени файла из его расшифровки
        /// </summary>
        /// <param name="text"></param>
        /// <returns></returns>
        string getFileNameFromText(string text)
        {
            if(fileNamesSubstitutions.ContainsValue(text))
            {
                string key = fileNamesSubstitutions.FirstOrDefault(x => x.Value == text).Key;
                return key;
            }

            return text;
        }

        /// <summary>
        /// получение расшифровки из имени файла
        /// </summary>
        /// <param name="fName"></param>
        /// <returns></returns>
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

        /// <summary>
        /// обработчик ракрытия узла дерева на SD
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
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

        /// <summary>
        /// начало чтения файла на SD
        /// </summary>
        private void SetSDFileReadingFlag()
        {
            this.answerBehaviour = AnswerBehaviour.SDCommandFILE;
            this.fileDataParseFunction = this.ViewFile;
            //this.fileDownloadFlags = FileDownloadFlags.View;
            this.SDQueryAnswer.Clear();
            ShowWaitCursor(true);
        }

        private int requestEthalonCounter = 0;

        /// <summary>
        /// начало чтения эталона на SD
        /// </summary>
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

        /// <summary>
        /// показ прогресса загрузки файла
        /// </summary>
        /// <param name="percents"></param>
        /// <param name="bytesReceived"></param>
        private void ShowDownloadPercents(int percents, int bytesReceived)
        {
            this.statusProgressBar.Value = percents;
        }

        /// <summary>
        /// запрос файла
        /// </summary>
        /// <param name="node"></param>
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

        /// <summary>
        /// разбор результата получения размера файла
        /// </summary>
        /// <param name="a"></param>
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

        /// <summary>
        /// начало архивирования
        /// </summary>
        /// <param name="fm"></param>
        public void StartArchive(ArchiveImportForm fm)
        {
            archiveImportForm = fm;
            archiveImportForm.DisableControls();

            DoArchive();

        }

        /// <summary>
        /// начало получения логов
        /// </summary>
        private void StartListLogs()
        {
            lsParseFunction = ParseLogsDone;
            lsRecordFunction = ParseLogsRecord;
            this.answerBehaviour = AnswerBehaviour.SDCommandLS;
            this.SDQueryAnswer.Clear();
        }

        /// <summary>
        /// окончание получения логов
        /// </summary>
        private void ParseLogsDone()
        {
            archiveListDoneCount++;

            if (archiveListDoneCount >= archiveWaitForListDone)
            {
                // получили список файлов, получаем их размеры
                ArchiveRequestFileSizes();
            }
        }

        /// <summary>
        /// разбор одной записи лога
        /// </summary>
        /// <param name="rec"></param>
        private void ParseLogsRecord(string rec)
        {
            archiveLogsList.Add(rec);
        }

        /// <summary>
        /// начало получения эталонов
        /// </summary>
        private void StartListEthalons()
        {
            lsParseFunction = ParseEthalonsDone;
            lsRecordFunction = ParseEthalonsRecord;
            this.answerBehaviour = AnswerBehaviour.SDCommandLS;
            this.SDQueryAnswer.Clear();
        }

        /// <summary>
        /// окончание получения эталонов
        /// </summary>
        private void ParseEthalonsDone()
        {
            archiveListDoneCount++;

            if (archiveListDoneCount >= archiveWaitForListDone)
            {
                // получили список файлов, получаем их размеры
                ArchiveRequestFileSizes();
            }
        }

        /// <summary>
        /// разбор одной записи эталона
        /// </summary>
        /// <param name="rec"></param>
        private void ParseEthalonsRecord(string rec)
        {
            archiveEthalonsList.Add(rec);
        }

        /// <summary>
        /// получение размера файлов для архивирования
        /// </summary>
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

        /// <summary>
        /// разбор размера файла эталона для архивирования
        /// </summary>
        /// <param name="a"></param>
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

        /// <summary>
        /// разбор размера файла лога для архивирования
        /// </summary>
        /// <param name="a"></param>
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

        /// <summary>
        /// скачивание файлов для архивирования
        /// </summary>
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

        /// <summary>
        /// начало скачивания эталона
        /// </summary>
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

        /// <summary>
        /// начало скачивания лог-файла
        /// </summary>
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

        /// <summary>
        /// импорт настроек при архивировании
        /// </summary>
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

        /// <summary>
        /// добавление записи в архив
        /// </summary>
        /// <param name="guid"></param>
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

        /// <summary>
        /// заполнение узла дерева архива информацией с диска
        /// </summary>
        /// <param name="n"></param>
        /// <param name="eti"></param>
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

        /// <summary>
        /// заполнение узла дерева логов архива информацией с диска
        /// </summary>
        /// <param name="n"></param>
        /// <param name="eti"></param>
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

        /// <summary>
        /// сохранение файла в архив
        /// </summary>
        /// <param name="fileName"></param>
        /// <param name="dirName"></param>
        /// <param name="content"></param>
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

        /// <summary>
        /// обработчик получения файла эталона при архивировании
        /// </summary>
        /// <param name="content"></param>
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

        /// <summary>
        /// обработчик получения файла лога при архивировании
        /// </summary>
        /// <param name="content"></param>
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

        /// <summary>
        /// обработчик прогресса скачивания при архивировании
        /// </summary>
        /// <param name="percents"></param>
        /// <param name="bytesReceived"></param>
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

        /// <summary>
        /// начало архивирования
        /// </summary>
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

        /// <summary>
        /// таймер перечисления портов в системе
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
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

        /// <summary>
        /// смена выбранной скорости соединения
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
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

        /// <summary>
        /// установка текущего моторесурса
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
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

        /// <summary>
        /// установка кол-ва импульсов
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
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

        /// <summary>
        /// установка дельты импульсов
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
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

        /// <summary>
        /// получения напряжения на входах
        /// </summary>
        private void GetVoltage()
        {
            PushCommandToQueue(GET_PREFIX + "VDATA", ParseVoltage);

        }

        /// <summary>
        /// таймер получения напряжения на входах
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void tmInductiveTimer_Tick(object sender, EventArgs e)
        {
            //DEPRECATED: GetInductiveSensors();
            GetVoltage();
        }

        /// <summary>
        /// сброс отображения напряжения на входах на значения по умолчанию
        /// </summary>
        private void ResetVoltage()
        {
            lblVoltage1.BackColor = Color.LightGray;
            lblVoltage1.Text = "-";
            /*

            lblVoltage2.BackColor = Color.LightGray;
            lblVoltage2.Text = "-";

            lblVoltage3.BackColor = Color.LightGray;
            lblVoltage3.Text = "-";
            */
        }

        /// <summary>
        /// разбор полученного напряжения на входах
        /// </summary>
        /// <param name="a"></param>
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

                /*
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
                */

            }
            else
            {
                ResetVoltage();
            }
        }

        /// <summary>
        /// обработчик смены значения текущего моторесурса
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
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

        /// <summary>
        /// обработчик выбора узла дерева SD
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
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

        /// <summary>
        /// удаление файла с SD
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
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

        /// <summary>
        /// просмотр файла на SD
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void btnViewSDFile_Click(object sender, EventArgs e)
        {
            RequestFile(treeViewSD.SelectedNode);
        }

        /// <summary>
        /// запрос файла с SD
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void treeViewSD_MouseDoubleClick(object sender, MouseEventArgs e)
        {
            RequestFile(treeViewSD.SelectedNode);
        }

        const int customLabelsCount = 15; // сколько всего наших меток будет на оси X

        /// <summary>
        /// отображение графика прерывания
        /// </summary>
        /// <param name="record"></param>
        /// <param name="stationID"></param>
        /// <param name="stationName"></param>
        /// <param name="modal"></param>
        private void ShowChart(InterruptRecord record, string stationID, string stationName, bool modal)
        {
            //  System.Diagnostics.Debug.Assert(record != null);
            if (record == null)
                return;

            ViewChartForm vcf = new ViewChartForm(record, stationID, stationName);

            vcf.setDefaultFileName(record.InterruptInfo.InterruptTime.ToString("yyyy-MM-dd HH.mm"));

            vcf.lblCaption.Text = "Срабатывание от " + record.InterruptInfo.InterruptTime.ToString("dd.MM.yyyy HH:mm:ss");

            // добавляем кол-во импульсов на график
            vcf.chart.Legends[1].CustomItems[0].Cells["PulsesCount"].Text = String.Format("Импульсов: {0}", record.InterruptData.Count * Config.Instance.SkipCounter);
            vcf.chart.Legends[1].CustomItems[0].Cells["EthalonPulses"].Text = String.Format("Эталон: {0}", record.EthalonData.Count);
            vcf.chart.Legends[1].CustomItems[0].Cells["TrigDate"].Text = "Дата: " + record.InterruptInfo.InterruptTime.ToString("dd.MM.yyyy HH:mm:ss");
            vcf.chart.Legends[1].CustomItems[0].Cells["Place"].Text = stationName;
            vcf.chart.Legends[1].CustomItems[0].Cells["PreviewCount"].Text = String.Format("Превью тока: {0} записей", record.PreviewCount);
            vcf.chart.Legends[1].CustomItems[0].Cells["tFact"].Text = String.Format("t факт: {0} ms", record.MoveTime/1000);


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

            if(timeList.Count > 1)
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

            if(record.CurrentTimes.Count > 0)
            {
                for(int z=0;z<record.CurrentTimes.Count;z++)
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
            int offsetLabelIndex = XValuesInterrupt.Count-1;

            int maxInterruptYVal = 0;


            // теперь считаем все остальные точки

            if (timeList.Count > 1)
            {

                for (int i = 1; i < timeList.Count; i++)
                {
                    int pulseTime = timeList[i] - timeList[i - 1];
                    

                    int pulseTimePercents = (pulseTime*100) / maxPulseTime;

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
            
            if(YValuesInterrupt.Count > 1)
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
            if(offsetLabelIndex != -1 && canAddInterruptLabels)
            {
                interruptSerie.Points[offsetLabelIndex].Label = String.Format("НАЧАЛО ПРЕРЫВАНИЯ, {0} ms", pulsesOffset/1000);
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


            if(computedInterruptLength != -1)
            {
                vcf.chart.Legends[1].CustomItems[0].Cells["tComputed"].Text = String.Format("t расчёт: {0} ms", computedInterruptLength/1000);

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

            int rodMoveLength = record.RodMoveLength > 0 ? record.RodMoveLength : Config.Instance.RodMoveLength; // величина перемещения штанги, мм
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

            if(record.InterruptData.Count > 0)
            {
                // тут раскрашиваем график направлениями движения
                RodPosition lastPos = RodPosition.Broken;

                System.Diagnostics.Debug.Assert(record.InterruptData.Count == record.Directions.Count);

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
                            interruptSerie.Points[curIdx-1].MarkerColor = Color.Red;
                            interruptSerie.Points[curIdx-1].MarkerStyle = MarkerStyle.Circle;
                            interruptSerie.Points[curIdx-1].MarkerSize = 6;
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
                string labelText = String.Format("{0:0.000}A", GetCurrentFromADC(currentStep*counter));
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
            float currentCoeff = Convert.ToSingle(Config.Instance.CurrentCoeff)/1000;

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

        /// <summary>
        /// клик на ячейке таблицы
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
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

        /// <summary>
        /// отсыл эталона в контроллер
        /// </summary>
        private void SendEthalonData()
        {
            this.currentTransport.Write(dataToSend, dataToSend.Length);
            inUploadFileToController = false;
        }

        /// <summary>
        /// заполнение эталона данными
        /// </summary>
        /// <param name="etl"></param>
        /// <param name="data"></param>
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

        /// <summary>
        /// загрузка эталона в контроллер
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
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

        /// <summary>
        /// окончание загрузки эталона в контроллер
        /// </summary>
        /// <param name="a"></param>
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

        /// <summary>
        /// закрытие главной формы
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void MainForm_FormClosed(object sender, FormClosedEventArgs e)
        {
            ControllerNames.Instance.Save();
            Config.Instance.Save();
        }

        /// <summary>
        /// установка имени контроллера
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
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

        /// <summary>
        /// импорт настроек контроллера в архив
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void btnImportSettings_Click(object sender, EventArgs e)
        {
            ArchiveImportForm af = new ArchiveImportForm(this);
            af.ShowDialog();
        }

        private string archiveEthalonChartExportFileName;

        /// <summary>
        /// просмотр эталона в архиве
        /// </summary>
        /// <param name="atei"></param>
        private void ShowArchiveEthalon(ArchiveTreeEthalonItemRecord atei)
        {
            string fname = atei.FileName;
            DateTime modification = System.IO.File.GetLastWriteTime(fname);
            archiveEthalonChartExportFileName = modification.ToString("yyyy-MM-dd HH.mm");

            string settingsFileName = Application.StartupPath + "\\Archive\\" + atei.Parent.Parent.GUID + "\\Settings\\settings.xml";
            ArchiveSettings aSett = ArchiveSettings.Load(settingsFileName);
            int rodMoveLength = Config.Instance.RodMoveLength;
            if(aSett != null)
            {
                rodMoveLength = aSett.RodMoveLength;
            }

            try
            {
                List<byte> content = new List<byte>(System.IO.File.ReadAllBytes(fname));
                CreateEthalonChart(content, this.archiveAthalonChart, rodMoveLength);
                this.plArchiveEthalonChart.BringToFront();
            }
            catch { }
            
        }

        /// <summary>
        /// чтение лог-файла архива
        /// </summary>
        /// <param name="frm"></param>
        /// <param name="fname"></param>
        private async void DoReadArchiveLog(ConnectForm frm, string fname)
        {

            byte[] result;
            using (System.IO.FileStream SourceStream = System.IO.File.Open(fname, System.IO.FileMode.Open))
            {
                result = new byte[SourceStream.Length];
                await SourceStream.ReadAsync(result, 0, (int)SourceStream.Length);
            }

            List<byte> content = new List<byte>(result);
            ShowLogFile(content, this.archiveLogDataGrid, "1", false,frm,null,false);
            this.archiveLogDataGrid.BringToFront();

            frm.DialogResult = DialogResult.OK;

        }

        private ArchiveTreeLogItemRecord archiveWorkRecord = null;

        /// <summary>
        /// показ лог-файла архива
        /// </summary>
        /// <param name="frm"></param>
        private void DoShowArchiveLog(ConnectForm frm)
        {
            System.Diagnostics.Debug.Assert(archiveWorkRecord != null);
            string fname = archiveWorkRecord.FileName;
            frm.Update();
            try
            {

                DoReadArchiveLog(frm, fname);
            }
            catch {
                frm.DialogResult = DialogResult.OK;
            }

            
        }

        /// <summary>
        /// просмотр лог-файла архива
        /// </summary>
        /// <param name="atlir"></param>
        private void ShowArchiveLog(ArchiveTreeLogItemRecord atlir)
        {
            ConnectForm cn = new ConnectForm(true);
            cn.OnConnectFormShown = this.DoShowArchiveLog;
            cn.lblCurrentAction.Text = "Загружаем лог...";
            archiveWorkRecord = atlir;
            cn.ShowDialog();
            
        }

        /// <summary>
        /// выбор узла дерева
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
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

        /// <summary>
        /// установка порогов трансформатора
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
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

        /// <summary>
        /// установка задержки реле
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void btnSetRelayDelay_Click(object sender, EventArgs e)
        {
            inSetRelayDelayToController = true;
            ShowWaitCursor(true);

            string s = "";
            s += Convert.ToString(nudRelayDelay.Value);
            s += PARAM_DELIMITER + Convert.ToString(nudACSDelay.Value);
            s += PARAM_DELIMITER + Convert.ToString(nudMaxIdleTime.Value);

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

        /// <summary>
        /// экспорт графика эталона в рисунок
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
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

        /// <summary>
        /// обработчик выбора узла дерева настроек
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
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

                    case TreeNodeType.AdditionalSettingsNode:
                        ShowAdditionalSettings();
                        break;

                    //TODO: Тут другие панели!!!

                    case TreeNodeType.SDSettingsNode:
                        this.plSDSettings.BringToFront();
                        break;


                } // switch
            }
        }
        private bool inSetEthalonRecordToController = false;

        /// <summary>
        /// запись эталона вверх
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void btnRecordEthalonUp_Click(object sender, EventArgs e)
        {
            recordEthalon("UP");
        }

        /// <summary>
        /// запись эталона вниз
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void btnRecordEthalonDown_Click(object sender, EventArgs e)
        {
            recordEthalon("DOWN");
        }

        /// <summary>
        /// начало записи эталона
        /// </summary>
        /// <param name="dir"></param>
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

        /// <summary>
        /// завершение записи эталона
        /// </summary>
        /// <param name="a"></param>
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

        /// <summary>
        /// отсоединение
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void btnDisconnect_Click(object sender, EventArgs e)
        {
            currentTransport.Disconnect();
            this.treeView.Nodes[0].Nodes.Clear();
        }

        /// <summary>
        /// соединение
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void btnConnect_Click(object sender, EventArgs e)
        {
            DoConnect("", false, true);
        }

        /// <summary>
        /// результат тестирования SD
        /// </summary>
        /// <param name="a"></param>
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

        /// <summary>
        /// начало теста SD
        /// </summary>
        /// <param name="frm"></param>
        private void StartSDTest(ConnectForm frm)
        {
            PushCommandToQueue(GET_PREFIX + "SDTEST", ParseSDTest);

            //frm.DialogResult = DialogResult.OK;
        }

        /// <summary>
        /// тест SD
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
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

        /// <summary>
        /// установка флагов АСУ ТП
        /// </summary>
        /// <param name="a"></param>
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

        /// <summary>
        /// установка коэффициента тока
        /// </summary>
        /// <param name="a"></param>
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

        /// <summary>
        /// начало установки коэффициента тока
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
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

        /// <summary>
        /// начало установки флагов АСУ ТП
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
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

        private bool inSetRodSettingsToController = true;

        /// <summary>
        /// установка величины перемещения привода
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void btnSetRodSettings_Click(object sender, EventArgs e)
        {
            inSetRodSettingsToController = true;
            ShowWaitCursor(true);

            string s = Convert.ToString(nudRodMoveLength.Value);
            PushCommandToQueue(SET_PREFIX + "RLENGTH" + PARAM_DELIMITER + s, ParseSetRodMoveLength);
        }

        private bool inSetMBUSIDToController = true;

        private void btnSetModbusSlaveID_Click(object sender, EventArgs e)
        {
            inSetMBUSIDToController = true;
            ShowWaitCursor(true);

            string s = Convert.ToString(nudModbusSlaveID.Value);
            PushCommandToQueue(SET_PREFIX + "MBUSID" + PARAM_DELIMITER + s, ParseSetMBUSID);
        }
    }

}
