# Contributing to Arduino Input Configurator

Thank you for your interest in contributing! This project welcomes contributions from everyone.

## Ways to Contribute

### 🐛 Report Bugs

Found a bug? Please create an issue with:
- Clear description of the problem
- Steps to reproduce
- Expected vs actual behavior
- Your setup (OS, Python version, Arduino boards)
- Screenshots/logs if applicable

### 💡 Suggest Features

Have an idea? Open an issue tagged "enhancement" with:
- Use case description
- Proposed solution
- Alternative approaches considered

### 📖 Improve Documentation

- Fix typos or unclear instructions
- Add examples
- Translate documentation
- Create tutorials or videos

### 🔧 Submit Code

See below for development setup and pull request process.

---

## Development Setup

### Prerequisites

- Python 3.7+
- Arduino IDE (for testing firmware)
- Arduino Mega 2560 & Pro Micro hardware (for testing)
- Git

### Setup

1. **Fork the repository** on GitHub

2. **Clone your fork:**
   ```bash
   git clone https://github.com/YOUR_USERNAME/arduino-input-configurator.git
   cd arduino-input-configurator
   ```

3. **Install Python dependencies:**
   ```bash
   cd gui
   pip install -r requirements.txt
   ```

4. **Test the GUI:**
   ```bash
   python main.py
   ```

5. **Upload Arduino firmware** to test hardware integration

### Making Changes

1. **Create a branch:**
   ```bash
   git checkout -b feature/your-feature-name
   ```

2. **Make your changes**

3. **Test thoroughly:**
   - Test GUI functionality
   - Test Arduino firmware
   - Test on different configurations if possible

4. **Commit with clear messages:**
   ```bash
   git commit -m "Add feature: description of what you added"
   ```

---

## Pull Request Process

1. **Update documentation** if needed (README.md, user-guide.md, etc.)

2. **Test your changes:**
   - GUI runs without errors
   - Arduino firmware compiles
   - Configuration uploads successfully
   - Inputs work as expected

3. **Push to your fork:**
   ```bash
   git push origin feature/your-feature-name
   ```

4. **Create Pull Request** on GitHub:
   - Clear title describing the change
   - Description of what was changed and why
   - Reference any related issues (#123)
   - Add screenshots/demos if applicable

5. **Respond to feedback:**
   - Address review comments
   - Update PR as needed

---

## Code Style

### Python

- Follow PEP 8 style guide
- Use meaningful variable names
- Add docstrings to functions/classes
- Keep functions focused and concise

**Example:**
```python
def validate_pin_assignment(pin, pin_type):
    """
    Validate that a pin assignment is valid for the given type.

    Args:
        pin (int): Pin number to validate
        pin_type (str): Type of input (button, encoder, etc.)

    Returns:
        bool: True if valid, False otherwise
    """
    # Implementation
```

### Arduino/C++

- Follow Arduino style conventions
- Use clear, descriptive variable names
- Comment complex logic
- Keep functions under 50 lines when possible

**Example:**
```cpp
/**
 * Process button input and send keyboard command
 *
 * @param index Button index in configuration array
 */
void processButton(int index) {
    // Implementation
}
```

### Documentation

- Use Markdown for all documentation
- Keep lines under 100 characters
- Use clear headings and structure
- Include code examples where helpful

---

## Areas Needing Help

Current priorities:

### High Priority
- [ ] Add support for Shift/Alt modifiers (in addition to Ctrl)
- [ ] Configuration import/export (JSON files)
- [ ] Linux serial port permission helper
- [ ] macOS testing and compatibility

### Medium Priority
- [ ] Multi-profile support
- [ ] Mouse movement support
- [ ] Encoder push button mode switching
- [ ] OLED display integration for input names

### Low Priority
- [ ] GUI themes/styling
- [ ] Configuration presets for common apps
- [ ] Web-based configuration UI
- [ ] Analog joystick support

### Documentation
- [ ] Video tutorial for setup
- [ ] More configuration examples
- [ ] Troubleshooting flowcharts
- [ ] Translation to other languages

---

## Testing

### GUI Testing

Test these scenarios:
- Serial port detection
- Connection/disconnection
- Adding/removing inputs
- Configuration upload
- Keyboard command testing
- Error handling

### Arduino Testing

Test with:
- Different input types (buttons, encoders, switches, pots)
- Multiple inputs simultaneously
- All encoder modes (1x, 10x, 100x, 1000x)
- EEPROM save/load across power cycles
- Edge cases (invalid pins, missing connections)

### Hardware Configurations

If possible, test on:
- Different Arduino boards (Mega, Pro Micro variants)
- Different OS (Windows, macOS, Linux)
- Different Python versions (3.7, 3.8, 3.9, 3.10+)

---

## Reporting Security Issues

If you discover a security vulnerability:
- **Do NOT** create a public issue
- Email the maintainer directly
- Provide details of the vulnerability
- Allow time for a fix before public disclosure

---

## Code of Conduct

### Our Standards

- Be respectful and inclusive
- Welcome newcomers
- Give constructive feedback
- Focus on what's best for the project

### Unacceptable Behavior

- Harassment or discriminatory language
- Personal attacks
- Publishing others' private information
- Other unprofessional conduct

---

## Communication

### GitHub Issues

Use for:
- Bug reports
- Feature requests
- Questions about usage

### GitHub Discussions

Use for:
- General questions
- Project ideas
- Showcasing your builds
- Community support

### Pull Request Reviews

- Reviews may take a few days
- Be patient and respectful
- Incorporate feedback constructively
- Discussion is encouraged!

---

## License

By contributing, you agree that your contributions will be licensed under the MIT License.

---

## Recognition

Contributors will be:
- Listed in CONTRIBUTORS.md (if created)
- Credited in release notes
- Thanked in commit messages

---

## Questions?

Not sure where to start? Open an issue tagged "question" and we'll help you get started!

**Thank you for contributing to Arduino Input Configurator!** 🎉
