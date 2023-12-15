#ifndef JIT_AOT_COMPILERS_COURSE_TRAVERSALS_H_
#define JIT_AOT_COMPILERS_COURSE_TRAVERSALS_H_

#include "BasicBlock.h"
#include <functional>
#include "PassBase.h"
#include <vector>


namespace ir {
class Graph;

template <typename CallbackT>
concept TraversalCallback = requires (CallbackT c, BasicBlock *b) {
    { c(b) } -> std::convertible_to<void>;
};

template <typename CallbackT>
concept ConstTraversalCallback = requires (CallbackT c, const BasicBlock *b) {
    { c(b) } -> std::convertible_to<void>;
};

template <typename CallbackT>
concept ValidCallback = TraversalCallback<CallbackT> || ConstTraversalCallback<CallbackT>;

template <typename GraphT>
concept GraphType = std::is_same_v<std::remove_cv_t<GraphT>, Graph>;

class DFO final {
public:
    template <GraphType GraphT, ValidCallback CallbackT, bool DoSafe = true>
    static void Traverse(GraphT *graph, CallbackT callback) {
        ASSERT(graph);
        if (graph->IsEmpty()) {
            return;
        }

        DFO helper(graph);
        helper.doTraverse(graph->GetFirstBasicBlock(), callback);
        if constexpr (DoSafe) {
            ASSERT(helper.visitedCounter == graph->GetBasicBlocksCount());
        }
    }

private:
    DFO(const Graph *graph) : markerMgr(graph) {}
    DEFAULT_COPY_SEMANTIC(DFO);
    DEFAULT_MOVE_SEMANTIC(DFO);
    DEFAULT_DTOR(DFO);

    template <typename BBlockT, ValidCallback CallbackT>
    void doTraverse(BBlockT *bblock, CallbackT callback)
    requires std::is_same_v<std::remove_cv_t<BBlockT>, BasicBlock>
    {
        visitedCounter = 0;
        auto visited = markerMgr->GetNewMarker();
        traverseImpl(bblock, visited, callback);
        markerMgr->ReleaseMarker(visited);
    }

    template <typename BBlockT, ValidCallback CallbackT>
    void traverseImpl(BBlockT *bblock, Marker visited, CallbackT callback)
    requires std::is_same_v<std::remove_cv_t<BBlockT>, BasicBlock>
    {
        ASSERT(bblock);
        ++visitedCounter;
        bblock->SetMarker(visited);
        for (auto *succ : bblock->GetSuccessors()) {
            ASSERT(succ->HasPredecessor(bblock));
            if (!succ->IsMarkerSet(visited)) {
                traverseImpl(succ, visited, callback);
            }
        }
        callback(bblock);
    }

private:
    const MarkerManager *markerMgr;
    size_t visitedCounter;
};

// concepts and helpers
template <GraphType GraphT>
struct BasicBlockTypeHelper {
    using type = const BasicBlock;
};

template <>
struct BasicBlockTypeHelper<Graph> {
    using type = BasicBlock;
};

template <GraphType GraphT>
using BasicBlockType = typename BasicBlockTypeHelper<GraphT>::type;

class RPO : public PassBase {
public:
    RPO(Graph *graph) : PassBase(graph) {}
    NO_COPY_SEMANTIC(RPO);
    NO_MOVE_SEMANTIC(RPO);
    ~RPO() noexcept override = default;

    bool Run() override {
        graph->SetRPO(std::move(DoRPO(graph)));
        return true;
    }

    template <GraphType GraphT>
    static std::pmr::vector<BasicBlockType<GraphT> *> DoRPO(GraphT *graph) {
        ASSERT(graph);
        std::pmr::vector<BasicBlockType<GraphT> *> result(graph->GetMemoryResource());
        if (graph->IsEmpty()) {
            return result;
        }

        result.reserve(graph->GetBasicBlocksCount());
        DFO::Traverse(graph, [&result](BasicBlockType<GraphT> *bblock){ result.push_back(bblock); });
        ASSERT(result.size() == graph->GetBasicBlocksCount());
        std::reverse(result.begin(), result.end());
        return result;
    }

public:
    static constexpr AnalysisFlag SET_FLAG = AnalysisFlag::RPO;
};

void DumpGraphRPO(const Graph *graph);

void DumpGraphDFO(const Graph *graph);
}   // namespace ir

#endif  // JIT_AOT_COMPILERS_COURSE_TRAVERSALS_H_
