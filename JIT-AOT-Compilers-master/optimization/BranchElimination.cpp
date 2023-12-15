#include <algorithm>
#include "BranchElimination.h"
#include "GraphChecker.h"


namespace ir {
// TODO: can remove more branches,
// e.g. falling under the same conditions as previously encountered
bool BranchElimination::Run() {
    bool domTreeValid = graph->IsAnalysisValid(AnalysisFlag::DOM_TREE);

    bool removed = propagateConstsInCond();
    if (!removed) {
        return false;
    }

    auto liveMarker = doMarkPhase();

    // remove not marked blocks
    graph->ForEachBasicBlock([liveMarker, domTreeValid, this](BasicBlock *b) {
        if (!b->IsMarkerSet(liveMarker)) {
            GetLogger(utils::LogPriority::INFO) << "Removing block #" << b->GetId();
            removeUnreachable(b, liveMarker, domTreeValid);
        }
    });

    postElimination(domTreeValid);
    graph->ReleaseMarker(liveMarker);
    return true;
}

bool BranchElimination::propagateConstsInCond() {
    bool removed = false;
    graph->ForEachBasicBlock([this, &removed](BasicBlock *bblock) {
        auto *cmp = bblock->EndsWithConditionalJump();
        if (cmp != nullptr) {
            auto eval = evaluateComparison(cmp);
            if (eval != CmpResult::UNDEFINED) {
                removeEdge(cmp, static_cast<bool>(eval));
                removed = true;

                cmp->Dump(GetLoggerRaw()) << "\twas evaluated to " << static_cast<bool>(eval);
            }
        }
    });
    return removed;
}

Marker BranchElimination::doMarkPhase() {
    // do RPO and mark reachable blocks
    auto liveMarker = graph->GetNewMarker();
    std::pmr::vector<BasicBlock *> rpo(graph->GetMemoryResource());
    rpo.reserve(graph->GetBasicBlocksCount());

    auto callback = [&rpo, liveMarker](BasicBlock *bblock){
        rpo.push_back(bblock);
        bblock->SetMarker(liveMarker);
    };
    DFO::Traverse<Graph, decltype(callback), false>(graph, callback);
    ASSERT(rpo.size() < graph->GetBasicBlocksCount());

    // reset RPO
    std::reverse(rpo.begin(), rpo.end());
    graph->SetRPO(std::move(rpo));

    return liveMarker;
}

CmpResult BranchElimination::evaluateComparison(CompareInstruction *instr) {
    ASSERT(instr);
    auto condCode = instr->GetCondCode();
    auto input0 = instr->GetInput(0);
    auto input1 = instr->GetInput(1);
    if (input0 == input1) {
        if (condCode == CondCode::GE || condCode == CondCode::EQ) {
            return CmpResult::TRUE;
        }
        return CmpResult::FALSE;
    }
    if (input0->IsConst() && input1->IsConst()) {
        return compare(condCode, input0->AsConst()->GetValue(), input1->AsConst()->GetValue())
            ? CmpResult::TRUE : CmpResult::FALSE;
    }
    return CmpResult::UNDEFINED;
}

void BranchElimination::removeEdge(CompareInstruction *instr, bool cmpRes) {
    ASSERT(instr);
    auto *nextInstr = instr->GetNextInstruction();
    ASSERT((nextInstr) && nextInstr->GetOpcode() == Opcode::JCMP);
    auto *jcmp = static_cast<CondJumpInstruction *>(nextInstr);

    auto *bblock = jcmp->GetBasicBlock();
    auto *removeBlock = jcmp->GetDestination(!cmpRes);

    ASSERT(removeBlock->GetPredecessorsCount() > 0);
    bblock->UnlinkInstruction(jcmp);
    bblock->UnlinkInstruction(instr);

    graph->DisconnectBasicBlocks(bblock, removeBlock);
}

/* static */
void BranchElimination::removeUnreachable(BasicBlock *bblock, Marker liveMarker, bool fixDomTree) {
    ASSERT(bblock);
    // check unreachability conditions for blocks after mark stage
    ASSERT(std::all_of(
        bblock->GetPredecessors().begin(),
        bblock->GetPredecessors().end(),
        [liveMarker](const BasicBlock* b){ return !b->IsMarkerSet(liveMarker); }));

    for (auto *succ : bblock->GetSuccessors()) {
        ASSERT(succ);
        if (succ->IsMarkerSet(liveMarker)) {
            bblock->GetGraph()->DisconnectBasicBlocks(bblock, succ);
        }
    }

    if (fixDomTree) {
        auto *dominator = bblock->GetDominator();
        ASSERT(dominator);
        if (dominator->IsMarkerSet(liveMarker)) {
            dominator->RemoveDominatedBlock(bblock);
        }
    }

    bblock->GetGraph()->UnlinkBasicBlockRaw(bblock);
}

void BranchElimination::postElimination(bool domTreeValid) {
    ASSERT(PassManager::Run<GraphChecker>(graph));

    PassManager::SetInvalid<AnalysisFlag::LOOP_ANALYSIS>(graph);
    graph->SetAnalysisValid<AnalysisFlag::DOM_TREE>(domTreeValid);
    graph->SetAnalysisValid<AnalysisFlag::RPO>(true);
}
}   // namespace ir
