# How to Create a Release (For Testing Auto-Update)

Quick guide to create your first release so you can test the auto-update feature.

## Step 1: Update Version (Optional for Testing)

To test the updater, temporarily change VERSION to an older version:

```bash
echo 0.9.0 > VERSION
git commit -am "Temporary: Set version to 0.9.0 for testing"
git push
```

## Step 2: Create Release on GitHub

### Via Web Interface

1. **Go to releases page:**
   https://github.com/bworthy89/seths-crap/releases

2. **Click:** "Draft a new release"

3. **Fill in the form:**

   **Tag version:** `v1.0.0`
   - Must start with 'v'
   - Use semantic versioning (MAJOR.MINOR.PATCH)

   **Release title:** `Arduino Input Configurator v1.0.0`

   **Description:**
   ```markdown
   ## 🎉 First Release!

   ### Features
   - ✅ Arduino Mega 2560 + Pro Micro dual-board architecture
   - ✅ Up to 40 inputs (buttons, encoders, switches, potentiometers)
   - ✅ Python PyQt5 GUI configurator
   - ✅ Auto-update feature with GitHub integration
   - ✅ Windows auto-installer scripts
   - ✅ EEPROM configuration storage
   - ✅ Ctrl+ keyboard command mapping
   - ✅ Rotary encoder modes (1x/10x/100x/1000x)

   ### Installation

   **Easy Way (Windows):**
   1. Download the source code (zip) below
   2. Extract to `D:\arduino\`
   3. Double-click `launch-gui.bat`

   **Or update from GUI:**
   - Help → Check for Updates

   ### Documentation
   - [User Guide](https://github.com/bworthy89/seths-crap/blob/main/docs/user-guide.md)
   - [Hardware Wiring](https://github.com/bworthy89/seths-crap/blob/main/docs/hardware-wiring.md)
   - [Auto-Update Guide](https://github.com/bworthy89/seths-crap/blob/main/docs/auto-update-guide.md)

   ### Requirements
   - Python 3.7+ (auto-installs with `install-python.bat`)
   - Arduino Mega 2560 (ELEGOO MEGA R3)
   - Arduino Pro Micro (ATmega32u4)

   ### Full Changelog
   Initial release - see README.md for complete feature list.
   ```

4. **Click:** "Publish release"

## Step 3: Test the Auto-Updater

### Test Automatic Check (Startup)

1. **Ensure VERSION file shows old version:**
   ```bash
   type VERSION
   # Should show: 0.9.0
   ```

2. **Launch GUI:**
   ```bash
   launch-gui.bat
   ```

3. **Wait 2 seconds**

4. **Should see notification:**
   ```
   Update Available
   A new version (1.0.0) is available!
   Current version: 0.9.0
   Do you want to download and install it now?
   [Yes] [No]
   ```

5. **Click "Yes"** to test installation

### Test Manual Check (Help Menu)

1. **Launch GUI**

2. **Click:** Help → Check for Updates

3. **Should show update dialog** with release notes

4. **Click:** "Install Update"

5. **Watch progress:**
   - Downloading update... [50%]
   - Extracting files... [75%]
   - Installing update... [90%]
   - Update complete! [100%]

6. **Restart when prompted**

7. **Verify:**
   - Help → About
   - Version should now be 1.0.0

### Test Command-Line Updater

```bash
check-updates.bat
```

Or:
```bash
cd gui
python updater.py
```

Should show:
```
Checking for updates...
Update available!
Current version: 0.9.0
Latest version:  1.0.0

Release notes:
[Release notes here]

Do you want to install this update? (yes/no):
```

## Step 4: Restore Current Version

After testing, update VERSION to current:

```bash
echo 1.0.0 > VERSION
git commit -am "Restore version to 1.0.0"
git push
```

## Future Releases

### Version Numbering

Follow semantic versioning:

**MAJOR.MINOR.PATCH**

- **MAJOR**: Breaking changes, incompatible updates
  - Example: 2.0.0 - Complete rewrite

- **MINOR**: New features, backward compatible
  - Example: 1.1.0 - Added mouse support

- **PATCH**: Bug fixes, small improvements
  - Example: 1.0.1 - Fixed serial port bug

### Release Checklist

- [ ] Update VERSION file
- [ ] Update CHANGELOG (if you have one)
- [ ] Test all features
- [ ] Commit and push changes
- [ ] Create GitHub release with tag
- [ ] Test auto-updater
- [ ] Announce on forums/social media

### Example Release Workflow

```bash
# 1. Make your changes
git add .
git commit -m "Add new feature"

# 2. Update version
echo 1.1.0 > VERSION
git add VERSION
git commit -m "Bump version to 1.1.0"

# 3. Push to GitHub
git push

# 4. Create release on GitHub
# - Tag: v1.1.0
# - Title: Arduino Input Configurator v1.1.0
# - Description: What's new, installation, changelog

# 5. Test updater with previous version
```

## Troubleshooting

### "No releases found"

**Problem:** API returns 404

**Solution:**
- Create at least one release on GitHub
- Make sure release is published (not draft)
- Check repository is public or you have access

### Update downloads but doesn't install

**Problem:** Files extracted but not copied

**Solution:**
- Close all instances of GUI
- Run as administrator
- Check file permissions
- Manually copy from downloaded ZIP

### Version doesn't change after update

**Problem:** VERSION file not updated

**Solution:**
- Update was interrupted
- VERSION file in wrong location
- Re-run updater
- Manually update VERSION file

## Advanced: Automated Releases

### Using GitHub CLI

```bash
# Install GitHub CLI: https://cli.github.com/

# Create release
gh release create v1.0.0 \
  --title "Arduino Input Configurator v1.0.0" \
  --notes "Release notes here" \
  --target main
```

### Using GitHub Actions

Create `.github/workflows/release.yml`:

```yaml
name: Create Release

on:
  push:
    tags:
      - 'v*'

jobs:
  release:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v3

      - name: Create Release
        uses: actions/create-release@v1
        env:
          GITHUB_TOKEN: ${{ secrets.GITHUB_TOKEN }}
        with:
          tag_name: ${{ github.ref }}
          release_name: Release ${{ github.ref }}
          body: |
            See CHANGELOG.md for details
          draft: false
          prerelease: false
```

Then create release with:
```bash
git tag v1.0.0
git push origin v1.0.0
```

## Testing Checklist

When testing auto-update:

- [ ] GUI launches without errors
- [ ] Update check shows correct versions
- [ ] Progress dialog displays during download
- [ ] Files extracted successfully
- [ ] Update installs to correct location
- [ ] User data preserved (.git, .claude, etc.)
- [ ] Application restarts after update
- [ ] New version shows in Help → About
- [ ] All features still work after update
- [ ] Arduino firmware unchanged (unless intended)

## Need Help?

See these documents:
- [Auto-Update Guide](docs/auto-update-guide.md) - Complete updater documentation
- [GitHub Setup](GITHUB_SETUP.md) - GitHub repository configuration
- [Contributing](CONTRIBUTING.md) - Development guidelines

---

**Ready to create your first release?** Follow Step 2 above! 🚀
