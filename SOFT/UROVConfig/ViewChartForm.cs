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
    }
}
