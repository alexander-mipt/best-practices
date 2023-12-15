#ifndef JIT_AOT_COMPILERS_COURSE_DCE_H_
#define JIT_AOT_COMPILERS_COURSE_DCE_H_

#include "Graph.h"
#include "logger.h"
#include "PassBase.h"


namespace ir {
class DCEPass : public PassBase, public utils::Logger {
public:
    explicit DCEPass(Graph *graph)
        : PassBase(graph),
          utils::Logger(log4cpp::Category::getInstance(GetName())),
          deadInstrs(graph->GetMemoryResource())
    {}
    ~DCEPass() noexcept override = default;

    bool Run() override;

    const char *GetName() const {
        return PASS_NAME;
    }

private:
    void markAlive(InstructionBase *instr);
    void markDead(InstructionBase *instr);
    void removeDead();

private:
    static constexpr const char *PASS_NAME = "dce";

private:
    Marker aliveMarker;

    std::pmr::vector<InstructionBase *> deadInstrs;
};
}   // namespace ir

#endif  // JIT_AOT_COMPILERS_COURSE_DCE_H_
