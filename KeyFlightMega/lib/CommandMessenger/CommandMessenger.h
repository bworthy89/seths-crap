#ifndef COMMANDMESSENGER_H
#define COMMANDMESSENGER_H

#include <Arduino.h>

// Command definitions matching C# Commands.cs
enum Commands {
    kStatus = 0,           // Request device status
    kInfo = 1,             // Device info response
    kSetConfig = 2,        // Set complete configuration
    kGetConfig = 3,        // Request current configuration
    kConfigInfo = 4,       // Configuration data response
    kSetPin = 5,           // Set module pin configuration
    kSetKey = 6,           // Set module keyboard command
    kTrigger = 7,          // Input trigger event
    kResetConfig = 8,      // Reset configuration
    kSaveConfig = 9,       // Save to EEPROM
    kAck = 10,             // Acknowledgment
    kError = 11,           // Error response
    kDebug = 12            // Debug message
};

// Function pointer type for command callbacks
typedef void (*CommandCallback)(char* args[], int argCount);

class CommandMessenger {
public:
    CommandMessenger(Stream& serial);

    void init();
    void processCommands();

    // Attach callback for specific command
    void attach(Commands command, CommandCallback callback);

    // Send command with variable arguments
    void sendCommand(Commands command);
    void sendCommand(Commands command, const char* arg1);
    void sendCommand(Commands command, const char* arg1, const char* arg2);
    void sendCommand(Commands command, const char* arg1, const char* arg2, const char* arg3);
    void sendCommand(Commands command, const char* arg1, const char* arg2, const char* arg3, const char* arg4);

    // Send command with int arguments
    void sendCommandInt(Commands command, int arg1);
    void sendCommandInt(Commands command, int arg1, int arg2);

private:
    Stream& _serial;
    char _buffer[256];
    int _bufferIndex;

    char _fieldSeparator;
    char _commandSeparator;

    static const int MAX_CALLBACKS = 13;
    CommandCallback _callbacks[MAX_CALLBACKS];

    void processCommand(char* commandStr);
    void parseCommand(char* commandStr, char* args[], int& argCount);
    void sendCommandStart(Commands command);
    void sendCommandArg(const char* arg);
    void sendCommandEnd();
};

#endif // COMMANDMESSENGER_H
