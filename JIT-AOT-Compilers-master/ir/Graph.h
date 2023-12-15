#ifndef JIT_AOT_COMPILERS_COURSE_GRAPH_H_
#define JIT_AOT_COMPILERS_COURSE_GRAPH_H_

#include "AnalysisValidityManager.h"
#include <algorithm>
#include "AllocatorUtils.h"
#include "BasicBlock.h"
#include "LiveAnalysisStructs.h"
#include "macros.h"
#include "marker/marker.h"
#include <ranges>


namespace ir {
class CompilerBase;
class InstructionBuilder;
class LinearOrdering;
class LivenessAnalyzer;
class Loop;

class Graph : public MarkerManager, public AnalysisValidityManager {
public:
    using IdType = FunctionId;

    Graph(CompilerBase *compiler, std::pmr::memory_resource *mem, InstructionBuilder *instrBuilder)
        : compiler(compiler),
          firstBlock(nullptr),
          lastBlock(nullptr),
          bblocks(mem),
          rpoBlocks(mem),
          loopTreeRoot(nullptr),
          instrBuilder(instrBuilder),
          liveIntervals(mem),
          memResource(mem)
    {
        ASSERT(compiler);
        ASSERT(memResource);
        ASSERT(instrBuilder);
    }
    NO_COPY_SEMANTIC(Graph);
    NO_MOVE_SEMANTIC(Graph);
    virtual DEFAULT_DTOR(Graph);

    IdType GetId() const {
        return id;
    }
    void SetId(IdType newId) {
        id = newId;
    }

    CompilerBase *GetCompiler() {
        return compiler;
    }

    std::pmr::memory_resource *GetMemoryResource() const {
        return memResource;
    }
    template <typename T, typename... ArgsT>
    [[nodiscard]] T *New(ArgsT&&... args) const {
        return utils::template New<T>(GetMemoryResource(), std::forward<ArgsT>(args)...);
    }

    BasicBlock *GetFirstBasicBlock() {
        return firstBlock;
    }
    const BasicBlock *GetFirstBasicBlock() const {
        return firstBlock;
    }
    BasicBlock *GetLastBasicBlock() {
        return lastBlock;
    }
    const BasicBlock *GetLastBasicBlock() const {
        return lastBlock;
    }
    size_t GetBasicBlocksCount() const {
        return bblocks.size() - unlinkedInstructionsCounter;
    }
    BasicBlock::IdType GetMaximumBlockId() const {
        return bblocks.size() - 1;
    }
    bool IsEmpty() const {
        return GetBasicBlocksCount() == 0;
    }
    // O(1) complexity.
    BasicBlock *FindBasicBlock(BasicBlock::IdType id) {
        ASSERT(id < bblocks.size());
        return bblocks[id];
    }
    // O(1) complexity.
    const BasicBlock *FindBasicBlock(BasicBlock::IdType id) const {
        ASSERT(id < bblocks.size());
        return bblocks[id];
    }

    auto GetRPO() const {
        ASSERT(IsAnalysisValid(AnalysisFlag::RPO));
        return std::span{rpoBlocks};
    }
    void SetRPO(std::pmr::vector<BasicBlock *> &&rpo) {
        rpoBlocks = std::move(rpo);
    }

    Loop *GetLoopTree() {
        return loopTreeRoot;
    }
    const Loop *GetLoopTree() const {
        return loopTreeRoot;
    }

    InstructionBuilder *GetInstructionBuilder() {
        return instrBuilder;
    }

    LiveIntervals &GetLiveIntervals() {
        return liveIntervals;
    }
    const LiveIntervals &GetLiveIntervals() const {
        return liveIntervals;
    }

    void SetFirstBasicBlock(BasicBlock *bblock) {
        firstBlock = bblock;
        invalidateAfterChangedCFG();
    }
    // Use this method carefully due to special meaning of last basic block.
    void SetLastBasicBlock(BasicBlock *bblock) {
        lastBlock = bblock;
        invalidateAfterChangedCFG();
    }
    void SetLoopTree(Loop *loop) {
        loopTreeRoot = loop;
    }

    // Applies the function to each basic block, in order.
    // Assume either no side-effects are applied on basic blocks or user knows what he does.
    template <typename FunctionType>
    requires UnaryFunctionType<FunctionType, BasicBlock *, void>
    void ForEachBasicBlock(FunctionType function) {
        auto nonNullPredicate = [](BasicBlock *bblock) { return bblock != nullptr; };
        std::ranges::for_each(
            std::views::all(bblocks) | std::views::filter(nonNullPredicate),
            function);
    }

    // Applies the function to each basic block, in order.
    template <typename FunctionType>
    requires UnaryFunctionType<FunctionType, const BasicBlock *, void>
    void ForEachBasicBlock(FunctionType function) const {
        auto nonNullPredicate = [](const BasicBlock *bblock) { return bblock != nullptr; };
        std::ranges::for_each(
            std::views::all(bblocks) | std::views::filter(nonNullPredicate),
            function);
    }

    // O(NumberOfBasicBlocks) complexity.
    size_t CountInstructions() const;

    BasicBlock *CreateEmptyBasicBlock(bool isTerminal = false);
    void ConnectBasicBlocks(BasicBlock *lhs, BasicBlock *rhs);
    void DisconnectBasicBlocks(BasicBlock *lhs, BasicBlock *rhs);
    void InsertBetween(BasicBlock *bblock, BasicBlock *pred, BasicBlock *succ);
    void FixPHIAfterDisconnect(BasicBlock *phiSource, BasicBlock *phiTarget);
    void AddBasicBlock(BasicBlock *bblock);
    void AddBasicBlockBefore(BasicBlock *before, BasicBlock *bblock);

    void UnlinkBasicBlock(BasicBlock *bblock);
    // Unlinks basic block without removing its edges.
    // Use this method only if all adjacent blocks will be deleted immediately after.
    void UnlinkBasicBlockRaw(BasicBlock *bblock);

    void RemoveUnlinkedBlocks();

    bool VerifyFirstBlock() const;

public:
    static constexpr IdType INVALID_ID = static_cast<IdType>(-1);

    // the classes needs access to `bblocks` field
    friend class LinearOrdering;

private:
    void removePredecessors(BasicBlock *bblock);
    void removeSuccessors(BasicBlock *bblock);
    void invalidateAfterChangedCFG();

private:
    IdType id = INVALID_ID;

    CompilerBase *compiler;

    // first basic block contains ONLY all arguments and all constants for the graph
    BasicBlock *firstBlock;
    // last basic block collects all control flow exits from the graph
    BasicBlock *lastBlock;
    std::pmr::vector<BasicBlock *> bblocks;
    size_t unlinkedInstructionsCounter = 0;

    std::pmr::vector<BasicBlock *> rpoBlocks;

    Loop *loopTreeRoot;

    InstructionBuilder *instrBuilder;

    LiveIntervals liveIntervals;

    mutable std::pmr::memory_resource *memResource;
};
}   // namespace ir

#endif  // JIT_AOT_COMPILERS_COURSE_GRAPH_H_
