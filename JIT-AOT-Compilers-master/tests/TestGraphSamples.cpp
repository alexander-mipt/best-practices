#include "TestGraphSamples.h"


namespace ir::tests {
TestGraphSamples::CFGInfoPair TestGraphSamples::BuildCase0() {
    /*
       B0
       |
       B1
      / \
     /   \
    B2   B3
     \   /
      \ /
       B4
       |
       B5
    */
    auto *graph = GetGraph();
    std::vector<BasicBlock *> bblocks(6);
    for (auto &it : bblocks) {
        it = graph->CreateEmptyBasicBlock();
    }
    graph->SetFirstBasicBlock(bblocks[0]);
    graph->SetLastBasicBlock(bblocks[5]);

    graph->ConnectBasicBlocks(bblocks[0], bblocks[1]);
    graph->ConnectBasicBlocks(bblocks[1], bblocks[2]);
    graph->ConnectBasicBlocks(bblocks[1], bblocks[3]);
    graph->ConnectBasicBlocks(bblocks[2], bblocks[4]);
    graph->ConnectBasicBlocks(bblocks[3], bblocks[4]);
    graph->ConnectBasicBlocks(bblocks[4], bblocks[5]);

    return {graph, bblocks};
}

TestGraphSamples::CFGInfoPair TestGraphSamples::BuildCase1() {
    /*
       B0
       |
       B1
      / \
     /   \
    B2   B5
    |    / \
    |   B4  \
    |  /     |
    | /      |
    B3<------B6
    */
    auto *graph = GetGraph();
    std::vector<BasicBlock *> bblocks(7);
    for (auto &it : bblocks) {
        it = graph->CreateEmptyBasicBlock();
    }
    graph->SetFirstBasicBlock(bblocks[0]);
    graph->ConnectBasicBlocks(bblocks[0], bblocks[1]);
    graph->ConnectBasicBlocks(bblocks[1], bblocks[2]);
    graph->ConnectBasicBlocks(bblocks[1], bblocks[5]);
    graph->ConnectBasicBlocks(bblocks[2], bblocks[3]);
    graph->ConnectBasicBlocks(bblocks[4], bblocks[3]);
    graph->ConnectBasicBlocks(bblocks[5], bblocks[4]);
    graph->ConnectBasicBlocks(bblocks[5], bblocks[6]);
    graph->ConnectBasicBlocks(bblocks[6], bblocks[3]);

    return {graph, bblocks};
}

TestGraphSamples::CFGInfoPair TestGraphSamples::BuildCase2() {
    /*
        B0
        |
    --->B1->B9
    |   |  /
    |   | /
    |   |/
    |   B2<--
    |   |   |
    |   B3---
    |   |
    |   B4<--
    |   |   |
    |   B5---
    |   |
    B7<-B6-->B8-->B10
    */
    auto *graph = GetGraph();
    std::vector<BasicBlock *> bblocks(11);
    for (auto &it : bblocks) {
        it = graph->CreateEmptyBasicBlock();
    }
    graph->SetFirstBasicBlock(bblocks[0]);
    graph->SetLastBasicBlock(bblocks[10]);
    for (size_t i = 0; i < 7; ++i) {
        graph->ConnectBasicBlocks(bblocks[i], bblocks[i + 1]);
    }
    graph->ConnectBasicBlocks(bblocks[3], bblocks[2]);
    graph->ConnectBasicBlocks(bblocks[5], bblocks[4]);
    graph->ConnectBasicBlocks(bblocks[7], bblocks[1]);
    graph->ConnectBasicBlocks(bblocks[1], bblocks[9]);
    graph->ConnectBasicBlocks(bblocks[9], bblocks[2]);
    graph->ConnectBasicBlocks(bblocks[6], bblocks[8]);
    graph->ConnectBasicBlocks(bblocks[8], bblocks[10]);

    return {graph, bblocks};
}

TestGraphSamples::CFGInfoPair TestGraphSamples::BuildCase3() {
    /*
          B0
          |
          |
    ----->B1
    |    /|
    |   / |
    |  B4  B2<--
    |  |\ |    |
    |  | \|    |
    ---B5 B3   |
       |  |    |
       |  |    |
       B7->B6---
        \ |
         \|
          B8
    */
    auto *graph = GetGraph();
    std::vector<BasicBlock *> bblocks(9);
    for (auto &it : bblocks) {
        it = graph->CreateEmptyBasicBlock();
    }
    graph->SetFirstBasicBlock(bblocks[0]);
    for (size_t i = 0; i < 3; ++i) {
        graph->ConnectBasicBlocks(bblocks[i], bblocks[i + 1]);
    }
    graph->ConnectBasicBlocks(bblocks[1], bblocks[4]);
    graph->ConnectBasicBlocks(bblocks[4], bblocks[3]);
    graph->ConnectBasicBlocks(bblocks[4], bblocks[5]);
    graph->ConnectBasicBlocks(bblocks[5], bblocks[7]);
    graph->ConnectBasicBlocks(bblocks[5], bblocks[1]);
    graph->ConnectBasicBlocks(bblocks[7], bblocks[6]);
    graph->ConnectBasicBlocks(bblocks[7], bblocks[8]);
    graph->ConnectBasicBlocks(bblocks[3], bblocks[6]);
    graph->ConnectBasicBlocks(bblocks[6], bblocks[2]);
    graph->ConnectBasicBlocks(bblocks[6], bblocks[8]);

    return {graph, bblocks};
}

TestGraphSamples::CFGInfoPair TestGraphSamples::BuildCase4() {
    /*
         B0
         |
         V
    ---->B1
    |   / \
    |  /   \
    --B2    B3
            |
            V
            B4
    */
    auto *graph = GetGraph();
    std::vector<BasicBlock *> bblocks(5);
    for (auto &it : bblocks) {
        it = graph->CreateEmptyBasicBlock();
    }
    graph->SetFirstBasicBlock(bblocks.front());
    graph->SetLastBasicBlock(bblocks.back());

    graph->ConnectBasicBlocks(bblocks[0], bblocks[1]);
    graph->ConnectBasicBlocks(bblocks[1], bblocks[2]);
    graph->ConnectBasicBlocks(bblocks[2], bblocks[1]);
    graph->ConnectBasicBlocks(bblocks[1], bblocks[3]);
    graph->ConnectBasicBlocks(bblocks[3], bblocks[4]);

    return {graph, bblocks};
}

TestGraphSamples::CFGInfoPair TestGraphSamples::BuildCase5() {
    /*
          B0
          |
          B1<-------
          |        |
          B2<----  |
         / \    |  |
        /   \   |  |
       B3   B4  |  |
      / \   /   |  |
     /   \ /    |  |
    B5    B6    |  |
          |     |  |
          B7-----  |
          |        |
          B8--------
          |
          B9
    */
    auto *graph = GetGraph();
    std::vector<BasicBlock *> bblocks(10);
    for (auto &it : bblocks) {
        it = graph->CreateEmptyBasicBlock();
    }
    graph->SetFirstBasicBlock(bblocks[0]);
    graph->SetLastBasicBlock(bblocks[9]);

    graph->ConnectBasicBlocks(bblocks[0], bblocks[1]);

    graph->ConnectBasicBlocks(bblocks[1], bblocks[2]);
    graph->ConnectBasicBlocks(bblocks[2], bblocks[3]);
    graph->ConnectBasicBlocks(bblocks[2], bblocks[4]);
    graph->ConnectBasicBlocks(bblocks[3], bblocks[5]);
    graph->ConnectBasicBlocks(bblocks[3], bblocks[6]);
    graph->ConnectBasicBlocks(bblocks[4], bblocks[6]);
    graph->ConnectBasicBlocks(bblocks[6], bblocks[7]);
    graph->ConnectBasicBlocks(bblocks[7], bblocks[8]);
    graph->ConnectBasicBlocks(bblocks[7], bblocks[2]);
    graph->ConnectBasicBlocks(bblocks[8], bblocks[9]);
    graph->ConnectBasicBlocks(bblocks[8], bblocks[1]);

    return {graph, bblocks};
}

TestGraphSamples::LivenessInfoTuple TestGraphSamples::FillCase1() {
    auto [graph, bblocks] = BuildCase1();
    auto type = OperandType::I32;
    auto *instrBuilder = GetInstructionBuilder(graph);

    auto *arg0 = instrBuilder->CreateARG(type);
    auto *arg1 = instrBuilder->CreateARG(type);
    auto *constZero = instrBuilder->CreateCONST(type, 0);
    auto *constOne = instrBuilder->CreateCONST(type, 1);
    instrBuilder->PushBackInstruction(
        bblocks[0],
        arg0, arg1, constZero, constOne);

    auto *constCmp = instrBuilder->CreateCMP(type, CondCode::GE, constZero, constOne);
    auto *constJcmp = instrBuilder->CreateJCMP();
    instrBuilder->PushBackInstruction(bblocks[1], constCmp, constJcmp);

    auto *add = instrBuilder->CreateADD(type, arg0, constOne);
    instrBuilder->PushBackInstruction(bblocks[2], add);

    auto *cmp = instrBuilder->CreateCMP(type, CondCode::NE, arg0, constOne);
    auto *jcmp = instrBuilder->CreateJCMP();
    instrBuilder->PushBackInstruction(bblocks[5], cmp, jcmp);

    auto *addi = instrBuilder->CreateADDI(type, arg0, 2);
    instrBuilder->PushBackInstruction(bblocks[4], addi);

    auto *subi = instrBuilder->CreateSUBI(type, arg1, 3);
    instrBuilder->PushBackInstruction(bblocks[6], subi);

    auto *phi = instrBuilder->CreatePHI(
        type,
        {add, addi, subi},
        {bblocks[2], bblocks[4], bblocks[6]});
    auto *ret = instrBuilder->CreateRET(type, phi);
    instrBuilder->PushBackInstruction(bblocks[3], phi, ret);

    std::pmr::vector<LiveInterval> linearOrder{
        {{{30, 32}, {26, 28}, {2, 22}}, arg0},
        {{{4, 24}}, arg1},
        {{{6, 12}}, constZero},
        {{{26, 28}, {8, 18}}, constOne},
        {{{12, 14}}, constCmp},
        {{{14, 16}}, constJcmp},
        {{{18, 20}}, cmp},
        {{{20, 22}}, jcmp},
        {{{24, 26}}, subi},
        {{{28, 30}}, add},
        {{{32, 34}}, addi},
        {{{34, 36}}, phi},
        {{{36, 38}}, ret}};

    return {graph, bblocks, linearOrder};
}

TestGraphSamples::LivenessInfoTuple TestGraphSamples::FillCase2() {
    auto [graph, bblocks] = BuildCase2();
    auto type = OperandType::I32;
    auto *instrBuilder = GetInstructionBuilder(graph);

    auto *arg0 = instrBuilder->CreateARG(type);
    auto *arg1 = instrBuilder->CreateARG(type);
    auto *constZero = instrBuilder->CreateCONST(type, 0);
    auto *constOne = instrBuilder->CreateCONST(type, 1);
    auto *constTwo = instrBuilder->CreateCONST(type, 2);
    auto *constHundred = instrBuilder->CreateCONST(type, 100);
    auto *constHundredFive = instrBuilder->CreateCONST(type, 105);
    instrBuilder->PushBackInstruction(
        bblocks[0],
        arg0, arg1, constZero, constOne, constTwo, constHundred, constHundredFive);

    auto *phi0 = instrBuilder->CreatePHI(type, {arg0}, {bblocks[0]});
    auto *cmp1 = instrBuilder->CreateCMP(type, CondCode::LT, phi0, constZero);
    auto *jcmp1 = instrBuilder->CreateJCMP();
    instrBuilder->PushBackInstruction(bblocks[1], phi0, cmp1, jcmp1);

    auto *modi = instrBuilder->CreateMODI(type, phi0, 3);
    instrBuilder->PushBackInstruction(bblocks[9], modi);

    auto *phi1 = instrBuilder->CreatePHI(type, {phi0, modi}, {bblocks[1], bblocks[9]});
    auto *sub = instrBuilder->CreateSUB(type, constZero, phi1);
    auto *subi = instrBuilder->CreateSUBI(type, sub, 1);
    instrBuilder->PushBackInstruction(bblocks[2], phi1, sub, subi);

    auto *cmp2 = instrBuilder->CreateCMP(type, CondCode::EQ, subi, arg1);
    auto *jcmp2 = instrBuilder->CreateJCMP();
    instrBuilder->PushBackInstruction(bblocks[3], cmp2, jcmp2);

    auto *mul = instrBuilder->CreateMUL(type, subi, constTwo);
    instrBuilder->PushBackInstruction(bblocks[4], mul);

    auto *cmp3 = instrBuilder->CreateCMP(type, CondCode::LT, mul, constHundred);
    auto *jcmp3 = instrBuilder->CreateJCMP();
    instrBuilder->PushBackInstruction(bblocks[5], cmp3, jcmp3);

    auto *cmp4 = instrBuilder->CreateCMP(type, CondCode::LT, mul, constHundredFive);
    auto *jcmp4 = instrBuilder->CreateJCMP();
    instrBuilder->PushBackInstruction(bblocks[6], cmp4, jcmp4);

    auto *sub2 = instrBuilder->CreateSUB(type, mul, constHundredFive);
    instrBuilder->PushBackInstruction(bblocks[7], sub2);
    phi0->AddPhiInput(sub2, bblocks[7]);

    auto *mod = instrBuilder->CreateMOD(type, mul, constHundredFive);
    auto *ret = instrBuilder->CreateRET(type, mod);
    instrBuilder->PushBackInstruction(bblocks[8], mod, ret);

    // TODO: fill live intervals info
    return {graph, bblocks, {}};
}

TestGraphSamples::LivenessInfoTuple TestGraphSamples::FillCase4() {
    auto [graph, bblocks] = BuildCase4();
    auto type = OperandType::I32;
    auto *instrBuilder = GetInstructionBuilder(graph);

    auto *constOne = instrBuilder->CreateCONST(type, 1);
    auto *constTen = instrBuilder->CreateCONST(type, 10);
    auto *constTwenty = instrBuilder->CreateCONST(type, 20);
    instrBuilder->PushBackInstruction(
        bblocks[0],
        constOne, constTen, constTwenty);

    auto *phi1 = instrBuilder->CreatePHI(type, {constOne}, {bblocks[0]});
    auto *phi2 = instrBuilder->CreatePHI(type, {constTen}, {bblocks[0]});
    auto *cmpEq = instrBuilder->CreateCMP(type, CondCode::EQ, phi2, constOne);
    auto *jcmp = instrBuilder->CreateJCMP();
    instrBuilder->PushBackInstruction(
        bblocks[1],
        phi2, phi1, cmpEq, jcmp);

    auto *mul = instrBuilder->CreateMUL(type, phi1, phi2);
    auto *sub = instrBuilder->CreateSUB(type, phi2, constOne);
    instrBuilder->PushBackInstruction(
        bblocks[2],
        mul, sub);
    phi1->AddPhiInput(mul, bblocks[2]);
    phi2->AddPhiInput(sub, bblocks[2]);

    auto *add = instrBuilder->CreateADD(type, constTwenty, phi1);
    auto *retvoid = instrBuilder->CreateRETVOID();
    instrBuilder->PushBackInstruction(
        bblocks[3],
        add, retvoid);

    std::pmr::vector<LiveInterval> linearOrder{
        {{{2, 18}}, constOne},
        {{{4, 8}}, constTen},
        {{{20, 22}, {6, 14}}, constTwenty},
        {{{20, 22}, {8, 16}}, phi1},
        {{{8, 18}}, phi2},
        {{{10, 12}}, cmpEq},
        {{{12, 14}}, jcmp},
        {{{16, 20}}, mul},
        {{{18, 20}}, sub},
        {{{22, 24}}, add},
        {{{24, 26}}, retvoid}};

    return {graph, bblocks, linearOrder};
}

TestGraphSamples::LivenessInfoTuple TestGraphSamples::FillCase5() {
    auto [graph, bblocks] = BuildCase5();
    auto type = OperandType::I32;
    auto *instrBuilder = GetInstructionBuilder(graph);

    auto *arg0 = instrBuilder->CreateARG(type);
    auto *arg1 = instrBuilder->CreateARG(type);
    auto *arg2 = instrBuilder->CreateARG(type);
    auto *constZero = instrBuilder->CreateCONST(type, 0);
    auto *constOne = instrBuilder->CreateCONST(type, 1);
    auto *constTen = instrBuilder->CreateCONST(type, 10);
    auto *constTwenty = instrBuilder->CreateCONST(type, 20);
    instrBuilder->PushBackInstruction(
        bblocks[0],
        arg0, arg1, arg2, constZero, constOne, constTen, constTwenty);

    auto *phi1 = instrBuilder->CreatePHI(type, {arg0}, {bblocks[0]});
    auto *subi1 = instrBuilder->CreateSUBI(type, phi1, 1);
    instrBuilder->PushBackInstruction(bblocks[1], phi1, subi1);

    auto *phi2 = instrBuilder->CreatePHI(type, {arg1}, {bblocks[1]});
    auto *cmp2 = instrBuilder->CreateCMP(type, CondCode::EQ, subi1, phi2);
    auto *jcmp2 = instrBuilder->CreateJCMP();
    instrBuilder->PushBackInstruction(bblocks[2], phi2, cmp2, jcmp2);

    auto *modi3 = instrBuilder->CreateMODI(type, arg2, 3);
    auto *cmp3 = instrBuilder->CreateCMP(type, CondCode::EQ, subi1, phi2);
    auto *jcmp3 = instrBuilder->CreateJCMP();
    instrBuilder->PushBackInstruction(bblocks[3], modi3, cmp3, jcmp3);

    auto *subi5 = instrBuilder->CreateSUBI(type, modi3, 1);
    auto *ret5 = instrBuilder->CreateRET(type, subi5);
    instrBuilder->PushBackInstruction(bblocks[5], subi5, ret5);

    auto *subi4 = instrBuilder->CreateSUBI(type, phi2, 3);
    instrBuilder->PushBackInstruction(bblocks[4], subi4);

    auto *phi6 = instrBuilder->CreatePHI(type, {modi3, subi4}, {bblocks[3], bblocks[4]});
    auto *addi6 = instrBuilder->CreateADDI(type, phi6, 5);
    instrBuilder->PushBackInstruction(bblocks[6], phi6, addi6);
    phi2->AddPhiInput(addi6, bblocks[7]);

    auto *sub7 = instrBuilder->CreateSUB(type, constZero, addi6);
    auto *cmp7 = instrBuilder->CreateCMP(type, CondCode::GE, sub7, constOne);
    auto *jcmp7 = instrBuilder->CreateJCMP();
    instrBuilder->PushBackInstruction(bblocks[7], sub7, cmp7, jcmp7);
    phi1->AddPhiInput(sub7, bblocks[8]);

    auto *cmp8 = instrBuilder->CreateCMP(type, CondCode::EQ, subi1, constTen);
    auto *jcmp8 = instrBuilder->CreateJCMP();
    instrBuilder->PushBackInstruction(bblocks[8], cmp8, jcmp8);

    auto *ret9 = instrBuilder->CreateRET(type, subi1);
    instrBuilder->PushBackInstruction(bblocks[9], ret9);

    std::pmr::vector<LiveInterval> linearOrder{
        {{{2, 16}}, arg0},
        {{{4, 20}}, arg1},
        {{{30, 32}, {6, 26}}, arg2},
        {{{8, 44}}, constZero},
        {{{10, 46}}, constOne},
        {{{12, 52}}, constTen},
        {{{14, 16}}, constTwenty},
        {{{16, 18}}, phi1},
        {{{62, 64}, {18, 56}}, subi1},
        {{{30, 34}, {20, 28}}, phi2},
        {{{22, 24}}, cmp2},
        {{{24, 26}}, jcmp2},
        {{{28, 30}}, subi4},
        {{{56, 58}, {32, 38}}, modi3},
        {{{34, 36}}, cmp3},
        {{{36, 38}}, jcmp3},
        {{{38, 40}}, phi6},
        {{{40, 50}}, addi6},
        {{{44, 56}}, sub7},
        {{{46, 48}}, cmp7},
        {{{48, 50}}, jcmp7},
        {{{52, 54}}, cmp8},
        {{{54, 56}}, jcmp8},
        {{{58, 60}}, subi5},
        {{{60, 62}}, ret5},
        {{{64, 66}}, ret9}};

    return {graph, bblocks, linearOrder};
}
}   // namespace ir::tests
