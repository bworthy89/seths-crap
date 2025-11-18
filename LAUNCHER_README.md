# Launcher Scripts - Easy Setup Guide

Simple scripts to automatically check for Python, install dependencies, and run the GUI!

## 🚀 Quick Start (Easiest Method)

### Option 1: Already Have Python Installed

**Just double-click:**
```
launch-gui.bat
```

That's it! The script will:
- ✅ Check if Python is installed
- ✅ Install PyQt5 and pyserial if needed
- ✅ Launch the GUI

### Option 2: Don't Have Python Installed

**Step 1: Install Python**
Double-click:
```
install-python.bat
```

**Step 2: Close and reopen Command Prompt**

**Step 3: Launch GUI**
Double-click:
```
launch-gui.bat
```

Done! 🎉

---

## 📁 Script Files

### `launch-gui.bat` - Main Launcher
**What it does:**
1. Checks if Python is installed
2. Checks if pip is available
3. Installs PyQt5 and pyserial if missing
4. Launches the GUI

**Usage:**
```cmd
launch-gui.bat
```

Or just double-click the file in Windows Explorer.

**When to use:**
- Every time you want to run the GUI
- After installing Python
- After pulling updates from version control

---

### `install-python.bat` - Python Installer
**What it does:**
1. Checks if Python is already installed
2. Tries multiple installation methods:
   - **Method 1:** winget (Windows 10 1809+ / Windows 11)
   - **Method 2:** Chocolatey (if installed)
   - **Method 3:** Direct download and install via PowerShell
3. Falls back to manual installation instructions if all fail

**Usage:**
```cmd
install-python.bat
```

**When to use:**
- First time setup (no Python installed)
- Updating Python to a newer version

**Important:** After running this script, you MUST:
1. Close the Command Prompt window
2. Open a NEW Command Prompt
3. Then run `launch-gui.bat`

This is because PATH changes don't take effect until you restart the terminal.

---

### `install-python.ps1` - PowerShell Installer
**What it does:**
- Downloads Python 3.12.7 directly from python.org
- Installs silently with PATH configuration
- Called automatically by `install-python.bat`

**Manual usage** (if needed):
```powershell
powershell -ExecutionPolicy Bypass -File install-python.ps1
```

**Note:** You normally don't need to run this directly - `install-python.bat` handles it.

---

## 🎯 Detailed Usage Examples

### Scenario 1: First Time Setup (No Python)

1. **Double-click** `install-python.bat`
2. Wait 5-10 minutes for Python to install
3. **Close** the Command Prompt window
4. **Double-click** `launch-gui.bat`
5. GUI opens - start configuring!

### Scenario 2: Already Have Python

1. **Double-click** `launch-gui.bat`
2. Wait 1-2 minutes for dependencies (first time only)
3. GUI opens - start configuring!

### Scenario 3: Python Installed but Not in PATH

If you see: `"python is not recognized..."`

1. **Run** `install-python.bat` to reinstall with PATH
2. Or manually add Python to PATH:
   - Win + R → `sysdm.cpl`
   - Advanced → Environment Variables
   - Edit Path → Add Python directories

### Scenario 4: Permission Issues

If installation fails with permission errors:

1. **Right-click** `launch-gui.bat`
2. Select **"Run as administrator"**
3. Packages will install with admin privileges

---

## 🔧 Troubleshooting

### "python is not recognized as an internal or external command"

**Problem:** Python not in PATH or not installed

**Solutions:**
1. Run `install-python.bat`
2. Restart Command Prompt after installation
3. Check Python is in PATH (see Scenario 3 above)

---

### "Failed to install dependencies"

**Problem:** Network issues or permission denied

**Solutions:**
```cmd
REM Try as administrator
Right-click launch-gui.bat → Run as administrator

REM Or install manually
cd gui
python -m pip install --user -r requirements.txt
```

---

### "winget is not recognized"

**Problem:** winget not available (older Windows 10)

**Solution:** The script will automatically try alternative methods:
- Chocolatey
- Direct PowerShell download
- Manual installation instructions

No action needed - let the script continue!

---

### PowerShell Script Blocked

**Problem:** "Execution Policy" prevents running PowerShell script

**Solution:**
```cmd
REM The batch file already handles this, but if needed:
powershell -ExecutionPolicy Bypass -File install-python.ps1
```

---

### Python Installed But Packages Won't Install

**Problem:** pip is broken or outdated

**Solution:**
```cmd
REM Upgrade pip
python -m pip install --upgrade pip

REM Then try again
cd gui
python -m pip install -r requirements.txt
```

---

### GUI Doesn't Open / Shows Error

**Check dependencies are installed:**
```cmd
python -c "import PyQt5; import serial; print('All packages OK')"
```

**Expected output:** `All packages OK`

**If error:** Run `launch-gui.bat` again to reinstall packages

---

## 📋 What Gets Installed

### Python Installation (via install-python.bat)
- **Python 3.12.7** (~30MB download, ~100MB installed)
- **pip** (package installer)
- **Python Standard Library**
- **PATH configuration** (automatic)

### GUI Dependencies (via launch-gui.bat)
- **PyQt5** 5.15.10 (~50MB)
- **pyserial** 3.5 (~100KB)

**Total:** ~150-180MB

---

## 🛠️ Advanced Usage

### Running from Command Prompt

```cmd
REM Navigate to project folder
cd D:\arduino

REM Install Python (if needed)
install-python.bat

REM Launch GUI
launch-gui.bat
```

### Installing Specific Python Version

Edit `install-python.ps1`:
```powershell
$pythonVersion = "3.11.9"  # Change this line
```

### Installing Additional Packages

Add to `gui/requirements.txt`:
```
PyQt5==5.15.10
pyserial==3.5
your-package==1.0.0
```

Then run `launch-gui.bat` - it will install the new package.

---

## 🔄 Updating

### Update GUI Code
Just replace the files - no need to reinstall anything.

### Update Python
1. Run `install-python.bat`
2. Choose to reinstall when prompted

### Update Dependencies
```cmd
cd gui
python -m pip install --upgrade -r requirements.txt
```

---

## 🚫 Uninstalling

### Uninstall Python
- Settings → Apps → Python 3.12 → Uninstall

### Remove Packages Only
```cmd
pip uninstall PyQt5 pyserial
```

---

## 📝 Script Behavior Details

### launch-gui.bat Flow

```
Start
  │
  ├─ Check Python installed? ─── No ──→ Show error & exit
  │                             Yes
  ├─ Check pip available? ────── No ──→ Try to install pip
  │                             Yes
  ├─ Check PyQt5 installed? ──── No ──→ Install from requirements.txt
  │                             Yes
  ├─ Check pyserial installed? ── No ──→ Install from requirements.txt
  │                             Yes
  └─ Launch GUI (python main.py)
```

### install-python.bat Flow

```
Start
  │
  ├─ Python already installed? ─ Yes ──→ Ask to reinstall
  │                             No
  ├─ Try winget ──────────────── Success ──→ Done
  │                             Fail
  ├─ Try Chocolatey ──────────── Success ──→ Done
  │                             Fail
  ├─ Try PowerShell download ─── Success ──→ Done
  │                             Fail
  └─ Show manual install instructions
```

---

## 💡 Tips

**Tip 1: Create Desktop Shortcut**
- Right-click `launch-gui.bat`
- Send to → Desktop (create shortcut)
- Rename to "Arduino Configurator"

**Tip 2: Pin to Taskbar**
- Create shortcut to `cmd.exe`
- Properties → Target: `cmd.exe /c "D:\arduino\launch-gui.bat"`
- Pin to taskbar

**Tip 3: Run in Background**
- Minimize Command Prompt window
- GUI stays open even if terminal is minimized

**Tip 4: Auto-start on Boot**
- Press Win + R → `shell:startup`
- Copy `launch-gui.bat` shortcut to Startup folder

**Tip 5: Check Installation**
```cmd
python --version
pip list
```

---

## ❓ FAQ

**Q: Do I need to run install-python.bat every time?**
A: No! Only once to install Python. After that, just use `launch-gui.bat`.

**Q: Can I use these scripts with different Python versions?**
A: Yes, the scripts work with any Python 3.7+. Edit `install-python.ps1` to change version.

**Q: What if I already have Python from Anaconda?**
A: `launch-gui.bat` will work fine with Anaconda Python. Just run it.

**Q: Are these scripts safe?**
A: Yes! All scripts are open source, included in this project. Review them yourself:
- `launch-gui.bat` - checks and installs packages
- `install-python.bat` - tries multiple installation methods
- `install-python.ps1` - downloads from official python.org

**Q: Do I need administrator privileges?**
A: Usually no. Scripts install for current user only. If you get permission errors, try "Run as administrator".

**Q: Can I delete these scripts after installation?**
A: You can delete `install-python.bat` and `install-python.ps1` after Python is installed. Keep `launch-gui.bat` for easy GUI launching.

**Q: What if automatic installation fails?**
A: The script will open python.org in your browser with manual installation instructions.

---

## 📚 Related Documentation

- **Python Installation Manual:** `docs/python-installation.md`
- **User Guide:** `docs/user-guide.md`
- **Hardware Setup:** `docs/hardware-wiring.md`
- **Main README:** `README.md`

---

## 🎉 Success Checklist

After running the scripts, verify everything works:

- [ ] `python --version` shows Python 3.x
- [ ] `pip --version` shows pip 20.x+
- [ ] `launch-gui.bat` opens the GUI without errors
- [ ] GUI shows "Ready - Not Connected" in status bar
- [ ] Can click "Refresh Ports" and see COM ports

If all checked, you're ready to configure your Arduino! 🚀

---

**Need help?** Check the troubleshooting section above or refer to `docs/user-guide.md` for detailed usage instructions.
