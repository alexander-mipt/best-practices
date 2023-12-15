#ifndef JIT_AOT_COMPILERS_COURSE_LOOP_ANALYZER_H_
#define JIT_AOT_COMPILERS_COURSE_LOOP_ANALYZER_H_

#include "Loop.h"
#include "PassBase.h"
#include <vector>


namespace ir {
class LoopAnalyzer : public PassBase {
public:
    explicit LoopAnalyzer(Graph *graph)
        : PassBase(graph),
          dfsBlocks(graph->GetMemoryResource()),
          loops(graph->GetMemoryResource())
    {}
    NO_COPY_SEMANTIC(LoopAnalyzer);
    NO_MOVE_SEMANTIC(LoopAnalyzer);
    ~LoopAnalyzer() noexcept override = default;

    bool Run() override;

public:
    static constexpr AnalysisFlag SET_FLAG = AnalysisFlag::LOOP_ANALYSIS;

private:
    void resetStructs();

    void collectBackEdges();
    void populateLoops();
    void buildLoopTree();

    void dfsBackEdgesSearch(BasicBlock *bblock);
    void addLoopInfo(BasicBlock *header, BasicBlock *backEdgeSource);

    void populateReducibleLoop(Loop *loop);
    void dfsPopulateLoops(Loop *loop, BasicBlock *bblock);

    static bool isLoopIrreducible(const BasicBlock *header, const BasicBlock *backEdgeSource) {
        return !header->Dominates(backEdgeSource);
    }

private:
    Marker greyMarker;
    Marker blackMarker;

    size_t blockId = 0;
    std::pmr::vector<BasicBlock *> dfsBlocks;

    std::pmr::vector<Loop *> loops;
};
}   // namespace ir

#endif  // JIT_AOT_COMPILERS_COURSE_LOOP_ANALYZER_H_
