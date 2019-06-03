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
        public ViewChartForm()
        {
            InitializeComponent();
        }

        public void setDefaultFileName(string fname)
        {
            saveFileDialog.FileName = fname;
            exportToCSVDialog.FileName = fname;
        }

        private bool SaveToCSV(string fname)
        {
            // серия эталона - у нас 0, прерывания - 1. При наличии прерывания - экспортируем прерывание, иначе - эталон
            System.Windows.Forms.DataVisualization.Charting.Series serieToExport = null;
            if(this.chart.Series.Count < 1)
            {
                MessageBox.Show("К экспорту не найдено ни одного графика с данными!", "Сообщение", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return false;
            }
            else
            {
                if(this.chart.Series.Count > 1)
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
                switch(saveFileDialog.FilterIndex)
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

                if(errors)
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
    }
}
