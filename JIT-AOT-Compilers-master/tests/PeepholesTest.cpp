#include "CompilerTestBase.h"
#include "Peephole.h"


namespace ir::tests {
class PeepholesTest : public CompilerTestBase {
};

// AND

TEST_F(PeepholesTest, TestAND1) {
    // case:
    // v0 = 0
    // v2 = v1 & v0
    // expected:
    // v2 is replaced with v0
    auto opType = OperandType::I32;
    auto *instrBuilder = GetInstructionBuilder();
    auto *graph = GetGraph();

    auto *arg = instrBuilder->CreateARG(opType);
    auto *constZero = instrBuilder->CreateCONST(opType, 0);
    auto *firstBlock = FillFirstBlock(graph, arg, constZero);

    auto *bblock = graph->CreateEmptyBasicBlock();
    graph->ConnectBasicBlocks(firstBlock, bblock);
    auto *andInstr = instrBuilder->CreateAND(opType, arg, constZero);
    auto *userInstr = instrBuilder->CreateADDI(opType, andInstr, 123);
    instrBuilder->PushBackInstruction(bblock, andInstr, userInstr);
    auto prevSize = bblock->GetSize();

    PassManager::Run<PeepholePass>(graph);

    CompilerTestBase::VerifyControlAndDataFlowGraphs(bblock);
    ASSERT_EQ(bblock->GetSize(), prevSize - 1);
    compareInstructions({userInstr}, bblock);
    ASSERT_EQ(bblock->GetLastInstruction(), userInstr);
    ASSERT_EQ(userInstr->GetInput(0), constZero);
}

TEST_F(PeepholesTest, TestAND2) {
    // case:
    // v0 = 0xffff (max value of this type)
    // v2 = v1 & v0
    // expected:
    // v2 is replaced with v1
    auto opType = OperandType::I32;
    auto *instrBuilder = GetInstructionBuilder();

    auto *arg = instrBuilder->CreateARG(opType);
    auto *constMax = instrBuilder->CreateCONST(opType, std::numeric_limits<int32_t>::max());
    auto *firstBlock = FillFirstBlock(GetGraph(), arg, constMax);

    auto *andInstr = instrBuilder->CreateAND(opType, arg, constMax);
    auto *userInstr = instrBuilder->CreateADDI(opType, andInstr, 123);

    auto *bblock = GetGraph()->CreateEmptyBasicBlock();
    GetGraph()->ConnectBasicBlocks(firstBlock, bblock);
    instrBuilder->PushBackInstruction(bblock, andInstr, userInstr);

    PassManager::Run<PeepholePass>(GetGraph());

    CompilerTestBase::VerifyControlAndDataFlowGraphs(bblock);
    compareInstructions({userInstr}, bblock);
}

TEST_F(PeepholesTest, TestANDWithNEGArgs) {
    // case:
    // v2 = ~v0 & ~v1
    // expected:
    // v3 = v0 | v1
    // v2 = ~v3
    auto opType = OperandType::I32;
    auto *instrBuilder = GetInstructionBuilder();

    auto *arg1 = instrBuilder->CreateARG(opType);
    auto *arg2 = instrBuilder->CreateARG(opType);
    auto *firstBlock = FillFirstBlock(GetGraph(), arg1, arg2);

    auto *not1 = instrBuilder->CreateNOT(opType, arg1);
    auto *not2 = instrBuilder->CreateNOT(opType, arg2);
    auto *andInstr = instrBuilder->CreateAND(opType, not1, not2);
    auto *userInstr = instrBuilder->CreateADDI(opType, andInstr, 123);

    auto *bblock = GetGraph()->CreateEmptyBasicBlock();
    GetGraph()->ConnectBasicBlocks(firstBlock, bblock);
    instrBuilder->PushBackInstruction(bblock, not1, not2, andInstr, userInstr);
    auto prevSize = bblock->GetSize();

    PassManager::Run<PeepholePass>(GetGraph());

    CompilerTestBase::VerifyControlAndDataFlowGraphs(bblock);
    ASSERT_EQ(bblock->GetSize(), prevSize - 1);
    ASSERT_EQ(bblock->GetLastInstruction(), userInstr);
    auto *notInstr = userInstr->GetInput(0).GetInstruction();
    ASSERT_NE(notInstr, nullptr);
    ASSERT_EQ(notInstr->GetOpcode(), Opcode::NOT);
    auto *orInstr = static_cast<UnaryRegInstruction *>(notInstr)->GetInput(0).GetInstruction();
    ASSERT_EQ(orInstr->GetOpcode(), Opcode::OR);
    auto *typed = static_cast<BinaryRegInstruction *>(orInstr);
    if (typed->GetInput(0) == arg1) {
        ASSERT_EQ(typed->GetInput(1), arg2);
    } else {
        ASSERT_EQ(typed->GetInput(0), arg2);
        ASSERT_EQ(typed->GetInput(1), arg1);
    }
}

TEST_F(PeepholesTest, TestANDRepeatedArgs) {
    // case:
    // v1 = v0 & v0
    // expected:
    // v1 is replaced with v0
    auto opType = OperandType::I32;
    auto *instrBuilder = GetInstructionBuilder();

    auto *arg = instrBuilder->CreateARG(opType);
    auto *firstBlock = FillFirstBlock(GetGraph(), arg);

    auto *bblock = GetGraph()->CreateEmptyBasicBlock();
    GetGraph()->ConnectBasicBlocks(firstBlock, bblock);
    auto *andInstr = instrBuilder->CreateAND(opType, arg, arg);
    auto *userInstr = instrBuilder->CreateADDI(opType, andInstr, 123);
    instrBuilder->PushBackInstruction(bblock, andInstr, userInstr);

    PassManager::Run<PeepholePass>(GetGraph());

    CompilerTestBase::VerifyControlAndDataFlowGraphs(bblock);
    compareInstructions({userInstr}, bblock);
}

static void CheckReplacementWithConstant(
    size_t firstBlockPrevSize,
    BasicBlock *bblock,
    size_t bblockPrevSize,
    BinaryImmInstruction *userInstr,
    BinaryImmInstruction::Type expectedValue)
{
    auto *firstBlock = bblock->GetGraph()->GetFirstBasicBlock();
    ASSERT(firstBlock);
    ASSERT_EQ(firstBlock->GetSize(), firstBlockPrevSize + 1);
    ASSERT_EQ(bblock->GetSize(), bblockPrevSize - 1);

    if (bblockPrevSize == 2) {
        ASSERT_EQ(bblock->GetFirstInstruction(), userInstr);
    }
    ASSERT_EQ(bblock->GetLastInstruction(), userInstr);
    auto *newInstr = firstBlock->GetLastInstruction();
    ASSERT_NE(newInstr, nullptr);
    ASSERT_TRUE(newInstr->IsConst());
    ASSERT_EQ(userInstr->GetInput(0), newInstr);
    ASSERT_EQ(newInstr->AsConst()->GetValue(), expectedValue);
}

TEST_F(PeepholesTest, TestANDFolding) {
    // case:
    // v0 = imm1
    // v1 = imm2
    // v2 = v0 & v1
    // expected:
    // v2 is replaced with CONST instruction with the corresponding value
    auto opType = OperandType::I32;
    int imm1 = 12;
    int imm2 = 34;
    auto *instrBuilder = GetInstructionBuilder();

    auto *const1 = instrBuilder->CreateCONST(opType, imm1);
    auto *const2 = instrBuilder->CreateCONST(opType, imm2);
    auto *firstBlock = FillFirstBlock(GetGraph(), const1, const2);
    auto firstBlockSize = firstBlock->GetSize();

    auto *bblock = GetGraph()->CreateEmptyBasicBlock();
    GetGraph()->ConnectBasicBlocks(firstBlock, bblock);
    auto *andInstr = instrBuilder->CreateAND(opType, const1, const2);
    auto *userInstr = instrBuilder->CreateDIVI(opType, andInstr, 3);
    instrBuilder->PushBackInstruction(bblock, andInstr, userInstr);
    auto prevSize = bblock->GetSize();

    PassManager::Run<PeepholePass>(GetGraph());

    CompilerTestBase::VerifyControlAndDataFlowGraphs(bblock);
    CheckReplacementWithConstant(firstBlockSize, bblock, prevSize, userInstr, imm1 & imm2);
}

#define TEST_CONST_ARG_NO_OPTIMIZATIONS(name, imm_val)                          \
TEST_F(PeepholesTest, TestNoOptimization##name) {                               \
    auto opType = OperandType::I32;                                             \
    auto *instrBuilder = GetInstructionBuilder();                               \
    auto *arg = instrBuilder->CreateARG(opType);                                \
    auto *constInstr = instrBuilder->CreateCONST(opType, imm_val);              \
    auto *firstBlock = FillFirstBlock(GetGraph(), arg, constInstr);             \
    auto *bblock = GetGraph()->CreateEmptyBasicBlock();                         \
    GetGraph()->ConnectBasicBlocks(firstBlock, bblock);                         \
    auto *targetInstr = instrBuilder->Create##name(opType, arg, constInstr);    \
    instrBuilder->PushBackInstruction(bblock, targetInstr);                     \
    PassManager::Run<PeepholePass>(GetGraph());                                 \
    CompilerTestBase::VerifyControlAndDataFlowGraphs(bblock);                   \
    compareInstructions({targetInstr}, bblock);                                 \
}

TEST_CONST_ARG_NO_OPTIMIZATIONS(AND, 44);

// SRA

TEST_F(PeepholesTest, TestZeroSRA) {
    // case:
    // v0 = 0
    // v2 = v0 >> v1
    // expected:
    // v2 is replaced with v0
    auto opType = OperandType::I32;
    auto *instrBuilder = GetInstructionBuilder();

    auto *arg = instrBuilder->CreateARG(opType);
    auto *constZero = instrBuilder->CreateCONST(opType, 0);
    auto *firstBlock = FillFirstBlock(GetGraph(), arg, constZero);

    auto *bblock = GetGraph()->CreateEmptyBasicBlock();
    GetGraph()->ConnectBasicBlocks(firstBlock, bblock);
    auto *sraInstr = instrBuilder->CreateSRA(opType, constZero, arg);
    auto *userInstr = instrBuilder->CreateADDI(opType, sraInstr, 123);
    instrBuilder->PushBackInstruction(bblock, sraInstr, userInstr);
    auto prevSize = bblock->GetSize();

    PassManager::Run<PeepholePass>(GetGraph());

    CompilerTestBase::VerifyControlAndDataFlowGraphs(bblock);
    ASSERT_EQ(bblock->GetSize(), prevSize - 1);
    compareInstructions({userInstr}, bblock);
    ASSERT_EQ(userInstr->GetInput(0), constZero);
}

TEST_F(PeepholesTest, TestSRAZero) {
    // case:
    // v0 = 0
    // v2 = v1 >> v0
    // expected:
    // v2 is replaced with v1
    auto opType = OperandType::I32;
    auto *instrBuilder = GetInstructionBuilder();

    auto *arg = instrBuilder->CreateARG(opType);
    auto *constZero = instrBuilder->CreateCONST(opType, 0);
    auto *firstBlock = FillFirstBlock(GetGraph(), arg, constZero);

    auto *bblock = GetGraph()->CreateEmptyBasicBlock();
    GetGraph()->ConnectBasicBlocks(firstBlock, bblock);
    auto *sraInstr = instrBuilder->CreateSRA(opType, arg, constZero);
    auto *userInstr = instrBuilder->CreateADDI(opType, sraInstr, 123);
    instrBuilder->PushBackInstruction(bblock, sraInstr, userInstr);
    auto prevSize = bblock->GetSize();

    PassManager::Run<PeepholePass>(GetGraph());

    CompilerTestBase::VerifyControlAndDataFlowGraphs(bblock);
    ASSERT_EQ(bblock->GetSize(), prevSize - 1);
    compareInstructions({userInstr}, bblock);
    ASSERT_EQ(userInstr->GetInput(0), arg);
}

static void testSRAFolding(
    InstructionBuilder *instrBuilder,
    Graph *graph,
    int imm1,
    int imm2)
{
    // case:
    // v1 = imm1
    // v2 = imm2
    // v3 = v1 >> v2
    // expected:
    // v3 is replaced with CONST instruction with the corresponding value
    auto opType = OperandType::I32;

    auto *const1 = instrBuilder->CreateCONST(opType, imm1);
    auto *const2 = instrBuilder->CreateCONST(opType, imm2);
    auto *firstBlock = CompilerTestBase::FillFirstBlock(graph, const1, const2);
    auto firstBlockSize = firstBlock->GetSize();

    auto *bblock = graph->CreateEmptyBasicBlock();
    graph->ConnectBasicBlocks(firstBlock, bblock);
    auto *sraInstr = instrBuilder->CreateSRA(opType, const1, const2);
    auto *userInstr = instrBuilder->CreateMULI(opType, sraInstr, 3);
    instrBuilder->PushBackInstruction(bblock, sraInstr, userInstr);
    auto prevSize = bblock->GetSize();

    PassManager::Run<PeepholePass>(graph);

    CompilerTestBase::VerifyControlAndDataFlowGraphs(bblock);
    CheckReplacementWithConstant(firstBlockSize, bblock, prevSize, userInstr, imm1 >> imm2);
}

TEST_F(PeepholesTest, TestSRAFolding1) {
    testSRAFolding(GetInstructionBuilder(), GetGraph(), 12345, 3);
}
TEST_F(PeepholesTest, TestSRAFolding2) {
    testSRAFolding(GetInstructionBuilder(), GetGraph(), 12345, 0);
}
TEST_F(PeepholesTest, TestSRAFolding3) {
    testSRAFolding(GetInstructionBuilder(), GetGraph(), -12345, 3);
}
TEST_F(PeepholesTest, TestSRAFolding4) {
    testSRAFolding(GetInstructionBuilder(), GetGraph(), -12345, 0);
}

TEST_F(PeepholesTest, TestSRANoOptimization) {
    auto opType = OperandType::I32;
    auto *instrBuilder = GetInstructionBuilder();

    auto *arg = instrBuilder->CreateARG(opType);
    auto *constInstr = instrBuilder->CreateCONST(opType, 43);
    auto *firstBlock = FillFirstBlock(GetGraph(), arg, constInstr);

    auto *bblock = GetGraph()->CreateEmptyBasicBlock();
    GetGraph()->ConnectBasicBlocks(firstBlock, bblock);
    auto *andInstr = instrBuilder->CreateSRA(opType, arg, constInstr);
    instrBuilder->PushBackInstruction(bblock, andInstr);

    PassManager::Run<PeepholePass>(GetGraph());

    CompilerTestBase::VerifyControlAndDataFlowGraphs(bblock);
    compareInstructions({andInstr}, bblock);
}

TEST_CONST_ARG_NO_OPTIMIZATIONS(SRA, 3);

// SUB

TEST_F(PeepholesTest, TestSUB1) {
    // case:
    // v2 = -v1
    // v3 = v0 - v2
    // expected:
    // v2 = -v1
    // v3 = v0 + v1
    auto opType = OperandType::I32;
    auto *instrBuilder = GetInstructionBuilder();

    auto *arg1 = instrBuilder->CreateARG(opType);
    auto *arg2 = instrBuilder->CreateARG(opType);
    auto *firstBlock = FillFirstBlock(GetGraph(), arg1, arg2);

    auto *bblock = GetGraph()->CreateEmptyBasicBlock();
    GetGraph()->ConnectBasicBlocks(firstBlock, bblock);
    auto *negInstr = instrBuilder->CreateNEG(opType, arg2);
    auto *subInstr = instrBuilder->CreateSUB(opType, arg1, negInstr);
    instrBuilder->PushBackInstruction(bblock, negInstr, subInstr);
    auto prevSize = bblock->GetSize();

    PassManager::Run<PeepholePass>(GetGraph());

    CompilerTestBase::VerifyControlAndDataFlowGraphs(bblock);
    ASSERT_EQ(bblock->GetSize(), prevSize);
    auto *newInstr = bblock->GetLastInstruction();
    ASSERT_NE(newInstr, nullptr);
    ASSERT_TRUE(newInstr->GetOpcode() == Opcode::ADD);
    auto *typed = static_cast<BinaryRegInstruction *>(newInstr);
    if (typed->GetInput(0) == arg1) {
        ASSERT_EQ(typed->GetInput(1), arg2);
    } else {
        ASSERT_EQ(typed->GetInput(0), arg2);
        ASSERT_EQ(typed->GetInput(1), arg1);
    }
}

static void testAddSubCombination(InstructionBuilder *instrBuilder, Graph *graph,
                                  bool firstAdd, bool firstFromAdd) {
    auto opType = OperandType::I32;

    auto *arg1 = instrBuilder->CreateARG(opType);
    auto *arg2 = instrBuilder->CreateARG(opType);
    auto *firstBlock = CompilerTestBase::FillFirstBlock(graph, arg1, arg2);

    auto *addInstr = instrBuilder->CreateADD(opType, arg1, arg2);

    InstructionBase *fromAdd = firstFromAdd ? arg1 : arg2;
    InstructionBase *firstArg = addInstr;
    InstructionBase *secondArg = fromAdd;
    if (!firstAdd) {
        firstArg = fromAdd;
        secondArg = addInstr;
    }
    auto *subInstr = instrBuilder->CreateSUB(opType, firstArg, secondArg);
    auto *userInstr = instrBuilder->CreateADDI(opType, subInstr, 123);

    auto *bblock = graph->CreateEmptyBasicBlock();
    graph->ConnectBasicBlocks(firstBlock, bblock);
    instrBuilder->PushBackInstruction(bblock, addInstr, subInstr, userInstr);

    PassManager::Run<PeepholePass>(graph);

    CompilerTestBase::VerifyControlAndDataFlowGraphs(bblock);
    ASSERT_EQ(userInstr, bblock->GetLastInstruction());
    auto *remainedInstr = firstFromAdd ? arg2 : arg1;
    if (firstAdd) {
        CompilerTestBase::compareInstructions({addInstr, userInstr}, bblock);
        ASSERT_EQ(userInstr->GetInput(0), remainedInstr);
    } else {
        auto *newInstr = userInstr->GetPrevInstruction();
        CompilerTestBase::compareInstructions({addInstr, newInstr, userInstr}, bblock);
        ASSERT_NE(newInstr, nullptr);
        ASSERT_EQ(newInstr->GetOpcode(), Opcode::NEG);
        auto newInstrArg = static_cast<UnaryRegInstruction *>(newInstr)->GetInput(0);
        ASSERT_EQ(newInstrArg, remainedInstr);
    }
}

TEST_F(PeepholesTest, TestSUB2) {
    testAddSubCombination(GetInstructionBuilder(), GetGraph(), true, true);
}
TEST_F(PeepholesTest, TestSUB3) {
    testAddSubCombination(GetInstructionBuilder(), GetGraph(), true, false);
}
TEST_F(PeepholesTest, TestSUB4) {
    testAddSubCombination(GetInstructionBuilder(), GetGraph(), false, true);
}
TEST_F(PeepholesTest, TestSUB5) {
    testAddSubCombination(GetInstructionBuilder(), GetGraph(), false, false);
}

TEST_F(PeepholesTest, TestZeroSUB) {
    // case:
    // v1 = 0
    // v2 = v1 - v0
    // expected:
    // v1 = 0
    // v2 = -v0
    auto opType = OperandType::I32;
    auto *instrBuilder = GetInstructionBuilder();

    auto *arg = instrBuilder->CreateARG(opType);
    auto *constZero = instrBuilder->CreateCONST(opType, 0);
    auto *firstBlock = FillFirstBlock(GetGraph(), arg, constZero);

    auto *bblock = GetGraph()->CreateEmptyBasicBlock();
    GetGraph()->ConnectBasicBlocks(firstBlock, bblock);
    auto *subInstr = instrBuilder->CreateSUB(opType, constZero, arg);
    auto *userInstr = instrBuilder->CreateADDI(opType, subInstr, 123);
    instrBuilder->PushBackInstruction(bblock, subInstr, userInstr);
    auto prevSize = bblock->GetSize();

    PassManager::Run<PeepholePass>(GetGraph());

    CompilerTestBase::VerifyControlAndDataFlowGraphs(bblock);
    ASSERT_EQ(bblock->GetSize(), prevSize);
    ASSERT_EQ(bblock->GetLastInstruction(), userInstr);
    auto *newInstr = userInstr->GetInput(0).GetInstruction();
    ASSERT_NE(newInstr, nullptr);
    ASSERT_EQ(newInstr->GetOpcode(), Opcode::NEG);
    ASSERT_EQ(static_cast<UnaryRegInstruction *>(newInstr)->GetInput(0), arg);
}

TEST_F(PeepholesTest, TestSUBZero) {
    // case:
    // v1 = 0
    // v2 = v0 - v1
    // expected:
    // v2 is replaced with v0
    auto opType = OperandType::I32;
    auto *instrBuilder = GetInstructionBuilder();

    auto *arg = instrBuilder->CreateARG(opType);
    auto *constZero = instrBuilder->CreateCONST(opType, 0);
    auto *firstBlock = FillFirstBlock(GetGraph(), arg, constZero);

    auto *bblock = GetGraph()->CreateEmptyBasicBlock();
    GetGraph()->ConnectBasicBlocks(firstBlock, bblock);
    auto *subInstr = instrBuilder->CreateSUB(opType, arg, constZero);
    auto *userInstr = instrBuilder->CreateADDI(opType, subInstr, 123);
    instrBuilder->PushBackInstruction(bblock, subInstr, userInstr);
    auto prevSize = bblock->GetSize();

    PassManager::Run<PeepholePass>(GetGraph());

    CompilerTestBase::VerifyControlAndDataFlowGraphs(bblock);
    ASSERT_EQ(bblock->GetSize(), prevSize - 1);
    compareInstructions({userInstr}, bblock);
    ASSERT_EQ(userInstr->GetInput(0), arg);
}

TEST_F(PeepholesTest, TestSUBRepeatedArgs) {
    // case:
    // v1 = v0 - v0
    // expected:
    // v1 is replaced with CONST instruction with the zero value
    auto opType = OperandType::I32;
    auto *instrBuilder = GetInstructionBuilder();

    auto *arg = instrBuilder->CreateARG(opType);
    auto *firstBlock = FillFirstBlock(GetGraph(), arg);
    auto firstBlockSize = firstBlock->GetSize();

    auto *bblock = GetGraph()->CreateEmptyBasicBlock();
    GetGraph()->ConnectBasicBlocks(firstBlock, bblock);
    auto *subInstr = instrBuilder->CreateSUB(opType, arg, arg);
    auto *userInstr = instrBuilder->CreateADDI(opType, subInstr, 123);
    instrBuilder->PushBackInstruction(bblock, subInstr, userInstr);
    auto prevSize = bblock->GetSize();

    PassManager::Run<PeepholePass>(GetGraph());

    CompilerTestBase::VerifyControlAndDataFlowGraphs(bblock);
    CheckReplacementWithConstant(firstBlockSize, bblock, prevSize, userInstr, 0);
}

TEST_F(PeepholesTest, TestSUBFolding) {
    // case:
    // v1 = imm1
    // v2 = imm2
    // v3 = v1 - v2
    // expected:
    // v3 is replaced with CONST instruction with the corresponding value
    auto opType = OperandType::I32;
    int imm1 = 12;
    int imm2 = 15;
    auto *instrBuilder = GetInstructionBuilder();

    auto *const1 = instrBuilder->CreateCONST(opType, imm1);
    auto *const2 = instrBuilder->CreateCONST(opType, imm2);
    auto *firstBlock = FillFirstBlock(GetGraph(), const1, const2);
    auto firstBlockSize = firstBlock->GetSize();

    auto *bblock = GetGraph()->CreateEmptyBasicBlock();
    GetGraph()->ConnectBasicBlocks(firstBlock, bblock);
    auto *subInstr = instrBuilder->CreateSUB(opType, const1, const2);
    auto *userInstr = instrBuilder->CreateMODI(opType, subInstr, 2);
    instrBuilder->PushBackInstruction(bblock, subInstr, userInstr);
    auto prevSize = bblock->GetSize();

    PassManager::Run<PeepholePass>(GetGraph());

    CompilerTestBase::VerifyControlAndDataFlowGraphs(bblock);
    CheckReplacementWithConstant(firstBlockSize, bblock, prevSize, userInstr, imm1 - imm2);
}

TEST_F(PeepholesTest, TestAddSubNoOptimizations) {
    auto opType = OperandType::I32;
    auto *instrBuilder = GetInstructionBuilder();

    auto *arg1 = instrBuilder->CreateARG(opType);
    auto *arg2 = instrBuilder->CreateARG(opType);
    auto *arg3 = instrBuilder->CreateARG(opType);
    auto *firstBlock = FillFirstBlock(GetGraph(), arg1, arg2, arg3);

    auto *bblock = GetGraph()->CreateEmptyBasicBlock();
    GetGraph()->ConnectBasicBlocks(firstBlock, bblock);
    auto *addInstr = instrBuilder->CreateADD(opType, arg1, arg2);
    auto *subInstr = instrBuilder->CreateSUB(opType, addInstr, arg3);
    instrBuilder->PushBackInstruction(bblock, addInstr, subInstr);

    PassManager::Run<PeepholePass>(GetGraph());

    CompilerTestBase::VerifyControlAndDataFlowGraphs(bblock);
    compareInstructions({addInstr, subInstr}, bblock);
}

TEST_CONST_ARG_NO_OPTIMIZATIONS(SUB, 33);

#undef TEST_CONST_ARG_NO_OPTIMIZATIONS
}   // namespace ir::tests
