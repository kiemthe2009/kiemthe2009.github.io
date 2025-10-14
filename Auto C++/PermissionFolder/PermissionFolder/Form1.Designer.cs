namespace PermissionFolder
{
    partial class Form1
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
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.tbFolder = new System.Windows.Forms.TextBox();
            this.btn_browser = new System.Windows.Forms.Button();
            this.cbUser = new System.Windows.Forms.ComboBox();
            this.btn_run = new System.Windows.Forms.Button();
            this.label3 = new System.Windows.Forms.Label();
            this.cb_systemRights = new System.Windows.Forms.ComboBox();
            this.label4 = new System.Windows.Forms.Label();
            this.cb_accessControlType = new System.Windows.Forms.ComboBox();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(41, 27);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(70, 13);
            this.label1.TabIndex = 0;
            this.label1.Text = "Folder Name:";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(44, 61);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(65, 13);
            this.label2.TabIndex = 1;
            this.label2.Text = "Select User:";
            // 
            // tbFolder
            // 
            this.tbFolder.Location = new System.Drawing.Point(117, 24);
            this.tbFolder.Name = "tbFolder";
            this.tbFolder.Size = new System.Drawing.Size(260, 20);
            this.tbFolder.TabIndex = 2;
            // 
            // btn_browser
            // 
            this.btn_browser.Location = new System.Drawing.Point(383, 20);
            this.btn_browser.Name = "btn_browser";
            this.btn_browser.Size = new System.Drawing.Size(80, 27);
            this.btn_browser.TabIndex = 3;
            this.btn_browser.Text = "Browser...";
            this.btn_browser.UseVisualStyleBackColor = true;
            this.btn_browser.Click += new System.EventHandler(this.btn_browser_Click);
            // 
            // cbUser
            // 
            this.cbUser.FormattingEnabled = true;
            this.cbUser.Location = new System.Drawing.Point(117, 53);
            this.cbUser.Name = "cbUser";
            this.cbUser.Size = new System.Drawing.Size(346, 21);
            this.cbUser.TabIndex = 4;
            // 
            // btn_run
            // 
            this.btn_run.Location = new System.Drawing.Point(383, 148);
            this.btn_run.Name = "btn_run";
            this.btn_run.Size = new System.Drawing.Size(80, 27);
            this.btn_run.TabIndex = 5;
            this.btn_run.Text = "Run";
            this.btn_run.UseVisualStyleBackColor = true;
            this.btn_run.Click += new System.EventHandler(this.btn_run_Click);
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(15, 88);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(96, 13);
            this.label3.TabIndex = 6;
            this.label3.Text = "File System Rights:";
            // 
            // cb_systemRights
            // 
            this.cb_systemRights.FormattingEnabled = true;
            this.cb_systemRights.Location = new System.Drawing.Point(117, 85);
            this.cb_systemRights.Name = "cb_systemRights";
            this.cb_systemRights.Size = new System.Drawing.Size(346, 21);
            this.cb_systemRights.TabIndex = 7;
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(3, 119);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(108, 13);
            this.label4.TabIndex = 8;
            this.label4.Text = "Access Control Type:";
            // 
            // cb_accessControlType
            // 
            this.cb_accessControlType.FormattingEnabled = true;
            this.cb_accessControlType.Location = new System.Drawing.Point(117, 116);
            this.cb_accessControlType.Name = "cb_accessControlType";
            this.cb_accessControlType.Size = new System.Drawing.Size(346, 21);
            this.cb_accessControlType.TabIndex = 9;
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(524, 196);
            this.Controls.Add(this.cb_accessControlType);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.cb_systemRights);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.btn_run);
            this.Controls.Add(this.cbUser);
            this.Controls.Add(this.btn_browser);
            this.Controls.Add(this.tbFolder);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label1);
            this.Name = "Form1";
            this.Text = "[C#] Permission Folder - https://laptrinhvb.net";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox tbFolder;
        private System.Windows.Forms.Button btn_browser;
        private System.Windows.Forms.ComboBox cbUser;
        private System.Windows.Forms.Button btn_run;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.ComboBox cb_systemRights;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.ComboBox cb_accessControlType;
    }
}

