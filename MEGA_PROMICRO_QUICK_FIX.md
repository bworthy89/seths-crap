# Quick Fix: Mega + Pro Micro Setup

Your hardware setup is **perfect for Xbox/consoles** - we just need to fix the upload issue.

## Problem Diagnosis

✅ Mega LED blinks = Mega working correctly
❌ No keyboard output = Pro Micro not uploaded correctly

## Quick Fix (5 minutes)

### Step 1: Verify Pro Micro Connection

1. **Unplug Pro Micro from Mega** (remove TX/RX/GND wires temporarily)
2. **Connect Pro Micro directly to PC** via Micro USB
3. **Open Device Manager** (Win + X → Device Manager)
4. **Look for:** "Arduino Leonardo" under "Ports (COM & LPT)"
   - Note the COM port (e.g., COM4)

**If you DON'T see it:**
- Try different Micro USB cable (many are power-only!)
- Try different USB port
- Press reset button twice quickly (enters bootloader mode)

### Step 2: Upload Test Sketch

1. **Arduino IDE → New Sketch**
2. **Paste this:**

```cpp
#include <Keyboard.h>

void setup() {
  Keyboard.begin();
  delay(2000);  // Wait 2 seconds

  // Type "TEST" every 3 seconds
  Keyboard.println("Hello from Pro Micro!");
}

void loop() {
  delay(3000);
  Keyboard.println("Still working!");
}
```

3. **CRITICAL: Tools → Board → Arduino Leonardo** (NOT Mega!)
4. **Tools → Port → Select Pro Micro port**
5. **Upload** (Ctrl+U)

### Step 3: Test in Notepad

1. **Open Notepad**
2. **Press reset button on Pro Micro**
3. **After 2 seconds:** Should type "Hello from Pro Micro!"
4. **Every 3 seconds:** Should type "Still working!"

**If this works:** ✅ Pro Micro is fine, just needs correct firmware
**If this doesn't work:** ❌ Hardware issue (cable/board/drivers)

### Step 4: Upload Correct Firmware

1. **Open:** `arduino-config-promicro/arduino-config-promicro.ino`
2. **Tools → Board → Arduino Leonardo** ⚠️
3. **Tools → Port → Pro Micro port**
4. **Upload**
5. **Wait for:** "Done uploading"

### Step 5: Reconnect to Mega

1. **Reconnect wiring:**
   ```
   Mega Pin 18 (TX1) → Pro Micro RXI (Pin 0)
   Mega Pin 19 (RX1) → Pro Micro TXO (Pin 1)
   Mega GND → Pro Micro GND
   ```

2. **Plug BOTH into PC:**
   - Mega: USB-B cable
   - Pro Micro: Micro USB cable

3. **Run GUI, configure input, test!**

---

## For Xbox/Console Use

Once working on PC:

1. **Configure your inputs** using GUI on PC
2. **Disconnect Pro Micro from PC**
3. **Disconnect Mega from PC**
4. **Power the system:**
   - Option A: Plug Mega into Xbox USB port (powers both boards)
   - Option B: Use external 5V power for Mega
5. **Plug Pro Micro into Xbox USB port**

**Xbox will see Pro Micro as a USB keyboard!**

---

## Why This Works for Xbox

**Mega:**
- Reads buttons/encoders
- Sends commands to Pro Micro via TX/RX pins
- Can be powered by Xbox or external power

**Pro Micro:**
- Receives commands from Mega
- Sends **real USB HID keyboard** commands
- Xbox recognizes it as keyboard
- No PC needed!

---

## Common Mistakes

### ❌ Wrong Board Selected
**Problem:** Uploaded to Pro Micro with "Arduino Mega" selected
**Fix:** MUST select "Arduino Leonardo" for Pro Micro

### ❌ Power-Only USB Cable
**Problem:** Pro Micro lights up but not recognized
**Fix:** Use data cable (try different cable)

### ❌ No Code on Pro Micro
**Problem:** Forgot to upload firmware to Pro Micro
**Fix:** Upload `arduino-config-promicro.ino` to Pro Micro

### ❌ TX/RX Swapped
**Problem:** Commands not reaching Pro Micro
**Fix:** TX goes to RX, RX goes to TX (crossover!)

---

## Alternative: All-in-One Board

If you can't get Pro Micro working, consider:

**Arduino Leonardo** (~$10-15)
- Drop-in replacement for Mega
- Native USB HID (no Pro Micro needed!)
- 20 digital pins (vs Mega's 54)
- Good for 10-15 inputs

**Teensy 3.6** (~$30)
- 58 digital pins
- Native USB HID
- Faster processor
- More expensive but best option

**Teensy 4.1** (~$30)
- 55 digital pins
- Native USB HID
- Very fast
- Works perfectly with Xbox

---

## Testing Checklist

Before declaring it broken:

- [ ] Pro Micro shows in Device Manager as "Arduino Leonardo"
- [ ] Test sketch types in Notepad
- [ ] Uploaded with "Arduino Leonardo" board selected
- [ ] Using data-capable USB cable
- [ ] Both boards connected to PC during testing
- [ ] TX/RX wires are crossover (not straight)
- [ ] GND connected between boards

---

## Still Not Working?

**If Pro Micro test sketch works but system doesn't:**
- Problem is wiring or Mega firmware
- Check TX/RX connections
- Verify Mega sending commands (check GUI console)

**If Pro Micro test sketch doesn't work:**
- Hardware issue with Pro Micro
- Try different Pro Micro board
- Consider Arduino Leonardo instead

**If nothing works:**
- Post photos of wiring
- Share Device Manager screenshot
- Check GitHub issues for help

---

## Bottom Line

**Your dual-board approach is CORRECT for Xbox!**

The Pro Micro IS a true USB HID device that Xbox will recognize.
You just need to get it uploading and communicating properly.

Follow the test sketch in Step 2 - if that works, your Pro Micro is fine.
