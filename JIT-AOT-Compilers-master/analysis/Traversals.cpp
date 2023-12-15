#include "Graph.h"
#include "logger.h"
#include "Traversals.h"


namespace ir {
void dumpBasicBlock(const BasicBlock *bblock) {
    ASSERT(bblock);
    std::cout << "=== BB #" << bblock->GetId() << " (" << bblock->GetSize();
    std::cout << " instructions)\npreds: < ";
    for (const auto &pred : bblock->GetPredecessors()) {
        std::cout << pred->GetId() << " ";
    }
    std::cout << ">\nsuccs: < ";
    for (const auto &succ : bblock->GetSuccessors()) {
        std::cout << succ->GetId() << " ";
    }
    std::cout << ">\n";

    auto &logger = utils::Logger::GetRoot();
    for (auto *instr : *bblock) {
        instr->Dump(logger);
    }
    std::cout << '\n';
}

void DumpGraphRPO(const Graph *graph) {
    // run RPO regardless of RPO contained in graph due to possible code errors
    auto rpoBBlocks = RPO::DoRPO(graph);
    std::cout << "======================================\n";
    for (auto bblock : rpoBBlocks) {
        dumpBasicBlock(bblock);
    }
    std::cout << "======================================" << std::endl;
}

void DumpGraphDFO(const Graph *graph) {
    std::cout << "======================================\n";
    DFO::Traverse(graph, dumpBasicBlock);
    std::cout << "======================================" << std::endl;
}
}   // namespace ir
