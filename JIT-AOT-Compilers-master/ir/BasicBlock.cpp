#include "BasicBlock.h"
#include "Graph.h"
#include "GraphTranslationHelper.h"
#include "Loop.h"


namespace ir {
BasicBlock::BasicBlock(Graph *graph)
    : id(INVALID_ID),
      preds(graph->GetMemoryResource()),
      succs(graph->GetMemoryResource()),
      dominated(graph->GetMemoryResource()),
      graph(graph)
{}

bool BasicBlock::IsFirstInGraph() const {
    return GetGraph()->GetFirstBasicBlock() == this;
}

bool BasicBlock::IsLastInGraph() const {
    return GetGraph()->GetLastBasicBlock() == this;
}

// concepts and type helpers
template <typename T>
concept BasicBlockType = std::is_same_v<std::remove_cv_t<T>, BasicBlock>;

template <BasicBlockType BasicBlockT, typename T>
struct BasicBlockConstHelper {
    using type = const T *;
};

template <typename T>
struct BasicBlockConstHelper<BasicBlock, T> {
    using type = T *;
};

template <BasicBlockType BasicBlockT>
using CmpType = BasicBlockConstHelper<BasicBlockT, CompareInstruction>::type;

template <BasicBlockType BasicBlockT>
static CmpType<BasicBlockT> endsWithConditionalJump(BasicBlockT *bblock) {
    auto *instr = bblock->GetLastInstruction();
    if (instr == nullptr || instr->GetOpcode() != Opcode::JCMP) {
        return nullptr;
    }
    ASSERT(bblock->GetSuccessorsCount() == 2);
    instr = instr->GetPrevInstruction();
    ASSERT((instr) && instr->GetOpcode() == Opcode::CMP);
    return static_cast<CmpType<BasicBlockT>>(instr);
}

CompareInstruction *BasicBlock::EndsWithConditionalJump() {
    return endsWithConditionalJump(this);
}

const CompareInstruction *BasicBlock::EndsWithConditionalJump() const {
    return endsWithConditionalJump(this);
}

void BasicBlock::InverseConditionalBranch() {
    auto *cmp = EndsWithConditionalJump();
    ASSERT(cmp != nullptr);
    cmp->Inverse();
    std::tie(succs[0], succs[1]) = std::make_tuple(succs[1], succs[0]);
}

bool BasicBlock::IsLoopHeader() const {
    return (loop != nullptr) && loop->GetHeader() == this;
}

bool BasicBlock::Dominates(const BasicBlock *other) const {
    ASSERT(other);
    if (other == this) {
        // basic block always dominated itself
        return true;
    }
    auto *dom = other->GetDominator();
    while (dom != nullptr) {
        if (dom == this) {
            return true;
        }
        dom = dom->GetDominator();
    }
    return false;
}

void BasicBlock::RemoveDominatedBlock(BasicBlock *bblock) {
    ASSERT(bblock);
    auto it = std::find(dominated.begin(), dominated.end(), bblock);
    ASSERT(it != dominated.end());

    *it = dominated.back();
    dominated.pop_back();
}

void BasicBlock::AddPredecessor(BasicBlock *bblock) {
    ASSERT(bblock);
    ASSERT(std::find(preds.begin(), preds.end(), bblock) == preds.end());
    preds.push_back(bblock);
}

void BasicBlock::AddSuccessor(BasicBlock *bblock) {
    ASSERT(bblock);
    ASSERT(succs.size() < 2);
    ASSERT(std::find(succs.begin(), succs.end(), bblock) == succs.end());
    succs.push_back(bblock);
}

void BasicBlock::RemovePredecessor(BasicBlock *bblock) {
    ASSERT(bblock);
    auto it = std::find(preds.begin(), preds.end(), bblock);
    ASSERT(it != preds.end());

    *it = preds.back();
    preds.pop_back();
}

void BasicBlock::RemoveSuccessor(BasicBlock *bblock) {
    ASSERT(bblock);
    auto it = std::find(succs.begin(), succs.end(), bblock);
    ASSERT(it != succs.end());
    succs.erase(it);
}

void BasicBlock::ReplaceSuccessor(BasicBlock *prevSucc, BasicBlock *newSucc) {
    ASSERT((prevSucc) && (newSucc));
    auto it = std::find(succs.begin(), succs.end(), prevSucc);
    ASSERT(it != succs.end());
    *it = newSucc;
}

void BasicBlock::ReplacePredecessor(BasicBlock *prevPred, BasicBlock *newPred) {
    ASSERT((prevPred) && (newPred));
    auto it = std::find(preds.begin(), preds.end(), prevPred);
    ASSERT(it != preds.end());
    *it = newPred;
}

void BasicBlock::MoveConstantUnsafe(InstructionBase *instr) {
    ASSERT(IsFirstInGraph());
    ASSERT((instr) && instr->IsConst());
    instr->SetNextInstruction(nullptr);
    if (lastInst == nullptr) {
        ASSERT(firstInst == nullptr);
        instr->SetPrevInstruction(nullptr);
        firstInst = instr;
        lastInst = instr;
    } else {
        lastInst->SetNextInstruction(instr);
        instr->SetPrevInstruction(lastInst);
        lastInst = instr;
    }
    ++instrsCount;
}

template <bool PushBack>
void BasicBlock::pushInstruction(InstructionBase *instr) {
    ASSERT(instr);
    ASSERT(instr->GetBasicBlock() == nullptr);
    ASSERT(instr->GetPrevInstruction() == nullptr);
    instr->SetBasicBlock(this);

    if (instr->IsPhi()) {
        pushPhi(instr);
    } else if (firstInst == nullptr) {
        instr->SetPrevInstruction(lastPhi);
        firstInst = instr;
        lastInst = instr;
        if (lastPhi) {
            lastPhi->SetNextInstruction(instr);
        }
    } else {
        if constexpr (PushBack) {
            instr->SetPrevInstruction(lastInst);
            lastInst->SetNextInstruction(instr);
            lastInst = instr;
        } else {
            instr->SetNextInstruction(firstInst);
            firstInst->SetPrevInstruction(instr);
            firstInst = instr;
        }
    }
    ++instrsCount;
}

void BasicBlock::pushPhi(InstructionBase *instr) {
    ASSERT((instr) && instr->IsPhi());

    if (firstPhi == nullptr) {
        firstPhi = instr->AsPhi();
        lastPhi = firstPhi;
        lastPhi->SetNextInstruction(firstInst);
        if (firstInst) {
            firstInst->SetPrevInstruction(lastPhi);
        }
    } else {
        instr->SetNextInstruction(firstPhi);
        firstPhi->SetPrevInstruction(instr);
        firstPhi = instr->AsPhi();
    }
}

void BasicBlock::PushForwardInstruction(InstructionBase *instr) {
    pushInstruction<false>(instr);
}

void BasicBlock::PushBackInstruction(InstructionBase *instr) {
    pushInstruction<true>(instr);
}

void BasicBlock::InsertBefore(InstructionBase *before, InstructionBase *target) {
    // use PushBackInstruction/PushForwardInstruction for PHI instructions
    ASSERT((target) && (target->GetBasicBlock() == nullptr) && !target->IsPhi());
    ASSERT((before) && (before->GetBasicBlock() == this));
    target->SetBasicBlock(this);
    auto *prev = before->GetPrevInstruction();
    before->SetPrevInstruction(target);
    target->SetPrevInstruction(prev);
    target->SetNextInstruction(before);

    if (prev) {
        prev->SetNextInstruction(target);
    } else {
        ASSERT(before == firstInst && firstPhi == nullptr && lastPhi == nullptr);
        firstInst = target;
    }
    ++instrsCount;
}

void BasicBlock::InsertAfter(InstructionBase *after, InstructionBase *target) {
    ASSERT((target) && (target->GetBasicBlock() == nullptr) && !target->IsPhi());
    ASSERT((after) && (after->GetBasicBlock() == this));
    target->SetBasicBlock(this);
    auto *next = after->GetNextInstruction();
    after->SetNextInstruction(target);
    target->SetPrevInstruction(after);
    target->SetNextInstruction(next);

    if (next) {
        next->SetPrevInstruction(target);
    } else {
        ASSERT(lastInst == after);
        lastInst = target;
    }
    ++instrsCount;
}

// TODO: write unit test for this method
void BasicBlock::UnlinkInstruction(InstructionBase *target) {
    ASSERT((target) && (target->GetBasicBlock() == this));
    target->SetBasicBlock(nullptr);
    auto *prev = target->GetPrevInstruction();
    auto *next = target->GetNextInstruction();
    target->SetPrevInstruction(nullptr);
    target->SetNextInstruction(nullptr);

    // basic connection
    if (prev) {
        prev->SetNextInstruction(next);
    }
    if (next) {
        next->SetPrevInstruction(prev);
    }

    // update members of basic block
    if (target->IsPhi()) {
        if (target == firstPhi) {
            if (target == lastPhi) {
                firstPhi = nullptr;
                lastPhi = nullptr;
            } else {
                ASSERT((next) && next->IsPhi());
                firstPhi = next->AsPhi();
            }
        } else if (target == lastPhi) {
            ASSERT((prev) && prev->IsPhi());
            lastPhi = prev->AsPhi();
        }
    } else {
        if (target == firstInst) {
            firstInst = next;
        }
        if (target == lastInst) {
            lastInst = (prev && prev->IsPhi()) ? nullptr : prev;
        }
    }
    instrsCount -= 1;
}

void BasicBlock::ReplaceInstruction(InstructionBase *prevInstr, InstructionBase *newInstr) {
    prevInstr->ReplaceInputInUsers(newInstr);
    replaceInControlFlow(prevInstr, newInstr);
    // TODO: replace users in inputs of prevInstr to newInstr
}

void BasicBlock::replaceInControlFlow(InstructionBase *prevInstr, InstructionBase *newInstr) {
    // TODO: check case newInstr->IsPhi() -> true (with tests)
    ASSERT((prevInstr) && (prevInstr->GetBasicBlock() == this));
    if (prevInstr->GetPrevInstruction()) {
        InsertAfter(prevInstr->GetPrevInstruction(), newInstr);
    } else if (prevInstr->GetNextInstruction()) {
        InsertBefore(prevInstr->GetNextInstruction(), newInstr);
    } else {
        PushForwardInstruction(newInstr);
    }
    UnlinkInstruction(prevInstr);
}

void copyInstruction(
    BasicBlock *targetBlock,
    const InstructionBase *orig,
    GraphTranslationHelper &translationHelper)
{
    ASSERT((targetBlock) && (orig));
    auto *copy = orig->Copy(targetBlock);
    targetBlock->PushBackInstruction(copy);
    translationHelper.InsertInstructionsPair(orig, copy);
}

BasicBlock *BasicBlock::Copy(Graph *targetGraph, GraphTranslationHelper &translationHelper) const
{
    ASSERT(targetGraph);
    auto *result = targetGraph->CreateEmptyBasicBlock();
    if (!IsEmpty()) {
        for (const auto *instr : *this) {
            copyInstruction(result, instr, translationHelper);
        }
        ASSERT(result->GetFirstInstruction());
        ASSERT(result->GetLastInstruction());
    }
    return result;
}

BasicBlock *BasicBlock::SplitAfterInstruction(InstructionBase *instr, bool connectAfterSplit) {
    ASSERT((instr) && instr->GetBasicBlock() == this);
    auto *nextInstr = instr->GetNextInstruction();
    ASSERT(nextInstr);

    auto *graph = GetGraph();
    auto *newBBlock = graph->CreateEmptyBasicBlock();

    // might leave unconnected, e.g. for further usage in inlining
    if (connectAfterSplit) {
        graph->ConnectBasicBlocks(this, newBBlock);
    }

    if (GetLoop()) {
        GetLoop()->AddBasicBlock(newBBlock);
        newBBlock->SetLoop(GetLoop());
    }
    for (auto *succ : GetSuccessors()) {
        succ->RemovePredecessor(this);
        graph->ConnectBasicBlocks(newBBlock, succ);
    }
    succs.clear();

    instr->SetNextInstruction(nullptr);
    nextInstr->SetPrevInstruction(nullptr);
    for (auto *iter = nextInstr; iter != nullptr; iter = iter->GetNextInstruction()) {
        iter->SetBasicBlock(newBBlock);
        newBBlock->instrsCount += 1;
    }
    instrsCount -= newBBlock->instrsCount;

    if (nextInstr->IsPhi()) {
        ASSERT(instr->IsPhi());
        newBBlock->firstPhi = nextInstr->AsPhi();
        newBBlock->lastPhi = lastPhi;
        lastPhi = instr->AsPhi();
        // can be done unconditionally
        newBBlock->firstInst = firstInst;
        newBBlock->lastInst = lastInst;
        firstInst = nullptr;
        lastInst = nullptr;
    } else {
        newBBlock->firstInst = nextInstr;
        newBBlock->lastInst = lastInst;
        lastInst = instr;
    }

    return newBBlock;
}
}   // namespace ir
