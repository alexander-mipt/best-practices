#ifndef JIT_AOT_COMPILERS_COURSE_BRANCH_ELIMINATION_H_
#define JIT_AOT_COMPILERS_COURSE_BRANCH_ELIMINATION_H_

#include "PassBase.h"


namespace ir {
enum class CmpResult : uint8_t {
    FALSE = 0,
    TRUE,
    UNDEFINED
};

class BranchElimination : public PassBase, public utils::Logger {
public:
    explicit BranchElimination(Graph *graph)
        : PassBase(graph),
          utils::Logger(log4cpp::Category::getInstance(GetName()))
    {}
    ~BranchElimination() noexcept override = default;

    bool Run() override;

    const char *GetName() const {
        return PASS_NAME;
    }

private:
    static void removeUnreachable(BasicBlock *bblock, Marker liveMarker, bool fixDomTree);

    bool propagateConstsInCond();
    CmpResult evaluateComparison(CompareInstruction *instr);
    void removeEdge(CompareInstruction *instr, bool cmpRes);

    Marker doMarkPhase();

    void postElimination(bool domTreeValid);

private:
    static constexpr const char *PASS_NAME = "branch_elimination";
};
}   // namespace ir

#endif  // JIT_AOT_COMPILERS_COURSE_BRANCH_ELIMINATION_H_
