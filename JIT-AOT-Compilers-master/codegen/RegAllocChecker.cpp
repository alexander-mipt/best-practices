#include "GraphChecker.h"
#include "LiveAnalysisStructs.h"
#include "RegAllocChecker.h"


namespace ir {
bool RegAllocChecker::Run() {
    graph->ForEachBasicBlock([](const BasicBlock *b){
        for (const auto *phi : b->IteratePhi()) {
            verifyPhiInputs(phi);
        }
    });
    return true;
}

/* static */
void RegAllocChecker::verifyPhiInputs(const PhiInstruction *phi) {
    ASSERT(phi);
    auto &liveIntervals = phi->GetBasicBlock()->GetGraph()->GetLiveIntervals();

    GraphChecker::VerifyPhiBasicBlocks(phi);
    const auto &loc = liveIntervals.GetLiveIntervals(phi)->GetLocation();
    for (size_t i = 0, end = phi->GetInputsCount(); i < end; ++i) {
        const auto *input = phi->GetInput(i).GetInstruction();
        ASSERT(liveIntervals.GetLiveIntervals(input)->GetLocation() == loc);
    }
}
}   // namespace ir
