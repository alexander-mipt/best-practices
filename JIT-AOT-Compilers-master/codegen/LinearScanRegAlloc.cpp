#include "InstructionBuilder.h"
#include "LinearScanRegAlloc.h"
#include "LivenessAnalyzer.h"
#include "Loop.h"
#include "RegAllocChecker.h"
#include "Traversals.h"


namespace ir::codegen {
bool LinearScanRegAlloc::Run() {
    PassManager::Run<LivenessAnalyzer>(graph);

    allocateAndAssignRegs();
    auto insertedNew = fixPhis();
    if (insertedNew) {
        postAlloc();
    }
    ASSERT(PassManager::Run<RegAllocChecker>(graph));
    return true;
}

void LinearScanRegAlloc::allocateAndAssignRegs() {
    // live intervals are sorted in ascending order
    for (auto *intervals : graph->GetLiveIntervals()) {
        expireOldIntervals(intervals);
        if (isFull()) {
            spillAtInterval(intervals);
        } else {
            assignToReg(intervals);
            addActiveInterval(intervals);
        }
    }
}

void LinearScanRegAlloc::expireOldIntervals(const LiveInterval *intervals) {
    ASSERT(intervals);
    for (auto iter = active.begin(), end = active.end(); iter != end;) {
        auto *liveInterval = *iter;
        ASSERT(liveInterval);
        if (liveInterval->GetEnd() > intervals->GetBegin()) {
            return;
        }
        regMap.ReleaseReg(liveInterval->GetLocation().GetId());
        iter = active.erase(iter);
    }
    ASSERT(intervals);
}

void LinearScanRegAlloc::spillAtInterval(LiveInterval *intervals) {
    ASSERT(intervals);
    auto *spilled = active.back();
    ASSERT(spilled);
    if (spilled->GetEnd() < intervals->GetEnd()) {
        spillToStack(intervals);
    } else {
        auto reg = spillToStack(spilled);
        ASSERT(reg.GetType() == LocationType::REGISTER);
        intervals->SetLocation(reg);
        active.pop_back();
    }
}

void LinearScanRegAlloc::addActiveInterval(LiveInterval *intervals) {
    ASSERT(intervals);
    ASSERT(!isFull());
    auto liveEnd = intervals->GetEnd();
    auto iter = std::find_if(active.begin(), active.end(), [liveEnd](LiveInterval *i) {
        return liveEnd <= i->GetEnd();
    });
    active.insert(iter, intervals);
}

ValueLocation LinearScanRegAlloc::spillToStack(LiveInterval *intervals) {
    ASSERT(intervals);
    auto prevLoc = intervals->GetLocation();
    intervals->SetLocation({LocationType::STACK, getNextSlot()});
    return prevLoc;
}

void LinearScanRegAlloc::assignToReg(LiveInterval *intervals) {
    ASSERT(intervals);
    int reg = regMap.AcquireReg();
    ASSERT(reg >= 0);
    intervals->SetLocation({LocationType::REGISTER, static_cast<LocationIdType>(reg)});
}

bool LinearScanRegAlloc::fixPhis() {
    auto insertedNew = false;
    graph->ForEachBasicBlock([this, &insertedNew](BasicBlock *b){
        if (b->GetFirstPhiInstruction()) {
            insertedNew |= resolvePhis(b);
        }
    });
    return insertedNew;
}

bool LinearScanRegAlloc::resolvePhis(BasicBlock *bblock) {
    ASSERT((bblock) && bblock->GetFirstPhiInstruction());
    auto &predecessors = bblock->GetPredecessors();
    ASSERT(predecessors.size() > 1);

    auto insertedNew = false;
    for (auto *pred : bblock->GetPredecessors()) {
        ASSERT(pred);

        BasicBlock *insertInto = pred;
        if (isCriticalEdge(bblock, pred)) {
            insertInto = nullptr;
        }

        auto &liveIntervals = graph->GetLiveIntervals();
        for (auto *phi : bblock->IteratePhi()) {
            const auto *phiInfo = liveIntervals.GetLiveIntervals(phi);
            auto idx = phi->IndexOf(pred);
            auto *input = phi->GetInput(idx).GetInstruction();
            const auto *inputInfo = liveIntervals.GetLiveIntervals(input);
            ASSERT((phiInfo) && (inputInfo));

            if (inputInfo->GetLocation() == phiInfo->GetLocation()) {
                continue;
            }
            // must insert spill-fill when location are unequal
            auto *move = graph->GetInstructionBuilder()->CreateMOVE(input);

            if (insertInto == nullptr) {
                // this implementation inserts an empty basic block to break critical edges
                insertInto = breakCriticalEdge(bblock, pred);
                insertedNew = true;
            }
            insertInto->PushBackInstruction(move);

            phi->SetInput(move, idx);
            input->RemoveUser(phi);
            auto *liveInt = liveIntervals.AddLiveInterval(LiveRange::INVALID_RANGE, move);
            liveInt->SetLocation(phiInfo->GetLocation());
        }
    }
    return insertedNew;
}

BasicBlock *LinearScanRegAlloc::breakCriticalEdge(BasicBlock *bblock, BasicBlock *pred) {
    ASSERT((bblock) && (pred));
    auto *newBlock = graph->CreateEmptyBasicBlock();
    graph->InsertBetween(newBlock, pred, bblock);

    pred->GetLoop()->AddBasicBlock(newBlock);

    return newBlock;
}

void LinearScanRegAlloc::postAlloc() {
    PassManager::SetInvalid<
        AnalysisFlag::DOM_TREE,
        AnalysisFlag::RPO,
        AnalysisFlag::LINEAR_ORDERING>(graph);
}
}   // namespace ir::codegen
