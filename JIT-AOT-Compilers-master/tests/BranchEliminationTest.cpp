#include <array>
#include "BranchElimination.h"
#include "TestGraphSamples.h"


namespace ir::tests {
class BranchEliminationTest : public TestGraphSamples {
};

TEST_F(BranchEliminationTest, TestSingleIf0) {
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
    auto type = OperandType::I32;
    auto [graph, bblocks] = BuildCase0();
    auto *instrBuilder = GetInstructionBuilder();

    auto *arg = instrBuilder->CreateARG(type);
    auto *constZero = instrBuilder->CreateCONST(type, 0);
    instrBuilder->PushBackInstruction(bblocks[0], arg, constZero);

    auto *constCmp = instrBuilder->CreateCMP(type, CondCode::EQ, arg, arg);
    auto *constJcmp = instrBuilder->CreateJCMP();
    instrBuilder->PushBackInstruction(bblocks[1], constCmp, constJcmp);

    auto *phiInput1 = instrBuilder->CreateADDI(type, arg, 1);
    instrBuilder->PushBackInstruction(bblocks[2], phiInput1);

    auto *phiInput2 = instrBuilder->CreateADDI(type, arg, 2);
    instrBuilder->PushBackInstruction(bblocks[3], phiInput2);

    auto *phi = instrBuilder->CreatePHI(
        type,
        {phiInput1, phiInput2},
        {bblocks[2], bblocks[3]});
    auto *ret = instrBuilder->CreateRET(type, phi);
    instrBuilder->PushBackInstruction(bblocks[4], phi, ret);

    PassManager::Run<BranchElimination>(graph);

    VerifyControlAndDataFlowGraphs(graph);
    ASSERT_TRUE(graph->IsAnalysisValid(AnalysisFlag::RPO));
    ASSERT_EQ(bblocks[3]->GetGraph(), nullptr);
    ASSERT_EQ(constCmp->GetBasicBlock(), nullptr);
    ASSERT_EQ(constJcmp->GetBasicBlock(), nullptr);
    ASSERT_EQ(phi->GetBasicBlock(), nullptr);
    ASSERT_EQ(ret->GetInput(0), phiInput1);
}

TEST_F(BranchEliminationTest, TestSingleIf1) {
    /*
       A
       |
       B
      / \
     /   \
    C     F
    |    / \
    |   E   \
    |  /    |
    | /     |
    D<------G
    */
    auto graphInfo = FillCase1();
    auto *graph = std::get<0>(graphInfo);
    auto bblocks = std::get<1>(graphInfo);

    auto *eliminatedBlock = bblocks[2];
    auto *phi = bblocks[3]->GetFirstPhiInstruction();
    ASSERT_NE(phi, nullptr);
    auto *constCmp = bblocks[1]->GetFirstInstruction();
    ASSERT_TRUE((constCmp) && constCmp->GetOpcode() == Opcode::CMP);
    auto *constJcmp = constCmp->GetNextInstruction();
    ASSERT_TRUE((constJcmp) && constJcmp->GetOpcode() == Opcode::JCMP);

    PassManager::Run<BranchElimination>(graph);

    VerifyControlAndDataFlowGraphs(graph);
    ASSERT_TRUE(graph->IsAnalysisValid(AnalysisFlag::RPO));
    ASSERT_EQ(eliminatedBlock->GetGraph(), nullptr);
    ASSERT_EQ(constCmp->GetBasicBlock(), nullptr);
    ASSERT_EQ(constJcmp->GetBasicBlock(), nullptr);
    ASSERT_EQ(phi->GetInputsCount(), 2);
}
}   // namespace ir::tests
