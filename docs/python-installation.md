# Python Installation Guide for Windows

Quick guide to install Python and get the Arduino Configurator GUI running.

## Step 1: Download Python

1. **Go to Python's official website:**
   - Visit: https://www.python.org/downloads/
   - Or direct link: https://www.python.org/downloads/windows/

2. **Download the latest version:**
   - Click the big yellow button: **"Download Python 3.12.x"** (or latest version)
   - This downloads the installer (python-3.12.x-amd64.exe)

## Step 2: Install Python

1. **Run the installer** (double-click the downloaded .exe file)

2. **⚠️ IMPORTANT: Check these boxes at the bottom:**
   ```
   ☑ Use admin privileges when installing py.exe
   ☑ Add python.exe to PATH  ← VERY IMPORTANT!
   ```

3. **Click "Install Now"** (recommended for most users)
   - This installs Python to: `C:\Users\YourName\AppData\Local\Programs\Python\Python312\`
   - Includes pip (package installer)
   - Includes IDLE (Python editor)

4. **Wait for installation** (1-2 minutes)

5. **Click "Close"** when finished

## Step 3: Verify Installation

1. **Open Command Prompt:**
   - Press `Win + R`
   - Type: `cmd`
   - Press Enter

2. **Check Python version:**
   ```cmd
   python --version
   ```

   **Expected output:**
   ```
   Python 3.12.x
   ```

3. **Check pip (package installer):**
   ```cmd
   pip --version
   ```

   **Expected output:**
   ```
   pip 24.x.x from C:\Users\...\Python312\lib\site-packages\pip (python 3.12)
   ```

## Step 4: Install GUI Dependencies

1. **Navigate to the GUI folder:**
   ```cmd
   cd D:\arduino\gui
   ```

2. **Install required packages:**
   ```cmd
   pip install -r requirements.txt
   ```

3. **Wait for installation** (1-2 minutes)

   **You should see:**
   ```
   Collecting PyQt5==5.15.10
   Collecting pyserial==3.5
   Downloading PyQt5-5.15.10...
   Installing collected packages: PyQt5, pyserial
   Successfully installed PyQt5-5.15.10 pyserial-3.5
   ```

## Step 5: Run the GUI

```cmd
python main.py
```

**The GUI window should open!** 🎉

## Troubleshooting

### Problem: "python is not recognized as an internal or external command"

**Solution 1: Restart Command Prompt**
- Close and reopen Command Prompt
- Try `python --version` again

**Solution 2: Manually Add Python to PATH**
1. Press `Win + R`, type `sysdm.cpl`, press Enter
2. Click "Advanced" tab → "Environment Variables"
3. Under "User variables", select "Path" → "Edit"
4. Click "New" and add:
   ```
   C:\Users\YourName\AppData\Local\Programs\Python\Python312
   C:\Users\YourName\AppData\Local\Programs\Python\Python312\Scripts
   ```
5. Click OK, close everything, restart Command Prompt

**Solution 3: Reinstall Python**
- Uninstall Python (Settings → Apps → Python)
- Re-download and install
- **DON'T FORGET** to check "Add python.exe to PATH"!

### Problem: "pip is not recognized..."

**Solution:**
```cmd
python -m pip install -r requirements.txt
```

### Problem: "No module named 'PyQt5'"

**Solution:**
```cmd
pip install PyQt5 pyserial
```

### Problem: Permission denied when installing packages

**Solution: Run as Administrator**
1. Press `Win + X`
2. Select "Command Prompt (Admin)" or "Windows PowerShell (Admin)"
3. Navigate to `D:\arduino\gui`
4. Run: `pip install -r requirements.txt`

### Problem: SSL Certificate Error

**Solution:**
```cmd
pip install --trusted-host pypi.org --trusted-host files.pythonhosted.org -r requirements.txt
```

## Alternative: Anaconda (Optional)

If you prefer Anaconda (includes many scientific packages):

1. Download: https://www.anaconda.com/download
2. Install Anaconda
3. Open "Anaconda Prompt"
4. Install dependencies:
   ```cmd
   conda install pyqt
   pip install pyserial
   ```

## Quick Command Reference

**Check Python version:**
```cmd
python --version
```

**Check pip version:**
```cmd
pip --version
```

**Install packages:**
```cmd
pip install package-name
```

**List installed packages:**
```cmd
pip list
```

**Upgrade pip:**
```cmd
python -m pip install --upgrade pip
```

**Run the GUI:**
```cmd
cd D:\arduino\gui
python main.py
```

## What Gets Installed

### Python Standard Library
- Built-in modules (json, serial communication basics, etc.)
- IDLE (simple Python editor)
- pip (package installer)

### Project Dependencies (from requirements.txt)
- **PyQt5** (5.15.10) - GUI framework (~50MB)
- **pyserial** (3.5) - Serial communication with Arduino (~100KB)

**Total download size:** ~50-60MB
**Total installed size:** ~200-250MB

## Testing Your Installation

**Test 1: Python Works**
```cmd
python -c "print('Hello from Python!')"
```
Expected: `Hello from Python!`

**Test 2: PyQt5 Installed**
```cmd
python -c "from PyQt5.QtWidgets import QApplication; print('PyQt5 OK')"
```
Expected: `PyQt5 OK`

**Test 3: pyserial Installed**
```cmd
python -c "import serial; print('pyserial OK')"
```
Expected: `pyserial OK`

**Test 4: Run GUI**
```cmd
cd D:\arduino\gui
python main.py
```
Expected: GUI window opens

## Next Steps

After Python is installed and GUI runs:
1. ✅ Connect Arduino Mega to PC
2. ✅ Upload firmware using Arduino IDE
3. ✅ Run GUI: `python main.py`
4. ✅ Configure your inputs
5. ✅ Start building!

## Need Help?

- Python documentation: https://docs.python.org/3/
- PyQt5 documentation: https://www.riverbankcomputing.com/static/Docs/PyQt5/
- pyserial documentation: https://pyserial.readthedocs.io/

---

**Installation should take 5-10 minutes total.** Good luck! 🐍
