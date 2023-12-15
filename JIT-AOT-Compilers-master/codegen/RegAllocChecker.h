#ifndef JIT_AOT_COMPILERS_COURSE_REG_ALLOC_CHECKER_H_
#define JIT_AOT_COMPILERS_COURSE_REG_ALLOC_CHECKER_H_

#include "PassBase.h"


namespace ir {
// TODO: implement one of general verification algorithms, e.g.:
// * Silvain Rideau, Xavier Leroy, "Validating Register Allocation and Spilling"
// * Gabriel Schörghuber, "Implementation of a Register Allocation Verifier for the Graal Compiler"
// * Yuqiang Huang, Bruce R. Childers, Mary Lou Soffa, "Catching and Identifying Bugs in Register Allocation"
class RegAllocChecker : public PassBase {
public:
    explicit RegAllocChecker(Graph *graph) : PassBase(graph) {}
    NO_COPY_SEMANTIC(RegAllocChecker);
    NO_MOVE_SEMANTIC(RegAllocChecker);
    ~RegAllocChecker() noexcept override = default;

    bool Run() override;

private:
    static void verifyPhiInputs(const PhiInstruction *phi);
};
}   // namespace ir

#endif  // JIT_AOT_COMPILERS_COURSE_REG_ALLOC_CHECKER_H_
