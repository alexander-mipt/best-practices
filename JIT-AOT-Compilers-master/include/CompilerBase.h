#ifndef JIT_AOT_COMPILERS_COURSE_COMPILER_BASE_H_
#define JIT_AOT_COMPILERS_COURSE_COMPILER_BASE_H_

#include "ArchInfoBase.h"
#include "CompilerOptions.h"


namespace ir {
class Graph;
class InstructionBuilder;

using FunctionId = size_t;

class CompilerBase {
public:
    CompilerBase() = default;
    NO_COPY_SEMANTIC(CompilerBase);
    NO_MOVE_SEMANTIC(CompilerBase);
    virtual DEFAULT_DTOR(CompilerBase);

    virtual codegen::ArchInfoBase *GetArch() const = 0;
    virtual Graph *CreateNewGraph() = 0;
    virtual Graph *CopyGraph(const Graph *source, InstructionBuilder *instrBuilder) = 0;
    virtual Graph *Optimize(Graph *graph) = 0;
    virtual Graph *GetFunction(FunctionId functionId) = 0;
    virtual bool DeleteFunctionGraph(FunctionId functionId) = 0;
    virtual const CompilerOptions &GetOptions() const = 0;
};
}   // namespace ir

#endif  // JIT_AOT_COMPILERS_COURSE_COMPILER_BASE_H_
