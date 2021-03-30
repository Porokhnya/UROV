using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace UROVModbus
{
    public partial class SDFilesForm : Form
    {
        private MainForm mainForm = null;

        public SDFilesForm(MainForm fm)
        {
            mainForm = fm;
            InitializeComponent();
        }

        private void SDFilesForm_Load(object sender, EventArgs e)
        {
            logDataGrid.Dock = DockStyle.Fill;
            plEthalonChart.Dock = DockStyle.Fill;
            plEmptySDWorkspace.Dock = DockStyle.Fill;

            plEmptySDWorkspace.BringToFront();
        }

        public TreeNode tempSDParentNode = null;

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
                folderName = 
                nt.FileName + "/" + folderName;
                parent = parent.Parent;
            }

            tempSDParentNode = wantedExpand;

            if(tg.Tag == SDNodeTags.TagFolderUninitedNode && folderName == "LOG")
            {
                // добавляем фейковый файл последнего срабатывания
                mainForm.AddFileToList("LAST_3.LOG", 1);
            }

            mainForm.GetFilesList(folderName);

            //TODO: ТУТ ЗАПРАШИВАЕМ СОДЕРЖИМОЕ ФАЙЛА !!!

            //PushCommandToQueue(GET_PREFIX + "LS" + PARAM_DELIMITER + folderName, DummyAnswerReceiver, SetSDFolderReadingFlag);
        }

        private void treeViewSD_MouseDoubleClick(object sender, MouseEventArgs e)
        {
            //TODO: ТУТ ЗАПРАШИВАЕМ СОДЕРЖИМОЕ ФАЙЛА !!!
            plEmptySDWorkspace.BringToFront();
            mainForm.RequestFile(treeViewSD.SelectedNode);
        }

        private void SDFilesForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            e.Cancel = true;
            Hide();
        }

        private void btnListSDFiles_Click(object sender, EventArgs e)
        {
            treeViewSD.Nodes.Clear();
            tempSDParentNode = null;
            mainForm.GetFilesList("/");
        }

        private void logDataGrid_CellContentClick(object sender, DataGridViewCellEventArgs e)
        {
            var senderGrid = (DataGridView)sender;

            if (senderGrid.Columns[e.ColumnIndex] is DataGridViewButtonColumn &&
                    e.RowIndex >= 0)
            {
                string stationID = "";
                string stationName = "";

                /*
                TreeNode child = treeView.SelectedNode;
                if (child != null && child.Tag is ArchiveTreeLogItemRecord)
                {
                    ArchiveTreeLogItemRecord ali = child.Tag as ArchiveTreeLogItemRecord;
                    ArchiveTreeRootItem atri = ali.Parent.Parent;
                    stationID = atri.GUID;
                    stationName = atri.GUID;
                    if (ControllerNames.Instance.Names.ContainsKey(stationID))
                        stationName = ControllerNames.Instance.Names[stationID];

                }
                */

                this.mainForm.ShowChart(senderGrid.Rows[e.RowIndex].Tag as InterruptRecord, stationID, stationName, true);
            }
        }

        private void logDataGrid_CellValueNeeded(object sender, DataGridViewCellValueEventArgs e)
        {
            DataGridView targetGrid = sender as DataGridView;

            int rowNumber = e.RowIndex;

            if (rowNumber >= targetGrid.RowCount)
                return;

            if (!mainForm.gridToListCollection.ContainsKey(targetGrid))
                return;

            LogInfo linf = mainForm.gridToListCollection[targetGrid];

            if (rowNumber >= linf.list.Count)
                return;

            InterruptRecord record = linf.list[rowNumber];

            DataGridViewRow row = targetGrid.Rows[rowNumber];
            row.Tag = record;

            row.DefaultCellStyle.BackColor = rowNumber % 2 == 0 ? Color.LightGray : Color.White;

            if (targetGrid.Columns[e.ColumnIndex].Name.StartsWith("Num"))
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
                if (linf.computeMotoresurcePercents)
                {
                    int resMax = 1;
                    switch (record.ChannelNumber)
                    {
                        //DEPRECATED: case 0:
                        default:
                            resMax = Convert.ToInt32(mainForm.nudMotoresourceMax1.Value);
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
    }
}
