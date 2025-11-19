#include "CommandMessenger.h"

CommandMessenger::CommandMessenger(Stream& serial)
    : _serial(serial), _bufferIndex(0), _fieldSeparator(','), _commandSeparator(';') {
    // Initialize callbacks to nullptr
    for (int i = 0; i < MAX_CALLBACKS; i++) {
        _callbacks[i] = nullptr;
    }
}

void CommandMessenger::init() {
    _bufferIndex = 0;
    _buffer[0] = '\0';
}

void CommandMessenger::attach(Commands command, CommandCallback callback) {
    if (command < MAX_CALLBACKS) {
        _callbacks[command] = callback;
    }
}

void CommandMessenger::processCommands() {
    // Read available serial data
    while (_serial.available() > 0) {
        char c = _serial.read();

        // Check for command separator
        if (c == _commandSeparator) {
            _buffer[_bufferIndex] = '\0';
            processCommand(_buffer);
            _bufferIndex = 0;
        }
        // Check for line ending (alternative terminator)
        else if (c == '\n' || c == '\r') {
            if (_bufferIndex > 0) {
                _buffer[_bufferIndex] = '\0';
                processCommand(_buffer);
                _bufferIndex = 0;
            }
        }
        // Add to buffer
        else if (_bufferIndex < sizeof(_buffer) - 1) {
            _buffer[_bufferIndex++] = c;
        }
        // Buffer overflow protection
        else {
            sendCommand(kError, "Buffer overflow");
            _bufferIndex = 0;
        }
    }
}

void CommandMessenger::processCommand(char* commandStr) {
    // Parse command and arguments
    char* args[10];
    int argCount = 0;
    parseCommand(commandStr, args, argCount);

    if (argCount == 0) return;

    // Get command ID
    int commandId = atoi(args[0]);

    // Validate command ID
    if (commandId < 0 || commandId >= MAX_CALLBACKS) {
        sendCommand(kError, "Invalid command ID");
        return;
    }

    // Execute callback if registered
    if (_callbacks[commandId] != nullptr) {
        // Pass arguments (excluding command ID)
        _callbacks[commandId](args + 1, argCount - 1);
    }
}

void CommandMessenger::parseCommand(char* commandStr, char* args[], int& argCount) {
    argCount = 0;
    char* token = strtok(commandStr, ",");

    while (token != nullptr && argCount < 10) {
        args[argCount++] = token;
        token = strtok(nullptr, ",");
    }
}

// Send command with no arguments
void CommandMessenger::sendCommand(Commands command) {
    sendCommandStart(command);
    sendCommandEnd();
}

// Send command with 1 argument
void CommandMessenger::sendCommand(Commands command, const char* arg1) {
    sendCommandStart(command);
    sendCommandArg(arg1);
    sendCommandEnd();
}

// Send command with 2 arguments
void CommandMessenger::sendCommand(Commands command, const char* arg1, const char* arg2) {
    sendCommandStart(command);
    sendCommandArg(arg1);
    sendCommandArg(arg2);
    sendCommandEnd();
}

// Send command with 3 arguments
void CommandMessenger::sendCommand(Commands command, const char* arg1, const char* arg2, const char* arg3) {
    sendCommandStart(command);
    sendCommandArg(arg1);
    sendCommandArg(arg2);
    sendCommandArg(arg3);
    sendCommandEnd();
}

// Send command with 4 arguments
void CommandMessenger::sendCommand(Commands command, const char* arg1, const char* arg2, const char* arg3, const char* arg4) {
    sendCommandStart(command);
    sendCommandArg(arg1);
    sendCommandArg(arg2);
    sendCommandArg(arg3);
    sendCommandArg(arg4);
    sendCommandEnd();
}

// Send command with int argument
void CommandMessenger::sendCommandInt(Commands command, int arg1) {
    char buffer[12];
    itoa(arg1, buffer, 10);
    sendCommand(command, buffer);
}

// Send command with 2 int arguments
void CommandMessenger::sendCommandInt(Commands command, int arg1, int arg2) {
    char buffer1[12], buffer2[12];
    itoa(arg1, buffer1, 10);
    itoa(arg2, buffer2, 10);
    sendCommand(command, buffer1, buffer2);
}

void CommandMessenger::sendCommandStart(Commands command) {
    _serial.print((int)command);
}

void CommandMessenger::sendCommandArg(const char* arg) {
    _serial.print(_fieldSeparator);
    _serial.print(arg);
}

void CommandMessenger::sendCommandEnd() {
    _serial.print(_commandSeparator);
    _serial.println();
    _serial.flush();
}
