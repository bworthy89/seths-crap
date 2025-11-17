@echo off
REM Arduino Input Configurator - GUI Launcher
REM Automatically checks Python, installs dependencies, and launches GUI

echo ========================================
echo Arduino Input Configurator
echo ========================================
echo.

REM Check if Python is installed
echo [1/4] Checking for Python installation...
python --version >nul 2>&1
if %errorlevel% neq 0 (
    echo.
    echo [ERROR] Python is not installed or not in PATH!
    echo.
    echo Please install Python by running:
    echo    install-python.bat
    echo.
    echo Or manually download from: https://www.python.org/downloads/
    echo IMPORTANT: Check "Add python.exe to PATH" during installation!
    echo.
    pause
    exit /b 1
)

REM Show Python version
for /f "tokens=*" %%i in ('python --version') do set PYTHON_VERSION=%%i
echo [OK] Found %PYTHON_VERSION%
echo.

REM Check if pip is available
echo [2/4] Checking for pip (package installer)...
python -m pip --version >nul 2>&1
if %errorlevel% neq 0 (
    echo [ERROR] pip is not installed!
    echo.
    echo Attempting to install pip...
    python -m ensurepip --upgrade
    if %errorlevel% neq 0 (
        echo [ERROR] Failed to install pip. Please reinstall Python.
        pause
        exit /b 1
    )
)
echo [OK] pip is available
echo.

REM Check and install dependencies
echo [3/4] Checking required packages (PyQt5, pyserial)...
cd /d "%~dp0gui"

python -c "import PyQt5" >nul 2>&1
set PYQT5_INSTALLED=%errorlevel%

python -c "import serial" >nul 2>&1
set PYSERIAL_INSTALLED=%errorlevel%

if %PYQT5_INSTALLED% neq 0 (
    echo [INSTALL] PyQt5 not found - installing...
) else (
    echo [OK] PyQt5 already installed
)

if %PYSERIAL_INSTALLED% neq 0 (
    echo [INSTALL] pyserial not found - installing...
) else (
    echo [OK] pyserial already installed
)

if %PYQT5_INSTALLED% neq 0 (
    echo.
    echo Installing dependencies from requirements.txt...
    echo This may take 2-3 minutes...
    echo.
    python -m pip install -r requirements.txt
    if %errorlevel% neq 0 (
        echo.
        echo [ERROR] Failed to install dependencies!
        echo.
        echo Try running as Administrator:
        echo    Right-click launch-gui.bat ^> Run as administrator
        echo.
        pause
        exit /b 1
    )
    echo.
    echo [OK] Dependencies installed successfully!
)
echo.

REM Launch the GUI
echo [4/4] Launching Arduino Configurator GUI...
echo.
echo ========================================
echo GUI is starting...
echo Close this window to stop the program
echo ========================================
echo.

python main.py

if %errorlevel% neq 0 (
    echo.
    echo [ERROR] GUI exited with error code %errorlevel%
    echo.
    pause
)

exit /b %errorlevel%
