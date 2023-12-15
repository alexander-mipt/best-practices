#ifndef RISC_V_SIMULATOR_CMP_H
#define RISC_V_SIMULATOR_CMP_H

#include "common.h"


namespace modules {

    enum class CmpControl : byte_ {
        NOP,
        EQ,
        NE,
        LT,
        GE,
        LTU,
        GEU
    };

    inline std::ostream& operator<<(std::ostream& os, const CmpControl& cmp_control) {
        switch (cmp_control) {
            case CmpControl::NOP:
                os << "NOP";
                break;
            case CmpControl::EQ:
                os << "EQ";
                break;
            case CmpControl::NE:
                os << "NE";
                break;
            case CmpControl::LT:
                os << "LT";
                break;
            case CmpControl::GE:
                os << "GE";
                break;
            case CmpControl::LTU:
                os << "LTU";
                break;
            case CmpControl::GEU:
                os << "GEU";
                break;
        }
        return os;
    }

    template <typename T>
    struct Cmp {
        inline bool operator()(CmpControl control, T lhs, T rhs) const {
            switch (control) {
                case CmpControl::NOP:
                    return false;
                case CmpControl::EQ:
                    return lhs == rhs;
                case CmpControl::NE:
                    return lhs != rhs;
                case CmpControl::LT:
                case CmpControl::LTU:
                    return lhs < rhs;
                case CmpControl::GE:
                case CmpControl::GEU:
                    return lhs >= rhs;
                default:
                    throw std::logic_error("unknown compare operation");
            }
        }
    };
}

#endif //RISC_V_SIMULATOR_CMP_H
