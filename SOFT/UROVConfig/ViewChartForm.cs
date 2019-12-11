using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

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
            StringBuilder sb = new StringBuilder();

            int recNum = 1;
            char eof = (char)0x0A;

            //int maxVal = 100;
            List<int> maxVals = new List<int>();
            maxVals.Add(100);
            maxVals.Add(0);


            int channelsCount = 1;

            // считаем данные графика прерывания

            List<int> timeList = record.InterruptData;
            List<int> percentsList = new List<int>();


            // получаем максимальное время импульса - это будет 100% по оси Y
            int maxPulseTime = 0;
            for (int i = 1; i < timeList.Count; i++)
            {
                maxPulseTime = Math.Max(maxPulseTime, (timeList[i] - timeList[i - 1]));
            }

            int endStop = timeList.Count;

            for (int i = 1; i < record.EthalonData.Count; i++)
            {
                maxPulseTime = Math.Max(maxPulseTime, (record.EthalonData[i] - record.EthalonData[i - 1]));
            }

            endStop = Math.Min(endStop, record.EthalonData.Count);

            if (record.EthalonData.Count < 1)
                endStop = timeList.Count;


            // теперь считаем все остальные точки
            for (int i = 1; i < endStop; i++)
            {
                int pulseTime = timeList[i] - timeList[i - 1];
                pulseTime *= 100;

                int pulseTimePercents = pulseTime / maxPulseTime;
                pulseTimePercents = 100 - pulseTimePercents;

                percentsList.Add(pulseTimePercents);

            } // for

            // теперь надо считать данные графика эталона, если он есть
            
            List<int> ethalonList = new List<int>();
            if(record.EthalonData.Count > 0)
            {
                maxVals[1] = 100;
            }
            // считаем график эталона
            for (int i = 1; i < record.EthalonData.Count; i++)
            {
                int pulseTime = record.EthalonData[i] - record.EthalonData[i - 1];
                pulseTime *= 100;

                int pulseTimePercents = pulseTime / maxPulseTime;
                pulseTimePercents = 100 - pulseTimePercents;


                ethalonList.Add(pulseTimePercents);

            } // for


            // тут запись данных в файл данных. у нас может быть больше одного канала, так что это надо учитывать!!!
            int cntr = 0;
            foreach (var pt in percentsList)
            {
                //                maxVal = Math.Max(maxVal, pt);
                int ethalonData = 0;
                if(ethalonList.Count > 0 && ethalonList.Count < cntr)
                {
                    ethalonData = ethalonList[cntr];
                }

                //string line = String.Format("{0:0000000000},{1:0000000000},{2:000000},{3:000000}", recNum, timeList[recNum-1], pt, ethalonData);
                string line = String.Format("{0:0000000000},{1:0000000000},{2:000000}", recNum, timeList[recNum - 1], pt);
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

            string configFile = System.IO.Path.ChangeExtension(filename, ".cfg");
            sb.Clear();

            // station_name,id<CR,LF>
            sb.AppendFormat("{0},{1}\r\n", stationName, stationID);

            // TT,nnt,nnt<CR,LF>
            sb.AppendFormat("{0:00},{1:00}A,{2:00}D\r\n", channelsCount, channelsCount, 0);

            // nn,id,p,cccccc,uu,a,b,skew,min,max<CR,LF>
            for (int i = 0; i < channelsCount; i++)
            {
                sb.AppendFormat("{0:00},{1},{2},{3:000000},{4},{5},{6},{7},{8},{9},1,1,P\r\n", i+1, i+1, i+1, i+1, "ms", 1, 0, 0, 0, maxVals[i]);
            }

            // lf<CR,LF>
            sb.AppendLine("50");

            // nrates<CR,LF>
            sb.AppendLine(channelsCount.ToString());

            // sssssl,endsampl<CR,LF>
            for (int i = 0; i < channelsCount; i++)
            {
                sb.AppendFormat("{0},{1}\r\n", 50, percentsList.Count);//record.InterruptData.Count);
            }

            // mm/dd/yy,hh:mm:ss.ssssss<CR,LF>
            // mm/dd/yy,hh:mm:ss.ssssss<CR,LF>
            string dt = record.InterruptInfo.InterruptTime.ToString("MM/dd/yyyy,HH:mm:ss.ffffff", System.Globalization.CultureInfo.InvariantCulture);
            sb.AppendLine(dt);
            sb.AppendLine(dt);

            // ft<CR,LF>
            sb.AppendLine("ASCII");

            // timemult
            sb.Append("1");

            try
            {
                System.IO.File.WriteAllText(configFile, sb.ToString());
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
    }
}
