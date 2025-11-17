@echo off
REM Arduino Input Configurator - Update Checker
REM Check for updates from GitHub releases

echo ========================================
echo Arduino Input Configurator
echo Update Checker
echo ========================================
echo.

REM Check if Python is installed
python --version >nul 2>&1
if %errorlevel% neq 0 (
    echo [ERROR] Python is not installed!
    echo.
    echo Please run: install-python.bat
    echo.
    pause
    exit /b 1
)

REM Run the updater CLI
cd /d "%~dp0gui"
python updater.py

if %errorlevel% neq 0 (
    echo.
    echo [ERROR] Update check failed
    echo.
)

pause
exit /b %errorlevel%
