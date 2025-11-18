# Auto-Update Feature Guide

The Arduino Input Configurator includes automatic update checking and installation from GitHub releases.

## Features

✅ **Automatic Update Check** - Checks for updates on startup (2 seconds after launch)
✅ **Manual Update Check** - Check anytime from Help menu
✅ **One-Click Installation** - Download and install updates automatically
✅ **Release Notes** - View changelog before updating
✅ **Safe Updates** - Preserves user data and configurations
✅ **Auto-Restart** - Restarts application after update

---

## How It Works

### Automatic Check (Startup)

When you launch the GUI:
1. After 2 seconds, checks GitHub for the latest release
2. Compares with your current version (from `VERSION` file)
3. If newer version available, shows notification popup
4. You can install immediately or skip

### Manual Check (Help Menu)

From the GUI:
1. Click **Help → Check for Updates**
2. Shows "Checking..." dialog
3. Displays update information with release notes
4. Click **"Install Update"** to download and install
5. Or click **"Later"** to skip

### Update Installation

When you choose to update:
1. **Downloads** the latest release from GitHub
2. **Extracts** files to temporary directory
3. **Installs** by copying files to project folder
4. **Preserves** user data (.claude, .git, etc.)
5. **Prompts** to restart application
6. **Restarts** automatically if you choose

---

## Using the Auto-Updater

### From GUI

**Check Automatically:**
- Launch GUI normally
- Wait 2 seconds
- Notification appears if update available

**Check Manually:**
```
1. Open GUI (launch-gui.bat)
2. Click: Help → Check for Updates
3. Follow prompts
```

### From Command Line

**Windows:**
```cmd
check-updates.bat
```

**Or directly:**
```cmd
cd gui
python updater.py
```

---

## Version Management

### Current Version

Located in: `VERSION` file in project root

**View current version:**
- GUI: Help → About
- Command line: `type VERSION`
- Python: `python -c "from updater import Updater; print(Updater().current_version)"`

### Version Format

Follows semantic versioning: `MAJOR.MINOR.PATCH`

**Example:** `1.2.3`
- **MAJOR** (1): Breaking changes
- **MINOR** (2): New features (backward compatible)
- **PATCH** (3): Bug fixes

---

## Creating Releases (For Maintainers)

### Step 1: Update VERSION File

```bash
echo 1.1.0 > VERSION
```

### Step 2: Commit Changes

```bash
git add VERSION
git commit -m "Bump version to 1.1.0"
git push
```

### Step 3: Create GitHub Release

1. Go to: https://github.com/bworthy89/seths-crap/releases
2. Click: **"Draft a new release"**
3. **Tag:** `v1.1.0` (prefix with 'v')
4. **Title:** `Arduino Input Configurator v1.1.0`
5. **Description:**
   ```markdown
   ## What's New
   - Added auto-update feature
   - Improved serial port detection
   - Fixed encoder debouncing issues

   ## Installation
   - Download source code (zip)
   - Or update from GUI: Help → Check for Updates

   ## Full Changelog
   https://github.com/bworthy89/seths-crap/compare/v1.0.0...v1.1.0
   ```
6. Click: **"Publish release"**

### Step 4: Test Auto-Update

1. Change `VERSION` file back to old version: `1.0.0`
2. Run GUI
3. Should prompt to update to `1.1.0`
4. Install and verify

---

## What Gets Updated

### Files Included in Update

✅ Arduino firmware (`.ino` files)
✅ Python GUI code (`.py` files)
✅ Documentation (`.md` files)
✅ Launcher scripts (`.bat`, `.ps1`)
✅ Dependencies (`requirements.txt`)
✅ LICENSE and README

### Files Preserved (Not Overwritten)

✅ `.git/` - Your local Git repository
✅ `.claude/` - Claude workspace files
✅ `.vs/` - Visual Studio settings
✅ `.vscode/` - VS Code settings
✅ `.pio/` - PlatformIO build files
✅ `__pycache__/` - Python cache
✅ User configurations and data

---

## Troubleshooting

### Update Check Fails

**Problem:** "Failed to check for updates"

**Solutions:**
1. Check internet connection
2. Verify GitHub is accessible: https://github.com/
3. Check firewall/antivirus blocking Python
4. Wait and try again (rate limiting)

**Manual check:**
```cmd
curl https://api.github.com/repos/bworthy89/seths-crap/releases/latest
```

---

### Update Download Fails

**Problem:** "Failed to download update"

**Solutions:**
1. Check available disk space (need ~50MB free)
2. Check internet connection stability
3. Try manual download from GitHub
4. Disable antivirus temporarily

**Manual download:**
1. Go to: https://github.com/bworthy89/seths-crap/releases
2. Download latest release ZIP
3. Extract to temp folder
4. Copy files to `D:\arduino\`

---

### Update Installation Fails

**Problem:** "Failed to install update"

**Solutions:**
1. Close all running instances of GUI
2. Run `launch-gui.bat` as administrator
3. Check file permissions
4. Manually install from downloaded ZIP

**Manual installation:**
```cmd
1. Close GUI
2. Download release ZIP from GitHub
3. Extract to: D:\arduino\
4. Overwrite when prompted (keep .git folder!)
5. Run: launch-gui.bat
```

---

### Application Won't Restart

**Problem:** Update installed but won't restart

**Solution:**
```cmd
1. Close any error dialogs
2. Navigate to: D:\arduino\
3. Double-click: launch-gui.bat
4. GUI should start with new version
```

**Verify update:**
- Help → About
- Check version number

---

### "Updater Not Available"

**Problem:** Update feature not working

**Solutions:**
1. Check `updater.py` exists in `gui/` folder
2. Reinstall dependencies:
   ```cmd
   cd gui
   pip install -r requirements.txt
   ```
3. Test updater:
   ```cmd
   cd gui
   python updater.py
   ```

---

## Advanced Configuration

### Change Update Repository

Edit `gui/updater.py`:

```python
class Updater:
    def __init__(self, repo_owner="YOUR_USERNAME", repo_name="YOUR_REPO"):
        # ...
```

Or pass when creating:
```python
updater = Updater(repo_owner="myuser", repo_name="myrepo")
```

### Disable Auto-Check on Startup

Edit `gui/arduino_configurator.py`:

```python
# Comment out this line:
# QTimer.singleShot(2000, self.check_for_updates_async)
```

Or set environment variable:
```cmd
set DISABLE_AUTO_UPDATE=1
launch-gui.bat
```

### Change Update Check Timeout

Edit `gui/updater.py`:

```python
def check_for_updates(self, timeout=30):  # Change from 10 to 30
```

---

## Update Safety

### Data Preservation

The updater **preserves**:
- Arduino configurations (EEPROM on hardware)
- Git repository (`.git/`)
- IDE settings (`.vscode/`, `.vs/`)
- User modifications
- Any files in `.gitignore`

### Rollback

If update causes issues:

**Method 1: Git Revert**
```bash
git log  # Find previous commit
git checkout COMMIT_HASH
```

**Method 2: Manual Reinstall**
1. Download previous release from GitHub
2. Extract and overwrite
3. Run `launch-gui.bat`

**Method 3: Restore from Backup**
If you backed up before update:
```cmd
xcopy /E /Y D:\arduino_backup\* D:\arduino\
```

---

## Best Practices

### Before Updating

1. **Close Arduino IDE** - Prevents file conflicts
2. **Disconnect Arduino** - Save any unsaved configurations
3. **Backup (Optional)** - Copy project folder
   ```cmd
   xcopy /E /I D:\arduino D:\arduino_backup
   ```

### After Updating

1. **Verify Version** - Help → About
2. **Test GUI** - Connect to Arduino, test features
3. **Re-upload Firmware** - If Arduino code updated
4. **Check Documentation** - Read release notes

### Regular Maintenance

- **Check monthly** for updates
- **Read release notes** before installing
- **Report issues** on GitHub if update fails

---

## Update Notification Examples

### Startup Notification

```
┌────────────────────────────────────┐
│     Update Available               │
├────────────────────────────────────┤
│ A new version (1.2.0) is available│
│                                    │
│ Current version: 1.1.0             │
│                                    │
│ Do you want to download and        │
│ install it now?                    │
│                                    │
│         [Yes]    [No]              │
└────────────────────────────────────┘
```

### Manual Check - Up to Date

```
┌────────────────────────────────────┐
│          No Updates                │
├────────────────────────────────────┤
│ You have the latest version        │
│ (1.2.0).                          │
│                                    │
│            [OK]                    │
└────────────────────────────────────┘
```

### Update Progress

```
┌────────────────────────────────────┐
│  Downloading update...             │
├────────────────────────────────────┤
│  ████████████░░░░░░░  60%         │
│                                    │
│  Extracting files...               │
│                                    │
│           [Cancel]                 │
└────────────────────────────────────┘
```

---

## FAQ

**Q: How often does it check for updates?**
A: Only on application startup (after 2 second delay). Never in background.

**Q: Does it update automatically without asking?**
A: No, always asks permission before downloading/installing.

**Q: Will it overwrite my Arduino configurations?**
A: No, configurations are stored on Arduino EEPROM, not affected by updates.

**Q: Can I update without internet?**
A: No, requires internet to check GitHub. Manual installation possible.

**Q: Does it work on Linux/macOS?**
A: Yes! The updater works cross-platform.

**Q: What if I'm using a fork?**
A: Change `repo_owner` and `repo_name` in `updater.py`.

**Q: Can I skip a version?**
A: Yes, updates always to latest, regardless of how many versions behind.

**Q: Will it notify me of pre-releases?**
A: No, only stable releases (not marked as pre-release on GitHub).

---

## For Developers

### Testing the Updater

**Test update check:**
```python
from updater import Updater
updater = Updater()
info = updater.check_for_updates()
print(info)
```

**Test version comparison:**
```python
updater = Updater(current_version="1.0.0")
result = updater._compare_versions("1.1.0", "1.0.0")
print(result)  # Should be 1 (newer)
```

**Test download (dry run):**
```python
updater = Updater()
info = updater.check_for_updates()
if info and info['available']:
    print(f"Would download: {info['download_url']}")
```

### CI/CD Integration

Automate releases with GitHub Actions:

```yaml
name: Release
on:
  push:
    tags:
      - 'v*'

jobs:
  release:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v2
      - uses: actions/create-release@v1
        env:
          GITHUB_TOKEN: ${{ secrets.GITHUB_TOKEN }}
        with:
          tag_name: ${{ github.ref }}
          release_name: Release ${{ github.ref }}
          draft: false
          prerelease: false
```

---

## Security Considerations

### HTTPS Only

All downloads use HTTPS from GitHub. Man-in-the-middle attacks prevented.

### Signature Verification

Currently not implemented. Future enhancement:
- Sign releases with GPG
- Verify signature before installing

### Sandboxing

Updater runs with same permissions as GUI. If concerned:
- Review code before updating
- Check release notes and changelog
- Test in virtual machine first

---

## Changelog

### Version 1.0.0 (Current)
- Initial release with auto-update feature
- GitHub API integration
- Progress dialogs
- Safe file copying
- Auto-restart capability

---

**Need help?** Check [GITHUB_SETUP.md](../GITHUB_SETUP.md) for GitHub configuration or [user-guide.md](user-guide.md) for general usage.
