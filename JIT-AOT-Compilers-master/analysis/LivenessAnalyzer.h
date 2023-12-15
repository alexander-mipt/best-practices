#ifndef JIT_AOT_COMPILERS_COURSE_LIVENESS_ANALYZER_H_
#define JIT_AOT_COMPILERS_COURSE_LIVENESS_ANALYZER_H_

#include <list>
#include "LiveAnalysisStructs.h"
#include "PassBase.h"


namespace ir {
class LivenessAnalyzer : public PassBase {
public:
    explicit LivenessAnalyzer(Graph *graph)
        : PassBase(graph),
          linearOrderedBlocks(graph->GetMemoryResource())
    {}
    NO_COPY_SEMANTIC(LivenessAnalyzer);
    NO_MOVE_SEMANTIC(LivenessAnalyzer);
    ~LivenessAnalyzer() noexcept override = default;

    bool Run() override;

private:
    void resetStructs();

    void initBlocksInfo();

    // Globally orderes instructions in linear order.
    LiveRange::RangeType orderInstructions(BasicBlock *bblock);

    void calculateLiveRanges(BasicBlock::IdType blockId);
    void calculateInitialLiveSet(BasicBlock *bblock, BlockInfo &info) const;

    BlockInfo &getBlockInfo(BasicBlock *bblock) {
        ASSERT((bblock) && bblock->GetId() < linearOrderedBlocks.size());
        auto &res = linearOrderedBlocks[bblock->GetId()];
        return res;
    }
    const BlockInfo &getBlockInfo(const BasicBlock *bblock) const {
        ASSERT((bblock) && bblock->GetId() < linearOrderedBlocks.size());
        auto &res = linearOrderedBlocks[bblock->GetId()];
        return res;
    }

private:
    std::pmr::vector<BlockInfo> linearOrderedBlocks;

    LiveRange::RangeType rangeBegin = 0;
};
}   // namespace ir

#endif  // JIT_AOT_COMPILERS_COURSE_LIVENESS_ANALYZER_H_
