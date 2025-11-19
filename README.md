# KeyFlight - Keyboard Command Controller

**MobiFlight-Inspired Keyboard Automation System**

KeyFlight is an open-source project that allows you to create custom keyboard command controllers using Arduino hardware. Send complex keyboard combinations (A+B+C, CTRL+ALT+DEL, etc.) from physical buttons, encoders, and switches to your PC or Xbox.

## Features

- 🎮 **Flexible Input Support**: Buttons, rotary encoders, switches, potentiometers
- ⌨️ **Complex Keyboard Commands**: Send single keys, modifier combinations, or multi-key chords
- 🖥️ **Visual Configuration**: Easy-to-use Windows application for setup
- 💾 **Persistent Storage**: Configurations saved to EEPROM
- 🔌 **Plug & Play**: USB HID keyboard - no drivers required
- 🎯 **Xbox Compatible**: Works as a standard USB keyboard
- 📦 **Modular Architecture**: Based on proven MobiFlight design patterns

## Quick Start

### What You Need
- Arduino Mega 2560 (or ELEGOO MEGA R3)
- Arduino Pro Micro (ATmega32u4)
- Your choice of inputs (buttons, encoders, etc.)
- 2x USB cables
- Jumper wires

### Installation

**1. Upload Firmware**
```bash
# Pro Micro (must use "Arduino Leonardo" board)
cd KeyFlight-ProMicro
pio run -t upload

# Mega 2560
cd KeyFlight-Mega
pio run -t upload
```

**2. Wire the Boards**
```
Mega TX1 (pin 18) → Pro Micro RX (pin 0)
Mega RX1 (pin 19) → Pro Micro TX (pin 1)
Mega GND → Pro Micro GND
```

**3. Run KeyFlight Connector**
- Download latest release from Releases page
- Run `KeyFlightConnector.exe`
- Click "Detect Devices"
- Configure your inputs
- Upload configuration to Mega

### Your First Button

**Hardware:**
- Connect button between Mega pin 2 and GND
- Enable internal pull-up (configured in software)

**Software:**
1. Open KeyFlight Connector
2. Add New Input → Button
3. Set Pin: 2
4. Set Command: `A+B+C`
5. Upload Configuration
6. Press button → A, B, and C are pressed simultaneously!

## Example Use Cases

### Gaming
- **Flight Simulators**: Map complex commands to physical switches
- **Racing Games**: Gear shifters, handbrake buttons
- **MMOs**: Macro buttons for ability combinations
- **FPS Games**: Quick weapon switches, crouch+jump combos

### Productivity
- **Video Editing**: Shuttle controls for scrubbing timeline
- **Music Production**: Transport controls, keyboard shortcuts
- **Photo Editing**: Zoom, brush size controls
- **Programming**: Code snippets, compile shortcuts

### Accessibility
- **One-Button Commands**: Complex shortcuts from single button
- **Custom Layouts**: Ergonomic button placement
- **Macro Support**: Reduce repetitive strain

## Architecture

KeyFlight uses a dual-board architecture to overcome Arduino Mega's lack of USB HID support:

```
┌────────────────┐
│  KeyFlight     │
│  Connector (PC)│
└────────┬───────┘
         │ USB Serial
┌────────▼──────────────┐
│  Arduino Mega 2560    │
│  (Input Handler)      │
│  - Reads buttons      │
│  - Reads encoders     │
│  - Manages config     │
└────────┬──────────────┘
         │ Serial1 (UART)
┌────────▼──────────────┐
│  Arduino Pro Micro    │
│  (HID Output)         │
│  - Receives commands  │
│  - Sends USB keyboard │
└───────────────────────┘
```

## Supported Input Types

| Input Type | Description | Example Use |
|------------|-------------|-------------|
| **Button** | Momentary push button | Fire button, hotkeys |
| **Encoder** | Rotary encoder with detents | Volume, zoom, throttle |
| **Switch** | Toggle switch (ON/OFF) | Landing gear, lights |
| **Analog** | Potentiometer, slider | Fine control, analog axes |

## Keyboard Command Syntax

KeyFlight supports flexible keyboard command formats:

```
A              - Single key
CTRL+C         - Modifier + key
CTRL+ALT+DEL   - Multiple modifiers
A+B+C          - Multiple keys simultaneously
SHIFT+F1       - Modifier + special key
UPARROW        - Arrow keys
```

**Supported Modifiers:**
- `CTRL` (Left Ctrl)
- `ALT` (Left Alt)
- `SHIFT` (Left Shift)
- `WIN` (Windows/Command key)

**Special Keys:**
- Arrow keys: `UPARROW`, `DOWNARROW`, `LEFTARROW`, `RIGHTARROW`
- Function keys: `F1` through `F12`
- Navigation: `HOME`, `END`, `PAGEUP`, `PAGEDOWN`
- Editing: `ESC`, `TAB`, `ENTER`, `BACKSPACE`, `DELETE`, `INSERT`
- Common: `SPACE`, `CAPSLOCK`

## Project Structure

```
KeyFlight/
├── KeyFlight-Mega/              # Mega 2560 firmware (PlatformIO)
│   ├── src/
│   │   ├── main.cpp
│   │   ├── CommandMessenger.cpp
│   │   ├── MF_Modules/          # Input modules
│   │   │   ├── MF_Button/
│   │   │   ├── MF_Encoder/
│   │   │   ├── MF_Switch/
│   │   │   └── MF_Analog/
│   │   └── HIDMessenger.cpp
│   └── platformio.ini
│
├── KeyFlight-ProMicro/          # Pro Micro firmware (PlatformIO)
│   ├── src/
│   │   ├── main.cpp
│   │   ├── KeyboardParser.cpp
│   │   └── KeyboardExecutor.cpp
│   └── platformio.ini
│
├── KeyFlightConnector/          # Windows application (C#)
│   ├── KeyFlightConnector.sln
│   └── KeyFlightConnector/
│       ├── Forms/
│       ├── Communication/
│       ├── Models/
│       └── Services/
│
├── docs/                        # Documentation
│   ├── hardware-wiring.md
│   ├── firmware-guide.md
│   └── user-guide.md
│
├── ARCHITECTURE.md              # Technical architecture
└── README.md                    # This file
```

## Configuration Example

```json
{
  "version": "1.0",
  "deviceName": "Flight Controller",
  "modules": [
    {
      "id": 0,
      "type": "Button",
      "name": "Fire",
      "pin": 2,
      "keyCommand": "SPACE"
    },
    {
      "id": 1,
      "type": "Encoder",
      "name": "Throttle",
      "pin": 3,
      "pin2": 4,
      "keyCommandCW": "W",
      "keyCommandCCW": "S"
    },
    {
      "id": 2,
      "type": "Switch",
      "name": "Landing Gear",
      "pin": 5,
      "keyCommandOn": "G",
      "keyCommandOff": "G"
    }
  ]
}
```

## Development

### Prerequisites
- **Firmware**: PlatformIO, VS Code
- **PC App**: Visual Studio 2022, .NET Framework 4.8
- **Testing**: 2x Arduino boards, USB cables

### Building from Source

**Firmware:**
```bash
cd KeyFlight-Mega
pio run

cd KeyFlight-ProMicro
pio run
```

**PC Application:**
```bash
cd KeyFlightConnector
dotnet build
# Or open in Visual Studio 2022
```

### Contributing
Contributions welcome! Please read [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines.

## Comparison: KeyFlight vs MobiFlight

KeyFlight is **inspired by** MobiFlight's architecture, but serves a different purpose:

| Feature | MobiFlight | KeyFlight |
|---------|------------|-----------|
| **Purpose** | Flight simulator cockpit | Keyboard automation |
| **Outputs** | Sim variables, LEDs, servos | USB HID keyboard |
| **Target** | Flight sim enthusiasts | Gamers, productivity users |
| **Hardware** | Single Arduino | Dual Arduino (Mega+ProMicro) |
| **Compatibility** | MSFS, X-Plane, FSX | Any software accepting keyboard |

**Why not use MobiFlight directly?**
- MobiFlight is designed for flight simulators, not general keyboard automation
- MobiFlight requires flight sim software running
- KeyFlight works standalone with any application
- KeyFlight is optimized for Xbox compatibility

## FAQ

**Q: Why two Arduinos?**
A: Arduino Mega 2560 has 54 I/O pins (great for many inputs) but cannot act as a USB HID keyboard. Pro Micro has native USB HID but only 18 I/O pins. Using both gives us maximum inputs + HID output.

**Q: Can I use just one Arduino?**
A: Yes, but with limitations:
- Arduino Leonardo/Micro: 18 I/O pins, has USB HID
- Arduino Mega: 54 I/O pins, no USB HID (requires PC software to send keyboard commands)

**Q: Does this work on Xbox?**
A: Yes! The Pro Micro appears as a standard USB keyboard. Xbox recognizes it automatically. However, not all games accept keyboard input.

**Q: What's the maximum number of inputs?**
A: Up to 40 configurable inputs (limited by EEPROM). Mega has 54 I/O pins, so physical limit is higher.

**Q: Can I send text strings?**
A: Not currently. KeyFlight sends keyboard commands (like pressing keys), not typing text. Future feature!

**Q: Is it compatible with MobiFlight Connector?**
A: No. KeyFlight uses a different protocol and purpose. However, the modular architecture is inspired by MobiFlight.

## Troubleshooting

### Pro Micro Upload Fails
- **Solution**: Select "Arduino Leonardo" (NOT Pro Micro) in board selection
- Double-tap reset button to enter bootloader (8-second window)

### Mega Not Detected
- Check USB cable (must be data cable, not power-only)
- Install Arduino drivers: https://www.arduino.cc/en/software
- Check Device Manager for COM port

### Keyboard Commands Not Working
- Verify Serial1 wiring (TX→RX crossover, not TX→TX)
- Confirm both boards at 115200 baud
- Check Pro Micro appears as keyboard in Device Manager
- Test Pro Micro with simple HID sketch

### Configuration Not Saving
- Power cycle Mega after upload
- Check Serial Monitor for "Configuration saved" message
- Verify EEPROM checksum valid

## Credits

- **Architecture Inspiration**: [MobiFlight Team](https://github.com/MobiFlight) - Thank you for the excellent modular design patterns!
- **CommandMessenger**: Communication protocol based on MobiFlight's implementation
- **Community**: Arduino and flight sim communities for inspiration

## License

MIT License - See [LICENSE](LICENSE) for details

MobiFlight is copyright © MobiFlight Team. KeyFlight is an independent project using similar architectural patterns with gratitude and respect.

---

**Status**: 🚧 In Development (Architecture Design Phase)
**Version**: 0.1.0-dev
**Created**: 2025-11-19
**Maintainer**: @bworthy89
