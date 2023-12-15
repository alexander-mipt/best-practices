#ifndef JIT_AOT_COMPILERS_COURSE_LINEAR_SCAN_REG_ALLOC_H_
#define JIT_AOT_COMPILERS_COURSE_LINEAR_SCAN_REG_ALLOC_H_

#include <list>
#include "LiveAnalysisStructs.h"
#include "logger.h"
#include "PassBase.h"
#include "RegMap.h"


namespace ir::codegen {
class LinearScanRegAlloc : public PassBase, public utils::Logger {
public:
    explicit LinearScanRegAlloc(Graph *graph)
        : PassBase(graph),
          utils::Logger(log4cpp::Category::getInstance(GetName())),
          regsCount(graph->GetCompiler()->GetArch()->GetIntRegsCount()),
          active(graph->GetMemoryResource()),
          regMap(regsCount)
    {}
    ~LinearScanRegAlloc() noexcept override = default;

    bool Run() override;

    const char *GetName() const {
        return PASS_NAME;
    }

public:
    static constexpr const char *PASS_NAME = "linear_regalloc";

private:
    void allocateAndAssignRegs();
    void expireOldIntervals(const LiveInterval *intervals);
    void spillAtInterval(LiveInterval *intervals);

    bool isFull() const {
        return regMap.IsFull();
    }

    // Adds active instruction while saving end points' increasing order.
    void addActiveInterval(LiveInterval *intervals);

    LocationIdType getNextSlot() {
        ASSERT(slotCounter < MAX_REG_NUMBER);
        return slotCounter++;
    }

    ValueLocation spillToStack(LiveInterval *intervals);
    void assignToReg(LiveInterval *intervals);

    bool fixPhis();
    bool resolvePhis(BasicBlock *bblock);

    BasicBlock *breakCriticalEdge(BasicBlock *bblock, BasicBlock *pred);

    void postAlloc();

    static bool isCriticalEdge(BasicBlock *bblock, BasicBlock *pred) {
        ASSERT((bblock) && (pred));
        return bblock->GetPredecessorsCount() > 1 && pred->GetSuccessorsCount() > 1;
    }

private:
    LocationIdType slotCounter = 0;

    LocationIdType regsCount;

    std::pmr::list<LiveInterval *> active;

    RegMap regMap;
};
}   // namespace ir::codegen

#endif  // JIT_AOT_COMPILERS_COURSE_LINEAR_SCAN_REG_ALLOC_H_
