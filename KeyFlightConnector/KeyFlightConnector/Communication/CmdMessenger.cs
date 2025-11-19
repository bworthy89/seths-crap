using System;
using System.IO.Ports;
using System.Text;
using System.Collections.Generic;
using System.Linq;

namespace KeyFlightConnector.Communication
{
    /// <summary>
    /// CommandMessenger protocol implementation
    /// Handles bidirectional command-based communication with Arduino
    /// Based on MobiFlight's CommandMessenger pattern
    /// </summary>
    public class CmdMessenger : IDisposable
    {
        private SerialPort _serialPort;
        private readonly char _fieldSeparator = ',';
        private readonly char _commandSeparator = ';';
        private readonly StringBuilder _receiveBuffer = new StringBuilder();

        // Event handlers
        public event EventHandler<CommandReceivedEventArgs> CommandReceived;
        public event EventHandler<string> DebugMessageReceived;
        public event EventHandler<string> ErrorReceived;

        public bool IsConnected => _serialPort?.IsOpen ?? false;

        /// <summary>
        /// Initializes a new instance of CmdMessenger
        /// </summary>
        public CmdMessenger(SerialPort serialPort)
        {
            _serialPort = serialPort ?? throw new ArgumentNullException(nameof(serialPort));
            _serialPort.DataReceived += SerialPort_DataReceived;
        }

        /// <summary>
        /// Sends a command with no parameters
        /// </summary>
        public void SendCommand(Commands command)
        {
            SendCommand(command, new string[] { });
        }

        /// <summary>
        /// Sends a command with parameters
        /// </summary>
        public void SendCommand(Commands command, params string[] parameters)
        {
            if (!IsConnected)
                throw new InvalidOperationException("Serial port not connected");

            var sb = new StringBuilder();
            sb.Append((int)command);

            foreach (var param in parameters)
            {
                sb.Append(_fieldSeparator);
                sb.Append(param);
            }

            sb.Append(_commandSeparator);

            var message = sb.ToString();
            _serialPort.WriteLine(message);

            DebugMessageReceived?.Invoke(this, $"TX: {message.TrimEnd('\r', '\n')}");
        }

        /// <summary>
        /// Handles incoming serial data
        /// </summary>
        private void SerialPort_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            try
            {
                string data = _serialPort.ReadExisting();
                _receiveBuffer.Append(data);

                // Process complete commands (ending with commandSeparator)
                string bufferStr = _receiveBuffer.ToString();
                int separatorIndex;

                while ((separatorIndex = bufferStr.IndexOf(_commandSeparator)) >= 0)
                {
                    // Extract complete command
                    string commandStr = bufferStr.Substring(0, separatorIndex).Trim();
                    bufferStr = bufferStr.Substring(separatorIndex + 1);

                    if (!string.IsNullOrWhiteSpace(commandStr))
                    {
                        ProcessCommand(commandStr);
                    }
                }

                // Keep remaining incomplete data in buffer
                _receiveBuffer.Clear();
                _receiveBuffer.Append(bufferStr);
            }
            catch (Exception ex)
            {
                ErrorReceived?.Invoke(this, $"Data receive error: {ex.Message}");
            }
        }

        /// <summary>
        /// Processes a complete command string
        /// </summary>
        private void ProcessCommand(string commandStr)
        {
            try
            {
                DebugMessageReceived?.Invoke(this, $"RX: {commandStr}");

                var parts = commandStr.Split(_fieldSeparator);
                if (parts.Length == 0) return;

                if (!int.TryParse(parts[0], out int cmdId))
                {
                    ErrorReceived?.Invoke(this, $"Invalid command ID: {parts[0]}");
                    return;
                }

                var command = (Commands)cmdId;
                var parameters = parts.Skip(1).ToArray();

                // Handle specific commands
                switch (command)
                {
                    case Commands.kDebug:
                        if (parameters.Length > 0)
                            DebugMessageReceived?.Invoke(this, $"Arduino: {string.Join(" ", parameters)}");
                        break;

                    case Commands.kError:
                        if (parameters.Length > 0)
                            ErrorReceived?.Invoke(this, $"Arduino Error: {string.Join(" ", parameters)}");
                        break;

                    default:
                        // Raise generic command received event
                        CommandReceived?.Invoke(this, new CommandReceivedEventArgs(command, parameters));
                        break;
                }
            }
            catch (Exception ex)
            {
                ErrorReceived?.Invoke(this, $"Command processing error: {ex.Message}");
            }
        }

        /// <summary>
        /// Disposes resources
        /// </summary>
        public void Dispose()
        {
            if (_serialPort != null)
            {
                _serialPort.DataReceived -= SerialPort_DataReceived;
                _serialPort = null;
            }
        }
    }

    /// <summary>
    /// Event arguments for received commands
    /// </summary>
    public class CommandReceivedEventArgs : EventArgs
    {
        public Commands Command { get; }
        public string[] Parameters { get; }

        public CommandReceivedEventArgs(Commands command, string[] parameters)
        {
            Command = command;
            Parameters = parameters ?? Array.Empty<string>();
        }
    }
}
