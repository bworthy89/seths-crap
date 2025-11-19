using System;
using System.IO.Ports;
using System.Linq;
using System.Threading.Tasks;
using KeyFlightConnector.Models;

namespace KeyFlightConnector.Communication
{
    /// <summary>
    /// Manages KeyFlight device detection, connection, and configuration
    /// </summary>
    public class DeviceManager : IDisposable
    {
        private SerialPort _serialPort;
        private CmdMessenger _messenger;

        // Device information
        public string BoardType { get; private set; }
        public string FirmwareVersion { get; private set; }
        public int MaxModules { get; private set; }
        public bool IsConnected => _serialPort?.IsOpen ?? false;

        // Events
        public event EventHandler Connected;
        public event EventHandler Disconnected;
        public event EventHandler<string> LogMessage;
        public event EventHandler<CommandReceivedEventArgs> CommandReceived;
        public event EventHandler<int> InputTriggered; // moduleId

        /// <summary>
        /// Scans for available COM ports
        /// </summary>
        public static string[] GetAvailablePorts()
        {
            return SerialPort.GetPortNames();
        }

        /// <summary>
        /// Attempts to connect to a KeyFlight device on the specified port
        /// </summary>
        public async Task<bool> ConnectAsync(string portName, int baudRate = 115200)
        {
            try
            {
                Log($"Connecting to {portName} at {baudRate} baud...");

                // Create serial port
                _serialPort = new SerialPort(portName, baudRate)
                {
                    DtrEnable = true,
                    RtsEnable = true,
                    ReadTimeout = 2000,
                    WriteTimeout = 2000
                };

                // Open port
                _serialPort.Open();
                await Task.Delay(2000); // Wait for Arduino reset

                // Create messenger
                _messenger = new CmdMessenger(_serialPort);
                _messenger.CommandReceived += Messenger_CommandReceived;
                _messenger.DebugMessageReceived += (s, msg) => Log(msg);
                _messenger.ErrorReceived += (s, msg) => Log($"ERROR: {msg}");

                // Request device info
                _messenger.SendCommand(Commands.kStatus);

                // Wait for info response (with timeout)
                var infoReceived = false;
                var timeout = DateTime.Now.AddSeconds(5);

                void InfoHandler(object s, CommandReceivedEventArgs e)
                {
                    if (e.Command == Commands.kInfo && e.Parameters.Length >= 3)
                    {
                        BoardType = e.Parameters[0];
                        FirmwareVersion = e.Parameters[1];
                        MaxModules = int.TryParse(e.Parameters[2], out int max) ? max : 40;
                        infoReceived = true;
                    }
                }

                _messenger.CommandReceived += InfoHandler;

                while (!infoReceived && DateTime.Now < timeout)
                {
                    await Task.Delay(100);
                }

                _messenger.CommandReceived -= InfoHandler;

                if (!infoReceived)
                {
                    Disconnect();
                    Log("Failed to receive device info");
                    return false;
                }

                Log($"Connected to {BoardType} v{FirmwareVersion} (Max modules: {MaxModules})");
                Connected?.Invoke(this, EventArgs.Empty);
                return true;
            }
            catch (Exception ex)
            {
                Log($"Connection failed: {ex.Message}");
                Disconnect();
                return false;
            }
        }

        /// <summary>
        /// Disconnects from the device
        /// </summary>
        public void Disconnect()
        {
            if (_messenger != null)
            {
                _messenger.CommandReceived -= Messenger_CommandReceived;
                _messenger.Dispose();
                _messenger = null;
            }

            if (_serialPort != null)
            {
                if (_serialPort.IsOpen)
                    _serialPort.Close();
                _serialPort.Dispose();
                _serialPort = null;
            }

            Disconnected?.Invoke(this, EventArgs.Empty);
            Log("Disconnected");
        }

        /// <summary>
        /// Uploads configuration to the device
        /// </summary>
        public void UploadConfiguration(DeviceConfiguration config)
        {
            if (!IsConnected)
                throw new InvalidOperationException("Not connected to device");

            Log($"Uploading configuration with {config.Modules.Count} modules...");

            // Reset configuration first
            _messenger.SendCommand(Commands.kResetConfig);
            System.Threading.Thread.Sleep(100);

            // Send each module
            foreach (var module in config.Modules.Where(m => m.Enabled))
            {
                // Set pin configuration
                _messenger.SendCommand(Commands.kSetPin,
                    module.Id.ToString(),
                    module.Type.ToString().ToUpper(),
                    module.Pin.ToString(),
                    module.Pin2.ToString());

                System.Threading.Thread.Sleep(50);

                // Set keyboard command
                _messenger.SendCommand(Commands.kSetKey,
                    module.Id.ToString(),
                    module.KeyCommand);

                // For encoders and switches, send second command
                if ((module.Type == InputType.Encoder || module.Type == InputType.Switch)
                    && !string.IsNullOrWhiteSpace(module.KeyCommand2))
                {
                    System.Threading.Thread.Sleep(50);
                    _messenger.SendCommand(Commands.kSetKey,
                        module.Id.ToString(),
                        module.KeyCommand2);
                }

                System.Threading.Thread.Sleep(50);
            }

            // Save to EEPROM
            _messenger.SendCommand(Commands.kSaveConfig);
            Log("Configuration uploaded successfully");
        }

        /// <summary>
        /// Handles incoming commands from device
        /// </summary>
        private void Messenger_CommandReceived(object sender, CommandReceivedEventArgs e)
        {
            // Handle trigger events
            if (e.Command == Commands.kTrigger && e.Parameters.Length >= 1)
            {
                if (int.TryParse(e.Parameters[0], out int moduleId))
                {
                    InputTriggered?.Invoke(this, moduleId);
                }
            }

            // Forward to subscribers
            CommandReceived?.Invoke(this, e);
        }

        /// <summary>
        /// Logs a message
        /// </summary>
        private void Log(string message)
        {
            LogMessage?.Invoke(this, $"[{DateTime.Now:HH:mm:ss}] {message}");
        }

        public void Dispose()
        {
            Disconnect();
        }
    }
}
