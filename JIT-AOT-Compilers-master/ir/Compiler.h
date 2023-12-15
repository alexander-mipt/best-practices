#ifndef JIT_AOT_COMPILERS_COURSE_COMPILER_H_
#define JIT_AOT_COMPILERS_COURSE_COMPILER_H_

#include "AllocatorUtils.h"
#include "CompilerBase.h"
#include "CompilerOptions.h"
#include "InstructionBuilder.h"
#include <memory_resource>


namespace ir {
class Compiler : public CompilerBase {
public:
    explicit Compiler(codegen::ArchInfoBase *arch)
        : memResource(), functionsGraphs(&memResource), arch(arch) {}

    codegen::ArchInfoBase *GetArch() const {
        return arch;
    }
    Graph *CreateNewGraph() override {
        auto *instrBuilder = utils::template New<InstructionBuilder>(&memResource, &memResource);
        return CreateNewGraph(instrBuilder);
    }
    Graph *CreateNewGraph(InstructionBuilder *instrBuilder);
    Graph *CopyGraph(const Graph *source, InstructionBuilder *instrBuilder) override;
    Graph *Optimize(Graph *graph) override {
        // TODO: run optimizations here
        return graph;
    }
    Graph *GetFunction(FunctionId functionId) override {
        if (functionId >= functionsGraphs.size()) {
            return nullptr;
        }
        return functionsGraphs[functionId];
    }
    bool DeleteFunctionGraph(FunctionId functionId) override {
        if (functionId >= functionsGraphs.size()) {
            return false;
        }
        functionsGraphs.erase(functionsGraphs.begin() + functionId);
        return true;
    }
    const CompilerOptions &GetOptions() const override {
        return options;
    }

private:
    // TODO: limit maximum memory size
    std::pmr::monotonic_buffer_resource memResource;

    std::pmr::vector<Graph *> functionsGraphs;

    CompilerOptions options;

    codegen::ArchInfoBase *arch;
};
}   // namespace ir

#endif  // JIT_AOT_COMPILERS_COURSE_COMPILER_H_
