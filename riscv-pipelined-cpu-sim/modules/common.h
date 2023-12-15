#ifndef RISC_V_SIMULATOR_COMMON_H
#define RISC_V_SIMULATOR_COMMON_H

#include <cstdint>

#ifdef DEBUG
#define DEBUG_LOG(obj) std::cout << std::dec << __LINE__ << std::hex << ": "; obj.debug()
#else
#define DEBUG_LOG(obj)
#endif

namespace modules {

    using byte_ = uint8_t;
    using byte2_ = uint16_t;
    using word_ = uint32_t;

    constexpr inline word_ signExtendFromByte(byte_ target) {
        if (target >> 7) {
            return static_cast<word_>(target) | 0xffffff00;
        }
        return static_cast<word_>(target);
    }

    constexpr inline word_ signExtendFromByte2(byte2_ target) {
        if (target >> 15) {
            return static_cast<word_>(target) | 0xffff0000;
        }
        return static_cast<word_>(target);
    }
}

#endif //RISC_V_SIMULATOR_COMMON_H
