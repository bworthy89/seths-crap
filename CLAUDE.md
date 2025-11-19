# Arduino Input Configurator - Development Guide

**Project:** Arduino ELEGOO MEGA R3 Input Configurator with Pro Micro HID
**Repository:** https://github.com/bworthy89/seths-crap
**Primary Goal:** GUI-based configuration system for assigning buttons, encoders, switches, and potentiometers to keyboard commands for Xbox/console compatibility

---

## Architecture Overview

This is a **dual-board system** designed to overcome the Arduino Mega 2560's lack of native USB HID support:

### Hardware Components
1. **Arduino Mega 2560 (ELEGOO MEGA R3)**
   - Input handler: reads buttons, rotary encoders, switches, potentiometers
   - Configuration storage: EEPROM with checksums and magic numbers
   - Communication: Serial1 (UART, pins 18/19) to Pro Micro at 115200 baud
   - USB Serial: Configuration interface to PC GUI at 115200 baud

2. **Arduino Pro Micro (ATmega32u4)**
   - USB HID keyboard emulation: native USB support
   - Receives serial commands from Mega
   - Converts to keyboard HID commands (Ctrl+key combinations)
   - **Critical:** Must be uploaded with "Arduino Leonardo" board selection

### Software Components
1. **Python GUI (PyQt5)** - `gui/arduino_configurator.py`
   - Visual configuration interface
   - Serial communication with Mega
   - Auto-update system via GitHub API
   - User-friendly input assignment

2. **Arduino Firmware**
   - `arduino-config-mega/arduino-config-mega.ino` - Mega firmware
   - `arduino-config-promicro/arduino-config-promicro.ino` - Pro Micro firmware

3. **Windows Launchers**
   - `launch-gui.bat` - One-click launcher with dependency checking
   - `install-python.bat` - Automated Python installation

---

## Build Commands

### Arduino Firmware

**For Arduino Mega:**
```
1. Open Arduino IDE
2. Tools → Board → Arduino AVR Boards → Arduino Mega or Mega 2560
3. Tools → Processor → ATmega2560 (Mega 2560)
4. Tools → Port → Select Mega's COM port
5. Open: arduino-config-mega/arduino-config-mega.ino
6. Sketch → Upload (Ctrl+U)
```

**For Arduino Pro Micro (CRITICAL BOARD SELECTION):**
```
1. Open Arduino IDE
2. Tools → Board → Arduino AVR Boards → Arduino Leonardo (NOT Pro Micro!)
3. Tools → Port → Select Pro Micro's COM port
4. Open: arduino-config-promicro/arduino-config-promicro.ino
5. Sketch → Upload (Ctrl+U)
6. If upload fails: Double-tap reset button, then immediately upload
```

**Required Arduino Libraries:**
- ArduinoJson (v6.x)
- Bounce2
- Encoder

Install via: Tools → Manage Libraries → Search and install each

### Python GUI

**Installation:**
```batch
# Method 1: Automated (Windows)
install-python.bat
launch-gui.bat

# Method 2: Manual
python -m pip install -r gui/requirements.txt
cd gui
python main.py
```

**Requirements:**
- Python 3.8+
- PyQt5==5.15.10
- pyserial==3.5
- keyboard==0.13.5 (for daemon mode, optional)

---

## Testing Commands

### Hardware Test 1: Mega Upload Test
```
1. Upload Blink sketch to Mega
2. Verify LED on pin 13 blinks every second
3. Confirms: Mega uploads working, Arduino IDE configured correctly
```

### Hardware Test 2: Pro Micro HID Test
```cpp
// Upload this to Pro Micro (Board: Arduino Leonardo)
#include <Keyboard.h>

void setup() {
  Keyboard.begin();
  delay(2000);
  Keyboard.println("Pro Micro HID Working!");
}

void loop() {
  delay(3000);
  Keyboard.println("Still working!");
}

// Expected: Text appears in Notepad every 3 seconds
```

### Hardware Test 3: Serial Communication Test
```
1. Wire: Mega TX1(18) → Pro Micro RXI(0)
2. Wire: Mega RX1(19) → Pro Micro TXO(1)
3. Wire: Mega GND → Pro Micro GND
4. Upload test sketches to both boards
5. Open Serial Monitor for Mega
6. Send test message
7. Verify Pro Micro LED blinks (confirms serial received)
```

### GUI Test
```batch
launch-gui.bat
1. Connect Arduino Mega via USB
2. Click "Connect"
3. Verify connection status shows "Connected"
4. Add test input (Button on Pin 2, Ctrl+F)
5. Click "Upload Configuration"
6. Press physical button
7. Verify Ctrl+F command works in text editor
```

---

## Key Development Workflows

### Workflow 1: Adding a New Input Type
1. Update `InputType` enum in `arduino-config-mega.ino`
2. Add handler in `processInputs()` function
3. Update GUI `input_types` dictionary in `arduino_configurator.py`
4. Add UI controls for new type-specific parameters
5. Update JSON serialization in both Arduino and Python

### Workflow 2: Modifying EEPROM Configuration
**CRITICAL:** Changing EEPROM structure requires:
1. Increment `CONFIG_MAGIC` constant (e.g., 0xAC01 → 0xAC02)
2. Update `Config` struct in Arduino firmware
3. Modify `calculateChecksum()` to include new fields
4. Update GUI's JSON generation
5. Users must reconfigure (old configs invalidated)

### Workflow 3: Creating a GitHub Release
```bash
# 1. Update version
echo "1.1.0" > VERSION

# 2. Commit changes
git add .
git commit -m "Release v1.1.0: Description of changes"
git push

# 3. Create GitHub release
git tag v1.1.0
git push origin v1.1.0

# 4. On GitHub.com:
#    - Go to Releases → Draft a new release
#    - Select tag: v1.1.0
#    - Upload: Zipped project files
#    - Publish release
```

The auto-updater will detect new releases via GitHub API.

### Workflow 4: Debugging Pro Micro Issues
See: `PRO_MICRO_DEBUG.md` (comprehensive 490-line guide)

Common issues:
- **Wrong board selected:** MUST be "Arduino Leonardo" not "Mega"
- **Port not found:** Check Device Manager, try different cable/port
- **Upload fails:** Double-tap reset button to enter bootloader mode
- **No keyboard output:** Verify Keyboard.h included, check USB connection
- **TX/RX not working:** Ensure TX→RX crossover, check GND connection

---

## Important Technical Details

### Serial Protocol (Mega ↔ Pro Micro)

**Mega sends to Pro Micro:**
```
Format: "CTRL+<KEY>\n"
Examples:
  "CTRL+F\n"
  "CTRL+UPARROW\n"
  "CTRL+1\n"
```

**Pro Micro processes:**
- Parses command string
- Maps special keys (UPARROW → KEY_UP_ARROW)
- Sends USB HID keyboard command
- Releases all keys after 10ms

### EEPROM Configuration Structure

```cpp
struct InputConfig {
  uint8_t pin;           // Primary pin (0-69 on Mega)
  uint8_t pin2;          // Secondary pin (encoders only)
  uint8_t type;          // 1=Button, 2=Encoder, 3=Switch, 4=Pot
  uint8_t mode;          // Encoder: 0=1x, 1=10x, 2=100x, 3=1000x
  char name[20];         // User-defined name
  char keyCommand[16];   // "CTRL+F" format
  bool enabled;          // Is this input active?
};

struct Config {
  uint16_t magic;        // 0xAC01 (dual-board), 0xAC02 (solo)
  uint8_t version;       // 1
  InputConfig inputs[40]; // Array of inputs
  uint16_t checksum;     // Validation
};
```

**Storage:** EEPROM address 0, total size ~1KB

### JSON Configuration Protocol

**GUI → Mega (via USB Serial):**
```json
{
  "type": "config",
  "inputs": [
    {
      "pin": 2,
      "pin2": 0,
      "type": 1,
      "mode": 0,
      "name": "Fire Button",
      "key": "CTRL+F"
    }
  ]
}
```

**Mega → GUI (status response):**
```json
{
  "status": "ok",
  "mode": "dual",
  "configLoaded": true,
  "maxInputs": 40,
  "activeInputs": 5
}
```

### Rotary Encoder Handling

```cpp
// Encoder position read (with detent handling)
long newPosition = encoders[i]->read() / 4;
int change = newPosition - encoderPositions[i];

// Apply increment mode
int multipliers[] = {1, 10, 100, 1000};
int actualChange = change * multipliers[mode];

// Send multiple commands (limited to 100)
for (int j = 0; j < abs(actualChange) && j < 100; j++) {
  sendKeyCommand(keyCommand);
  delay(20);
}
```

**Best practices:**
- Use interrupt-capable pins (2, 3, 18, 19, 20, 21 on Mega)
- 20ms delay between commands prevents overwhelming Pro Micro
- Divide encoder.read() by 4 for detent clicks

---

## Known Limitations and Constraints

### Hardware Limitations
1. **Mega 2560 has NO native USB HID support**
   - Cannot emulate keyboard directly
   - Requires Pro Micro or similar HID-capable board
   - Alternative: CH9328/CH9329 serial-to-HID module

2. **Pro Micro bootloader timeout**
   - 8 second window after double-reset
   - Must upload immediately after entering bootloader
   - COM port changes during bootloader mode

3. **EEPROM write cycles**
   - ~100,000 write cycles per address
   - Configuration changes are infrequent, not a practical concern
   - Wear leveling not implemented

### Software Limitations
1. **Maximum 40 inputs**
   - Configurable via MAX_INPUTS constant
   - Limited by EEPROM size (4KB on Mega)
   - Current config uses ~1KB

2. **Serial buffer size**
   - JSON messages limited by Serial buffer (64 bytes default)
   - ArduinoJson uses 4096-byte static buffer
   - Large configs may need chunking (not currently implemented)

3. **Encoder command flooding**
   - Fast encoder rotation can queue many commands
   - Limited to 100 commands per rotation to prevent overflow
   - 20ms delay between commands

4. **Windows-only auto-installer**
   - `install-python.bat` only works on Windows
   - Linux/Mac users must install Python manually
   - GUI itself is cross-platform (PyQt5)

### Xbox/Console Compatibility
1. **Pro Micro must be plugged into Xbox USB**
   - Xbox recognizes it as standard USB keyboard
   - Mega can be powered separately (5V external) or by Xbox

2. **Not all games accept keyboard input**
   - Xbox system UI accepts keyboard
   - Game compatibility varies by title
   - No software solution exists for keyboard-blocking games

3. **No PC daemon on Xbox**
   - `keyboard_daemon.py` only works on PC
   - `arduino-config-mega-solo.ino` requires PC
   - Xbox requires hardware HID (Pro Micro essential)

---

## File Structure Reference

```
arduino/
├── arduino-config-mega/
│   └── arduino-config-mega.ino          # Main Mega firmware (dual-board)
├── arduino-config-promicro/
│   └── arduino-config-promicro.ino      # Pro Micro HID firmware
├── arduino-config-mega-solo/
│   └── arduino-config-mega-solo.ino     # Mega-only (PC daemon required)
├── gui/
│   ├── main.py                          # GUI entry point
│   ├── arduino_configurator.py          # Main GUI application
│   ├── updater.py                       # Auto-update system
│   ├── keyboard_daemon.py               # PC daemon (solo mode only)
│   └── requirements.txt                 # Python dependencies
├── launch-gui.bat                       # Windows GUI launcher
├── install-python.bat                   # Automated Python installer
├── install-python.ps1                   # PowerShell Python installer
├── git-init.bat                         # GitHub repository setup
├── VERSION                              # Current version (1.0.0)
├── README.md                            # User documentation
├── PRO_MICRO_DEBUG.md                   # Comprehensive Pro Micro troubleshooting
├── MEGA_PROMICRO_QUICK_FIX.md          # Quick-start debugging guide
├── HARDWARE_ALTERNATIVES.md             # Single-board alternatives
├── TROUBLESHOOTING_NO_KEYBOARD.md       # Keyboard output debugging
└── CLAUDE.md                            # This file
```

---

## Common Issues and Solutions

### Issue: "Port not found" in GUI
**Diagnosis:** Arduino not detected by serial library
**Solutions:**
1. Check USB cable (many are power-only, no data)
2. Try different USB port (prefer USB 2.0 rear panel)
3. Install Arduino drivers: https://www.arduino.cc/en/software
4. Check Device Manager for COM port number
5. Restart computer to reset USB stack

### Issue: Pro Micro won't upload
**Diagnosis:** Bootloader not entered or wrong board selected
**Solutions:**
1. **CRITICAL:** Select "Arduino Leonardo" board (NOT "Arduino Mega")
2. Double-tap reset button (0.5 seconds between taps)
3. Upload immediately (8 second window)
4. Note: COM port changes during bootloader
5. Try different USB cable/port
6. See: PRO_MICRO_DEBUG.md lines 139-178

### Issue: Keyboard commands not working
**Diagnosis:** Pro Micro not sending HID or not receiving serial
**Solutions:**
1. Verify Pro Micro shows under "Keyboards" in Device Manager
2. Test with Pro Micro HID test sketch (see Testing Commands)
3. Check TX/RX wiring: Mega TX1(18)→Pro Micro RX(0), crossover!
4. Verify GND connection between boards
5. Confirm both boards using 115200 baud
6. Check Serial Monitor for "Configuration loaded" on Mega
7. See: PRO_MICRO_DEBUG.md lines 262-302

### Issue: Configuration not saving
**Diagnosis:** EEPROM write failure or checksum mismatch
**Solutions:**
1. Check Serial Monitor for "Configuration saved" message
2. Power cycle Mega after config save
3. Verify magic number matches in code (0xAC01)
4. If corrupted: Upload fresh firmware to reset EEPROM
5. Check: arduino-config-mega.ino:130-151

### Issue: Auto-update not working
**Diagnosis:** GitHub API not accessible or version parsing failure
**Solutions:**
1. Check internet connection
2. Verify GitHub repository is public
3. Check VERSION file contains valid semver (e.g., "1.0.0")
4. Check GitHub rate limit (60 requests/hour unauthenticated)
5. See: gui/updater.py:28-71

---

## Development Notes

### Why Dual-Board Architecture?
The Arduino Mega 2560 uses an ATmega2560 microcontroller with an ATmega16U2 USB-to-serial converter chip. The 16U2 only handles serial communication, NOT USB HID. The Pro Micro uses ATmega32u4, which has native USB HID support built into the main chip.

**Alternatives researched:**
- **CH9328/CH9329 module:** $2-7, simple serial-to-HID, best single-board option
- **HoodLoader2:** Free firmware for 16U2, complex to install, voids warranty
- **ESP32-S3:** Modern alternative, native USB HID, WiFi bonus
- **Teensy 3.6/4.1:** Premium option, more pins, native HID, $30
- **Arduino Leonardo:** Has native HID, fewer pins (20 vs 54)

### Why "Arduino Leonardo" for Pro Micro?
The Pro Micro uses the same ATmega32u4 chip as the Leonardo. The Arduino IDE doesn't have a built-in "Pro Micro" board definition, but the Leonardo definition is 100% compatible. Using "Arduino Mega" causes upload failures because it tries to program via external programmer instead of USB bootloader.

### EEPROM Magic Numbers
- `0xAC01` = Dual-board configuration (Mega + Pro Micro)
- `0xAC02` = Solo configuration (Mega only with PC daemon)

Changing the magic number invalidates old configs, forcing reconfiguration. Use this when making breaking changes to the Config struct.

### GUI Auto-Update System
The updater checks GitHub releases API every time the GUI launches. It compares the local VERSION file against the latest GitHub release tag. If newer, it downloads the release ZIP, extracts to a temp directory, and copies files while preserving `.git/`, `.claude/`, and user data. This ensures users always have the latest features and bug fixes.

### Future Enhancement Ideas
1. **Web-based GUI:** Replace PyQt5 with web interface (Flask/React)
2. **Wireless configuration:** ESP32 WiFi for remote config
3. **Multi-profile support:** Save/load different configurations
4. **Analog value mapping:** Map potentiometer ranges to specific commands
5. **LED feedback:** Visual confirmation on Arduino during config
6. **Macro sequences:** Chain multiple keyboard commands
7. **Conditional inputs:** "Hold button A + press button B" combinations

---

## Questions for Future Claude Instances

If you're continuing work on this project, you likely need to know:

1. **"How do I add a new input type?"**
   - See: Workflow 1 in Key Development Workflows section

2. **"Why isn't the Pro Micro working?"**
   - Read: PRO_MICRO_DEBUG.md (comprehensive guide)
   - Key: Must select "Arduino Leonardo" board

3. **"Can this work without Pro Micro?"**
   - Yes, but NOT for Xbox compatibility
   - See: HARDWARE_ALTERNATIVES.md for CH9328 module option
   - arduino-config-mega-solo.ino + keyboard_daemon.py works on PC only

4. **"How do I create a new release?"**
   - See: Workflow 3 in Key Development Workflows section

5. **"What's the serial protocol format?"**
   - See: Serial Protocol section above
   - Format: "CTRL+KEY\n" strings at 115200 baud

6. **"Why does the GUI say 'Port not found'?"**
   - See: Common Issues and Solutions section
   - Usually: wrong cable, wrong port, or driver issue

7. **"Can I use this on Linux/Mac?"**
   - Yes: GUI is cross-platform (PyQt5)
   - No: Auto-installers are Windows-only (use pip manually)
   - Arduino firmware is platform-independent

---

## Contact and Repository

**GitHub:** https://github.com/bworthy89/seths-crap
**Issues:** Use GitHub Issues for bug reports
**Primary maintainer:** User @bworthy89

---

**Last updated:** 2025-01-XX
**Document version:** 1.0
**Firmware version:** 1.0.0
