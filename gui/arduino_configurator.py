"""
Arduino Input Configurator - Main GUI Application (Enhanced v2.0)

This application provides a graphical interface for configuring
Arduino Mega 2560 inputs (buttons, encoders, switches, potentiometers)
and mapping them to keyboard commands.

NEW FEATURES (v2.0):
- 7-segment display support for encoders (TM1637, TM1638, MAX7219)
- Encoder button functionality (short/long press actions)
- KD2-22 latching switches with LED control and mutual exclusion
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
    QFrame, QTextEdit, QProgressDialog, QMenu, QAction, QDialog,
    QFormLayout, QCheckBox, QListWidget, QDialogButtonBox
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


class AdvancedSettingsDialog(QDialog):
    """Dialog for advanced input configuration"""
    def __init__(self, parent=None, input_type="Button", advanced_config=None):
        super().__init__(parent)
        self.input_type = input_type
        self.config = advanced_config or {}
        self.all_inputs = []  # Will be populated by parent
        self.init_ui()

    def init_ui(self):
        self.setWindowTitle(f"Advanced Settings - {self.input_type}")
        self.setModal(True)
        self.resize(500, 600)

        layout = QVBoxLayout()

        # Create tabs for different feature categories
        if self.input_type == "Rotary Encoder":
            layout.addWidget(self.create_display_section())
            layout.addWidget(self.create_encoder_button_section())
        elif self.input_type == "Latching Switch":
            layout.addWidget(self.create_led_section())
            layout.addWidget(self.create_mutex_section())

        # Dialog buttons
        button_box = QDialogButtonBox(QDialogButtonBox.Ok | QDialogButtonBox.Cancel)
        button_box.accepted.connect(self.accept)
        button_box.rejected.connect(self.reject)
        layout.addWidget(button_box)

        self.setLayout(layout)

    def create_display_section(self):
        group = QGroupBox("7-Segment Display")
        form = QFormLayout()

        # Display type
        self.display_type = QComboBox()
        self.display_type.addItems(["None", "TM1637", "TM1638", "MAX7219"])
        self.display_type.setCurrentText(self.config.get("displayType", "None"))
        self.display_type.currentTextChanged.connect(self.on_display_type_changed)
        form.addRow("Display Type:", self.display_type)

        # CLK Pin
        self.display_clk_pin = QSpinBox()
        self.display_clk_pin.setRange(2, 69)
        self.display_clk_pin.setValue(self.config.get("displayClkPin", 22))
        form.addRow("CLK Pin:", self.display_clk_pin)

        # Data Pin (DIO/DIN)
        self.display_data_pin = QSpinBox()
        self.display_data_pin.setRange(2, 69)
        self.display_data_pin.setValue(self.config.get("displayDataPin", 23))
        form.addRow("DIO/DIN Pin:", self.display_data_pin)

        # CS/STB Pin (for TM1638/MAX7219)
        self.display_cs_pin = QSpinBox()
        self.display_cs_pin.setRange(0, 69)
        self.display_cs_pin.setValue(self.config.get("displayCsPin", 24))
        self.display_cs_pin.setSpecialValueText("N/A")
        form.addRow("CS/STB Pin:", self.display_cs_pin)

        # Number of digits to use
        self.display_digits = QSpinBox()
        self.display_digits.setRange(1, 8)
        self.display_digits.setValue(self.config.get("displayDigits", 4))
        form.addRow("Digits to Use:", self.display_digits)

        # Min/Max values
        self.display_min = QSpinBox()
        self.display_min.setRange(-9999, 9999)
        self.display_min.setValue(self.config.get("displayMin", 0))
        form.addRow("Minimum Value:", self.display_min)

        self.display_max = QSpinBox()
        self.display_max.setRange(-9999, 9999)
        self.display_max.setValue(self.config.get("displayMax", 9999))
        form.addRow("Maximum Value:", self.display_max)

        # Enable/disable based on type
        self.on_display_type_changed(self.display_type.currentText())

        group.setLayout(form)
        return group

    def on_display_type_changed(self, display_type):
        enabled = (display_type != "None")
        self.display_clk_pin.setEnabled(enabled)
        self.display_data_pin.setEnabled(enabled)
        self.display_cs_pin.setEnabled(enabled and display_type in ["TM1638", "MAX7219"])
        self.display_digits.setEnabled(enabled)
        self.display_min.setEnabled(enabled)
        self.display_max.setEnabled(enabled)

    def create_encoder_button_section(self):
        group = QGroupBox("Encoder Push Button")
        form = QFormLayout()

        # Button pin
        self.button_pin = QSpinBox()
        self.button_pin.setRange(0, 69)
        self.button_pin.setValue(self.config.get("buttonPin", 0))
        self.button_pin.setSpecialValueText("None")
        self.button_pin.valueChanged.connect(self.on_button_pin_changed)
        form.addRow("Button Pin:", self.button_pin)

        # Short press action
        self.button_short_action = QComboBox()
        self.button_short_action.addItems(["None", "Cycle Modes", "Send Key", "Reset Display"])
        self.button_short_action.setCurrentText(self.config.get("buttonShortAction", "None"))
        self.button_short_action.currentTextChanged.connect(self.on_short_action_changed)
        form.addRow("Short Press:", self.button_short_action)

        # Short press key command
        self.button_short_key = QLineEdit()
        self.button_short_key.setText(self.config.get("buttonShortKey", ""))
        self.button_short_key.setPlaceholderText("e.g., CTRL+ENTER")
        form.addRow("Short Press Key:", self.button_short_key)

        # Long press action
        self.button_long_action = QComboBox()
        self.button_long_action.addItems(["None", "Cycle Modes", "Send Key", "Reset Display"])
        self.button_long_action.setCurrentText(self.config.get("buttonLongAction", "None"))
        self.button_long_action.currentTextChanged.connect(self.on_long_action_changed)
        form.addRow("Long Press:", self.button_long_action)

        # Long press key command
        self.button_long_key = QLineEdit()
        self.button_long_key.setText(self.config.get("buttonLongKey", ""))
        self.button_long_key.setPlaceholderText("e.g., CTRL+0")
        form.addRow("Long Press Key:", self.button_long_key)

        # Long press threshold
        self.long_press_ms = QSpinBox()
        self.long_press_ms.setRange(100, 5000)
        self.long_press_ms.setValue(self.config.get("longPressMs", 1000))
        self.long_press_ms.setSuffix(" ms")
        form.addRow("Long Press Time:", self.long_press_ms)

        # Enable/disable based on button pin
        self.on_button_pin_changed(self.button_pin.value())

        group.setLayout(form)
        return group

    def on_button_pin_changed(self, pin):
        enabled = (pin > 0)
        self.button_short_action.setEnabled(enabled)
        self.button_short_key.setEnabled(enabled and self.button_short_action.currentText() == "Send Key")
        self.button_long_action.setEnabled(enabled)
        self.button_long_key.setEnabled(enabled and self.button_long_action.currentText() == "Send Key")
        self.long_press_ms.setEnabled(enabled)

    def on_short_action_changed(self, action):
        self.button_short_key.setEnabled(action == "Send Key")

    def on_long_action_changed(self, action):
        self.button_long_key.setEnabled(action == "Send Key")

    def create_led_section(self):
        group = QGroupBox("LED Control")
        form = QFormLayout()

        # LED pin
        self.led_pin = QSpinBox()
        self.led_pin.setRange(0, 69)
        self.led_pin.setValue(self.config.get("ledPin", 0))
        self.led_pin.setSpecialValueText("None")
        form.addRow("LED Pin:", self.led_pin)

        form.addRow(QLabel("LED will turn ON when switch is latched, OFF when unlatched."))

        group.setLayout(form)
        return group

    def create_mutex_section(self):
        group = QGroupBox("Mutual Exclusion Rules")
        layout = QVBoxLayout()

        layout.addWidget(QLabel("Select inputs that should turn OFF when this switch is activated:"))

        # List widget for mutex inputs
        self.mutex_list = QListWidget()
        self.mutex_list.setSelectionMode(QListWidget.MultiSelection)

        # Populate with all inputs except current
        for idx, input_name in enumerate(self.all_inputs):
            self.mutex_list.addItem(f"Input {idx}: {input_name}")

        # Select previously configured mutex inputs
        mutex_indices = self.config.get("mutexList", [])
        for idx in mutex_indices:
            if idx < self.mutex_list.count():
                self.mutex_list.item(idx).setSelected(True)

        layout.addWidget(self.mutex_list)

        layout.addWidget(QLabel("When this switch turns ON, the selected inputs will turn OFF."))

        group.setLayout(layout)
        return group

    def get_config(self):
        """Return the advanced configuration dictionary"""
        config = {}

        if self.input_type == "Rotary Encoder":
            # Display configuration
            display_type_map = {"None": 0, "TM1637": 1, "TM1638": 2, "MAX7219": 3}
            config["displayType"] = display_type_map[self.display_type.currentText()]
            config["displayClkPin"] = self.display_clk_pin.value()
            config["displayDataPin"] = self.display_data_pin.value()
            config["displayCsPin"] = self.display_cs_pin.value()
            config["displayDigits"] = self.display_digits.value()
            config["displayMin"] = self.display_min.value()
            config["displayMax"] = self.display_max.value()

            # Encoder button configuration
            action_map = {"None": 0, "Cycle Modes": 1, "Send Key": 2, "Reset Display": 3}
            config["buttonPin"] = self.button_pin.value()
            config["buttonShortAction"] = action_map[self.button_short_action.currentText()]
            config["buttonLongAction"] = action_map[self.button_long_action.currentText()]
            config["buttonShortKey"] = self.button_short_key.text()
            config["buttonLongKey"] = self.button_long_key.text()
            config["longPressMs"] = self.long_press_ms.value()

        elif self.input_type == "Latching Switch":
            # LED configuration
            config["ledPin"] = self.led_pin.value()

            # Mutex configuration
            selected_items = self.mutex_list.selectedItems()
            config["mutexList"] = [self.mutex_list.row(item) for item in selected_items]
            config["mutexCount"] = len(config["mutexList"])

        return config


class ArduinoConfigurator(QMainWindow):
    def __init__(self):
        super().__init__()
        self.serial_connection = None
        self.inputs = []
        self.max_inputs = 40
        self.advanced_configs = {}  # Store advanced configs per row

        self.input_types = {
            "Button": 1,
            "Rotary Encoder": 2,
            "Switch": 3,
            "Potentiometer": 4,
            "Latching Switch": 5  # New type
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
        self.setWindowTitle("Arduino Input Configurator - ELEGOO MEGA R3 (v2.0)")
        self.setGeometry(100, 100, 1300, 800)

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

        # Check for updates on startup (after a delay)
        if UPDATER_AVAILABLE:
            QTimer.singleShot(2000, self.check_for_updates_async)

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

        self.advanced_settings_btn = QPushButton("⚙ Advanced Settings")
        self.advanced_settings_btn.clicked.connect(self.open_advanced_settings)
        button_layout.addWidget(self.advanced_settings_btn)

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
            "All keyboard commands start with CTRL+. "
            "Use 'Advanced Settings' for displays, encoder buttons, and LED control."
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

        # Initialize advanced config for this row
        self.advanced_configs[row] = {}

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
        key_input = QLineEdit("CTRL+")
        key_input.setPlaceholderText("e.g., CTRL+F or CTRL+UPARROW")
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

    def open_advanced_settings(self):
        """Open advanced settings dialog for selected row"""
        current_row = self.config_table.currentRow()
        if current_row < 0:
            QMessageBox.information(self, "No Selection", "Please select an input row first")
            return

        # Get input type
        type_combo = self.config_table.cellWidget(current_row, 1)
        input_type = type_combo.currentText()

        # Only show advanced settings for Rotary Encoder and Latching Switch
        if input_type not in ["Rotary Encoder", "Latching Switch"]:
            QMessageBox.information(
                self, "Not Available",
                "Advanced settings are only available for Rotary Encoders and Latching Switches."
            )
            return

        # Get all input names for mutex list
        all_input_names = []
        for row in range(self.config_table.rowCount()):
            name_widget = self.config_table.cellWidget(row, 0)
            all_input_names.append(name_widget.text() if name_widget else f"Input {row}")

        # Create and show dialog
        dialog = AdvancedSettingsDialog(self, input_type, self.advanced_configs.get(current_row, {}))
        dialog.all_inputs = all_input_names

        if dialog.exec_() == QDialog.Accepted:
            self.advanced_configs[current_row] = dialog.get_config()
            self.log_console(f"Advanced settings updated for row {current_row + 1}")

    def remove_input_row(self):
        """Remove selected input row"""
        current_row = self.config_table.currentRow()
        if current_row >= 0:
            self.config_table.removeRow(current_row)

            # Remove advanced config and shift subsequent rows
            if current_row in self.advanced_configs:
                del self.advanced_configs[current_row]

            # Shift advanced configs for rows after deleted row
            new_configs = {}
            for row, config in self.advanced_configs.items():
                if row > current_row:
                    new_configs[row - 1] = config
                elif row < current_row:
                    new_configs[row] = config
            self.advanced_configs = new_configs

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
            self.advanced_configs = {}
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

            # Add advanced configuration if exists
            if row in self.advanced_configs:
                input_config.update(self.advanced_configs[row])
            else:
                # Add default values for new fields
                input_config.update({
                    "displayType": 0,
                    "displayClkPin": 0,
                    "displayDataPin": 0,
                    "displayCsPin": 0,
                    "displayDigits": 4,
                    "displayMin": 0,
                    "displayMax": 9999,
                    "buttonPin": 0,
                    "buttonShortAction": 0,
                    "buttonLongAction": 0,
                    "buttonShortKey": "",
                    "buttonLongKey": "",
                    "longPressMs": 1000,
                    "ledPin": 0,
                    "mutexCount": 0,
                    "mutexList": []
                })

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
            self.log_console(f"Test error: {str(e)}")

    def request_status(self):
        """Request status from Arduino"""
        if self.serial_connection is None or not self.serial_connection.is_open:
            return

        try:
            status_cmd = {"type": "status"}
            json_str = json.dumps(status_cmd)
            self.serial_connection.write((json_str + '\n').encode())

            QTimer.singleShot(300, self.read_arduino_response)

        except Exception as e:
            self.log_console(f"Failed to request status: {str(e)}")

    def read_arduino_response(self):
        """Read and display Arduino responses"""
        if self.serial_connection is None or not self.serial_connection.is_open:
            return

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
        """Check for updates in background (non-blocking)"""
        if not UPDATER_AVAILABLE:
            return

        try:
            updater = Updater()
            update_available, latest_version = updater.check_for_updates()

            if update_available:
                self.log_console(f"Update available: v{latest_version}")
                self.show_update_notification(latest_version)

        except Exception as e:
            # Silently fail for background check
            pass

    def check_for_updates_manual(self):
        """Manual update check (user initiated)"""
        if not UPDATER_AVAILABLE:
            QMessageBox.information(self, "Updater Not Available",
                                  "The updater module is not available.")
            return

        try:
            updater = Updater()
            update_available, latest_version = updater.check_for_updates()

            if update_available:
                self.show_update_dialog(latest_version)
            else:
                QMessageBox.information(self, "Up to Date",
                                      "You are already using the latest version.")

        except Exception as e:
            QMessageBox.critical(self, "Update Check Failed",
                               f"Failed to check for updates:\n{str(e)}")

    def show_update_notification(self, latest_version):
        """Show non-intrusive update notification"""
        self.statusBar().showMessage(f"Update available: v{latest_version} (Help → Check for Updates)", 10000)

    def show_update_dialog(self, latest_version):
        """Show update dialog"""
        reply = QMessageBox.question(
            self, "Update Available",
            f"A new version (v{latest_version}) is available.\n\nWould you like to update now?",
            QMessageBox.Yes | QMessageBox.No
        )

        if reply == QMessageBox.Yes:
            self.perform_update()

    def perform_update(self):
        """Perform the update"""
        if not UPDATER_AVAILABLE:
            return

        progress = QProgressDialog("Downloading update...", "Cancel", 0, 0, self)
        progress.setWindowModality(Qt.WindowModal)
        progress.setWindowTitle("Updating")
        progress.show()

        try:
            updater = Updater()
            success = updater.download_and_install_update()

            progress.close()

            if success:
                QMessageBox.information(
                    self, "Update Complete",
                    "Update installed successfully!\n\nPlease restart the application."
                )
            else:
                QMessageBox.warning(
                    self, "Update Failed",
                    "Failed to install update. Please try again later."
                )

        except Exception as e:
            progress.close()
            QMessageBox.critical(self, "Update Error", f"Update failed:\n{str(e)}")

    def show_about(self):
        """Show about dialog"""
        about_text = """
        <h2>Arduino Input Configurator v2.0</h2>
        <p>Enhanced configuration tool for Arduino Mega 2560</p>

        <p><b>New Features in v2.0:</b></p>
        <ul>
            <li>7-segment display support (TM1637, TM1638, MAX7219)</li>
            <li>Encoder push button functionality</li>
            <li>Latching switches with LED control</li>
            <li>Mutual exclusion rules for switches</li>
        </ul>

        <p><b>Hardware:</b> ELEGOO MEGA R3 + Pro Micro</p>
        <p><b>Repository:</b> github.com/bworthy89/seths-crap</p>
        """

        QMessageBox.about(self, "About", about_text)


def main():
    app = QApplication(sys.argv)
    app.setStyle('Fusion')  # Use Fusion style for consistent look

    window = ArduinoConfigurator()
    window.show()

    sys.exit(app.exec_())


if __name__ == "__main__":
    main()
