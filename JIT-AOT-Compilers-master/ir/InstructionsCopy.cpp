#include "InstructionBuilder.h"


namespace ir {
// implement Copy overrided methods for general classes
#define OVERRIDE_GENERAL_CLASS_COPY(name, ...)                                          \
name *name::Copy(BasicBlock *targetBBlock) const {                                      \
    auto *graph = targetBBlock->GetGraph();                                             \
    auto *instr = graph->template New<name>(__VA_ARGS__, graph->GetMemoryResource());   \
    graph->GetInstructionBuilder()->AttachInstruction(instr);                           \
    instr->SetProperty(GetProperties());                                                \
    return instr;                                                                       \
}

OVERRIDE_GENERAL_CLASS_COPY(BinaryImmInstruction, GetOpcode(), GetType(), nullptr, GetValue())
OVERRIDE_GENERAL_CLASS_COPY(BinaryRegInstruction, GetOpcode(), GetType(), nullptr, nullptr)
OVERRIDE_GENERAL_CLASS_COPY(UnaryRegInstruction, GetOpcode(), GetType(), nullptr)
OVERRIDE_GENERAL_CLASS_COPY(LoadImmInstruction, GetOpcode(), GetType(), nullptr, GetValue());
OVERRIDE_GENERAL_CLASS_COPY(StoreImmInstruction, GetOpcode(), nullptr, nullptr, GetValue());

#undef OVERRIDE_GENERAL_CLASS_COPY

// special instructions' classes implementations
#define OVERRIDE_COPY_METHOD_FIXED(name, opcode)                            \
name *name::Copy(BasicBlock *targetBBlock) const {                          \
    auto *instrBuilder = targetBBlock->GetGraph()->GetInstructionBuilder(); \
    return instrBuilder->Create##opcode ();                                 \
}

#define OVERRIDE_COPY_METHOD(name, opcode, ...)                             \
name *name::Copy(BasicBlock *targetBBlock) const {                          \
    auto *instrBuilder = targetBBlock->GetGraph()->GetInstructionBuilder(); \
    return instrBuilder->Create##opcode (__VA_ARGS__);                      \
}

OVERRIDE_COPY_METHOD(ConstantInstruction, CONST, GetType(), GetValue())
OVERRIDE_COPY_METHOD(CastInstruction, CAST, GetType(), GetTargetType(), nullptr)
OVERRIDE_COPY_METHOD(CompareInstruction, CMP, GetType(), GetCondCode(), nullptr, nullptr)
OVERRIDE_COPY_METHOD_FIXED(CondJumpInstruction, JCMP)
OVERRIDE_COPY_METHOD_FIXED(JumpInstruction, JMP)
OVERRIDE_COPY_METHOD(RetInstruction, RET, GetType(), nullptr)
OVERRIDE_COPY_METHOD_FIXED(RetVoidInstruction, RETVOID)
OVERRIDE_COPY_METHOD(CallInstruction, CALL, GetType(), GetCallTarget())
OVERRIDE_COPY_METHOD(PhiInstruction, PHI, GetType())
OVERRIDE_COPY_METHOD(InputArgumentInstruction, ARG, GetType())
OVERRIDE_COPY_METHOD(LengthInstruction, LEN, nullptr);
OVERRIDE_COPY_METHOD(NewArrayInstruction, NEW_ARRAY, nullptr, GetTypeId());
OVERRIDE_COPY_METHOD(NewArrayImmInstruction, NEW_ARRAY_IMM, GetValue(), GetTypeId());
OVERRIDE_COPY_METHOD(NewObjectInstruction, NEW_OBJECT, GetTypeId());
OVERRIDE_COPY_METHOD(LoadArrayInstruction, LOAD_ARRAY, GetType(), nullptr, nullptr);
OVERRIDE_COPY_METHOD(StoreArrayInstruction, STORE_ARRAY, nullptr, nullptr, nullptr);
OVERRIDE_COPY_METHOD(BoundsCheckInstruction, BOUNDS_CHECK, nullptr, nullptr);

#undef OVERRIDE_COPY_METHOD_FIXED
#undef OVERRIDE_COPY_METHOD
}   // namespace ir
