#ifndef JIT_AOT_COMPILERS_COURSE_DEFAULT_ARCH_H_
#define JIT_AOT_COMPILERS_COURSE_DEFAULT_ARCH_H_

#include "ArchInfoBase.h"


namespace ir::codegen {
class DefaultArch : public ArchInfoBase {
public:
    ~DefaultArch() noexcept override {
        if (instance != nullptr) {
            delete instance;
        }
    }

    static DefaultArch *GetInstance() {
        if (instance == nullptr) {
            instance = new DefaultArch();
        }
        return instance;
    }

    constexpr LocationIdType GetIntRegsCount() const override {
        return IntRegsCount;
    }
    constexpr LocationIdType GetFloatRegsCount() const override {
        return FloatRegsCount;
    }

public:
    static constexpr LocationIdType IntRegsCount = 31;
    static constexpr LocationIdType FloatRegsCount = 31;

private:
    static DefaultArch *instance;
};
}   // namespace ir::codegen

#endif  // JIT_AOT_COMPILERS_COURSE_DEFAULT_ARCH_H_
