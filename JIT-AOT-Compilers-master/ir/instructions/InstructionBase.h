#ifndef JIT_AOT_COMPILERS_COURSE_INSTRUCTION_BASE_H_
#define JIT_AOT_COMPILERS_COURSE_INSTRUCTION_BASE_H_

#include "Concepts.h"
#include <cstdint>
#include "helpers.h"
#include <log4cpp/Category.hh>
#include "macros.h"
#include "marker/marker.h"
#include "Types.h"
#include "Users.h"


namespace ir {
class BasicBlock;
class ConstantInstruction;
class Input;
class InputsInstruction;
class PhiInstruction;

// Opcodes & Conditional Codes
#define INSTS_LIST(DEF)     \
    DEF(DIV)                \
    DEF(DIVI)               \
    DEF(MOD)                \
    DEF(MODI)               \
    DEF(CALL)               \
    DEF(CMP)                \
    DEF(JCMP)               \
    DEF(JMP)                \
    DEF(RET)                \
    DEF(RETVOID)            \
    DEF(CONST)              \
    DEF(NOT)                \
    DEF(AND)                \
    DEF(OR)                 \
    DEF(XOR)                \
    DEF(NEG)                \
    DEF(ADD)                \
    DEF(SUB)                \
    DEF(MUL)                \
    DEF(SRA)                \
    DEF(SLA)                \
    DEF(SLL)                \
    DEF(ANDI)               \
    DEF(ORI)                \
    DEF(XORI)               \
    DEF(ADDI)               \
    DEF(SUBI)               \
    DEF(MULI)               \
    DEF(SRAI)               \
    DEF(SLAI)               \
    DEF(SLLI)               \
    DEF(CAST)               \
    DEF(PHI)                \
    DEF(ARG)                \
    DEF(LEN)                \
    DEF(NEW_ARRAY)          \
    DEF(NEW_ARRAY_IMM)      \
    DEF(NEW_OBJECT)         \
    DEF(LOAD_ARRAY)         \
    DEF(LOAD_ARRAY_IMM)     \
    DEF(LOAD_OBJECT)        \
    DEF(STORE_ARRAY)        \
    DEF(STORE_ARRAY_IMM)    \
    DEF(STORE_OBJECT)       \
    DEF(NULL_CHECK)         \
    DEF(ZERO_CHECK)         \
    DEF(NEGATIVE_CHECK)     \
    DEF(BOUNDS_CHECK)       \
    DEF(MOVE)

enum class Opcode {
#define OPCODE_DEF(name, ...) name,
    INSTS_LIST(OPCODE_DEF)
#undef OPCODE_DEF
    INVALID,
    NUM_OPCODES = INVALID
};

constexpr inline const char *getOpcodeName(Opcode opcode) {
    std::array<const char *, static_cast<size_t>(Opcode::NUM_OPCODES)> names{
#define OPCODE_NAME(name, ...) #name,
    INSTS_LIST(OPCODE_NAME)
#undef OPCODE_NAME
    };
    return names[static_cast<size_t>(opcode)];
}

// Instructions properties, used in optimizations
using InstructionPropT = uint8_t;

enum class InstrProp : InstructionPropT {
    ARITH = 0b1,
    MEM = 0b10,
    COMMUTABLE = 0b100,
    CF = 0b1000,
    INPUT = 0b10000,
    SIDE_EFFECTS = 0b100000,
};

constexpr inline InstructionPropT operator|(InstrProp lhs, InstrProp rhs) {
    return utils::to_underlying(lhs) | utils::to_underlying(rhs);
}

constexpr inline InstructionPropT operator|(InstructionPropT lhs, InstrProp rhs) {
    return lhs | utils::to_underlying(rhs);
}

constexpr inline InstructionPropT operator|(InstrProp lhs, InstructionPropT rhs) {
    return utils::to_underlying(lhs) | rhs;
}

template <typename T>
constexpr inline InstructionPropT &operator|=(InstructionPropT &lhs, T rhs) {
    lhs = lhs | rhs;
    return lhs;
}

// Instructions
class InstructionBase : public Markable, public Users {
public:
    using IdType = size_t;

    InstructionBase(Opcode opcode,
                    OperandType type,
                    std::pmr::memory_resource *memResource,
                    size_t id = INVALID_ID,
                    InstructionPropT prop = 0)
        : Users(memResource),
          id(id),
          opcode(opcode),
          type(type),
          properties(prop)
    {}
    NO_COPY_SEMANTIC(InstructionBase);
    NO_MOVE_SEMANTIC(InstructionBase);
    virtual DEFAULT_DTOR(InstructionBase);

    InstructionBase *GetPrevInstruction() {
        return prev;
    }
    const InstructionBase *GetPrevInstruction() const {
        return prev;
    }
    InstructionBase *GetNextInstruction() {
        return next;
    }
    const InstructionBase *GetNextInstruction() const {
        return next;
    }
    BasicBlock *GetBasicBlock() {
        return parent;
    }
    const BasicBlock *GetBasicBlock() const {
        return parent;
    }
    auto GetOpcode() const {
        return opcode;
    }
    auto GetType() const {
        return type;
    }
    const char *GetOpcodeName() const {
        return getOpcodeName(GetOpcode());
    }
    log4cpp::CategoryStream Dump(log4cpp::Category &logger) const {
        auto stream = logger << utils::LogPriority::INFO;
        dumpImpl(stream);
        // dump users
        if (!users.empty()) {
            stream << "\t(";
            for (size_t i = 0, end = users.size() - 1; i < end; ++i) {
                ASSERT(users[i]);
                stream << users[i]->GetId() << ", ";
            }
            stream << users.back()->GetId() << ")";
        }
        return stream;
    }
    size_t GetId() const {
        return id;
    }
    InstructionPropT GetProperties() const {
        return properties;
    }
    bool SatisfiesProperty(InstrProp prop) const {
        return GetProperties() & utils::to_underlying(prop);
    }
    size_t GetLinearNumber() const {
        return linearNumber;
    }

    bool IsInputArgument() const {
        return GetOpcode() == Opcode::ARG;
    }
    bool IsPhi() const {
        return GetOpcode() == Opcode::PHI;
    }
    bool IsCall() const {
        return GetOpcode() == Opcode::CALL;
    }
    bool IsConst() const {
        return GetOpcode() == Opcode::CONST;
    }
    bool IsBranch() const {
        return GetOpcode() == Opcode::JCMP;
    }
    bool HasInputs() const {
        return SatisfiesProperty(InstrProp::INPUT);
    }
    bool HasSideEffects() const {
        return SatisfiesProperty(InstrProp::SIDE_EFFECTS);
    }

    Input ToInput();
    ConstantInstruction *AsConst();
    const ConstantInstruction *AsConst() const;
    PhiInstruction *AsPhi();
    const PhiInstruction *AsPhi() const;
    InputsInstruction *AsInputsInstruction();
    const InputsInstruction *AsInputsInstruction() const;

    bool Dominates(const InstructionBase *other) const;

    void SetPrevInstruction(InstructionBase *inst) {
        prev = inst;
    }
    void SetNextInstruction(InstructionBase *inst) {
        next = inst;
    }
    void SetBasicBlock(BasicBlock *bblock) {
        parent = bblock;
    }
    void SetType(OperandType newType) {
        type = newType;
    }
    void SetId(size_t newId) {
        id = newId;
    }
    void SetLinearNumber(size_t number) {
        linearNumber = number;
    }
    template <typename T>
    constexpr inline void SetProperty(T prop) {
        properties |= prop;
    }
    void UnlinkFromParent();
    void InsertBefore(InstructionBase *before);
    void InsertAfter(InstructionBase *after);
    void ReplaceInputInUsers(InstructionBase *newInput);

    virtual InstructionBase *Copy(BasicBlock *targetBBlock) const = 0;

    NO_NEW_DELETE;

public:
    static constexpr size_t INVALID_ID = static_cast<size_t>(0) - 1;

protected:
    virtual void dumpImpl(log4cpp::CategoryStream &stream) const {
        stream << '#' << GetId() << '.' << getTypeName(GetType())
               << "\t\t" << GetOpcodeName() << "\t\t";
    }

    bool isEarlierInBasicBlock(const InstructionBase *other) const;

private:
    size_t id;

    Opcode opcode;
    OperandType type;

    InstructionBase *prev = nullptr;
    InstructionBase *next = nullptr;

    BasicBlock *parent = nullptr;

    InstructionPropT properties = 0;

    size_t linearNumber = 0;
};

template <typename T>
concept InstructionPointerType =
    std::is_base_of_v<InstructionBase, std::remove_pointer_t<std::remove_cv_t<T>>>;
}   // namespace ir

#endif  // JIT_AOT_COMPILERS_COURSE_INSTRUCTION_BASE_H_
