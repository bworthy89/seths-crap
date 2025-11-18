# GitHub Setup Guide

Complete guide to uploading the Arduino Input Configurator project to GitHub.

## Prerequisites

1. **Git Installed**
   - Download from: https://git-scm.com/downloads
   - Install with default options
   - Verify: `git --version` in Command Prompt

2. **GitHub Account**
   - Sign up at: https://github.com/join
   - Free account is sufficient

---

## Method 1: Using GitHub Desktop (Easiest)

### Step 1: Install GitHub Desktop

1. Download: https://desktop.github.com/
2. Install and sign in with your GitHub account

### Step 2: Create Repository

1. **Open GitHub Desktop**
2. **File → Add Local Repository**
3. **Choose:** `D:\arduino`
4. Click: **"Create a repository"** (if not initialized)
5. Fill in:
   - **Name:** arduino-input-configurator
   - **Description:** GUI for configuring Arduino Mega inputs to send keyboard commands
   - **Keep Local:** Checked
   - **Git Ignore:** None (we already have .gitignore)
   - **License:** MIT (we already have LICENSE)
6. Click: **"Create Repository"**

### Step 3: Commit Initial Files

1. GitHub Desktop shows all files to commit
2. **Summary:** "Initial commit - Arduino Input Configurator"
3. **Description:** (optional)
   ```
   - Arduino Mega 2560 firmware for input handling
   - Arduino Pro Micro firmware for USB HID keyboard
   - Python PyQt5 GUI application
   - Auto-installer scripts for Windows
   - Complete documentation
   ```
4. Click: **"Commit to main"**

### Step 4: Publish to GitHub

1. Click: **"Publish repository"** (top right)
2. **Name:** arduino-input-configurator
3. **Description:** GUI configurator for Arduino Mega inputs → keyboard commands
4. **Keep private:** Uncheck (make it public) or leave checked
5. Click: **"Publish Repository"**

Done! Your project is now on GitHub! 🎉

**View online:** https://github.com/YOUR_USERNAME/arduino-input-configurator

---

## Method 2: Using Command Line (Git Bash)

### Step 1: Install Git

Download and install from: https://git-scm.com/downloads

### Step 2: Configure Git (First Time Only)

```bash
git config --global user.name "Your Name"
git config --global user.email "your.email@example.com"
```

### Step 3: Initialize Repository

Open Git Bash in `D:\arduino`:

```bash
cd /d/arduino
git init
```

### Step 4: Add All Files

```bash
git add .
```

This stages all files except those in `.gitignore`.

### Step 5: Create Initial Commit

```bash
git commit -m "Initial commit - Arduino Input Configurator

- Arduino Mega 2560 firmware for input handling
- Arduino Pro Micro firmware for USB HID keyboard
- Python PyQt5 GUI application
- Auto-installer scripts for Windows
- Complete documentation"
```

### Step 6: Create GitHub Repository

1. **Go to:** https://github.com/new
2. **Repository name:** arduino-input-configurator
3. **Description:** GUI configurator for Arduino Mega inputs to send keyboard commands
4. **Public** or **Private:** Your choice
5. **DO NOT** initialize with README, .gitignore, or license (we have them)
6. Click: **"Create repository"**

### Step 7: Link Local to GitHub

GitHub shows you commands. Copy and run:

```bash
git remote add origin https://github.com/YOUR_USERNAME/arduino-input-configurator.git
git branch -M main
git push -u origin main
```

**Enter GitHub credentials when prompted.**

Done! Your project is now on GitHub! 🎉

---

## Method 3: Using GitHub Web Interface

### Step 1: Create Repository on GitHub

1. Go to: https://github.com/new
2. **Name:** arduino-input-configurator
3. **Description:** GUI configurator for Arduino Mega inputs to send keyboard commands
4. **Public** or **Private**
5. **DO NOT** check any initialization options
6. Click: **"Create repository"**

### Step 2: Install Git

If not installed: https://git-scm.com/downloads

### Step 3: Follow GitHub's Instructions

After creating the repo, GitHub shows quick setup commands:

```bash
cd D:\arduino
git init
git add .
git commit -m "Initial commit"
git branch -M main
git remote add origin https://github.com/YOUR_USERNAME/arduino-input-configurator.git
git push -u origin main
```

---

## Verify Your Upload

1. Go to: `https://github.com/YOUR_USERNAME/arduino-input-configurator`
2. You should see:
   - ✅ README.md (displayed on main page)
   - ✅ All project folders (arduino-config-mega, gui, docs)
   - ✅ Launcher scripts (launch-gui.bat, install-python.bat)
   - ✅ Documentation files

---

## Recommended Repository Settings

### Description

```
🎮 GUI configurator for Arduino Mega 2560 + Pro Micro to create custom USB HID keyboard controllers with up to 40 inputs (buttons, encoders, switches, pots)
```

### Topics (Tags)

Add these topics for discoverability:
- `arduino`
- `arduino-mega`
- `pro-micro`
- `keyboard-emulation`
- `hid`
- `pyqt5`
- `python`
- `gui`
- `configurator`
- `input-device`
- `usb-hid`
- `rotary-encoder`

### Website

If you create a project page, add the URL here.

### About Section

Enable:
- ✅ Releases
- ✅ Packages
- ✅ Issues (for bug reports)
- ✅ Wiki (optional)

---

## Creating a Great README on GitHub

Your README.md already has everything, but you might want to add:

### Badges

Add at the top of README.md:

```markdown
![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)
![Python](https://img.shields.io/badge/python-3.7+-blue.svg)
![Arduino](https://img.shields.io/badge/arduino-mega2560-blue.svg)
![Platform](https://img.shields.io/badge/platform-windows-lightgrey.svg)
```

### Screenshots

Add screenshots to a `screenshots/` folder:
- GUI main window
- Configuration table
- Hardware setup photo
- Example control panel

Then embed in README:
```markdown
## Screenshots

![GUI Screenshot](screenshots/gui-main.png)
```

---

## Updating Your Repository

### After Making Changes

**Using GitHub Desktop:**
1. GitHub Desktop shows changed files
2. Write commit message
3. Click "Commit to main"
4. Click "Push origin"

**Using Command Line:**
```bash
git add .
git commit -m "Description of changes"
git push
```

---

## Collaboration Features

### Enable Issues

Settings → Features → ✅ Issues

Users can:
- Report bugs
- Request features
- Ask questions

### Enable Discussions

Settings → Features → ✅ Discussions

Create categories:
- 💬 General
- 💡 Ideas
- 🙏 Q&A
- 🎉 Show and tell (user projects)

### Add Contributors

Settings → Collaborators → Add people

---

## Creating Releases

### When to Create a Release

- First stable version (v1.0.0)
- Major updates
- Bug fix releases

### How to Create a Release

1. **Go to:** Releases → "Create a new release"
2. **Tag version:** v1.0.0
3. **Release title:** Arduino Input Configurator v1.0.0
4. **Description:**
   ```
   ## Features
   - 40 input support (buttons, encoders, switches, pots)
   - PyQt5 GUI configurator
   - Auto-installer scripts for Windows
   - EEPROM configuration storage
   - Ctrl+ keyboard command mapping

   ## Installation
   See [README.md](README.md) for complete setup instructions.

   ## Downloads
   - Source code (zip/tar.gz) - Contains all files
   ```
5. **Attach binaries** (optional):
   - Could package as executable with PyInstaller
6. Click: **"Publish release"**

---

## Best Practices

### Commit Messages

**Good:**
```
Add encoder debouncing support
Fix serial port detection on Linux
Update documentation for Pro Micro wiring
```

**Bad:**
```
updates
fix stuff
changes
```

### Branch Strategy

For solo development:
- Work on `main` branch
- Create feature branches for major changes

For collaboration:
- `main` - stable code
- `develop` - development branch
- `feature/xyz` - feature branches

### .gitignore

Already set up! Excludes:
- Python cache files
- PlatformIO build files
- IDE settings
- OS files
- Logs

---

## GitHub Pages (Optional)

Host documentation website:

1. Settings → Pages
2. Source: Deploy from branch
3. Branch: main, /docs folder
4. Save

Create `docs/index.html` or use Jekyll/MkDocs for nice docs site.

---

## Protecting Your Repository

### Branch Protection

For collaboration, protect main branch:

1. Settings → Branches
2. Add rule for `main`
3. Options:
   - ✅ Require pull request reviews
   - ✅ Require status checks to pass
   - ✅ Require branches to be up to date

### Security

- Settings → Security → Enable security alerts
- GitHub will notify you of vulnerable dependencies

---

## Promoting Your Project

### README Enhancements

- Add demo GIF/video
- Add usage examples
- Link to documentation
- Add contribution guidelines

### Share

- Reddit: r/arduino, r/electronics, r/Python
- Arduino Forum: https://forum.arduino.cc/
- Twitter/X with hashtags: #Arduino #Maker #DIY
- Hackaday: Submit project
- Arduino Project Hub

### Documentation

- Create Wiki pages
- Video tutorials on YouTube
- Blog post about development

---

## Common Git Commands

```bash
# Check status
git status

# Add files
git add .
git add filename.txt

# Commit
git commit -m "Message"

# Push to GitHub
git push

# Pull latest changes
git pull

# View history
git log

# Create branch
git branch feature-name
git checkout feature-name

# Merge branch
git checkout main
git merge feature-name

# Undo changes
git checkout -- filename.txt

# View remotes
git remote -v
```

---

## Troubleshooting

### "fatal: not a git repository"

```bash
cd D:\arduino
git init
```

### "Permission denied (publickey)"

**Use HTTPS instead:**
```bash
git remote set-url origin https://github.com/USERNAME/arduino-input-configurator.git
```

### "Updates were rejected"

```bash
git pull --rebase
git push
```

### Large files error

GitHub has 100MB file limit. Check what's large:
```bash
git ls-files -z | xargs -0 du -h | sort -h -r | head -20
```

Add to .gitignore if needed.

### Forgot to add .gitignore

```bash
git rm -r --cached .
git add .
git commit -m "Apply .gitignore"
```

---

## GitHub Features Checklist

After upload:

- [ ] README.md displays properly
- [ ] Add repository description
- [ ] Add topics/tags
- [ ] Enable Issues
- [ ] Add LICENSE (MIT already included)
- [ ] Create first release (v1.0.0)
- [ ] Star your own repo 😄
- [ ] Share with community

---

## Next Steps

1. **Upload to GitHub** using Method 1, 2, or 3 above
2. **Verify** everything uploaded correctly
3. **Add topics** for discoverability
4. **Create release** (v1.0.0)
5. **Share** on Arduino forums and Reddit
6. **Accept** contributions from community

---

## Repository URL

After setup, your project will be at:

```
https://github.com/YOUR_USERNAME/arduino-input-configurator
```

**Example README URL:**
```
https://github.com/YOUR_USERNAME/arduino-input-configurator/blob/main/README.md
```

---

## Need Help?

- Git documentation: https://git-scm.com/doc
- GitHub Guides: https://guides.github.com/
- GitHub Desktop Help: https://docs.github.com/en/desktop

---

**Ready to share your project with the world? Follow the steps above!** 🚀
