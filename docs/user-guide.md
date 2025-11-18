# Arduino Input Configurator - User Guide

Complete guide for setting up and using the Arduino Input Configurator system.

## Table of Contents
1. [System Overview](#system-overview)
2. [Initial Setup](#initial-setup)
3. [Uploading Firmware](#uploading-firmware)
4. [Installing Python GUI](#installing-python-gui)
5. [Using the GUI](#using-the-gui)
6. [Configuration Examples](#configuration-examples)
7. [Troubleshooting](#troubleshooting)
8. [Advanced Features](#advanced-features)

---

## System Overview

### What This System Does
The Arduino Input Configurator allows you to:
- Connect up to 40 physical inputs (buttons, encoders, switches, potentiometers) to your computer
- Configure each input to send keyboard commands (Ctrl+ combinations)
- Name each input for easy identification
- Set rotary encoders to increment by 1x, 10x, 100x, or 1000x
- Persist configuration in Arduino EEPROM (survives power cycles)
- Use a graphical interface for easy configuration

### System Architecture
```
┌─────────────────────────────────────────────────────────┐
│  Physical Inputs (Buttons, Encoders, Switches, Pots)   │
└──────────────────────┬──────────────────────────────────┘
                       │
         ┌─────────────▼──────────────┐
         │   Arduino Mega 2560        │
         │   (Input Handler)          │
         │   - Reads inputs           │
         │   - Stores configuration   │
         │   - Sends commands         │
         └───┬──────────────┬─────────┘
             │              │
        UART Serial    USB Serial
       (Serial1)      (Configuration)
             │              │
   ┌─────────▼────────┐    │
   │ Arduino Pro Micro│    │
   │ (HID Keyboard)   │    │
   └─────────┬────────┘    │
             │              │
         USB HID       ┌────▼─────┐
        (Keyboard)     │   PC     │
             │         │  (GUI)   │
             └─────────►          │
                       └──────────┘
```

---

## Initial Setup

### Prerequisites
- **Operating System**: Windows, macOS, or Linux
- **Python**: Version 3.7 or higher
- **Arduino IDE**: Version 1.8.13 or higher (or Arduino IDE 2.x)
- **Hardware**: Arduino Mega 2560 + Arduino Pro Micro (see hardware-wiring.md)

### Install Arduino IDE
1. Download from https://www.arduino.cc/en/software
2. Install for your operating system
3. Launch Arduino IDE

### Install Required Arduino Libraries
1. Open Arduino IDE
2. Go to **Tools → Manage Libraries** (or Ctrl+Shift+I)
3. Install the following libraries:
   - **ArduinoJson** by Benoit Blanchon (version 6.x)
   - **Bounce2** by Thomas Fredericks
   - **Encoder** by Paul Stoffregen

Search for each library name and click "Install".

---

## Uploading Firmware

### Step 1: Upload to Arduino Pro Micro

1. **Open the sketch:**
   - File → Open
   - Navigate to `arduino-config-promicro/arduino-config-promicro.ino`

2. **Select board and port:**
   - Tools → Board → Arduino AVR Boards → **Arduino Leonardo**
     (Pro Micro uses the same chip and driver)
   - Tools → Port → Select the Pro Micro port
     - Windows: COMx (Arduino Leonardo)
     - macOS: /dev/cu.usbmodemXXXX
     - Linux: /dev/ttyACMx

3. **Upload:**
   - Click the Upload button (→) or Ctrl+U
   - Wait for "Done uploading"
   - Pro Micro LED should turn on solid

4. **Verify:**
   - Pro Micro is now ready to act as a keyboard
   - Keep it connected to the PC

### Step 2: Upload to Arduino Mega 2560

1. **Open the sketch:**
   - File → Open
   - Navigate to `arduino-config-mega/arduino-config-mega.ino`

2. **Select board and port:**
   - Tools → Board → Arduino AVR Boards → **Arduino Mega or Mega 2560**
   - Tools → Processor → **ATmega2560 (Mega 2560)**
   - Tools → Port → Select the Mega port
     - Windows: COMx (Arduino Mega 2560)
     - macOS: /dev/cu.usbserial-XXXX
     - Linux: /dev/ttyUSBx or /dev/ttyACMx

3. **Upload:**
   - Click the Upload button (→) or Ctrl+U
   - Wait for "Done uploading"

4. **Verify:**
   - Tools → Serial Monitor (Ctrl+Shift+M)
   - Set baud rate to **115200**
   - You should see: "Arduino Mega Input Handler Ready"

---

## Installing Python GUI

### Step 1: Install Python
If not already installed:
- **Windows**: Download from https://www.python.org/downloads/
  - Check "Add Python to PATH" during installation
- **macOS**: `brew install python3` or download from python.org
- **Linux**: `sudo apt-get install python3 python3-pip` (Debian/Ubuntu)

### Step 2: Install Dependencies

Open terminal/command prompt and navigate to the `gui` folder:

```bash
cd path/to/arduino/gui
pip install -r requirements.txt
```

This installs:
- PyQt5 (GUI framework)
- pyserial (serial communication)

**Note for Linux users**: You may need to install additional Qt dependencies:
```bash
sudo apt-get install python3-pyqt5
```

### Step 3: Grant Serial Port Permissions (Linux/macOS)

**Linux:**
```bash
sudo usermod -a -G dialout $USER
```
Then log out and log back in.

**macOS:**
Usually no special permissions needed.

**Windows:**
No special permissions needed.

---

## Using the GUI

### Launching the Application

**Option 1: Command Line**
```bash
cd path/to/arduino/gui
python main.py
```

**Option 2: Double-click (Windows)**
- Right-click `main.py` → Open with → Python

**Option 3: Create Desktop Shortcut**
- Create a shortcut to `main.py`
- Set target to: `python "C:\path\to\arduino\gui\main.py"`

### GUI Overview

```
┌───────────────────────────────────────────────────────────┐
│  Arduino Input Configurator - ELEGOO MEGA R3             │
├───────────────────────────────────────────────────────────┤
│  Arduino Connection                                       │
│  ┌─────────────────────────────────────────────────────┐ │
│  │ Serial Port: [COM3 - Arduino Mega] [Refresh Ports] │ │
│  │ [Connect] ● Disconnected                            │ │
│  └─────────────────────────────────────────────────────┘ │
├───────────────────────────────────────────────────────────┤
│  Input Configuration                                      │
│  ┌─────────────────────────────────────────────────────┐ │
│  │ [+ Add Input] [- Remove] [Clear All] [⬆ Upload]    │ │
│  ├─────┬────────┬──────┬──────┬─────────┬────────────┤ │
│  │Name │  Type  │ Pin1 │ Pin2 │ Encoder │  Keyboard  │ │
│  │     │        │      │      │  Mode   │  Command   │ │
│  ├─────┼────────┼──────┼──────┼─────────┼────────────┤ │
│  │Fire │ Button │  22  │ N/A  │         │  CTRL+F    │ │
│  │Vol  │Encoder │   2  │   3  │  1x     │  CTRL+V    │ │
│  └─────┴────────┴──────┴──────┴─────────┴────────────┘ │
├───────────────────────────────────────────────────────────┤
│  Console Output                                           │
│  ┌─────────────────────────────────────────────────────┐ │
│  │ Connected to Arduino on COM3                        │ │
│  │ Uploaded configuration: 2 inputs                    │ │
│  │ Arduino: Configuration updated: 2 inputs configured │ │
│  └─────────────────────────────────────────────────────┘ │
└───────────────────────────────────────────────────────────┘
```

---

## Step-by-Step Configuration

### Step 1: Connect to Arduino

1. **Connect hardware:**
   - Plug Mega 2560 into PC via USB
   - Plug Pro Micro into PC via USB
   - Ensure boards are wired together (see hardware-wiring.md)

2. **In GUI:**
   - Click **"Refresh Ports"** to scan for connected Arduinos
   - Select the **Mega 2560** port from dropdown
   - Click **"Connect"**
   - Status should change to **"● Connected"** (green)
   - Console shows: "Connected to Arduino on COMx"

### Step 2: Add Input Configurations

1. Click **"+ Add Input"** to add a new row to the table

2. **Configure each input:**

   **Name** (Column 1):
   - Enter a descriptive name (e.g., "Fire Button", "Volume Knob")
   - Max 19 characters

   **Type** (Column 2):
   - Select from dropdown:
     - **Button**: Momentary push button
     - **Rotary Encoder**: Encoder with A/B phases
     - **Switch**: Toggle switch
     - **Potentiometer**: Analog knob/slider

   **Pin 1** (Column 3):
   - Select the Arduino pin connected to this input
   - For encoders: This is the A (CLK) pin
   - For potentiometers: Select analog pin (A0-A15)
   - For buttons/switches: Any digital pin

   **Pin 2** (Column 4):
   - Only for **Rotary Encoders**
   - Select the B (DT) pin
   - For all other types, leave as "N/A"

   **Encoder Mode** (Column 5):
   - Only for **Rotary Encoders**
   - Select increment mode:
     - **1x**: Each click = 1 command
     - **10x**: Each click = 10 commands
     - **100x**: Each click = 100 commands
     - **1000x**: Each click = 1000 commands
   - For other types, this is disabled

   **Keyboard Command** (Column 6):
   - Enter the keyboard command to send
   - **Must start with "CTRL+"**
   - Examples:
     - `CTRL+F` - Ctrl+F
     - `CTRL+UPARROW` - Ctrl+Up Arrow
     - `CTRL+1` - Ctrl+1
     - `CTRL+SPACE` - Ctrl+Space
   - See [Keyboard Commands](#keyboard-commands) for full list

3. Repeat for all inputs (max 40)

### Step 3: Upload Configuration

1. Click **"⬆ Upload to Arduino"** button
2. Configuration is sent to Mega via USB serial
3. Mega stores configuration in EEPROM
4. Console shows: "Configuration updated: X inputs configured"
5. A popup confirms successful upload

**Note**: Configuration persists even after power cycle!

### Step 4: Test Inputs

**Option 1: Test Individual Key Command**
1. Select a row in the table
2. Click **"Test Selected Key"**
3. Keyboard command should be sent immediately
4. Open Notepad to see the result

**Option 2: Test Physical Input**
1. Ensure configuration is uploaded
2. Open Notepad or target application
3. Press button, turn encoder, flip switch, or move potentiometer
4. Corresponding keyboard command should execute
5. Console shows: "Sent: CTRL+X"

---

## Configuration Examples

### Example 1: Gaming Controller

**Flight Simulator Setup:**

| Name | Type | Pin 1 | Pin 2 | Mode | Command |
|------|------|-------|-------|------|---------|
| Fire Primary | Button | 22 | N/A | - | CTRL+F |
| Fire Secondary | Button | 23 | N/A | - | CTRL+G |
| Throttle Up | Button | 24 | N/A | - | CTRL+UPARROW |
| Throttle Down | Button | 25 | N/A | - | CTRL+DOWNARROW |
| Rudder Left | Button | 26 | N/A | - | CTRL+LEFTARROW |
| Rudder Right | Button | 27 | N/A | - | CTRL+RIGHTARROW |
| Landing Gear | Switch | 28 | N/A | - | CTRL+L |
| Flaps | Switch | 29 | N/A | - | CTRL+K |
| Zoom | Encoder | 2 | 3 | 1x | CTRL+Z |
| Trim | Encoder | 20 | 21 | 1x | CTRL+T |

### Example 2: Audio/Video Editing

**DAW Control Surface:**

| Name | Type | Pin 1 | Pin 2 | Mode | Command |
|------|------|-------|-------|------|---------|
| Play/Pause | Button | 30 | N/A | - | CTRL+SPACE |
| Stop | Button | 31 | N/A | - | CTRL+S |
| Record | Button | 32 | N/A | - | CTRL+R |
| Track Up | Button | 33 | N/A | - | CTRL+UPARROW |
| Track Down | Button | 34 | N/A | - | CTRL+DOWNARROW |
| Volume | Encoder | 2 | 3 | 1x | CTRL+V |
| Pan | Encoder | 20 | 21 | 1x | CTRL+P |
| Zoom In | Button | 35 | N/A | - | CTRL+PLUS |
| Zoom Out | Button | 36 | N/A | - | CTRL+MINUS |

### Example 3: Industrial/Lab Equipment

**Precision Control:**

| Name | Type | Pin 1 | Pin 2 | Mode | Command |
|------|------|-------|-------|------|---------|
| Coarse Adjust | Encoder | 2 | 3 | 1000x | CTRL+C |
| Medium Adjust | Encoder | 20 | 21 | 100x | CTRL+M |
| Fine Adjust | Encoder | 5 | 6 | 10x | CTRL+F |
| Ultra Fine | Encoder | 8 | 9 | 1x | CTRL+U |
| Start Process | Button | 40 | N/A | - | CTRL+G |
| Emergency Stop | Button | 41 | N/A | - | CTRL+E |
| Mode A | Switch | 42 | N/A | - | CTRL+A |
| Mode B | Switch | 43 | N/A | - | CTRL+B |

---

## Keyboard Commands

### Supported Keys

**Single Character Keys:**
- `CTRL+A` through `CTRL+Z`
- `CTRL+0` through `CTRL+9`
- `CTRL+SPACE`

**Arrow Keys:**
- `CTRL+UPARROW` or `CTRL+UP`
- `CTRL+DOWNARROW` or `CTRL+DOWN`
- `CTRL+LEFTARROW` or `CTRL+LEFT`
- `CTRL+RIGHTARROW` or `CTRL+RIGHT`

**Function Keys:**
- `CTRL+F1` through `CTRL+F12`

**Special Keys:**
- `CTRL+ENTER` or `CTRL+RETURN`
- `CTRL+ESC` or `CTRL+ESCAPE`
- `CTRL+TAB`
- `CTRL+BACKSPACE`
- `CTRL+DELETE` or `CTRL+DEL`
- `CTRL+HOME`
- `CTRL+END`
- `CTRL+PAGEUP` or `CTRL+PGUP`
- `CTRL+PAGEDOWN` or `CTRL+PGDN`

**Symbols:**
- `CTRL+-` (minus)
- `CTRL+=` (plus/equals)
- `CTRL+[` (left bracket)
- `CTRL+]` (right bracket)

**Note**: Commands are case-insensitive. `CTRL+F` and `ctrl+f` are equivalent.

---

## Advanced Features

### Encoder Increment Modes Explained

Rotary encoders can send multiple keyboard commands per click:

**1x Mode (default):**
- 1 click clockwise = 1 `CTRL+X` command
- Best for: Menu navigation, volume control, zoom

**10x Mode:**
- 1 click clockwise = 10 `CTRL+X` commands
- Best for: Scrolling through long lists, timeline scrubbing

**100x Mode:**
- 1 click clockwise = 100 `CTRL+X` commands
- Best for: Coarse adjustments, fast seeking

**1000x Mode:**
- 1 click clockwise = 1000 `CTRL+X` commands
- Best for: Very large value changes, rapid traversal

**Use Case Example:**
- Frequency tuner:
  - **1x mode**: Adjust by 1 Hz
  - **10x mode**: Adjust by 10 Hz (press encoder button to switch)
  - **100x mode**: Adjust by 100 Hz
  - **1000x mode**: Adjust by 1 kHz

### EEPROM Persistence

Configuration is stored in Arduino Mega's EEPROM:
- **Capacity**: 4KB (enough for 40 inputs)
- **Retention**: Survives power cycles and sketch uploads
- **Validation**: Checksum ensures data integrity
- **Updates**: Only when you click "Upload to Arduino"

**To clear configuration:**
1. Click "Clear All" in GUI
2. Click "Upload to Arduino"

### Configuration File Export/Import (Future Feature)

*Currently not implemented, but you can manually save configurations by documenting your setup in a text file.*

### Multi-Profile Support (Future Feature)

*Currently supports one configuration at a time. Switch profiles by uploading different configurations.*

---

## Troubleshooting

### GUI Issues

**Problem**: "No serial ports detected"
- **Solution**:
  - Check USB cables are connected
  - Click "Refresh Ports"
  - Try different USB ports
  - Check Device Manager (Windows) for COM ports
  - Install CH340/CP2102 drivers if using clone boards

**Problem**: Can't connect to Arduino
- **Solution**:
  - Ensure correct port selected (Mega, not Pro Micro)
  - Close Arduino IDE Serial Monitor (conflicts with GUI)
  - Restart Arduino (press reset button)
  - Try different USB cable

**Problem**: Upload fails or times out
- **Solution**:
  - Check connection status (should be green)
  - Verify configuration has valid pins and commands
  - Check console for error messages
  - Restart Arduino and reconnect

### Arduino Issues

**Problem**: No response from Arduino
- **Solution**:
  - Re-upload firmware to Mega
  - Check Serial Monitor (115200 baud) for messages
  - Verify USB connection
  - Press reset button on Mega

**Problem**: Encoders not working
- **Solution**:
  - Ensure using interrupt pins (2, 3, 20, 21)
  - Check encoder wiring (CLK to Pin1, DT to Pin2)
  - Add 0.1µF capacitors for debouncing
  - Verify encoder library installed

**Problem**: Buttons triggering multiple times
- **Solution**:
  - Increase `DEBOUNCE_MS` in firmware (default 5ms)
  - Add 0.1µF capacitor across button
  - Check for loose connections
  - Verify Bounce2 library installed

**Problem**: Configuration not persisting
- **Solution**:
  - Wait for "Configuration saved to EEPROM" message
  - Verify checksum calculation (check Serial Monitor)
  - EEPROM may be worn (unlikely, 100k write cycles)

### Keyboard Output Issues

**Problem**: No keyboard commands sent
- **Solution**:
  - Verify Pro Micro is connected to PC
  - Re-upload Pro Micro firmware
  - Check Serial1 connection between boards (TX→RX, RX→TX)
  - Verify GND connection between boards
  - Test with Serial Monitor: should see "Sent: CTRL+X"

**Problem**: Wrong keys being sent
- **Solution**:
  - Double-check keyboard command in GUI
  - Ensure command starts with "CTRL+"
  - Verify special key names (e.g., UPARROW, not UP_ARROW)
  - Test with "Test Selected Key" button

**Problem**: Keyboard lag or delay
- **Solution**:
  - Reduce encoder increment mode (1000x → 100x)
  - Check USB cable quality
  - Reduce number of simultaneous commands
  - Add delay between commands in firmware

---

## Tips & Best Practices

1. **Start Small**: Configure 2-3 inputs first, test, then add more
2. **Label Hardware**: Use physical labels on buttons/encoders
3. **Document Setup**: Keep notes on pin assignments
4. **Use Interrupt Pins**: For encoders, always use pins 2, 3, 20, 21
5. **Test Incrementally**: Test each input as you add it
6. **Backup Configuration**: Take screenshot or write down settings
7. **Application Compatibility**: Verify target app supports Ctrl+ shortcuts
8. **Power Considerations**: Use external power for 30+ inputs

---

## Application Integration

### Configuring Your Application

Most applications allow customizing keyboard shortcuts:

**Example: OBS Studio (Streaming)**
1. File → Settings → Hotkeys
2. Assign Ctrl+F to "Start Recording"
3. Assign Ctrl+G to "Stop Recording"
4. Configure Arduino button to send `CTRL+F`

**Example: Premiere Pro (Video Editing)**
1. Edit → Keyboard Shortcuts
2. Assign Ctrl+Space to "Play/Pause"
3. Assign Ctrl+V to "Increase Volume"
4. Configure Arduino inputs accordingly

**Example: AutoCAD**
1. Manage → Customization → Keyboard
2. Assign Ctrl+ commands to tools
3. Map Arduino encoders to zoom, pan, rotate

---

## Frequently Asked Questions

**Q: Can I use Shift+ or Alt+ instead of Ctrl+?**
A: Currently only Ctrl+ is supported. Modifying the Pro Micro firmware can add Shift/Alt support.

**Q: How many inputs can I really use?**
A: Theoretically 40, but practically limited by:
- Number of interrupt pins (6) for encoders
- Number of analog pins (16) for potentiometers
- Firmware array sizes
- Power consumption

**Q: Does this work on Mac/Linux?**
A: Yes! The system is cross-platform. Keyboard commands work on all OS.

**Q: Can I use this with game consoles?**
A: No, it only works with PCs that accept USB keyboard input.

**Q: Will this work with any Arduino Mega?**
A: Yes, any ATmega2560-based board (Mega 2560, ELEGOO MEGA R3, etc.)

**Q: Can I use Arduino Leonardo instead of Pro Micro?**
A: Yes! Leonardo has the same 32u4 chip and native USB HID support.

**Q: Does the GUI need to stay open?**
A: No! Once configured, the Arduino works independently. Only open GUI to change configuration.

**Q: Can I power both boards from one USB cable?**
A: No, both need separate USB connections (Mega for config, Pro Micro for keyboard output).

---

## Getting Help

1. **Check Console Output**: Often shows error messages
2. **Serial Monitor**: Open at 115200 baud to see Arduino messages
3. **Review Documentation**: hardware-wiring.md, this guide
4. **Test Systematically**: Isolate the problem (hardware vs software)
5. **Check Connections**: 90% of problems are loose wires

---

## Next Steps

- Experiment with different input configurations
- Create custom control panels for specific applications
- Share your setup with the community
- Consider designing a custom PCB for permanent installation

Happy configuring! 🎮🎛️
