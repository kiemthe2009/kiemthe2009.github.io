using System;
using System.IO;
using System.Management;
using System.Security.AccessControl;
using System.Windows.Forms;

namespace PermissionFolder
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
            GetUsers();
            cb_systemRights.DataSource = Enum.GetValues(typeof(FileSystemRights));
            cb_accessControlType.DataSource = Enum.GetValues(typeof(AccessControlType));

          

        }
        public void GetUsers()
        {

            SelectQuery sQuery = new SelectQuery("Win32_UserAccount", "Domain='" + System.Environment.UserDomainName.ToString() + "'");

            try
            {
                ManagementObjectSearcher mSearcher = new ManagementObjectSearcher(sQuery);
                foreach (ManagementObject mObject in mSearcher.Get())
                {
                    cbUser.Items.Add(mObject["Name"]);
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.ToString());
            }
        }

        private void btn_browser_Click(object sender, EventArgs e)
        {
            FolderBrowserDialog myFolderBrowserDialog = new FolderBrowserDialog();
            myFolderBrowserDialog.ShowDialog();
            tbFolder.Text = myFolderBrowserDialog.SelectedPath.ToString();
        }

        private void btn_run_Click(object sender, EventArgs e)
        {
            DirectoryInfo myDirectoryInfo = new DirectoryInfo(tbFolder.Text);
            DirectorySecurity myDirectorySecurity = myDirectoryInfo.GetAccessControl();
            string User = Environment.UserDomainName + "\\" + cbUser.SelectedItem.ToString();
            var fileSystemRights = (FileSystemRights) Enum.Parse(typeof(FileSystemRights), cb_systemRights.Text);
            var accessControlType = (AccessControlType) Enum.Parse(typeof(AccessControlType), cb_accessControlType.Text);
            myDirectorySecurity.AddAccessRule(new FileSystemAccessRule(User, fileSystemRights, accessControlType));
            //myDirectorySecurity.AddAccessRule(new FileSystemAccessRule(User, FileSystemRights.FullControl, AccessControlType.Allow));
            myDirectoryInfo.SetAccessControl(myDirectorySecurity);

            MessageBox.Show("Permissions Altered Successfully");
        }
    }

}
