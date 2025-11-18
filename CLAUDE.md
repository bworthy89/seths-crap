# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Arduino Input Configurator is a dual-board system that transforms an Arduino Mega 2560 into a custom USB HID keyboard with up to 40 configurable inputs (buttons, rotary encoders, switches, potentiometers). Configuration is managed through a PyQt5 desktop GUI application.

**Hardware Architecture:**
- Arduino Mega 2560 (ATmega2560): Handles physical input reading and configuration storage
- Arduino Pro Micro (ATmega32u4): Provides native USB HID keyboard emulation
- Serial communication bridge (Serial1 on Mega → Pro Micro via UART at 115200 baud)

## Development Commands

### Python GUI Application

```bash
# Install dependencies
cd gui
pip install -r requirements.txt

# Run the GUI application
python main.py

# Or use the Windows launcher (installs dependencies automatically)
launch-gui.bat
```

### Arduino Firmware

**Upload to Pro Micro:**
1. Open `arduino-config-promicro/arduino-config-promicro.ino` in Arduino IDE
2. Select Board: Tools → Board → Arduino Leonardo
3. Select Port: Tools → Port → (Pro Micro COM port)
4. Upload

**Upload to Mega 2560:**
1. Open `arduino-config-mega/arduino-config-mega.ino` in Arduino IDE
2. Select Board: Tools → Board → Arduino Mega or Mega 2560
3. Select Processor: Tools → Processor → ATmega2560
4. Select Port: Tools → Port → (Mega COM port)
5. Upload

**Required Arduino Libraries:**
- ArduinoJson (v6.x)
- Bounce2
- Encoder

Install via: Arduino IDE → Tools → Manage Libraries

## System Architecture

### Two-Board Communication Flow

```
Physical Input → Mega 2560 → Pro Micro → PC (USB HID)
                     ↑                        ↓
                     └─── Python GUI ─────────┘
```

1. **Mega 2560** (`arduino-config-mega.ino`):
   - Reads physical inputs from pins
   - Stores configuration in EEPROM (4KB)
   - Processes encoder rotation/button presses
   - Sends keyboard commands to Pro Micro via Serial1 (TX1/RX1)
   - Receives configuration from Python GUI via USB Serial

2. **Pro Micro** (`arduino-config-promicro.ino`):
   - Receives newline-delimited commands from Mega via Serial1
   - Translates to USB HID keyboard commands (Ctrl+ combinations)
   - Uses native ATmega32u4 USB HID support

3. **Python GUI** (`gui/main.py`, `gui/arduino_configurator.py`):
   - Connects to Mega 2560 via USB Serial (115200 baud)
   - Sends JSON configuration messages
   - Provides visual input configuration interface
   - Handles auto-update checking via `gui/updater.py`

### Configuration Protocol

**GUI → Mega (JSON over Serial):**
```json
{
  "command": "upload_config",
  "inputs": [
    {
      "pin": 22,
      "pin2": 0,
      "type": 1,
      "mode": 0,
      "name": "Fire Button",
      "keyCommand": "CTRL+F",
      "enabled": true
    }
  ]
}
```

**Mega → Pro Micro (Text over Serial1):**
```
A\n
L+N+A+V\n
CTRL+F\n
SHIFT+CTRL+S\n
UPARROW\n
```

### Input Types and Pin Considerations

**Input Types (defined in `arduino-config-mega.ino`):**
- `INPUT_BUTTON = 1`: Momentary push button (any digital pin)
- `INPUT_ENCODER = 2`: Rotary encoder (use interrupt pins: 2, 3, 18, 19, 20, 21)
- `INPUT_SWITCH = 3`: Toggle switch (any digital pin)
- `INPUT_POT = 4`: Potentiometer (analog pins A0-A15)

**Encoder Modes:**
- `MODE_1X = 0`: 1 command per click (fine control)
- `MODE_10X = 1`: 10 commands per click
- `MODE_100X = 2`: 100 commands per click
- `MODE_1000X = 3`: 1000 commands per click (coarse adjustment)

**Pin Assignment Rules:**
- Encoders MUST use interrupt pins for reliable operation
- Avoid pins 0-1 (reserved for USB Serial)
- Avoid pins 18-19 (Serial1 for Pro Micro communication)
- Maximum 40 inputs (configurable via `MAX_INPUTS` constant)

### EEPROM Configuration Storage

The Mega 2560 stores configuration persistently in EEPROM:
- Magic number (`0xAC01`) for validation
- Version field for future compatibility
- Array of `InputConfig` structs (~40 bytes each)
- Simple checksum for integrity checking

Configuration survives power cycles and is loaded automatically on boot.

## Code Organization

```
arduino-config-mega/
└── arduino-config-mega.ino    # Main firmware: input handling, EEPROM, serial protocol

arduino-config-promicro/
└── arduino-config-promicro.ino  # USB HID bridge: serial → keyboard

gui/
├── main.py                     # Entry point
├── arduino_configurator.py     # Main GUI application (PyQt5)
├── updater.py                  # GitHub release auto-update system
└── requirements.txt            # PyQt5==5.15.10, pyserial==3.5

docs/
├── user-guide.md              # Complete setup and usage instructions
├── hardware-wiring.md         # Wiring diagrams and pin assignments
├── auto-update-guide.md       # Update system documentation
└── python-installation.md     # Manual Python setup guide
```

## Key Implementation Details

### Debouncing and Input Processing

Buttons and switches use the Bounce2 library with 5ms debounce time (configurable via `DEBOUNCE_MS`). Encoders use the Encoder library and require hardware on interrupt pins for reliable step detection.

### Serial Communication

- USB Serial (Mega ↔ PC): 115200 baud, JSON protocol
- Serial1 (Mega ↔ Pro Micro): 115200 baud, newline-delimited text
- Pro Micro uses 10ms key hold time for reliable HID registration

### Keyboard Command Format

The system supports flexible keyboard commands using the `+` delimiter to combine multiple keys:

**Single Keys:**
- Letters: `A`, `B`, `C`, etc.
- Numbers: `0`, `1`, `2`, etc.
- Special keys: `UPARROW`, `DOWNARROW`, `LEFTARROW`, `RIGHTARROW`, `ENTER`, `TAB`, `SPACE`, `ESC`, etc.
- Function keys: `F1` through `F24`

**Multiple Simultaneous Keys:**
- `L+N+A+V` - Presses L, N, A, and V simultaneously
- `A+B+C` - Presses A, B, and C simultaneously

**Modifier Keys:**
- Supported modifiers: `CTRL`, `SHIFT`, `ALT`, `GUI` (Windows key)
- With single key: `CTRL+A`, `SHIFT+B`, `ALT+F4`
- Multiple modifiers: `CTRL+SHIFT+S`, `SHIFT+ALT+DELETE`
- Modifiers with multiple keys: `CTRL+SHIFT+A+B`

**Examples:**
- `A` - Single key A
- `CTRL+C` - Ctrl+C (copy)
- `CTRL+SHIFT+V` - Ctrl+Shift+V (paste special)
- `L+N+A+V` - L, N, A, V pressed together
- `CTRL+F1` - Ctrl+F1
- `SHIFT+UPARROW` - Shift+Up Arrow

The Pro Micro converts commands to uppercase automatically, so `a`, `A`, `ctrl+a`, and `CTRL+A` are all equivalent.

See `pressKey()` and `pressSpecialKey()` in `arduino-config-promicro.ino` for the complete key mapping.

## Wiring Between Boards

Critical connections:
- Mega Pin 18 (TX1) → Pro Micro RXI
- Mega Pin 19 (RX1) → Pro Micro TXO
- Mega GND → Pro Micro GND

These connections are REQUIRED for the system to function. See `docs/hardware-wiring.md` for detailed diagrams.

## Common Development Scenarios

### Testing Configuration Changes

1. Connect Mega 2560 to PC via USB
2. Run `python gui/main.py`
3. Click "Connect" and select the Mega's COM port
4. Add/modify inputs in the GUI
5. Click "Upload to Arduino"
6. Configuration is written to EEPROM and takes effect immediately

### Debugging Serial Communication

- USB Serial monitor (115200 baud): Shows JSON messages between GUI and Mega
- Serial1 monitor: Not accessible without additional hardware
- LED on Pin 13: Indicates Mega is running
- Pro Micro LED: Blinks on activity, flashes on command

### Adding New Input Types

1. Define new `InputType` enum in `arduino-config-mega.ino`
2. Add corresponding input class/logic in `setup()` and `loop()`
3. Update GUI's `input_types` dictionary in `arduino_configurator.py`
4. Add table widget configuration in GUI if needed

### Extending Keyboard Commands

**Adding new special keys:**
1. Add new key mappings in `pressSpecialKey()` in `arduino-config-promicro.ino`
2. Update keyboard command documentation in README.md and user-guide.md
3. No changes needed in Mega firmware or GUI

**Increasing simultaneous key limit:**
1. In `arduino-config-promicro.ino`, change `String keys[10]` array size in `processCommand()` (line 99)
2. Update `if (keyCount < 10)` condition to match new limit (line 105)
3. Update CLAUDE.md limitations section

**Adding new modifiers:**
1. Add new modifier detection in `pressKey()` function (around line 137-144)
2. Use appropriate `KEY_LEFT_*` or `KEY_RIGHT_*` constants from Keyboard library

## Auto-Update System

The GUI includes automatic update checking via `updater.py`:
- Checks GitHub releases for newer versions
- Compares semantic versions (e.g., 1.0.0 vs 1.1.0)
- Downloads and extracts updates automatically
- Requires GitHub repository with tagged releases

Current version is stored in `VERSION` file (format: `MAJOR.MINOR.PATCH`)

## Testing Without Hardware

The GUI can be tested without Arduino hardware connected:
- Serial port detection will show no ports
- Configuration table and UI elements still functional
- Upload operations will fail gracefully with error messages
- Useful for GUI development and testing

For firmware testing without complete hardware:
- Mega can be tested standalone (inputs won't send keyboard commands)
- Pro Micro can be tested with manual Serial1 input (simulated from Mega)

## Platform-Specific Notes

**Windows:**
- Use `.bat` launcher scripts for easy setup
- May require CH340/CP2102 drivers for clone Arduino boards
- PowerShell script available: `install-python.ps1`

**macOS/Linux:**
- Manual Python setup required
- Serial port names differ (e.g., `/dev/ttyACM0` vs `COM3`)
- May need `sudo` for serial port access on Linux
- Consider adding user to `dialout` group on Linux

## Limitations and Constraints

- Maximum 10 simultaneous keys per command (limited by array size in Pro Micro firmware)
- 40 input maximum (limited by EEPROM size and MAX_INPUTS constant)
- Encoder step loss possible at very high rotation speeds
- No support for mouse movement (keyboard only)
- EEPROM has ~100,000 write cycle lifetime (configuration uploads count as writes)
- Commands are case-insensitive (automatically converted to uppercase)
