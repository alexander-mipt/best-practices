#include "CompilerTestBase.h"


namespace ir::tests {
class InstructionsTest : public CompilerTestBase {
};

TEST_F(InstructionsTest, TestMul) {
    auto opType = OperandType::I32;
    auto *arg1 = GetInstructionBuilder()->CreateARG(opType);
    auto *arg2 = GetInstructionBuilder()->CreateARG(opType);

    auto *instr = GetInstructionBuilder()->CreateMUL(opType, arg1, arg2);

    ASSERT_NE(instr, nullptr);
    ASSERT_EQ(instr->GetOpcode(), Opcode::MUL);
    ASSERT_EQ(instr->GetType(), opType);
    ASSERT_EQ(instr->GetInput(0), arg1);
    ASSERT_EQ(instr->GetInput(1), arg2);
}

TEST_F(InstructionsTest, TestAddi) {
    auto opType = OperandType::U64;
    auto *arg = GetInstructionBuilder()->CreateARG(opType);
    auto imm = 11UL;

    auto *instr = GetInstructionBuilder()->CreateADDI(opType, arg, imm);

    ASSERT_NE(instr, nullptr);
    ASSERT_EQ(instr->GetOpcode(), Opcode::ADDI);
    ASSERT_EQ(instr->GetType(), opType);
    ASSERT_EQ(instr->GetInput(), arg);
    ASSERT_EQ(instr->GetValue(), imm);
}

TEST_F(InstructionsTest, TestConst) {
    auto opType = OperandType::I64;
    auto imm = 12L;

    auto *instr = GetInstructionBuilder()->CreateCONST(opType, imm);

    ASSERT_NE(instr, nullptr);
    ASSERT_EQ(instr->GetOpcode(), Opcode::CONST);
    ASSERT_EQ(instr->GetType(), opType);
    ASSERT_EQ(instr->GetValue(), imm);
}

TEST_F(InstructionsTest, TestCast) {
    auto fromType = OperandType::I32;
    auto toType = OperandType::U8;
    auto *arg = GetInstructionBuilder()->CreateARG(fromType);
    
    auto *instr = GetInstructionBuilder()->CreateCAST(fromType, toType, arg);

    ASSERT_NE(instr, nullptr);
    ASSERT_EQ(instr->GetOpcode(), Opcode::CAST);
    ASSERT_EQ(instr->GetType(), fromType);
    ASSERT_EQ(instr->GetTargetType(), toType);
    ASSERT_EQ(instr->GetInput(), arg);
}

TEST_F(InstructionsTest, TestCmp) {
    auto opType = OperandType::U64;
    auto ccode = CondCode::LT;
    auto *arg1 = GetInstructionBuilder()->CreateARG(opType);
    auto *arg2 = GetInstructionBuilder()->CreateARG(opType);

    auto *instr = GetInstructionBuilder()->CreateCMP(opType, ccode, arg1, arg2);

    ASSERT_NE(instr, nullptr);
    ASSERT_EQ(instr->GetOpcode(), Opcode::CMP);
    ASSERT_EQ(instr->GetType(), opType);
    ASSERT_EQ(instr->GetCondCode(), ccode);
    ASSERT_EQ(instr->GetInput(0), arg1);
    ASSERT_EQ(instr->GetInput(1), arg2);
}

TEST_F(InstructionsTest, TestJumpCMP) {
    auto *graph = GetGraph();
    auto *instrBuilder = GetInstructionBuilder();
    auto opType = OperandType::I16;

    auto *lhs = instrBuilder->CreateARG(opType);
    auto *rhs = instrBuilder->CreateARG(opType);
    auto *firstBlock = FillFirstBlock(graph, lhs, rhs);

    auto *bblockSource = graph->CreateEmptyBasicBlock();
    graph->ConnectBasicBlocks(firstBlock, bblockSource);
    auto *cmp = instrBuilder->CreateCMP(opType, CondCode::GE, lhs, rhs);
    auto *jcmp = instrBuilder->CreateJCMP();
    instrBuilder->PushBackInstruction(bblockSource, cmp, jcmp);

    auto *bblockTrue = graph->CreateEmptyBasicBlock();
    graph->ConnectBasicBlocks(bblockSource, bblockTrue);
    auto *bblockFalse = graph->CreateEmptyBasicBlock();
    graph->ConnectBasicBlocks(bblockSource, bblockFalse);

    ASSERT_NE(jcmp, nullptr);
    ASSERT_EQ(jcmp->GetOpcode(), Opcode::JCMP);
    ASSERT_EQ(jcmp->GetType(), OperandType::I64);
    ASSERT_EQ(jcmp->GetTrueDestination(), bblockTrue);
    ASSERT_EQ(jcmp->GetFalseDestination(), bblockFalse);
}

TEST_F(InstructionsTest, TestJmp) {
    auto *graph = GetGraph();
    auto *instrBuilder = GetInstructionBuilder();

    auto *firstBlock = graph->CreateEmptyBasicBlock();
    graph->SetFirstBasicBlock(firstBlock);

    auto *bblockSource = graph->CreateEmptyBasicBlock();
    graph->ConnectBasicBlocks(firstBlock, bblockSource);
    auto *jmp = instrBuilder->CreateJMP();
    instrBuilder->PushBackInstruction(bblockSource, jmp);

    auto *bblockDest = graph->CreateEmptyBasicBlock();
    graph->ConnectBasicBlocks(bblockSource, bblockDest);

    ASSERT_NE(jmp, nullptr);
    ASSERT_EQ(jmp->GetOpcode(), Opcode::JMP);
    ASSERT_EQ(jmp->GetType(), OperandType::I64);
    ASSERT_EQ(jmp->GetDestination(), bblockDest);
}

TEST_F(InstructionsTest, TestRet) {
    auto opType = OperandType::U8;
    auto *arg = GetInstructionBuilder()->CreateARG(opType);
    auto *instr = GetInstructionBuilder()->CreateRET(opType, arg);

    ASSERT_NE(instr, nullptr);
    ASSERT_EQ(instr->GetOpcode(), Opcode::RET);
    ASSERT_EQ(instr->GetType(), opType);
    ASSERT_EQ(instr->GetInput(), arg);
}

TEST_F(InstructionsTest, TestCall) {
    auto *instrBuilder = GetInstructionBuilder();

    auto opType = OperandType::U8;
    FunctionId callTarget = 1;

    auto *call = instrBuilder->CreateCALL(opType, callTarget);
    ASSERT_NE(call, nullptr);
    ASSERT_TRUE(call->IsCall());
    ASSERT_EQ(call->GetType(), opType);
    ASSERT_EQ(call->GetCallTarget(), callTarget);
    ASSERT_TRUE(call->HasInputs());
    ASSERT_EQ(call->GetInputsCount(), 0);

    auto *arg0 = instrBuilder->CreateARG(OperandType::U16);
    auto *arg1 = instrBuilder->CreateARG(OperandType::I64);
    callTarget = 7;
    call = instrBuilder->CreateCALL(opType, callTarget, {arg0, arg1});
    ASSERT_NE(call, nullptr);
    ASSERT_TRUE(call->IsCall());
    ASSERT_EQ(call->GetType(), opType);
    ASSERT_EQ(call->GetCallTarget(), callTarget);
    ASSERT_TRUE(call->HasInputs());
    ASSERT_EQ(call->GetInputsCount(), 2);
    ASSERT_EQ(call->GetInput(0), arg0);
    ASSERT_EQ(call->GetInput(1), arg1);
}

TEST_F(InstructionsTest, TestPhi) {
    auto opType = OperandType::U16;
    size_t numArgs = 3;
    auto args = std::vector<InstructionBase *>();
    args.reserve(numArgs);
    for (size_t i = 0; i < numArgs; ++i) {
        args.push_back(GetInstructionBuilder()->CreateARG(opType));
    }

    std::vector<BasicBlock *> bblocks{3, nullptr};
    for (auto &bblock : bblocks) {
        bblock = GetGraph()->CreateEmptyBasicBlock();
    }

    auto *instr = GetInstructionBuilder()->CreatePHI(opType, args, bblocks);

    ASSERT_NE(instr, nullptr);
    ASSERT_TRUE(instr->IsPhi());
    ASSERT_EQ(instr->GetType(), opType);
    auto inputs = instr->GetInputs();
    ASSERT_EQ(inputs.size(), numArgs);
    for (size_t i = 0; i < numArgs; ++i) {
        ASSERT_EQ(inputs[i], args[i]);
    }
}

TEST_F(InstructionsTest, TestNewArray) {
    auto *len = GetInstructionBuilder()->CreateCONST(OperandType::U64, 2);
    auto *instr = GetInstructionBuilder()->CreateNEW_ARRAY(len, MAGIC_TYPE_ID);

    ASSERT_NE(instr, nullptr);
    ASSERT_EQ(instr->GetOpcode(), Opcode::NEW_ARRAY);
    ASSERT_EQ(instr->GetType(), OperandType::REF);
    ASSERT_TRUE(instr->HasInputs());
    ASSERT_TRUE(instr->HasInputs());
    ASSERT_EQ(instr->GetInputsCount(), 1);
    ASSERT_EQ(instr->GetInput(0), len);
    ASSERT_EQ(instr->GetTypeId(), MAGIC_TYPE_ID);
}

TEST_F(InstructionsTest, TestNewArrayImm) {
    uint64_t len = 2;
    auto *instr = GetInstructionBuilder()->CreateNEW_ARRAY_IMM(len, MAGIC_TYPE_ID);

    ASSERT_NE(instr, nullptr);
    ASSERT_EQ(instr->GetOpcode(), Opcode::NEW_ARRAY_IMM);
    ASSERT_EQ(instr->GetType(), OperandType::REF);
    ASSERT_EQ(instr->GetValue(), len);
    ASSERT_EQ(instr->GetTypeId(), MAGIC_TYPE_ID);
}

TEST_F(InstructionsTest, TestLen) {
    auto *array = GetInstructionBuilder()->CreateNEW_ARRAY_IMM(2, 1234);
    auto *instr = GetInstructionBuilder()->CreateLEN(array);

    ASSERT_NE(instr, nullptr);
    ASSERT_EQ(instr->GetOpcode(), Opcode::LEN);
    ASSERT_EQ(instr->GetType(), OperandType::U64);
    ASSERT_EQ(instr->GetInput(0), array);
}

TEST_F(InstructionsTest, TestNewObject) {
    auto *instr = GetInstructionBuilder()->CreateNEW_OBJECT(MAGIC_TYPE_ID);

    ASSERT_NE(instr, nullptr);
    ASSERT_EQ(instr->GetOpcode(), Opcode::NEW_OBJECT);
    ASSERT_EQ(instr->GetType(), OperandType::REF);
    ASSERT_EQ(instr->GetTypeId(), MAGIC_TYPE_ID);
}

TEST_F(InstructionsTest, TestLoadArray) {
    auto opType = OperandType::U8;

    auto *constOne = GetInstructionBuilder()->CreateCONST(OperandType::U64, 1);
    auto *array = GetInstructionBuilder()->CreateNEW_ARRAY_IMM(2, 1234);
    auto *instr = GetInstructionBuilder()->CreateLOAD_ARRAY(opType, array, constOne);

    ASSERT_NE(instr, nullptr);
    ASSERT_EQ(instr->GetOpcode(), Opcode::LOAD_ARRAY);
    ASSERT_EQ(instr->GetType(), opType);
    ASSERT_TRUE(instr->HasInputs());
    ASSERT_EQ(instr->GetInputsCount(), 2);
    ASSERT_EQ(instr->GetInput(0), array);
    ASSERT_EQ(instr->GetInput(1), constOne);
}

TEST_F(InstructionsTest, TestLoadArrayImm) {
    auto opType = OperandType::U8;

    uint64_t idx = 1;
    auto *array = GetInstructionBuilder()->CreateNEW_ARRAY_IMM(2, 1234);
    auto *instr = GetInstructionBuilder()->CreateLOAD_ARRAY_IMM(opType, array, idx);

    ASSERT_NE(instr, nullptr);
    ASSERT_EQ(instr->GetOpcode(), Opcode::LOAD_ARRAY_IMM);
    ASSERT_EQ(instr->GetType(), opType);
    ASSERT_TRUE(instr->HasInputs());
    ASSERT_EQ(instr->GetInputsCount(), 1);
    ASSERT_EQ(instr->GetInput(0), array);
    ASSERT_EQ(instr->GetValue(), idx);
}

TEST_F(InstructionsTest, TestLoadObject) {
    auto opType = OperandType::U8;

    uint64_t offset = 8;
    auto *obj = GetInstructionBuilder()->CreateNEW_OBJECT(1234);
    auto *instr = GetInstructionBuilder()->CreateLOAD_OBJECT(opType, obj, offset);

    ASSERT_NE(instr, nullptr);
    ASSERT_EQ(instr->GetOpcode(), Opcode::LOAD_OBJECT);
    ASSERT_EQ(instr->GetType(), opType);
    ASSERT_TRUE(instr->HasInputs());
    ASSERT_EQ(instr->GetInputsCount(), 1);
    ASSERT_EQ(instr->GetInput(0), obj);
    ASSERT_EQ(instr->GetValue(), offset);
}

TEST_F(InstructionsTest, TestStoreArray) {
    auto *constOne = GetInstructionBuilder()->CreateCONST(OperandType::U64, 1);
    auto *value = GetInstructionBuilder()->CreateCONST(OperandType::U64, -1);
    auto *array = GetInstructionBuilder()->CreateNEW_ARRAY_IMM(2, 1234);
    auto *instr = GetInstructionBuilder()->CreateSTORE_ARRAY(array, value, constOne);

    ASSERT_NE(instr, nullptr);
    ASSERT_EQ(instr->GetOpcode(), Opcode::STORE_ARRAY);
    ASSERT_TRUE(instr->HasInputs());
    ASSERT_EQ(instr->GetInputsCount(), 3);
    ASSERT_EQ(instr->GetInput(0), array);
    ASSERT_EQ(instr->GetInput(1), value);
    ASSERT_EQ(instr->GetInput(2), constOne);
}

TEST_F(InstructionsTest, TestStoreArrayImm) {
    uint64_t idx = 1;
    auto *value = GetInstructionBuilder()->CreateCONST(OperandType::U64, -1);
    auto *array = GetInstructionBuilder()->CreateNEW_ARRAY_IMM(2, 1234);
    auto *instr = GetInstructionBuilder()->CreateSTORE_ARRAY_IMM(array, value, idx);

    ASSERT_NE(instr, nullptr);
    ASSERT_EQ(instr->GetOpcode(), Opcode::STORE_ARRAY_IMM);
    ASSERT_TRUE(instr->HasInputs());
    ASSERT_EQ(instr->GetInputsCount(), 2);
    ASSERT_EQ(instr->GetInput(0), array);
    ASSERT_EQ(instr->GetInput(1), value);
    ASSERT_EQ(instr->GetValue(), idx);
}

TEST_F(InstructionsTest, TestStoreObject) {
    uint64_t offset = 8;
    auto *value = GetInstructionBuilder()->CreateCONST(OperandType::U64, -1);
    auto *obj = GetInstructionBuilder()->CreateNEW_OBJECT(1234);
    auto *instr = GetInstructionBuilder()->CreateSTORE_OBJECT(obj, value, offset);

    ASSERT_NE(instr, nullptr);
    ASSERT_EQ(instr->GetOpcode(), Opcode::STORE_OBJECT);
    ASSERT_TRUE(instr->HasInputs());
    ASSERT_EQ(instr->GetInputsCount(), 2);
    ASSERT_EQ(instr->GetInput(0), obj);
    ASSERT_EQ(instr->GetInput(1), value);
    ASSERT_EQ(instr->GetValue(), offset);
}

TEST_F(InstructionsTest, TestNullCheck) {
    auto *obj = GetInstructionBuilder()->CreateNEW_OBJECT(MAGIC_TYPE_ID);
    auto *instr = GetInstructionBuilder()->CreateNULL_CHECK(obj);

    ASSERT_NE(instr, nullptr);
    ASSERT_EQ(instr->GetOpcode(), Opcode::NULL_CHECK);
    ASSERT_TRUE(instr->HasInputs());
    ASSERT_TRUE(instr->HasSideEffects());
    ASSERT_EQ(instr->GetInputsCount(), 1);
    ASSERT_EQ(instr->GetInput(0), obj);
}

TEST_F(InstructionsTest, TestZeroCheck) {
    auto *constZero = GetInstructionBuilder()->CreateCONST(OperandType::I16, 0);
    auto *instr = GetInstructionBuilder()->CreateZERO_CHECK(constZero);

    ASSERT_NE(instr, nullptr);
    ASSERT_EQ(instr->GetOpcode(), Opcode::ZERO_CHECK);
    ASSERT_TRUE(instr->HasInputs());
    ASSERT_TRUE(instr->HasSideEffects());
    ASSERT_EQ(instr->GetInputsCount(), 1);
    ASSERT_EQ(instr->GetInput(0), constZero);
}

TEST_F(InstructionsTest, TestNegativeCheck) {
    auto *constZero = GetInstructionBuilder()->CreateCONST(OperandType::I16, 0);
    auto *instr = GetInstructionBuilder()->CreateNEGATIVE_CHECK(constZero);

    ASSERT_NE(instr, nullptr);
    ASSERT_EQ(instr->GetOpcode(), Opcode::NEGATIVE_CHECK);
    ASSERT_TRUE(instr->HasInputs());
    ASSERT_TRUE(instr->HasSideEffects());
    ASSERT_EQ(instr->GetInputsCount(), 1);
    ASSERT_EQ(instr->GetInput(0), constZero);
}

TEST_F(InstructionsTest, TestBoundsCheck) {
    auto *arr = GetInstructionBuilder()->CreateNEW_ARRAY_IMM(MAGIC_TYPE_ID, MAGIC_TYPE_ID);
    auto *idx = GetInstructionBuilder()->CreateCONST(OperandType::I16, 1);
    auto *instr = GetInstructionBuilder()->CreateBOUNDS_CHECK(arr, idx);

    ASSERT_NE(instr, nullptr);
    ASSERT_EQ(instr->GetOpcode(), Opcode::BOUNDS_CHECK);
    ASSERT_TRUE(instr->HasInputs());
    ASSERT_TRUE(instr->HasSideEffects());
    ASSERT_EQ(instr->GetInputsCount(), 2);
    ASSERT_EQ(instr->GetInput(0), arr);
    ASSERT_EQ(instr->GetInput(1), idx);
}
}   // namespace ir::tests
