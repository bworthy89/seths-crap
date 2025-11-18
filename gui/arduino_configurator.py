"""
Arduino Input Configurator - Main GUI Application

This application provides a graphical interface for configuring
Arduino Mega 2560 inputs (buttons, encoders, switches, potentiometers)
and mapping them to keyboard commands.
"""

import sys
import json
import serial
import serial.tools.list_ports
from pathlib import Path
from PyQt5.QtWidgets import (
    QApplication, QMainWindow, QWidget, QVBoxLayout, QHBoxLayout,
    QLabel, QComboBox, QPushButton, QTableWidget, QTableWidgetItem,
    QMessageBox, QGroupBox, QLineEdit, QSpinBox, QHeaderView,
    QFrame, QTextEdit, QProgressDialog, QMenu, QAction
)
from PyQt5.QtCore import Qt, QTimer, QThread, pyqtSignal
from PyQt5.QtGui import QFont, QColor

# Import updater module
try:
    from updater import Updater
    UPDATER_AVAILABLE = True
except ImportError:
    UPDATER_AVAILABLE = False
    print("Warning: Updater module not available")


class ArduinoConfigurator(QMainWindow):
    def __init__(self):
        super().__init__()
        self.serial_connection = None
        self.inputs = []
        self.max_inputs = 40

        self.input_types = {
            "Button": 1,
            "Rotary Encoder": 2,
            "Switch": 3,
            "Potentiometer": 4
        }

        self.encoder_modes = {
            "1x (increment by 1)": 0,
            "10x (increment by 10)": 1,
            "100x (increment by 100)": 2,
            "1000x (increment by 1000)": 3
        }

        # Arduino Mega 2560 pin definitions
        self.digital_pins = list(range(2, 54))  # Skip 0-1 (Serial)
        self.analog_pins = [f"A{i}" for i in range(16)]
        self.interrupt_pins = [2, 3, 18, 19, 20, 21]  # Best for encoders

        self.init_ui()

    def init_ui(self):
        self.setWindowTitle("Arduino Input Configurator - ELEGOO MEGA R3")
        self.setGeometry(100, 100, 1200, 800)

        # Main widget and layout
        main_widget = QWidget()
        self.setCentralWidget(main_widget)
        main_layout = QVBoxLayout()
        main_widget.setLayout(main_layout)

        # Connection section
        connection_group = self.create_connection_section()
        main_layout.addWidget(connection_group)

        # Input configuration section
        config_group = self.create_configuration_section()
        main_layout.addWidget(config_group)

        # Console output section
        console_group = self.create_console_section()
        main_layout.addWidget(console_group)

        # Status bar
        self.statusBar().showMessage("Ready - Not Connected")

        # Menu bar
        self.create_menu_bar()

        # Apply styling
        self.apply_styling()

        # Refresh serial ports on startup
        self.refresh_ports()

    def create_menu_bar(self):
        """Create menu bar with Help menu"""
        menubar = self.menuBar()

        # Help menu
        help_menu = menubar.addMenu("Help")

        # Check for updates action
        if UPDATER_AVAILABLE:
            update_action = QAction("Check for Updates", self)
            update_action.triggered.connect(self.check_for_updates_manual)
            help_menu.addAction(update_action)

        # About action
        about_action = QAction("About", self)
        about_action.triggered.connect(self.show_about)
        help_menu.addAction(about_action)

    def create_connection_section(self):
        group = QGroupBox("Arduino Connection")
        layout = QHBoxLayout()

        # Serial port selection
        layout.addWidget(QLabel("Serial Port:"))
        self.port_combo = QComboBox()
        self.port_combo.setMinimumWidth(200)
        layout.addWidget(self.port_combo)

        # Refresh button
        self.refresh_btn = QPushButton("Refresh Ports")
        self.refresh_btn.clicked.connect(self.refresh_ports)
        layout.addWidget(self.refresh_btn)

        # Connect button
        self.connect_btn = QPushButton("Connect")
        self.connect_btn.clicked.connect(self.toggle_connection)
        self.connect_btn.setStyleSheet("background-color: #4CAF50; color: white; font-weight: bold;")
        layout.addWidget(self.connect_btn)

        # Status indicator
        self.connection_status = QLabel("● Disconnected")
        self.connection_status.setStyleSheet("color: red; font-weight: bold;")
        layout.addWidget(self.connection_status)

        layout.addStretch()

        group.setLayout(layout)
        return group

    def create_configuration_section(self):
        group = QGroupBox("Input Configuration")
        layout = QVBoxLayout()

        # Buttons row
        button_layout = QHBoxLayout()

        self.add_input_btn = QPushButton("+ Add Input")
        self.add_input_btn.clicked.connect(self.add_input_row)
        button_layout.addWidget(self.add_input_btn)

        self.remove_input_btn = QPushButton("- Remove Selected")
        self.remove_input_btn.clicked.connect(self.remove_input_row)
        button_layout.addWidget(self.remove_input_btn)

        self.clear_all_btn = QPushButton("Clear All")
        self.clear_all_btn.clicked.connect(self.clear_all_inputs)
        button_layout.addWidget(self.clear_all_btn)

        button_layout.addStretch()

        self.upload_config_btn = QPushButton("⬆ Upload to Arduino")
        self.upload_config_btn.clicked.connect(self.upload_configuration)
        self.upload_config_btn.setStyleSheet("background-color: #2196F3; color: white; font-weight: bold;")
        button_layout.addWidget(self.upload_config_btn)

        self.test_key_btn = QPushButton("Test Selected Key")
        self.test_key_btn.clicked.connect(self.test_selected_key)
        button_layout.addWidget(self.test_key_btn)

        layout.addLayout(button_layout)

        # Configuration table
        self.config_table = QTableWidget()
        self.config_table.setColumnCount(6)
        self.config_table.setHorizontalHeaderLabels([
            "Name", "Type", "Pin 1", "Pin 2", "Encoder Mode", "Keyboard Command"
        ])

        # Set column widths
        header = self.config_table.horizontalHeader()
        header.setSectionResizeMode(0, QHeaderView.Stretch)  # Name
        header.setSectionResizeMode(1, QHeaderView.ResizeToContents)  # Type
        header.setSectionResizeMode(2, QHeaderView.ResizeToContents)  # Pin 1
        header.setSectionResizeMode(3, QHeaderView.ResizeToContents)  # Pin 2
        header.setSectionResizeMode(4, QHeaderView.ResizeToContents)  # Encoder Mode
        header.setSectionResizeMode(5, QHeaderView.Stretch)  # Keyboard Command

        self.config_table.setSelectionBehavior(QTableWidget.SelectRows)
        layout.addWidget(self.config_table)

        # Info label
        info_label = QLabel(
            "💡 Tips: Use interrupt pins (2,3,18,19,20,21) for encoders. "
            "Use '+' to combine keys (e.g., L+N+A+V, CTRL+A). Supports CTRL, SHIFT, ALT, GUI modifiers. "
            "Encoder Pin 2 is only for rotary encoders (A/B pins)."
        )
        info_label.setWordWrap(True)
        info_label.setStyleSheet("color: #666; font-size: 10px; padding: 5px;")
        layout.addWidget(info_label)

        group.setLayout(layout)
        return group

    def create_console_section(self):
        group = QGroupBox("Console Output")
        layout = QVBoxLayout()

        self.console = QTextEdit()
        self.console.setReadOnly(True)
        self.console.setMaximumHeight(150)
        self.console.setStyleSheet("background-color: #1e1e1e; color: #d4d4d4; font-family: Consolas, monospace;")
        layout.addWidget(self.console)

        # Clear console button
        clear_btn = QPushButton("Clear Console")
        clear_btn.clicked.connect(self.console.clear)
        layout.addWidget(clear_btn)

        group.setLayout(layout)
        return group

    def apply_styling(self):
        self.setStyleSheet("""
            QMainWindow {
                background-color: #f5f5f5;
            }
            QGroupBox {
                font-weight: bold;
                border: 2px solid #cccccc;
                border-radius: 5px;
                margin-top: 10px;
                padding-top: 10px;
            }
            QGroupBox::title {
                subcontrol-origin: margin;
                left: 10px;
                padding: 0 5px;
            }
            QPushButton {
                padding: 8px 15px;
                border-radius: 3px;
                border: 1px solid #ccc;
                background-color: #ffffff;
            }
            QPushButton:hover {
                background-color: #e0e0e0;
            }
            QPushButton:pressed {
                background-color: #d0d0d0;
            }
            QTableWidget {
                background-color: white;
                gridline-color: #d0d0d0;
            }
            QHeaderView::section {
                background-color: #e0e0e0;
                padding: 5px;
                border: 1px solid #c0c0c0;
                font-weight: bold;
            }
        """)

    def refresh_ports(self):
        """Refresh the list of available serial ports"""
        self.port_combo.clear()
        ports = serial.tools.list_ports.comports()

        for port in ports:
            # Show port name and description
            self.port_combo.addItem(f"{port.device} - {port.description}", port.device)

        if self.port_combo.count() == 0:
            self.port_combo.addItem("No ports found", None)
            self.log_console("No serial ports detected")
        else:
            self.log_console(f"Found {self.port_combo.count()} serial port(s)")

    def toggle_connection(self):
        """Connect or disconnect from Arduino"""
        if self.serial_connection is None or not self.serial_connection.is_open:
            self.connect_to_arduino()
        else:
            self.disconnect_from_arduino()

    def connect_to_arduino(self):
        """Establish serial connection to Arduino"""
        port = self.port_combo.currentData()
        if port is None:
            QMessageBox.warning(self, "Connection Error", "No valid serial port selected")
            return

        try:
            self.serial_connection = serial.Serial(port, 115200, timeout=2)
            self.connect_btn.setText("Disconnect")
            self.connect_btn.setStyleSheet("background-color: #f44336; color: white; font-weight: bold;")
            self.connection_status.setText("● Connected")
            self.connection_status.setStyleSheet("color: green; font-weight: bold;")
            self.statusBar().showMessage(f"Connected to {port}")
            self.log_console(f"Connected to Arduino on {port}")

            # Request status
            QTimer.singleShot(1000, self.request_status)

        except serial.SerialException as e:
            QMessageBox.critical(self, "Connection Error", f"Failed to connect:\n{str(e)}")
            self.log_console(f"Connection failed: {str(e)}")

    def disconnect_from_arduino(self):
        """Close serial connection to Arduino"""
        if self.serial_connection and self.serial_connection.is_open:
            self.serial_connection.close()

        self.serial_connection = None
        self.connect_btn.setText("Connect")
        self.connect_btn.setStyleSheet("background-color: #4CAF50; color: white; font-weight: bold;")
        self.connection_status.setText("● Disconnected")
        self.connection_status.setStyleSheet("color: red; font-weight: bold;")
        self.statusBar().showMessage("Disconnected")
        self.log_console("Disconnected from Arduino")

    def add_input_row(self):
        """Add a new input configuration row"""
        if self.config_table.rowCount() >= self.max_inputs:
            QMessageBox.warning(self, "Limit Reached", f"Maximum {self.max_inputs} inputs supported")
            return

        row = self.config_table.rowCount()
        self.config_table.insertRow(row)

        # Name
        name_item = QLineEdit(f"Input {row + 1}")
        self.config_table.setCellWidget(row, 0, name_item)

        # Type
        type_combo = QComboBox()
        type_combo.addItems(list(self.input_types.keys()))
        type_combo.currentTextChanged.connect(lambda text, r=row: self.on_type_changed(r, text))
        self.config_table.setCellWidget(row, 1, type_combo)

        # Pin 1
        pin1_combo = QComboBox()
        pin1_combo.addItems([str(p) for p in self.digital_pins] + self.analog_pins)
        self.config_table.setCellWidget(row, 2, pin1_combo)

        # Pin 2 (for encoders)
        pin2_combo = QComboBox()
        pin2_combo.addItem("N/A")
        pin2_combo.addItems([str(p) for p in self.digital_pins])
        pin2_combo.setEnabled(False)
        self.config_table.setCellWidget(row, 3, pin2_combo)

        # Encoder Mode
        mode_combo = QComboBox()
        mode_combo.addItems(list(self.encoder_modes.keys()))
        mode_combo.setEnabled(False)
        self.config_table.setCellWidget(row, 4, mode_combo)

        # Keyboard Command
        key_input = QLineEdit("")
        key_input.setPlaceholderText("e.g., A, CTRL+F, L+N+A+V, SHIFT+CTRL+S")
        self.config_table.setCellWidget(row, 5, key_input)

        self.log_console(f"Added input row {row + 1}")

    def on_type_changed(self, row, input_type):
        """Handle input type change"""
        pin2_combo = self.config_table.cellWidget(row, 3)
        mode_combo = self.config_table.cellWidget(row, 4)

        if input_type == "Rotary Encoder":
            pin2_combo.setEnabled(True)
            pin2_combo.setCurrentIndex(1)  # Select first pin after N/A
            mode_combo.setEnabled(True)
        else:
            pin2_combo.setEnabled(False)
            pin2_combo.setCurrentIndex(0)  # Set to N/A
            mode_combo.setEnabled(False)

    def remove_input_row(self):
        """Remove selected input row"""
        current_row = self.config_table.currentRow()
        if current_row >= 0:
            self.config_table.removeRow(current_row)
            self.log_console(f"Removed input row {current_row + 1}")
        else:
            QMessageBox.information(self, "No Selection", "Please select a row to remove")

    def clear_all_inputs(self):
        """Clear all input configurations"""
        reply = QMessageBox.question(
            self, "Clear All",
            "Are you sure you want to remove all input configurations?",
            QMessageBox.Yes | QMessageBox.No
        )

        if reply == QMessageBox.Yes:
            self.config_table.setRowCount(0)
            self.log_console("Cleared all input configurations")

    def upload_configuration(self):
        """Upload configuration to Arduino"""
        if self.serial_connection is None or not self.serial_connection.is_open:
            QMessageBox.warning(self, "Not Connected", "Please connect to Arduino first")
            return

        # Build configuration JSON
        config = {
            "type": "config",
            "inputs": []
        }

        for row in range(self.config_table.rowCount()):
            name_widget = self.config_table.cellWidget(row, 0)
            type_combo = self.config_table.cellWidget(row, 1)
            pin1_combo = self.config_table.cellWidget(row, 2)
            pin2_combo = self.config_table.cellWidget(row, 3)
            mode_combo = self.config_table.cellWidget(row, 4)
            key_input = self.config_table.cellWidget(row, 5)

            # Validate
            if not key_input.text().strip():
                QMessageBox.warning(self, "Invalid Configuration",
                                  f"Row {row + 1}: Keyboard command is required")
                return

            # Parse pin values
            pin1_text = pin1_combo.currentText()
            pin1 = int(pin1_text.replace('A', '')) + 54 if 'A' in pin1_text else int(pin1_text)

            pin2_text = pin2_combo.currentText()
            pin2 = 0 if pin2_text == "N/A" else int(pin2_text)

            input_config = {
                "pin": pin1,
                "pin2": pin2,
                "type": self.input_types[type_combo.currentText()],
                "mode": self.encoder_modes[mode_combo.currentText()],
                "name": name_widget.text(),
                "key": key_input.text().strip()
            }

            config["inputs"].append(input_config)

        # Send to Arduino
        try:
            json_str = json.dumps(config)
            self.serial_connection.write((json_str + '\n').encode())
            self.log_console(f"Uploaded configuration: {len(config['inputs'])} inputs")

            # Read response
            QTimer.singleShot(500, self.read_arduino_response)

            QMessageBox.information(self, "Upload Complete",
                                  f"Configuration uploaded successfully!\n{len(config['inputs'])} inputs configured.")

        except Exception as e:
            QMessageBox.critical(self, "Upload Error", f"Failed to upload configuration:\n{str(e)}")
            self.log_console(f"Upload error: {str(e)}")

    def test_selected_key(self):
        """Test the keyboard command of selected row"""
        if self.serial_connection is None or not self.serial_connection.is_open:
            QMessageBox.warning(self, "Not Connected", "Please connect to Arduino first")
            return

        current_row = self.config_table.currentRow()
        if current_row < 0:
            QMessageBox.information(self, "No Selection", "Please select a row to test")
            return

        key_input = self.config_table.cellWidget(current_row, 5)
        key_command = key_input.text().strip()

        if not key_command:
            QMessageBox.warning(self, "Invalid Command", "Keyboard command is empty")
            return

        # Send test command
        test_cmd = {
            "type": "test",
            "key": key_command
        }

        try:
            json_str = json.dumps(test_cmd)
            self.serial_connection.write((json_str + '\n').encode())
            self.log_console(f"Testing key command: {key_command}")

            QTimer.singleShot(200, self.read_arduino_response)

        except Exception as e:
            QMessageBox.critical(self, "Test Error", f"Failed to send test command:\n{str(e)}")

    def request_status(self):
        """Request status from Arduino"""
        if self.serial_connection and self.serial_connection.is_open:
            try:
                status_cmd = {"type": "status"}
                json_str = json.dumps(status_cmd)
                self.serial_connection.write((json_str + '\n').encode())
                QTimer.singleShot(300, self.read_arduino_response)
            except:
                pass

    def read_arduino_response(self):
        """Read response from Arduino"""
        if self.serial_connection and self.serial_connection.is_open:
            try:
                while self.serial_connection.in_waiting > 0:
                    line = self.serial_connection.readline().decode('utf-8', errors='ignore').strip()
                    if line:
                        self.log_console(f"Arduino: {line}")
            except Exception as e:
                self.log_console(f"Read error: {str(e)}")

    def log_console(self, message):
        """Log message to console"""
        self.console.append(message)
        # Auto-scroll to bottom
        self.console.verticalScrollBar().setValue(
            self.console.verticalScrollBar().maximum()
        )

    def check_for_updates_async(self):
        """Check for updates asynchronously on startup"""
        if not UPDATER_AVAILABLE:
            return

        try:
            updater = Updater()
            update_info = updater.check_for_updates(timeout=5)

            if update_info and update_info['available']:
                # Show notification
                reply = QMessageBox.question(
                    self,
                    "Update Available",
                    f"A new version ({update_info['latest_version']}) is available!\n\n"
                    f"Current version: {update_info['current_version']}\n\n"
                    f"Do you want to download and install it now?",
                    QMessageBox.Yes | QMessageBox.No,
                    QMessageBox.No
                )

                if reply == QMessageBox.Yes:
                    self.perform_update(updater, update_info)

        except Exception as e:
            # Silent fail on startup check
            print(f"Auto-update check failed: {e}")

    def check_for_updates_manual(self):
        """Manually check for updates (from menu)"""
        if not UPDATER_AVAILABLE:
            QMessageBox.warning(
                self,
                "Updater Not Available",
                "The updater module is not available."
            )
            return

        try:
            # Show checking dialog
            progress = QProgressDialog("Checking for updates...", None, 0, 0, self)
            progress.setWindowModality(Qt.WindowModal)
            progress.setMinimumDuration(0)
            progress.show()
            QApplication.processEvents()

            updater = Updater()
            update_info = updater.check_for_updates(timeout=10)

            progress.close()

            if update_info is None:
                QMessageBox.warning(
                    self,
                    "Update Check Failed",
                    "Failed to check for updates. Please check your internet connection."
                )
                return

            if not update_info['available']:
                QMessageBox.information(
                    self,
                    "No Updates",
                    f"You have the latest version ({update_info['current_version']})."
                )
                return

            # Show update dialog with release notes
            msg_box = QMessageBox(self)
            msg_box.setWindowTitle("Update Available")
            msg_box.setText(
                f"A new version ({update_info['latest_version']}) is available!\n\n"
                f"Current version: {update_info['current_version']}"
            )
            msg_box.setDetailedText(update_info['release_notes'])
            msg_box.setStandardButtons(QMessageBox.Yes | QMessageBox.No)
            msg_box.setDefaultButton(QMessageBox.Yes)
            msg_box.button(QMessageBox.Yes).setText("Install Update")
            msg_box.button(QMessageBox.No).setText("Later")

            if msg_box.exec_() == QMessageBox.Yes:
                self.perform_update(updater, update_info)

        except Exception as e:
            QMessageBox.critical(
                self,
                "Error",
                f"Update check failed:\n{str(e)}"
            )

    def perform_update(self, updater, update_info):
        """Download and install update"""
        # Create progress dialog
        progress = QProgressDialog(
            "Downloading update...",
            "Cancel",
            0,
            100,
            self
        )
        progress.setWindowModality(Qt.WindowModal)
        progress.setMinimumDuration(0)
        progress.setAutoClose(False)
        progress.show()

        def update_progress(percent, status):
            progress.setValue(percent)
            progress.setLabelText(status)
            QApplication.processEvents()

            if progress.wasCanceled():
                raise InterruptedError("Update cancelled by user")

        try:
            # Download and install
            success = updater.download_and_install_update(
                update_info['download_url'],
                callback=update_progress
            )

            progress.close()

            if success:
                reply = QMessageBox.question(
                    self,
                    "Update Complete",
                    "Update installed successfully!\n\n"
                    "The application needs to restart to apply the update.\n\n"
                    "Restart now?",
                    QMessageBox.Yes | QMessageBox.No,
                    QMessageBox.Yes
                )

                if reply == QMessageBox.Yes:
                    # Close serial connection
                    if self.serial_connection and self.serial_connection.is_open:
                        self.serial_connection.close()

                    # Restart application
                    updater.restart_application()

                    # Exit current instance
                    QApplication.quit()
            else:
                QMessageBox.warning(
                    self,
                    "Update Failed",
                    "Failed to install update. Please try again or download manually."
                )

        except InterruptedError:
            progress.close()
            QMessageBox.information(self, "Cancelled", "Update cancelled.")
        except Exception as e:
            progress.close()
            QMessageBox.critical(
                self,
                "Update Error",
                f"An error occurred during update:\n{str(e)}"
            )

    def show_about(self):
        """Show about dialog"""
        version = "Unknown"
        if UPDATER_AVAILABLE:
            try:
                updater = Updater()
                version = updater.current_version
            except:
                pass

        QMessageBox.about(
            self,
            "About Arduino Input Configurator",
            f"<h2>Arduino Input Configurator</h2>"
            f"<p><b>Version:</b> {version}</p>"
            f"<p>GUI configurator for Arduino Mega 2560 + Pro Micro<br>"
            f"to create custom USB HID keyboard controllers.</p>"
            f"<p><b>Features:</b></p>"
            f"<ul>"
            f"<li>Up to 40 inputs (buttons, encoders, switches, pots)</li>"
            f"<li>USB HID keyboard emulation</li>"
            f"<li>EEPROM configuration storage</li>"
            f"<li>Rotary encoder modes (1x/10x/100x/1000x)</li>"
            f"</ul>"
            f"<p><b>GitHub:</b> <a href='https://github.com/bworthy89/seths-crap'>"
            f"github.com/bworthy89/seths-crap</a></p>"
            f"<p><b>License:</b> MIT</p>"
        )

    def closeEvent(self, event):
        """Handle window close event"""
        if self.serial_connection and self.serial_connection.is_open:
            self.serial_connection.close()
        event.accept()


def main():
    app = QApplication(sys.argv)

    # Check for updates before showing main window
    if UPDATER_AVAILABLE:
        try:
            from updater import Updater
            updater = Updater()
            update_info = updater.check_for_updates(timeout=5)

            if update_info and update_info['available']:
                # Show update dialog
                reply = QMessageBox.question(
                    None,
                    "Update Available",
                    f"A new version is available!\n\n"
                    f"Current version: {update_info['current_version']}\n"
                    f"Latest version: {update_info['latest_version']}\n\n"
                    f"Would you like to update now?",
                    QMessageBox.Yes | QMessageBox.No,
                    QMessageBox.Yes
                )

                if reply == QMessageBox.Yes:
                    # Show progress dialog
                    progress = QMessageBox(
                        QMessageBox.Information,
                        "Updating",
                        "Downloading and installing update...",
                        QMessageBox.NoButton
                    )
                    progress.show()
                    app.processEvents()

                    # Download and install update
                    success = updater.download_and_install_update(update_info['download_url'])
                    progress.close()

                    if success:
                        QMessageBox.information(
                            None,
                            "Update Complete",
                            "Update installed successfully!\n\n"
                            "Please restart the application."
                        )
                        sys.exit(0)
                    else:
                        QMessageBox.warning(
                            None,
                            "Update Failed",
                            "Failed to install update. The application will start normally.\n\n"
                            "You can try updating manually from the Help menu."
                        )
        except Exception as e:
            print(f"Update check failed: {e}")

    # Show main window
    window = ArduinoConfigurator()
    window.show()
    sys.exit(app.exec_())


if __name__ == '__main__':
    main()
