# KeyFlight Pro Micro Firmware

Arduino Pro Micro firmware for USB HID keyboard command execution.

## Overview

This firmware runs on the Arduino Pro Micro and handles:
- USB HID keyboard emulation
- Parsing keyboard command strings (A+B+C, CTRL+C, etc.)
- Executing keyboard commands via USB
- Receiving trigger messages from Mega via Serial
- EEPROM configuration storage for key mappings

## Architecture

```
PC <--USB HID--> Pro Micro <--Serial1--> Mega 2560 <--USB Serial--> PC (C# App)
```

### Components

**KeyParser**
- Parses keyboard command strings
- Supports modifiers: CTRL, ALT, SHIFT, WIN
- Supports special keys: F1-F12, arrows, navigation keys
- Supports multiple simultaneous keys (USB HID limit: 6)
- Supports punctuation and symbols

**KeyExecutor**
- Executes parsed keyboard commands via USB HID
- Uses Arduino Keyboard library
- Automatic key press/release
- Modifier key support

**KeyConfig**
- EEPROM storage with "KEYFPM" signature
- Stores module ID → key command mappings
- Dual key command support for encoders/switches
- Max 40 modules

**Main Loop**
- Receives trigger/config messages from Mega via Serial1
- Executes keyboard commands
- LED status indication

## Project Structure

```
KeyFlightProMicro/
├── platformio.ini          # PlatformIO configuration
├── src/
│   └── main.cpp            # Main firmware sketch
└── lib/
    ├── KeyParser/
    │   ├── KeyParser.h
    │   └── KeyParser.cpp
    ├── KeyExecutor/
    │   ├── KeyExecutor.h
    │   └── KeyExecutor.cpp
    └── KeyConfig/
        ├── KeyConfig.h
        └── KeyConfig.cpp
```

## Building

### Using PlatformIO

```bash
# Install PlatformIO
pip install platformio

# Build firmware
cd KeyFlightProMicro
pio run

# Upload to Pro Micro
pio run --target upload

# Monitor serial output
pio device monitor
```

### Using Arduino IDE

1. Open `src/main.cpp` in Arduino IDE
2. Copy library folders to Arduino libraries folder
3. Select **Board**: SparkFun Pro Micro
4. Select **Processor**: ATmega32U4 (5V, 16 MHz)
5. Select **Port**: Your Pro Micro's COM port
6. Click **Upload**

**Important**: Pro Micro uses different bootloader than Arduino Leonardo. If upload fails:
1. Press reset button twice quickly to enter bootloader mode
2. COM port will change briefly - upload within 8 seconds

## Configuration

### Serial Communication

- **Mega Serial**: `Serial1` @ 115200 baud (RX/TX pins)
- **USB Serial**: Not used (USB used for HID only)

### Pin Assignments

- **RX (pin 0)**: Receives data from Mega TX1 (pin 18)
- **TX (pin 1)**: Sends debug to Mega RX1 (pin 19)
- **LED_BUILTIN**: Status indicator (blinks on key press)

### Wiring

```
Pro Micro          Mega 2560
---------          ----------
RX (0)     <-----  TX1 (18)
TX (1)     ----->  RX1 (19)
GND        ------  GND
```

## Communication Protocol

### Messages Received from Mega

| Command | Format | Description | Example |
|---------|--------|-------------|---------|
| Trigger | `<moduleId>,<value>;` | Execute key command | `0,0;` |
| Config | `CFG,<moduleId>,<key1>,<key2>;` | Set module key mapping | `CFG,0,SPACE,;` |
| Save | `SAVE;` | Save config to EEPROM | `SAVE;` |
| Reset | `RESET;` | Reset configuration | `RESET;` |

### Messages Sent to Mega

| Format | Description | Example |
|--------|-------------|---------|
| `[PM] <message>` | Debug message | `[PM] KeyFlight ProMicro v1.0.0 ready` |

## Keyboard Command Format

### Supported Keys

**Letters**: A-Z (case-insensitive)
```
A, B, C, ..., Z
```

**Numbers**: 0-9
```
0, 1, 2, ..., 9
```

**Modifiers**: CTRL, ALT, SHIFT, WIN
```
CTRL, ALT, SHIFT, WIN
CONTROL, WINDOWS, GUI
```

**Function Keys**: F1-F12
```
F1, F2, F3, ..., F12
```

**Navigation Keys**:
```
UPARROW, DOWNARROW, LEFTARROW, RIGHTARROW
UP, DOWN, LEFT, RIGHT
HOME, END, PAGEUP, PAGEDOWN
INSERT, DELETE
```

**Special Keys**:
```
ENTER, RETURN, ESC, ESCAPE
TAB, SPACE, BACKSPACE
CAPSLOCK, PRINTSCREEN, SCROLLLOCK, PAUSE
```

**Punctuation**:
```
MINUS (-), EQUAL (=)
LEFTBRACE ([), RIGHTBRACE (])
BACKSLASH (\), SEMICOLON (;)
APOSTROPHE ('), GRAVE (`)
COMMA (,), DOT (.), SLASH (/)
```

### Command Examples

**Single Key**:
```
A              → Press 'A'
SPACE          → Press Space
F1             → Press F1
```

**Modifier + Key**:
```
CTRL+C         → Ctrl+C (copy)
ALT+F4         → Alt+F4 (close window)
SHIFT+F1       → Shift+F1
WIN+D          → Windows+D (show desktop)
```

**Multiple Modifiers**:
```
CTRL+ALT+DEL   → Ctrl+Alt+Delete
CTRL+SHIFT+ESC → Ctrl+Shift+Esc (task manager)
```

**Multiple Keys** (up to 6 simultaneous):
```
A+B+C          → Press A, B, and C together
1+2+3          → Press 1, 2, and 3 together
```

**Complex Combinations**:
```
CTRL+SHIFT+A   → Ctrl+Shift+A
ALT+LEFT       → Alt+Left Arrow
WIN+UPARROW    → Windows+Up Arrow
```

## Key Parsing Logic

The KeyParser processes command strings as follows:

1. **Tokenize** by '+' delimiter
2. **Identify modifiers** (CTRL, ALT, SHIFT, WIN)
3. **Identify keys** (letters, numbers, special keys)
4. **Build KeyCommand structure**:
   - `modifiers`: Bitmask of active modifiers
   - `keys[]`: Array of key codes (max 6)
   - `keyCount`: Number of keys

5. **Execute** via KeyExecutor:
   - Press all modifiers
   - Press all keys
   - Wait 10ms
   - Release all keys

## EEPROM Configuration

### Structure

```cpp
struct KeyConfiguration {
    char signature[8];        // "KEYFPM"
    uint8_t version;          // 1
    uint8_t moduleCount;      // 0-40
    ModuleKeyMap modules[40];
};

struct ModuleKeyMap {
    uint8_t moduleId;
    char keyCommand[32];
    char keyCommand2[32];
};
```

### Storage
- **Address**: 0x0000
- **Size**: ~2.6 KB (for 40 modules)
- **Signature**: "KEYFPM" for validation

## Trigger Value Interpretation

When a trigger message is received, the `value` parameter determines which key command to execute:

| Input Type | Value | Key Command Used |
|------------|-------|------------------|
| Button | 0 | keyCommand |
| Encoder | 1 (CW) | keyCommand |
| Encoder | -1 (CCW) | keyCommand2 |
| Switch | 1 (ON) | keyCommand |
| Switch | 0 (OFF) | keyCommand2 |
| Analog | 0-1023 | keyCommand |

## Status Indication

The built-in LED provides visual feedback:
- **Solid ON**: Pro Micro is ready
- **50ms Blink**: Keyboard command executed successfully

## Testing

### Serial Monitor Test

Connect to Mega's serial monitor and send commands to Pro Micro:

```
# Configure module 0 with SPACE key
CFG,0,SPACE;

# Configure module 1 with CTRL+C
CFG,1,CTRL+C;

# Save configuration
SAVE;

# Trigger module 0 (will press SPACE)
0,0;

# Trigger module 1 (will press CTRL+C)
1,0;
```

### Keyboard Test

1. Open a text editor
2. Send trigger: `0,0;` (assuming module 0 = "A")
3. Letter 'a' should appear in editor
4. Send trigger with CTRL+C configured
5. Copy operation should execute

## Troubleshooting

### No Keyboard Response
- Verify USB cable is connected
- Check Device Manager for "Arduino Leonardo" (Pro Micro emulates Leonardo)
- Ensure Keyboard.begin() was called in setup()
- Try different USB port

### Module Not Executing
- Check module is configured: Send `CFG,<id>,<key>;`
- Verify key command syntax is correct
- Check EEPROM: Send `SAVE;` and `RESET;`
- Monitor LED: Should blink on trigger

### Upload Fails
- Press reset button twice quickly
- New COM port appears for ~8 seconds
- Upload immediately after reset
- If still fails, try different USB cable

### Serial Communication Issues
- Verify baud rate is 115200
- Check TX/RX wiring to Mega
- Ensure GND is connected
- Test with Serial1.println() debug messages

## Integration with Mega

The Pro Micro expects the Mega to:

1. **Send configuration** during initialization:
   ```
   CFG,0,SPACE;
   CFG,1,W;
   CFG,2,S;
   SAVE;
   ```

2. **Send triggers** when inputs change:
   ```
   0,0;      // Button 0 pressed
   1,1;      // Encoder 1 turned CW
   1,-1;     // Encoder 1 turned CCW
   2,1;      // Switch 2 turned ON
   ```

The Mega firmware currently sends triggers in format:
```
<moduleId>,<value>;
```

This matches the Pro Micro's expected format.

## Next Steps

1. **Build and upload** firmware to Pro Micro
2. **Connect** to Mega via Serial1
3. **Test** with simple commands (SPACE, A, B, C)
4. **Test** with modifiers (CTRL+C, ALT+TAB)
5. **Test** with multiple keys (A+B+C)
6. **Integrate** with C# Connector and Mega

## Version

- **Firmware**: 1.0.0
- **Board**: SparkFun Pro Micro (ATmega32U4)
- **Platform**: PlatformIO / Arduino IDE

---

**KeyFlight Project** - MobiFlight-inspired keyboard command controller
