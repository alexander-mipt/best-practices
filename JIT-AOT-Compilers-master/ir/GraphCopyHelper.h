#ifndef JIT_AOT_COMPILERS_COURSE_GRAPH_COPY_HELPER_H_
#define JIT_AOT_COMPILERS_COURSE_GRAPH_COPY_HELPER_H_

#include "Graph.h"
#include "GraphTranslationHelper.h"
#include "InstructionBuilder.h"


namespace ir {
class GraphCopyHelper final {
public:
    static Graph *CreateCopy(const Graph *source, Graph *copyTarget);

private:
    GraphCopyHelper(const Graph *source, Graph *copyTarget)
        : source(source),
          target(copyTarget),
          translationHelper(copyTarget->GetMemoryResource()),
          visited(copyTarget->GetMemoryResource())
    {}
    NO_COPY_SEMANTIC(GraphCopyHelper);
    NO_MOVE_SEMANTIC(GraphCopyHelper);
    DEFAULT_DTOR(GraphCopyHelper);

    void dfoCopy(const BasicBlock *currentBBlock);
    void fixDFG();

private:
    const Graph *source;
    Graph *target;

    GraphTranslationHelper translationHelper;
    std::pmr::unordered_map<BasicBlock::IdType, BasicBlock *> visited;
};
}   // namespace ir

#endif  // JIT_AOT_COMPILERS_COURSE_GRAPH_COPY_HELPER_H_
