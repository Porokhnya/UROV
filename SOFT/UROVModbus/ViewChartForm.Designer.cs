namespace UROVModbus
{
    partial class ViewChartForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.Windows.Forms.DataVisualization.Charting.ChartArea chartArea1 = new System.Windows.Forms.DataVisualization.Charting.ChartArea();
            System.Windows.Forms.DataVisualization.Charting.ChartArea chartArea2 = new System.Windows.Forms.DataVisualization.Charting.ChartArea();
            System.Windows.Forms.DataVisualization.Charting.ChartArea chartArea3 = new System.Windows.Forms.DataVisualization.Charting.ChartArea();
            System.Windows.Forms.DataVisualization.Charting.ChartArea chartArea4 = new System.Windows.Forms.DataVisualization.Charting.ChartArea();
            System.Windows.Forms.DataVisualization.Charting.Legend legend1 = new System.Windows.Forms.DataVisualization.Charting.Legend();
            System.Windows.Forms.DataVisualization.Charting.Legend legend2 = new System.Windows.Forms.DataVisualization.Charting.Legend();
            System.Windows.Forms.DataVisualization.Charting.LegendItem legendItem1 = new System.Windows.Forms.DataVisualization.Charting.LegendItem();
            System.Windows.Forms.DataVisualization.Charting.LegendCell legendCell1 = new System.Windows.Forms.DataVisualization.Charting.LegendCell();
            System.Windows.Forms.DataVisualization.Charting.LegendCell legendCell2 = new System.Windows.Forms.DataVisualization.Charting.LegendCell();
            System.Windows.Forms.DataVisualization.Charting.LegendCell legendCell3 = new System.Windows.Forms.DataVisualization.Charting.LegendCell();
            System.Windows.Forms.DataVisualization.Charting.LegendCell legendCell4 = new System.Windows.Forms.DataVisualization.Charting.LegendCell();
            System.Windows.Forms.DataVisualization.Charting.LegendCell legendCell5 = new System.Windows.Forms.DataVisualization.Charting.LegendCell();
            System.Windows.Forms.DataVisualization.Charting.LegendCell legendCell6 = new System.Windows.Forms.DataVisualization.Charting.LegendCell();
            System.Windows.Forms.DataVisualization.Charting.LegendCell legendCell7 = new System.Windows.Forms.DataVisualization.Charting.LegendCell();
            System.Windows.Forms.DataVisualization.Charting.Legend legend3 = new System.Windows.Forms.DataVisualization.Charting.Legend();
            System.Windows.Forms.DataVisualization.Charting.LegendItem legendItem2 = new System.Windows.Forms.DataVisualization.Charting.LegendItem();
            System.Windows.Forms.DataVisualization.Charting.LegendCell legendCell8 = new System.Windows.Forms.DataVisualization.Charting.LegendCell();
            System.Windows.Forms.DataVisualization.Charting.Legend legend4 = new System.Windows.Forms.DataVisualization.Charting.Legend();
            System.Windows.Forms.DataVisualization.Charting.LegendItem legendItem3 = new System.Windows.Forms.DataVisualization.Charting.LegendItem();
            System.Windows.Forms.DataVisualization.Charting.LegendCell legendCell9 = new System.Windows.Forms.DataVisualization.Charting.LegendCell();
            System.Windows.Forms.DataVisualization.Charting.Legend legend5 = new System.Windows.Forms.DataVisualization.Charting.Legend();
            System.Windows.Forms.DataVisualization.Charting.LegendItem legendItem4 = new System.Windows.Forms.DataVisualization.Charting.LegendItem();
            System.Windows.Forms.DataVisualization.Charting.LegendCell legendCell10 = new System.Windows.Forms.DataVisualization.Charting.LegendCell();
            System.Windows.Forms.DataVisualization.Charting.Series series1 = new System.Windows.Forms.DataVisualization.Charting.Series();
            System.Windows.Forms.DataVisualization.Charting.Series series2 = new System.Windows.Forms.DataVisualization.Charting.Series();
            System.Windows.Forms.DataVisualization.Charting.Series series3 = new System.Windows.Forms.DataVisualization.Charting.Series();
            System.Windows.Forms.DataVisualization.Charting.Series series4 = new System.Windows.Forms.DataVisualization.Charting.Series();
            System.Windows.Forms.DataVisualization.Charting.Series series5 = new System.Windows.Forms.DataVisualization.Charting.Series();
            this.panel1 = new System.Windows.Forms.Panel();
            this.lblCaption = new System.Windows.Forms.Label();
            this.panel2 = new System.Windows.Forms.Panel();
            this.chart = new System.Windows.Forms.DataVisualization.Charting.Chart();
            this.panel3 = new System.Windows.Forms.Panel();
            this.btnExportToCOMTRADE = new System.Windows.Forms.Button();
            this.btnExportToCSV = new System.Windows.Forms.Button();
            this.btnExport = new System.Windows.Forms.Button();
            this.saveFileDialog = new System.Windows.Forms.SaveFileDialog();
            this.exportToCSVDialog = new System.Windows.Forms.SaveFileDialog();
            this.exportToCoMTRADEDialog = new System.Windows.Forms.SaveFileDialog();
            this.panel1.SuspendLayout();
            this.panel2.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.chart)).BeginInit();
            this.panel3.SuspendLayout();
            this.SuspendLayout();
            // 
            // panel1
            // 
            this.panel1.AutoSize = true;
            this.panel1.BackColor = System.Drawing.Color.SteelBlue;
            this.panel1.Controls.Add(this.lblCaption);
            this.panel1.Dock = System.Windows.Forms.DockStyle.Top;
            this.panel1.Location = new System.Drawing.Point(0, 0);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(1026, 40);
            this.panel1.TabIndex = 0;
            // 
            // lblCaption
            // 
            this.lblCaption.AutoSize = true;
            this.lblCaption.Dock = System.Windows.Forms.DockStyle.Top;
            this.lblCaption.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.lblCaption.ForeColor = System.Drawing.Color.White;
            this.lblCaption.Location = new System.Drawing.Point(0, 0);
            this.lblCaption.Margin = new System.Windows.Forms.Padding(0);
            this.lblCaption.Name = "lblCaption";
            this.lblCaption.Padding = new System.Windows.Forms.Padding(10);
            this.lblCaption.Size = new System.Drawing.Size(20, 40);
            this.lblCaption.TabIndex = 1;
            // 
            // panel2
            // 
            this.panel2.Controls.Add(this.chart);
            this.panel2.Controls.Add(this.panel3);
            this.panel2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panel2.Location = new System.Drawing.Point(0, 40);
            this.panel2.Name = "panel2";
            this.panel2.Size = new System.Drawing.Size(1026, 639);
            this.panel2.TabIndex = 1;
            // 
            // chart
            // 
            chartArea1.Name = "ethalonChartArea";
            chartArea1.Position.Auto = false;
            chartArea1.Position.Height = 28F;
            chartArea1.Position.Width = 95F;
            chartArea1.Position.X = 3F;
            chartArea1.Position.Y = 3F;
            chartArea2.AlignWithChartArea = "ethalonChartArea";
            chartArea2.Name = "phase1Area";
            chartArea2.Position.Auto = false;
            chartArea2.Position.Height = 20F;
            chartArea2.Position.Width = 95F;
            chartArea2.Position.X = 3F;
            chartArea2.Position.Y = 30F;
            chartArea3.AlignWithChartArea = "ethalonChartArea";
            chartArea3.Name = "phase2Area";
            chartArea3.Position.Auto = false;
            chartArea3.Position.Height = 20F;
            chartArea3.Position.Width = 95F;
            chartArea3.Position.X = 3F;
            chartArea3.Position.Y = 50F;
            chartArea4.AlignWithChartArea = "ethalonChartArea";
            chartArea4.Name = "phase3Area";
            chartArea4.Position.Auto = false;
            chartArea4.Position.Height = 20F;
            chartArea4.Position.Width = 95F;
            chartArea4.Position.X = 3F;
            chartArea4.Position.Y = 70F;
            this.chart.ChartAreas.Add(chartArea1);
            this.chart.ChartAreas.Add(chartArea2);
            this.chart.ChartAreas.Add(chartArea3);
            this.chart.ChartAreas.Add(chartArea4);
            this.chart.Dock = System.Windows.Forms.DockStyle.Fill;
            legend1.Docking = System.Windows.Forms.DataVisualization.Charting.Docking.Bottom;
            legend1.LegendStyle = System.Windows.Forms.DataVisualization.Charting.LegendStyle.Row;
            legend1.Name = "Legend1";
            legend1.Position.Auto = false;
            legend1.Position.Height = 5F;
            legend1.Position.Width = 90F;
            legend1.Position.X = 3F;
            legend1.Position.Y = 95F;
            legendCell1.Name = "PulsesCount";
            legendCell1.Text = "Импульсов:";
            legendCell2.Name = "EthalonPulses";
            legendCell2.Text = "Эталон:";
            legendCell3.Name = "TrigDate";
            legendCell4.Name = "Place";
            legendCell5.Name = "PreviewCount";
            legendCell6.Name = "tComputed";
            legendCell7.Name = "tFact";
            legendItem1.Cells.Add(legendCell1);
            legendItem1.Cells.Add(legendCell2);
            legendItem1.Cells.Add(legendCell3);
            legendItem1.Cells.Add(legendCell4);
            legendItem1.Cells.Add(legendCell5);
            legendItem1.Cells.Add(legendCell6);
            legendItem1.Cells.Add(legendCell7);
            legend2.CustomItems.Add(legendItem1);
            legend2.Docking = System.Windows.Forms.DataVisualization.Charting.Docking.Bottom;
            legend2.LegendStyle = System.Windows.Forms.DataVisualization.Charting.LegendStyle.Row;
            legend2.Name = "Legend2";
            legend2.Position.Auto = false;
            legend2.Position.Height = 5F;
            legend2.Position.Width = 90F;
            legend2.Position.X = 3F;
            legend2.Position.Y = 90F;
            legendCell8.Name = "Cell1";
            legendCell8.Text = "ФАЗА 1";
            legendItem2.Cells.Add(legendCell8);
            legend3.CustomItems.Add(legendItem2);
            legend3.DockedToChartArea = "phase1Area";
            legend3.Name = "Legend3";
            legendCell9.Name = "Cell1";
            legendCell9.Text = "ФАЗА 2";
            legendItem3.Cells.Add(legendCell9);
            legend4.CustomItems.Add(legendItem3);
            legend4.DockedToChartArea = "phase2Area";
            legend4.Name = "Legend4";
            legendCell10.Name = "Cell1";
            legendCell10.Text = "ФАЗА 3";
            legendItem4.Cells.Add(legendCell10);
            legend5.CustomItems.Add(legendItem4);
            legend5.DockedToChartArea = "phase3Area";
            legend5.Name = "Legend5";
            this.chart.Legends.Add(legend1);
            this.chart.Legends.Add(legend2);
            this.chart.Legends.Add(legend3);
            this.chart.Legends.Add(legend4);
            this.chart.Legends.Add(legend5);
            this.chart.Location = new System.Drawing.Point(0, 0);
            this.chart.Name = "chart";
            series1.BorderWidth = 6;
            series1.ChartArea = "ethalonChartArea";
            series1.ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Spline;
            series1.Color = System.Drawing.Color.DimGray;
            series1.Legend = "Legend1";
            series1.LegendText = "Эталон";
            series1.Name = "serieEthalon";
            series2.BorderWidth = 2;
            series2.ChartArea = "ethalonChartArea";
            series2.ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Spline;
            series2.Color = System.Drawing.Color.SteelBlue;
            series2.Legend = "Legend1";
            series2.LegendText = "Прерывание";
            series2.Name = "serieInterrupt";
            series3.BorderWidth = 2;
            series3.ChartArea = "phase1Area";
            series3.ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Spline;
            series3.Color = System.Drawing.Color.Red;
            series3.Legend = "Legend1";
            series3.LegendText = "Фаза 1";
            series3.Name = "seriePhase1";
            series4.BorderWidth = 2;
            series4.ChartArea = "phase2Area";
            series4.ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Spline;
            series4.Color = System.Drawing.Color.Blue;
            series4.Legend = "Legend1";
            series4.LegendText = "Фаза 2";
            series4.Name = "seriePhase2";
            series5.BorderWidth = 2;
            series5.ChartArea = "phase3Area";
            series5.ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Spline;
            series5.Color = System.Drawing.Color.Goldenrod;
            series5.Legend = "Legend1";
            series5.LegendText = "Фаза 3";
            series5.Name = "seriePhase3";
            this.chart.Series.Add(series1);
            this.chart.Series.Add(series2);
            this.chart.Series.Add(series3);
            this.chart.Series.Add(series4);
            this.chart.Series.Add(series5);
            this.chart.Size = new System.Drawing.Size(1026, 575);
            this.chart.TabIndex = 3;
            this.chart.Text = "chart1";
            this.chart.MouseDown += new System.Windows.Forms.MouseEventHandler(this.chart_MouseDown);
            // 
            // panel3
            // 
            this.panel3.Controls.Add(this.btnExportToCOMTRADE);
            this.panel3.Controls.Add(this.btnExportToCSV);
            this.panel3.Controls.Add(this.btnExport);
            this.panel3.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.panel3.Location = new System.Drawing.Point(0, 575);
            this.panel3.Name = "panel3";
            this.panel3.Size = new System.Drawing.Size(1026, 64);
            this.panel3.TabIndex = 2;
            // 
            // btnExportToCOMTRADE
            // 
            this.btnExportToCOMTRADE.BackColor = System.Drawing.Color.LightGreen;
            this.btnExportToCOMTRADE.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnExportToCOMTRADE.ForeColor = System.Drawing.Color.Black;
            this.btnExportToCOMTRADE.Location = new System.Drawing.Point(358, 12);
            this.btnExportToCOMTRADE.Name = "btnExportToCOMTRADE";
            this.btnExportToCOMTRADE.Size = new System.Drawing.Size(167, 40);
            this.btnExportToCOMTRADE.TabIndex = 11;
            this.btnExportToCOMTRADE.Text = "Экспорт в COMTRADE";
            this.btnExportToCOMTRADE.UseVisualStyleBackColor = false;
            this.btnExportToCOMTRADE.Click += new System.EventHandler(this.btnExportToCOMTRADE_Click);
            // 
            // btnExportToCSV
            // 
            this.btnExportToCSV.BackColor = System.Drawing.Color.LightGreen;
            this.btnExportToCSV.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnExportToCSV.ForeColor = System.Drawing.Color.Black;
            this.btnExportToCSV.Location = new System.Drawing.Point(185, 12);
            this.btnExportToCSV.Name = "btnExportToCSV";
            this.btnExportToCSV.Size = new System.Drawing.Size(167, 40);
            this.btnExportToCSV.TabIndex = 10;
            this.btnExportToCSV.Text = "Экспорт в CSV";
            this.btnExportToCSV.UseVisualStyleBackColor = false;
            this.btnExportToCSV.Click += new System.EventHandler(this.btnExportToCSV_Click);
            // 
            // btnExport
            // 
            this.btnExport.BackColor = System.Drawing.Color.LightGreen;
            this.btnExport.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnExport.ForeColor = System.Drawing.Color.Black;
            this.btnExport.Location = new System.Drawing.Point(12, 12);
            this.btnExport.Name = "btnExport";
            this.btnExport.Size = new System.Drawing.Size(167, 40);
            this.btnExport.TabIndex = 9;
            this.btnExport.Text = "Экспорт рисунка";
            this.btnExport.UseVisualStyleBackColor = false;
            this.btnExport.Click += new System.EventHandler(this.btnExport_Click);
            // 
            // saveFileDialog
            // 
            this.saveFileDialog.Filter = "Рисунок PNG|*.png|Рисунок JPEG|*.jpg";
            this.saveFileDialog.RestoreDirectory = true;
            // 
            // exportToCSVDialog
            // 
            this.exportToCSVDialog.Filter = "Файл CSV|*.csv";
            this.exportToCSVDialog.RestoreDirectory = true;
            // 
            // exportToCoMTRADEDialog
            // 
            this.exportToCoMTRADEDialog.Filter = "Файл COMTRADE|*.dat";
            this.exportToCoMTRADEDialog.RestoreDirectory = true;
            // 
            // ViewChartForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1026, 679);
            this.Controls.Add(this.panel2);
            this.Controls.Add(this.panel1);
            this.Name = "ViewChartForm";
            this.ShowIcon = false;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Просмотр прерывания";
            this.Load += new System.EventHandler(this.ViewChartForm_Load);
            this.panel1.ResumeLayout(false);
            this.panel1.PerformLayout();
            this.panel2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.chart)).EndInit();
            this.panel3.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.Panel panel2;
        public System.Windows.Forms.Label lblCaption;
        public System.Windows.Forms.DataVisualization.Charting.Chart chart;
        private System.Windows.Forms.Panel panel3;
        private System.Windows.Forms.Button btnExport;
        private System.Windows.Forms.SaveFileDialog saveFileDialog;
        private System.Windows.Forms.Button btnExportToCSV;
        private System.Windows.Forms.SaveFileDialog exportToCSVDialog;
        private System.Windows.Forms.Button btnExportToCOMTRADE;
        private System.Windows.Forms.SaveFileDialog exportToCoMTRADEDialog;
    }
}