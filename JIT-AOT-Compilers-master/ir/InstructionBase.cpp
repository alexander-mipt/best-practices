#include <array>
#include "BasicBlock.h"


namespace ir {
Input InstructionBase::ToInput() {
    return {this};
}

ConstantInstruction *InstructionBase::AsConst() {
    ASSERT(IsConst());
    return static_cast<ConstantInstruction *>(this);
}

const ConstantInstruction *InstructionBase::AsConst() const {
    ASSERT(IsConst());
    return static_cast<const ConstantInstruction *>(this);
}

PhiInstruction *InstructionBase::AsPhi() {
    ASSERT(IsPhi());
    return static_cast<PhiInstruction *>(this);
}

const PhiInstruction *InstructionBase::AsPhi() const {
    ASSERT(IsPhi());
    return static_cast<const PhiInstruction *>(this);
}

InputsInstruction *InstructionBase::AsInputsInstruction() {
    ASSERT(HasInputs());
    return static_cast<InputsInstruction *>(this);
}
const InputsInstruction *InstructionBase::AsInputsInstruction() const {
    ASSERT(HasInputs());
    return static_cast<const InputsInstruction *>(this);
}

bool InstructionBase::Dominates(const InstructionBase *other) const {
    ASSERT(other);
    if (other == this) {
        // instruction always dominates itself
        return true;
    }
    auto *otherBlock = other->GetBasicBlock();
    if (otherBlock == GetBasicBlock()) {
        return isEarlierInBasicBlock(other);
    }
    return GetBasicBlock()->Dominates(otherBlock);
}

bool InstructionBase::isEarlierInBasicBlock(const InstructionBase *other) const {
    ASSERT((other) && (GetBasicBlock()) && other->GetBasicBlock() == GetBasicBlock());
    for (auto *instr : *GetBasicBlock()) {
        if (instr == this) {
            return true;
        }
        if (instr == other) {
            return false;
        }
    }
    UNREACHABLE("must have encountered either this or other instruction");
    return false;
}

void InstructionBase::UnlinkFromParent() {
    ASSERT(parent);
    parent->UnlinkInstruction(this);
}

void InstructionBase::InsertBefore(InstructionBase *before) {
    ASSERT((before) && (before->GetBasicBlock()));
    before->GetBasicBlock()->InsertBefore(before, this);
}

void InstructionBase::InsertAfter(InstructionBase *after) {
    ASSERT((after) && (after->GetBasicBlock()));
    after->GetBasicBlock()->InsertAfter(after, this);
}

void InstructionBase::ReplaceInputInUsers(InstructionBase *newInput) {
    newInput->AddUsers(GetUsers());
    for (auto &it : GetUsers()) {
        ASSERT(it->HasInputs());
        auto *typed = static_cast<InputsInstruction *>(it);
        typed->ReplaceInput(this, newInput);
    }
}
}   // namespace ir
