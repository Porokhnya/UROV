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
    public delegate void ConnectFormShown(ConnectForm frm);

    public partial class ConnectForm : Form
    {
        private bool hasNoActions = false;
        public ConnectForm(bool noActions)
        {
            InitializeComponent();
            firstTimeActivate = true;
            hasNoActions = noActions;
        }

        private bool firstTimeActivate = true;
        private MainForm mainForm = null;
        private string portToConnect;
        private bool withHandshake;
        private bool findDevice;

        public ConnectFormShown OnConnectFormShown = null;

        /// <summary>
        /// активания формы
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void ConnectForm_Activated(object sender, EventArgs e)
        {
            if (!firstTimeActivate)
                return;
            firstTimeActivate = false;

            if (!hasNoActions)
            {
                System.Diagnostics.Debug.WriteLine("CONNECT FORM: CALL MAIN FORM HANDLER!");
                this.mainForm.StartConnectToPort(this.portToConnect, this.withHandshake, this.findDevice);
            }

            if(OnConnectFormShown != null)
            {
                OnConnectFormShown(this);
            }
        }

        /// <summary>
        /// установка ссылки на главную форму и на настройки соединения
        /// </summary>
        /// <param name="fm"></param>
        /// <param name="port"></param>
        /// <param name="withHandshake"></param>
        /// <param name="findDevice"></param>
        public void SetMainFormAndPort(MainForm fm, string port, bool withHandshake, bool findDevice)
        {
            mainForm = fm;
            portToConnect = port;
            this.withHandshake = withHandshake;
            this.findDevice = findDevice;
        }
    }
}
