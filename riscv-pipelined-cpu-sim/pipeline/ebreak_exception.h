#ifndef RISC_V_SIMULATOR_EBREAK_EXCEPTION_H
#define RISC_V_SIMULATOR_EBREAK_EXCEPTION_H

#include <exception>


namespace pipeline {

    class EbreakException : public std::exception {
    public:
        EbreakException() = default;
    };
}

#endif //RISC_V_SIMULATOR_EBREAK_EXCEPTION_H
