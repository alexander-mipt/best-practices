#ifndef JIT_AOT_COMPILERS_COURSE_REG_MAP_H_
#define JIT_AOT_COMPILERS_COURSE_REG_MAP_H_

#include "macros.h"


namespace ir {
class RegMap {
private:
    using MapType = unsigned long long;

public:
    static constexpr size_t MAX_REGS_COUNT = sizeof(MapType) * 8;

    // a free register is denoted by 1-bit
    RegMap(size_t regsCount)
        : regsCount(regsCount),
          map(static_cast<MapType>(0) - 1),
          fullMap(regsCount >= MAX_REGS_COUNT ? 0 : ~((1 << regsCount) - 1))
    {
        ASSERT(regsCount <= MAX_REGS_COUNT);
    }
    RegMap() = delete;
    DEFAULT_COPY_SEMANTIC(RegMap);
    DEFAULT_MOVE_SEMANTIC(RegMap);
    virtual DEFAULT_DTOR(RegMap);

    bool IsFull() const {
        return map == fullMap;
    }

    int AcquireReg() {
        // __builtin_ffsll
        // Returns one plus the index of the least significant 1-bit of x, or if x is zero, returns zero.
        int reg = __builtin_ffsll(map) - 1;
        if (reg >= 0 && reg < regsCount) {
            // mark the acquired reg with 0-bit
            map &= ~(1 << reg);
        }
        return reg;
    }

    void ReleaseReg(int reg) {
        ASSERT(reg >= 0 && reg < regsCount);
        // mark the released reg with 1-bit
        map |= 1 << reg;
    }

private:
    int_fast32_t regsCount;
    MapType map;
    const MapType fullMap;
};
}   // namespace ir

#endif  // JIT_AOT_COMPILERS_COURSE_REG_MAP_H_
