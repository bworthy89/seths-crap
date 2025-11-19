# Pro Micro Debugging Guide

Let's fix your Pro Micro step by step.

## STEP 1: Basic Connection Test

### Check Device Manager

1. **Plug Pro Micro into PC** (Micro USB cable)
2. **Open Device Manager:**
   - Press `Win + X`
   - Click "Device Manager"
3. **Look under "Ports (COM & LPT)"**

**What do you see?**

### ✅ GOOD - "Arduino Leonardo (COMx)"
- Pro Micro is recognized
- Note the COM port number
- Go to STEP 2

### ⚠️ PARTIAL - "USB Serial Device (COMx)" or "Unknown Device"
- Driver issue
- Go to STEP 1B (Driver Fix)

### ❌ BAD - Nothing appears
- USB cable issue OR Pro Micro dead
- Go to STEP 1A (Cable Test)

---

## STEP 1A: Cable Test

**Problem:** Nothing shows in Device Manager

### Test the Cable

1. **Try different Micro USB cable** (very common issue!)
   - Many cables are power-only (no data lines)
   - Try phone charging cable or different cable

2. **Try different USB port on PC**
   - USB 2.0 ports often work better
   - Front panel USB ports can be flaky
   - Try back panel USB port directly on motherboard

3. **Check Pro Micro LED**
   - Does power LED light up? (usually red/green)
   - If no LED at all: dead board OR no power

4. **Try another computer/laptop**
   - Helps identify if issue is with PC or Pro Micro

**If still nothing:** Pro Micro may be dead. Contact seller for replacement.

---

## STEP 1B: Driver Fix

**Problem:** Shows as "Unknown Device" or "USB Serial Device"

### Windows Driver Installation

1. **Download Arduino IDE** (if not installed)
   - https://www.arduino.cc/en/software
   - Install it (includes drivers)

2. **Manual Driver Install:**
   - Device Manager → Right-click unknown device
   - Update Driver → Browse my computer
   - Navigate to: `C:\Program Files (x86)\Arduino\drivers`
   - Select folder and click OK

3. **Alternative: Use Sparkfun drivers**
   - https://learn.sparkfun.com/tutorials/pro-micro--fio-v3-hookup-guide/drivers
   - Download and install

4. **After driver install:**
   - Unplug Pro Micro
   - Replug Pro Micro
   - Check Device Manager again

**If still not working:** Try on different computer to rule out Windows issues.

---

## STEP 2: Arduino IDE Setup

### Configure Arduino IDE

1. **Open Arduino IDE**

2. **Tools → Board → Arduino AVR Boards → Arduino Leonardo**
   - ⚠️ CRITICAL: Must be "Leonardo" not "Mega"!
   - Pro Micro uses same chip as Leonardo

3. **Tools → Port → Select your Pro Micro port**
   - Should say "Arduino Leonardo (COM#)"
   - If you don't see it, go back to STEP 1

4. **Tools → Programmer → "AVR ISP"** (or leave as default)

---

## STEP 3: Upload Test

### Simple Blink Test (No HID)

Let's test if we can upload code at all.

1. **File → Examples → 01.Basics → Blink**

2. **Click Upload** (→ button or Ctrl+U)

3. **Watch for:**
   - "Compiling sketch..."
   - "Uploading..."
   - TX/RX LEDs on Pro Micro should flash rapidly
   - "Done uploading"

**Results:**

### ✅ SUCCESS - "Done uploading"
- Pro Micro is working!
- LED should blink every second
- Go to STEP 4 (Keyboard Test)

### ❌ ERROR - "Couldn't find a Board on the selected port"
- Wrong port selected, OR
- Pro Micro not in bootloader mode
- Go to STEP 3A (Bootloader Mode)

### ❌ ERROR - "Verification failed" or "Device not responding"
- Communication issue
- Go to STEP 3B (Reset Method)

---

## STEP 3A: Enter Bootloader Mode

**Problem:** Can't upload sketch

Pro Micro has a trick: **double-tap reset button** to enter bootloader mode.

### Method 1: Double Reset

1. **Locate reset button** on Pro Micro (tiny button, usually labeled RST or RESET)

2. **Press reset button twice quickly** (like double-clicking a mouse)
   - Press, release, press, release
   - Within 0.5 seconds

3. **Watch for:**
   - Port changes (COMx → COMy)
   - You have 8 seconds to upload before it times out

4. **In Arduino IDE:**
   - Tools → Port → Select NEW port
   - Click Upload immediately

### Method 2: Ground Reset Pin

If no reset button or doesn't work:

1. **Find RST and GND pins** on Pro Micro
2. **Touch a wire** from RST to GND briefly (0.5 seconds)
3. **Repeat quickly** (touch twice)
4. Upload immediately

### Method 3: Auto-Reset

Some Pro Micros support auto-reset:

1. **Click Upload in Arduino IDE first**
2. **Wait until it says "Uploading..."**
3. **Then double-tap reset on Pro Micro**

---

## STEP 3B: Try Different Upload Settings

1. **Slower upload speed:**
   - Tools → Upload Speed → 57600 (if available)

2. **Different USB port:**
   - Back panel USB 2.0 port (not USB 3.0)

3. **Restart Arduino IDE:**
   - Close and reopen
   - Reselect board and port

4. **Restart computer:**
   - Sometimes clears USB stack issues

---

## STEP 4: Keyboard Test

Now let's test HID keyboard functionality.

### Test Sketch

1. **Arduino IDE → New Sketch**

2. **Paste this code:**

```cpp
#include <Keyboard.h>

void setup() {
  // Initialize keyboard
  Keyboard.begin();

  // Wait 5 seconds (gives you time to open Notepad)
  delay(5000);

  // Type a test message
  Keyboard.println("Pro Micro Keyboard Test!");

  // Flash built-in LED to confirm code is running
  pinMode(LED_BUILTIN, OUTPUT);
  for (int i = 0; i < 10; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
  }

  Keyboard.println("Test complete!");
}

void loop() {
  // Do nothing
}
```

3. **Verify board selection:**
   - Tools → Board → **Arduino Leonardo** ✓
   - Tools → Port → Your Pro Micro port

4. **Upload** (Ctrl+U)

5. **Immediately after upload:**
   - Open Notepad quickly
   - Wait 5 seconds
   - Watch for text to appear

### Expected Results

**✅ SUCCESS:**
- After 5 seconds: "Pro Micro Keyboard Test!" appears in Notepad
- LED blinks 10 times (1 second total)
- "Test complete!" appears
- **Your Pro Micro works perfectly!**

**❌ No text appears:**
- LED blinks? Code is running, HID not working → STEP 5
- LED doesn't blink? Code not running → STEP 3A

---

## STEP 5: HID Troubleshooting

**Problem:** Code uploads and runs, but no keyboard output

### Check Windows

1. **Open Notepad or any text editor**
   - Pro Micro must have focus on a text input area
   - Try clicking in Notepad window

2. **Check USB connection**
   - Keyboard.h requires good USB connection
   - Try different USB port
   - Try USB 2.0 port specifically

3. **Check Device Manager:**
   - Should see Pro Micro under both:
     - "Ports (COM & LPT)" - for serial
     - "Keyboards" - for HID (when Keyboard.h used)
   - If not under "Keyboards," HID not working

### Check Anti-virus/Security

Some security software blocks virtual keyboards:

1. **Temporarily disable anti-virus**
2. **Upload and test again**
3. **If works:** Add exception for Arduino IDE

### Try Different Keyboard Library Function

Replace `Keyboard.println()` with:

```cpp
Keyboard.press('H');
delay(100);
Keyboard.release('H');
```

---

## STEP 6: Communication with Mega

Once Pro Micro keyboard test works, connect to Mega.

### Wiring

```
Arduino Mega          Pro Micro
────────────          ─────────
Pin 18 (TX1)    →     RXI (Pin 0)
Pin 19 (RX1)    →     TXO (Pin 1)
GND             →     GND
```

**Important:**
- TX goes to RX (crossover!)
- Keep both boards plugged into PC via USB during testing

### Test Serial Communication

**Upload this to Pro Micro:**

```cpp
void setup() {
  Serial1.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  if (Serial1.available()) {
    // Flash LED when data received
    digitalWrite(LED_BUILTIN, HIGH);
    delay(50);
    digitalWrite(LED_BUILTIN, LOW);

    // Read and discard data (just testing)
    while (Serial1.available()) {
      Serial1.read();
    }
  }
}
```

**Test:**
1. Open Serial Monitor for Mega (115200 baud)
2. Type something and hit send
3. Pro Micro LED should flash

**If LED flashes:** Communication working!
**If LED doesn't flash:** Check TX/RX wiring

---

## COMMON ISSUES CHECKLIST

### Issue: "Port not found" error

- [ ] Pro Micro plugged in?
- [ ] Different cable tried?
- [ ] Different USB port tried?
- [ ] Device Manager shows Arduino Leonardo?
- [ ] Correct port selected in Arduino IDE?

### Issue: Upload fails

- [ ] Board set to "Arduino Leonardo"?
- [ ] Port selected?
- [ ] Double-reset tried?
- [ ] Different USB port tried?
- [ ] Arduino IDE restarted?

### Issue: Upload succeeds but no keyboard output

- [ ] Keyboard.h included in sketch?
- [ ] Notepad/text editor has focus?
- [ ] Anti-virus disabled (temporarily)?
- [ ] Pro Micro shows under "Keyboards" in Device Manager?
- [ ] Different USB port tried?

### Issue: LED blinks but communication doesn't work

- [ ] TX→RX and RX→TX (crossover)?
- [ ] GND connected?
- [ ] Both boards using 115200 baud?
- [ ] Wires firmly connected?
- [ ] Mega pin 18 (TX1) used, not pin 1 (TX0)?

---

## CLUES FROM LED PATTERNS

### Power LED (solid red/green)
- ✅ On: Board getting power
- ❌ Off: No power (cable or board issue)

### TX/RX LEDs (yellow, flash during upload)
- ✅ Flash rapidly during upload: Good
- ❌ Don't flash at all: Not communicating
- ⚠️ Flash once then stop: Upload started but failed

### Built-in LED (programmable, pin 13 on most boards)
- Controlled by your sketch
- Use for debugging (blink patterns)

---

## PRO MICRO VARIANTS

### Pin Labels Vary!

Different Pro Micro clones label pins differently:

**Serial pins might be labeled as:**
- RXI/TXO (most common)
- RX/TX (also common)
- 0/1 (pin numbers)
- RX1/TX1 (on some clones)

**Find the right pins:**
1. Look for pins labeled with serial function
2. Usually near the USB connector
3. Often marked on bottom of board
4. Check schematic/pinout diagram

**Common pinouts:**
- **RXI** = Pin 0 = RX
- **TXO** = Pin 1 = TX

---

## LAST RESORT: Test Another Pro Micro

If nothing works:

1. **Board may be defective**
   - Contact seller for replacement
   - Clone boards have ~5-10% failure rate

2. **Try on different computer**
   - Rules out PC issues
   - Borrow a laptop to test

3. **Buy from reputable source next time**
   - SparkFun (original Pro Micro)
   - Adafruit
   - Amazon with good reviews
   - Avoid cheapest clones

---

## SUCCESS CHECKLIST

You know it's working when:

- [ ] Pro Micro appears in Device Manager as "Arduino Leonardo"
- [ ] Blink sketch uploads successfully
- [ ] Built-in LED blinks as programmed
- [ ] Keyboard test sketch types in Notepad
- [ ] Pro Micro shows under "Keyboards" in Device Manager
- [ ] Serial communication with Mega confirmed (LED flash)
- [ ] Final keyboard daemon firmware uploaded
- [ ] Keyboard commands work in GUI test mode

---

## WHAT TO TRY NEXT

**If you got the keyboard test working:**
→ Upload the Pro Micro firmware: `arduino-config-promicro.ino`
→ Connect to Mega and test full system

**If keyboard test failed:**
→ Tell me exactly what happens:
   - Does LED blink? (Yes/No)
   - Does text appear in Notepad? (Yes/No)
   - Any error messages?
   - Device Manager shows what?

**If upload fails completely:**
→ Tell me:
   - What error message?
   - Does Pro Micro appear in Device Manager?
   - What board is selected in Arduino IDE?
   - Did you try double-reset?

---

Let's figure this out! What exactly happened when you tried the test?
