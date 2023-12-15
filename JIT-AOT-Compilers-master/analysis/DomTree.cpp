#include "DomTree.h"
#include <numeric>
#include "Traversals.h"


namespace ir {
DSU DomTreeBuilder::resetStructs() {
    graph->ForEachBasicBlock([](BasicBlock *bblock) {
        bblock->ClearDominatedBlocks();
        bblock->SetDominator(nullptr);
    });

    lastNumber = -1;

    auto bblocksCount = graph->GetMaximumBlockId() + 1;
    sdoms.resize(bblocksCount, BasicBlock::INVALID_ID);
    sdomsSet.resize(bblocksCount);
    idoms.resize(bblocksCount, nullptr);
    labels.resize(bblocksCount, nullptr);
    orderedBBlocks.resize(bblocksCount, nullptr);
    bblocksParents.resize(bblocksCount, nullptr);

    return DSU(labels, sdoms, graph->GetMemoryResource());
}

void DomTreeBuilder::dfsTraverse(BasicBlock *bblock) {
    ++lastNumber;
    ASSERT((bblock) && (lastNumber < static_cast<int>(getSize())));

    ASSERT(bblock->GetId() < labels.size());
    labels[bblock->GetId()] = bblock;
    setSemiDomNumber(bblock, lastNumber);
    setOrderedBlock(lastNumber, bblock);

    for (auto *succ : bblock->GetSuccessors()) {
        if (getLabel(succ) == nullptr) {
            setBlockDFOParent(succ, bblock);
            dfsTraverse(succ);
        }
    }
}

void DomTreeBuilder::computeSDoms(DSU &sdomsHelper) {
    for (int i = getSize() - 1; i >= 0; --i) {
        auto *currentBlock = getOrderedBlock(i);

        for (const auto &pred : currentBlock->GetPredecessors()) {
            auto nodeWithMinLabel = sdomsHelper.Find(pred);
            auto id = std::min(
                getSemiDomNumber(currentBlock), getSemiDomNumber(nodeWithMinLabel));
            setSemiDomNumber(currentBlock, id);
        }

        if (i > 0) {
            registerSemiDom(currentBlock);
            sdomsHelper.Unite(currentBlock, getBlockDFOParent(currentBlock));
        }

        for (auto dominatee : getSemiDoms(currentBlock)) {
            auto minSDom = sdomsHelper.Find(dominatee);

            auto dominateeId = dominatee->GetId();
            if (getSemiDomNumber(minSDom) == getSemiDomNumber(dominatee)) {
                setImmDominator(dominateeId, getOrderedBlock(getSemiDomNumber(dominatee)));
            } else {
                setImmDominator(dominateeId, minSDom);
            }
        }
    }
}
}   // namespace ir
