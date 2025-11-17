# Arduino Input Configurator

A comprehensive system for configuring Arduino Mega 2560 (ELEGOO MEGA R3) inputs to send USB HID keyboard commands via a graphical interface.

## Overview

Transform your Arduino Mega into a powerful custom input device with up to 40 configurable inputs (buttons, rotary encoders, switches, and potentiometers) that send keyboard commands to your computer. Perfect for:

- 🎮 Gaming controllers
- 🎚️ Audio/video editing control surfaces
- 🔬 Laboratory equipment interfaces
- 🎹 MIDI controllers
- ✈️ Flight simulator panels
- 🤖 Industrial control panels

## Features

✅ **Up to 40 Inputs**: Buttons, rotary encoders, switches, and potentiometers
✅ **USB HID Keyboard**: Native keyboard emulation (Ctrl+ commands)
✅ **Encoder Modes**: 4 increment modes (1x, 10x, 100x, 1000x)
✅ **User-Friendly GUI**: PyQt5 desktop application for easy configuration
✅ **Auto-Update**: Automatic update checking and one-click installation from GitHub
✅ **Persistent Storage**: EEPROM-based configuration (survives power cycles)
✅ **Pin Selection**: Visual pin assignment with validation
✅ **Custom Naming**: Name each input for easy identification
✅ **Real-Time Testing**: Test keyboard commands directly from GUI
✅ **Cross-Platform**: Works on Windows, macOS, and Linux

## System Architecture

```
Physical Inputs → Arduino Mega 2560 → Arduino Pro Micro → PC (USB HID Keyboard)
                         ↑                                     ↓
                         └──────── Python GUI (Config) ────────┘
```

### Hardware Requirements

**Essential:**
- Arduino Mega 2560 (ELEGOO MEGA R3 or compatible)
- Arduino Pro Micro (ATmega32u4) or Arduino Leonardo
- 2x USB cables (USB-B for Mega, Micro USB for Pro Micro)
- 3x jumper wires (for board-to-board connection)

**Inputs (as needed):**
- Rotary encoders with push buttons
- Momentary push buttons
- Toggle switches
- 10kΩ potentiometers
- 0.1µF capacitors (for debouncing)

**Estimated Cost**: $50-$90 for complete 30-input setup

### Software Requirements

- **Arduino IDE**: 1.8.13+ or 2.x
- **Python**: 3.7+
- **Libraries**:
  - Arduino: ArduinoJson, Bounce2, Encoder
  - Python: PyQt5, pyserial

## Quick Start

### 🚀 Easiest Way (Windows)

**Don't have Python installed?**
1. **Double-click** `install-python.bat` (installs Python automatically)
2. Close and reopen Command Prompt
3. **Double-click** `launch-gui.bat` (installs packages and opens GUI)

**Already have Python?**
1. **Double-click** `launch-gui.bat` (that's it!)

The launcher automatically checks for Python, installs dependencies, and launches the GUI!

See **[LAUNCHER_README.md](LAUNCHER_README.md)** for details.

---

### 📖 Manual Setup

### 1. Hardware Setup

Wire the two Arduino boards together:
- Mega Pin 18 (TX1) → Pro Micro RXI
- Mega Pin 19 (RX1) → Pro Micro TXO
- Mega GND → Pro Micro GND

See `docs/hardware-wiring.md` for detailed wiring diagrams.

### 2. Install Arduino Libraries

Open Arduino IDE → Tools → Manage Libraries, install:
- **ArduinoJson** (v6.x)
- **Bounce2**
- **Encoder**

### 3. Upload Firmware

**Pro Micro:**
1. Open `arduino-config-promicro/arduino-config-promicro.ino`
2. Select: Tools → Board → Arduino Leonardo
3. Select: Tools → Port → (Pro Micro port)
4. Upload

**Mega 2560:**
1. Open `arduino-config-mega/arduino-config-mega.ino`
2. Select: Tools → Board → Arduino Mega or Mega 2560
3. Select: Tools → Processor → ATmega2560
4. Select: Tools → Port → (Mega port)
5. Upload

### 4. Install Python GUI (Manual Method)

**Option A: Use launcher (recommended)**
```cmd
launch-gui.bat
```

**Option B: Manual install**
```bash
cd gui
pip install -r requirements.txt
python main.py
```

### 5. Configure Inputs

1. Click "Connect" to connect to Arduino Mega
2. Click "+ Add Input" to add each input
3. Configure: Name, Type, Pins, Keyboard Command
4. Click "Upload to Arduino"
5. Test your inputs!

## Documentation

- **[Launcher Scripts Guide](LAUNCHER_README.md)**: Auto-install Python and dependencies (Windows)
- **[Auto-Update Guide](docs/auto-update-guide.md)**: Automatic updates from GitHub releases
- **[Python Installation](docs/python-installation.md)**: Manual Python installation guide
- **[User Guide](docs/user-guide.md)**: Complete setup and usage instructions
- **[Hardware Wiring](docs/hardware-wiring.md)**: Detailed wiring diagrams and pin assignments
- **[Troubleshooting](docs/user-guide.md#troubleshooting)**: Common issues and solutions

## Configuration Examples

### Flight Simulator Controller

| Name | Type | Pin 1 | Pin 2 | Command |
|------|------|-------|-------|---------|
| Fire | Button | 22 | N/A | CTRL+F |
| Throttle Up | Button | 24 | N/A | CTRL+UPARROW |
| Throttle Down | Button | 25 | N/A | CTRL+DOWNARROW |
| Trim | Encoder | 2 | 3 | CTRL+T |
| Zoom | Encoder | 20 | 21 | CTRL+Z |

### Audio Production Control Surface

| Name | Type | Pin 1 | Pin 2 | Command |
|------|------|-------|-------|---------|
| Play/Pause | Button | 30 | N/A | CTRL+SPACE |
| Record | Button | 32 | N/A | CTRL+R |
| Volume | Encoder | 2 | 3 | CTRL+V |
| Pan | Encoder | 20 | 21 | CTRL+P |

## Supported Keyboard Commands

All commands must start with `CTRL+`:

- **Letters**: `CTRL+A` through `CTRL+Z`
- **Numbers**: `CTRL+0` through `CTRL+9`
- **Function Keys**: `CTRL+F1` through `CTRL+F12`
- **Arrow Keys**: `CTRL+UPARROW`, `CTRL+DOWNARROW`, `CTRL+LEFTARROW`, `CTRL+RIGHTARROW`
- **Special Keys**: `CTRL+SPACE`, `CTRL+ENTER`, `CTRL+TAB`, `CTRL+ESC`, etc.

See [User Guide](docs/user-guide.md#keyboard-commands) for complete list.

## Rotary Encoder Increment Modes

Encoders can send multiple commands per click:

- **1x Mode**: 1 command per click (precise control)
- **10x Mode**: 10 commands per click (medium speed)
- **100x Mode**: 100 commands per click (fast traversal)
- **1000x Mode**: 1000 commands per click (coarse adjustment)

Perfect for applications requiring both fine and coarse control!

## Project Structure

```
arduino/
├── launch-gui.bat                # 🚀 Easy launcher - checks Python, installs deps, runs GUI
├── install-python.bat            # 🐍 Auto-install Python (Windows)
├── install-python.ps1            # PowerShell installer script
├── LAUNCHER_README.md            # Launcher scripts documentation
├── arduino-config-mega/          # Mega 2560 firmware
│   └── arduino-config-mega.ino   # Input handler & config manager
├── arduino-config-promicro/      # Pro Micro firmware
│   └── arduino-config-promicro.ino # USB HID keyboard bridge
├── gui/                          # Python GUI application
│   ├── main.py                   # Entry point
│   ├── arduino_configurator.py   # Main GUI code
│   └── requirements.txt          # Python dependencies
├── docs/                         # Documentation
│   ├── python-installation.md    # Manual Python install guide
│   ├── hardware-wiring.md        # Wiring guide
│   └── user-guide.md             # Complete user manual
└── README.md                     # This file
```

## How It Works

### Architecture Flow

1. **Physical Input**: User presses button, turns encoder, etc.
2. **Mega 2560**: Reads input, retrieves keyboard command from EEPROM
3. **Serial Communication**: Mega sends command to Pro Micro via UART (Serial1)
4. **USB HID**: Pro Micro sends keyboard command to PC via USB
5. **PC**: Receives keyboard input (e.g., Ctrl+F)

### Why Two Boards?

The Arduino Mega 2560 (ATmega2560) does NOT have native USB HID keyboard support. The dual-board approach provides:

✅ **Maximum Pin Count**: 54 digital + 16 analog pins (Mega)
✅ **Native HID Support**: Pro Micro (32u4 chip)
✅ **Easy Development**: No firmware hacking required
✅ **Reliability**: Proven, stable solution
✅ **Debuggability**: Separate concerns for troubleshooting

**Alternative**: Use Arduino Leonardo instead of Pro Micro (same chip, slightly different form factor)

## Technical Specifications

### Arduino Mega 2560
- **MCU**: ATmega2560
- **Digital I/O**: 54 pins
- **Analog Input**: 16 pins (A0-A15)
- **Interrupt Pins**: 6 (2, 3, 18, 19, 20, 21)
- **EEPROM**: 4KB (config storage)
- **Serial Ports**: 4 (Serial, Serial1, Serial2, Serial3)

### Arduino Pro Micro
- **MCU**: ATmega32u4
- **Native USB**: HID keyboard support
- **Clock**: 16 MHz
- **Voltage**: 5V

### Communication Protocol
- **Baud Rate**: 115200
- **Format**: JSON over serial (GUI ↔ Mega)
- **Format**: Newline-delimited text (Mega ↔ Pro Micro)

### Storage
- **EEPROM**: 4KB available on Mega
- **Config Size**: ~40 bytes per input
- **Max Inputs**: 40 (configurable in firmware)

## Troubleshooting

### Common Issues

**No serial ports detected:**
- Check USB cables
- Install CH340/CP2102 drivers (for clone boards)
- Try different USB ports

**Encoders missing steps:**
- Use interrupt pins (2, 3, 20, 21)
- Add 0.1µF capacitors for debouncing
- Don't turn too fast

**Buttons triggering multiple times:**
- Check Bounce2 library installed
- Increase debounce time in firmware
- Add 0.1µF capacitor across button

**No keyboard commands sent:**
- Verify Pro Micro connected to PC
- Check Serial1 wiring (TX→RX, RX→TX)
- Ensure GND connected between boards
- Verify commands start with "CTRL+"

See [Troubleshooting Guide](docs/user-guide.md#troubleshooting) for more solutions.

## Limitations

- **Modifier Keys**: Only Ctrl+ supported (can modify firmware for Shift/Alt)
- **Input Limit**: 40 inputs (can increase in firmware, limited by EEPROM)
- **Encoder Speed**: Very fast rotation may lose steps
- **Platform**: PC only (not game consoles)

## Future Enhancements

Potential improvements:
- [ ] Configuration export/import (JSON files)
- [ ] Multi-profile support
- [ ] Shift/Alt modifier support
- [ ] Mouse movement support
- [ ] OLED display for input names
- [ ] Encoder push button mode switching
- [ ] Analog value display (potentiometers)
- [ ] Web-based configuration UI

## Contributing

Contributions welcome! Areas for improvement:
- Additional keyboard modifier support
- GUI enhancements (drag-drop, themes)
- Configuration file import/export
- Additional input types (analog joysticks, etc.)
- Documentation translations

## License

This project is provided as-is for educational and personal use. Feel free to modify and adapt to your needs.

## Credits

Built using:
- **Arduino** platform and libraries
- **PyQt5** GUI framework
- **ArduinoJson** library by Benoit Blanchon
- **Bounce2** library by Thomas Fredericks
- **Encoder** library by Paul Stoffregen

## Support

For issues, questions, or feature requests:
1. Check the [User Guide](docs/user-guide.md)
2. Review [Troubleshooting](docs/user-guide.md#troubleshooting)
3. Check [Hardware Wiring](docs/hardware-wiring.md)

## Screenshots

### GUI Main Window
```
┌────────────────────────────────────────────┐
│  Arduino Input Configurator                │
├────────────────────────────────────────────┤
│  [Connection Panel]                        │
│  • Serial Port Selection                   │
│  • Connect/Disconnect Button               │
├────────────────────────────────────────────┤
│  [Configuration Table]                     │
│  • Add/Remove/Clear Inputs                 │
│  • Pin Assignment                          │
│  • Type Selection                          │
│  • Encoder Mode Selection                  │
│  • Keyboard Command Mapping                │
├────────────────────────────────────────────┤
│  [Console Output]                          │
│  • Real-time Status Messages               │
│  • Upload Confirmation                     │
│  • Test Results                            │
└────────────────────────────────────────────┘
```

---

**Ready to build your custom input device? Start with the [User Guide](docs/user-guide.md)!** 🚀
