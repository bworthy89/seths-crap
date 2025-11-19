# KeyFlight Mega Firmware

Arduino Mega 2560 firmware for KeyFlight input handling system.

## Overview

This firmware runs on the Arduino Mega 2560 and handles:
- Reading input modules (buttons, encoders, switches, analog inputs)
- CommandMessenger protocol communication with PC
- EEPROM configuration storage
- Serial communication with Pro Micro for keyboard command execution

## Architecture

```
PC (C# App) <--USB Serial--> Mega 2560 <--Serial1--> Pro Micro <--USB HID--> PC
```

### Components

**CommandMessenger Protocol**
- Text-based serial communication
- Format: `<CommandID>,<Param1>,<Param2>,...;`
- 13 commands matching C# implementation

**Input Modules**
- `MF_Button`: Digital button with debouncing
- `MF_Encoder`: Rotary encoder with quadrature decoding
- `MF_Switch`: Toggle switch with debouncing
- `MF_Analog`: Analog input with threshold detection

**Configuration Manager**
- EEPROM storage with "KEYFLT" signature
- Stores up to 40 module configurations
- Pin assignments, key commands, sensitivity settings

**Main Loop**
- Process CommandMessenger commands from PC
- Update all input modules
- Check for triggered inputs
- Forward triggers to Pro Micro

## Project Structure

```
KeyFlightMega/
├── platformio.ini              # PlatformIO configuration
├── src/
│   └── main.cpp                # Main firmware sketch
└── lib/
    ├── CommandMessenger/
    │   ├── CommandMessenger.h
    │   └── CommandMessenger.cpp
    ├── InputModules/
    │   ├── InputModule.h        # Base class
    │   ├── InputModule.cpp
    │   ├── MF_Button.h
    │   ├── MF_Button.cpp
    │   ├── MF_Encoder.h
    │   ├── MF_Encoder.cpp
    │   ├── MF_Switch.h
    │   ├── MF_Switch.cpp
    │   ├── MF_Analog.h
    │   └── MF_Analog.cpp
    └── Config/
        ├── ConfigManager.h
        └── ConfigManager.cpp
```

## Building

### Using PlatformIO

```bash
# Install PlatformIO
pip install platformio

# Build firmware
cd KeyFlightMega
pio run

# Upload to Mega
pio run --target upload

# Monitor serial output
pio device monitor
```

### Using Arduino IDE

1. Open `src/main.cpp` in Arduino IDE
2. Copy library folders to Arduino libraries folder
3. Select **Board**: Arduino Mega 2560
4. Select **Port**: Your Mega's COM port
5. Click **Upload**

## Configuration

### Serial Communication

- **PC Serial**: `Serial` @ 115200 baud (USB connection)
- **Pro Micro Serial**: `Serial1` @ 115200 baud (RX1/TX1 pins)

### Pin Assignments

- **Digital Pins**: 0-53 (avoid 0, 1, 18, 19 for serial)
- **Analog Pins**: A0-A15 (54-69 in digital mode)
- **Serial1**: TX1 (pin 18), RX1 (pin 19) for Pro Micro

### Wiring

```
Mega 2560          Pro Micro
---------          ----------
TX1 (18)   ----->  RX (pin 0)
RX1 (19)   <-----  TX (pin 1)
GND        ------  GND
```

## CommandMessenger Protocol

### Commands Received from PC

| Command | ID | Format | Description |
|---------|----|----|-------------|
| kStatus | 0 | `0;` | Request device status |
| kSetPin | 5 | `5,<id>,<type>,<pin>,<pin2>;` | Set module pin config |
| kSetKey | 6 | `6,<id>,<keyCommand>,<keyCommand2>;` | Set keyboard command |
| kResetConfig | 8 | `8;` | Reset configuration |
| kSaveConfig | 9 | `9;` | Save to EEPROM |
| kGetConfig | 3 | `3;` | Request configuration |

### Commands Sent to PC

| Command | ID | Format | Description |
|---------|----|----|-------------|
| kInfo | 1 | `1,MEGA,1.0.0,40;` | Device information |
| kConfigInfo | 4 | `4,<id>,<type>,<pin>,<pin2>;` | Module configuration |
| kTrigger | 7 | `7,<moduleId>,<value>;` | Input triggered |
| kAck | 10 | `10,<message>;` | Acknowledgment |
| kError | 11 | `11,<message>;` | Error message |
| kDebug | 12 | `12,<message>;` | Debug message |

### Communication with Pro Micro

When an input triggers, Mega sends to Pro Micro:
```
<moduleId>,<value>;
```

**Value meanings:**
- **Button**: Always 0
- **Encoder**: 1 (CW), -1 (CCW)
- **Switch**: 1 (ON), 0 (OFF)
- **Analog**: 0-1023 (analog value)

## Input Module Details

### MF_Button
- Digital input with INPUT_PULLUP
- 50ms debounce delay
- Triggers on button press (HIGH → LOW)

### MF_Encoder
- Quadrature encoding (2 pins)
- Sensitivity: 1, 10, 100, 1000 pulses per trigger
- Returns direction: 1 (CW), -1 (CCW)

### MF_Switch
- Toggle switch with INPUT_PULLUP
- 50ms debounce delay
- Returns position: 1 (ON/LOW), 0 (OFF/HIGH)

### MF_Analog
- Analog input (0-1023)
- Configurable threshold (default: 10)
- Triggers when value changes by threshold amount

## EEPROM Configuration

### Structure

```cpp
struct DeviceConfig {
    char signature[8];     // "KEYFLT"
    uint8_t version;       // 1
    uint8_t moduleCount;   // 0-40
    ModuleConfig modules[40];
};

struct ModuleConfig {
    uint8_t id;
    uint8_t type;
    uint8_t pin;
    uint8_t pin2;
    char keyCommand[32];
    char keyCommand2[32];
    uint16_t sensitivity;
    bool enabled;
};
```

### Storage
- **Address**: 0x0000
- **Size**: ~3.2 KB (for 40 modules)
- **Signature**: "KEYFLT" for validation

## Testing

### Serial Monitor Test

1. Open serial monitor @ 115200 baud
2. Send: `0;` (request status)
3. Expect: `1,MEGA,1.0.0,40;` (device info)

### Module Configuration Test

```
# Add button on pin 2
5,0,BUTTON,2,0;

# Set key command for module 0
6,0,SPACE;

# Save configuration
9;
```

### Input Test

Connect a button to pin 2 and GND. When pressed, you should see:
```
7,0,0;
```

## Troubleshooting

### No Serial Response
- Check baud rate is 115200
- Verify USB cable is connected
- Try pressing reset button on Mega

### Module Not Triggering
- Verify pin number is correct
- Check wiring (buttons should connect pin to GND)
- Use INPUT_PULLUP (button connects to GND, not VCC)
- Check module is enabled in configuration

### Serial Communication Issues
- Ensure Serial1 TX/RX are connected correctly
- Verify both boards use same baud rate (115200)
- Check GND is connected between boards

### EEPROM Issues
- If configuration fails to load, firmware creates new default config
- To reset: send `8;` (kResetConfig)
- Signature "KEYFLT" must match exactly

## Next Steps

1. **Build and upload** firmware to Mega 2560
2. **Test** with C# Connector application
3. **Implement** Pro Micro firmware for HID keyboard
4. **Connect** all three components (PC ↔ Mega ↔ Pro Micro)
5. **Test** end-to-end keyboard command execution

## Version

- **Firmware**: 1.0.0
- **Board**: Arduino Mega 2560
- **Platform**: PlatformIO / Arduino IDE

---

**KeyFlight Project** - MobiFlight-inspired keyboard command controller
