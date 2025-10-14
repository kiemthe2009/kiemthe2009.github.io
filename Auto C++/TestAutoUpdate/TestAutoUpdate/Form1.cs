using AutoUpdaterDotNET;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace TestAutoUpdate
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            System.Reflection.Assembly assembly = System.Reflection.Assembly.GetExecutingAssembly();
            System.Diagnostics.FileVersionInfo fvi = System.Diagnostics.FileVersionInfo.GetVersionInfo(assembly.Location);
            AutoUpdater.CheckForUpdateEvent += AutoUpdaterOnCheckForUpdateEvent;
            string version = fvi.FileVersion;
            label1.Text = "Phiên bản: " + version;
            AutoUpdater.DownloadPath = "update";
            System.Timers.Timer timer = new System.Timers.Timer
            {
                Interval = 15 * 60 * 1000,
                SynchronizingObject = this
            };
            timer.Elapsed += delegate
            {
                AutoUpdater.Start("http://laptrinhvb.net/update.xml");
            };
            timer.Start();
        }      

        private void AutoUpdaterOnCheckForUpdateEvent(UpdateInfoEventArgs args)
        {
                if (args.IsUpdateAvailable)
                {
                    DialogResult dialogResult;                  
                    dialogResult =
                            MessageBox.Show(@"Bạn ơi, phần mềm của bạn có phiên bản mới {args.CurrentVersion}. Phiên bản bạn đang sử dụng hiện tại  {args.InstalledVersion}. Bạn có muốn cập nhật phần mềm không?", @"Cập nhật phần mềm",
                                MessageBoxButtons.YesNo,
                                MessageBoxIcon.Information);
                     
                    if (dialogResult.Equals(DialogResult.Yes) || dialogResult.Equals(DialogResult.OK))
                    {
                        try
                        {
                            if (AutoUpdater.DownloadUpdate(args))
                            {
                                Application.Exit();
                            }
                        }
                        catch (Exception exception)
                        {
                            MessageBox.Show(exception.Message, exception.GetType().ToString(), MessageBoxButtons.OK,
                                MessageBoxIcon.Error);
                        }
                    }
                }
                else
                {
                    MessageBox.Show(@"Phiên bản bạn đang sử dụng đã được cập nhật mới nhất.", @"Cập nhật phần mềm",
                        MessageBoxButtons.OK, MessageBoxIcon.Information);
                }
            }

        private void button1_Click(object sender, EventArgs e)
        {
            AutoUpdater.Start("http://laptrinhvb.net/update.xml");
        }
    }
}
