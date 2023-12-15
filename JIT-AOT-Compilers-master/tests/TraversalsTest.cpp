#include "CompilerTestBase.h"
#include "Traversals.h"


namespace ir::tests {
class TraversalsTest : public CompilerTestBase {
};

TEST_F(TraversalsTest, TestDFO) {
    // create graph
    auto *graph = GetGraph();
    auto *blockA = graph->CreateEmptyBasicBlock();
    auto *blockB = graph->CreateEmptyBasicBlock();
    auto *blockC = graph->CreateEmptyBasicBlock();
    auto *blockD = graph->CreateEmptyBasicBlock();

    graph->SetFirstBasicBlock(blockA);
    graph->ConnectBasicBlocks(blockA, blockB);
    graph->ConnectBasicBlocks(blockA, blockC);
    graph->ConnectBasicBlocks(blockB, blockD);
    graph->ConnectBasicBlocks(blockC, blockD);

    std::vector<BasicBlock *> bblocks;
    bblocks.reserve(4);

    // do depth-first order traversal
    DFO::Traverse(graph, [&bblocks](BasicBlock *bblock) { bblocks.push_back(bblock); });
    ASSERT_EQ(bblocks.size(), 4);
    ASSERT_EQ(bblocks[0], blockD);
    ASSERT_TRUE(bblocks[1] == blockB || bblocks[1] == blockC);
    ASSERT_TRUE(bblocks[2] == blockB || bblocks[2] == blockC);
    ASSERT_EQ(bblocks[3], blockA);
}

TEST_F(TraversalsTest, TestRPO) {
    // create graph
    auto *graph = GetGraph();
    auto *blockA = graph->CreateEmptyBasicBlock();
    auto *blockB = graph->CreateEmptyBasicBlock();
    auto *blockC = graph->CreateEmptyBasicBlock();
    auto *blockD = graph->CreateEmptyBasicBlock();

    graph->SetFirstBasicBlock(blockA);
    graph->ConnectBasicBlocks(blockA, blockB);
    graph->ConnectBasicBlocks(blockA, blockC);
    graph->ConnectBasicBlocks(blockB, blockD);
    graph->ConnectBasicBlocks(blockC, blockD);

    // do reverse post-order traversal
    PassManager::Run<RPO>(graph);
    ASSERT_TRUE(graph->IsAnalysisValid(AnalysisFlag::RPO));

    auto rpoBlocks = graph->GetRPO();
    auto bblocks = std::vector(rpoBlocks.begin(), rpoBlocks.end());
    ASSERT_EQ(bblocks.size(), 4);
    ASSERT_EQ(bblocks[0], blockA);
    ASSERT_TRUE(bblocks[1] == blockB || bblocks[1] == blockC);
    ASSERT_TRUE(bblocks[2] == blockB || bblocks[2] == blockC);
    ASSERT_EQ(bblocks[3], blockD);

    // change layout and do RPO once again
    graph->ConnectBasicBlocks(blockD, blockA);

    PassManager::Run<RPO>(graph);
    ASSERT_TRUE(graph->IsAnalysisValid(AnalysisFlag::RPO));

    auto bblocks2 = graph->GetRPO();
    ASSERT_EQ(bblocks.size(), bblocks2.size());
    for (size_t i = 0; i < bblocks.size(); ++i) {
        ASSERT_EQ(bblocks[i], bblocks2[i]);
    }
}
}   // namespace ir::tests
