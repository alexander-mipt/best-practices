#ifndef RISC_V_SIMULATOR_HAZARD_UNIT_H
#define RISC_V_SIMULATOR_HAZARD_UNIT_H

#include "instruction.h"
#include "../modules/common.h"


namespace pipeline {

    enum class BypassOptionsEncoding : modules::byte_ {
        REG,
        MEM,
        WB
    };

    inline std::ostream& operator<<(std::ostream& os, const BypassOptionsEncoding& enc) {
        if (enc == BypassOptionsEncoding::REG) {
            os << "REG";
        } else if (enc == BypassOptionsEncoding::MEM) {
            os << "MEM";
        } else {
            os << "WB";
        }
        return os;
    }
}

#endif //RISC_V_SIMULATOR_HAZARD_UNIT_H
