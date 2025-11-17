# Arduino Input Configurator - Python Installer (PowerShell)
# Downloads and installs Python automatically

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "Python Auto-Installer (PowerShell)" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# Configuration
$pythonVersion = "3.12.7"  # Update this to latest stable version
$pythonUrl = "https://www.python.org/ftp/python/$pythonVersion/python-$pythonVersion-amd64.exe"
$installerPath = "$env:TEMP\python-installer.exe"

Write-Host "[1/3] Downloading Python $pythonVersion..." -ForegroundColor Yellow
Write-Host "      From: $pythonUrl" -ForegroundColor Gray
Write-Host ""

try {
    # Download Python installer
    $ProgressPreference = 'SilentlyContinue'  # Hide progress bar for speed
    Invoke-WebRequest -Uri $pythonUrl -OutFile $installerPath -UseBasicParsing
    Write-Host "[OK] Download complete!" -ForegroundColor Green
    Write-Host ""
} catch {
    Write-Host "[ERROR] Failed to download Python installer" -ForegroundColor Red
    Write-Host "Error: $_" -ForegroundColor Red
    Write-Host ""
    Write-Host "Please install Python manually from: https://www.python.org/downloads/" -ForegroundColor Yellow
    exit 1
}

Write-Host "[2/3] Installing Python..." -ForegroundColor Yellow
Write-Host "      This may take 5-10 minutes..." -ForegroundColor Gray
Write-Host ""

try {
    # Install Python silently
    # /quiet = silent install
    # InstallAllUsers=0 = install for current user only
    # PrependPath=1 = add Python to PATH (IMPORTANT!)
    # Include_test=0 = don't install test suite
    $installArgs = "/quiet InstallAllUsers=0 PrependPath=1 Include_test=0"

    $process = Start-Process -FilePath $installerPath -ArgumentList $installArgs -Wait -PassThru

    if ($process.ExitCode -eq 0) {
        Write-Host "[OK] Python installed successfully!" -ForegroundColor Green
        Write-Host ""
    } else {
        Write-Host "[ERROR] Installation failed with exit code: $($process.ExitCode)" -ForegroundColor Red
        Write-Host ""
        exit 1
    }
} catch {
    Write-Host "[ERROR] Failed to install Python" -ForegroundColor Red
    Write-Host "Error: $_" -ForegroundColor Red
    exit 1
}

Write-Host "[3/3] Cleaning up..." -ForegroundColor Yellow
Remove-Item $installerPath -ErrorAction SilentlyContinue

Write-Host ""
Write-Host "========================================" -ForegroundColor Green
Write-Host "Installation Complete!" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Green
Write-Host ""
Write-Host "IMPORTANT: " -ForegroundColor Yellow -NoNewline
Write-Host "Close this window and open a NEW Command Prompt"
Write-Host "           Then run: launch-gui.bat"
Write-Host ""

exit 0
