#ifndef JIT_AOT_COMPILERS_COURSE_GRAPH_CHECKER_H_
#define JIT_AOT_COMPILERS_COURSE_GRAPH_CHECKER_H_

#include "PassBase.h"
#include "Traversals.h"
#include <vector>


namespace ir {
class GraphChecker : public PassBase {
public:
    explicit GraphChecker(Graph *graph) : PassBase(graph) {}
    NO_COPY_SEMANTIC(GraphChecker);
    NO_MOVE_SEMANTIC(GraphChecker);
    ~GraphChecker() noexcept override = default;

    bool Run() override;

    static void VerifyDomTree(Graph *graph);
    static void VerifyControlAndDataFlowGraphs(const BasicBlock *bblock);
    static void VerifyPhiBasicBlocks(const PhiInstruction *phi);

private:
    static bool sameBasicBlocks(const std::pmr::vector<BasicBlock *> &lhs,
                                const std::pmr::vector<BasicBlock *> &rhs);

    static void verifyBlockStructure(const BasicBlock *bblock, const InstructionBase *instr);
    static void verifyDFG(const InstructionBase *instr);
};
}   // namespace ir

#endif  // JIT_AOT_COMPILERS_COURSE_GRAPH_CHECKER_H_
