namespace KeyFlightConnector.Forms
{
    partial class InputEditorForm
    {
        private System.ComponentModel.IContainer components = null;

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
            this.labelName = new System.Windows.Forms.Label();
            this.textBoxName = new System.Windows.Forms.TextBox();
            this.labelType = new System.Windows.Forms.Label();
            this.comboBoxType = new System.Windows.Forms.ComboBox();
            this.labelPin = new System.Windows.Forms.Label();
            this.numericUpDownPin = new System.Windows.Forms.NumericUpDown();
            this.labelPin2 = new System.Windows.Forms.Label();
            this.numericUpDownPin2 = new System.Windows.Forms.NumericUpDown();
            this.labelKeyCommand = new System.Windows.Forms.Label();
            this.textBoxKeyCommand = new System.Windows.Forms.TextBox();
            this.labelKeyCommand2 = new System.Windows.Forms.Label();
            this.textBoxKeyCommand2 = new System.Windows.Forms.TextBox();
            this.labelSensitivity = new System.Windows.Forms.Label();
            this.comboBoxSensitivity = new System.Windows.Forms.ComboBox();
            this.checkBoxEnabled = new System.Windows.Forms.CheckBox();
            this.buttonOK = new System.Windows.Forms.Button();
            this.buttonCancel = new System.Windows.Forms.Button();
            this.buttonKeyBuilder = new System.Windows.Forms.Button();

            ((System.ComponentModel.ISupportInitialize)(this.numericUpDownPin)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDownPin2)).BeginInit();
            this.SuspendLayout();

            // labelName
            this.labelName.AutoSize = true;
            this.labelName.Location = new System.Drawing.Point(20, 20);
            this.labelName.Name = "labelName";
            this.labelName.Size = new System.Drawing.Size(42, 15);
            this.labelName.TabIndex = 0;
            this.labelName.Text = "Name:";

            // textBoxName
            this.textBoxName.Location = new System.Drawing.Point(120, 17);
            this.textBoxName.Name = "textBoxName";
            this.textBoxName.Size = new System.Drawing.Size(300, 23);
            this.textBoxName.TabIndex = 1;

            // labelType
            this.labelType.AutoSize = true;
            this.labelType.Location = new System.Drawing.Point(20, 55);
            this.labelType.Name = "labelType";
            this.labelType.Size = new System.Drawing.Size(34, 15);
            this.labelType.TabIndex = 2;
            this.labelType.Text = "Type:";

            // comboBoxType
            this.comboBoxType.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboBoxType.FormattingEnabled = true;
            this.comboBoxType.Location = new System.Drawing.Point(120, 52);
            this.comboBoxType.Name = "comboBoxType";
            this.comboBoxType.Size = new System.Drawing.Size(150, 23);
            this.comboBoxType.TabIndex = 3;
            this.comboBoxType.SelectedIndexChanged += new System.EventHandler(this.ComboBoxType_SelectedIndexChanged);

            // labelPin
            this.labelPin.AutoSize = true;
            this.labelPin.Location = new System.Drawing.Point(20, 90);
            this.labelPin.Name = "labelPin";
            this.labelPin.Size = new System.Drawing.Size(27, 15);
            this.labelPin.TabIndex = 4;
            this.labelPin.Text = "Pin:";

            // numericUpDownPin
            this.numericUpDownPin.Location = new System.Drawing.Point(120, 87);
            this.numericUpDownPin.Maximum = new decimal(new int[] { 69, 0, 0, 0 });
            this.numericUpDownPin.Name = "numericUpDownPin";
            this.numericUpDownPin.Size = new System.Drawing.Size(80, 23);
            this.numericUpDownPin.TabIndex = 5;

            // labelPin2
            this.labelPin2.AutoSize = true;
            this.labelPin2.Location = new System.Drawing.Point(220, 90);
            this.labelPin2.Name = "labelPin2";
            this.labelPin2.Size = new System.Drawing.Size(34, 15);
            this.labelPin2.TabIndex = 6;
            this.labelPin2.Text = "Pin2:";

            // numericUpDownPin2
            this.numericUpDownPin2.Location = new System.Drawing.Point(270, 87);
            this.numericUpDownPin2.Maximum = new decimal(new int[] { 69, 0, 0, 0 });
            this.numericUpDownPin2.Name = "numericUpDownPin2";
            this.numericUpDownPin2.Size = new System.Drawing.Size(80, 23);
            this.numericUpDownPin2.TabIndex = 7;

            // labelKeyCommand
            this.labelKeyCommand.AutoSize = true;
            this.labelKeyCommand.Location = new System.Drawing.Point(20, 125);
            this.labelKeyCommand.Name = "labelKeyCommand";
            this.labelKeyCommand.Size = new System.Drawing.Size(95, 15);
            this.labelKeyCommand.TabIndex = 8;
            this.labelKeyCommand.Text = "Key Command:";

            // textBoxKeyCommand
            this.textBoxKeyCommand.Location = new System.Drawing.Point(120, 122);
            this.textBoxKeyCommand.Name = "textBoxKeyCommand";
            this.textBoxKeyCommand.Size = new System.Drawing.Size(230, 23);
            this.textBoxKeyCommand.TabIndex = 9;

            // buttonKeyBuilder
            this.buttonKeyBuilder.Location = new System.Drawing.Point(356, 121);
            this.buttonKeyBuilder.Name = "buttonKeyBuilder";
            this.buttonKeyBuilder.Size = new System.Drawing.Size(64, 23);
            this.buttonKeyBuilder.TabIndex = 10;
            this.buttonKeyBuilder.Text = "Help...";
            this.buttonKeyBuilder.UseVisualStyleBackColor = true;
            this.buttonKeyBuilder.Click += new System.EventHandler(this.ButtonKeyBuilder_Click);

            // labelKeyCommand2
            this.labelKeyCommand2.AutoSize = true;
            this.labelKeyCommand2.Location = new System.Drawing.Point(20, 160);
            this.labelKeyCommand2.Name = "labelKeyCommand2";
            this.labelKeyCommand2.Size = new System.Drawing.Size(95, 15);
            this.labelKeyCommand2.TabIndex = 11;
            this.labelKeyCommand2.Text = "Key Command 2:";

            // textBoxKeyCommand2
            this.textBoxKeyCommand2.Location = new System.Drawing.Point(120, 157);
            this.textBoxKeyCommand2.Name = "textBoxKeyCommand2";
            this.textBoxKeyCommand2.Size = new System.Drawing.Size(300, 23);
            this.textBoxKeyCommand2.TabIndex = 12;

            // labelSensitivity
            this.labelSensitivity.AutoSize = true;
            this.labelSensitivity.Location = new System.Drawing.Point(20, 195);
            this.labelSensitivity.Name = "labelSensitivity";
            this.labelSensitivity.Size = new System.Drawing.Size(69, 15);
            this.labelSensitivity.TabIndex = 13;
            this.labelSensitivity.Text = "Sensitivity:";

            // comboBoxSensitivity
            this.comboBoxSensitivity.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboBoxSensitivity.FormattingEnabled = true;
            this.comboBoxSensitivity.Location = new System.Drawing.Point(120, 192);
            this.comboBoxSensitivity.Name = "comboBoxSensitivity";
            this.comboBoxSensitivity.Size = new System.Drawing.Size(80, 23);
            this.comboBoxSensitivity.TabIndex = 14;

            // checkBoxEnabled
            this.checkBoxEnabled.AutoSize = true;
            this.checkBoxEnabled.Checked = true;
            this.checkBoxEnabled.CheckState = System.Windows.Forms.CheckState.Checked;
            this.checkBoxEnabled.Location = new System.Drawing.Point(120, 230);
            this.checkBoxEnabled.Name = "checkBoxEnabled";
            this.checkBoxEnabled.Size = new System.Drawing.Size(68, 19);
            this.checkBoxEnabled.TabIndex = 15;
            this.checkBoxEnabled.Text = "Enabled";
            this.checkBoxEnabled.UseVisualStyleBackColor = true;

            // buttonOK
            this.buttonOK.Location = new System.Drawing.Point(250, 270);
            this.buttonOK.Name = "buttonOK";
            this.buttonOK.Size = new System.Drawing.Size(80, 30);
            this.buttonOK.TabIndex = 16;
            this.buttonOK.Text = "OK";
            this.buttonOK.UseVisualStyleBackColor = true;
            this.buttonOK.Click += new System.EventHandler(this.ButtonOK_Click);

            // buttonCancel
            this.buttonCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.buttonCancel.Location = new System.Drawing.Point(340, 270);
            this.buttonCancel.Name = "buttonCancel";
            this.buttonCancel.Size = new System.Drawing.Size(80, 30);
            this.buttonCancel.TabIndex = 17;
            this.buttonCancel.Text = "Cancel";
            this.buttonCancel.UseVisualStyleBackColor = true;
            this.buttonCancel.Click += new System.EventHandler(this.ButtonCancel_Click);

            // InputEditorForm
            this.AcceptButton = this.buttonOK;
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 15F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.buttonCancel;
            this.ClientSize = new System.Drawing.Size(444, 320);
            this.Controls.Add(this.buttonCancel);
            this.Controls.Add(this.buttonOK);
            this.Controls.Add(this.checkBoxEnabled);
            this.Controls.Add(this.comboBoxSensitivity);
            this.Controls.Add(this.labelSensitivity);
            this.Controls.Add(this.textBoxKeyCommand2);
            this.Controls.Add(this.labelKeyCommand2);
            this.Controls.Add(this.buttonKeyBuilder);
            this.Controls.Add(this.textBoxKeyCommand);
            this.Controls.Add(this.labelKeyCommand);
            this.Controls.Add(this.numericUpDownPin2);
            this.Controls.Add(this.labelPin2);
            this.Controls.Add(this.numericUpDownPin);
            this.Controls.Add(this.labelPin);
            this.Controls.Add(this.comboBoxType);
            this.Controls.Add(this.labelType);
            this.Controls.Add(this.textBoxName);
            this.Controls.Add(this.labelName);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "InputEditorForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Input Module Editor";

            ((System.ComponentModel.ISupportInitialize)(this.numericUpDownPin)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDownPin2)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();
        }

        #endregion

        private System.Windows.Forms.Label labelName;
        private System.Windows.Forms.TextBox textBoxName;
        private System.Windows.Forms.Label labelType;
        private System.Windows.Forms.ComboBox comboBoxType;
        private System.Windows.Forms.Label labelPin;
        private System.Windows.Forms.NumericUpDown numericUpDownPin;
        private System.Windows.Forms.Label labelPin2;
        private System.Windows.Forms.NumericUpDown numericUpDownPin2;
        private System.Windows.Forms.Label labelKeyCommand;
        private System.Windows.Forms.TextBox textBoxKeyCommand;
        private System.Windows.Forms.Label labelKeyCommand2;
        private System.Windows.Forms.TextBox textBoxKeyCommand2;
        private System.Windows.Forms.Label labelSensitivity;
        private System.Windows.Forms.ComboBox comboBoxSensitivity;
        private System.Windows.Forms.CheckBox checkBoxEnabled;
        private System.Windows.Forms.Button buttonOK;
        private System.Windows.Forms.Button buttonCancel;
        private System.Windows.Forms.Button buttonKeyBuilder;
    }
}
