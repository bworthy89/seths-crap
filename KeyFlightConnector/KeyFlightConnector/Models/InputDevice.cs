using System;

namespace KeyFlightConnector.Models
{
    /// <summary>
    /// Represents a configured input device (button, encoder, switch, or analog)
    /// </summary>
    public class InputDevice
    {
        /// <summary>Unique identifier for this input (0-39)</summary>
        public int Id { get; set; }

        /// <summary>Type of input device</summary>
        public InputType Type { get; set; }

        /// <summary>User-friendly name</summary>
        public string Name { get; set; } = string.Empty;

        /// <summary>Primary pin number (0-69 on Mega)</summary>
        public int Pin { get; set; }

        /// <summary>Secondary pin (for encoders)</summary>
        public int Pin2 { get; set; }

        /// <summary>Keyboard command to send (e.g., "A+B+C", "CTRL+F")</summary>
        public string KeyCommand { get; set; } = string.Empty;

        /// <summary>Secondary keyboard command (encoder CCW, switch OFF)</summary>
        public string KeyCommand2 { get; set; } = string.Empty;

        /// <summary>Encoder sensitivity (1, 10, 100, 1000)</summary>
        public int Sensitivity { get; set; } = 1;

        /// <summary>Whether this input is currently enabled</summary>
        public bool Enabled { get; set; } = true;

        /// <summary>
        /// Validates the input configuration
        /// </summary>
        public bool Validate(out string error)
        {
            error = string.Empty;

            // Validate pin range
            if (Pin < 0 || Pin > 69)
            {
                error = $"Pin {Pin} is out of range (0-69)";
                return false;
            }

            // Validate encoder has two pins
            if (Type == InputType.Encoder && (Pin2 < 0 || Pin2 > 69))
            {
                error = "Encoder requires a valid Pin2";
                return false;
            }

            // Validate key command exists
            if (string.IsNullOrWhiteSpace(KeyCommand))
            {
                error = "Key command cannot be empty";
                return false;
            }

            // Validate encoder has both commands
            if (Type == InputType.Encoder && string.IsNullOrWhiteSpace(KeyCommand2))
            {
                error = "Encoder requires both KeyCommand (CW) and KeyCommand2 (CCW)";
                return false;
            }

            return true;
        }

        /// <summary>
        /// Returns a display-friendly string representation
        /// </summary>
        public override string ToString()
        {
            return $"{Name} ({Type}) - Pin {Pin}";
        }
    }
}
