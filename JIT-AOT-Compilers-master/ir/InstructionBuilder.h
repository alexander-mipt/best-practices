#ifndef JIT_AOT_COMPILERS_COURSE_INSTRUCTION_BUILDER_H_
#define JIT_AOT_COMPILERS_COURSE_INSTRUCTION_BUILDER_H_

#include "Concepts.h"
#include "Graph.h"
#include "instructions/Instruction.h"
#include "macros.h"
#include <vector>


namespace ir {
// Per-graph instruction builder.
// Contains pointers to constructed instructions; instances of this class
// must be destroyed before the corresponding allocator object frees the memory
// in order to prevent dangling pointers.
class InstructionBuilder {
public:
    explicit InstructionBuilder(std::pmr::memory_resource *memResource) : allocator(memResource)
    {
        ASSERT(memResource);
    }
    NO_COPY_SEMANTIC(InstructionBuilder);
    NO_MOVE_SEMANTIC(InstructionBuilder);
    virtual DEFAULT_DTOR(InstructionBuilder);

    void AttachInstruction(InstructionBase *inst) {
        ASSERT((inst) && (inst->GetId() == InstructionBase::INVALID_ID));
        inst->SetId(currentId++);
    }

    static void PushBackInstruction(BasicBlock *bblock, InstructionBase *instr) {
        bblock->PushBackInstruction(instr);
    }
    template <std::ranges::range InstructionsT>
    static void PushBackInstruction(BasicBlock *bblock, InstructionsT &&instructions)
    requires std::is_same_v<typename InstructionsT::value_type, InstructionBase *>
    {
        for (auto *instr : instructions) {
            PushBackInstruction(bblock, instr);
        }
    }
    template <typename... T>
    static void PushBackInstruction(BasicBlock *bblock, InstructionBase *instr, T *... reminder)
    requires InstructionType<InstructionBase, T...> {
        bblock->PushBackInstruction(instr);
        PushBackInstruction(bblock, reminder...);
    }

    void PushForwardInstruction(BasicBlock *bblock, InstructionBase *instr) {
        bblock->PushForwardInstruction(instr);
    }
    template <typename... T>
    void PushForwardInstruction(BasicBlock *bblock, InstructionBase *instr, T *... reminder)
    requires InstructionType<InstructionBase, T...> {
        bblock->PushForwardInstruction(instr);
        PushForwardInstruction(bblock, reminder...);
    }

#define CREATE_FIXED_INST(name)                                                 \
    auto *inst = utils::template New<name>(allocator, allocator.resource());    \
    inst->SetId(currentId++);                                                   \
    return inst

#define CREATE_INST(name, ...)                                                              \
    auto *inst = utils::template New<name>(allocator, __VA_ARGS__, allocator.resource());   \
    inst->SetId(currentId++);                                                               \
    return inst

#define CREATE_INST_WITH_PROP(name, prop, ...)                                              \
    auto *inst = utils::template New<name>(allocator, __VA_ARGS__, allocator.resource());   \
    inst->SetId(currentId++);                                                               \
    inst->SetProperty(prop);                                                                \
    return inst

#define CREATE_ARITHM(opcode)                                                                   \
    BinaryRegInstruction *Create##opcode(OperandType type, Input in1, Input in2) {              \
        CREATE_INST_WITH_PROP(BinaryRegInstruction, ARITHM, Opcode::opcode, type, in1, in2);    \
    }

#define CREATE_COMMUTABLE_ARITHM(opcode)                                                    \
    BinaryRegInstruction *Create##opcode(OperandType type, Input in1, Input in2) {          \
        auto prop = ARITHM | utils::to_underlying(InstrProp::COMMUTABLE);                   \
        CREATE_INST_WITH_PROP(BinaryRegInstruction, prop, Opcode::opcode, type, in1, in2);  \
    }

#define CREATE_IMM_INST(opcode)                                                                 \
    template <ValidOpType T>                                                                    \
    BinaryImmInstruction *Create##opcode(OperandType type, Input input, T imm) {                \
        CREATE_INST_WITH_PROP(BinaryImmInstruction, ARITHM, Opcode::opcode, type, input, imm);  \
    }

#define CREATE_IMM_INST_WITH_PROP(opcode, prop)                                                 \
    template <ValidOpType T>                                                                    \
    BinaryImmInstruction *Create##opcode(OperandType type, Input input, T imm) {                \
        CREATE_INST_WITH_PROP(BinaryImmInstruction, prop, Opcode::opcode, type, input, imm);    \
    }

    CREATE_COMMUTABLE_ARITHM(AND)
    CREATE_COMMUTABLE_ARITHM(OR)
    CREATE_COMMUTABLE_ARITHM(XOR)
    CREATE_COMMUTABLE_ARITHM(ADD)
    CREATE_COMMUTABLE_ARITHM(MUL)

    CREATE_ARITHM(SUB)
    BinaryRegInstruction *CreateDIV(OperandType type, Input in1, Input in2) {
        CREATE_INST_WITH_PROP(
            BinaryRegInstruction,
            SIDE_EFFECTS_ARITHM,
            Opcode::DIV,
            type,
            in1,
            in2);
    }
    BinaryRegInstruction *CreateMOD(OperandType type, Input in1, Input in2) {
        CREATE_INST_WITH_PROP(
            BinaryRegInstruction,
            SIDE_EFFECTS_ARITHM,
            Opcode::MOD,
            type,
            in1,
            in2);
    }
    CREATE_ARITHM(SRA)
    CREATE_ARITHM(SLA)
    CREATE_ARITHM(SLL)

    CREATE_IMM_INST(ANDI)
    CREATE_IMM_INST(ORI)
    CREATE_IMM_INST(XORI)
    CREATE_IMM_INST(ADDI)
    CREATE_IMM_INST(SUBI)
    CREATE_IMM_INST(MULI)
    CREATE_IMM_INST_WITH_PROP(DIVI, SIDE_EFFECTS_ARITHM)
    CREATE_IMM_INST_WITH_PROP(MODI, SIDE_EFFECTS_ARITHM)
    CREATE_IMM_INST(SRAI)
    CREATE_IMM_INST(SLAI)
    CREATE_IMM_INST(SLLI)

    UnaryRegInstruction *CreateNOT(OperandType type, Input input) {
        CREATE_INST_WITH_PROP(UnaryRegInstruction, ARITHM, Opcode::NOT, type, input);
    }
    UnaryRegInstruction *CreateNEG(OperandType type, Input input) {
        CREATE_INST_WITH_PROP(UnaryRegInstruction, ARITHM, Opcode::NEG, type, input);
    }
    template <ValidOpType T>
    ConstantInstruction *CreateCONST(OperandType type, T imm) {
        CREATE_INST(ConstantInstruction, Opcode::CONST, type, imm);
    }
    CastInstruction *CreateCAST(OperandType fromType, OperandType toType, Input input) {
        CREATE_INST_WITH_PROP(CastInstruction, InstrProp::INPUT, fromType, toType, input);
    }
    CompareInstruction *CreateCMP(OperandType type, CondCode ccode, Input in1, Input in2) {
        CREATE_INST_WITH_PROP(
            CompareInstruction,
            INPUT_SIDE_EFFECTS,
            Opcode::CMP,
            type,
            ccode,
            in1,
            in2);
    }
    CondJumpInstruction *CreateJCMP() {
        CREATE_FIXED_INST(CondJumpInstruction);
    }
    JumpInstruction *CreateJMP() {
        CREATE_INST(JumpInstruction, Opcode::JMP);
    }
    RetInstruction *CreateRET(OperandType type, Input input) {
        CREATE_INST_WITH_PROP(
            RetInstruction,
            utils::underlying_logic_or(InstrProp::CF, InstrProp::INPUT, InstrProp::SIDE_EFFECTS),
            type,
            input);
    }
    RetVoidInstruction *CreateRETVOID() {
        CREATE_FIXED_INST(RetVoidInstruction);
    }

    CallInstruction *CreateCALL(OperandType type, FunctionId target) {
        CREATE_INST_WITH_PROP(CallInstruction, INPUT_SIDE_EFFECTS, type, target);
    }
    template <AllowedInputType Ins>
    CallInstruction *CreateCALL(OperandType type, FunctionId target,
                                std::initializer_list<Ins> arguments) {
        CREATE_INST_WITH_PROP(CallInstruction, INPUT_SIDE_EFFECTS, type, target, arguments);
    }
    template <AllowedInputType Ins, typename AllocatorT>
    CallInstruction *CreateCALL(OperandType type, FunctionId target,
                                std::vector<Ins, AllocatorT> arguments) {
        CREATE_INST_WITH_PROP(CallInstruction, INPUT_SIDE_EFFECTS, type, target, arguments);
    }

    LengthInstruction *CreateLEN(Input array) {
        CREATE_INST_WITH_PROP(LengthInstruction, INPUT_MEM, array);
    }

    NewArrayInstruction *CreateNEW_ARRAY(Input length, TypeId typeId) {
        CREATE_INST_WITH_PROP(
            NewArrayInstruction,
            INPUT_MEM,
            length,
            typeId);
    }
    NewArrayImmInstruction *CreateNEW_ARRAY_IMM(uint64_t length, TypeId typeId) {
        CREATE_INST_WITH_PROP(
            NewArrayImmInstruction,
            utils::underlying_logic_or(InstrProp::MEM, InstrProp::SIDE_EFFECTS),
            length,
            typeId);
    }
    NewObjectInstruction *CreateNEW_OBJECT(TypeId typeId) {
        CREATE_INST_WITH_PROP(
            NewObjectInstruction,
            utils::underlying_logic_or(InstrProp::MEM, InstrProp::SIDE_EFFECTS),
            typeId);
    }

    LoadArrayInstruction *CreateLOAD_ARRAY(OperandType type, Input array, Input idx) {
        CREATE_INST_WITH_PROP(
            LoadArrayInstruction,
            INPUT_MEM,
            type,
            array,
            idx);
    }
    LoadImmInstruction *CreateLOAD_ARRAY_IMM(OperandType type, Input array, uint64_t idx) {
        CREATE_INST_WITH_PROP(
            LoadImmInstruction,
            INPUT_MEM,
            Opcode::LOAD_ARRAY_IMM,
            type,
            array,
            idx);
    }
    LoadImmInstruction *CreateLOAD_OBJECT(OperandType type, Input obj, uint64_t offset) {
        CREATE_INST_WITH_PROP(
            LoadImmInstruction,
            INPUT_MEM,
            Opcode::LOAD_OBJECT,
            type,
            obj,
            offset);
    }

    StoreArrayInstruction *CreateSTORE_ARRAY(Input array, Input storedValue, Input idx) {
        CREATE_INST_WITH_PROP(
            StoreArrayInstruction,
            INPUT_MEM,
            array,
            storedValue,
            idx);
    }
    StoreImmInstruction *CreateSTORE_ARRAY_IMM(Input array, Input storedValue, uint64_t idx) {
        CREATE_INST_WITH_PROP(
            StoreImmInstruction,
            INPUT_MEM,
            Opcode::STORE_ARRAY_IMM,
            array,
            storedValue,
            idx);
    }
    StoreImmInstruction *CreateSTORE_OBJECT(Input obj, Input storedValue, uint64_t offset) {
        CREATE_INST_WITH_PROP(
            StoreImmInstruction,
            INPUT_MEM,
            Opcode::STORE_OBJECT,
            obj,
            storedValue,
            offset);
    }

    PhiInstruction *CreatePHI(OperandType type) {
        CREATE_INST_WITH_PROP(PhiInstruction, InstrProp::INPUT, type);
    }
    template <typename Ins, typename Sources>
    PhiInstruction *CreatePHI(OperandType type, Ins inputs, Sources sources)
    requires std::is_same_v<std::remove_cv_t<typename Sources::value_type>, BasicBlock *>
             && AllowedInputType<typename Ins::value_type>
    {
        CREATE_INST_WITH_PROP(PhiInstruction, InstrProp::INPUT, type, inputs, sources);
    }
    template <typename Sources>
    PhiInstruction *CreatePHI(
        OperandType type,
        std::initializer_list<InstructionBase *> inputs,
        std::initializer_list<Sources> sources)
    requires std::is_same_v<std::remove_cv_t<Sources>, BasicBlock *>
    {
        CREATE_INST_WITH_PROP(PhiInstruction, InstrProp::INPUT, type, inputs, sources);
    }

    InputArgumentInstruction *CreateARG(OperandType type) {
        CREATE_INST(InputArgumentInstruction, type);
    }

    UnaryRegInstruction *CreateNULL_CHECK(Input input) {
        CREATE_INST_WITH_PROP(
            UnaryRegInstruction,
            INPUT_SIDE_EFFECTS,
            Opcode::NULL_CHECK,
            OperandType::INVALID,
            input);
    }
    UnaryRegInstruction *CreateZERO_CHECK(Input input) {
        CREATE_INST_WITH_PROP(
            UnaryRegInstruction,
            INPUT_SIDE_EFFECTS,
            Opcode::ZERO_CHECK,
            OperandType::INVALID,
            input);
    }
    UnaryRegInstruction *CreateNEGATIVE_CHECK(Input input) {
        CREATE_INST_WITH_PROP(
            UnaryRegInstruction,
            INPUT_SIDE_EFFECTS,
            Opcode::NEGATIVE_CHECK,
            OperandType::INVALID,
            input);
    }
    BoundsCheckInstruction *CreateBOUNDS_CHECK(Input arr, Input idx) {
        CREATE_INST_WITH_PROP(BoundsCheckInstruction, INPUT_SIDE_EFFECTS, arr, idx);
    }
    // Utility instruction to fix PHIs data flow before codegen.
    UnaryRegInstruction *CreateMOVE(Input input) {
        CREATE_INST_WITH_PROP(
            UnaryRegInstruction,
            InstrProp::INPUT,
            Opcode::MOVE,
            input->GetType(),
            input);
    }

#undef CREATE_FIXED_INST
#undef CREATE_INST
#undef CREATE_INST_WITH_PROP
#undef CREATE_ARITHM
#undef CREATE_COMMUTABLE_ARITHM
#undef CREATE_IMM_INST
#undef CREATE_IMM_INST_WITH_PROP

private:
    static constexpr InstructionPropT ARITHM =
        utils::underlying_logic_or(InstrProp::ARITH, InstrProp::INPUT);
    static constexpr InstructionPropT SIDE_EFFECTS_ARITHM =
        utils::underlying_logic_or(InstrProp::ARITH, InstrProp::INPUT, InstrProp::SIDE_EFFECTS);
    static constexpr InstructionPropT INPUT_SIDE_EFFECTS =
        utils::underlying_logic_or(InstrProp::INPUT, InstrProp::SIDE_EFFECTS);
    static constexpr InstructionPropT INPUT_MEM =
        utils::underlying_logic_or(InstrProp::INPUT, InstrProp::MEM, InstrProp::SIDE_EFFECTS);

private:
    std::pmr::polymorphic_allocator<> allocator;

    InstructionBase::IdType currentId = 0;
};
}   // namespace ir

#endif  // JIT_AOT_COMPILERS_COURSE_INSTRUCTION_BUILDER_H_
