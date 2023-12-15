#ifndef RISC_V_SIMULATOR_ADDRESS_EXCEPTIONS_H
#define RISC_V_SIMULATOR_ADDRESS_EXCEPTIONS_H

#include <exception>
#include "common.h"


namespace modules {

    class InvalidAddressException : public std::out_of_range {
    public:
        explicit InvalidAddressException(const std::string& mes) : std::out_of_range(mes) {};
        explicit InvalidAddressException(const char *mes) : std::out_of_range(mes) {};
    };

    class AlignmentException : public std::invalid_argument {
    public:
        explicit AlignmentException(const std::string& mes) : std::invalid_argument(mes) {};
        explicit AlignmentException(const char *mes) : std::invalid_argument(mes) {};
    };
}

#endif //RISC_V_SIMULATOR_ADDRESS_EXCEPTIONS_H
