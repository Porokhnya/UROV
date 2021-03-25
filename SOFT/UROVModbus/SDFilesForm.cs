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
            mainForm.GetFilesList(folderName);

            //TODO: ТУТ ЗАПРАШИВАЕМ СОДЕРЖИМОЕ ФАЙЛА !!!

            //PushCommandToQueue(GET_PREFIX + "LS" + PARAM_DELIMITER + folderName, DummyAnswerReceiver, SetSDFolderReadingFlag);
        }

        private void treeViewSD_MouseDoubleClick(object sender, MouseEventArgs e)
        {
            //TODO: ТУТ ЗАПРАШИВАЕМ СОДЕРЖИМОЕ ФАЙЛА !!!

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
    }
}
