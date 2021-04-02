namespace UROVModbus
{
    partial class SDFilesForm
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
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(SDFilesForm));
            System.Windows.Forms.DataVisualization.Charting.ChartArea chartArea3 = new System.Windows.Forms.DataVisualization.Charting.ChartArea();
            System.Windows.Forms.DataVisualization.Charting.Legend legend3 = new System.Windows.Forms.DataVisualization.Charting.Legend();
            System.Windows.Forms.DataVisualization.Charting.LegendItem legendItem3 = new System.Windows.Forms.DataVisualization.Charting.LegendItem();
            System.Windows.Forms.DataVisualization.Charting.LegendCell legendCell3 = new System.Windows.Forms.DataVisualization.Charting.LegendCell();
            System.Windows.Forms.DataVisualization.Charting.Series series3 = new System.Windows.Forms.DataVisualization.Charting.Series();
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle9 = new System.Windows.Forms.DataGridViewCellStyle();
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle10 = new System.Windows.Forms.DataGridViewCellStyle();
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle11 = new System.Windows.Forms.DataGridViewCellStyle();
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle12 = new System.Windows.Forms.DataGridViewCellStyle();
            this.statusStrip = new System.Windows.Forms.StatusStrip();
            this.toolStripProgressBar1 = new System.Windows.Forms.ToolStripProgressBar();
            this.toolStripStatusLabel1 = new System.Windows.Forms.ToolStripStatusLabel();
            this.plSDSettings = new System.Windows.Forms.Panel();
            this.splitContainer3 = new System.Windows.Forms.SplitContainer();
            this.treeViewSD = new System.Windows.Forms.TreeView();
            this.sdImagesNormal = new System.Windows.Forms.ImageList(this.components);
            this.plEthalonChart = new System.Windows.Forms.Panel();
            this.ethalonChart = new System.Windows.Forms.DataVisualization.Charting.Chart();
            this.logDataGrid = new System.Windows.Forms.DataGridView();
            this.Num = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.Time = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.Temp = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.Motoresource = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.Channel = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.Pulses = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.Rod = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.Etl = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.Compare = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.Btn = new System.Windows.Forms.DataGridViewButtonColumn();
            this.plEmptySDWorkspace = new System.Windows.Forms.Panel();
            this.toolStripSD = new System.Windows.Forms.ToolStrip();
            this.btnViewSDFile = new System.Windows.Forms.ToolStripButton();
            this.btnDeleteSDFile = new System.Windows.Forms.ToolStripButton();
            this.statusStrip.SuspendLayout();
            this.plSDSettings.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer3)).BeginInit();
            this.splitContainer3.Panel1.SuspendLayout();
            this.splitContainer3.Panel2.SuspendLayout();
            this.splitContainer3.SuspendLayout();
            this.plEthalonChart.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.ethalonChart)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.logDataGrid)).BeginInit();
            this.toolStripSD.SuspendLayout();
            this.SuspendLayout();
            // 
            // statusStrip
            // 
            this.statusStrip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripProgressBar1,
            this.toolStripStatusLabel1});
            this.statusStrip.Location = new System.Drawing.Point(0, 489);
            this.statusStrip.Name = "statusStrip";
            this.statusStrip.Size = new System.Drawing.Size(851, 22);
            this.statusStrip.TabIndex = 0;
            this.statusStrip.Text = "statusStrip1";
            // 
            // toolStripProgressBar1
            // 
            this.toolStripProgressBar1.Name = "toolStripProgressBar1";
            this.toolStripProgressBar1.Size = new System.Drawing.Size(100, 16);
            this.toolStripProgressBar1.Visible = false;
            // 
            // toolStripStatusLabel1
            // 
            this.toolStripStatusLabel1.Name = "toolStripStatusLabel1";
            this.toolStripStatusLabel1.Size = new System.Drawing.Size(79, 17);
            this.toolStripStatusLabel1.Text = "                        ";
            // 
            // plSDSettings
            // 
            this.plSDSettings.Controls.Add(this.splitContainer3);
            this.plSDSettings.Controls.Add(this.toolStripSD);
            this.plSDSettings.Dock = System.Windows.Forms.DockStyle.Fill;
            this.plSDSettings.Location = new System.Drawing.Point(0, 0);
            this.plSDSettings.Name = "plSDSettings";
            this.plSDSettings.Size = new System.Drawing.Size(851, 489);
            this.plSDSettings.TabIndex = 7;
            // 
            // splitContainer3
            // 
            this.splitContainer3.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer3.Location = new System.Drawing.Point(0, 25);
            this.splitContainer3.Name = "splitContainer3";
            // 
            // splitContainer3.Panel1
            // 
            this.splitContainer3.Panel1.Controls.Add(this.treeViewSD);
            this.splitContainer3.Panel1MinSize = 100;
            // 
            // splitContainer3.Panel2
            // 
            this.splitContainer3.Panel2.Controls.Add(this.plEthalonChart);
            this.splitContainer3.Panel2.Controls.Add(this.logDataGrid);
            this.splitContainer3.Panel2.Controls.Add(this.plEmptySDWorkspace);
            this.splitContainer3.Panel2MinSize = 100;
            this.splitContainer3.Size = new System.Drawing.Size(851, 464);
            this.splitContainer3.SplitterDistance = 200;
            this.splitContainer3.TabIndex = 5;
            // 
            // treeViewSD
            // 
            this.treeViewSD.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.treeViewSD.Dock = System.Windows.Forms.DockStyle.Fill;
            this.treeViewSD.HideSelection = false;
            this.treeViewSD.ImageIndex = 2;
            this.treeViewSD.ImageList = this.sdImagesNormal;
            this.treeViewSD.Location = new System.Drawing.Point(0, 0);
            this.treeViewSD.Name = "treeViewSD";
            this.treeViewSD.SelectedImageIndex = 0;
            this.treeViewSD.Size = new System.Drawing.Size(200, 464);
            this.treeViewSD.TabIndex = 1;
            this.treeViewSD.BeforeExpand += new System.Windows.Forms.TreeViewCancelEventHandler(this.treeViewSD_BeforeExpand);
            this.treeViewSD.AfterSelect += new System.Windows.Forms.TreeViewEventHandler(this.treeViewSD_AfterSelect);
            this.treeViewSD.MouseDoubleClick += new System.Windows.Forms.MouseEventHandler(this.treeViewSD_MouseDoubleClick);
            // 
            // sdImagesNormal
            // 
            this.sdImagesNormal.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("sdImagesNormal.ImageStream")));
            this.sdImagesNormal.TransparentColor = System.Drawing.Color.Transparent;
            this.sdImagesNormal.Images.SetKeyName(0, "folder-blue.png");
            this.sdImagesNormal.Images.SetKeyName(1, "document-open-folder.png");
            this.sdImagesNormal.Images.SetKeyName(2, "list.png");
            // 
            // plEthalonChart
            // 
            this.plEthalonChart.Controls.Add(this.ethalonChart);
            this.plEthalonChart.Location = new System.Drawing.Point(331, 70);
            this.plEthalonChart.Name = "plEthalonChart";
            this.plEthalonChart.Size = new System.Drawing.Size(200, 193);
            this.plEthalonChart.TabIndex = 4;
            // 
            // ethalonChart
            // 
            chartArea3.Name = "ethalonChartArea";
            this.ethalonChart.ChartAreas.Add(chartArea3);
            this.ethalonChart.Dock = System.Windows.Forms.DockStyle.Fill;
            legendCell3.Name = "PulsesCount";
            legendItem3.Cells.Add(legendCell3);
            legend3.CustomItems.Add(legendItem3);
            legend3.Docking = System.Windows.Forms.DataVisualization.Charting.Docking.Bottom;
            legend3.Name = "Legend1";
            this.ethalonChart.Legends.Add(legend3);
            this.ethalonChart.Location = new System.Drawing.Point(0, 0);
            this.ethalonChart.Name = "ethalonChart";
            series3.ChartArea = "ethalonChartArea";
            series3.ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Spline;
            series3.IsVisibleInLegend = false;
            series3.Legend = "Legend1";
            series3.Name = "ethalonChartSerie";
            this.ethalonChart.Series.Add(series3);
            this.ethalonChart.Size = new System.Drawing.Size(200, 193);
            this.ethalonChart.TabIndex = 0;
            this.ethalonChart.Text = "chart1";
            // 
            // logDataGrid
            // 
            this.logDataGrid.AllowUserToAddRows = false;
            this.logDataGrid.AllowUserToDeleteRows = false;
            this.logDataGrid.AllowUserToResizeRows = false;
            this.logDataGrid.BackgroundColor = System.Drawing.Color.White;
            this.logDataGrid.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.logDataGrid.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.Num,
            this.Time,
            this.Temp,
            this.Motoresource,
            this.Channel,
            this.Pulses,
            this.Rod,
            this.Etl,
            this.Compare,
            this.Btn});
            this.logDataGrid.EditMode = System.Windows.Forms.DataGridViewEditMode.EditProgrammatically;
            this.logDataGrid.Location = new System.Drawing.Point(17, 20);
            this.logDataGrid.MultiSelect = false;
            this.logDataGrid.Name = "logDataGrid";
            this.logDataGrid.ReadOnly = true;
            this.logDataGrid.RowHeadersVisible = false;
            this.logDataGrid.SelectionMode = System.Windows.Forms.DataGridViewSelectionMode.FullRowSelect;
            this.logDataGrid.ShowEditingIcon = false;
            this.logDataGrid.Size = new System.Drawing.Size(240, 150);
            this.logDataGrid.TabIndex = 3;
            this.logDataGrid.VirtualMode = true;
            this.logDataGrid.CellContentClick += new System.Windows.Forms.DataGridViewCellEventHandler(this.logDataGrid_CellContentClick);
            this.logDataGrid.CellValueNeeded += new System.Windows.Forms.DataGridViewCellValueEventHandler(this.logDataGrid_CellValueNeeded);
            // 
            // Num
            // 
            this.Num.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.AllCells;
            dataGridViewCellStyle9.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleRight;
            this.Num.DefaultCellStyle = dataGridViewCellStyle9;
            this.Num.HeaderText = "№";
            this.Num.Name = "Num";
            this.Num.ReadOnly = true;
            this.Num.Width = 43;
            // 
            // Time
            // 
            this.Time.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.Fill;
            this.Time.HeaderText = "Время";
            this.Time.Name = "Time";
            this.Time.ReadOnly = true;
            // 
            // Temp
            // 
            dataGridViewCellStyle10.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleCenter;
            this.Temp.DefaultCellStyle = dataGridViewCellStyle10;
            this.Temp.HeaderText = "Температура";
            this.Temp.Name = "Temp";
            this.Temp.ReadOnly = true;
            this.Temp.Width = 80;
            // 
            // Motoresource
            // 
            this.Motoresource.HeaderText = "Срабатываний";
            this.Motoresource.Name = "Motoresource";
            this.Motoresource.ReadOnly = true;
            this.Motoresource.Width = 80;
            // 
            // Channel
            // 
            dataGridViewCellStyle11.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleCenter;
            this.Channel.DefaultCellStyle = dataGridViewCellStyle11;
            this.Channel.HeaderText = "Канал";
            this.Channel.Name = "Channel";
            this.Channel.ReadOnly = true;
            this.Channel.Width = 80;
            // 
            // Pulses
            // 
            dataGridViewCellStyle12.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleCenter;
            this.Pulses.DefaultCellStyle = dataGridViewCellStyle12;
            this.Pulses.HeaderText = "Импульсов";
            this.Pulses.Name = "Pulses";
            this.Pulses.ReadOnly = true;
            this.Pulses.Width = 80;
            // 
            // Rod
            // 
            this.Rod.HeaderText = "Штанга";
            this.Rod.Name = "Rod";
            this.Rod.ReadOnly = true;
            this.Rod.Width = 80;
            // 
            // Etl
            // 
            this.Etl.HeaderText = "Эталон";
            this.Etl.Name = "Etl";
            this.Etl.ReadOnly = true;
            // 
            // Compare
            // 
            this.Compare.HeaderText = "Сравнение";
            this.Compare.Name = "Compare";
            this.Compare.ReadOnly = true;
            // 
            // Btn
            // 
            this.Btn.HeaderText = "График";
            this.Btn.Name = "Btn";
            this.Btn.ReadOnly = true;
            // 
            // plEmptySDWorkspace
            // 
            this.plEmptySDWorkspace.Location = new System.Drawing.Point(81, 220);
            this.plEmptySDWorkspace.Name = "plEmptySDWorkspace";
            this.plEmptySDWorkspace.Size = new System.Drawing.Size(200, 100);
            this.plEmptySDWorkspace.TabIndex = 2;
            // 
            // toolStripSD
            // 
            this.toolStripSD.ImageScalingSize = new System.Drawing.Size(32, 32);
            this.toolStripSD.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.btnViewSDFile,
            this.btnDeleteSDFile});
            this.toolStripSD.Location = new System.Drawing.Point(0, 0);
            this.toolStripSD.Name = "toolStripSD";
            this.toolStripSD.Size = new System.Drawing.Size(851, 25);
            this.toolStripSD.TabIndex = 4;
            // 
            // btnViewSDFile
            // 
            this.btnViewSDFile.Enabled = false;
            this.btnViewSDFile.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.btnViewSDFile.Name = "btnViewSDFile";
            this.btnViewSDFile.Size = new System.Drawing.Size(68, 22);
            this.btnViewSDFile.Text = "Просмотр";
            this.btnViewSDFile.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText;
            this.btnViewSDFile.Click += new System.EventHandler(this.btnViewSDFile_Click);
            // 
            // btnDeleteSDFile
            // 
            this.btnDeleteSDFile.Enabled = false;
            this.btnDeleteSDFile.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.btnDeleteSDFile.Name = "btnDeleteSDFile";
            this.btnDeleteSDFile.Size = new System.Drawing.Size(55, 22);
            this.btnDeleteSDFile.Text = "Удалить";
            this.btnDeleteSDFile.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText;
            this.btnDeleteSDFile.ToolTipText = "Удалить выбранный файл";
            this.btnDeleteSDFile.Click += new System.EventHandler(this.btnDeleteSDFile_Click);
            // 
            // SDFilesForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(851, 511);
            this.Controls.Add(this.plSDSettings);
            this.Controls.Add(this.statusStrip);
            this.MinimizeBox = false;
            this.Name = "SDFilesForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Список файлов на SD";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.SDFilesForm_FormClosing);
            this.Load += new System.EventHandler(this.SDFilesForm_Load);
            this.statusStrip.ResumeLayout(false);
            this.statusStrip.PerformLayout();
            this.plSDSettings.ResumeLayout(false);
            this.plSDSettings.PerformLayout();
            this.splitContainer3.Panel1.ResumeLayout(false);
            this.splitContainer3.Panel2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer3)).EndInit();
            this.splitContainer3.ResumeLayout(false);
            this.plEthalonChart.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.ethalonChart)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.logDataGrid)).EndInit();
            this.toolStripSD.ResumeLayout(false);
            this.toolStripSD.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.StatusStrip statusStrip;
        private System.Windows.Forms.Panel plSDSettings;
        private System.Windows.Forms.SplitContainer splitContainer3;
        private System.Windows.Forms.DataGridViewTextBoxColumn Num;
        private System.Windows.Forms.DataGridViewTextBoxColumn Time;
        private System.Windows.Forms.DataGridViewTextBoxColumn Temp;
        private System.Windows.Forms.DataGridViewTextBoxColumn Motoresource;
        private System.Windows.Forms.DataGridViewTextBoxColumn Channel;
        private System.Windows.Forms.DataGridViewTextBoxColumn Pulses;
        private System.Windows.Forms.DataGridViewTextBoxColumn Rod;
        private System.Windows.Forms.DataGridViewTextBoxColumn Etl;
        private System.Windows.Forms.DataGridViewTextBoxColumn Compare;
        private System.Windows.Forms.DataGridViewButtonColumn Btn;
        private System.Windows.Forms.ToolStrip toolStripSD;
        private System.Windows.Forms.ToolStripButton btnViewSDFile;
        private System.Windows.Forms.ToolStripButton btnDeleteSDFile;
        private System.Windows.Forms.ImageList sdImagesNormal;
        public System.Windows.Forms.TreeView treeViewSD;
        public System.Windows.Forms.ToolStripStatusLabel toolStripStatusLabel1;
        public System.Windows.Forms.ToolStripProgressBar toolStripProgressBar1;
        public System.Windows.Forms.DataGridView logDataGrid;
        public System.Windows.Forms.Panel plEmptySDWorkspace;
        public System.Windows.Forms.Panel plEthalonChart;
        public System.Windows.Forms.DataVisualization.Charting.Chart ethalonChart;
    }
}