using System;
using System.Windows.Forms;
using System.IO;
using KeyFlightConnector.Models;
using KeyFlightConnector.Communication;

namespace KeyFlightConnector.Forms
{
    /// <summary>
    /// Main application form for KeyFlight Connector
    /// MobiFlight-inspired UI for device configuration
    /// </summary>
    public partial class MainForm : Form
    {
        private DeviceManager _deviceManager;
        private DeviceConfiguration _currentConfig;
        private string _currentConfigPath;

        public MainForm()
        {
            InitializeComponent();
            InitializeApplication();
        }

        /// <summary>
        /// Initializes application state
        /// </summary>
        private void InitializeApplication()
        {
            _deviceManager = new DeviceManager();
            _deviceManager.Connected += DeviceManager_Connected;
            _deviceManager.Disconnected += DeviceManager_Disconnected;
            _deviceManager.LogMessage += DeviceManager_LogMessage;
            _deviceManager.InputTriggered += DeviceManager_InputTriggered;

            _currentConfig = new DeviceConfiguration();

            UpdateUI();
            RefreshPortList();
            Log("KeyFlight Connector started");
        }

        /// <summary>
        /// Refreshes available COM ports
        /// </summary>
        private void RefreshPortList()
        {
            comboBoxPorts.Items.Clear();
            var ports = DeviceManager.GetAvailablePorts();
            comboBoxPorts.Items.AddRange(ports);
            if (ports.Length > 0)
                comboBoxPorts.SelectedIndex = 0;
        }

        /// <summary>
        /// Connects to selected device
        /// </summary>
        private async void ButtonConnect_Click(object sender, EventArgs e)
        {
            if (_deviceManager.IsConnected)
            {
                _deviceManager.Disconnect();
                return;
            }

            if (comboBoxPorts.SelectedItem == null)
            {
                MessageBox.Show("Please select a COM port", "No Port Selected",
                    MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return;
            }

            buttonConnect.Enabled = false;
            var portName = comboBoxPorts.SelectedItem.ToString();
            var success = await _deviceManager.ConnectAsync(portName);

            if (!success)
            {
                MessageBox.Show($"Failed to connect to {portName}", "Connection Failed",
                    MessageBoxButtons.OK, MessageBoxIcon.Error);
            }

            buttonConnect.Enabled = true;
            UpdateUI();
        }

        /// <summary>
        /// Adds a new input module
        /// </summary>
        private void ButtonAddInput_Click(object sender, EventArgs e)
        {
            var inputForm = new InputEditorForm();
            inputForm.ModuleId = _currentConfig.GetNextModuleId();

            if (inputForm.ShowDialog() == DialogResult.OK)
            {
                if (_currentConfig.AddModule(inputForm.InputDevice, out string error))
                {
                    RefreshModuleList();
                    Log($"Added {inputForm.InputDevice.Name}");
                }
                else
                {
                    MessageBox.Show(error, "Cannot Add Module",
                        MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
            }
        }

        /// <summary>
        /// Removes selected input module
        /// </summary>
        private void ButtonRemoveInput_Click(object sender, EventArgs e)
        {
            if (listViewModules.SelectedItems.Count == 0) return;

            var item = listViewModules.SelectedItems[0];
            var module = (InputDevice)item.Tag;

            var result = MessageBox.Show(
                $"Remove {module.Name}?",
                "Confirm Remove",
                MessageBoxButtons.YesNo,
                MessageBoxIcon.Question);

            if (result == DialogResult.Yes)
            {
                _currentConfig.RemoveModule(module.Id);
                RefreshModuleList();
                Log($"Removed {module.Name}");
            }
        }

        /// <summary>
        /// Uploads configuration to device
        /// </summary>
        private void ButtonUploadConfig_Click(object sender, EventArgs e)
        {
            if (!_deviceManager.IsConnected)
            {
                MessageBox.Show("Please connect to a device first", "Not Connected",
                    MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return;
            }

            try
            {
                _deviceManager.UploadConfiguration(_currentConfig);
                MessageBox.Show("Configuration uploaded successfully!", "Success",
                    MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
            catch (Exception ex)
            {
                MessageBox.Show($"Upload failed: {ex.Message}", "Upload Error",
                    MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        /// <summary>
        /// Saves configuration to JSON file
        /// </summary>
        private void MenuFileSave_Click(object sender, EventArgs e)
        {
            if (string.IsNullOrEmpty(_currentConfigPath))
            {
                MenuFileSaveAs_Click(sender, e);
                return;
            }

            SaveConfiguration(_currentConfigPath);
        }

        /// <summary>
        /// Saves configuration to new file
        /// </summary>
        private void MenuFileSaveAs_Click(object sender, EventArgs e)
        {
            using (var dialog = new SaveFileDialog())
            {
                dialog.Filter = "KeyFlight Configuration (*.kfc)|*.kfc|JSON Files (*.json)|*.json|All Files (*.*)|*.*";
                dialog.DefaultExt = "kfc";

                if (dialog.ShowDialog() == DialogResult.OK)
                {
                    SaveConfiguration(dialog.FileName);
                }
            }
        }

        /// <summary>
        /// Opens configuration from file
        /// </summary>
        private void MenuFileOpen_Click(object sender, EventArgs e)
        {
            using (var dialog = new OpenFileDialog())
            {
                dialog.Filter = "KeyFlight Configuration (*.kfc)|*.kfc|JSON Files (*.json)|*.json|All Files (*.*)|*.*";

                if (dialog.ShowDialog() == DialogResult.OK)
                {
                    LoadConfiguration(dialog.FileName);
                }
            }
        }

        /// <summary>
        /// Creates new configuration
        /// </summary>
        private void MenuFileNew_Click(object sender, EventArgs e)
        {
            if (_currentConfig.Modules.Count > 0)
            {
                var result = MessageBox.Show(
                    "Discard current configuration?",
                    "New Configuration",
                    MessageBoxButtons.YesNo,
                    MessageBoxIcon.Question);

                if (result != DialogResult.Yes) return;
            }

            _currentConfig = new DeviceConfiguration();
            _currentConfigPath = null;
            RefreshModuleList();
            Log("New configuration created");
        }

        /// <summary>
        /// Exits application
        /// </summary>
        private void MenuFileExit_Click(object sender, EventArgs e)
        {
            Close();
        }

        /// <summary>
        /// Shows about dialog
        /// </summary>
        private void MenuHelpAbout_Click(object sender, EventArgs e)
        {
            MessageBox.Show(
                "KeyFlight Connector v0.1.0\n\n" +
                "MobiFlight-inspired keyboard command configurator\n\n" +
                "Architecture based on MobiFlight's proven patterns\n" +
                "with gratitude to the MobiFlight team.\n\n" +
                "© 2025 KeyFlight Project",
                "About KeyFlight Connector",
                MessageBoxButtons.OK,
                MessageBoxIcon.Information);
        }

        /// <summary>
        /// Saves configuration to file
        /// </summary>
        private void SaveConfiguration(string path)
        {
            try
            {
                var json = _currentConfig.ToJson();
                File.WriteAllText(path, json);
                _currentConfigPath = path;
                Log($"Configuration saved to {Path.GetFileName(path)}");
            }
            catch (Exception ex)
            {
                MessageBox.Show($"Failed to save: {ex.Message}", "Save Error",
                    MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        /// <summary>
        /// Loads configuration from file
        /// </summary>
        private void LoadConfiguration(string path)
        {
            try
            {
                var json = File.ReadAllText(path);
                _currentConfig = DeviceConfiguration.FromJson(json);
                _currentConfigPath = path;
                RefreshModuleList();
                Log($"Configuration loaded from {Path.GetFileName(path)}");
            }
            catch (Exception ex)
            {
                MessageBox.Show($"Failed to load: {ex.Message}", "Load Error",
                    MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        /// <summary>
        /// Refreshes module list view
        /// </summary>
        private void RefreshModuleList()
        {
            listViewModules.Items.Clear();

            foreach (var module in _currentConfig.Modules)
            {
                var item = new ListViewItem(new[]
                {
                    module.Id.ToString(),
                    module.Name,
                    module.Type.ToString(),
                    module.Pin.ToString(),
                    module.KeyCommand,
                    module.Enabled ? "Yes" : "No"
                });
                item.Tag = module;
                listViewModules.Items.Add(item);
            }

            labelModuleCount.Text = $"{_currentConfig.Modules.Count} / {DeviceConfiguration.MaxModules} modules";
        }

        /// <summary>
        /// Updates UI based on connection state
        /// </summary>
        private void UpdateUI()
        {
            var connected = _deviceManager.IsConnected;

            buttonConnect.Text = connected ? "Disconnect" : "Connect";
            comboBoxPorts.Enabled = !connected;
            buttonRefreshPorts.Enabled = !connected;
            buttonUploadConfig.Enabled = connected;

            if (connected)
            {
                labelConnectionStatus.Text = $"Connected: {_deviceManager.BoardType} v{_deviceManager.FirmwareVersion}";
                labelConnectionStatus.ForeColor = System.Drawing.Color.Green;
            }
            else
            {
                labelConnectionStatus.Text = "Not Connected";
                labelConnectionStatus.ForeColor = System.Drawing.Color.Red;
            }
        }

        /// <summary>
        /// Logs a message to the console
        /// </summary>
        private void Log(string message)
        {
            if (textBoxLog.InvokeRequired)
            {
                textBoxLog.Invoke(new Action<string>(Log), message);
                return;
            }

            textBoxLog.AppendText($"{message}\r\n");
            textBoxLog.SelectionStart = textBoxLog.Text.Length;
            textBoxLog.ScrollToCaret();
        }

        // Event handlers
        private void DeviceManager_Connected(object sender, EventArgs e) => UpdateUI();
        private void DeviceManager_Disconnected(object sender, EventArgs e) => UpdateUI();
        private void DeviceManager_LogMessage(object sender, string message) => Log(message);

        private void DeviceManager_InputTriggered(object sender, int moduleId)
        {
            Log($"Input triggered: Module {moduleId}");
            // Could highlight the module in the list view
        }

        private void ButtonRefreshPorts_Click(object sender, EventArgs e)
        {
            RefreshPortList();
            Log("Port list refreshed");
        }

        protected override void OnFormClosing(FormClosingEventArgs e)
        {
            _deviceManager?.Disconnect();
            _deviceManager?.Dispose();
            base.OnFormClosing(e);
        }
    }
}
