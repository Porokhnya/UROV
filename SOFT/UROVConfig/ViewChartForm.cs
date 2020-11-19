using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Windows.Forms.DataVisualization.Charting;

namespace UROVConfig
{
    public partial class ViewChartForm : Form
    {
        public ViewChartForm(InterruptRecord rec, string si, string sn)
        {
            InitializeComponent();

            record = rec;
            stationID = si;
            stationName = sn;

            if(stationID == "")
            {
                stationID = "1";
            }

            if(stationName == "")
            {
                stationName = "Station";
            }
        }

        private InterruptRecord record = null;
        private string stationID;
        private string stationName;

        public void setDefaultFileName(string fname)
        {
            saveFileDialog.FileName = fname;
            exportToCSVDialog.FileName = fname;
            exportToCoMTRADEDialog.FileName = fname;
        }

        private bool SaveToCSV(string fname)
        {
            // серия эталона - у нас 0, прерывания - 1. При наличии прерывания - экспортируем прерывание, иначе - эталон
            System.Windows.Forms.DataVisualization.Charting.Series serieToExport = null;
            if (this.chart.Series.Count < 1)
            {
                MessageBox.Show("К экспорту не найдено ни одного графика с данными!", "Сообщение", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return false;
            }
            else
            {
                if (this.chart.Series.Count > 1)
                {
                    serieToExport = this.chart.Series[1]; // прерывание
                }
                else
                {
                    serieToExport = this.chart.Series[0]; // эталон
                }
            }

            StringBuilder sb = new StringBuilder();
            foreach (var pt in serieToExport.Points)
            {
                sb.AppendLine(pt.YValues[0].ToString());
            }

            try
            {
                System.IO.File.WriteAllText(fname, sb.ToString());
            }
            catch
            {
                return false;
            }

            return true;
        }

        private void btnExport_Click(object sender, EventArgs e)
        {

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
                                this.chart.SaveImage(fname, System.Windows.Forms.DataVisualization.Charting.ChartImageFormat.Png);

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
                                this.chart.SaveImage(fname, System.Windows.Forms.DataVisualization.Charting.ChartImageFormat.Jpeg);
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

        private void btnExportToCSV_Click(object sender, EventArgs e)
        {
            if (exportToCSVDialog.ShowDialog() == DialogResult.OK)
            {
                bool errors = false;
                string fname = exportToCSVDialog.FileName;

                errors = !SaveToCSV(fname);

                if (errors)
                {
                    MessageBox.Show("Ошибка экспорта!", "Сообщение", MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
                else
                {
                    MessageBox.Show("Данные экспортированы.", "Сообщение", MessageBoxButtons.OK, MessageBoxIcon.Information);
                }
            }
        }

        private bool SaveToCOMTRADE(string filename)
        {
            //TODO: ТУТ ЭКСПОРТ В COMTRADE !!!

            //TODO: УБРАТЬ ТЕСТОВЫЕ ДАННЫЕ !!!
            /*
            // начало тестовых данных
            record.DataArrivedTime = 10; // через 10 миллисекунд пришли данные по импульсам

            int tm = 0;
            Random rnd = new Random();

            for (int i=0;i<20;i++)
            {
                record.CurrentTimes.Add(tm);
                tm += 45000; // каждые 45 миллисекунд - семплирование
                record.CurrentData1.Add(rnd.Next(1024));
                record.CurrentData2.Add(rnd.Next(1024));
                record.CurrentData3.Add(rnd.Next(1024));
            }

            // конец тестовых данных
            */


            /* 
             у нас есть данные в нескольких списках. Нам надо КОРРЕКТНО отобразить эти данные, с учётом разных временнЫх интервалов между ними.
             проблема в том, что для каждой записи в COMTRADE должны быть ВСЕ данные ВСЕХ выборок, а у нас - выборки по току и выборки по графику эталона - 
             НЕ совпадают по времени. Всё, что мы можем гарантировать - это тот факт, что последняя точка графика тока - находится по времени близко к правой границе
             графиков импульсов.

            у нас 5 каналов входящих данных:

                1. Канал тока 1 (0-1024)
                2. Канал тока 2 (0-1024)
                3. Канал тока 3 (0-1024)
                4. Импульсы штанги (0-100% по высоте заполнения графика)
                5. Импульсы эталона (0-100% по высоте заполнения графика)

            у первых трёх каналов - время каждой записи одинаковое
            остальные два канала - времена записи на осциллограмме отличаются

            ЗАДАЧА: совместить на графике все эти данные, чтобы поток был неразрывным, присутствовали все данные и всё отображалось корректно.
            */

            // создаём список уникальных временнЫх меток
            List<int> times = new List<int>();
            times.AddRange(record.CurrentTimes);
            times.AddRange(record.InterruptData);
            times.AddRange(record.EthalonData);

            const int NUM_CHANNELS = 5;

            // получаем уникальный список временнЫх меток
            IEnumerable<int> uniqueTimes = times.Distinct().OrderBy(i=>i);

            // теперь надо заполнить данные по каналам, забиваем их нулями для начала
            List<ChannelData> channelsData = new List<ChannelData>();
            foreach (int time in uniqueTimes)
            {
                ChannelData chData = new ChannelData();
                chData.RecordTime = time;

                // у нас 5 каналов, для каждого из каналов - добавляем 0 как данные
                for(int i=0;i< NUM_CHANNELS;i++)
                {
                    chData.Values.Add(0);
                }

                channelsData.Add(chData);
                
            } // foreach

            // теперь проходимся по списку данных тока, запоминая итератор списка. Как только текущее значение временнОй метки станет больше 
            // значения временнОй метки в списке по току - увеличиваем итератор. Если он больше размера списка по току - пишем 0.
            // в начале - пропускаем N записей до первой актуальной.

            int dataIter = 0;
            while(record.CurrentTimes.Count > 0 && channelsData[dataIter].RecordTime < record.CurrentTimes[0])
            {
                dataIter++;
            }

            for(int i=0;i<record.CurrentTimes.Count;i++)
            {
                int currentTime = record.CurrentTimes[i];

                if(i < record.CurrentTimes.Count-1)
                {
                    currentTime = record.CurrentTimes[i+1];

                    // берём следующую запись
                    while (channelsData[dataIter].RecordTime < currentTime)
                    {
                        channelsData[dataIter].Values[0] = record.CurrentData1[i];
                        channelsData[dataIter].Values[1] = record.CurrentData2[i];
                        channelsData[dataIter].Values[2] = record.CurrentData3[i];
                        dataIter++;

                        if (dataIter >= channelsData.Count)
                            break;
                    }
                }
                else
                {
                    // просто добавляем
                    channelsData[dataIter].Values[0] = record.CurrentData1[i];
                    channelsData[dataIter].Values[1] = record.CurrentData2[i];
                    channelsData[dataIter].Values[2] = record.CurrentData3[i];
                    dataIter++;
                }

                if (dataIter >= channelsData.Count)
                    break;


            }

            // теперь заполняем список данных по прерыванию, предварительно его сформировав
            // считаем данные графика прерывания

            List<int> interruptPercentsList = new List<int>();

            if (record.InterruptData.Count > 0)
            {


                // получаем максимальное время импульса - это будет 100% по оси Y
                int maxPulseTime = 0;
                for (int i = 1; i < record.InterruptData.Count; i++)
                {
                    maxPulseTime = Math.Max(maxPulseTime, (record.InterruptData[i] - record.InterruptData[i - 1]));
                }

                // теперь считаем все остальные точки
                for (int i = 1; i < record.InterruptData.Count; i++)
                {
                    int pulseTime = record.InterruptData[i] - record.InterruptData[i - 1];
                    pulseTime *= 100;

                    int pulseTimePercents = pulseTime / maxPulseTime;
                    pulseTimePercents = 100 - pulseTimePercents;

                    interruptPercentsList.Add(pulseTimePercents);

                } // for

                // в interruptPercentsList у нас теперь график от 0 до 100%, по прерыванию
                // можем заполнять список данных по каналу
                dataIter = 0;
                while (channelsData[dataIter].RecordTime < record.InterruptData[0])
                {
                    dataIter++;
                }

                for (int i = 0; i < interruptPercentsList.Count; i++)
                {
                    int currentTime = record.InterruptData[i];

                    if (i < interruptPercentsList.Count - 1)
                    {
                        currentTime = record.InterruptData[i + 1];

                        // берём следующую запись
                        while (channelsData[dataIter].RecordTime < currentTime)
                        {
                            channelsData[dataIter].Values[3] = interruptPercentsList[i];
                            dataIter++;

                            if (dataIter >= channelsData.Count)
                                break;
                        }
                    }
                    else
                    {
                        // просто добавляем
                        channelsData[dataIter].Values[3] = interruptPercentsList[i];
                        dataIter++;
                    }

                    if (dataIter >= channelsData.Count)
                        break;


                } // for
            } // if (record.InterruptData.Count > 0)

            // теперь точно так же заполняем данные по эталону
            interruptPercentsList.Clear();

            if (record.EthalonData.Count > 0)
            {

                // получаем максимальное время импульса - это будет 100% по оси Y
                int maxPulseTime = 0;

                for (int i = 1; i < record.EthalonData.Count; i++)
                {
                    maxPulseTime = Math.Max(maxPulseTime, (record.EthalonData[i] - record.EthalonData[i - 1]));
                }

                // теперь считаем все остальные точки

                for (int i = 1; i < record.EthalonData.Count; i++)
                {
                    int pulseTime = record.EthalonData[i] - record.EthalonData[i - 1];
                    pulseTime *= 100;

                    int pulseTimePercents = pulseTime / maxPulseTime;
                    pulseTimePercents = 100 - pulseTimePercents;

                    interruptPercentsList.Add(pulseTimePercents);

                } // for

                // в interruptPercentsList у нас теперь график от 0 до 100%, по прерыванию
                // можем заполнять список данных по каналу
                dataIter = 0;
                while (channelsData[dataIter].RecordTime < record.EthalonData[0])
                {
                    dataIter++;
                }

                for (int i = 0; i < interruptPercentsList.Count; i++)
                {
                    int currentTime = record.EthalonData[i];

                    if (i < interruptPercentsList.Count - 1)
                    {
                        currentTime = record.EthalonData[i + 1];

                        // берём следующую запись
                        while (channelsData[dataIter].RecordTime < currentTime)
                        {
                            channelsData[dataIter].Values[4] = interruptPercentsList[i];
                            dataIter++;

                            if (dataIter >= channelsData.Count)
                                break;
                        }
                    }
                    else
                    {
                        // просто добавляем
                        channelsData[dataIter].Values[4] = interruptPercentsList[i];
                        dataIter++;
                    }

                    if (dataIter >= channelsData.Count)
                        break;


                } // for

            } // record.EthalonData.Count > 0


            // тут мы получили данные по каналам, можно экспортировать
            List<string> channelsNames = new List<string>();
            channelsNames.Add("ТОК1");
            channelsNames.Add("ТОК2");
            channelsNames.Add("ТОК3");
            channelsNames.Add("ИМПУЛЬСЫ");
            channelsNames.Add("ЭТАЛОН");

            List<string> channelsFormats = new List<string>();
            channelsFormats.Add("V");
            channelsFormats.Add("V");
            channelsFormats.Add("V");
            channelsFormats.Add("ms");
            channelsFormats.Add("ms");

            // пишем максимальные данные по каналам
            List<int> maxVals = new List<int>();
            maxVals.Add(1024);
            maxVals.Add(1024);
            maxVals.Add(1024);
            maxVals.Add(100);
            maxVals.Add(100);


            StringBuilder sb = new StringBuilder();
            // формируем файл конфига
            char eof = (char)0x0A;

            string configFile = System.IO.Path.ChangeExtension(filename, ".cfg");
            sb.Clear();

            // station_name,id<CR,LF>
            sb.AppendFormat("{0},{1}\r\n", stationName, stationID);

            // TT,nnt,nnt<CR,LF>
            sb.AppendFormat("{0:00},{1:00}A,{2:00}D\r\n", NUM_CHANNELS, NUM_CHANNELS, 0);

            // nn,id,p,cccccc,uu,a,b,skew,min,max<CR,LF>
            for (int i = 0; i < NUM_CHANNELS; i++)
            {
                int dataArrivTime = record.DataArrivedTime * 1000;
                if(i < 3)
                {
                    dataArrivTime = 0;
                }
                sb.AppendFormat("{0:00},{1},{2},{3:000000},{4},{5},{6},{7},{8},{9},1,1,P\r\n", i + 1, channelsNames[i], i + 1, i + 1, channelsFormats[i], 1, 0, dataArrivTime, 0, maxVals[i]);
            }

            // lf<CR,LF>
            sb.AppendLine("50");

            // nrates<CR,LF>
            sb.AppendLine("1");

            // sssssl,endsampl<CR,LF>
            sb.AppendFormat("{0},{1}\r\n", 50, channelsData.Count);

            // mm/dd/yy,hh:mm:ss.ssssss<CR,LF>
            // mm/dd/yy,hh:mm:ss.ssssss<CR,LF>
            string dtStr = record.InterruptInfo.InterruptTime.ToString("MM/dd/yyyy,HH:mm:ss.ffffff", System.Globalization.CultureInfo.InvariantCulture);
            sb.AppendLine(dtStr);
            sb.AppendLine(dtStr);

            // ft<CR,LF>
            sb.AppendLine("ASCII");

            // timemult
            sb.Append("1");

            try
            {
                System.IO.File.WriteAllText(configFile, sb.ToString(), Encoding.UTF8);
            }
            catch
            {
                return false;
            }


            sb.Clear();

            // теперь пишем все данные в файл данных
            int cntr = 0;
            int recNum = 1;
            foreach (var pt in channelsData)
            {
                string line = String.Format("{0:0000000000},{1:0000000000}", recNum, pt.RecordTime);
                foreach(int dt in pt.Values)
                {
                    line +=  String.Format(",{0:000000}", dt);
                }
               
               // string line = String.Format("{0:0000000000},{1:0000000000},{2:000000},{3:000000}", recNum, timeList[recNum - 1], pt, ethalonData);
                sb.AppendLine(line);

                recNum++;
                cntr++;
            }

            try
            {
                System.IO.File.WriteAllText(filename, sb.ToString() + eof);
            }
            catch
            {
                return false;
            }

            return true;
        }

        private void btnExportToCOMTRADE_Click(object sender, EventArgs e)
        {
            // экспорт в COMTRADE
            if (exportToCoMTRADEDialog.ShowDialog() == DialogResult.OK)
            {
                bool errors = false;
                string fname = exportToCoMTRADEDialog.FileName;

                errors = !SaveToCOMTRADE(fname);

                if (errors)
                {
                    MessageBox.Show("Ошибка экспорта!", "Сообщение", MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
                else
                {
                    MessageBox.Show("Данные экспортированы.", "Сообщение", MessageBoxButtons.OK, MessageBoxIcon.Information);
                }
            }
        }

        private int interval = 1;
        public void setInterval(int val)
        {
            interval = val;
        }

        private bool ethalonVisible = true;
        private bool interruptVisible = true;

        public void SetEthalonAvailable(bool avail)
        {
            ethalonVisible = avail;

            if (avail)
            {
                chart.Series[0].Enabled = true;
            }
            else
            {
                chart.Series[0].Enabled = false;
            }
        }

        public void SetChartOfCurrentAvailable(bool avail)
        {
            if (avail) // есть токи
            {

                int cntOfVisiblePhases = 0;
                Config conf = Config.Instance;

                if(conf.ChartPhase1Visible)
                {
                    cntOfVisiblePhases++;
                }

                if (conf.ChartPhase2Visible)
                {
                    cntOfVisiblePhases++;
                }

                if (conf.ChartPhase3Visible)
                {
                    cntOfVisiblePhases++;
                }


                if(cntOfVisiblePhases < 1) // нет ни одной видимой фазы
                {
                    chart.ChartAreas[0].Position.Height = 80;// 26;
                    chart.ChartAreas[1].Visible = false;
                    chart.ChartAreas[2].Visible = false;
                    chart.ChartAreas[3].Visible = false;
                }
                else
                {
                    int availHeight = 80 - 26; // доступная высота для фаз, в процентах (не считая графика прерывания)
                    int onePhaseHeight = availHeight / cntOfVisiblePhases;

                    chart.ChartAreas[1].Visible = conf.ChartPhase1Visible;
                    chart.ChartAreas[1].Position.Height = onePhaseHeight;

                    chart.ChartAreas[2].Visible = conf.ChartPhase2Visible;
                    chart.ChartAreas[2].Position.Height = onePhaseHeight;

                    chart.ChartAreas[3].Visible = conf.ChartPhase3Visible;
                    chart.ChartAreas[3].Position.Height = onePhaseHeight;

                }

                /*
                chart.ChartAreas[0].Position.Height = 80;// 26;
                chart.ChartAreas[1].Visible = false;
                chart.ChartAreas[2].Visible = false;
                chart.ChartAreas[3].Visible = false;
                */

                chart.Series[2].Enabled = true;
                chart.Series[3].Enabled = true;
                chart.Series[4].Enabled = true;

            }
            else
            {
                // токов нет, скрываем серии
                chart.ChartAreas[0].Position.Height = 80;

                chart.ChartAreas[1].Visible = false;
                chart.ChartAreas[2].Visible = false;
                chart.ChartAreas[3].Visible = false;

                chart.Series[2].Enabled = false;
                chart.Series[3].Enabled = false;
                chart.Series[4].Enabled = false;

            }
        }

        private void ViewChartForm_Load(object sender, EventArgs e)
        {
            // задаём настройки масштабирования графиков

            for (int i = 0; i < chart.ChartAreas.Count; i++)
            {
                ChartArea area = chart.ChartAreas[i];


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

            /*
            // все птички по умолчанию включены
            for(int i=0;i<chart.Series.Count;i++)
            {
                chart.Series[i].SetCustomProperty("CHECK", "☑");
            }
            */

            Config conf = Config.Instance;

            // включаем птички по умолчанию только для графика прерываний
            chart.Series["serieEthalon"].SetCustomProperty("CHECK", "☑");
            chart.Series["serieInterrupt"].SetCustomProperty("CHECK", "☑");
            chart.Series["seriePhase1"].SetCustomProperty("CHECK", conf.ChartPhase1Visible ? "☑" : "☐");
            chart.Series["seriePhase2"].SetCustomProperty("CHECK", conf.ChartPhase2Visible ? "☑" : "☐");
            chart.Series["seriePhase3"].SetCustomProperty("CHECK", conf.ChartPhase3Visible ? "☑" : "☐");

            chart.Legends[0].CellColumns.Clear();

            chart.Legends[0].CellColumns.Add(new LegendCellColumn()
            {
                Name = "chbx",
                ColumnType = LegendCellColumnType.Text,
                Text = "#CUSTOMPROPERTY(CHECK)",
                Font = new Font("Segoe UI", 12, FontStyle.Bold)
            });

            chart.Legends[0].CellColumns.Add(new LegendCellColumn()
            {
                Name = "symbol",
                ColumnType = LegendCellColumnType.SeriesSymbol
            });

            chart.Legends[0].CellColumns.Add(new LegendCellColumn()
            {
                Name = "title",
                ColumnType = LegendCellColumnType.Text,
                Text = "#LEGENDTEXT"
            });

            chart.ApplyPaletteColors();

            foreach (Series series in chart.Series)
            {
                chart.Series[series.Name].SetCustomProperty(
                    "COLOR",
                    ColorTranslator.ToHtml(series.Color));
            }

            realignAreas();
        }

        private void dealWithEthalonAndInterrupt(string area)
        {
            if (!ethalonVisible && !interruptVisible)
            {
                chart.ChartAreas[area].Visible = false;
            }
            else
            {
                chart.ChartAreas[area].Visible = true;

                if (!ethalonVisible)
                {
                    chart.Series["serieEthalon"].Color = Color.FromArgb(0, chart.Series["serieEthalon"].Color);
                }
                else
                {
                    chart.Series["serieEthalon"].Color = ColorTranslator.FromHtml(chart.Series["serieEthalon"].GetCustomProperty("COLOR"));
                }

                if (!interruptVisible)
                {
                    chart.Series["serieInterrupt"].Color = Color.FromArgb(0, chart.Series["serieInterrupt"].Color);
                }
                else
                {
                    chart.Series["serieInterrupt"].Color = ColorTranslator.FromHtml(chart.Series["serieInterrupt"].GetCustomProperty("COLOR"));
                }
            }
        }

        private void realignAreas()
        {
            // выравниваем видимые области по высоте
            float totalPercentsAvailable = 90.0f; // сколько всего процентов по высоте доступно
            float ySpacingPercents = 2.0f; // зазор, в процентах, по Y между видимыми областями

            int totalVisibleAreas = 0;
            for(int i=0;i<chart.ChartAreas.Count;i++)
            {
                if(chart.ChartAreas[i].Visible)
                {
                    totalVisibleAreas++;
                }
            }

            if(totalVisibleAreas < 1) // нет видимых областей
            {
                return;
            }

            // теперь делим равномерно высоту между областями
            float oneAreaHeight = totalPercentsAvailable / totalVisibleAreas;
            float curY = ySpacingPercents;

            for (int i = 0; i < chart.ChartAreas.Count; i++)
            {
                if (chart.ChartAreas[i].Visible)
                {
                    chart.ChartAreas[i].Position.Height = oneAreaHeight - ySpacingPercents;
                    chart.ChartAreas[i].Position.Y = curY;

                    curY += oneAreaHeight;

                }

            }
        }

        private void chart_MouseDown(object sender, MouseEventArgs e)
        {
            HitTestResult result = chart.HitTest(e.X, e.Y);

            if (result != null && result.Object != null
                && result.Object is LegendItem && e.Button == MouseButtons.Left)
            {
                LegendItem legendItem = (LegendItem)result.Object;
                Config conf = Config.Instance;

                Series series = chart.Series[legendItem.SeriesName];

                if (series.GetCustomProperty("CHECK").Equals("☑"))
                {
                    series.SetCustomProperty("CHECK", "☐");
                    // series.Color = Color.FromArgb(0, series.Color);
                    if (legendItem.SeriesName == "serieEthalon")
                    {
                        ethalonVisible = false;
                        dealWithEthalonAndInterrupt(series.ChartArea);
                    }
                    else if (legendItem.SeriesName == "serieInterrupt")
                    {
                        interruptVisible = false;
                        dealWithEthalonAndInterrupt(series.ChartArea);
                    }
                    else
                    {
                        chart.ChartAreas[series.ChartArea].Visible = false;
                        if(legendItem.SeriesName == "seriePhase1")
                        {
                            conf.ChartPhase1Visible = false;
                        }
                        else if (legendItem.SeriesName == "seriePhase2")
                        {
                            conf.ChartPhase2Visible = false;
                        }
                        else if (legendItem.SeriesName == "seriePhase3")
                        {
                            conf.ChartPhase3Visible = false;
                        }

                    }


                }
                else
                {
                    series.SetCustomProperty("CHECK", "☑");
                    //    series.Color = ColorTranslator.FromHtml(
                    //             series.GetCustomProperty("COLOR"));
                    if (legendItem.SeriesName == "serieEthalon")
                    {
                        ethalonVisible = true;
                        dealWithEthalonAndInterrupt(series.ChartArea);
                    }
                    else if (legendItem.SeriesName == "serieInterrupt")
                    {
                        interruptVisible = true;
                        dealWithEthalonAndInterrupt(series.ChartArea);
                    }
                    else
                    {
                        chart.ChartAreas[series.ChartArea].Visible = true;
                        if (legendItem.SeriesName == "seriePhase1")
                        {
                            conf.ChartPhase1Visible = true;
                        }
                        else if (legendItem.SeriesName == "seriePhase2")
                        {
                            conf.ChartPhase2Visible = true;
                        }
                        else if (legendItem.SeriesName == "seriePhase3")
                        {
                            conf.ChartPhase3Visible = true;
                        }
                    }

                    
                }

                realignAreas();
            }
        }
    }
}
