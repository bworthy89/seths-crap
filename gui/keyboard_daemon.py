"""
Keyboard Daemon for Arduino Input Configurator (Mega-Only Version)

This daemon runs in the background, reads serial commands from Arduino Mega,
and sends keyboard commands to the computer.

Replaces the need for Arduino Pro Micro.
"""

import sys
import time
import serial
import serial.tools.list_ports
import threading
import json
from pathlib import Path

# Try to import keyboard library
try:
    import keyboard
    KEYBOARD_AVAILABLE = True
except ImportError:
    KEYBOARD_AVAILABLE = False
    print("Warning: 'keyboard' library not available. Install with: pip install keyboard")


class KeyboardDaemon:
    def __init__(self, port=None, baud_rate=115200):
        """
        Initialize the keyboard daemon.

        Args:
            port (str): Serial port to connect to (auto-detect if None)
            baud_rate (int): Serial baud rate
        """
        self.port = port
        self.baud_rate = baud_rate
        self.serial_connection = None
        self.running = False
        self.thread = None

    def find_arduino_port(self):
        """Auto-detect Arduino Mega port."""
        ports = serial.tools.list_ports.comports()

        for port in ports:
            # Look for Arduino Mega
            if 'Arduino' in port.description or 'CH340' in port.description or 'USB-SERIAL' in port.description:
                print(f"Found potential Arduino at {port.device}: {port.description}")
                return port.device

        return None

    def connect(self):
        """Connect to Arduino Mega via serial."""
        if not KEYBOARD_AVAILABLE:
            print("ERROR: 'keyboard' library not installed!")
            print("Install with: pip install keyboard")
            return False

        # Auto-detect port if not specified
        if self.port is None:
            self.port = self.find_arduino_port()
            if self.port is None:
                print("ERROR: Could not find Arduino Mega. Please specify port manually.")
                return False

        try:
            print(f"Connecting to Arduino Mega on {self.port}...")
            self.serial_connection = serial.Serial(self.port, self.baud_rate, timeout=1)
            time.sleep(2)  # Wait for Arduino to reset
            print(f"Connected to {self.port}")
            return True
        except serial.SerialException as e:
            print(f"Failed to connect: {e}")
            return False

    def process_command(self, command):
        """
        Process a keyboard command from Arduino.

        Args:
            command (str): Command like "CTRL+F" or "CTRL+UPARROW"
        """
        if not KEYBOARD_AVAILABLE:
            return

        command = command.strip()

        # All commands should start with "CTRL+"
        if not command.startswith("CTRL+"):
            return

        # Extract the key part after "CTRL+"
        key_part = command[5:]

        if not key_part:
            return

        try:
            # Map special key names to keyboard library format
            key_mapping = {
                'UPARROW': 'up', 'UP': 'up',
                'DOWNARROW': 'down', 'DOWN': 'down',
                'LEFTARROW': 'left', 'LEFT': 'left',
                'RIGHTARROW': 'right', 'RIGHT': 'right',
                'ENTER': 'enter', 'RETURN': 'enter',
                'ESC': 'esc', 'ESCAPE': 'esc',
                'TAB': 'tab',
                'SPACE': 'space',
                'BACKSPACE': 'backspace',
                'DELETE': 'delete', 'DEL': 'delete',
                'HOME': 'home',
                'END': 'end',
                'PAGEUP': 'page up', 'PGUP': 'page up',
                'PAGEDOWN': 'page down', 'PGDN': 'page down',
                'F1': 'f1', 'F2': 'f2', 'F3': 'f3', 'F4': 'f4',
                'F5': 'f5', 'F6': 'f6', 'F7': 'f7', 'F8': 'f8',
                'F9': 'f9', 'F10': 'f10', 'F11': 'f11', 'F12': 'f12',
            }

            # Convert to keyboard library format
            key_part_upper = key_part.upper()
            if key_part_upper in key_mapping:
                final_key = key_mapping[key_part_upper]
            else:
                # Single character or unmapped
                final_key = key_part.lower()

            # Send the keyboard command
            keyboard.send('ctrl+' + final_key)
            print(f"Sent: ctrl+{final_key}")

        except Exception as e:
            print(f"Error sending keyboard command: {e}")

    def run(self):
        """Main daemon loop - reads serial and sends keyboard commands."""
        if not self.connect():
            return

        self.running = True
        print("\nKeyboard Daemon running...")
        print("Reading commands from Arduino and sending keyboard input.")
        print("Press Ctrl+C to stop.\n")

        input_buffer = ""

        try:
            while self.running:
                if self.serial_connection and self.serial_connection.in_waiting > 0:
                    try:
                        # Read incoming data
                        char = self.serial_connection.read(1).decode('utf-8', errors='ignore')

                        if char == '\n':
                            # Complete command received
                            if input_buffer:
                                self.process_command(input_buffer)
                                input_buffer = ""
                        else:
                            input_buffer += char
                            # Prevent buffer overflow
                            if len(input_buffer) > 100:
                                input_buffer = ""

                    except Exception as e:
                        print(f"Error reading serial: {e}")

                time.sleep(0.001)  # Small delay to prevent CPU spinning

        except KeyboardInterrupt:
            print("\nStopping daemon...")
        finally:
            self.stop()

    def start_background(self):
        """Start the daemon in a background thread."""
        if self.thread and self.thread.is_alive():
            print("Daemon already running")
            return False

        self.thread = threading.Thread(target=self.run, daemon=True)
        self.thread.start()
        return True

    def stop(self):
        """Stop the daemon."""
        self.running = False
        if self.serial_connection and self.serial_connection.is_open:
            self.serial_connection.close()
        print("Daemon stopped")


def main():
    """Command-line interface for keyboard daemon."""
    import argparse

    parser = argparse.ArgumentParser(description='Arduino Keyboard Daemon')
    parser.add_argument('-p', '--port', help='Serial port (auto-detect if not specified)')
    parser.add_argument('-b', '--baud', type=int, default=115200, help='Baud rate (default: 115200)')
    parser.add_argument('-l', '--list', action='store_true', help='List available serial ports')

    args = parser.parse_args()

    if args.list:
        print("Available serial ports:")
        ports = serial.tools.list_ports.comports()
        for port in ports:
            print(f"  {port.device}: {port.description}")
        return

    if not KEYBOARD_AVAILABLE:
        print("\nERROR: 'keyboard' library not installed!")
        print("\nInstall with:")
        print("  pip install keyboard")
        print("\nNote: On Linux, you may need to run as sudo")
        print("      or add your user to the 'input' group")
        sys.exit(1)

    # Create and run daemon
    daemon = KeyboardDaemon(port=args.port, baud_rate=args.baud)
    daemon.run()


if __name__ == '__main__':
    main()
