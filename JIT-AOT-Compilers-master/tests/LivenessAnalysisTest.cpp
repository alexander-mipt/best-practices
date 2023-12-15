#include "LivenessAnalyzer.h"
#include "TestGraphSamples.h"


namespace ir::tests {
class LivenessAnalysisTest : public TestGraphSamples {
};

template <typename AllocT>
static void checkLinearOrder(std::vector<LiveInterval, AllocT> &&expectedLinearOrder,
                             const LiveIntervals &liveIntervals)
{
    ASSERT_EQ(liveIntervals.Size(), expectedLinearOrder.size());
    for (int i = 0, end = liveIntervals.Size(); i < end; ++i) {
        ASSERT_EQ(liveIntervals[i]->GetInstruction(), expectedLinearOrder[i].GetInstruction());
        ASSERT_EQ(*(liveIntervals[i]), expectedLinearOrder[i]);
    }
}

TEST_F(LivenessAnalysisTest, TestAnalysis4) {
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
    auto [graph, bblocks, expectedLinearOrder] = FillCase4();

    PassManager::Run<LivenessAnalyzer>(graph);

    checkLinearOrder(std::move(expectedLinearOrder), graph->GetLiveIntervals());
}

TEST_F(LivenessAnalysisTest, TestAnalysis1) {
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
    auto [graph, bblocks, linearOrder] = FillCase1();

    PassManager::Run<LivenessAnalyzer>(graph);

    checkLinearOrder(std::move(linearOrder), graph->GetLiveIntervals());
}

TEST_F(LivenessAnalysisTest, TestAnalysis5) {
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

    PassManager::Run<LivenessAnalyzer>(graph);

    checkLinearOrder(std::move(linearOrder), graph->GetLiveIntervals());
}
}   // namespace ir::tests
