@echo off
REM Arduino Input Configurator - Python Installer
REM Automatically downloads and installs Python

echo ========================================
echo Python Auto-Installer
echo ========================================
echo.
echo This script will install Python 3.12.x
echo.

REM Check if Python is already installed
python --version >nul 2>&1
if %errorlevel% equ 0 (
    for /f "tokens=*" %%i in ('python --version') do set PYTHON_VERSION=%%i
    echo [INFO] Python is already installed: %PYTHON_VERSION%
    echo.
    choice /C YN /M "Do you want to reinstall/update Python?"
    if errorlevel 2 goto :check_packages
)

echo.
echo Attempting to install Python...
echo.

REM Try Method 1: winget (Windows 10 1809+ / Windows 11)
echo [Method 1] Trying winget (Windows Package Manager)...
winget --version >nul 2>&1
if %errorlevel% equ 0 (
    echo [OK] winget is available
    echo.
    echo Installing Python 3.12 via winget...
    echo This may take 5-10 minutes...
    echo.
    winget install Python.Python.3.12 --silent --accept-package-agreements --accept-source-agreements

    if %errorlevel% equ 0 (
        echo.
        echo [SUCCESS] Python installed via winget!
        echo.
        echo IMPORTANT: Please close and reopen this Command Prompt window
        echo            then run launch-gui.bat
        echo.
        pause
        exit /b 0
    ) else (
        echo [WARN] winget installation failed, trying alternative methods...
        echo.
    )
) else (
    echo [WARN] winget not available on this system
    echo.
)

REM Try Method 2: Chocolatey
echo [Method 2] Trying Chocolatey package manager...
choco --version >nul 2>&1
if %errorlevel% equ 0 (
    echo [OK] Chocolatey is available
    echo.
    echo Installing Python via Chocolatey...
    choco install python -y

    if %errorlevel% equ 0 (
        echo.
        echo [SUCCESS] Python installed via Chocolatey!
        echo.
        echo IMPORTANT: Please close and reopen this Command Prompt window
        echo            then run launch-gui.bat
        echo.
        pause
        exit /b 0
    ) else (
        echo [WARN] Chocolatey installation failed
        echo.
    )
) else (
    echo [WARN] Chocolatey not available
    echo.
)

REM Method 3: PowerShell download and install
echo [Method 3] Downloading Python installer...
echo.
echo Running PowerShell script to download and install Python...
echo This may take 5-10 minutes...
echo.

powershell -ExecutionPolicy Bypass -File "%~dp0install-python.ps1"

if %errorlevel% equ 0 (
    echo.
    echo [SUCCESS] Python installed successfully!
    echo.
    echo IMPORTANT: Please close and reopen this Command Prompt window
    echo            then run launch-gui.bat
    echo.
    pause
    exit /b 0
) else (
    echo [ERROR] Automatic installation failed
    goto :manual_install
)

:manual_install
echo.
echo ========================================
echo Manual Installation Required
echo ========================================
echo.
echo Automatic installation methods failed.
echo Please install Python manually:
echo.
echo 1. Open: https://www.python.org/downloads/
echo 2. Click "Download Python 3.12.x"
echo 3. Run the installer
echo 4. CHECK THE BOX: "Add python.exe to PATH"
echo 5. Click "Install Now"
echo 6. After installation, run: launch-gui.bat
echo.
echo Opening Python download page in browser...
start https://www.python.org/downloads/
echo.
pause
exit /b 1

:check_packages
echo.
echo Checking for required packages...
cd /d "%~dp0gui"

python -c "import PyQt5" >nul 2>&1
if %errorlevel% neq 0 (
    echo [INSTALL] Installing PyQt5 and pyserial...
    python -m pip install -r requirements.txt
    if %errorlevel% equ 0 (
        echo [OK] Packages installed successfully!
    ) else (
        echo [ERROR] Failed to install packages
    )
)

echo.
echo All set! Run launch-gui.bat to start the application.
echo.
pause
exit /b 0
