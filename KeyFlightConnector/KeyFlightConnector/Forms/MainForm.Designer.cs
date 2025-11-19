namespace KeyFlightConnector.Forms
{
    partial class MainForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        private void InitializeComponent()
        {
            this.menuStrip = new System.Windows.Forms.MenuStrip();
            this.fileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.menuFileNew = new System.Windows.Forms.ToolStripMenuItem();
            this.menuFileOpen = new System.Windows.Forms.ToolStripMenuItem();
            this.menuFileSave = new System.Windows.Forms.ToolStripMenuItem();
            this.menuFileSaveAs = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
            this.menuFileExit = new System.Windows.Forms.ToolStripMenuItem();
            this.helpToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.menuHelpAbout = new System.Windows.Forms.ToolStripMenuItem();
            this.groupBoxConnection = new System.Windows.Forms.GroupBox();
            this.buttonRefreshPorts = new System.Windows.Forms.Button();
            this.labelConnectionStatus = new System.Windows.Forms.Label();
            this.buttonConnect = new System.Windows.Forms.Button();
            this.comboBoxPorts = new System.Windows.Forms.ComboBox();
            this.labelPort = new System.Windows.Forms.Label();
            this.groupBoxModules = new System.Windows.Forms.GroupBox();
            this.labelModuleCount = new System.Windows.Forms.Label();
            this.buttonRemoveInput = new System.Windows.Forms.Button();
            this.buttonAddInput = new System.Windows.Forms.Button();
            this.listViewModules = new System.Windows.Forms.ListView();
            this.columnId = new System.Windows.Forms.ColumnHeader();
            this.columnName = new System.Windows.Forms.ColumnHeader();
            this.columnType = new System.Windows.Forms.ColumnHeader();
            this.columnPin = new System.Windows.Forms.ColumnHeader();
            this.columnCommand = new System.Windows.Forms.ColumnHeader();
            this.columnEnabled = new System.Windows.Forms.ColumnHeader();
            this.buttonUploadConfig = new System.Windows.Forms.Button();
            this.groupBoxLog = new System.Windows.Forms.GroupBox();
            this.textBoxLog = new System.Windows.Forms.TextBox();

            this.menuStrip.SuspendLayout();
            this.groupBoxConnection.SuspendLayout();
            this.groupBoxModules.SuspendLayout();
            this.groupBoxLog.SuspendLayout();
            this.SuspendLayout();

            // menuStrip
            this.menuStrip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
                this.fileToolStripMenuItem,
                this.helpToolStripMenuItem});
            this.menuStrip.Location = new System.Drawing.Point(0, 0);
            this.menuStrip.Name = "menuStrip";
            this.menuStrip.Size = new System.Drawing.Size(1000, 24);
            this.menuStrip.TabIndex = 0;

            // fileToolStripMenuItem
            this.fileToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
                this.menuFileNew,
                this.menuFileOpen,
                this.menuFileSave,
                this.menuFileSaveAs,
                this.toolStripSeparator1,
                this.menuFileExit});
            this.fileToolStripMenuItem.Name = "fileToolStripMenuItem";
            this.fileToolStripMenuItem.Size = new System.Drawing.Size(37, 20);
            this.fileToolStripMenuItem.Text = "&File";

            // menuFileNew
            this.menuFileNew.Name = "menuFileNew";
            this.menuFileNew.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.N)));
            this.menuFileNew.Size = new System.Drawing.Size(180, 22);
            this.menuFileNew.Text = "&New";
            this.menuFileNew.Click += new System.EventHandler(this.MenuFileNew_Click);

            // menuFileOpen
            this.menuFileOpen.Name = "menuFileOpen";
            this.menuFileOpen.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.O)));
            this.menuFileOpen.Size = new System.Drawing.Size(180, 22);
            this.menuFileOpen.Text = "&Open...";
            this.menuFileOpen.Click += new System.EventHandler(this.MenuFileOpen_Click);

            // menuFileSave
            this.menuFileSave.Name = "menuFileSave";
            this.menuFileSave.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.S)));
            this.menuFileSave.Size = new System.Drawing.Size(180, 22);
            this.menuFileSave.Text = "&Save";
            this.menuFileSave.Click += new System.EventHandler(this.MenuFileSave_Click);

            // menuFileSaveAs
            this.menuFileSaveAs.Name = "menuFileSaveAs";
            this.menuFileSaveAs.Size = new System.Drawing.Size(180, 22);
            this.menuFileSaveAs.Text = "Save &As...";
            this.menuFileSaveAs.Click += new System.EventHandler(this.MenuFileSaveAs_Click);

            // toolStripSeparator1
            this.toolStripSeparator1.Name = "toolStripSeparator1";
            this.toolStripSeparator1.Size = new System.Drawing.Size(177, 6);

            // menuFileExit
            this.menuFileExit.Name = "menuFileExit";
            this.menuFileExit.Size = new System.Drawing.Size(180, 22);
            this.menuFileExit.Text = "E&xit";
            this.menuFileExit.Click += new System.EventHandler(this.MenuFileExit_Click);

            // helpToolStripMenuItem
            this.helpToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
                this.menuHelpAbout});
            this.helpToolStripMenuItem.Name = "helpToolStripMenuItem";
            this.helpToolStripMenuItem.Size = new System.Drawing.Size(44, 20);
            this.helpToolStripMenuItem.Text = "&Help";

            // menuHelpAbout
            this.menuHelpAbout.Name = "menuHelpAbout";
            this.menuHelpAbout.Size = new System.Drawing.Size(180, 22);
            this.menuHelpAbout.Text = "&About";
            this.menuHelpAbout.Click += new System.EventHandler(this.MenuHelpAbout_Click);

            // groupBoxConnection
            this.groupBoxConnection.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBoxConnection.Controls.Add(this.buttonRefreshPorts);
            this.groupBoxConnection.Controls.Add(this.labelConnectionStatus);
            this.groupBoxConnection.Controls.Add(this.buttonConnect);
            this.groupBoxConnection.Controls.Add(this.comboBoxPorts);
            this.groupBoxConnection.Controls.Add(this.labelPort);
            this.groupBoxConnection.Location = new System.Drawing.Point(12, 27);
            this.groupBoxConnection.Name = "groupBoxConnection";
            this.groupBoxConnection.Size = new System.Drawing.Size(976, 80);
            this.groupBoxConnection.TabIndex = 1;
            this.groupBoxConnection.TabStop = false;
            this.groupBoxConnection.Text = "Device Connection";

            // labelPort
            this.labelPort.AutoSize = true;
            this.labelPort.Location = new System.Drawing.Point(15, 25);
            this.labelPort.Name = "labelPort";
            this.labelPort.Size = new System.Drawing.Size(61, 15);
            this.labelPort.TabIndex = 0;
            this.labelPort.Text = "COM Port:";

            // comboBoxPorts
            this.comboBoxPorts.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboBoxPorts.FormattingEnabled = true;
            this.comboBoxPorts.Location = new System.Drawing.Point(82, 22);
            this.comboBoxPorts.Name = "comboBoxPorts";
            this.comboBoxPorts.Size = new System.Drawing.Size(121, 23);
            this.comboBoxPorts.TabIndex = 1;

            // buttonRefreshPorts
            this.buttonRefreshPorts.Location = new System.Drawing.Point(209, 21);
            this.buttonRefreshPorts.Name = "buttonRefreshPorts";
            this.buttonRefreshPorts.Size = new System.Drawing.Size(75, 23);
            this.buttonRefreshPorts.TabIndex = 2;
            this.buttonRefreshPorts.Text = "Refresh";
            this.buttonRefreshPorts.UseVisualStyleBackColor = true;
            this.buttonRefreshPorts.Click += new System.EventHandler(this.ButtonRefreshPorts_Click);

            // buttonConnect
            this.buttonConnect.Location = new System.Drawing.Point(290, 21);
            this.buttonConnect.Name = "buttonConnect";
            this.buttonConnect.Size = new System.Drawing.Size(100, 23);
            this.buttonConnect.TabIndex = 3;
            this.buttonConnect.Text = "Connect";
            this.buttonConnect.UseVisualStyleBackColor = true;
            this.buttonConnect.Click += new System.EventHandler(this.ButtonConnect_Click);

            // labelConnectionStatus
            this.labelConnectionStatus.AutoSize = true;
            this.labelConnectionStatus.Font = new System.Drawing.Font("Segoe UI", 9F, System.Drawing.FontStyle.Bold);
            this.labelConnectionStatus.ForeColor = System.Drawing.Color.Red;
            this.labelConnectionStatus.Location = new System.Drawing.Point(15, 52);
            this.labelConnectionStatus.Name = "labelConnectionStatus";
            this.labelConnectionStatus.Size = new System.Drawing.Size(89, 15);
            this.labelConnectionStatus.TabIndex = 4;
            this.labelConnectionStatus.Text = "Not Connected";

            // groupBoxModules
            this.groupBoxModules.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) | System.Windows.Forms.AnchorStyles.Left) | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBoxModules.Controls.Add(this.labelModuleCount);
            this.groupBoxModules.Controls.Add(this.buttonRemoveInput);
            this.groupBoxModules.Controls.Add(this.buttonAddInput);
            this.groupBoxModules.Controls.Add(this.listViewModules);
            this.groupBoxModules.Location = new System.Drawing.Point(12, 113);
            this.groupBoxModules.Name = "groupBoxModules";
            this.groupBoxModules.Size = new System.Drawing.Size(976, 350);
            this.groupBoxModules.TabIndex = 2;
            this.groupBoxModules.TabStop = false;
            this.groupBoxModules.Text = "Input Modules";

            // listViewModules
            this.listViewModules.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) | System.Windows.Forms.AnchorStyles.Left) | System.Windows.Forms.AnchorStyles.Right)));
            this.listViewModules.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
                this.columnId,
                this.columnName,
                this.columnType,
                this.columnPin,
                this.columnCommand,
                this.columnEnabled});
            this.listViewModules.FullRowSelect = true;
            this.listViewModules.GridLines = true;
            this.listViewModules.Location = new System.Drawing.Point(15, 22);
            this.listViewModules.Name = "listViewModules";
            this.listViewModules.Size = new System.Drawing.Size(945, 280);
            this.listViewModules.TabIndex = 0;
            this.listViewModules.UseCompatibleStateImageBehavior = false;
            this.listViewModules.View = System.Windows.Forms.View.Details;

            // columnId
            this.columnId.Text = "ID";
            this.columnId.Width = 40;

            // columnName
            this.columnName.Text = "Name";
            this.columnName.Width = 200;

            // columnType
            this.columnType.Text = "Type";
            this.columnType.Width = 100;

            // columnPin
            this.columnPin.Text = "Pin";
            this.columnPin.Width = 60;

            // columnCommand
            this.columnCommand.Text = "Keyboard Command";
            this.columnCommand.Width = 300;

            // columnEnabled
            this.columnEnabled.Text = "Enabled";
            this.columnEnabled.Width = 80;

            // buttonAddInput
            this.buttonAddInput.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.buttonAddInput.Location = new System.Drawing.Point(15, 315);
            this.buttonAddInput.Name = "buttonAddInput";
            this.buttonAddInput.Size = new System.Drawing.Size(100, 23);
            this.buttonAddInput.TabIndex = 1;
            this.buttonAddInput.Text = "Add Input";
            this.buttonAddInput.UseVisualStyleBackColor = true;
            this.buttonAddInput.Click += new System.EventHandler(this.ButtonAddInput_Click);

            // buttonRemoveInput
            this.buttonRemoveInput.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.buttonRemoveInput.Location = new System.Drawing.Point(121, 315);
            this.buttonRemoveInput.Name = "buttonRemoveInput";
            this.buttonRemoveInput.Size = new System.Drawing.Size(100, 23);
            this.buttonRemoveInput.TabIndex = 2;
            this.buttonRemoveInput.Text = "Remove Input";
            this.buttonRemoveInput.UseVisualStyleBackColor = true;
            this.buttonRemoveInput.Click += new System.EventHandler(this.ButtonRemoveInput_Click);

            // labelModuleCount
            this.labelModuleCount.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.labelModuleCount.Location = new System.Drawing.Point(810, 319);
            this.labelModuleCount.Name = "labelModuleCount";
            this.labelModuleCount.Size = new System.Drawing.Size(150, 15);
            this.labelModuleCount.TabIndex = 3;
            this.labelModuleCount.Text = "0 / 40 modules";
            this.labelModuleCount.TextAlign = System.Drawing.ContentAlignment.TopRight;

            // buttonUploadConfig
            this.buttonUploadConfig.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.buttonUploadConfig.Enabled = false;
            this.buttonUploadConfig.Font = new System.Drawing.Font("Segoe UI", 9F, System.Drawing.FontStyle.Bold);
            this.buttonUploadConfig.Location = new System.Drawing.Point(820, 469);
            this.buttonUploadConfig.Name = "buttonUploadConfig";
            this.buttonUploadConfig.Size = new System.Drawing.Size(168, 35);
            this.buttonUploadConfig.TabIndex = 3;
            this.buttonUploadConfig.Text = "Upload to Device";
            this.buttonUploadConfig.UseVisualStyleBackColor = true;
            this.buttonUploadConfig.Click += new System.EventHandler(this.ButtonUploadConfig_Click);

            // groupBoxLog
            this.groupBoxLog.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left) | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBoxLog.Controls.Add(this.textBoxLog);
            this.groupBoxLog.Location = new System.Drawing.Point(12, 510);
            this.groupBoxLog.Name = "groupBoxLog";
            this.groupBoxLog.Size = new System.Drawing.Size(976, 150);
            this.groupBoxLog.TabIndex = 4;
            this.groupBoxLog.TabStop = false;
            this.groupBoxLog.Text = "Log";

            // textBoxLog
            this.textBoxLog.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) | System.Windows.Forms.AnchorStyles.Left) | System.Windows.Forms.AnchorStyles.Right)));
            this.textBoxLog.BackColor = System.Drawing.Color.Black;
            this.textBoxLog.Font = new System.Drawing.Font("Consolas", 9F);
            this.textBoxLog.ForeColor = System.Drawing.Color.Lime;
            this.textBoxLog.Location = new System.Drawing.Point(15, 22);
            this.textBoxLog.Multiline = true;
            this.textBoxLog.Name = "textBoxLog";
            this.textBoxLog.ReadOnly = true;
            this.textBoxLog.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.textBoxLog.Size = new System.Drawing.Size(945, 110);
            this.textBoxLog.TabIndex = 0;

            // MainForm
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 15F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1000, 672);
            this.Controls.Add(this.groupBoxLog);
            this.Controls.Add(this.buttonUploadConfig);
            this.Controls.Add(this.groupBoxModules);
            this.Controls.Add(this.groupBoxConnection);
            this.Controls.Add(this.menuStrip);
            this.MainMenuStrip = this.menuStrip;
            this.MinimumSize = new System.Drawing.Size(800, 600);
            this.Name = "MainForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "KeyFlight Connector v0.1.0";

            this.menuStrip.ResumeLayout(false);
            this.menuStrip.PerformLayout();
            this.groupBoxConnection.ResumeLayout(false);
            this.groupBoxConnection.PerformLayout();
            this.groupBoxModules.ResumeLayout(false);
            this.groupBoxLog.ResumeLayout(false);
            this.groupBoxLog.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();
        }

        #endregion

        private System.Windows.Forms.MenuStrip menuStrip;
        private System.Windows.Forms.ToolStripMenuItem fileToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem menuFileNew;
        private System.Windows.Forms.ToolStripMenuItem menuFileOpen;
        private System.Windows.Forms.ToolStripMenuItem menuFileSave;
        private System.Windows.Forms.ToolStripMenuItem menuFileSaveAs;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
        private System.Windows.Forms.ToolStripMenuItem menuFileExit;
        private System.Windows.Forms.ToolStripMenuItem helpToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem menuHelpAbout;
        private System.Windows.Forms.GroupBox groupBoxConnection;
        private System.Windows.Forms.Button buttonRefreshPorts;
        private System.Windows.Forms.Label labelConnectionStatus;
        private System.Windows.Forms.Button buttonConnect;
        private System.Windows.Forms.ComboBox comboBoxPorts;
        private System.Windows.Forms.Label labelPort;
        private System.Windows.Forms.GroupBox groupBoxModules;
        private System.Windows.Forms.Label labelModuleCount;
        private System.Windows.Forms.Button buttonRemoveInput;
        private System.Windows.Forms.Button buttonAddInput;
        private System.Windows.Forms.ListView listViewModules;
        private System.Windows.Forms.ColumnHeader columnId;
        private System.Windows.Forms.ColumnHeader columnName;
        private System.Windows.Forms.ColumnHeader columnType;
        private System.Windows.Forms.ColumnHeader columnPin;
        private System.Windows.Forms.ColumnHeader columnCommand;
        private System.Windows.Forms.ColumnHeader columnEnabled;
        private System.Windows.Forms.Button buttonUploadConfig;
        private System.Windows.Forms.GroupBox groupBoxLog;
        private System.Windows.Forms.TextBox textBoxLog;
    }
}
