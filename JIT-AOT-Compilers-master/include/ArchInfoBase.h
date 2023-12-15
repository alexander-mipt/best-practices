#ifndef JIT_AOT_COMPILERS_COURSE_ARCH_INFO_BASE_H_
#define JIT_AOT_COMPILERS_COURSE_ARCH_INFO_BASE_H_

#include <limits>
#include "macros.h"


namespace ir::codegen {
using LocationIdType = uint16_t;
static constexpr LocationIdType MAX_REG_NUMBER = std::numeric_limits<LocationIdType>::max();

class ArchInfoBase {
public:
    NO_COPY_SEMANTIC(ArchInfoBase);
    NO_MOVE_SEMANTIC(ArchInfoBase);
    virtual DEFAULT_DTOR(ArchInfoBase);

    virtual constexpr LocationIdType GetIntRegsCount() const = 0;
    virtual constexpr LocationIdType GetFloatRegsCount() const = 0;

protected:
    ArchInfoBase() = default;
};
}   // namespace ir::codegen

#endif  // JIT_AOT_COMPILERS_COURSE_ARCH_INFO_BASE_H_
