using System;
using System.Windows.Forms;
using KeyFlightConnector.Models;

namespace KeyFlightConnector.Forms
{
    /// <summary>
    /// Dialog for adding/editing input module configuration
    /// </summary>
    public partial class InputEditorForm : Form
    {
        public InputDevice InputDevice { get; private set; }
        public int ModuleId { get; set; }

        public InputEditorForm()
        {
            InitializeComponent();
            InitializeForm();
        }

        public InputEditorForm(InputDevice existingDevice) : this()
        {
            InputDevice = existingDevice;
            ModuleId = existingDevice.Id;
            LoadDevice();
        }

        private void InitializeForm()
        {
            // Populate input types
            comboBoxType.Items.Add("Button");
            comboBoxType.Items.Add("Encoder");
            comboBoxType.Items.Add("Switch");
            comboBoxType.Items.Add("Analog");
            comboBoxType.SelectedIndex = 0;

            // Populate sensitivity (for encoders)
            comboBoxSensitivity.Items.AddRange(new object[] { 1, 10, 100, 1000 });
            comboBoxSensitivity.SelectedIndex = 0;

            UpdateUIForInputType();
        }

        private void LoadDevice()
        {
            textBoxName.Text = InputDevice.Name;
            comboBoxType.SelectedIndex = (int)InputDevice.Type - 1;
            numericUpDownPin.Value = InputDevice.Pin;
            numericUpDownPin2.Value = InputDevice.Pin2;
            textBoxKeyCommand.Text = InputDevice.KeyCommand;
            textBoxKeyCommand2.Text = InputDevice.KeyCommand2;
            comboBoxSensitivity.SelectedItem = InputDevice.Sensitivity;
            checkBoxEnabled.Checked = InputDevice.Enabled;
        }

        private void ComboBoxType_SelectedIndexChanged(object sender, EventArgs e)
        {
            UpdateUIForInputType();
        }

        private void UpdateUIForInputType()
        {
            var type = (InputType)(comboBoxType.SelectedIndex + 1);

            // Pin2 only for encoders
            labelPin2.Visible = type == InputType.Encoder;
            numericUpDownPin2.Visible = type == InputType.Encoder;

            // KeyCommand2 for encoders and switches
            var needsKeyCommand2 = type == InputType.Encoder || type == InputType.Switch;
            labelKeyCommand2.Visible = needsKeyCommand2;
            textBoxKeyCommand2.Visible = needsKeyCommand2;

            if (type == InputType.Encoder)
            {
                labelKeyCommand.Text = "Key Command (CW):";
                labelKeyCommand2.Text = "Key Command (CCW):";
            }
            else if (type == InputType.Switch)
            {
                labelKeyCommand.Text = "Key Command (ON):";
                labelKeyCommand2.Text = "Key Command (OFF):";
            }
            else
            {
                labelKeyCommand.Text = "Key Command:";
            }

            // Sensitivity only for encoders
            labelSensitivity.Visible = type == InputType.Encoder;
            comboBoxSensitivity.Visible = type == InputType.Encoder;
        }

        private void ButtonOK_Click(object sender, EventArgs e)
        {
            // Validate inputs
            if (string.IsNullOrWhiteSpace(textBoxName.Text))
            {
                MessageBox.Show("Please enter a name", "Validation Error",
                    MessageBoxButtons.OK, MessageBoxIcon.Warning);
                textBoxName.Focus();
                return;
            }

            if (string.IsNullOrWhiteSpace(textBoxKeyCommand.Text))
            {
                MessageBox.Show("Please enter a keyboard command", "Validation Error",
                    MessageBoxButtons.OK, MessageBoxIcon.Warning);
                textBoxKeyCommand.Focus();
                return;
            }

            var type = (InputType)(comboBoxType.SelectedIndex + 1);

            if (type == InputType.Encoder && string.IsNullOrWhiteSpace(textBoxKeyCommand2.Text))
            {
                MessageBox.Show("Encoders require both CW and CCW commands", "Validation Error",
                    MessageBoxButtons.OK, MessageBoxIcon.Warning);
                textBoxKeyCommand2.Focus();
                return;
            }

            // Create input device
            InputDevice = new InputDevice
            {
                Id = ModuleId,
                Type = type,
                Name = textBoxName.Text.Trim(),
                Pin = (int)numericUpDownPin.Value,
                Pin2 = (int)numericUpDownPin2.Value,
                KeyCommand = textBoxKeyCommand.Text.Trim(),
                KeyCommand2 = textBoxKeyCommand2.Text.Trim(),
                Sensitivity = (int)comboBoxSensitivity.SelectedItem,
                Enabled = checkBoxEnabled.Checked
            };

            DialogResult = DialogResult.OK;
            Close();
        }

        private void ButtonCancel_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.Cancel;
            Close();
        }

        private void ButtonKeyBuilder_Click(object sender, EventArgs e)
        {
            MessageBox.Show(
                "Keyboard Command Format:\n\n" +
                "Single key: A, B, F1, SPACE\n" +
                "Modifier + key: CTRL+C, ALT+F4\n" +
                "Multiple modifiers: CTRL+ALT+DEL\n" +
                "Multiple keys: A+B+C\n\n" +
                "Special Keys:\n" +
                "CTRL, ALT, SHIFT, WIN\n" +
                "F1-F12\n" +
                "UPARROW, DOWNARROW, LEFTARROW, RIGHTARROW\n" +
                "ENTER, ESC, TAB, SPACE, BACKSPACE\n" +
                "HOME, END, PAGEUP, PAGEDOWN",
                "Keyboard Command Help",
                MessageBoxButtons.OK,
                MessageBoxIcon.Information);
        }
    }
}
