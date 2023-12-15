#include "codegen/default/DefaultArch.h"
#include "Compiler.h"
#include "Traversals.h"


int main() {
    ir::Compiler compiler(ir::codegen::DefaultArch::GetInstance());
    auto *graph = compiler.CreateNewGraph();
    graph = compiler.Optimize(graph);
    ir::DumpGraphRPO(graph);
    return 0;
}
