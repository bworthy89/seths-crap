# Upgrading to Arduino Input Configurator v2.0

## Quick Summary

Your Arduino Input Configurator has been upgraded to **version 2.0** with three major new features:

1. **7-Segment Displays** for encoders (TM1637, TM1638, MAX7219)
2. **Encoder Push Buttons** for mode switching and custom actions
3. **Latching Switches** with LED control and mutual exclusion rules

---

## What's Changed

### Files Modified
- `arduino-config-mega/arduino-config-mega.ino` - Enhanced firmware (v2.0)
- `gui/arduino_configurator.py` - Enhanced GUI (v2.0)
- `VERSION` - Updated to 2.0.0

### New Files Created
- `FEATURES_V2.md` - Comprehensive feature documentation
- `UPGRADE_TO_V2.md` - This file

### Breaking Changes
⚠️ **IMPORTANT:** The EEPROM configuration format has changed!
- Old configurations will NOT load automatically
- You must reconfigure your inputs using the GUI
- New magic number: `0xAC02` (was `0xAC01`)

---

## Quick Start Guide

### Step 1: Install Arduino Libraries (Optional)

**Only needed if using 7-segment displays:**

Open Arduino IDE → Tools → Manage Libraries

For TM1637 displays:
```
Search: "TM1637"
Install: "TM1637" by Avishay Orpaz
```

For TM1638 displays:
```
Search: "TM1638"
Install: "TM1638" by Ricardo Batista
```

For MAX7219 displays:
```
Search: "LedControl"
Install: "LedControl" by Eberhard Fahle
```

### Step 2: Upload New Firmware

1. Open Arduino IDE
2. Open: `arduino-config-mega/arduino-config-mega.ino`
3. Select Board: **Arduino Mega or Mega 2560**
4. Select Processor: **ATmega2560 (Mega 2560)**
5. Select Port: Your Mega's COM port
6. Click **Upload** (Ctrl+U)

**If using displays:**
- Uncomment lines 40-42 for library includes
- Uncomment lines 411-447 in `initializeDisplay()`
- Uncomment lines 670-703 in `updateDisplay()`

### Step 3: Launch New GUI

Run the GUI as usual:
```batch
launch-gui.bat
```

Or manually:
```batch
cd gui
python arduino_configurator.py
```

You'll see the new **v2.0** title and **"Advanced Settings"** button.

### Step 4: Reconfigure Your Inputs

Since the EEPROM format changed, you need to:
1. Connect to Arduino
2. Add your inputs again using the GUI
3. For encoders or latching switches, use **"Advanced Settings"** to configure new features
4. Upload configuration

---

## New GUI Features

### Advanced Settings Button
Click this button after selecting a row to configure:
- **For Rotary Encoders:**
  - 7-segment display (type, pins, digits, min/max values)
  - Encoder push button (short/long press actions)

- **For Latching Switches:**
  - LED control pin
  - Mutual exclusion rules (which switches to turn off)

### New Input Type
**Latching Switch** - Now available in the Type dropdown
- Use for KD2-22 toggle switches
- Supports LED feedback
- Supports mutual exclusion rules

---

## Hardware Setup Examples

### Example 1: Simple Display
Connect a TM1637 4-digit display to an encoder:
```
TM1637 CLK → Mega Pin 22
TM1637 DIO → Mega Pin 23
TM1637 VCC → Mega 5V
TM1637 GND → Mega GND

Encoder A → Mega Pin 2 (interrupt)
Encoder B → Mega Pin 3 (interrupt)
Encoder SW → Mega Pin 4 (optional button)
Encoder GND → Mega GND
```

GUI Configuration:
1. Add Rotary Encoder: pins 2 and 3
2. Advanced Settings → Display Type: TM1637
3. CLK Pin: 22, DIO Pin: 23
4. Digits: 4, Min: 0, Max: 9999
5. (Optional) Button Pin: 4, Short Press: Cycle Modes

### Example 2: Mode Switches with LEDs
Three latching switches for game modes:
```
Switch 1 → Mega Pin 5,  LED → Pin 8 (via 220Ω resistor)
Switch 2 → Mega Pin 6,  LED → Pin 9 (via 220Ω resistor)
Switch 3 → Mega Pin 7,  LED → Pin 10 (via 220Ω resistor)
All GND → Mega GND
```

GUI Configuration:
1. Add 3 Latching Switches (pins 5, 6, 7)
2. For each, set keyboard commands (CTRL+1, CTRL+2, CTRL+3)
3. Advanced Settings for each:
   - Switch 1: LED Pin 8, Mutex List: [1, 2]
   - Switch 2: LED Pin 9, Mutex List: [0, 2]
   - Switch 3: LED Pin 10, Mutex List: [0, 1]

Result: Only one switch can be ON at a time, LED shows which is active.

---

## Compatibility Notes

### Backward Compatibility
- ❌ Old EEPROM configs will NOT load (must reconfigure)
- ✅ Old hardware still works (backward compatible)
- ✅ Old GUI commands still work
- ✅ Pro Micro firmware unchanged (no need to re-upload)

### Memory Usage
- New config uses ~99 bytes per input (was ~41 bytes)
- 40 inputs now use ~3,960 bytes (was ~1,640 bytes)
- Arduino Mega has 4KB EEPROM, so still plenty of room

### Pin Usage
- No change in pin requirements for basic features
- Displays use 2-3 additional pins each
- LEDs use 1 additional pin each
- Encoder buttons use 1 additional pin each

---

## Troubleshooting

### "Invalid config magic/version" on Serial Monitor
**Normal!** This means the old v1.0 config was detected and rejected.
**Solution:** Upload a new configuration via the GUI.

### Display not showing anything
1. Check library installation (Arduino IDE → Tools → Manage Libraries)
2. Verify wiring with multimeter
3. Uncomment library includes in firmware (lines 40-42)
4. Uncomment display code (lines 411-447, 670-703)
5. Re-upload firmware

### "Advanced Settings" button does nothing
**Expected** for Button, Switch, and Potentiometer types.
**Only works** for Rotary Encoder and Latching Switch types.

### LEDs always on/off
1. Check polarity: Long leg = positive (to pin via resistor)
2. Verify resistor: Use 220Ω-470Ω
3. Test LED directly: Connect to 5V/GND to verify it works
4. Check Serial Monitor for "Switch X turned off" messages

---

## Testing Your Upgrade

### Test 1: Basic Upload
1. Upload firmware to Mega
2. Launch GUI
3. Connect to Mega
4. Check console for "Arduino Mega Input Handler Ready (v2.0 - Enhanced)"

### Test 2: Display Feature (if using displays)
1. Add Rotary Encoder
2. Configure Advanced Settings → Display
3. Upload to Arduino
4. Turn encoder
5. Display should show changing numbers

### Test 3: Latching Switch
1. Add Latching Switch
2. Configure LED pin in Advanced Settings
3. Upload to Arduino
4. Press switch
5. LED should turn on/off with switch state

---

## Next Steps

1. **Read Full Documentation**: `FEATURES_V2.md` has detailed examples
2. **Configure Your Setup**: Use the GUI to set up your inputs
3. **Test Incrementally**: Add one input at a time, test, then add more
4. **Check Serial Monitor**: Verify Arduino is responding correctly

---

## Need Help?

- **Full Feature Guide**: `FEATURES_V2.md`
- **Main Documentation**: `CLAUDE.md`
- **Pro Micro Issues**: `PRO_MICRO_DEBUG.md`
- **Keyboard Not Working**: `TROUBLESHOOTING_NO_KEYBOARD.md`
- **GitHub Issues**: https://github.com/bworthy89/seths-crap/issues

---

## Rollback to v1.0 (if needed)

If you need to go back to v1.0 for any reason:
1. Restore files from git: `git checkout v1.0.0`
2. Or download v1.0 release from GitHub
3. Re-upload old firmware to Mega
4. Reconfigure your inputs with old GUI

---

**Upgrade completed!** 🎉

Your Arduino Input Configurator is now running v2.0 with advanced features.
See `FEATURES_V2.md` for detailed usage examples and configuration guides.
