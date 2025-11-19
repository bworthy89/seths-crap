namespace KeyFlightConnector.Communication
{
    /// <summary>
    /// CommandMessenger protocol commands
    /// Must match Arduino firmware command definitions
    /// </summary>
    public enum Commands
    {
        /// <summary>Request device status</summary>
        kStatus = 0,

        /// <summary>Device info response (board, version, max modules)</summary>
        kInfo = 1,

        /// <summary>Set complete configuration</summary>
        kSetConfig = 2,

        /// <summary>Request current configuration</summary>
        kGetConfig = 3,

        /// <summary>Configuration data response</summary>
        kConfigInfo = 4,

        /// <summary>Set module pin configuration</summary>
        kSetPin = 5,

        /// <summary>Set module keyboard command</summary>
        kSetKey = 6,

        /// <summary>Input trigger event (from Arduino)</summary>
        kTrigger = 7,

        /// <summary>Reset configuration to defaults</summary>
        kResetConfig = 8,

        /// <summary>Save configuration to EEPROM</summary>
        kSaveConfig = 9,

        /// <summary>Generic acknowledgment</summary>
        kAck = 10,

        /// <summary>Error response</summary>
        kError = 11,

        /// <summary>Debug message</summary>
        kDebug = 12
    }
}
