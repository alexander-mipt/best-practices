#ifndef RISC_V_SIMULATOR_GATES_H
#define RISC_V_SIMULATOR_GATES_H

#include <exception>
#include "common.h"


namespace modules {

    template <typename T>
    struct Add {
        constexpr inline T operator()(T lhs, T rhs) const {
            return lhs + rhs;
        }
    };

    template <typename T>
    struct And {
        constexpr inline T operator()(T lhs, T rhs) const {
            return lhs & rhs;
        }
    };

    template <typename T>
    struct Or {
        constexpr inline T operator()(T lhs, T rhs) const {
            return lhs | rhs;
        }
    };

    template <typename T>
    struct Multiplexer2 {
        constexpr inline word_ operator()(bool control, T src1, T src2) const {
            return control ? src2 : src1;
        }
    };

    template <typename T>
    struct Multiplexer3 {
        constexpr inline word_ operator()(byte_ control, T src1, T src2, T src3) const {
            if (control == 0) {
                return src1;
            }
            if (control == 1) {
                return src2;
            }
            if (control == 2) {
                return src3;
            }
            throw std::logic_error("invalid control signal: " + std::to_string(control));
        }
    };
}

#endif //RISC_V_SIMULATOR_GATES_H
