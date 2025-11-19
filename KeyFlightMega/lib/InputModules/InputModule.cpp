#include "InputModule.h"

InputModule::InputModule(int id, InputType type, int pin, int pin2)
    : _id(id), _type(type), _pin(pin), _pin2(pin2), _enabled(true), _triggered(false) {
}
