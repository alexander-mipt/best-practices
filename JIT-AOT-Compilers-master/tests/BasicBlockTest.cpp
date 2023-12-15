#include "CompilerTestBase.h"


namespace ir::tests {
class BasicBlockTest : public CompilerTestBase {
};

TEST_F(BasicBlockTest, TestBasicBlock1) {
    auto *bblock = GetGraph()->CreateEmptyBasicBlock();
    ASSERT_NE(bblock, nullptr);
    ASSERT_EQ(bblock->GetFirstInstruction(), nullptr);
    ASSERT_EQ(bblock->GetLastInstruction(), nullptr);
    ASSERT_EQ(bblock->GetGraph(), GetGraph());

    auto opType = OperandType::I32;
    auto *mul = GetInstructionBuilder()->CreateMUL(opType, nullptr, nullptr);

    // add 1st instruction
    bblock->PushBackInstruction(mul);
    ASSERT_EQ(mul->GetBasicBlock(), bblock);
    ASSERT_EQ(bblock->GetFirstInstruction(), mul);
    ASSERT_EQ(bblock->GetLastInstruction(), mul);

    auto *addi1 = GetInstructionBuilder()->CreateADDI(opType, nullptr, 32);
    auto *addi2 = GetInstructionBuilder()->CreateADDI(opType, nullptr, 32);

    // add 2nd instruction into start of the basic block
    bblock->PushForwardInstruction(addi2);
    ASSERT_EQ(addi2->GetBasicBlock(), bblock);
    ASSERT_EQ(bblock->GetFirstInstruction(), addi2);
    ASSERT_EQ(bblock->GetLastInstruction(), mul);

    // add 3rd instruction into start of the basic block
    bblock->PushForwardInstruction(addi1);
    ASSERT_EQ(addi1->GetBasicBlock(), bblock);
    ASSERT_EQ(bblock->GetFirstInstruction(), addi1);
    ASSERT_EQ(bblock->GetLastInstruction(), mul);
    ASSERT_EQ(addi1->GetNextInstruction(), addi2);
}

TEST_F(BasicBlockTest, TestBasicBlock2) {
    auto *bblock = GetGraph()->CreateEmptyBasicBlock();

    auto opType = OperandType::I32;
    auto *mul = GetInstructionBuilder()->CreateMUL(opType, nullptr, nullptr);
    auto *addi1 = GetInstructionBuilder()->CreateADDI(opType, nullptr, 32);
    auto *addi2 = GetInstructionBuilder()->CreateADDI(opType, nullptr, 32);

    // add all 3 instructions
    GetInstructionBuilder()->PushBackInstruction(bblock, addi1, addi2, mul);
    // check correct order
    ASSERT_EQ(bblock->GetFirstInstruction(), addi1);
    ASSERT_EQ(addi1->GetNextInstruction(), addi2);
    ASSERT_EQ(addi2->GetNextInstruction(), mul);
    ASSERT_EQ(bblock->GetLastInstruction(), mul);
}

TEST_F(BasicBlockTest, TestBasicBlock3) {
    auto opType = OperandType::I32;
    auto *mul = GetInstructionBuilder()->CreateMUL(opType, nullptr, nullptr);
    auto *addi1 = GetInstructionBuilder()->CreateADDI(opType, nullptr, 32);
    auto *addi2 = GetInstructionBuilder()->CreateADDI(opType, nullptr, 32);

    // add all 3 instructions
    auto *bblock = GetGraph()->CreateEmptyBasicBlock();
    GetInstructionBuilder()->PushBackInstruction(bblock, addi1, addi2, mul);
    // unlink addi2 (2/3 instruction)
    bblock->UnlinkInstruction(addi2);
    ASSERT_EQ(addi2->GetBasicBlock(), nullptr);
    ASSERT_EQ(addi2->GetPrevInstruction(), nullptr);
    ASSERT_EQ(addi2->GetNextInstruction(), nullptr);
    ASSERT_EQ(bblock->GetFirstInstruction(), addi1);
    ASSERT_EQ(addi1->GetNextInstruction(), mul);
    ASSERT_EQ(bblock->GetLastInstruction(), mul);
    ASSERT_EQ(mul->GetPrevInstruction(), addi1);

    // unlink mul
    bblock->UnlinkInstruction(mul);
    ASSERT_EQ(mul->GetBasicBlock(), nullptr);
    ASSERT_EQ(mul->GetPrevInstruction(), nullptr);
    ASSERT_EQ(mul->GetNextInstruction(), nullptr);
    ASSERT_EQ(bblock->GetFirstInstruction(), addi1);
    ASSERT_EQ(bblock->GetLastInstruction(), addi1);
    ASSERT_EQ(addi1->GetPrevInstruction(), nullptr);
    ASSERT_EQ(addi1->GetNextInstruction(), nullptr);
}

TEST_F(BasicBlockTest, TestSplitAfterInstruction) {
    // callValue = foo(arg0, arg1)
    // divi = callValue / 3
    // if (divi >= 10) {
    //     muli = callValue * 2
    //     return muli
    // } else {
    //     return divi
    // }
    auto *instrBuilder = GetInstructionBuilder();
    auto type = OperandType::I16;
    auto *graph = GetGraph();

    auto *firstBlock = graph->CreateEmptyBasicBlock();
    graph->SetFirstBasicBlock(firstBlock);
    auto *arg0 = instrBuilder->CreateARG(type);
    auto *arg1 = instrBuilder->CreateARG(type);
    auto *constTen = instrBuilder->CreateCONST(type, 10);
    instrBuilder->PushBackInstruction(firstBlock, arg0, arg1, constTen);

    auto *splittedBlock = graph->CreateEmptyBasicBlock();
    graph->ConnectBasicBlocks(firstBlock, splittedBlock);
    std::pmr::vector<Input> args(GetGraph()->GetMemoryResource());
    args.push_back(arg0);
    args.push_back(arg1);
    auto *call = instrBuilder->CreateCALL(type, INVALID_FUNCTION_ID, args);
    auto *divi = instrBuilder->CreateDIVI(type, call, 3);
    auto *cmp = instrBuilder->CreateCMP(type, CondCode::GE, divi, constTen);
    auto *jcmp = instrBuilder->CreateJCMP();
    instrBuilder->PushBackInstruction(splittedBlock, call, divi, cmp, jcmp);

    auto *trueBranch = graph->CreateEmptyBasicBlock(true);
    graph->ConnectBasicBlocks(splittedBlock, trueBranch);
    auto *muli = instrBuilder->CreateMULI(type, call, 2);
    auto *ret1 = instrBuilder->CreateRET(type, muli);
    instrBuilder->PushBackInstruction(trueBranch, muli, ret1);

    auto *falseBranch = graph->CreateEmptyBasicBlock(true);
    graph->ConnectBasicBlocks(splittedBlock, falseBranch);
    auto *ret2 = instrBuilder->CreateRET(type, divi);
    instrBuilder->PushBackInstruction(falseBranch, ret2);
    auto bblocksCount = graph->GetBasicBlocksCount();

    auto *newBlock = call->GetBasicBlock()->SplitAfterInstruction(call, false);

    ASSERT_EQ(graph->GetBasicBlocksCount(), bblocksCount + 1);
    ASSERT_EQ(call->GetNextInstruction(), nullptr);
    ASSERT_TRUE(splittedBlock->GetSuccessors().empty());
    ASSERT_EQ(splittedBlock->GetSize(), 1);
    ASSERT_EQ(splittedBlock->GetFirstInstruction(), call);
    ASSERT_EQ(splittedBlock->GetLastInstruction(), call);
    ASSERT_TRUE(newBlock->GetPredecessors().empty());
    ASSERT_EQ(newBlock->GetSize(), 3);
    ASSERT_EQ(newBlock->GetFirstInstruction(), divi);
    ASSERT_EQ(divi->GetPrevInstruction(), nullptr);
    ASSERT_EQ(divi->GetNextInstruction(), cmp);
    ASSERT_EQ(newBlock->GetLastInstruction(), jcmp);
    ASSERT_EQ(jcmp->GetPrevInstruction(), cmp);
    ASSERT_EQ(jcmp->GetNextInstruction(), nullptr);
}
}   // namespace ir::tests
