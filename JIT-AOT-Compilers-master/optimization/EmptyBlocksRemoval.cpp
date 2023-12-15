#include "EmptyBlocksRemoval.h"
#include "Traversals.h"


namespace ir {
/* static */
bool EmptyBlocksRemoval::RemoveIfEmpty(BasicBlock *bblock) {
    ASSERT(bblock);
    if (!bblock->IsEmpty() || bblock->IsFirstInGraph() || bblock->IsLastInGraph()) {
        return false;
    }
    BasicBlock *successorToSet = nullptr;
    if (!bblock->HasNoSuccessors()) {
        auto &succs = bblock->GetSuccessors();
        ASSERT(succs.size() == 1);
        successorToSet = succs[0];
        successorToSet->RemovePredecessor(bblock);

        if (successorToSet->GetFirstPhiInstruction()) {
            auto &preds = bblock->GetPredecessors();
            ASSERT(preds.size() == 1);
            // relink PHI source basic blocks if there are any
            for (auto *phi : successorToSet->IteratePhi()) {
                phi->ReplaceSourceBasicBlock(bblock, preds[0]);
            }
        }
    }
    for (auto *pred : bblock->GetPredecessors()) {
        ASSERT(pred);
        auto succs = pred->GetSuccessors();
        ASSERT(!succs.empty() && succs.size() <= 2);

        if (successorToSet == nullptr) {
            if (succs.size() == 2) {
                // branch can be removed, as it always taken / not taken
                auto *jcmp = pred->GetLastInstruction();
                ASSERT(jcmp && jcmp->IsBranch());
                auto *cmp = jcmp->GetPrevInstruction();
                ASSERT(cmp && cmp->GetOpcode() == Opcode::CMP);
                pred->UnlinkInstruction(jcmp);
                pred->UnlinkInstruction(cmp);
            }
            pred->RemoveSuccessor(bblock);
        } else {
            successorToSet->AddPredecessor(pred);
            pred->ReplaceSuccessor(bblock, successorToSet);
        }
    }
    bblock->GetPredecessors().clear();
    bblock->GetSuccessors().clear();
    bblock->GetGraph()->UnlinkBasicBlock(bblock);
    return true;
}

void EmptyBlocksRemoval::postRemoval() {
    PassManager::SetInvalid<
        AnalysisFlag::DOM_TREE,
        AnalysisFlag::LOOP_ANALYSIS,
        AnalysisFlag::RPO>(graph);
}
}   // namespace ir
