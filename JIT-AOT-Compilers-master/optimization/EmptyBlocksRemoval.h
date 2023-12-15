#ifndef JIT_AOT_COMPILERS_COURSE_EMPTY_BLOCKS_REMOVAL_H_
#define JIT_AOT_COMPILERS_COURSE_EMPTY_BLOCKS_REMOVAL_H_

#include "logger.h"
#include "PassBase.h"


namespace ir {
class EmptyBlocksRemoval : public PassBase, public utils::Logger {
public:
    explicit EmptyBlocksRemoval(Graph *graph)
        : PassBase(graph), utils::Logger(log4cpp::Category::getInstance(GetName())) {}
    ~EmptyBlocksRemoval() noexcept override = default;

    bool Run() override {
        bool wasRemoved = false;
        auto removeCallback = [&wasRemoved, this](BasicBlock *bblock) {
            auto id = bblock->GetId();
            auto removed = RemoveIfEmpty(bblock);
            wasRemoved |= removed;
            if (removed) {
                this->GetLogger(utils::LogPriority::DEBUG) << "Removed empty basic block #" << id;
            }
        };
        graph->ForEachBasicBlock(removeCallback);
        if (wasRemoved) {
            postRemoval();
        }
        return wasRemoved;
    }

    const char *GetName() const {
        return PASS_NAME;
    }

    static bool RemoveIfEmpty(BasicBlock *bblock);

private:
    void postRemoval();

private:
    static constexpr const char *PASS_NAME = "empty_blocks_removal";
};
}   // namespace ir

#endif  // JIT_AOT_COMPILERS_COURSE_EMPTY_BLOCKS_REMOVAL_H_
