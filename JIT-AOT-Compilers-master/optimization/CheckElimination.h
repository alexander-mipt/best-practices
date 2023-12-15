#ifndef JIT_AOT_COMPILERS_COURSE_CHECK_ELIMINATION_H_
#define JIT_AOT_COMPILERS_COURSE_CHECK_ELIMINATION_H_

#include "PassBase.h"


namespace ir {
class CheckElimination : public PassBase, public utils::Logger {
public:
    explicit CheckElimination(Graph *graph)
        : PassBase(graph),
          utils::Logger(log4cpp::Category::getInstance(GetName()))
    {}
    ~CheckElimination() noexcept override = default;

    bool Run() override;

    const char *GetName() const {
        return PASS_NAME;
    }

private:
    static constexpr const char *PASS_NAME = "check_elimination";

private:
    bool tryRemoveCheck(InstructionBase *instr);
    bool singleInputCheckDominates(InputsInstruction *check, InstructionBase *checkedValue);
    bool boundsCheckDominates(InputsInstruction *check, InstructionBase *ref, InstructionBase *idx);
};
};  // namespace ir

#endif  // JIT_AOT_COMPILERS_COURSE_CHECK_ELIMINATION_H_
