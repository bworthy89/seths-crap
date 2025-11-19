# KeyFlight Connector

**Windows application for configuring KeyFlight keyboard command controllers**

MobiFlight-inspired C# Windows Forms application for visual configuration of Arduino-based keyboard input devices.

## Features

- 📱 **Visual Configuration**: Drag-and-drop interface for input configuration
- 🔌 **Device Management**: Auto-detect and connect to KeyFlight devices
- ⌨️ **Keyboard Command Builder**: Easy creation of complex keyboard combinations
- 💾 **Save/Load Configurations**: JSON-based configuration files (.kfc)
- 📊 **Live Monitoring**: See input events in real-time
- 🚀 **One-Click Upload**: Upload configurations directly to device EEPROM

## Requirements

- **Operating System**: Windows 10/11 (64-bit)
- **.NET**: .NET 8.0 or higher
- **Hardware**: KeyFlight device (Arduino Mega + Pro Micro)

## Building from Source

### Prerequisites
- Visual Studio 2022 (Community Edition or higher)
- .NET 8.0 SDK

### Build Steps

```bash
# Open solution
cd KeyFlightConnector
start KeyFlightConnector.sln

# Or build from command line
dotnet build KeyFlightConnector.sln --configuration Release
```

### Dependencies
All dependencies are managed via NuGet:
- **System.IO.Ports** (v8.0.0) - Serial communication
- **Newtonsoft.Json** (v13.0.3) - Configuration serialization

## Usage

### 1. Connect Device
1. Connect Arduino Mega to PC via USB
2. Select COM port from dropdown
3. Click **Connect**
4. Wait for device info to appear

### 2. Add Input Modules
1. Click **Add Input**
2. Configure input properties:
   - **Name**: Descriptive name (e.g., "Fire Button")
   - **Type**: Button, Encoder, Switch, or Analog
   - **Pin**: Arduino pin number (0-69)
   - **Pin2**: Second pin for encoders
   - **Key Command**: Keyboard command to send
3. Click **OK**

### 3. Configure Keyboard Commands

**Format Examples:**
```
A              - Single key
CTRL+C         - Modifier + key
CTRL+ALT+DEL   - Multiple modifiers
A+B+C          - Multiple keys simultaneously
SHIFT+F1       - Modifier + function key
```

**Supported Keys:**
- Modifiers: `CTRL`, `ALT`, `SHIFT`, `WIN`
- Function: `F1` through `F12`
- Arrows: `UPARROW`, `DOWNARROW`, `LEFTARROW`, `RIGHTARROW`
- Special: `ENTER`, `ESC`, `TAB`, `SPACE`, `BACKSPACE`, `DELETE`
- Navigation: `HOME`, `END`, `PAGEUP`, `PAGEDOWN`

### 4. Upload Configuration
1. Click **Upload to Device**
2. Wait for "Configuration uploaded successfully"
3. Configuration is saved to device EEPROM
4. Test your inputs!

### 5. Save/Load Configurations
- **File → Save**: Save current configuration
- **File → Open**: Load saved configuration
- **File Extension**: `.kfc` (KeyFlight Configuration)

## Project Structure

```
KeyFlightConnector/
├── Communication/
│   ├── Commands.cs              # Command protocol definitions
│   ├── CmdMessenger.cs          # CommandMessenger implementation
│   └── DeviceManager.cs         # Device connection manager
├── Models/
│   ├── InputType.cs             # Input type enumeration
│   ├── InputDevice.cs           # Input module model
│   └── DeviceConfiguration.cs   # Complete configuration model
├── Forms/
│   ├── MainForm.cs              # Main application window
│   └── InputEditorForm.cs       # Input module editor dialog
└── Program.cs                   # Application entry point
```

## CommandMessenger Protocol

KeyFlight uses a text-based protocol for Arduino communication:

**Command Format:** `<CommandID>,<Param1>,<Param2>,...;`

**Example Commands:**
```
0;                      # Request status
1,MEGA,1.0.0,40;       # Device info response
5,0,BUTTON,2,0;        # Set module 0 as Button on pin 2
6,0,A+B+C;             # Set module 0 command to A+B+C
9;                     # Save configuration to EEPROM
```

See `Communication/Commands.cs` for full protocol definition.

## Configuration File Format

Configurations are saved as JSON:

```json
{
  "version": "1.0",
  "deviceName": "My Controller",
  "modules": [
    {
      "id": 0,
      "type": "Button",
      "name": "Fire",
      "pin": 2,
      "pin2": 0,
      "keyCommand": "SPACE",
      "keyCommand2": "",
      "sensitivity": 1,
      "enabled": true
    },
    {
      "id": 1,
      "type": "Encoder",
      "name": "Throttle",
      "pin": 3,
      "pin2": 4,
      "keyCommand": "W",
      "keyCommand2": "S",
      "sensitivity": 1,
      "enabled": true
    }
  ]
}
```

## Troubleshooting

### "Port not found" or connection fails
- Check USB cable (must be data cable, not charge-only)
- Install Arduino drivers
- Try different COM port
- Check Device Manager for COM port number

### "Failed to receive device info"
- Ensure correct firmware is uploaded to Mega
- Verify baud rate is 115200
- Try disconnecting and reconnecting
- Check Serial Monitor for errors

### Configuration upload fails
- Verify device is connected
- Check serial communication in log
- Ensure firmware matches connector version
- Try power cycling the Arduino

### Module not responding
- Verify pin number is correct
- Check keyboard command format
- Test with simple command first (e.g., "A")
- Check Pro Micro is receiving commands (see log)

## Architecture Inspiration

KeyFlight Connector's architecture is inspired by [MobiFlight Connector](https://github.com/MobiFlight/MobiFlight-Connector):

| MobiFlight | KeyFlight |
|------------|-----------|
| Flight simulator interface | Keyboard command interface |
| Modular input architecture | ✅ Same pattern |
| CommandMessenger protocol | ✅ Adapted protocol |
| C# Windows Forms | ✅ Same technology |
| Visual configuration | ✅ Similar UI |

We're grateful to the MobiFlight team for their excellent architectural patterns!

## License

MIT License - See LICENSE file for details

MobiFlight is copyright © MobiFlight Team. KeyFlight is an independent project using similar architectural patterns.

## Contributing

Contributions welcome! Please:
1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test thoroughly
5. Submit a pull request

---

**Version**: 0.1.0-dev
**Status**: 🚧 In Development
**Platform**: Windows (.NET 8.0)
