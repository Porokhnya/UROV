namespace UROVConfig
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
            System.Windows.Forms.DataVisualization.Charting.Legend legend1 = new System.Windows.Forms.DataVisualization.Charting.Legend();
            System.Windows.Forms.DataVisualization.Charting.Series series1 = new System.Windows.Forms.DataVisualization.Charting.Series();
            System.Windows.Forms.DataVisualization.Charting.Series series2 = new System.Windows.Forms.DataVisualization.Charting.Series();
            this.panel1 = new System.Windows.Forms.Panel();
            this.lblCaption = new System.Windows.Forms.Label();
            this.panel2 = new System.Windows.Forms.Panel();
            this.chart = new System.Windows.Forms.DataVisualization.Charting.Chart();
            this.panel3 = new System.Windows.Forms.Panel();
            this.btnExportToCSV = new System.Windows.Forms.Button();
            this.btnExport = new System.Windows.Forms.Button();
            this.saveFileDialog = new System.Windows.Forms.SaveFileDialog();
            this.exportToCSVDialog = new System.Windows.Forms.SaveFileDialog();
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
            this.panel1.Size = new System.Drawing.Size(870, 40);
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
            this.panel2.Size = new System.Drawing.Size(870, 548);
            this.panel2.TabIndex = 1;
            // 
            // chart
            // 
            chartArea1.Name = "ethalonChartArea";
            this.chart.ChartAreas.Add(chartArea1);
            this.chart.Dock = System.Windows.Forms.DockStyle.Fill;
            legend1.Docking = System.Windows.Forms.DataVisualization.Charting.Docking.Bottom;
            legend1.LegendStyle = System.Windows.Forms.DataVisualization.Charting.LegendStyle.Row;
            legend1.Name = "Legend1";
            this.chart.Legends.Add(legend1);
            this.chart.Location = new System.Drawing.Point(0, 0);
            this.chart.Name = "chart";
            series1.BorderWidth = 6;
            series1.ChartArea = "ethalonChartArea";
            series1.ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Spline;
            series1.Color = System.Drawing.Color.LightGreen;
            series1.Legend = "Legend1";
            series1.LegendText = "Эталон";
            series1.Name = "serie1";
            series2.BorderWidth = 2;
            series2.ChartArea = "ethalonChartArea";
            series2.ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Spline;
            series2.Color = System.Drawing.Color.SteelBlue;
            series2.Legend = "Legend1";
            series2.LegendText = "Прерывание";
            series2.Name = "serie2";
            this.chart.Series.Add(series1);
            this.chart.Series.Add(series2);
            this.chart.Size = new System.Drawing.Size(870, 484);
            this.chart.TabIndex = 3;
            this.chart.Text = "chart1";
            // 
            // panel3
            // 
            this.panel3.Controls.Add(this.btnExportToCSV);
            this.panel3.Controls.Add(this.btnExport);
            this.panel3.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.panel3.Location = new System.Drawing.Point(0, 484);
            this.panel3.Name = "panel3";
            this.panel3.Size = new System.Drawing.Size(870, 64);
            this.panel3.TabIndex = 2;
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
            // ViewChartForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(870, 588);
            this.Controls.Add(this.panel2);
            this.Controls.Add(this.panel1);
            this.Name = "ViewChartForm";
            this.ShowIcon = false;
            this.ShowInTaskbar = false;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Просмотр прерывания";
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
    }
}