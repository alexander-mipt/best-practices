#ifndef JIT_AOT_COMPILERS_COURSE_INSTRUCTION_H_
#define JIT_AOT_COMPILERS_COURSE_INSTRUCTION_H_

#include "AllocatorUtils.h"
#include <array>
#include "CompilerBase.h"
#include "Concepts.h"
#include <cstdint>
#include <functional>
#include "InstructionBase.h"
#include "Input.h"
#include "macros.h"
#include <span>
#include "Types.h"
#include <vector>
#include "logger.h"


namespace ir {
static inline constexpr FunctionId INVALID_FUNCTION_ID = static_cast<FunctionId>(-1);

enum class CondCode {
    EQ,
    NE,
    LT,
    LE,
    GE,
    GT,
    NUM_CODES
};

constexpr inline CondCode inverseCondCode(CondCode cc) {
    std::array<CondCode, static_cast<size_t>(CondCode::NUM_CODES)> inversedCodes{
        CondCode::NE,
        CondCode::EQ,
        CondCode::GT,
        CondCode::GE,
        CondCode::LE,
        CondCode::LT
    };
    return inversedCodes[static_cast<size_t>(cc)];
}

constexpr inline const char *getCondCodeName(CondCode cc) {
    std::array<const char *, static_cast<size_t>(CondCode::NUM_CODES)> names{
        "EQ",
        "NE",
        "LT",
        "LE",
        "GE",
        "GT"
    };
    return names[static_cast<size_t>(cc)];
}

template <typename T, typename V>
constexpr inline bool compare(CondCode cc, T &&lhs, V &&rhs) {
    switch (cc) {
    case CondCode::EQ:
        return lhs == rhs;
    case CondCode::NE:
        return lhs != rhs;
    case CondCode::LT:
        return lhs < rhs;
    case CondCode::GE:
        return lhs >= rhs;
    default:
        UNREACHABLE("");
        return false;
    }
}

class InputsInstruction: public InstructionBase {
public:
    InputsInstruction(Opcode opcode, OperandType type, std::pmr::memory_resource *memResource)
        : InstructionBase(opcode, type, memResource) {}
    virtual DEFAULT_DTOR(InputsInstruction);

    virtual size_t GetInputsCount() const = 0;
    virtual Input &GetInput(size_t idx) = 0;
    virtual const Input &GetInput(size_t idx) const = 0;
    virtual void SetInput(Input newInput, size_t idx) = 0;
    virtual void ReplaceInput(const Input &oldInput, Input newInput) = 0;
    virtual void RemoveUserFromInputs() {
        for (size_t i = 0, end = GetInputsCount(); i < end; ++i) {
            GetInput(i)->RemoveUser(this);
        }
    }
    virtual void ForEachInput(std::function<void(Input&)> function) {
        for (size_t i = 0, end = GetInputsCount(); i < end; ++i) {
            std::invoke(function, GetInput(i));
        }
    }
    virtual void ForEachInput(std::function<void(const Input&)> function) const {
        for (size_t i = 0, end = GetInputsCount(); i < end; ++i) {
            std::invoke(function, GetInput(i));
        }
    }

protected:
    void dumpImpl(log4cpp::CategoryStream &stream) const override {
        InstructionBase::dumpImpl(stream);
        for (size_t i = 0, end = GetInputsCount(); i < end; ++i) {
            ASSERT(GetInput(i).GetInstruction());
            stream << " #" << GetInput(i)->GetId();
        }
    }
};

template <int InputsNum>
class FixedInputsInstruction: public InputsInstruction {
public:
    FixedInputsInstruction(Opcode opcode, OperandType type, std::pmr::memory_resource *memResource)
        : InputsInstruction(opcode, type, memResource) {}

    template <IsSameType<Input>... T>
    FixedInputsInstruction(Opcode opcode, OperandType type, std::pmr::memory_resource *memResource,
                           T... ins)
        : InputsInstruction(opcode, type, memResource), inputs{ins...}
    {
        for (auto &it : inputs) {
            if (it.GetInstruction()) {
                it->AddUser(this);
            }
        }
    }

    size_t GetInputsCount() const override {
        return InputsNum;
    }
    Input &GetInput(size_t idx) override {
        ASSERT(idx < inputs.size());
        return inputs[idx];
    }
    const Input &GetInput(size_t idx) const override {
        ASSERT(idx < inputs.size());
        return inputs[idx];
    }
    void SetInput(Input newInput, size_t idx) override {
        ASSERT(idx < inputs.size());
        inputs[idx] = newInput;
        if (newInput.GetInstruction()) {
            newInput->AddUser(this);
        }
    }
    void ReplaceInput(const Input &oldInput, Input newInput) override {
        auto iter = std::find(inputs.begin(), inputs.end(), oldInput);
        ASSERT(iter != inputs.end());
        *iter = newInput;
    }

    std::array<Input, InputsNum> &GetInputs() {
        // TODO: return span?
        return inputs;
    }
    auto GetInputs() const {
        // TODO: return array?
        return std::span{inputs};
    }

private:
    std::array<Input, InputsNum> inputs;
};

template <>
class FixedInputsInstruction<1>: public InputsInstruction {
public:
    FixedInputsInstruction(Opcode opcode, OperandType type, std::pmr::memory_resource *memResource)
        : InputsInstruction(opcode, type, memResource) {}
    FixedInputsInstruction(Opcode opcode, OperandType type, Input input, std::pmr::memory_resource *memResource)
        : InputsInstruction(opcode, type, memResource), input(input)
    {
        if (input.GetInstruction()) {
            input->AddUser(this);
        }
    }

    size_t GetInputsCount() const override {
        return 1;
    }
    Input &GetInput() {
        return input;
    }
    const Input &GetInput() const {
        return input;
    }
    Input &GetInput(size_t idx) override {
        ASSERT(idx == 0);
        return input;
    }
    const Input &GetInput(size_t idx) const override {
        ASSERT(idx == 0);
        return input;
    }
    void SetInput(Input newInput, size_t idx) override {
        ASSERT(idx == 0);
        input = newInput;
        if (input.GetInstruction()) {
            input->AddUser(this);
        }
    }
    void ReplaceInput(const Input &oldInput, Input newInput) override {
        ASSERT(input == oldInput);
        input = newInput;
    }
    void ForEachInput(std::function<void(Input&)> function) override {
        std::invoke(function, input);
    }
    void ForEachInput(std::function<void(const Input&)> function) const override {
        std::invoke(function, input);
    }

private:
    Input input;
};

template <typename T>
concept AllowedInputType = (IsSameType<Input, std::remove_cv_t<T>>
    || (std::is_pointer_v<T>
        && std::is_base_of_v<InstructionBase, std::remove_cv_t<std::remove_pointer_t<T>>>));

class VariableInputsInstruction: public InputsInstruction {
public:
    VariableInputsInstruction(Opcode opcode, OperandType type, std::pmr::memory_resource *memResource)
        : InputsInstruction(opcode, type, memResource),
          inputs(memResource)
    {}

    // TODO: specify correct concept
    template <typename Ins>
    VariableInputsInstruction(Opcode opcode, OperandType type, Ins ins,
                              std::pmr::memory_resource *memResource)
    requires AllowedInputType<typename Ins::value_type>
        : InputsInstruction(opcode, type, memResource),
          inputs(ins.begin(), ins.end(), memResource)
    {
        for (auto &it : inputs) {
            if (it.GetInstruction()) {
                it->AddUser(this);
            }
        }
    }

    // TODO: try generalizing constructor in respect to `ins` argument (with type-hints)
    template <typename Ins>
    VariableInputsInstruction(Opcode opcode, OperandType type, std::initializer_list<Ins> ins,
                              std::pmr::memory_resource *memResource)
    requires AllowedInputType<Ins>
        : InputsInstruction(opcode, type, memResource),
          inputs(ins.begin(), ins.end(), memResource)
    {
        for (auto &it : inputs) {
            if (it.GetInstruction()) {
                it->AddUser(this);
            }
        }
    }

    template <typename Ins, typename AllocatorT>
    VariableInputsInstruction(Opcode opcode, OperandType type, std::vector<Ins, AllocatorT> ins,
                              std::pmr::memory_resource *memResource)
    requires AllowedInputType<Ins>
        : InputsInstruction(opcode, type, memResource),
          inputs(ins.begin(), ins.end(), memResource)
    {
        for (auto &it : inputs) {
            if (it.GetInstruction()) {
                it->AddUser(this);
            }
        }
    }

    DEFAULT_DTOR(VariableInputsInstruction);

    size_t GetInputsCount() const override {
        return inputs.size();
    }
    Input &GetInput(size_t idx) override {
        ASSERT(idx < inputs.size());
        return inputs[idx];
    }
    const Input &GetInput(size_t idx) const override {
        ASSERT(idx < inputs.size());
        return inputs[idx];
    }
    void SetInput(Input newInput, size_t idx) override {
        ASSERT(idx < inputs.size());
        inputs[idx] = newInput;
        if (newInput.GetInstruction()) {
            newInput->AddUser(this);
        }
    }
    void ReplaceInput(const Input &oldInput, Input newInput) override {
        auto iter = std::find(inputs.begin(), inputs.end(), oldInput);
        ASSERT(iter != inputs.end());
        *iter = newInput;
    }

    std::pmr::vector<Input> &GetInputs() {
        return inputs;
    }
    const std::pmr::vector<Input> &GetInputs() const {
        return inputs;
    }
    void AddInput(Input newInput) {
        inputs.push_back(newInput);
        if (newInput.GetInstruction()) {
            newInput->AddUser(this);
        }
    }

protected:
    std::pmr::vector<Input> inputs;
};

template <Numeric T>
class ImmediateMixin {
public:
    using Type = T;

    ImmediateMixin(T value) : value(value) {}

    auto GetValue() const {
        return value;
    }
    void SetValue(T new_value) {
        value = new_value;
    }

private:
    T value;
};

class ConditionMixin {
public:
    explicit ConditionMixin(CondCode ccode) : ccode(ccode) {}

    auto GetCondCode() const {
        return ccode;
    }
    void SetCondCode(CondCode cc) {
        ccode = cc;
    }
    void InverseCondCode() {
        ccode = inverseCondCode(ccode);
    }

private:
    CondCode ccode;
};

class TypeIdMixin {
public:
    explicit TypeIdMixin(TypeId type) : typeId(type) {}

    auto GetTypeId() const {
        return typeId;
    }
    void SetTypeId(TypeId newType) {
        typeId = newType;
    }

private:
    TypeId typeId;
};

// Specific instructions classes
class UnaryRegInstruction : public FixedInputsInstruction<1> {
public:
    UnaryRegInstruction(Opcode opcode, OperandType type, Input input, std::pmr::memory_resource *memResource)
        : FixedInputsInstruction(opcode, type, input, memResource) {}

    UnaryRegInstruction *Copy(BasicBlock *targetBBlock) const override;
};

class BinaryRegInstruction : public FixedInputsInstruction<2> {
public:
    BinaryRegInstruction(Opcode opcode, OperandType type, Input in1, Input in2,
                         std::pmr::memory_resource *memResource)
        : FixedInputsInstruction(opcode, type, memResource, in1, in2) {}

    BinaryRegInstruction *Copy(BasicBlock *targetBBlock) const override;
};

class ConstantInstruction : public InstructionBase, public ImmediateMixin<uint64_t> {
public:
    ConstantInstruction(Opcode opcode, OperandType type, std::pmr::memory_resource *memResource)
        : InstructionBase(opcode, type, memResource), ImmediateMixin<uint64_t>(0) {}
    ConstantInstruction(Opcode opcode, OperandType type, uint64_t value, std::pmr::memory_resource *memResource)
        : InstructionBase(opcode, type, memResource), ImmediateMixin<uint64_t>(value) {}

    ConstantInstruction *Copy(BasicBlock *targetBBlock) const override;

protected:
    void dumpImpl(log4cpp::CategoryStream &stream) const override {
        InstructionBase::dumpImpl(stream);
        stream << ' ' << GetValue();
    }
};

class BinaryImmInstruction : public FixedInputsInstruction<1>, public ImmediateMixin<uint64_t> {
public:
    using ImmediateMixin<uint64_t>::Type;

    BinaryImmInstruction(Opcode opcode, OperandType type, Input input, Type imm,
                         std::pmr::memory_resource *memResource)
        : FixedInputsInstruction<1>(opcode, type, input, memResource),
          ImmediateMixin<Type>(imm)
    {}

    BinaryImmInstruction *Copy(BasicBlock *targetBBlock) const override;

protected:
    void dumpImpl(log4cpp::CategoryStream &stream) const override {
        InputsInstruction::dumpImpl(stream);
        stream << ' ' << GetValue();
    }
};

class CompareInstruction : public FixedInputsInstruction<2>, public ConditionMixin {
public:
    CompareInstruction(Opcode opcode, OperandType type, CondCode ccode, Input in1, Input in2,
                       std::pmr::memory_resource *memResource)
        : FixedInputsInstruction(opcode, type, memResource, in1, in2),
          ConditionMixin(ccode)
    {}

    CompareInstruction *Copy(BasicBlock *targetBBlock) const override;
    void Inverse();

protected:
    void dumpImpl(log4cpp::CategoryStream &stream) const override {
        stream << '#' << GetId() << '.' << getTypeName(GetType()) << "\t\t" << GetOpcodeName() << '.';
        stream << getCondCodeName(GetCondCode()) << '\t';
        for (size_t i = 0, end = GetInputsCount(); i < end; ++i) {
            stream << " #" << GetInput(i)->GetId();
        }
    }
};

class CastInstruction : public FixedInputsInstruction<1> {
public:
    CastInstruction(OperandType fromType, OperandType toType, Input input,
                    std::pmr::memory_resource *memResource)
        : FixedInputsInstruction(Opcode::CAST, fromType, input, memResource),
          toType(toType)
    {}

    auto GetTargetType() const {
        return toType;
    }
    void SetTargetType(OperandType newType) {
        toType = newType;
    }

    CastInstruction *Copy(BasicBlock *targetBBlock) const override;

private:
    OperandType toType;
};

class JumpInstruction : public InstructionBase {
public:
    JumpInstruction(Opcode opcode, std::pmr::memory_resource *memResource)
        : InstructionBase(
            opcode,
            OperandType::I64,
            memResource,
            InstructionBase::INVALID_ID,
            utils::underlying_logic_or(InstrProp::CF, InstrProp::SIDE_EFFECTS))
    {}

    BasicBlock *GetDestination();

    JumpInstruction *Copy(BasicBlock *targetBBlock) const override;
};

class CondJumpInstruction : public InstructionBase {
public:
    CondJumpInstruction(std::pmr::memory_resource *memResource)
        : InstructionBase(
            Opcode::JCMP,
            OperandType::I64,
            memResource,
            InstructionBase::INVALID_ID,
            utils::underlying_logic_or(InstrProp::CF, InstrProp::SIDE_EFFECTS))
    {}

    BasicBlock *GetDestination(bool cmpRes) {
        return cmpRes ? GetTrueDestination() : GetFalseDestination();
    }
    BasicBlock *GetTrueDestination();
    BasicBlock *GetFalseDestination();

    CondJumpInstruction *Copy(BasicBlock *targetBBlock) const override;

private:
    // true branch must always be the first successor, false branch - the second
    template <int CmpRes>
    BasicBlock *getBranchDestinationImpl();
};

class RetInstruction : public FixedInputsInstruction<1> {
public:
    RetInstruction(OperandType type, Input input, std::pmr::memory_resource *memResource)
        : FixedInputsInstruction<1>(Opcode::RET, type, input, memResource) {}

    RetInstruction *Copy(BasicBlock *targetBBlock) const override;
};

class RetVoidInstruction : public InstructionBase {
public:
    RetVoidInstruction(std::pmr::memory_resource *memResource)
        : InstructionBase(
            Opcode::RETVOID,
            OperandType::VOID,
            memResource,
            InstructionBase::INVALID_ID,
            utils::underlying_logic_or(InstrProp::CF, InstrProp::SIDE_EFFECTS))
    {}

    RetVoidInstruction *Copy(BasicBlock *targetBBlock) const override;
};

class PhiInstruction : public VariableInputsInstruction {
public:
    PhiInstruction(OperandType type, std::pmr::memory_resource *memResource)
        : VariableInputsInstruction(Opcode::PHI, type, memResource),
          sourceBBlocks(memResource) {}

    template <typename Ins, typename Sources>
    PhiInstruction(OperandType type, Ins input, Sources sources, std::pmr::memory_resource *memResource)
    requires std::is_same_v<std::remove_cv_t<typename Sources::value_type>, BasicBlock *>
             && AllowedInputType<typename Ins::value_type>
        : VariableInputsInstruction(Opcode::PHI, type, input, memResource),
          sourceBBlocks(sources.cbegin(), sources.cend(), memResource)
    {
        ASSERT(inputs.size() == sourceBBlocks.size());
    }

    template <typename Ins, typename Sources>
    PhiInstruction(OperandType type, std::initializer_list<Ins> input, std::initializer_list<Sources> sources,
                   std::pmr::memory_resource *memResource)
    requires std::is_same_v<std::remove_cv_t<Sources>, BasicBlock *> && AllowedInputType<Ins>
        : VariableInputsInstruction(Opcode::PHI, type, input, memResource),
          sourceBBlocks(sources.begin(), sources.end(), memResource)
    {
        ASSERT(inputs.size() == sourceBBlocks.size());
    }

    std::pmr::vector<BasicBlock *> &GetSourceBasicBlocks() {
        return sourceBBlocks;
    }
    const std::pmr::vector<BasicBlock *> &GetSourceBasicBlocks() const {
        return sourceBBlocks;
    }
    BasicBlock *GetSourceBasicBlock(size_t idx) {
        ASSERT(idx < sourceBBlocks.size());
        return sourceBBlocks[idx];
    }
    const BasicBlock *GetSourceBasicBlock(size_t idx) const {
        ASSERT(idx < sourceBBlocks.size());
        return sourceBBlocks[idx];
    }
    void SetSourceBasicBlock(BasicBlock *inputSource, size_t idx) {
        ASSERT((inputSource) && idx < sourceBBlocks.size());
        sourceBBlocks[idx] = inputSource;
    }
    void ReplaceSourceBasicBlock(BasicBlock *inputSource, BasicBlock *newSource) {
        ASSERT((inputSource) && (newSource));
        sourceBBlocks[IndexOf(inputSource)] = newSource;
    }
    size_t IndexOf(const BasicBlock *inputSource) const {
        ASSERT(inputSource);
        return std::find(sourceBBlocks.begin(), sourceBBlocks.end(), inputSource) - sourceBBlocks.begin();
    }
    Input ResolveInput(BasicBlock *inputSource) {
        return GetInput(IndexOf(inputSource));
    }

    void AddPhiInput(Input newInput, BasicBlock *inputSource) {
        ASSERT(inputSource);
        AddInput(newInput);
        sourceBBlocks.push_back(inputSource);
    }
    void RemovePhiInput(BasicBlock *bblock) {
        ASSERT(bblock);
        auto idx = IndexOf(bblock);
        ASSERT(idx < sourceBBlocks.size());

        sourceBBlocks[idx] = sourceBBlocks.back();
        sourceBBlocks.pop_back();
        inputs[idx] = inputs.back();
        inputs.pop_back();
    }

    PhiInstruction *Copy(BasicBlock *targetBBlock) const override;

protected:
    void dumpImpl(log4cpp::CategoryStream &stream) const override;

private:
    std::pmr::vector<BasicBlock *> sourceBBlocks;
};

class InputArgumentInstruction : public InstructionBase {
public:
    InputArgumentInstruction(OperandType type, std::pmr::memory_resource *memResource)
        : InstructionBase(Opcode::ARG, type, memResource) {}

    InputArgumentInstruction *Copy(BasicBlock *targetBBlock) const override;
};

class CallInstruction : public VariableInputsInstruction {
public:
    CallInstruction(OperandType type, FunctionId target, std::pmr::memory_resource *memResource)
        : VariableInputsInstruction(Opcode::CALL, type, memResource), callTarget(target) {}

    template <typename InputsType>
    CallInstruction(OperandType type,
                    FunctionId target,
                    InputsType input,
                    std::pmr::memory_resource *memResource)
        : VariableInputsInstruction(Opcode::CALL, type, input, memResource), callTarget(target) {}

    FunctionId GetCallTarget() const {
        return callTarget;
    }
    void SetCallTarget(FunctionId newTarget) {
        callTarget = newTarget;
    }

    CallInstruction *Copy(BasicBlock *targetBBlock) const override;

protected:
    void dumpImpl(log4cpp::CategoryStream &stream) const {
        InputsInstruction::dumpImpl(stream);
        stream << " (to " << GetCallTarget() << ')';
    }

private:
    // TODO: add callee function resolution?
    FunctionId callTarget;
};

class LengthInstruction : public UnaryRegInstruction {
public:
    LengthInstruction(Input array, std::pmr::memory_resource *memResource)
        : UnaryRegInstruction(Opcode::LEN, OperandType::U64, array, memResource)
    {
        ASSERT(!array.GetInstruction() || array->GetType() == OperandType::REF);
    }

    LengthInstruction *Copy(BasicBlock *targetBBlock) const override;
};

class NewArrayInstruction : public FixedInputsInstruction<1>,
                            public TypeIdMixin
{
public:
    NewArrayInstruction(Input length, TypeId typeId, std::pmr::memory_resource *memResource)
        : FixedInputsInstruction<1>(Opcode::NEW_ARRAY, OperandType::REF, length, memResource),
          TypeIdMixin(typeId)
    {
        ASSERT(!length.GetInstruction() || IsIntegerType(length->GetType()));
    }

    NewArrayInstruction *Copy(BasicBlock *targetBBlock) const override;

protected:
    void dumpImpl(log4cpp::CategoryStream &stream) const override {
        InstructionBase::dumpImpl(stream);
        ASSERT(GetInput(0).GetInstruction());
        stream << ' ' << GetTypeId() << " len(#" << GetInput(0)->GetId() << ')';
    }
};

class NewArrayImmInstruction : public InstructionBase,
                               public ImmediateMixin<uint64_t>,
                               public TypeIdMixin
{
public:
    NewArrayImmInstruction(uint64_t length, TypeId typeId, std::pmr::memory_resource *memResource)
        : InstructionBase(Opcode::NEW_ARRAY_IMM, OperandType::REF, memResource),
          ImmediateMixin<uint64_t>(length),
          TypeIdMixin(typeId)
    {
        ASSERT(length > 0);
    }

    NewArrayImmInstruction *Copy(BasicBlock *targetBBlock) const override;

protected:
    void dumpImpl(log4cpp::CategoryStream &stream) const override {
        InstructionBase::dumpImpl(stream);
        stream << ' ' << GetTypeId() << " len(" << GetValue() << ')';
    }
};

class NewObjectInstruction : public InstructionBase, public TypeIdMixin {
public:
    NewObjectInstruction(TypeId typeId, std::pmr::memory_resource *memResource)
        : InstructionBase(Opcode::NEW_OBJECT, OperandType::REF, memResource), TypeIdMixin(typeId) {}

    NewObjectInstruction *Copy(BasicBlock *targetBBlock) const override;

protected:
    void dumpImpl(log4cpp::CategoryStream &stream) const override {
        InstructionBase::dumpImpl(stream);
        stream << ' ' << GetTypeId();
    }
};

class LoadArrayInstruction : public BinaryRegInstruction {
public:
    LoadArrayInstruction(OperandType type, Input array, Input idx, std::pmr::memory_resource *memResource)
        : BinaryRegInstruction(Opcode::LOAD_ARRAY, type, array, idx, memResource)
    {
        // TODO: must somehow validate return type over underlying
        ASSERT(!array.GetInstruction() || array->GetType() == OperandType::REF);
        ASSERT(!idx.GetInstruction() || IsIntegerType(idx->GetType()));
    }

    LoadArrayInstruction *Copy(BasicBlock *targetBBlock) const override;
};

class LoadImmInstruction : public BinaryImmInstruction {
public:
    LoadImmInstruction(Opcode opcode,
                       OperandType type,
                       Input obj,
                       uint64_t offset,
                       std::pmr::memory_resource *memResource)
        : BinaryImmInstruction(opcode, type, obj, offset, memResource)
    {
        ASSERT(opcode == Opcode::LOAD_ARRAY_IMM || opcode == Opcode::LOAD_OBJECT);
        ASSERT(!obj.GetInstruction() || obj->GetType() == OperandType::REF);
    }

    LoadImmInstruction *Copy(BasicBlock *targetBBlock) const override;
};

class StoreArrayInstruction : public FixedInputsInstruction<3> {
public:
    StoreArrayInstruction(Input array, Input storedValue, Input idx, std::pmr::memory_resource *memResource)
        : FixedInputsInstruction<3>(Opcode::STORE_ARRAY, OperandType::VOID, memResource, array, storedValue, idx)
    {
        ASSERT(!array.GetInstruction() || array->GetType() == OperandType::REF);
        ASSERT(!idx.GetInstruction() || IsIntegerType(idx->GetType()));
    }

    StoreArrayInstruction *Copy(BasicBlock *targetBBlock) const override;
};

class StoreImmInstruction : public FixedInputsInstruction<2>, public ImmediateMixin<uint64_t> {
public:
    StoreImmInstruction(Opcode opcode,
                        Input obj,
                        Input storedValue,
                        uint64_t offset,
                        std::pmr::memory_resource *memResource)
        : FixedInputsInstruction<2>(opcode, OperandType::VOID, memResource, obj, storedValue),
          ImmediateMixin<uint64_t>(offset)
    {
        ASSERT(opcode == Opcode::STORE_ARRAY_IMM || opcode == Opcode::STORE_OBJECT);
        ASSERT(!obj.GetInstruction() || obj->GetType() == OperandType::REF);
    }

    StoreImmInstruction *Copy(BasicBlock *targetBBlock) const override;
};

class BoundsCheckInstruction : public FixedInputsInstruction<2> {
public:
    BoundsCheckInstruction(Input arr, Input idx, std::pmr::memory_resource *memResource)
        : FixedInputsInstruction<2>(
            Opcode::BOUNDS_CHECK,
            OperandType::INVALID,
            memResource,
            arr,
            idx)
    {
        ASSERT(!arr.GetInstruction() || arr->GetType() == OperandType::REF);
        ASSERT(!idx.GetInstruction() || IsIntegerType(idx->GetType()));
    }

    BoundsCheckInstruction *Copy(BasicBlock *targetBBlock) const override;
};
}   // namespace ir

#endif  // JIT_AOT_COMPILERS_COURSE_INSTRUCTION_H_
