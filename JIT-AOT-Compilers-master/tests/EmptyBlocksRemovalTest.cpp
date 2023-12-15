#include <array>
#include "CompilerTestBase.h"
#include "EmptyBlocksRemoval.h"


namespace ir::tests {
class EmptyBlocksRemovalTest : public CompilerTestBase {
public:
    enum InstructionCreationMode {
        FULL = 0,
        EMPTY_FIRST,
        EMPTY_SIMPLE,
        EMPTY_TRUE_BRANCH,
    };

    static constexpr size_t BlocksCount = 7;
    using InstructionsArrayT = std::array<std::vector<InstructionBase *>, BlocksCount>;

    InstructionsArrayT CreateInstructions(InstructionCreationMode mode);
    Graph *BuildTemplate(InstructionsArrayT &&instructions);
};

EmptyBlocksRemovalTest::InstructionsArrayT EmptyBlocksRemovalTest::CreateInstructions(
    EmptyBlocksRemovalTest::InstructionCreationMode mode)
{
    auto *instrBuilder = GetInstructionBuilder();
    auto type = OperandType::I16;
    InstructionsArrayT instructions;

    auto *arg = instrBuilder->CreateARG(type);
    auto *constOne = instrBuilder->CreateCONST(type, 1);
    auto *subi = instrBuilder->CreateSUBI(type, arg, 3);
    auto *addi = instrBuilder->CreateADDI(type, subi, 9);

    instructions[0] = {arg, constOne};
    instructions[1] = {};
    if (mode == InstructionCreationMode::EMPTY_FIRST) {
        instructions[2] = {
            subi,
            addi,
            instrBuilder->CreateCMP(type, CondCode::EQ, subi, constOne),
            instrBuilder->CreateJCMP()};
    } else {
        instructions[1] = {subi, addi};
        instructions[2] = {
            instrBuilder->CreateCMP(type, CondCode::EQ, subi, constOne),
            instrBuilder->CreateJCMP()};
    }
    if (mode != InstructionCreationMode::EMPTY_TRUE_BRANCH) {
        instructions[3] = {instrBuilder->CreateMULI(type, subi, 3)};
    }
    instructions[4] = {instrBuilder->CreateDIVI(type, subi, 2)};
    if (mode != InstructionCreationMode::EMPTY_SIMPLE) {
        instructions[5] = {instrBuilder->CreateADDI(type, constOne, 12)};
    }
    instructions[6] = {instrBuilder->CreateRET(type, addi)};

    return instructions;
}

Graph *EmptyBlocksRemovalTest::BuildTemplate(
    EmptyBlocksRemovalTest::InstructionsArrayT &&instructions)
{
    /*
        firstBlock
            |
            v
        secondBlock
        /          \
  trueBranch    falseBranch
        \          /
        prevLastBlock
              |
              v
          lastBlock
    */
    auto *instrBuilder = GetInstructionBuilder();

    auto *sourceBlock = FillFirstBlock(graph, std::move(instructions[0]));
    graph->SetFirstBasicBlock(sourceBlock);

    auto *firstBlock = graph->CreateEmptyBasicBlock();
    graph->ConnectBasicBlocks(sourceBlock, firstBlock);
    instrBuilder->PushBackInstruction(firstBlock, std::move(instructions[1]));

    auto *secondBlock = graph->CreateEmptyBasicBlock();
    graph->ConnectBasicBlocks(firstBlock, secondBlock);
    instrBuilder->PushBackInstruction(secondBlock, std::move(instructions[2]));

    auto *trueBranch = graph->CreateEmptyBasicBlock();
    graph->ConnectBasicBlocks(secondBlock, trueBranch);
    instrBuilder->PushBackInstruction(trueBranch, std::move(instructions[3]));

    auto *falseBranch = graph->CreateEmptyBasicBlock();
    graph->ConnectBasicBlocks(secondBlock, falseBranch);
    instrBuilder->PushBackInstruction(falseBranch, std::move(instructions[4]));

    auto *prevLastBlock = graph->CreateEmptyBasicBlock();
    graph->ConnectBasicBlocks(trueBranch, prevLastBlock);
    graph->ConnectBasicBlocks(falseBranch, prevLastBlock);
    instrBuilder->PushBackInstruction(prevLastBlock, std::move(instructions[5]));

    auto *lastBlock = graph->CreateEmptyBasicBlock(true);
    graph->ConnectBasicBlocks(prevLastBlock, lastBlock);
    instrBuilder->PushBackInstruction(lastBlock, std::move(instructions[6]));

    return graph;
}

TEST_F(EmptyBlocksRemovalTest, TestUnreachableBlock) {
    auto *graph = BuildTemplate(
        std::move(CreateInstructions(EmptyBlocksRemovalTest::InstructionCreationMode::FULL)));
    auto *unreachableBlock = graph->CreateEmptyBasicBlock();
    ASSERT_EQ(graph->GetBasicBlocksCount(), 9);

    PassManager::Run<EmptyBlocksRemoval>(GetGraph());

    ASSERT_EQ(graph->GetBasicBlocksCount(), 8);
    ASSERT_EQ(unreachableBlock->GetGraph(), nullptr);
    VerifyControlAndDataFlowGraphs(GetGraph());
}

TEST_F(EmptyBlocksRemovalTest, TestEmptySimpleBlock) {
    auto *graph = BuildTemplate(
        std::move(CreateInstructions(EmptyBlocksRemovalTest::InstructionCreationMode::EMPTY_SIMPLE)));
    auto *expectedDeletedBlock = graph
        ->GetLastBasicBlock()
        ->GetPredecessors()[0]
        ->GetPredecessors()[0];
    ASSERT_EQ(graph->GetBasicBlocksCount(), 8);

    PassManager::Run<EmptyBlocksRemoval>(GetGraph());

    ASSERT_EQ(graph->GetBasicBlocksCount(), 7);
    ASSERT_EQ(expectedDeletedBlock->GetGraph(), nullptr);
    VerifyControlAndDataFlowGraphs(GetGraph());
}

TEST_F(EmptyBlocksRemovalTest, TestEmptyFirstBlock) {
    auto *graph = BuildTemplate(
        std::move(CreateInstructions(EmptyBlocksRemovalTest::InstructionCreationMode::EMPTY_FIRST)));
    auto *expectedDeletedBlock = graph->GetFirstBasicBlock()->GetSuccessors()[0];
    ASSERT_EQ(graph->GetBasicBlocksCount(), 8);

    PassManager::Run<EmptyBlocksRemoval>(GetGraph());

    ASSERT_EQ(graph->GetBasicBlocksCount(), 7);
    ASSERT_EQ(expectedDeletedBlock->GetGraph(), nullptr);
}

TEST_F(EmptyBlocksRemovalTest, TestEmptyTrueBranchBlock) {
    auto *graph = BuildTemplate(
        std::move(CreateInstructions(EmptyBlocksRemovalTest::InstructionCreationMode::EMPTY_TRUE_BRANCH)));
    auto *expectedDeletedBlock = graph
        ->GetFirstBasicBlock()
        ->GetSuccessors()[0]
        ->GetSuccessors()[0]
        ->GetSuccessors()[0];
    ASSERT_EQ(graph->GetBasicBlocksCount(), 8);

    PassManager::Run<EmptyBlocksRemoval>(GetGraph());

    ASSERT_EQ(graph->GetBasicBlocksCount(), 7);
    ASSERT_EQ(expectedDeletedBlock->GetGraph(), nullptr);
    VerifyControlAndDataFlowGraphs(GetGraph());
}
}   // namespace ir::tests
