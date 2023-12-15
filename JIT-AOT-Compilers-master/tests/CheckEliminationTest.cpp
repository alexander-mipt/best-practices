#include <array>
#include "CheckElimination.h"
#include "TestGraphSamples.h"


namespace ir::tests {
class CheckEliminationTest : public TestGraphSamples {
public:
    InputsInstruction *FillGraphWithChecks(Opcode opcode);

    static InputsInstruction *CopyCheck(InputsInstruction *originalCheck,
                                        BasicBlock *insertionBlock)
    {
        auto *copy = originalCheck->Copy(insertionBlock);
        ASSERT(copy->HasInputs());
        auto *copyCheck = copy->AsInputsInstruction();
        for (size_t i = 0, end = originalCheck->GetInputsCount(); i < end; ++i) {
            copyCheck->SetInput(originalCheck->GetInput(i), i);
        }
        return copyCheck;
    }
    static auto GetInstructionsVector(BasicBlock *bblock) {
        ASSERT((bblock) && (bblock->GetGraph()));
        std::pmr::vector<InstructionBase *> instrs(bblock->GetGraph()->GetMemoryResource());
        for (auto *instr : *bblock) {
            instrs.push_back(instr);
        }
        return instrs;
    }

private:
    UnaryRegInstruction *insertNullCheck(ConstantInstruction *constZero,
                                         InputArgumentInstruction *arg,
                                         InputArgumentInstruction *refArg,
                                         BasicBlock *bblock);
    UnaryRegInstruction *insertZeroCheck(ConstantInstruction *constZero,
                                         InputArgumentInstruction *arg,
                                         InputArgumentInstruction *refArg,
                                         BasicBlock *bblock);
    UnaryRegInstruction *insertNegativeCheck(ConstantInstruction *constZero,
                                             InputArgumentInstruction *arg,
                                             InputArgumentInstruction *refArg,
                                             BasicBlock *bblock);
    BoundsCheckInstruction *insertBoundsCheck(ConstantInstruction *constZero,
                                              InputArgumentInstruction *arg,
                                              InputArgumentInstruction *refArg,
                                              BasicBlock *bblock);

public:
    static constexpr OperandType TYPE = OperandType::I32;
};

InputsInstruction *CheckEliminationTest::FillGraphWithChecks(Opcode opcode) {
    /*
       A
       |
       B
      / \
     /   \
    C     D
     \   /
      \ /
       E
       |
       F
    */
    auto [graph, bblocks] = BuildCase0();
    auto *instrBuilder = GetInstructionBuilder();

    auto *unusedArg = instrBuilder->CreateARG(TYPE);
    auto *unusedRefArg = instrBuilder->CreateARG(OperandType::REF);
    auto *arg = instrBuilder->CreateARG(TYPE);
    auto *refArg = instrBuilder->CreateARG(OperandType::REF);
    auto *constZero = instrBuilder->CreateCONST(TYPE, 0);
    instrBuilder->PushBackInstruction(
        bblocks[0],
        unusedArg, unusedRefArg, arg, refArg, constZero);

    InputsInstruction *checkInstr = nullptr;
    switch (opcode)
    {
    case Opcode::NULL_CHECK:
        checkInstr = insertNullCheck(constZero, arg, refArg, bblocks[1]);
        break;
    case Opcode::ZERO_CHECK:
        checkInstr = insertZeroCheck(constZero, arg, refArg, bblocks[1]);
        break;
    case Opcode::NEGATIVE_CHECK:
        checkInstr = insertNegativeCheck(constZero, arg, refArg, bblocks[1]);
        break;
    case Opcode::BOUNDS_CHECK:
        checkInstr = insertBoundsCheck(constZero, arg, refArg, bblocks[1]);
        break;
    default:
        UNREACHABLE("must insert one of the check instructions");
    }

    auto *phiInput1 = instrBuilder->CreateADDI(TYPE, arg, 1);
    instrBuilder->PushBackInstruction(bblocks[2], phiInput1);

    auto *phiInput2 = instrBuilder->CreateSUBI(TYPE, arg, 1);
    instrBuilder->PushBackInstruction(bblocks[3], phiInput2);

    auto *phi = instrBuilder->CreatePHI(
        TYPE,
        {phiInput1, phiInput2},
        {bblocks[2], bblocks[3]});
    auto *ret = instrBuilder->CreateRET(TYPE, phi);
    instrBuilder->PushBackInstruction(bblocks[4], phi, ret);

    return checkInstr;
}

UnaryRegInstruction *CheckEliminationTest::insertNullCheck(ConstantInstruction *constZero,
                                                           InputArgumentInstruction *arg,
                                                           InputArgumentInstruction *refArg,
                                                           BasicBlock *bblock)
{
    ASSERT((arg) && (refArg) && (bblock));
    auto *instrBuilder = GetInstructionBuilder();

    auto *nullCheck = instrBuilder->CreateNULL_CHECK(refArg);
    auto *constCmp = instrBuilder->CreateCMP(TYPE, CondCode::EQ, arg, constZero);
    auto *constJcmp = instrBuilder->CreateJCMP();
    instrBuilder->PushBackInstruction(
        bblock,
        nullCheck, constCmp, constJcmp);
    return nullCheck;
}

UnaryRegInstruction *CheckEliminationTest::insertZeroCheck(ConstantInstruction *constZero,
                                                           InputArgumentInstruction *arg,
                                                           InputArgumentInstruction *refArg,
                                                           BasicBlock *bblock)
{
    ASSERT((arg) && (refArg) && (bblock));
    auto *instrBuilder = GetInstructionBuilder();

    auto *subi = instrBuilder->CreateMULI(TYPE, arg, 1);
    auto *zeroCheck = instrBuilder->CreateZERO_CHECK(subi);
    auto *mul = instrBuilder->CreateMUL(TYPE, arg, subi);
    auto *constCmp = instrBuilder->CreateCMP(TYPE, CondCode::LT, mul, constZero);
    auto *constJcmp = instrBuilder->CreateJCMP();
    instrBuilder->PushBackInstruction(
        bblock,
        subi, zeroCheck, mul, constCmp, constJcmp);
    return zeroCheck;
}

UnaryRegInstruction *CheckEliminationTest::insertNegativeCheck(ConstantInstruction *constZero,
                                                               InputArgumentInstruction *arg,
                                                               InputArgumentInstruction *refArg,
                                                               BasicBlock *bblock)
{
    ASSERT((arg) && (refArg) && (bblock));
    auto *instrBuilder = GetInstructionBuilder();

    auto *negCheck = instrBuilder->CreateNEGATIVE_CHECK(arg);
    auto *array = instrBuilder->CreateNEW_ARRAY(arg, MAGIC_TYPE_ID);
    auto *constCmp = instrBuilder->CreateCMP(TYPE, CondCode::LT, arg, constZero);
    auto *constJcmp = instrBuilder->CreateJCMP();
    instrBuilder->PushBackInstruction(
        bblock,
        negCheck, array, constCmp, constJcmp);
    return negCheck;
}

BoundsCheckInstruction *CheckEliminationTest::insertBoundsCheck(ConstantInstruction *constZero,
                                                                InputArgumentInstruction *arg,
                                                                InputArgumentInstruction *refArg,
                                                                BasicBlock *bblock)
{
    ASSERT((arg) && (refArg) && (bblock));
    auto *instrBuilder = GetInstructionBuilder();

    auto *boundsCheck = instrBuilder->CreateBOUNDS_CHECK(refArg, arg);
    auto *arrayVal = instrBuilder->CreateLOAD_ARRAY(TYPE, refArg, arg);
    auto *constCmp = instrBuilder->CreateCMP(TYPE, CondCode::EQ, arrayVal, constZero);
    auto *constJcmp = instrBuilder->CreateJCMP();
    instrBuilder->PushBackInstruction(
        bblock,
        boundsCheck, arrayVal, constCmp, constJcmp);
    return boundsCheck;
}

static void CheckNoEliminations(InstructionBase *instr) {
    ASSERT((instr) && (instr->GetBasicBlock()) && (instr->GetBasicBlock()->GetGraph()));
    auto *bblock = instr->GetBasicBlock();
    auto *graph = bblock->GetGraph();
    auto countBefore = graph->CountInstructions();

    ASSERT_FALSE(PassManager::Run<CheckElimination>(graph));
    ASSERT_EQ(countBefore, graph->CountInstructions());
    ASSERT_EQ(bblock, instr->GetBasicBlock());
    ASSERT_EQ(graph, instr->GetBasicBlock()->GetGraph());
}

#define TESTS_LIST(TEST_CASE)   \
    TEST_CASE(NULL_CHECK)       \
    TEST_CASE(ZERO_CHECK)       \
    TEST_CASE(NEGATIVE_CHECK)   \
    TEST_CASE(BOUNDS_CHECK)

#define TEST_NO_ELIMINATIONS(OPCODE_NAME)                           \
TEST_F(CheckEliminationTest, TestNoEliminations##OPCODE_NAME) {     \
    CheckNoEliminations(FillGraphWithChecks(Opcode::OPCODE_NAME));  \
}
TESTS_LIST(TEST_NO_ELIMINATIONS)
#undef TEST_NO_ELIMINATIONS

static void SubsequentChecksElimination(InputsInstruction *originalCheck) {
    ASSERT((originalCheck)
        && (originalCheck->GetBasicBlock())
        && (originalCheck->GetBasicBlock()->GetGraph()));
    auto *bblock = originalCheck->GetBasicBlock();
    auto *graph = bblock->GetGraph();

    // get instructions from the basic block to later check
    auto originalInstructions{CheckEliminationTest::GetInstructionsVector(bblock)};

    // copy check and insert right after the original one
    auto *copyCheck = CheckEliminationTest::CopyCheck(originalCheck, bblock);
    copyCheck->InsertAfter(originalCheck);
    ASSERT_EQ(bblock, copyCheck->GetBasicBlock());
    ASSERT_EQ(graph, copyCheck->GetBasicBlock()->GetGraph());

    // run optimization and check that copy was removed
    auto countBefore = graph->CountInstructions();
    ASSERT_TRUE(PassManager::Run<CheckElimination>(graph));
    ASSERT_EQ(countBefore, graph->CountInstructions() + 1);
    ASSERT_EQ(bblock, originalCheck->GetBasicBlock());
    ASSERT_EQ(graph, originalCheck->GetBasicBlock()->GetGraph());
    ASSERT_EQ(copyCheck->GetBasicBlock(), nullptr);
    CompilerTestBase::compareInstructions(originalInstructions, bblock);
}

#define TEST_SUBSEQUENT(OPCODE_NAME)                                            \
TEST_F(CheckEliminationTest, TestSubsequentChecksElimination##OPCODE_NAME) {    \
    SubsequentChecksElimination(FillGraphWithChecks(Opcode::OPCODE_NAME));      \
}
TESTS_LIST(TEST_SUBSEQUENT)
#undef TEST_SUBSEQUENT

static void DominatedChecksElimination(InputsInstruction *originalCheck) {
    ASSERT((originalCheck)
        && (originalCheck->GetBasicBlock())
        && (originalCheck->GetBasicBlock()->GetGraph()));
    auto *bblock = originalCheck->GetBasicBlock();
    ASSERT_EQ(bblock->GetSuccessorsCount(), 2);
    auto *graph = bblock->GetGraph();

    // get instructions from the basic block to later check
    auto *trueBranch = bblock->GetSuccessors()[0];
    auto *falseBranch = bblock->GetSuccessors()[1];
    auto trueInstrs{CheckEliminationTest::GetInstructionsVector(trueBranch)};
    auto falseInstrs{CheckEliminationTest::GetInstructionsVector(falseBranch)};

    // copy check into dominated blocks
    auto *trueCheck = CheckEliminationTest::CopyCheck(originalCheck, trueBranch);
    trueBranch->PushForwardInstruction(trueCheck);
    ASSERT_EQ(trueBranch, trueCheck->GetBasicBlock());
    ASSERT_EQ(graph, trueCheck->GetBasicBlock()->GetGraph());
    auto *falseCheck = CheckEliminationTest::CopyCheck(originalCheck, falseBranch);
    falseBranch->PushForwardInstruction(falseCheck);
    ASSERT_EQ(falseBranch, falseCheck->GetBasicBlock());
    ASSERT_EQ(graph, falseCheck->GetBasicBlock()->GetGraph());

    // run optimization and check that copies were removed
    auto countBefore = graph->CountInstructions();
    ASSERT_TRUE(PassManager::Run<CheckElimination>(graph));
    ASSERT_EQ(countBefore, graph->CountInstructions() + 2);
    ASSERT_EQ(bblock, originalCheck->GetBasicBlock());
    ASSERT_EQ(graph, originalCheck->GetBasicBlock()->GetGraph());

    ASSERT_EQ(trueCheck->GetBasicBlock(), nullptr);
    CompilerTestBase::compareInstructions(trueInstrs, trueBranch);
    ASSERT_EQ(falseCheck->GetBasicBlock(), nullptr);
    CompilerTestBase::compareInstructions(falseInstrs, falseBranch);
}

#define TEST_DOMINATED(OPCODE_NAME)                                         \
TEST_F(CheckEliminationTest, TestDominatedChecksElimination##OPCODE_NAME) { \
    DominatedChecksElimination(FillGraphWithChecks(Opcode::OPCODE_NAME));   \
}
TESTS_LIST(TEST_DOMINATED)
#undef TEST_DOMINATED

static void DifferentCheckNoEliminations(InputsInstruction *originalCheck)
{
    ASSERT((originalCheck)
        && (originalCheck->GetBasicBlock())
        && (originalCheck->GetBasicBlock()->GetGraph()));
    auto *bblock = originalCheck->GetBasicBlock();
    auto *graph = bblock->GetGraph();

    // get a value which is not used by any other check
    InstructionBase *differentInput = graph->GetFirstBasicBlock()->GetFirstInstruction();
    if (originalCheck->GetOpcode() == Opcode::NULL_CHECK
        || originalCheck->GetOpcode() == Opcode::BOUNDS_CHECK)
    {
        differentInput = differentInput->GetNextInstruction();
    }

    // copy check but use another checked value, so no instructions must be eliminated
    auto *copy = originalCheck->Copy(bblock);
    ASSERT_TRUE(copy->HasInputs());
    auto *differentCheck = copy->AsInputsInstruction();
    differentCheck->SetInput(differentInput, 0);
    for (size_t i = 1, end = originalCheck->GetInputsCount(); i < end; ++i) {
        differentCheck->SetInput(originalCheck->GetInput(i), i);
    }
    differentCheck->InsertAfter(originalCheck);
    ASSERT_EQ(bblock, differentCheck->GetBasicBlock());
    ASSERT_EQ(graph, differentCheck->GetBasicBlock()->GetGraph());

    // get instructions from the basic block to later check
    auto originalInstructions{CheckEliminationTest::GetInstructionsVector(bblock)};

    // run optimization and check that copy was removed
    auto countBefore = graph->CountInstructions();
    ASSERT_FALSE(PassManager::Run<CheckElimination>(graph));
    ASSERT_EQ(countBefore, graph->CountInstructions());
    ASSERT_EQ(bblock, originalCheck->GetBasicBlock());
    ASSERT_EQ(graph, originalCheck->GetBasicBlock()->GetGraph());
    ASSERT_EQ(bblock, differentCheck->GetBasicBlock());
    ASSERT_EQ(graph, differentCheck->GetBasicBlock()->GetGraph());
    CompilerTestBase::compareInstructions(originalInstructions, bblock);
}

#define TEST_DIFFERENT_INPUT(OPCODE_NAME)                                       \
TEST_F(CheckEliminationTest, TestDifferentCheckNoEliminations##OPCODE_NAME) {   \
    DifferentCheckNoEliminations(FillGraphWithChecks(Opcode::OPCODE_NAME));     \
}
TESTS_LIST(TEST_DIFFERENT_INPUT)
#undef TEST_DIFFERENT_INPUT
}   // namespace ir::tests
