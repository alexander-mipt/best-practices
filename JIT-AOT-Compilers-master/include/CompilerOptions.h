#ifndef JIT_AOT_COMPILERS_COURSE_COMPILER_OPTIONS_H_
#define JIT_AOT_COMPILERS_COURSE_COMPILER_OPTIONS_H_

#include "macros.h"


namespace ir {
#define PASS_OPTION(type, name, value)      \
public:                                     \
    type Get##name() const { return name; } \
    void Set##name(type New##name)  {       \
        name = New##name;                   \
    }                                       \
private:                                    \
    type name = value

class CompilerOptions final {
public:
    CompilerOptions() = default;
    NO_COPY_SEMANTIC(CompilerOptions);
    NO_MOVE_SEMANTIC(CompilerOptions);
    DEFAULT_DTOR(CompilerOptions);

    PASS_OPTION(size_t, MaxCalleeInstrs, 25);
    PASS_OPTION(size_t, MaxInstrsAfterInlining, 250);
};

#undef PASS_OPTION
}   // namespace ir

#endif  // JIT_AOT_COMPILERS_COURSE_COMPILER_OPTIONS_H_
