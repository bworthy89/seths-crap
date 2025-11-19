# Troubleshooting: No Keyboard Output

## Problem: Arduino LED blinks but Notepad doesn't receive keyboard commands

This means:
- ✅ Mega is working (detecting inputs)
- ✅ Mega firmware is correct
- ❌ Pro Micro not sending keyboard commands

---

## Step 1: Verify Pro Micro Firmware

**Check if you uploaded the Pro Micro firmware:**

1. **Open Arduino IDE**
2. **File → Open:** `arduino-config-promicro/arduino-config-promicro.ino`
3. **Tools → Board:** Arduino Leonardo (NOT Mega!)
4. **Tools → Port:** Select Pro Micro port
   - Windows: Different COM port than Mega
   - Look for "Arduino Leonardo" in port list
5. **Upload** (Ctrl+U)

**Watch for:**
- TX/RX LEDs on Pro Micro should flash during upload
- "Done uploading" message
- No errors

---

## Step 2: Verify Pro Micro USB Connection

**Check physical connection:**

1. **Pro Micro connected to PC via Micro USB cable**
   - Not just power - needs data connection
   - Try different USB port
   - Try different USB cable (some are power-only!)

2. **Check Device Manager (Windows):**
   - Win + X → Device Manager
   - Look under "Ports (COM & LPT)"
   - Should see: "Arduino Leonardo (COMx)"
   - Note the COM port number

**If not showing:**
- Driver issue (rare, usually auto-installs)
- Bad USB cable (very common!)
- Bad USB port
- Pro Micro not working

---

## Step 3: Test Pro Micro Independently

**Upload test sketch to Pro Micro:**

1. **Open Arduino IDE**
2. **New sketch, paste this:**

```cpp
#include <Keyboard.h>

void setup() {
  Keyboard.begin();
  pinMode(LED_BUILTIN, OUTPUT);
  delay(2000);  // Wait 2 seconds

  // Test: Type "Hello" in Notepad
  Keyboard.print("Hello from Pro Micro!");

  // Blink LED to show it ran
  for (int i = 0; i < 5; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(200);
    digitalWrite(LED_BUILTIN, LOW);
    delay(200);
  }
}

void loop() {
  // Do nothing
}
```

3. **Upload to Pro Micro**
4. **Open Notepad**
5. **Press reset button on Pro Micro**

**Expected result:**
- After 2 seconds, "Hello from Pro Micro!" appears in Notepad
- LED blinks 5 times

**If this works:**
- Pro Micro HID keyboard is working
- Problem is communication between Mega and Pro Micro

**If this doesn't work:**
- Pro Micro USB HID not working
- Check board selection (must be Leonardo)
- Check USB cable/port
- Try different computer to rule out OS issue

---

## Step 4: Check Wiring Between Boards

**Verify connections:**

```
Arduino Mega          Arduino Pro Micro
────────────          ─────────────────
Pin 18 (TX1)    ──→   RXI (Pin 0)
Pin 19 (RX1)    ──→   TXO (Pin 1)
GND             ──→   GND
```

**Important:**
- TX goes to RX (crossover!)
- RX goes to TX (crossover!)
- GND to GND

**Pro Micro Pin Labels:**
- Look for "RXI" or "RX" or "0"
- Look for "TXO" or "TX" or "1"
- Some Pro Micros label them differently

**Test continuity:**
- Use multimeter in continuity mode
- Or visually trace the wires

---

## Step 5: Test Serial Communication

**Test if Mega is sending to Pro Micro:**

1. **Disconnect Pro Micro from Mega**
   - Remove TX, RX wires (leave GND for now)

2. **Connect Pro Micro RX to computer via Serial Monitor:**
   - Arduino IDE → Tools → Serial Monitor
   - Baud: 115200
   - Board: Pro Micro

3. **Trigger input on Mega**
   - Press button
   - LED should blink

4. **Check Serial Monitor on Pro Micro**
   - Should see: "CTRL+F" (or your command)

**If you see commands:**
- Communication working
- Pro Micro receiving but not sending keyboard

**If you don't see commands:**
- Check wiring
- Check baud rate (115200 on both)
- Mega might not be sending

---

## Step 6: Debug Pro Micro Reception

**Modify Pro Micro sketch to show status:**

Replace `arduino-config-promicro.ino` temporarily with this debug version:

```cpp
#include <Keyboard.h>

const unsigned long BAUD_RATE = 115200;
const int LED_PIN = LED_BUILTIN;

String inputBuffer = "";

void setup() {
  // Initialize USB HID Keyboard
  Keyboard.begin();

  // Initialize Serial communication with Mega
  Serial1.begin(BAUD_RATE);

  // Initialize LED
  pinMode(LED_PIN, OUTPUT);

  // Blink LED 3 times to show ready
  for (int i = 0; i < 3; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(200);
    digitalWrite(LED_PIN, LOW);
    delay(200);
  }
}

void loop() {
  // Read from Serial1 (connected to Mega)
  while (Serial1.available() > 0) {
    char inChar = Serial1.read();

    // Flash LED on ANY received character
    digitalWrite(LED_PIN, HIGH);
    delay(50);
    digitalWrite(LED_PIN, LOW);

    if (inChar == '\n') {
      // Process complete command
      if (inputBuffer.length() > 0) {
        // Send keyboard command
        if (inputBuffer.startsWith("CTRL+")) {
          String keyPart = inputBuffer.substring(5);

          Keyboard.press(KEY_LEFT_CTRL);
          delay(5);

          if (keyPart.length() == 1) {
            Keyboard.press(keyPart.charAt(0));
          }

          delay(10);
          Keyboard.releaseAll();

          // Long blink for successful send
          digitalWrite(LED_PIN, HIGH);
          delay(200);
          digitalWrite(LED_PIN, LOW);
        }

        inputBuffer = "";
      }
    } else {
      inputBuffer += inChar;
      if (inputBuffer.length() > 50) {
        inputBuffer = "";
      }
    }
  }
}
```

**Upload this to Pro Micro**

**Test:**
1. Press button on Mega
2. **Pro Micro LED should:**
   - Quick flash for each character received
   - Long blink (200ms) when keyboard command sent
3. Check if keyboard output now works in Notepad

**What the LED tells you:**
- **No flashing:** Not receiving serial data from Mega
- **Quick flashes only:** Receiving data but not sending keyboard
- **Quick + long blinks:** Receiving and sending - should work!

---

## Step 7: Common Issues and Fixes

### Issue: Pro Micro LED doesn't blink

**Cause:** Not receiving serial data from Mega

**Fix:**
1. Check TX1 wire from Mega pin 18 to Pro Micro RXI
2. Check GND connection
3. Check Mega is actually sending (check GUI console)
4. Try swapping TX/RX wires (might be reversed)

---

### Issue: LED blinks but no keyboard output

**Cause:** Pro Micro not acting as keyboard

**Fix:**
1. Verify board selection: **Arduino Leonardo** (not Uno, not Mega!)
2. Re-upload firmware
3. Check USB cable (data not just power)
4. Try different USB port
5. Restart computer (Windows can lock USB devices)

---

### Issue: Random characters instead of Ctrl+ commands

**Cause:** Wrong baud rate or electrical noise

**Fix:**
1. Verify both sketches use 115200 baud
2. Check Serial1.begin(115200) in Mega
3. Check Serial1.begin(115200) in Pro Micro
4. Add 0.1µF capacitor between GND and signal wires (noise reduction)
5. Use shorter wires

---

### Issue: Works once then stops

**Cause:** Serial buffer overflow or lock-up

**Fix:**
1. Add delay in Mega sketch after sending command
2. Clear serial buffer periodically
3. Restart Pro Micro (press reset button)

---

### Issue: Only some keys work

**Cause:** Special keys not mapped correctly

**Fix:**
1. Use single characters (CTRL+F, CTRL+A, etc.)
2. Avoid complex special keys initially
3. Check Pro Micro firmware handles the key type

---

## Step 8: Verify Complete System

**Full test procedure:**

1. **Upload both firmwares:**
   - Pro Micro: `arduino-config-promicro.ino` (as Arduino Leonardo)
   - Mega: `arduino-config-mega.ino` (as Arduino Mega)

2. **Connect hardware:**
   - Mega USB to PC
   - Pro Micro USB to PC
   - Mega pin 18 → Pro Micro RXI
   - Mega pin 19 → Pro Micro TXO
   - Mega GND → Pro Micro GND

3. **Configure in GUI:**
   - Connect to Mega
   - Add one button: Pin 22, CTRL+F
   - Upload config

4. **Wire button:**
   - One terminal → Pin 22
   - Other terminal → GND

5. **Test:**
   - Open Notepad
   - Press button
   - **Expected:**
     - Mega LED blinks
     - Pro Micro LED blinks
     - Find dialog opens in Notepad (Ctrl+F)

---

## Quick Diagnostic Chart

| Symptom | Cause | Fix |
|---------|-------|-----|
| Mega LED blinks, Pro Micro LED doesn't | No serial connection | Check TX/RX wiring |
| Both LEDs blink, no keyboard | Pro Micro not HID device | Check board selection (Leonardo) |
| Random characters | Wrong baud rate | Set both to 115200 |
| Works once then stops | Buffer overflow | Add delays, reduce command frequency |
| Keyboard works in test, not in system | Wiring issue | Check connections |

---

## Still Not Working?

**Collect this info:**

1. **Arduino IDE version:** _____
2. **Mega COM port:** _____
3. **Pro Micro COM port:** _____
4. **Board selected for Pro Micro:** _____ (should be "Arduino Leonardo")
5. **Mega LED blinks?** Yes / No
6. **Pro Micro LED blinks?** Yes / No
7. **Test sketch worked?** Yes / No (Step 3)
8. **Serial monitor shows commands?** Yes / No (Step 5)

**Check these common mistakes:**

- [ ] Pro Micro uploaded with wrong board (must be Leonardo!)
- [ ] USB cable is power-only (try different cable)
- [ ] TX/RX wires swapped (should be crossover)
- [ ] Pro Micro not connected to PC via USB
- [ ] Wrong COM port selected
- [ ] Mega and Pro Micro different baud rates

---

## Need More Help?

1. Take photos of:
   - Arduino connections (TX/RX/GND)
   - Both Arduino boards
   - GUI console output

2. Share:
   - Arduino IDE upload output
   - Serial Monitor output (when testing)
   - Device Manager screenshot (COM ports)

3. Post on:
   - GitHub Issues: https://github.com/bworthy89/seths-crap/issues
   - Arduino Forum: https://forum.arduino.cc/

---

**Most common fix:** Re-upload Pro Micro firmware with "Arduino Leonardo" board selected!
