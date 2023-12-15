#include "GraphCopyHelper.h"


namespace ir {
/* static */
Graph *GraphCopyHelper::CreateCopy(const Graph *source, Graph *copyTarget) {
    ASSERT((source) && (copyTarget) && copyTarget->IsEmpty());
    GraphCopyHelper helper(source, copyTarget);
    helper.dfoCopy(source->GetFirstBasicBlock());
    ASSERT(helper.target->GetBasicBlocksCount() == helper.source->GetBasicBlocksCount());
    helper.fixDFG();
    // TODO: run GraphChecker on this class
    return copyTarget;
}

void GraphCopyHelper::dfoCopy(const BasicBlock *currentBBlock) {
    ASSERT((currentBBlock) && !visited.contains(currentBBlock->GetId()));

    auto *bblockCopy = currentBBlock->Copy(target, translationHelper);
    translationHelper.blocksToCopy.insert({currentBBlock->GetId(), bblockCopy});

    if (currentBBlock == source->GetFirstBasicBlock()) {
        target->SetFirstBasicBlock(bblockCopy);
    }
    if (currentBBlock == source->GetLastBasicBlock()) {
        target->SetLastBasicBlock(bblockCopy);
    }
    visited.insert({currentBBlock->GetId(), bblockCopy});

    for (const auto *succ : currentBBlock->GetSuccessors()) {
        auto succCopyIter = visited.find(succ->GetId());
        if (succCopyIter != visited.end()) {
            // basic block was already visited
            target->ConnectBasicBlocks(bblockCopy, succCopyIter->second);
        } else {
            // visit basic block and attach the created copy with its predecessor
            dfoCopy(succ);
            target->ConnectBasicBlocks(bblockCopy, visited.at(succ->GetId()));
        }
    }
}

void GraphCopyHelper::fixDFG() {
    ASSERT(translationHelper.Verify(target->CountInstructions()));
    target->ForEachBasicBlock([&translation = std::as_const(translationHelper)](BasicBlock *bblock) {
        ASSERT(bblock);
        std::for_each(bblock->begin(), bblock->end(), [&translation](InstructionBase *instr) {
            const auto *origInstr = translation.ToOrig(instr);
            // set correct inputs
            if (origInstr->IsCall()) {
                ASSERT(instr->IsCall());
                auto *callCopy = static_cast<CallInstruction *>(instr);
                const auto *callOrig = static_cast<const CallInstruction *>(origInstr);
                for (size_t i = 0, end = callOrig->GetInputsCount(); i < end; ++i) {
                    callCopy->AddInput(translation.ToCopy(callOrig->GetInput(i)));
                }
            } else if (origInstr->IsPhi()) {
                ASSERT(instr->IsPhi());
                auto *phiCopy = instr->AsPhi();
                const auto *phiOrig = origInstr->AsPhi();
                for (size_t i = 0, end = phiOrig->GetInputsCount(); i < end; ++i) {
                    phiCopy->AddPhiInput(translation.ToCopy(phiOrig->GetInput(i)),
                                         translation.ToCopy(phiOrig->GetSourceBasicBlock(i)));
                }
            } else if (origInstr->HasInputs()) {
                ASSERT(instr->HasInputs());
                auto *inputsCopy = instr->AsInputsInstruction();
                const auto *inputsOrig = static_cast<const InputsInstruction *>(origInstr);
                for (size_t i = 0, end = inputsOrig->GetInputsCount(); i < end; ++i) {
                    inputsCopy->SetInput(translation.ToCopy(inputsOrig->GetInput(i)), i);
                }
            }
        });
    });
}
}   // namespace ir
