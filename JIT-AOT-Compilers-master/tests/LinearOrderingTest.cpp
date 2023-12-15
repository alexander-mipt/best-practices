#include <algorithm>
#include "LinearOrdering.h"
#include <numeric>
#include "TestGraphSamples.h"


namespace ir::tests {
class LinearOrderingTest : public TestGraphSamples {
};

template <typename BasicBlockT>
concept BasicBlockType = std::is_same_v<std::remove_cv_t<BasicBlockT>, BasicBlock>;

static std::pmr::vector<const BasicBlock *> extractLinearOrder(const Graph *graph) {
    std::pmr::vector<const BasicBlock *> linearOrder(graph->GetMemoryResource());
    linearOrder.reserve(graph->GetBasicBlocksCount());

    graph->ForEachBasicBlock([&linearOrder](const BasicBlock *b) { linearOrder.push_back(b); });
    return linearOrder;
}

template <BasicBlockType LhsBlockT, typename AllocLhsT, BasicBlockType RhsBlockT, typename AllocRhsT>
static bool checkLinearOrderSafe(const std::vector<RhsBlockT *, AllocLhsT> &linearOrder,
                                 std::vector<LhsBlockT *, AllocRhsT> &&expectedOrder)
{
    if (linearOrder.size() != expectedOrder.size()) {
        return false;
    }
    std::vector<int> indexes(linearOrder.size());
    std::iota(indexes.begin(), indexes.end(), 0);
    return std::all_of(indexes.begin(),
                       indexes.end(),
                       [exp = std::move(expectedOrder), act = std::move(linearOrder)](int i) {
        return exp[i] == act[i];
    });
}

template <BasicBlockType BasicBlockT, typename AllocLhsT, typename AllocRhsT>
static void checkLinearOrder(std::vector<BasicBlockT *, AllocLhsT> &&linearOrder,
                             std::vector<BasicBlockT *, AllocRhsT> &&expectedOrder)
{
    ASSERT_EQ(linearOrder.size(), expectedOrder.size());
    for (int i = 0, end = linearOrder.size(); i < end; ++i) {
        ASSERT_EQ(linearOrder[i], expectedOrder[i]);
    }
}

template <BasicBlockType LhsBlockT, typename AllocLhsT, BasicBlockType... RhsBlockT, typename... AllocRhsT>
static void checkLinearOrder(const std::vector<LhsBlockT *, AllocLhsT> &linearOrder,
                             std::vector<RhsBlockT *, AllocRhsT> &&... expectedOrder)
{
    std::bitset<sizeof...(AllocRhsT)> matchMask{false};
    ([i = 0, match = &matchMask, actual = std::as_const(linearOrder), expected = std::move(expectedOrder)]() mutable {
        match[i++] = checkLinearOrderSafe(actual, std::move(expected));
    } (), ...);
    ASSERT_EQ(matchMask.count(), 1);
}

TEST_F(LinearOrderingTest, TestAnalysis1) {
    /*
         B
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
    auto [graph, bblocks, instrsLinearOrder] = FillCase4();

    PassManager::Run<LinearOrdering>(graph);

    checkLinearOrder(std::move(extractLinearOrder(graph)), std::move(bblocks));
}

TEST_F(LinearOrderingTest, TestAnalysis2) {
    /*
       B0
       |
       B1
      / \
     /   \
    B2    B5
    |    / \
    |   B4  \
    |  /    |
    | /     |
    B3<-----B6
    */
    auto [graph, bblocks, linearOrder] = FillCase1();

    PassManager::Run<LinearOrdering>(graph);

    checkLinearOrder(
        std::move(extractLinearOrder(graph)),
        std::vector<const BasicBlock *>{bblocks[0],
            bblocks[1],
            bblocks[5],
            bblocks[6],
            bblocks[4],
            bblocks[2],
            bblocks[3]},
        std::vector<const BasicBlock *>{bblocks[0],
            bblocks[1],
            bblocks[5],
            bblocks[6],
            bblocks[2],
            bblocks[4],
            bblocks[3]});
}

TEST_F(LinearOrderingTest, TestAnalysis3) {
    /*
        B0
        |
    --->B1->B9
    |   |  /
    |   | /
    |   |/
    |   B2<-
    |   |  |
    |   B3--
    |   |
    |   B4<-
    |   |  |
    |   B5--
    |   |
    B7<-B6-->B8-->B10
    */
    auto [graph, bblocks, linearOrder] = FillCase2();

    PassManager::Run<LinearOrdering>(graph);

    checkLinearOrder(
        std::move(extractLinearOrder(graph)),
        std::vector<const BasicBlock *>{bblocks[0],
            bblocks[1],
            bblocks[9],
            bblocks[2],
            bblocks[3],
            bblocks[4],
            bblocks[5],
            bblocks[6],
            bblocks[7],
            bblocks[8],
            bblocks[10]});
}

TEST_F(LinearOrderingTest, TestAnalysis4) {
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
    auto [graph, bblocks, linearOrder] = FillCase5();

    PassManager::Run<LinearOrdering>(graph);

    checkLinearOrder(
        std::move(extractLinearOrder(graph)),
        std::vector<const BasicBlock *>{bblocks[0],
            bblocks[1],
            bblocks[2],
            bblocks[4],
            bblocks[3],
            bblocks[6],
            bblocks[7],
            bblocks[8],
            bblocks[5],
            bblocks[9]});
}
}   // namespace ir::tests
