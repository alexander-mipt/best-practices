#include <algorithm>
#include "LinearOrdering.h"
#include "LivenessAnalyzer.h"
#include "LoopAnalyzer.h"
#include <ranges>


namespace ir {
bool LivenessAnalyzer::Run() {
    PassManager::Run<LoopAnalyzer>(graph);
    PassManager::Run<LinearOrdering>(graph);

    resetStructs();
    initBlocksInfo();
    for (int i = linearOrderedBlocks.size() - 1; i >= 0; --i) {
        calculateLiveRanges(i);
    }
    return true;
}

void LivenessAnalyzer::resetStructs() {
    linearOrderedBlocks.reserve(graph->GetBasicBlocksCount());
    linearOrderedBlocks.clear();

    graph->GetLiveIntervals().Clear();

    rangeBegin = 0;
}

void LivenessAnalyzer::initBlocksInfo() {
    // graph's basic blocks are properly linearized after `LinearOrdering` pass
    graph->ForEachBasicBlock([this](BasicBlock *b) {
        auto rangeEnd = orderInstructions(b);
        linearOrderedBlocks.emplace_back(std::move(LiveRange{rangeBegin, rangeEnd}));
        rangeBegin = rangeEnd;
    });
    ASSERT(linearOrderedBlocks.size() == graph->GetBasicBlocksCount());
}

LiveRange::RangeType LivenessAnalyzer::orderInstructions(BasicBlock *bblock) {
    ASSERT(bblock);
    auto &liveIntervals = graph->GetLiveIntervals();
    auto range = rangeBegin;
    for (auto *instr : bblock->IteratePhi()) {
        liveIntervals.AddLiveInterval(range, instr);
    }
    for (auto *instr : bblock->IterateNonPhi()) {
        range += LiveInterval::LIVE_RANGE_STEP;
        liveIntervals.AddLiveInterval(range, instr);
    }
    return range + LiveInterval::LIVE_RANGE_STEP;
}

void LivenessAnalyzer::calculateLiveRanges(BasicBlock::IdType blockId) {
    auto *bblock = graph->FindBasicBlock(blockId);
    ASSERT(bblock);

    auto &liveIntervals = graph->GetLiveIntervals();
    auto &info = linearOrderedBlocks[blockId];
    auto blockRange = info.GetRange();
    auto &liveSet = info.GetLiveIn();

    calculateInitialLiveSet(bblock, info);

    for (auto *instr : liveSet) {
        liveIntervals.GetLiveIntervals(instr)->AddRange(blockRange);
    }

    auto blockRangeBegin = blockRange.GetBegin();
    // reverse order instructions
    for (auto *instr = bblock->GetLastInstruction();
         instr != nullptr && !instr->IsPhi();
         instr = instr->GetPrevInstruction())
    {
        auto *intervals = liveIntervals.GetLiveIntervals(instr);
        auto liveNumber = intervals->GetLiveNumber();
        intervals->SetBegin(liveNumber);
        liveSet.Remove(instr);

        if (!instr->HasInputs()) {
            continue;
        }
        auto *withInputs = instr->AsInputsInstruction();
        for (size_t i = 0, end = withInputs->GetInputsCount(); i < end; ++i) {
            auto *input = withInputs->GetInput(i).GetInstruction();
            liveSet.Add(input);
            liveIntervals.GetLiveIntervals(input)->AddRange(blockRangeBegin, liveNumber);
        }
    }

    for (auto *phi : bblock->IteratePhi()) {
        liveSet.Remove(phi);
    }

    if (bblock->IsLoopHeader()) {
        auto *loopInfo = bblock->GetLoop();
        ASSERT(loopInfo);
    }
}

void LivenessAnalyzer::calculateInitialLiveSet(BasicBlock *bblock, BlockInfo &info) const {
    auto &liveSet = info.GetLiveIn();
    // union of successors' liveIn sets + successors' corresponding edges' PHIs' inputs
    for (auto *succ : bblock->GetSuccessors()) {
        liveSet.Union(getBlockInfo(succ).GetLiveIn());
        for (auto *phi : succ->IteratePhi()) {
            liveSet.Add(phi->ResolveInput(bblock).GetInstruction());
        }
    }
}
}   // namespace ir
