#include "LinearScanRegAlloc.h"
#include "TestGraphSamples.h"


namespace ir::codegen::tests {
class LinearScanRegAllocTest : public ir::tests::TestGraphSamples {
};

TEST_F(LinearScanRegAllocTest, TestCase1) {
    auto graphInfo = FillCase1();
    auto *graph = std::get<0>(graphInfo);

    PassManager::Run<LinearScanRegAlloc>(graph);
    // validation is done inside the pass
}

TEST_F(LinearScanRegAllocTest, TestCase4) {
    auto graphInfo = FillCase4();
    auto *graph = std::get<0>(graphInfo);

    PassManager::Run<LinearScanRegAlloc>(graph);
    // validation is done inside the pass
}

TEST_F(LinearScanRegAllocTest, TestCase5) {
    auto graphInfo = FillCase5();
    auto *graph = std::get<0>(graphInfo);

    PassManager::Run<LinearScanRegAlloc>(graph);
    // validation is done inside the pass
}

TEST_F(LinearScanRegAllocTest, TestUseAfterPhi) {
    /*
        add
        if
        | \
        |  \
        |  sub
        |  /
        | /
    phi{add,sub}
    use of add
    use of sub

    add & sub must be assigned to different locations
    */
    auto type = OperandType::I32;
    auto *instrBuilder = GetInstructionBuilder();

    auto *arg0 = instrBuilder->CreateARG(type);
    auto *arg1 = instrBuilder->CreateARG(type);
    auto *constOne = instrBuilder->CreateCONST(type, 1);
    auto *constTwo = instrBuilder->CreateCONST(type, 2);
    auto *firstBlock = FillFirstBlock(graph, arg0, arg1, constOne, constTwo);

    auto *ifBlock = graph->CreateEmptyBasicBlock();
    graph->ConnectBasicBlocks(firstBlock, ifBlock);
    auto *add = instrBuilder->CreateADD(type, arg0, constOne);
    auto *cmp = instrBuilder->CreateCMP(type, CondCode::GE, add, constTwo);
    auto *jcmp = instrBuilder->CreateJCMP();
    instrBuilder->PushBackInstruction(ifBlock, add, cmp, jcmp);

    auto *trueBranch = graph->CreateEmptyBasicBlock(true);
    graph->ConnectBasicBlocks(ifBlock, trueBranch);

    auto *falseBranch = graph->CreateEmptyBasicBlock();
    graph->ConnectBasicBlocks(ifBlock, falseBranch);
    graph->ConnectBasicBlocks(falseBranch, trueBranch);
    auto *sub = instrBuilder->CreateSUB(type, arg1, constTwo);
    instrBuilder->PushBackInstruction(falseBranch, sub);

    auto *phi = instrBuilder->CreatePHI(type, {add, sub}, {ifBlock, falseBranch});
    auto *useAdd = instrBuilder->CreateADDI(type, add, 1);
    auto *useSub = instrBuilder->CreateSUBI(type, sub, 1);
    auto *mul = instrBuilder->CreateMUL(type, useAdd, useSub);
    auto *ret = instrBuilder->CreateRET(type, mul);
    instrBuilder->PushBackInstruction(trueBranch, phi, useAdd, useSub, mul, ret);

    PassManager::Run<LinearScanRegAlloc>(graph);

    const auto &liveIntervals = graph->GetLiveIntervals();
    ASSERT_NE(liveIntervals.GetLiveIntervals(add)->GetLocation(),
              liveIntervals.GetLiveIntervals(sub)->GetLocation());
}
}   // namespace ir::codegen::tests
