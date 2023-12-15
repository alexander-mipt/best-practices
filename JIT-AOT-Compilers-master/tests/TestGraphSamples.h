#ifndef JIT_AOT_COMPILERS_COURSE_TEST_GRAPH_SAMPLES_H_
#define JIT_AOT_COMPILERS_COURSE_TEST_GRAPH_SAMPLES_H_

#include "CompilerTestBase.h"
#include "LiveAnalysisStructs.h"


namespace ir::tests {
class TestGraphSamples : public CompilerTestBase {
public:
    using CFGInfoPair = std::pair<Graph *, std::vector<BasicBlock *>>;
    using LivenessInfoTuple = std::tuple<Graph *, std::vector<BasicBlock *>, std::pmr::vector<LiveInterval>>;

    CFGInfoPair BuildCase0();
    CFGInfoPair BuildCase1();
    CFGInfoPair BuildCase2();
    CFGInfoPair BuildCase3();
    CFGInfoPair BuildCase4();
    CFGInfoPair BuildCase5();

    LivenessInfoTuple FillCase1();
    LivenessInfoTuple FillCase2();
    LivenessInfoTuple FillCase4();
    LivenessInfoTuple FillCase5();
};
}   // namespace ir::tests

#endif  // JIT_AOT_COMPILERS_COURSE_TEST_GRAPH_SAMPLES_H_
