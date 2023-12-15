#include <algorithm>
#include "Graph.h"


namespace ir {
bool Graph::VerifyFirstBlock() const {
#ifndef NDEBUG
    if (!firstBlock) {
        return true;
    }
    ASSERT(firstBlock->GetFirstPhiInstruction() == nullptr);
    ASSERT(firstBlock->GetLastPhiInstruction() == nullptr);
    bool foundConst = false;
    for (auto *instr : *firstBlock) {
        if (instr->IsConst()) {
            foundConst = true;
        } else if (instr->GetOpcode() == Opcode::ARG) {
            ASSERT(!foundConst);
        } else {
            UNREACHABLE("First basic block must contain only sequence of ARG and CONST instructions");
        }
    }
#endif  // NDEBUG
    return true;
}

size_t Graph::CountInstructions() const {
    size_t counter = 0;
    ForEachBasicBlock([&counter](const BasicBlock *bblock) { counter += bblock->GetSize(); });
    return counter;
}

BasicBlock *Graph::CreateEmptyBasicBlock(bool isTerminal) {
    auto *bblock = New<BasicBlock>(this);
    AddBasicBlock(bblock);
    if (isTerminal) {
        if (!GetLastBasicBlock()) {
            SetLastBasicBlock(CreateEmptyBasicBlock(false));
        }
        ConnectBasicBlocks(bblock, GetLastBasicBlock());
    }
    return bblock;
}

void Graph::ConnectBasicBlocks(BasicBlock *lhs, BasicBlock *rhs) {
    ASSERT((lhs) && (rhs));
    lhs->AddSuccessor(rhs);
    rhs->AddPredecessor(lhs);
    invalidateAfterChangedCFG();
}

void Graph::DisconnectBasicBlocks(BasicBlock *lhs, BasicBlock *rhs) {
    ASSERT((lhs) && (rhs));
    FixPHIAfterDisconnect(lhs, rhs);
    lhs->RemoveSuccessor(rhs);
    rhs->RemovePredecessor(lhs);
    invalidateAfterChangedCFG();
}

void Graph::InsertBetween(BasicBlock *bblock, BasicBlock *pred, BasicBlock *succ) {
    ASSERT((bblock) && (pred) && (succ));
    pred->ReplaceSuccessor(succ, bblock);
    bblock->AddPredecessor(pred);
    succ->ReplacePredecessor(pred, bblock);
    bblock->AddSuccessor(succ);

    // replace basic blocks in PHIs
    if (succ->GetFirstPhiInstruction()) {
        ASSERT(succ->GetPredecessorsCount() > 1);
        for (auto *phi : succ->IteratePhi()) {
            phi->ReplaceSourceBasicBlock(pred, bblock);
        }
    }
}

void Graph::FixPHIAfterDisconnect(BasicBlock *phiSource, BasicBlock *phiTarget) {
    ASSERT((phiSource) && (phiTarget) && phiTarget->HasPredecessor(phiSource));
    auto predsCount = phiTarget->GetPredecessorsCount();
    ASSERT(predsCount >= 1);
    if (predsCount == 2) {
        for (auto *phi : phiTarget->IteratePhi()) {
            ASSERT(phi->GetInputsCount() == 2);
            auto idx = phi->IndexOf(phiSource);
            ASSERT(idx <= 1);
            auto *remainingInput = phi->GetInput(1 - idx).GetInstruction();
            ASSERT(remainingInput);
            phi->ReplaceInputInUsers(remainingInput);
            phiTarget->UnlinkInstruction(phi);
        }
    } else if (predsCount > 2) {
        for (auto *phi : phiTarget->IteratePhi()) {
            phi->RemovePhiInput(phiSource);
        }
    }
}

void Graph::AddBasicBlock(BasicBlock *bblock) {
    ASSERT(bblock);
    bblock->SetId(bblocks.size());
    bblocks.push_back(bblock);
    bblock->SetGraph(this);
    invalidateAfterChangedCFG();
}

void Graph::AddBasicBlockBefore(BasicBlock *before, BasicBlock *bblock) {
    ASSERT((before) && (before->GetGraph() == this));
    ASSERT((bblock) && bblock->GetPredecessors().empty() && bblock->GetSuccessors().empty());

    bblock->SetGraph(this);
    for (auto *b : before->GetPredecessors()) {
        b->RemoveSuccessor(before);
        b->AddSuccessor(bblock);
        bblock->AddPredecessor(b);
    }
    before->GetPredecessors().clear();
    before->AddPredecessor(bblock);

    bblock->AddSuccessor(before);

    invalidateAfterChangedCFG();
}

void Graph::UnlinkBasicBlock(BasicBlock *bblock) {
    UnlinkBasicBlockRaw(bblock);
    if (bblock != GetLastBasicBlock()) {
        removePredecessors(bblock);
        removeSuccessors(bblock);
    } else {
#ifndef NDEBUG
        for (auto *pred : bblock->GetPredecessors()) {
            auto succs = pred->GetSuccessors();
            ASSERT(std::find(succs.begin(), succs.end(), bblock) == succs.end());
        }
        for (auto *succ : bblock->GetSuccessors()) {
            auto preds = succ->GetPredecessors();
            ASSERT(std::find(preds.begin(), preds.end(), bblock) == preds.end());
        }
#endif  // NDEBUG

        bblock->GetPredecessors().clear();
        bblock->GetSuccessors().clear();
    }
}

void Graph::UnlinkBasicBlockRaw(BasicBlock *bblock) {
    ASSERT((bblock) && bblock->GetGraph() == this);
    auto id = bblock->GetId();
    ASSERT(id < bblocks.size() && bblocks[id] == bblock);
    bblocks[id] = nullptr;
    bblock->SetId(BasicBlock::INVALID_ID);
    bblock->SetGraph(nullptr);
    ++unlinkedInstructionsCounter;

    invalidateAfterChangedCFG();
}

void Graph::RemoveUnlinkedBlocks() {
    UNREACHABLE("TBD");
}

void Graph::removePredecessors(BasicBlock *bblock) {
    for (auto *b : bblock->GetPredecessors()) {
        b->RemoveSuccessor(bblock);
    }
    bblock->GetPredecessors().clear();
}

void Graph::removeSuccessors(BasicBlock *bblock) {
    for (auto *b : bblock->GetSuccessors()) {
        FixPHIAfterDisconnect(bblock, b);
        b->RemovePredecessor(bblock);
    }
    bblock->GetSuccessors().clear();
}

void Graph::invalidateAfterChangedCFG() {
    SetAnalysisValid<AnalysisFlag::DOM_TREE>(false);
    SetAnalysisValid<AnalysisFlag::LOOP_ANALYSIS>(false);
    SetAnalysisValid<AnalysisFlag::RPO>(false);
    SetAnalysisValid<AnalysisFlag::LINEAR_ORDERING>(false);
}
}   // namespace ir
