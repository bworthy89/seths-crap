# Arduino Input Configurator v2.0 - New Features Guide

## Overview

Version 2.0 introduces three major feature enhancements to the Arduino Input Configurator:

1. **7-Segment Display Support** - Show encoder values on LED displays
2. **Encoder Push Button** - Use encoder shaft button for mode switching and custom actions
3. **Latching Switches** - KD2-22 switches with LED control and mutual exclusion

---

## Feature 1: 7-Segment Display Support

### What It Does
Connect a 7-segment LED display to any rotary encoder to show the current encoder position/value in real-time.

### Supported Display Types
- **TM1637** (4-digit, 2-wire interface)
- **TM1638** (8-digit, 3-wire interface)
- **MAX7219** (8-digit, SPI interface)

### Hardware Connections

#### TM1637 Example
```
Display Pin  →  Arduino Mega Pin
CLK          →  22 (configurable)
DIO          →  23 (configurable)
VCC          →  5V
GND          →  GND
```

#### TM1638 Example
```
Display Pin  →  Arduino Mega Pin
CLK          →  22 (configurable)
DIO          →  23 (configurable)
STB          →  24 (configurable)
VCC          →  5V
GND          →  GND
```

#### MAX7219 Example
```
Display Pin  →  Arduino Mega Pin
CLK          →  22 (configurable)
DIN          →  23 (configurable)
CS/LOAD      →  24 (configurable)
VCC          →  5V
GND          →  GND
```

### GUI Configuration Steps

1. Add a **Rotary Encoder** input to your configuration
2. Click **"Advanced Settings"** with the encoder row selected
3. In the **7-Segment Display** section:
   - Select Display Type (TM1637, TM1638, or MAX7219)
   - Set CLK pin (e.g., 22)
   - Set DIO/DIN pin (e.g., 23)
   - Set CS/STB pin if using TM1638 or MAX7219 (e.g., 24)
   - Choose how many digits to display (1-8, default 4)
   - Set minimum value (e.g., 0)
   - Set maximum value (e.g., 9999)
4. Click **OK** to save

### How It Works
- The encoder position is mapped to a display value
- Turning the encoder updates the display in real-time
- Value is clamped between min/max
- Encoder mode (1x, 10x, 100x, 1000x) affects how fast the value changes
- Display shows actual value, not raw encoder position

### Example Use Cases
- **Volume Control**: Display shows 0-100
- **Frequency Tuner**: Display shows frequency (e.g., 880-1200 Hz)
- **Counter**: Display shows item count (0-9999)
- **Temperature Setpoint**: Display shows desired temperature

---

## Feature 2: Encoder Push Button

### What It Does
Use the encoder's built-in push button (shaft button) to trigger actions:
- Cycle through increment modes (1x → 10x → 100x → 1000x)
- Send custom keyboard commands
- Reset the display value to minimum
- Or combinations of the above

### Hardware Connection
```
Encoder Pin  →  Arduino Mega Pin
A            →  2 or 3 (interrupt pin, for rotation)
B            →  3 or 18 (interrupt pin, for rotation)
SW/BTN       →  4-53 (any digital pin, for button)
GND          →  GND
```

**Note:** The switch pin on most encoders pulls LOW when pressed, so it uses INPUT_PULLUP mode.

### GUI Configuration Steps

1. Select your Rotary Encoder row
2. Click **"Advanced Settings"**
3. In the **Encoder Push Button** section:
   - Set Button Pin (e.g., 4)
   - Choose **Short Press** action:
     - None
     - Cycle Modes (changes 1x/10x/100x/1000x)
     - Send Key (specify keyboard command like CTRL+ENTER)
     - Reset Display (resets to minimum value)
   - Choose **Long Press** action (same options)
   - Set long press threshold (default 1000ms = 1 second)
4. Click **OK** to save

### Short Press vs. Long Press
- **Short Press**: Tap the button quickly (less than threshold)
- **Long Press**: Hold the button for longer than threshold
- You can configure different actions for each

### Example Configurations

#### Example 1: Mode Cycling
```
Button Pin: 4
Short Press: Cycle Modes
Long Press: Reset Display
Long Press Time: 1000 ms
```
- Tap button: Switch between 1x, 10x, 100x, 1000x modes
- Hold button: Reset display to 0 (or minimum value)

#### Example 2: Custom Commands
```
Button Pin: 4
Short Press: Send Key → "CTRL+ENTER"
Long Press: Send Key → "CTRL+0"
Long Press Time: 1500 ms
```
- Tap button: Sends CTRL+ENTER
- Hold button 1.5 seconds: Sends CTRL+0

#### Example 3: Hybrid
```
Button Pin: 4
Short Press: Cycle Modes
Long Press: Send Key → "CTRL+R"
Long Press Time: 800 ms
```
- Tap button: Cycle modes
- Hold button 0.8 seconds: Sends CTRL+R (e.g., refresh)

---

## Feature 3: Latching Switches with LED Control

### What It Does
Support for KD2-22 style latching/toggle switches that:
- Stay in ON or OFF position when pressed
- Control an LED that reflects the switch state
- Can automatically turn off other switches (mutual exclusion)

### Hardware Connections

#### Basic Latching Switch
```
Switch Pin  →  Arduino Mega Pin
One side    →  2-53 (any digital pin)
Other side  →  GND
```

#### With LED Control
```
Switch Pin  →  Arduino Mega Pin
One side    →  5 (switch input)
Other side  →  GND

LED+        →  6 (LED control pin) via 220Ω resistor
LED-        →  GND
```

### GUI Configuration Steps

1. Add a **Latching Switch** input to your configuration
2. Set the Pin (switch input pin)
3. Set the Keyboard Command (sent when switch turns ON)
4. Click **"Advanced Settings"**
5. In the **LED Control** section:
   - Set LED Pin (or 0 for none)
6. In the **Mutual Exclusion Rules** section:
   - Select other inputs that should turn OFF when this switch turns ON
   - You can select multiple inputs
7. Click **OK** to save

### How It Works

#### LED Control
- When switch latches ON: LED pin goes HIGH, LED lights up
- When switch latches OFF: LED pin goes LOW, LED turns off

#### Mutual Exclusion
When you activate Switch A and it has Switch B and Switch C in its mutex list:
1. Switch A turns ON
2. LED for Switch A turns ON
3. Switches B and C automatically turn OFF
4. LEDs for B and C turn OFF

This creates "one-on-at-a-time" groups.

### Example Use Case: Mode Selection Switches

**Setup:** Three latching switches for game modes
- Switch 0: "Easy Mode" (pin 5, LED pin 8)
- Switch 1: "Normal Mode" (pin 6, LED pin 9)
- Switch 2: "Hard Mode" (pin 7, LED pin 10)

**Mutual Exclusion Configuration:**
- Switch 0 mutex list: [1, 2] (turn off Normal and Hard)
- Switch 1 mutex list: [0, 2] (turn off Easy and Hard)
- Switch 2 mutex list: [0, 1] (turn off Easy and Normal)

**Result:** Only one mode can be active at a time, with LED indicating current mode.

---

## Required Arduino Libraries

To use the display features, you need to install these libraries via Arduino IDE:

### For TM1637
```
Library Manager → Search "TM1637" → Install "TM1637" by Avishay Orpaz
```

### For TM1638
```
Library Manager → Search "TM1638" → Install "TM1638" by Ricardo Batista
OR "TM1638lite" by Jason Cox
```

### For MAX7219
```
Library Manager → Search "LedControl" → Install "LedControl" by Eberhard Fahle
```

### Enabling Libraries in Firmware

In `arduino-config-mega.ino`, **uncomment** the library includes (lines 40-42):

```cpp
// Before (displays won't work):
// #include <TM1637Display.h>
// #include <TM1638.h>
// #include <LedControl.h>

// After (displays will work):
#include <TM1637Display.h>
#include <TM1638.h>
#include <LedControl.h>
```

Then uncomment the display initialization code in:
- `initializeDisplay()` function (lines 411-447)
- `updateDisplay()` function (lines 670-703)

**Note:** You only need to uncomment the libraries you're actually using.

---

## Compatibility Notes

### EEPROM Magic Number Change
**IMPORTANT:** Version 2.0 uses a new EEPROM structure with magic number `0xAC02`.

**This means:**
- Old configurations (v1.0) will NOT load automatically
- You must reconfigure your inputs via the GUI
- This is intentional to ensure data integrity

### Memory Usage
The new InputConfig structure uses approximately **99 bytes per input**.
- 40 inputs × 99 bytes = ~3,960 bytes
- Arduino Mega has 4KB (4,096 bytes) EEPROM
- Leaves ~136 bytes overhead (adequate)

### Pin Usage Recommendations

**For Encoders with Displays:**
- Use interrupt pins (2, 3, 18, 19, 20, 21) for encoder A/B
- Use any digital pin for encoder button
- Use any digital pins for display (CLK, DIO, CS)

**For Latching Switches:**
- Switch input: Any digital pin (2-53)
- LED output: Any digital pin (2-53), avoid TX/RX (0,1)

---

## Troubleshooting

### Display Not Working
1. **Check library installation**: Tools → Manage Libraries
2. **Verify wiring**: Use multimeter to test connections
3. **Uncomment includes**: Check lines 40-42 in firmware
4. **Uncomment display code**: Lines 411-447 and 670-703
5. **Check Serial Monitor**: Look for "Display configured" message
6. **Test with simple sketch**: Upload TM1637 example sketch first

### Encoder Button Not Responding
1. **Check pin number**: Verify button pin in Advanced Settings
2. **Test with multimeter**: Button should connect pin to GND when pressed
3. **Check Serial Monitor**: Look for "Encoder X mode changed" messages
4. **Verify debounce**: Try increasing DEBOUNCE_MS if bouncy

### LED Not Lighting
1. **Check resistor**: Use 220Ω-470Ω resistor in series with LED
2. **Verify polarity**: Long leg (anode) to resistor/pin, short leg to GND
3. **Test LED**: Connect LED+resistor directly to 5V/GND
4. **Check pin mode**: Firmware sets LED pins to OUTPUT automatically

### Mutual Exclusion Not Working
1. **Verify mutex list**: Check Advanced Settings → Mutex list
2. **Input indices**: List uses input index (0, 1, 2...), not pin numbers
3. **Check Serial Monitor**: Look for "Switch X turned off by mutex rule"
4. **Latching Switch only**: Mutex only works with "Latching Switch" type

---

## Example Configurations

### Example 1: Volume Knob with Display
**Hardware:**
- Encoder on pins 2 (A), 3 (B), 4 (SW)
- TM1637 display on pins 22 (CLK), 23 (DIO)

**Configuration:**
```
Type: Rotary Encoder
Pin 1: 2
Pin 2: 3
Encoder Mode: 1x
Keyboard Command: CTRL+UPARROW (or CTRL+DOWNARROW for CCW)

Advanced Settings:
  Display Type: TM1637
  CLK Pin: 22
  DIO Pin: 23
  Digits: 3
  Min Value: 0
  Max Value: 100

  Button Pin: 4
  Short Press: Cycle Modes
  Long Press: Reset Display
  Long Press Time: 1000 ms
```

**Usage:**
- Turn encoder: Volume increases (sends CTRL+UPARROW), display shows 0-100
- Tap button: Switch between 1x (fine) and 10x (coarse) adjustment
- Hold button: Reset volume to 0

### Example 2: Weapon Selection (Gaming)
**Hardware:**
- 3 latching switches on pins 5, 6, 7
- 3 LEDs on pins 8, 9, 10

**Configuration:**
```
Input 0:
  Type: Latching Switch
  Pin: 5
  Keyboard Command: CTRL+1
  Advanced → LED Pin: 8
  Advanced → Mutex List: [1, 2]

Input 1:
  Type: Latching Switch
  Pin: 6
  Keyboard Command: CTRL+2
  Advanced → LED Pin: 9
  Advanced → Mutex List: [0, 2]

Input 2:
  Type: Latching Switch
  Pin: 7
  Keyboard Command: CTRL+3
  Advanced → LED Pin: 10
  Advanced → Mutex List: [0, 1]
```

**Usage:**
- Press switch 0: Sends CTRL+1, LED 8 ON, others turn OFF
- Press switch 1: Sends CTRL+2, LED 9 ON, others turn OFF
- Press switch 2: Sends CTRL+3, LED 10 ON, others turn OFF

### Example 3: Frequency Tuner
**Hardware:**
- Encoder on pins 18 (A), 19 (B), 20 (SW)
- MAX7219 8-digit display on pins 24 (CLK), 25 (DIN), 26 (CS)

**Configuration:**
```
Type: Rotary Encoder
Pin 1: 18
Pin 2: 19
Encoder Mode: 10x
Keyboard Command: CTRL+F (or custom)

Advanced Settings:
  Display Type: MAX7219
  CLK Pin: 24
  DIO/DIN Pin: 25
  CS/STB Pin: 26
  Digits: 4
  Min Value: 880
  Max Value: 8000

  Button Pin: 20
  Short Press: Cycle Modes
  Long Press: Send Key → CTRL+ENTER
  Long Press Time: 1200 ms
```

**Usage:**
- Turn encoder: Adjust frequency (880-8000 Hz)
- Tap button: Cycle between 1x, 10x, 100x, 1000x adjustment speeds
- Hold button 1.2s: Send CTRL+ENTER to apply/confirm frequency

---

## Additional Resources

- **Main Documentation**: `CLAUDE.md`
- **Troubleshooting**: `TROUBLESHOOTING_NO_KEYBOARD.md`, `PRO_MICRO_DEBUG.md`
- **Hardware Alternatives**: `HARDWARE_ALTERNATIVES.md`
- **GitHub Repository**: https://github.com/bworthy89/seths-crap
- **Arduino Library Manager**: Tools → Manage Libraries
- **TM1637 Datasheet**: https://www.makerguides.com/tm1637-arduino-tutorial/
- **MAX7219 Datasheet**: https://www.analog.com/media/en/technical-documentation/data-sheets/MAX7219-MAX7221.pdf

---

## Version History

**v2.0.0** (Current)
- Added 7-segment display support (TM1637, TM1638, MAX7219)
- Added encoder push button functionality
- Added latching switch type with LED control
- Added mutual exclusion rules for switches
- Updated EEPROM magic number to 0xAC02
- Enhanced GUI with Advanced Settings dialog

**v1.0.0**
- Initial release
- Basic button, encoder, switch, potentiometer support
- EEPROM configuration storage
- Dual-board architecture (Mega + Pro Micro)

---

**Document Version:** 1.0
**Last Updated:** 2025-01-19
**Firmware Version:** 2.0.0
**GUI Version:** 2.0
