@echo off
REM Quick Git Setup for Arduino Input Configurator

echo ========================================
echo Git Repository Setup
echo ========================================
echo.

REM Check if git is installed
git --version >nul 2>&1
if %errorlevel% neq 0 (
    echo [ERROR] Git is not installed!
    echo.
    echo Please install Git from: https://git-scm.com/downloads
    echo Then run this script again.
    echo.
    pause
    exit /b 1
)

echo [OK] Git is installed
echo.

REM Check if already initialized
if exist ".git" (
    echo [INFO] Git repository already initialized
    echo.
    choice /C YN /M "Do you want to reinitialize?"
    if errorlevel 2 goto :skip_init
    echo.
    echo Removing existing .git folder...
    rmdir /s /q .git
)

:init_repo
echo [1/5] Initializing Git repository...
git init
if %errorlevel% neq 0 (
    echo [ERROR] Failed to initialize git repository
    pause
    exit /b 1
)
echo [OK] Repository initialized
echo.

:skip_init
echo [2/5] Checking Git configuration...
git config user.name >nul 2>&1
if %errorlevel% neq 0 (
    echo.
    echo Git user not configured. Please enter your details:
    echo.
    set /p GIT_NAME="Your Name: "
    set /p GIT_EMAIL="Your Email: "
    git config --global user.name "%GIT_NAME%"
    git config --global user.email "%GIT_EMAIL%"
    echo [OK] Git configured
) else (
    for /f "delims=" %%i in ('git config user.name') do set GIT_NAME=%%i
    for /f "delims=" %%i in ('git config user.email') do set GIT_EMAIL=%%i
    echo [OK] Git user: %GIT_NAME% ^<%GIT_EMAIL%^>
)
echo.

echo [3/5] Adding all files...
git add .
if %errorlevel% neq 0 (
    echo [ERROR] Failed to add files
    pause
    exit /b 1
)
echo [OK] Files staged
echo.

echo [4/5] Creating initial commit...
git commit -m "Initial commit - Arduino Input Configurator

- Arduino Mega 2560 firmware for input handling
- Arduino Pro Micro firmware for USB HID keyboard
- Python PyQt5 GUI application
- Auto-installer scripts for Windows
- Complete documentation"

if %errorlevel% neq 0 (
    echo [ERROR] Failed to create commit
    pause
    exit /b 1
)
echo [OK] Initial commit created
echo.

echo [5/5] Repository setup complete!
echo.
echo ========================================
echo Next Steps:
echo ========================================
echo.
echo 1. Create a repository on GitHub:
echo    https://github.com/new
echo.
echo 2. Repository name: arduino-input-configurator
echo    Description: GUI for configuring Arduino Mega inputs to send keyboard commands
echo    Public or Private: Your choice
echo    DO NOT initialize with README, .gitignore, or license
echo.
echo 3. After creating the repository, run these commands:
echo.
echo    git remote add origin https://github.com/YOUR_USERNAME/arduino-input-configurator.git
echo    git branch -M main
echo    git push -u origin main
echo.
echo OR use GitHub Desktop for easier management!
echo.
echo See GITHUB_SETUP.md for detailed instructions.
echo.
pause
exit /b 0
