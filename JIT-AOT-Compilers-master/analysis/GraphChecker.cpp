#include "DomTree.h"
#include "GraphChecker.h"


namespace ir {
bool GraphChecker::Run() {
    auto *g = graph;
    graph->ForEachBasicBlock([g](const BasicBlock *bblock) {
        VerifyControlAndDataFlowGraphs(bblock);
    });
    DFO::Traverse(graph, []([[maybe_unused]] const BasicBlock *bblock) {
        /* DFO dry run to check graph is properly connected */
    });
    ASSERT(graph->VerifyFirstBlock());
    return true;
}

/* static */
void GraphChecker::VerifyDomTree(Graph *graph) {
    ASSERT(graph);
    if (!graph->IsAnalysisValid(AnalysisFlag::DOM_TREE)) {
        return;
    }
    DomTreeBuilder builder(graph);
    auto domsTreeInfo = builder.Build();
    graph->ForEachBasicBlock([&doms = std::as_const(domsTreeInfo)](const BasicBlock *bblock) {
        auto info = doms[bblock->GetId()];
        ASSERT(info.GetDominator() == bblock->GetDominator());
        ASSERT(sameBasicBlocks(info.GetDominatedBlocks(), bblock->GetDominatedBlocks()));
    });
}

/* static */
void GraphChecker::VerifyControlAndDataFlowGraphs(const BasicBlock *bblock) {
    // TODO: verify Loop Analysis
    ASSERT(bblock != nullptr);
    const InstructionBase *instr = bblock->GetFirstPhiInstruction();
    instr = instr ? instr : bblock->GetFirstInstruction();
    if (!instr) {
        ASSERT(bblock->GetLastPhiInstruction() == nullptr);
        ASSERT(bblock->GetLastInstruction() == nullptr);
        return;
    }

    bool blockNotFirst = !bblock->IsFirstInGraph();
    size_t counter = 0;
    while (instr) {
        if (blockNotFirst) {
            ASSERT(!instr->IsConst() && instr->GetOpcode() != Opcode::ARG);
        }
        verifyBlockStructure(bblock, instr);
        if (instr->IsPhi()) {
            VerifyPhiBasicBlocks(instr->AsPhi());
        }
        verifyDFG(instr);

        ++counter;
        instr = instr->GetNextInstruction();
    }
    ASSERT(bblock->GetSize() == counter);
}

/* static */
void GraphChecker::VerifyPhiBasicBlocks(const PhiInstruction *phi) {
    ASSERT(phi);
    auto *bblock = phi->GetBasicBlock();
    ASSERT(bblock);
    const auto &preds = bblock->GetPredecessors();
    const auto &sources = phi->GetSourceBasicBlocks();
    ASSERT(sources.size() == phi->GetInputsCount());
    ASSERT(sources.size() == preds.size());

    std::pmr::vector<bool> sourcesMask(
        sources.size(), false, bblock->GetGraph()->GetMemoryResource());
    for (const auto *pred : preds) {
        auto it = std::find(sources.begin(), sources.end(), pred);
        ASSERT(it != sources.end());
        sourcesMask[it - sources.begin()] = true;
    }
    ASSERT(std::all_of(sourcesMask.begin(), sourcesMask.end(), [](auto i){ return i == true; }));
}

/* static */
bool GraphChecker::sameBasicBlocks(const std::pmr::vector<BasicBlock *> &lhs,
                                   const std::pmr::vector<BasicBlock *> &rhs) {
    ASSERT(lhs.size() == rhs.size());
    auto sortedLhs = lhs;
    auto sortedRhs = rhs;
    auto comparator = [](const BasicBlock *l, const BasicBlock *r) {
        return l->GetId() < r->GetId();
    };
    std::sort(sortedLhs.begin(), sortedLhs.end(), comparator);
    std::sort(sortedRhs.begin(), sortedRhs.end(), comparator);
    for (size_t i = 0, end = sortedLhs.size(); i < end; ++i) {
        ASSERT(sortedLhs[i] == sortedRhs[i]);
    }
    return true;
}

/* static */
void GraphChecker::verifyBlockStructure(const BasicBlock *bblock, const InstructionBase *instr) {
    ASSERT((bblock) && (instr));

    if (bblock->GetFirstPhiInstruction()) {
        if (instr != bblock->GetFirstPhiInstruction()) {
            ASSERT(instr->GetPrevInstruction() != nullptr);
        }
    } else if (instr != bblock->GetFirstInstruction()) {
        ASSERT(instr->GetPrevInstruction() != nullptr);
    }

    if (instr == bblock->GetLastInstruction()
        || (bblock->GetLastInstruction() == nullptr && instr == bblock->GetLastPhiInstruction()))
    {
        ASSERT(instr->GetNextInstruction() == nullptr);
    } else {
        ASSERT(instr->GetNextInstruction() != nullptr);
    }
}

/* static */
void GraphChecker::verifyDFG(const InstructionBase *instr) {
    ASSERT(instr);

    if (instr->HasInputs()) {
        const auto *graph = instr->GetBasicBlock()->GetGraph();
        const auto *typed = static_cast<const InputsInstruction *>(instr);
        auto found = false;

        for (size_t i = 0, end_idx = typed->GetInputsCount(); i < end_idx; ++i) {
            const auto &input = typed->GetInput(i);
            ASSERT((input.GetInstruction()) && (input->GetBasicBlock()));
            ASSERT(input->GetBasicBlock()->GetGraph() == graph);

            auto currUsers = input->GetUsers();
            auto iter = std::find(currUsers.begin(), currUsers.end(), instr);
            if (iter != currUsers.end()) {
                found = true;
                break;
            }
        }
        ASSERT(found == true);
    }

    auto inputUsers = instr->GetUsers();
    for (auto &&user : inputUsers) {
        ASSERT(user->HasInputs() == true);
        auto *typed = static_cast<const InputsInstruction *>(user);
        bool found = false;
        for (size_t i = 0, end_idx = typed->GetInputsCount(); i < end_idx; ++i) {
            if (typed->GetInput(i) == instr) {
                found = true;
                break;
            }
        }
        ASSERT(found == true);
    }
}
}   // namespace ir
