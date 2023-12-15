#ifndef JIT_AOT_COMPILERS_COURSE_DOM_TREE_H_
#define JIT_AOT_COMPILERS_COURSE_DOM_TREE_H_

#include "AllocatorUtils.h"
#include "DSU.h"
#include "PassBase.h"
#include <vector>


namespace ir {
class DominatorInfo final {
public:
    explicit DominatorInfo(std::pmr::memory_resource *mem) : dominated(mem) {}

    void SetDominator(BasicBlock *newIDom) {
        dominator = newIDom;
    }
    BasicBlock *GetDominator() {
        return dominator;
    }
    const BasicBlock *GetDominator() const {
        return dominator;
    }
    std::pmr::vector<BasicBlock *> &GetDominatedBlocks() {
        return dominated;
    }
    const std::pmr::vector<BasicBlock *> &GetDominatedBlocks() const {
        return dominated;
    }
    void AddDominatedBlock(BasicBlock *bblock) {
        ASSERT(bblock);
        dominated.push_back(bblock);
    }

private:
    BasicBlock *dominator = nullptr;
    std::pmr::vector<BasicBlock *> dominated;
};

// This pass implements Tarjan's algorithm for building Dominators Tree for the inputs graph
class DomTreeBuilder : public PassBase {
public:
    using VectorBBlocks = std::pmr::vector<BasicBlock *>;

    explicit DomTreeBuilder(Graph *graph)
        : PassBase(graph),
          idoms(graph->GetMemoryResource()),
          sdoms(graph->GetMemoryResource()),
          sdomsSet(graph->GetMemoryResource()),
          labels(graph->GetMemoryResource()),
          orderedBBlocks(graph->GetMemoryResource()),
          bblocksParents(graph->GetMemoryResource())
    {}
    NO_COPY_SEMANTIC(DomTreeBuilder);
    NO_MOVE_SEMANTIC(DomTreeBuilder);
    ~DomTreeBuilder() noexcept override = default;

    bool Run() override {
        return build<true>(nullptr);
    }
    auto Build() {
        std::pmr::vector<DominatorInfo> doms(graph->GetMemoryResource());
        build<false>(&doms);
        return doms;
    }

    const VectorBBlocks &GetImmediateDominators() const {
        return idoms;
    }

public:
    static constexpr AnalysisFlag SET_FLAG = AnalysisFlag::DOM_TREE;

private:
    template <bool InPlace>
    bool build(std::pmr::vector<DominatorInfo> *doms);

    template <bool InPlace>
    void computeIDoms(std::pmr::vector<DominatorInfo> *doms);

    DSU resetStructs();
    void dfsTraverse(BasicBlock *bblock);
    void computeSDoms(DSU &sdomsHelper);

    size_t getSize() const {
        return idoms.size();
    }

    BasicBlock *getImmDominator(size_t id) {
        ASSERT(id < idoms.size());
        return idoms[id];
    }
    void setImmDominator(size_t id, BasicBlock *bblock) {
        ASSERT(id < idoms.size());
        idoms[id] = bblock;
    }

    size_t getSemiDomNumber(BasicBlock *bblock) {
        ASSERT((bblock) && bblock->GetId() < sdoms.size());
        return sdoms[bblock->GetId()];
    }
    void setSemiDomNumber(BasicBlock *bblock, size_t visitNumber) {
        ASSERT((bblock) && bblock->GetId() < sdoms.size());
        sdoms[bblock->GetId()] = visitNumber;
    }

    const VectorBBlocks &getSemiDoms(BasicBlock *bblock) {
        ASSERT((bblock) && bblock->GetId() < sdomsSet.size());
        return sdomsSet[bblock->GetId()];
    }
    void registerSemiDom(BasicBlock *bblock) {
        auto semiDomNumber = getSemiDomNumber(bblock);
        ASSERT(getOrderedBlock(semiDomNumber)->GetId() < sdomsSet.size());
        sdomsSet[getOrderedBlock(semiDomNumber)->GetId()].push_back(bblock);
    }

    BasicBlock *getLabel(BasicBlock *bblock) {
        ASSERT((bblock) && bblock->GetId() < labels.size());
        return labels[bblock->GetId()];
    }

    BasicBlock *getOrderedBlock(size_t visitNumber) {
        ASSERT(visitNumber < orderedBBlocks.size());
        return orderedBBlocks[visitNumber];
    }
    void setOrderedBlock(size_t visitNumber, BasicBlock *bblock) {
        ASSERT((bblock) && visitNumber < orderedBBlocks.size());
        orderedBBlocks[visitNumber] = bblock;
    }

    BasicBlock *getBlockDFOParent(BasicBlock *bblock) {
        ASSERT((bblock) && bblock->GetId() < bblocksParents.size());
        return bblocksParents[bblock->GetId()];
    }
    void setBlockDFOParent(BasicBlock *child, BasicBlock *parent) {
        ASSERT((child) && (parent) && child->GetId() < bblocksParents.size());
        bblocksParents[child->GetId()] = parent;
    }

private:
    int lastNumber = 0;

    VectorBBlocks idoms;

    // mapping: node-id -> DFO-number of node's semidominator
    std::pmr::vector<size_t> sdoms;
    // mapping: node-id -> nodes dominated by this node-id
    std::pmr::vector<VectorBBlocks> sdomsSet;

    // nodes with min semidominators within DFO tree ancestors
    VectorBBlocks labels;
    // DFO basic blocks
    VectorBBlocks orderedBBlocks;
    // basic blocks' parents in DFO tree
    VectorBBlocks bblocksParents;
};

template <bool InPlace>
bool DomTreeBuilder::build(std::pmr::vector<DominatorInfo> *doms) {
    if (!graph->IsEmpty()) {
        auto sdomsHelper = resetStructs();

        dfsTraverse(graph->GetFirstBasicBlock());
        // check graph's connectivity
        ASSERT(lastNumber == static_cast<int>(graph->GetBasicBlocksCount()) - 1);
        computeSDoms(sdomsHelper);
        computeIDoms<InPlace>(doms);
    }
    return true;
}

template <bool InPlace>
void DomTreeBuilder::computeIDoms(std::pmr::vector<DominatorInfo> *doms) {
    for (size_t i = 1; i < getSize(); ++i) {
        auto *currentBlock = getOrderedBlock(i);
        auto currentBlockId = currentBlock->GetId();
        if (getImmDominator(currentBlockId) != getOrderedBlock(getSemiDomNumber(currentBlock))) {
            setImmDominator(currentBlockId,
                            getImmDominator(getImmDominator(currentBlockId)->GetId()));
        }

        auto *immDom = getImmDominator(currentBlockId);
        if constexpr (InPlace) {
            currentBlock->SetDominator(immDom);
            immDom->AddDominatedBlock(currentBlock);
        } else {
            ASSERT((doms) && currentBlock->GetId() < doms->size() && immDom->GetId() < doms->size());
            doms->at(currentBlock->GetId()).SetDominator(immDom);
            doms->at(immDom->GetId()).AddDominatedBlock(currentBlock);
        }
    }
}
}   // namespace ir

#endif  // JIT_AOT_COMPILERS_COURSE_DOM_TREE_H_
