#include "DomTree.h"
#include "GraphChecker.h"
#include "InstructionBuilder.h"
#include "LinearOrdering.h"
#include "LoopAnalyzer.h"


namespace ir {
bool LinearOrdering::Run() {
    PassManager::Run<DomTreeBuilder>(graph);
    PassManager::Run<LoopAnalyzer>(graph);

    cfgChanged = false;

    std::pmr::vector<BasicBlock *> newOrder(graph->GetMemoryResource());
    newOrder.reserve(graph->GetBasicBlocksCount());
    orderBlocks(newOrder);

    setNewOrder(std::move(newOrder));
    if (cfgChanged) {
        postOrder();
    }
    return true;
}

void LinearOrdering::orderBlocks(std::pmr::vector<BasicBlock *> &newOrder) {
    visitedMarker = graph->GetNewMarker();

    std::pmr::list<BasicBlock *> remainedBlocks(graph->GetMemoryResource());
    remainedBlocks.push_back(graph->GetFirstBasicBlock());

    while (!remainedBlocks.empty()) {
        auto *bblock = remainedBlocks.front();
        ASSERT(bblock);
        ASSERT(!isVisited(bblock));
        remainedBlocks.pop_front();

        bblock->SetMarker(visitedMarker);
        newOrder.push_back(bblock);

        auto &succs = bblock->GetSuccessors();
        if (succs.size() == 1
            && !isVisited(succs[0])
            && !unvisitedForwardEdgesExist(succs[0]))
        {
            remainedBlocks.push_front(succs[0]);
        } else if (succs.size() == 2) {
            queueCond(remainedBlocks, bblock);
        }
    }

    graph->ReleaseMarker(visitedMarker);
    visitedMarker = utils::to_underlying(MarkersConstants::UNDEF_VALUE);
}

void LinearOrdering::queueCond(std::pmr::list<BasicBlock *> &remainedBlocks,
                               BasicBlock *bblock) {
    ASSERT(bblock);
    auto *loop = bblock->GetLoop();
    ASSERT(loop);
    auto &succs = bblock->GetSuccessors();
    ASSERT(succs.size() == 2);
    auto *trueBranch = succs[0];
    auto *falseBranch = succs[1];
    ASSERT((trueBranch) && (falseBranch));

    if (isVisited(falseBranch) && isVisited(trueBranch)) {
        remainedBlocks.push_front(insertJmpBlock(bblock, falseBranch));
    } else if (mustInverseCondition(bblock)) {
        bblock->InverseConditionalBranch();
        queueCond(remainedBlocks, bblock);
    } else {
        if (falseBranch->GetLoop() != loop) {
            addIntoQueue(remainedBlocks, falseBranch);
        } else {
            remainedBlocks.push_front(falseBranch);
        }
        addIntoQueue(remainedBlocks, trueBranch);
    }
}

void LinearOrdering::addIntoQueue(std::pmr::list<BasicBlock *> &remainedBlocks,
                                  BasicBlock *bblock) {
    if (isVisited(bblock) || unvisitedForwardEdgesExist(bblock)) {
        return;
    }
    ASSERT(bblock);
    auto *loop = bblock->GetLoop();
    ASSERT(loop);
    auto insertPos = std::find_if(remainedBlocks.begin(),
                                  remainedBlocks.end(),
                                  [loop](const BasicBlock *b) {
        return loop->IsIn(b->GetLoop());
    });
    remainedBlocks.insert(insertPos, bblock);
}

// Utility function used to place false successor immediately after conditional branch
bool LinearOrdering::mustInverseCondition(BasicBlock *bblock) {
    ASSERT(bblock);
    auto *loop = bblock->GetLoop();
    ASSERT(loop);
    auto &succs = bblock->GetSuccessors();
    ASSERT(succs.size() == 2);
    auto *trueBranch = succs[0];
    auto *falseBranch = succs[1];

    if (isVisited(trueBranch)) {
        // no need to inverse when true branch was already visited
        return false;
    }
    return isVisited(falseBranch)
        || (falseBranch->GetLoop() != loop
            && trueBranch->GetLoop() == loop)
        || (unvisitedForwardEdgesExist(falseBranch)
            && !unvisitedForwardEdgesExist(trueBranch));
}

bool LinearOrdering::unvisitedForwardEdgesExist(BasicBlock *bblock) {
    ASSERT(bblock);
    if (bblock->GetLoop()->IsIrreducible()) {
        return false;
    }

    // here we ensure that all predecessors come before the given block
    auto preds = bblock->GetPredecessors();
    if (!bblock->IsLoopHeader()) {
        return std::any_of(preds.begin(), preds.end(), [this](auto pred) {
            return !isVisited(pred);
        });
    }
    return std::any_of(preds.begin(), preds.end(), [bblock, this](auto pred) {
        return !bblock->Dominates(pred) && !isVisited(pred);
    });
}

BasicBlock *LinearOrdering::insertJmpBlock(BasicBlock *pred, BasicBlock *succ) {
    bool isTerminal = (succ == graph->GetLastBasicBlock());
    auto *newBlock = graph->CreateEmptyBasicBlock(isTerminal);
    newBlock->PushBackInstruction(graph->GetInstructionBuilder()->CreateJMP());

    pred->GetLoop()->AddBasicBlock(newBlock);
    graph->InsertBetween(newBlock, pred, succ);

    cfgChanged = true;
    return newBlock;
}

void LinearOrdering::setNewOrder(std::pmr::vector<BasicBlock *> &&newOrder) {
    for (size_t i = 0, count = newOrder.size(); i < count; ++i) {
        newOrder[i]->SetId(i);
    }

    graph->bblocks = std::move(newOrder);
    graph->unlinkedInstructionsCounter = 0;
}

void LinearOrdering::postOrder() {
    ASSERT(PassManager::Run<GraphChecker>(graph));

    PassManager::SetInvalid<AnalysisFlag::DOM_TREE, AnalysisFlag::RPO>(graph);
}
}   // namespace ir
