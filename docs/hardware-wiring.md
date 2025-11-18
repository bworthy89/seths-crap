# Hardware Wiring Guide

This guide explains how to wire the dual-board Arduino system for the Input Configurator project.

## Required Hardware

### Core Components
- **Arduino Mega 2560** (ELEGOO MEGA R3 or compatible) - Main input handler
- **Arduino Pro Micro** (ATmega32u4) or Arduino Leonardo - USB HID keyboard
- **USB Cables**:
  - 1x USB-B cable (for Mega 2560)
  - 1x Micro USB cable (for Pro Micro)
- **Jumper Wires**: 3x male-to-male jumper wires

### Input Components (Example Quantities)
- **Rotary Encoders**: 5-10 units (recommend with push button)
- **Push Buttons**: 10-20 units (momentary, normally open)
- **Toggle Switches**: 5-10 units (SPST or SPDT)
- **Potentiometers**: 2-5 units (10kΩ linear or logarithmic)
- **0.1µF Capacitors**: 10-20 units (for encoder debouncing)
- **10kΩ Resistors**: Optional (if not using INPUT_PULLUP)

### Assembly Materials
- Breadboard (large, 830 tie-points) or custom PCB
- Wire (22-24 AWG solid core for breadboard)
- Project enclosure (optional)

---

## Board-to-Board Wiring

### UART Serial Connection (Mega ↔ Pro Micro)

The Arduino Mega communicates with the Pro Micro via hardware serial (Serial1).

| Arduino Mega 2560 | Arduino Pro Micro | Purpose |
|-------------------|-------------------|---------|
| Pin 18 (TX1) | RXI (Pin 0) | Data: Mega → Pro Micro |
| Pin 19 (RX1) | TXO (Pin 1) | Data: Pro Micro → Mega (unused in this project) |
| GND | GND | Common ground |

**Wiring Steps:**
1. Connect Mega pin 18 (TX1) to Pro Micro RXI
2. Connect Mega pin 19 (RX1) to Pro Micro TXO
3. Connect GND on Mega to GND on Pro Micro

**Important Notes:**
- Use Serial1 on Mega (pins 18/19), NOT Serial (pins 0/1)
- Serial (pins 0/1) on Mega is for USB communication with PC
- Cross-connect TX→RX and RX→TX
- Pro Micro should be powered via its USB connection to the computer

---

## Input Wiring

### Pin Allocation Strategy

**Arduino Mega 2560 Pinout:**
- **Digital Pins**: 2-53 (52 pins total)
- **Analog Pins**: A0-A15 (16 pins, can also be used as digital)
- **Interrupt Pins**: 2, 3, 18, 19, 20, 21 (best for rotary encoders)
- **I2C/Serial Pins**:
  - 20, 21 (SDA, SCL) - I2C
  - 18, 19 (TX1, RX1) - Serial1 (used for Pro Micro communication)
  - 16, 17 (TX2, RX2) - Serial2 (available)
  - 14, 15 (TX3, RX3) - Serial3 (available)

**Pin Recommendations:**
- **Rotary Encoders**: Use interrupt pins (2, 3, 18, 19, 20, 21) for best responsiveness
  - Note: Pins 18, 19 are used for Serial1, so use 2, 3, 20, 21 for encoders
- **Buttons/Switches**: Any digital pin (2-53)
- **Potentiometers**: Analog pins (A0-A15)

---

### 1. Rotary Encoder Wiring

Rotary encoders have 3-5 pins depending on the model:
- **CLK (A)**: Clock/Phase A
- **DT (B)**: Data/Phase B
- **SW**: Push button (optional)
- **+ (VCC)**: Power (usually 5V)
- **GND**: Ground

**Basic Wiring (without hardware debouncing):**
```
Rotary Encoder          Arduino Mega
━━━━━━━━━━━━━━━       ━━━━━━━━━━━━━━━
CLK (A)         ───────→ Pin 2 (or other interrupt pin)
DT (B)          ───────→ Pin 3 (or other interrupt pin)
SW              ───────→ Pin 4 (any digital pin)
+               ───────→ 5V
GND             ───────→ GND
```

**Recommended Wiring (with hardware debouncing):**
```
Rotary Encoder          Capacitor       Arduino Mega
━━━━━━━━━━━━━━━       ━━━━━━━━━━━    ━━━━━━━━━━━━━━━
CLK (A)         ────┬───[0.1µF]───┬─→ Pin 2
                    └─────────────┘
                         GND

DT (B)          ────┬───[0.1µF]───┬─→ Pin 3
                    └─────────────┘
                         GND

SW              ────┬───[0.1µF]───┬─→ Pin 4
                    └─────────────┘
                         GND

+               ────────────────────→ 5V
GND             ────────────────────→ GND
```

**Multiple Encoder Example:**
| Encoder | CLK (A) Pin | DT (B) Pin | SW Pin | Purpose Example |
|---------|-------------|------------|--------|-----------------|
| 1 | 2 | 3 | 4 | Volume Control |
| 2 | 20 | 21 | 22 | Frequency Tuning |
| 3 | 5 | 6 | 7 | Parameter Adjust |
| 4 | 8 | 9 | 10 | Menu Navigation |
| 5 | 11 | 12 | 13 | Fine Tuning |

---

### 2. Push Button Wiring

Push buttons are typically momentary switches (normally open).

**Using Internal Pull-up Resistor (Recommended):**
```
Push Button             Arduino Mega
━━━━━━━━━━━            ━━━━━━━━━━━━━━━
One terminal    ───────→ Pin 23 (any digital pin)
Other terminal  ───────→ GND
```

The firmware uses `INPUT_PULLUP` mode, so no external resistor is needed.

**Using External Pull-down Resistor (Alternative):**
```
                    10kΩ
Push Button         Resistor        Arduino Mega
━━━━━━━━━━━        ━━━━━━━━━━     ━━━━━━━━━━━━━━━
One terminal ───┬──────────────────→ Pin 23
                │
                └───[10kΩ]─────────→ GND

Other terminal ─────────────────────→ 5V
```

---

### 3. Toggle Switch Wiring

Toggle switches (SPST or SPDT) are wired identically to push buttons.

**SPST (Single Pole Single Throw):**
```
Toggle Switch           Arduino Mega
━━━━━━━━━━━━━          ━━━━━━━━━━━━━━━
Common          ───────→ Pin 24 (any digital pin)
NO/NC           ───────→ GND
```

**SPDT (Single Pole Double Throw):**
Connect only the common terminal and one of the other terminals (NO or NC).

---

### 4. Potentiometer Wiring

Potentiometers are analog inputs (0-5V = 0-1023 ADC value).

```
Potentiometer           Arduino Mega
━━━━━━━━━━━━━          ━━━━━━━━━━━━━━━
Left terminal   ───────→ GND
Center (wiper)  ───────→ A0 (any analog pin)
Right terminal  ───────→ 5V
```

**Multiple Potentiometer Example:**
| Potentiometer | Analog Pin | Purpose Example |
|---------------|------------|-----------------|
| 1 | A0 | Throttle |
| 2 | A1 | Rudder |
| 3 | A2 | Pitch |
| 4 | A3 | Mix Level |
| 5 | A4 | Balance |

---

## USB Connections

### Arduino Mega 2560
- **USB-B cable** → PC
- **Purpose**:
  - Upload firmware
  - Communicate with Python GUI
  - Power the board (if not using external power)

### Arduino Pro Micro
- **Micro USB cable** → PC
- **Purpose**:
  - Upload firmware
  - Send USB HID keyboard commands to computer
  - Power the board

**Both boards must be connected to the PC simultaneously during operation.**

---

## Power Considerations

### Option 1: USB Power (Recommended for Development)
- Both boards powered via USB
- Simple, no additional components needed
- Suitable for up to ~20-30 inputs

### Option 2: External Power for Mega (For Large Setups)
- Mega: External 7-12V DC adapter (center positive, 2.1mm barrel jack)
- Pro Micro: USB power
- Recommended if using 30+ inputs or high-current components
- Provides more stable power for many components

**Warning**: Do NOT power the Mega via both USB and external power simultaneously unless you understand the voltage regulator limitations.

---

## Physical Layout Recommendations

### Breadboard Layout
```
┌──────────────────────────────────────────────────┐
│                                                  │
│   [Mega 2560]              [Pro Micro]           │
│                                                  │
│   ┌───────┐                ┌───────┐            │
│   │USB-B  │                │MicroUSB│           │
│   └───┬───┘                └───┬───┘            │
│       │                        │                 │
│   [Encoders Area]          [Buttons Area]        │
│   ┌─────────────┐         ┌──────────────┐      │
│   │ ○  ○  ○  ○  │         │ ●  ●  ●  ●   │      │
│   │ ○  ○  ○  ○  │         │ ●  ●  ●  ●   │      │
│   └─────────────┘         └──────────────┘      │
│                                                  │
│   [Potentiometers Area]    [Switches Area]       │
│   ┌─────────────┐         ┌──────────────┐      │
│   │ ⚊  ⚊  ⚊     │         │ ⎅  ⎅  ⎅      │      │
│   └─────────────┘         └──────────────┘      │
│                                                  │
└──────────────────────────────────────────────────┘
```

### Tips
1. **Keep encoders close to Mega**: Minimize wire length to reduce noise
2. **Group similar inputs**: Makes troubleshooting easier
3. **Label everything**: Use tape or labels for each input
4. **Use color-coded wires**:
   - Red: 5V
   - Black: GND
   - Other colors: Signal wires
5. **Cable management**: Use zip ties or breadboard wire organizers

---

## Testing Connections

### Step 1: Test Board Communication
1. Upload firmware to both boards
2. Connect USB cables to PC
3. Open Serial Monitor for Mega (115200 baud)
4. You should see: "Arduino Mega Input Handler Ready"

### Step 2: Test Pro Micro Keyboard Output
1. Open Notepad or text editor
2. Send test command from GUI
3. Pro Micro LED should blink
4. Text should appear in editor

### Step 3: Test Individual Inputs
1. Configure one input in GUI
2. Upload configuration
3. Trigger input (press button, turn encoder, etc.)
4. Verify keyboard command is sent

### Step 4: Test Multiple Inputs
1. Configure all inputs
2. Upload configuration
3. Test each input systematically
4. Verify no pin conflicts

---

## Troubleshooting

### Problem: No communication between boards
- **Check**: GND connection between Mega and Pro Micro
- **Check**: TX/RX crossover (TX→RX, RX→TX)
- **Check**: Baud rate matches (115200)
- **Check**: Pro Micro Serial Monitor is NOT open (conflicts with Serial1)

### Problem: Encoders missing steps
- **Check**: Using interrupt pins (2, 3, 20, 21)
- **Add**: 0.1µF capacitors for hardware debouncing
- **Check**: Encoder library installed
- **Check**: Encoder not rotating too fast

### Problem: Buttons triggering multiple times
- **Check**: Bounce2 library installed
- **Check**: DEBOUNCE_MS value (try increasing to 10-20ms)
- **Add**: 0.1µF capacitor across button terminals

### Problem: Potentiometers jittery
- **Check**: Good connections (no loose wires)
- **Check**: ANALOG_THRESHOLD value (try increasing to 15-20)
- **Add**: 0.1µF capacitor between wiper and GND
- **Check**: Power supply stability

### Problem: Keyboard commands not working
- **Check**: Pro Micro has Keyboard.h support (32u4 chip)
- **Check**: Pro Micro connected to correct USB port
- **Check**: Commands start with "CTRL+"
- **Check**: Active window accepts keyboard input

---

## Advanced: Custom PCB Design

For a permanent installation, consider designing a custom PCB with:
- Arduino Mega socket or footprint
- Pro Micro socket or footprint
- Screw terminals for all inputs
- Built-in debouncing capacitors
- LED indicators for each input
- Labeled input sections
- Mounting holes

**Recommended Tools:**
- KiCad (free, open-source PCB design)
- EasyEDA (free, browser-based)
- Eagle (free for hobbyist boards)

---

## Safety Notes

1. **ESD Protection**: Use anti-static wrist strap when handling boards
2. **Polarity**: Check 5V/GND connections before powering on
3. **Short Circuits**: Avoid shorting 5V to GND
4. **Component Ratings**: Ensure all components are rated for 5V operation
5. **Heat**: Boards may get warm during extended use (normal)
6. **USB Ports**: Don't hot-swap while programming (can damage USB ports)

---

## Bill of Materials (BOM)

### For 30-Input Setup

| Component | Quantity | Est. Cost | Notes |
|-----------|----------|-----------|-------|
| Arduino Mega 2560 | 1 | $12-15 | ELEGOO or clone |
| Arduino Pro Micro | 1 | $5-8 | 5V/16MHz ATmega32u4 |
| Rotary Encoders | 5 | $10-15 | With push button |
| Push Buttons | 15 | $5-10 | Momentary, NO |
| Toggle Switches | 5 | $5-8 | SPST |
| Potentiometers | 5 | $5-8 | 10kΩ linear |
| 0.1µF Capacitors | 20 | $2-5 | Ceramic |
| Breadboard | 1 | $5-10 | 830 tie-points |
| Jumper Wires | 1 pack | $3-5 | Male-to-male |
| USB-B Cable | 1 | $3-5 | For Mega |
| Micro USB Cable | 1 | $2-4 | For Pro Micro |
| **Total** | | **$57-93** | |

---

## Next Steps

After completing the hardware wiring:
1. Upload firmware to both Arduino boards
2. Install Python dependencies: `pip install -r requirements.txt`
3. Run the GUI: `python gui/main.py`
4. Configure your inputs and upload to Arduino
5. Test each input systematically

Refer to `user-guide.md` for detailed software usage instructions.
