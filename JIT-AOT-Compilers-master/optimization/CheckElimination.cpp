#include "CheckElimination.h"
#include "DomTree.h"
#include "GraphChecker.h"
#include "Traversals.h"


namespace ir {
bool CheckElimination::Run() {
    PassManager::Run<RPO>(graph);
    PassManager::Run<DomTreeBuilder>(graph);

    bool removed = false;
    for (auto *bblock : graph->GetRPO()) {
        for (auto *current : bblock->IterateNonPhi()) {
            removed |= tryRemoveCheck(current);
        }
    }

    if (removed) {
        ASSERT(PassManager::Run<GraphChecker>(graph));
    }
    return removed;
}

bool CheckElimination::tryRemoveCheck(InstructionBase *instr) {
    ASSERT(instr);
    auto opcode = instr->GetOpcode();
    switch (opcode) {
    case Opcode::NULL_CHECK: {
        auto *inputsInstr = instr->AsInputsInstruction();
        ASSERT(inputsInstr->GetInputsCount() == 1);
        auto input = inputsInstr->GetInput(0);
        ASSERT((input.GetInstruction()) && input->GetType() == OperandType::REF);
        return singleInputCheckDominates(inputsInstr, input.GetInstruction());
    }
    case Opcode::ZERO_CHECK:
    case Opcode::NEGATIVE_CHECK:{
        auto *inputsInstr = instr->AsInputsInstruction();
        ASSERT(inputsInstr->GetInputsCount() == 1);
        auto input = inputsInstr->GetInput(0);
        ASSERT((input.GetInstruction()) && IsIntegerType(input->GetType()));
        return singleInputCheckDominates(inputsInstr, input.GetInstruction());
    }
    case Opcode::BOUNDS_CHECK: {
        auto *inputsInstr = instr->AsInputsInstruction();
        ASSERT(inputsInstr->GetInputsCount() == 2);
        auto ref = inputsInstr->GetInput(0);
        auto idx = inputsInstr->GetInput(1);
        ASSERT((ref.GetInstruction()) && ref->GetType() == OperandType::REF);
        ASSERT((idx.GetInstruction()) && IsIntegerType(idx->GetType()));
        return boundsCheckDominates(inputsInstr, ref.GetInstruction(), idx.GetInstruction());
    }
    default:
        return false;
    }
}

bool CheckElimination::singleInputCheckDominates(InputsInstruction *check,
                                                 InstructionBase *checkedValue)
{
    ASSERT((check) && (checkedValue) && check->GetInputsCount() == 1);
    auto opcode = check->GetOpcode();
    bool removed = false;
    for (auto *user : checkedValue->GetUsers()) {
        if (user != check && user->GetOpcode() == opcode && check->Dominates(user)) {
            user->GetBasicBlock()->UnlinkInstruction(user);
            GetLogger(utils::LogPriority::INFO) << "Removed redundant "
                                                << user->GetOpcodeName() << " #" << user->GetId();
            removed = true;
        }
    }
    return removed;
}

bool CheckElimination::boundsCheckDominates(InputsInstruction *check,
                                            InstructionBase *ref,
                                            InstructionBase *idx)
{
    ASSERT((check) && (ref) && (idx) && check->GetInputsCount() == 2);
    auto opcode = check->GetOpcode();
    bool removed = false;
    for (auto *user : ref->GetUsers()) {
        if (user != check && user->GetOpcode() == opcode && check->Dominates(user)) {
            auto *inputsInstr = user->AsInputsInstruction();
            ASSERT(inputsInstr->GetInput(0) == ref);
            if (inputsInstr->GetInput(1) == idx) {
                user->GetBasicBlock()->UnlinkInstruction(user);
                GetLogger(utils::LogPriority::INFO) << "Removed redundant "
                                                    << user->GetOpcodeName() << " #" << user->GetId();
                removed = true;
            }
        }
    }
    return removed;
}
};  // namespace ir
