#include "ConstantFolding.h"
#include "InstructionBuilder.h"


namespace ir {
bool ConstantFolding::ProcessAND(BinaryRegInstruction *instr) {
    ASSERT((instr) && (instr->GetOpcode() == Opcode::AND));
    auto input1 = instr->GetInput(0);
    auto input2 = instr->GetInput(1);
    if (input1->IsConst() && input2->IsConst()) {
        auto value = input1->AsConst()->GetValue() & input2->AsConst()->GetValue();
        auto *newInstr = getInstructionBuilder(instr)->CreateCONST(instr->GetType(), value);

        input1->RemoveUser(instr);
        input2->RemoveUser(instr);
        ReplaceWithConst(instr, newInstr);

        return true;
    }
    return false;
}

bool ConstantFolding::ProcessSRA(BinaryRegInstruction *instr) {
    ASSERT((instr) && (instr->GetOpcode() == Opcode::SRA));
    auto input1 = instr->GetInput(0);
    auto input2 = instr->GetInput(1);
    if (input1->IsConst() && input2->IsConst()) {
        auto value = ToSigned(input1->AsConst()->GetValue(), instr->GetType()) >>\
            input2->AsConst()->GetValue();
        auto *newInstr = getInstructionBuilder(instr)->CreateCONST(instr->GetType(), value);

        input1->RemoveUser(instr);
        input2->RemoveUser(instr);
        ReplaceWithConst(instr, newInstr);

        return true;
    }
    return false;
}

bool ConstantFolding::ProcessSUB(BinaryRegInstruction *instr) {
    ASSERT((instr) && (instr->GetOpcode() == Opcode::SUB));
    auto input1 = instr->GetInput(0);
    auto input2 = instr->GetInput(1);
    if (input1->IsConst() && input2->IsConst()) {
        auto value = input1->AsConst()->GetValue() - input2->AsConst()->GetValue();
        auto *newInstr = getInstructionBuilder(instr)->CreateCONST(instr->GetType(), value);

        input1->RemoveUser(instr);
        input2->RemoveUser(instr);
        ReplaceWithConst(instr, newInstr);

        return true;
    }
    return false;
}

/* static */
void ConstantFolding::ReplaceWithConst(InstructionBase *instr, ConstantInstruction *targetConst) {
    ASSERT((instr) && (targetConst));
    instr->ReplaceInputInUsers(targetConst);
    auto *bblock = instr->GetBasicBlock();
    bblock->UnlinkInstruction(instr);
    ASSERT(bblock->GetGraph()->GetFirstBasicBlock());
    bblock->GetGraph()->GetFirstBasicBlock()->PushBackInstruction(targetConst);
}

/* static */
InstructionBuilder *ConstantFolding::getInstructionBuilder(InstructionBase *instr) {
    return instr->GetBasicBlock()->GetGraph()->GetInstructionBuilder();
}
}   // namespace ir
