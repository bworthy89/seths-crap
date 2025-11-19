namespace KeyFlightConnector.Models
{
    /// <summary>
    /// Types of input devices supported by KeyFlight
    /// Based on MobiFlight's modular input architecture
    /// </summary>
    public enum InputType
    {
        /// <summary>Momentary push button</summary>
        Button = 1,

        /// <summary>Rotary encoder with detents</summary>
        Encoder = 2,

        /// <summary>Toggle switch (ON/OFF)</summary>
        Switch = 3,

        /// <summary>Analog input (potentiometer, slider)</summary>
        Analog = 4
    }
}
