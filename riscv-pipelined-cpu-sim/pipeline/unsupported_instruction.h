#ifndef RISC_V_SIMULATOR_UNSUPPORTED_INSTRUCTION_H
#define RISC_V_SIMULATOR_UNSUPPORTED_INSTRUCTION_H

#include <exception>


namespace pipeline {

    class UnsupportedException : public std::invalid_argument {
    public:
        explicit UnsupportedException(const std::string& mes) : std::invalid_argument(mes) {};
        explicit UnsupportedException(const char *mes) : std::invalid_argument(mes) {};
    };
}

#endif //RISC_V_SIMULATOR_UNSUPPORTED_INSTRUCTION_H
