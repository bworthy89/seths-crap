# KeyFlight - MobiFlight-Inspired Keyboard Command System

## Project Overview

**KeyFlight** is a flexible, modular system for sending keyboard commands (A+B+C, A, A+B, etc.) using Arduino Mega 2560 and Pro Micro hardware. The architecture is inspired by MobiFlight's proven design patterns.

## Architecture Comparison: MobiFlight vs KeyFlight

| Component | MobiFlight | KeyFlight |
|-----------|------------|-----------|
| **Purpose** | Flight simulator cockpit control | Keyboard command automation |
| **PC Application** | C# Windows Forms (.NET) | C# Windows Forms (.NET) |
| **Communication** | CommandMessenger protocol | CommandMessenger protocol |
| **Hardware** | Single Arduino (Mega/Nano/etc.) | Dual-board (Mega + Pro Micro) |
| **Inputs** | Buttons, encoders, switches, pots | Buttons, encoders, switches, pots |
| **Outputs** | LEDs, servos, steppers, displays | USB HID keyboard commands |
| **Config Storage** | PC-side config files | PC-side config + EEPROM backup |

## System Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                    KeyFlight Connector (PC)                  │
│                     C# Windows Forms App                     │
├─────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐         │
│  │   Config    │  │  Serial     │  │   Device    │         │
│  │  Manager    │  │ Comm (CM)   │  │  Manager    │         │
│  └─────────────┘  └─────────────┘  └─────────────┘         │
│         ↓                ↑                  ↓                │
│  ┌──────────────────────┴──────────────────┘                │
│  │         CommandMessenger Protocol                        │
│  └──────────────────────┬──────────────────┐                │
└────────────────────────|──────────────────|─────────────────┘
                         │ USB Serial       │
                         ↓                  │
┌─────────────────────────────────────────────────────────────┐
│              Arduino Mega 2560 (Input Handler)               │
├─────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐         │
│  │ MF_Button   │  │ MF_Encoder  │  │  MF_Analog  │         │
│  │   Module    │  │   Module    │  │   Module    │         │
│  └─────────────┘  └─────────────┘  └─────────────┘         │
│         ↓                ↓                  ↓                │
│  ┌──────────────────────┴──────────────────┘                │
│  │          CommandMessenger Handler                        │
│  └──────────────────────┬──────────────────┐                │
│                         │ Serial1 (UART)   │                │
│                         ↓                  │                │
└─────────────────────────────────────────────────────────────┘
                         │
                         ↓
┌─────────────────────────────────────────────────────────────┐
│          Arduino Pro Micro (HID Keyboard Output)             │
├─────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐         │
│  │   Serial    │  │  Keyboard   │  │   Command   │         │
│  │  Receiver   │  │   Parser    │  │  Executor   │         │
│  └─────────────┘  └─────────────┘  └─────────────┘         │
│         ↓                ↓                  ↓                │
│  ┌──────────────────────┴──────────────────┘                │
│  │            USB HID Keyboard.h                            │
│  └──────────────────────┬──────────────────┐                │
└─────────────────────────|──────────────────|─────────────────┘
                          │ USB HID          │
                          ↓                  │
                    ┌──────────┐             │
                    │   PC /   │             │
                    │   Xbox   │◄────────────┘
                    └──────────┘
```

## Core Components

### 1. KeyFlight Connector (C# Application)

**File Structure:**
```
KeyFlightConnector/
├── KeyFlightConnector.sln
├── KeyFlightConnector/
│   ├── Forms/
│   │   ├── MainForm.cs              # Main GUI
│   │   ├── ConfigurationForm.cs     # Device configuration
│   │   └── InputMappingForm.cs      # Keyboard mapping UI
│   ├── Communication/
│   │   ├── CmdMessenger.cs          # CommandMessenger protocol
│   │   ├── SerialConnection.cs      # Serial port management
│   │   └── DeviceManager.cs         # Device detection/management
│   ├── Models/
│   │   ├── InputDevice.cs           # Button/Encoder/Switch models
│   │   ├── KeyCommand.cs            # Keyboard command definition
│   │   └── Configuration.cs         # Config serialization
│   └── Services/
│       ├── ConfigService.cs         # Save/load configurations
│       └── FirmwareUploader.cs      # Arduino firmware upload
```

**Key Features:**
- Visual device configuration (drag-and-drop inputs)
- Keyboard command builder (A, A+B, A+B+C, etc.)
- Live input monitoring
- Configuration save/load (JSON format)
- Auto-detect Arduino devices
- Firmware upload wizard

### 2. Mega Firmware (Input Handler)

**File Structure:**
```
KeyFlight-Mega/
├── src/
│   ├── main.cpp                     # Main entry point
│   ├── CommandMessenger.cpp/.h      # CM protocol handler
│   ├── Config.cpp/.h                # Configuration management
│   ├── MF_Modules/
│   │   ├── MF_Button/
│   │   │   ├── MF_Button.cpp/.h     # Button module
│   │   │   └── MF_Button.md         # Documentation
│   │   ├── MF_Encoder/
│   │   │   ├── MF_Encoder.cpp/.h    # Rotary encoder module
│   │   │   └── MF_Encoder.md
│   │   ├── MF_Analog/
│   │   │   ├── MF_Analog.cpp/.h     # Analog input (potentiometers)
│   │   │   └── MF_Analog.md
│   │   └── MF_Switch/
│   │       ├── MF_Switch.cpp/.h     # Toggle switch module
│   │       └── MF_Switch.md
│   └── HIDMessenger.cpp/.h          # Serial1 communication to Pro Micro
├── platformio.ini                   # PlatformIO config
└── README.md
```

**Responsibilities:**
- Read hardware inputs (buttons, encoders, switches, pots)
- Manage configuration via CommandMessenger
- Send keyboard commands to Pro Micro via Serial1
- Store configuration backup in EEPROM
- Report input events to PC (for monitoring)

### 3. Pro Micro Firmware (HID Output)

**File Structure:**
```
KeyFlight-ProMicro/
├── src/
│   ├── main.cpp                     # Main entry point
│   ├── KeyboardParser.cpp/.h        # Parse "A+B+C" format
│   ├── KeyboardExecutor.cpp/.h      # Execute HID commands
│   └── SerialReceiver.cpp/.h        # Receive from Mega
├── platformio.ini
└── README.md
```

**Responsibilities:**
- Receive keyboard commands from Mega via Serial1
- Parse command strings (e.g., "A+B+C", "CTRL+ALT+DEL")
- Execute USB HID keyboard output
- Handle modifier keys (CTRL, ALT, SHIFT, WIN)
- Support special keys (F1-F12, arrows, etc.)

## CommandMessenger Protocol

### Command Structure

Commands follow the MobiFlight pattern: `<CommandID>,<Param1>,<Param2>,...;`

**Example Commands:**

| Command ID | Name | Direction | Parameters | Example |
|------------|------|-----------|------------|---------|
| 0 | kStatus | PC→Mega | None | `0;` |
| 1 | kInfo | Mega→PC | board, version | `1,MEGA,1.0.0;` |
| 2 | kSetConfig | PC→Mega | JSON config | `2,{...};` |
| 3 | kGetConfig | PC→Mega | None | `3;` |
| 4 | kConfigInfo | Mega→PC | JSON config | `4,{...};` |
| 5 | kSetPin | PC→Mega | moduleID, type, pin, pin2 | `5,0,BUTTON,2,0;` |
| 6 | kSetKey | PC→Mega | moduleID, keyCommand | `6,0,A+B+C;` |
| 7 | kTrigger | Mega→PC | moduleID, state | `7,0,1;` |
| 8 | kResetConfig | PC→Mega | None | `8;` |
| 9 | kSaveConfig | PC→Mega | None | `9;` |

### Communication Flow

**1. Device Detection:**
```
PC:   0;                    # Request status
Mega: 1,MEGA,1.0.0,10;      # Board, version, max modules
```

**2. Configure Button:**
```
PC:   5,0,BUTTON,2,0;       # Module 0, Button on pin 2
PC:   6,0,A+B+C;            # Module 0, press A+B+C
Mega: 1;                    # ACK
```

**3. Button Press Event:**
```
Mega: 7,0,1;                # Module 0 pressed (state=1)
Mega: [Send "A+B+C" to Pro Micro via Serial1]
PC:   (Updates UI - button 0 highlighted)
```

**4. Save Configuration:**
```
PC:   9;                    # Save to EEPROM
Mega: 1;                    # Config saved successfully
```

## Keyboard Command Format

### Command Syntax

KeyFlight supports flexible keyboard command syntax:

**Single Key:**
- `A` - Press 'A'
- `F1` - Press F1
- `SPACE` - Press spacebar

**Modifier + Key:**
- `CTRL+C` - Ctrl+C
- `ALT+F4` - Alt+F4
- `SHIFT+A` - Shift+A

**Multiple Modifiers:**
- `CTRL+ALT+DEL` - Ctrl+Alt+Delete
- `CTRL+SHIFT+ESC` - Ctrl+Shift+Escape

**Multiple Keys (Chord):**
- `A+B` - Press A and B simultaneously
- `A+B+C` - Press A, B, and C simultaneously
- `1+2+3+4` - Press 1, 2, 3, 4 simultaneously

**Special Keys:**
- `UPARROW`, `DOWNARROW`, `LEFTARROW`, `RIGHTARROW`
- `ESC`, `TAB`, `ENTER`, `BACKSPACE`, `DELETE`
- `HOME`, `END`, `PAGEUP`, `PAGEDOWN`
- `F1` through `F12`

### Implementation on Pro Micro

**Parsing:**
```cpp
// Input: "CTRL+ALT+A+B"
// 1. Split by '+'
// 2. Identify modifiers (CTRL, ALT, SHIFT, WIN)
// 3. Identify regular keys (A, B)
// 4. Press modifiers first
// 5. Press regular keys
// 6. Release all keys after 50ms
```

**Example Code:**
```cpp
void executeCommand(String cmd) {
    // Parse command string
    String keys[10];
    int keyCount = splitString(cmd, '+', keys);

    // Press modifiers
    for (int i = 0; i < keyCount; i++) {
        if (keys[i] == "CTRL") Keyboard.press(KEY_LEFT_CTRL);
        else if (keys[i] == "ALT") Keyboard.press(KEY_LEFT_ALT);
        else if (keys[i] == "SHIFT") Keyboard.press(KEY_LEFT_SHIFT);
    }

    // Press regular keys
    for (int i = 0; i < keyCount; i++) {
        if (isRegularKey(keys[i])) {
            Keyboard.press(keys[i].charAt(0));
        }
    }

    // Release all
    delay(50);
    Keyboard.releaseAll();
}
```

## Configuration Storage

### JSON Configuration Format

**PC-Side (KeyFlightConnector):**
```json
{
  "version": "1.0",
  "deviceName": "My Keyboard Controller",
  "modules": [
    {
      "id": 0,
      "type": "Button",
      "name": "Fire Button",
      "pin": 2,
      "pin2": 0,
      "keyCommand": "A+B+C",
      "enabled": true
    },
    {
      "id": 1,
      "type": "Encoder",
      "name": "Throttle",
      "pin": 3,
      "pin2": 4,
      "keyCommandCW": "UPARROW",
      "keyCommandCCW": "DOWNARROW",
      "sensitivity": 1,
      "enabled": true
    },
    {
      "id": 2,
      "type": "Switch",
      "name": "Landing Gear",
      "pin": 5,
      "pin2": 0,
      "keyCommandOn": "G",
      "keyCommandOff": "G",
      "enabled": true
    }
  ]
}
```

### EEPROM Storage (Mega Backup)

**Structure:**
```cpp
struct ModuleConfig {
    uint8_t id;              // Module ID (0-39)
    uint8_t type;            // BUTTON=1, ENCODER=2, SWITCH=3, ANALOG=4
    uint8_t pin;             // Primary pin
    uint8_t pin2;            // Secondary pin (encoders)
    char keyCommand[32];     // "A+B+C" format
    char keyCommand2[32];    // For encoders (CCW), switches (OFF)
    uint8_t sensitivity;     // Encoder sensitivity
    bool enabled;            // Is module active
};

struct Config {
    uint16_t magic;          // 0xKF01 (KeyFlight v1)
    uint8_t version;         // Config version
    uint8_t moduleCount;     // Number of configured modules
    ModuleConfig modules[40]; // Up to 40 modules
    uint16_t checksum;       // CRC16 checksum
};
```

## Development Roadmap

### Phase 1: Core Infrastructure ✅ (You are here)
- [x] Architecture design
- [ ] CommandMessenger protocol implementation
- [ ] Basic serial communication
- [ ] Device detection

### Phase 2: Mega Firmware
- [ ] MF_Button module
- [ ] MF_Encoder module
- [ ] MF_Switch module
- [ ] MF_Analog module
- [ ] Configuration management
- [ ] EEPROM storage

### Phase 3: Pro Micro Firmware
- [ ] Serial receiver
- [ ] Keyboard command parser
- [ ] HID keyboard executor
- [ ] Special key mapping

### Phase 4: PC Connector
- [ ] Main GUI framework
- [ ] Device manager
- [ ] Configuration editor
- [ ] Input mapping UI
- [ ] Save/load configurations

### Phase 5: Advanced Features
- [ ] Live input monitoring
- [ ] Firmware updater
- [ ] Multi-profile support
- [ ] Macro sequences
- [ ] Conditional triggers

## MobiFlight Compatibility

While KeyFlight is inspired by MobiFlight, it is **NOT** compatible with MobiFlight Connector. Key differences:

| Feature | MobiFlight | KeyFlight |
|---------|------------|-----------|
| Output | Flight sim variables | USB HID keyboard |
| Hardware | Single Arduino | Dual Arduino (Mega+ProMicro) |
| Config Format | MobiFlight XML | KeyFlight JSON |
| Protocol | MobiFlight CM | KeyFlight CM (custom) |

However, KeyFlight **reuses** MobiFlight's proven patterns:
- ✅ Modular input architecture (MF_Button, MF_Encoder, etc.)
- ✅ CommandMessenger protocol structure
- ✅ Configuration management approach
- ✅ Visual PC configuration tool

## Getting Started

### Hardware Requirements
- Arduino Mega 2560 (or ELEGOO MEGA R3)
- Arduino Pro Micro (ATmega32u4)
- Buttons, encoders, switches, potentiometers
- USB cables (2x - one for each Arduino)
- Jumper wires for Serial1 connection (Mega TX1→ProMicro RX, Mega RX1→ProMicro TX, GND→GND)

### Software Requirements
- Windows 10/11 (for KeyFlight Connector)
- Visual Studio 2022 (Community Edition)
- PlatformIO (for Arduino firmware)
- VS Code (recommended)

### Quick Start Guide
1. Upload Pro Micro firmware (Board: Arduino Leonardo)
2. Upload Mega firmware (Board: Arduino Mega 2560)
3. Connect Serial1 wires (TX1→RX, RX1→TX, GND→GND)
4. Run KeyFlight Connector
5. Click "Detect Devices"
6. Configure inputs and keyboard commands
7. Click "Upload Configuration"
8. Test your inputs!

## License

This project is open source. MobiFlight is Copyright MobiFlight Team, used as architectural inspiration with gratitude.

---

**Created:** 2025-11-19
**Version:** 1.0
**Status:** Architecture Design Phase
