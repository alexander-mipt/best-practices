#include "CompilerTestBase.h"
#include "DCE.h"


namespace ir::tests {
class DCETest : public CompilerTestBase {
};

TEST_F(DCETest, TestDCE1) {
    // v0 = arg0 * 2
    // v1 = arg0 * 3
    // v2 = v0 - 1
    // return v2
    //
    // v1 must be cleared by DCE
    auto type = OperandType::I32;
    auto *instrBuilder = GetInstructionBuilder();

    auto *arg = instrBuilder->CreateARG(type);
    auto *firstBlock = FillFirstBlock(GetGraph(), arg);

    auto *bblock = GetGraph()->CreateEmptyBasicBlock(true);
    GetGraph()->ConnectBasicBlocks(firstBlock, bblock);
    auto *v0 = instrBuilder->CreateMULI(type, arg, 2);
    auto *v1 = instrBuilder->CreateMULI(type, arg, 3);
    auto *v2 = instrBuilder->CreateSUBI(type, v0, 1);
    auto *ret = instrBuilder->CreateRET(type, v2);
    instrBuilder->PushBackInstruction(bblock, v0, v1, v2, ret);

    ASSERT_EQ(bblock->GetSize(), 4);

    PassManager::Run<DCEPass>(GetGraph());

    CompilerTestBase::compareInstructions({v0, v2, ret}, bblock);
    ASSERT_EQ(v1->GetBasicBlock(), nullptr);
    ASSERT_EQ(v0->GetNextInstruction(), v2);
}

TEST_F(DCETest, TestDCE2) {
    // v0 = arg0 * 2
    // if (arg1 == 0) {
    //     v1 = v0 - 1
    //     v2 = v1 * 2
    // } else {
    //     v0 = v0 * 2
    // }
    // return v0
    //
    // v1 and v2 must be cleared by DCE
    auto *graph = GetGraph();
    auto *instrBuilder = GetInstructionBuilder();
    auto type = OperandType::I32;

    auto *arg0 = instrBuilder->CreateARG(type);
    auto *arg1 = instrBuilder->CreateARG(type);
    auto *constZero = instrBuilder->CreateCONST(type, 0);
    auto *firstBlock = FillFirstBlock(graph, arg0, arg1, constZero);

    auto *bblockSource = graph->CreateEmptyBasicBlock();
    graph->ConnectBasicBlocks(firstBlock, bblockSource);
    auto *v0 = instrBuilder->CreateMULI(type, arg0, 2);
    auto *cmp = instrBuilder->CreateCMP(type, CondCode::EQ, arg1, constZero);
    auto *jcmp = instrBuilder->CreateJCMP();
    instrBuilder->PushBackInstruction(bblockSource, v0, cmp, jcmp);
    ASSERT_EQ(bblockSource->GetSize(), 3);

    auto *bblockTrue = graph->CreateEmptyBasicBlock();
    auto *v1 = instrBuilder->CreateSUBI(type, v0, 1);
    auto *v2 = instrBuilder->CreateMULI(type, v1, 2);
    instrBuilder->PushBackInstruction(bblockTrue, v1, v2);
    ASSERT_EQ(bblockTrue->GetSize(), 2);

    auto *bblockFalse = graph->CreateEmptyBasicBlock();
    auto *v3 = instrBuilder->CreateMULI(type, v0, 1);
    instrBuilder->PushBackInstruction(bblockFalse, v3);
    ASSERT_EQ(bblockFalse->GetSize(), 1);

    auto *bblockDest = graph->CreateEmptyBasicBlock(true);
    auto *v4 = instrBuilder->CreatePHI(type, {v0, v3}, {bblockSource, bblockFalse});
    auto *ret = instrBuilder->CreateRET(type, v4);
    instrBuilder->PushBackInstruction(bblockDest, v4, ret);
    ASSERT_EQ(bblockDest->GetSize(), 2);

    graph->ConnectBasicBlocks(bblockSource, bblockTrue);
    graph->ConnectBasicBlocks(bblockSource, bblockFalse);
    graph->ConnectBasicBlocks(bblockTrue, bblockDest);
    graph->ConnectBasicBlocks(bblockFalse, bblockDest);
    auto bblocksCount = graph->GetBasicBlocksCount();

    PassManager::Run<DCEPass>(graph);

    ASSERT_EQ(graph->GetBasicBlocksCount(), bblocksCount);
    CompilerTestBase::compareInstructions({arg0, arg1, constZero}, firstBlock);
    CompilerTestBase::compareInstructions({v0, cmp, jcmp}, bblockSource);
    CompilerTestBase::compareInstructions({}, bblockTrue);
    CompilerTestBase::compareInstructions({v3}, bblockFalse);
    CompilerTestBase::compareInstructions({v4, ret}, bblockDest);
}

TEST_F(DCETest, TestNoDCE) {
    // v0 = arg0 * 2
    // v1 = arg0 / arg1
    // v2 = v0 - 1
    // return v2
    //
    // v1 must NOT be cleared by DCE, as long as division may produce an exception
    auto type = OperandType::I32;
    auto *graph = GetGraph();
    auto *instrBuilder = GetInstructionBuilder();

    auto *arg0 = instrBuilder->CreateARG(type);
    auto *arg1 = instrBuilder->CreateARG(type);
    auto *firstBlock = FillFirstBlock(graph, arg0, arg1);

    auto *bblock = graph->CreateEmptyBasicBlock(true);
    graph->ConnectBasicBlocks(firstBlock, bblock);
    auto *v0 = instrBuilder->CreateMULI(type, arg0, 2);
    auto *v1 = instrBuilder->CreateDIV(type, arg0, arg1);
    auto *v2 = instrBuilder->CreateSUBI(type, v0, 1);
    auto *ret = instrBuilder->CreateRET(type, v2);
    instrBuilder->PushBackInstruction(bblock, v0, v1, v2, ret);
    ASSERT_EQ(bblock->GetSize(), 4);
    auto bblocksCount = graph->GetBasicBlocksCount();

    PassManager::Run<DCEPass>(graph);

    ASSERT_EQ(graph->GetBasicBlocksCount(), bblocksCount);
    CompilerTestBase::compareInstructions({v0, v1, v2, ret}, bblock);
}

TEST_F(DCETest, TestLoop) {
    // v0 = arg + 1
    // v1 = arg * 2
    // while (v0 < 4) {
    //     v1 = v1 * 2
    //     v0 = v0 + 1
    // }
    // return v0
    //
    // v1 must be cleared by DCE
    auto type = OperandType::I32;
    auto *graph = GetGraph();
    auto *instrBuilder = GetInstructionBuilder();

    auto *arg = instrBuilder->CreateARG(type);
    auto *constFour = instrBuilder->CreateCONST(type, 4);
    auto *firstBlock = FillFirstBlock(graph, arg, constFour);

    auto *varsBlock = graph->CreateEmptyBasicBlock();
    graph->ConnectBasicBlocks(firstBlock, varsBlock);
    auto *v0 = instrBuilder->CreateADDI(type, arg, 1);
    auto *v1 = instrBuilder->CreateMULI(type, arg, 2);
    instrBuilder->PushBackInstruction(varsBlock, v0, v1);

    auto *preLoop = graph->CreateEmptyBasicBlock();
    graph->ConnectBasicBlocks(varsBlock, preLoop);
    auto *phiV0 = instrBuilder->CreatePHI(type, {v0}, {varsBlock});
    auto *cmp = instrBuilder->CreateCMP(type, CondCode::LT, phiV0, constFour);
    auto *jcmp = instrBuilder->CreateJCMP();
    instrBuilder->PushBackInstruction(preLoop, phiV0, cmp, jcmp);

    auto *loopBody = graph->CreateEmptyBasicBlock();
    graph->ConnectBasicBlocks(preLoop, loopBody);
    graph->ConnectBasicBlocks(loopBody, preLoop);
    auto *phiV1 = instrBuilder->CreatePHI(type, {v1}, {varsBlock});
    auto *muliV1 = instrBuilder->CreateMULI(type, phiV1, 2);
    auto *addiV0 = instrBuilder->CreateADDI(type, phiV0, 1);
    instrBuilder->PushBackInstruction(loopBody, phiV1, muliV1, addiV0);
    phiV0->AddPhiInput(addiV0, loopBody);

    auto *postLoop = graph->CreateEmptyBasicBlock(true);
    graph->ConnectBasicBlocks(preLoop, postLoop);
    auto *ret = instrBuilder->CreateRET(type, phiV0);
    instrBuilder->PushBackInstruction(postLoop, ret);

    PassManager::Run<DCEPass>(graph);

    VerifyControlAndDataFlowGraphs(graph);
    CompilerTestBase::compareInstructions({v0}, varsBlock);
    CompilerTestBase::compareInstructions({addiV0}, loopBody);
}
}   // namespace ir::tests
