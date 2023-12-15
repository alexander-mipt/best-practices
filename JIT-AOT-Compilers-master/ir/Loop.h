#ifndef JIT_AOT_COMPILERS_COURSE_LOOP_H_
#define JIT_AOT_COMPILERS_COURSE_LOOP_H_

#include "BasicBlock.h"
#include "macros.h"
#include <vector>


namespace ir {
class Loop final {
public:
    Loop(size_t id,
         BasicBlock *header,
         bool isIrreducible,
         std::pmr::memory_resource *memResource,
         bool isRoot = false)
        : id(id),
          header(header),
          backEdges(memResource),
          basicBlocks(memResource),
          outerLoop(nullptr),
          innerLoops(memResource),
          isIrreducible(isIrreducible),
          isRoot(isRoot)
    {
        ASSERT(memResource);
    }

    auto GetId() const {
        return id;
    }

    BasicBlock *GetHeader() {
        return header;
    }
    const BasicBlock *GetHeader() const {
        return header;
    }

    void AddBackEdge(BasicBlock *backEdgeSource) {
        ASSERT(std::find(backEdges.begin(), backEdges.end(), backEdgeSource) == backEdges.end());
        backEdges.push_back(backEdgeSource);
    }
    std::pmr::vector<BasicBlock *> GetBackEdges() {
        return backEdges;
    }
    const std::pmr::vector<BasicBlock *> &GetBackEdges() const {
        return backEdges;
    }
    size_t GetBackEdgesCount() const {
        return backEdges.size();
    }

    std::pmr::vector<BasicBlock *> GetBasicBlocks() {
        return basicBlocks;
    }
    const std::pmr::vector<BasicBlock *> &GetBasicBlocks() const {
        return basicBlocks;
    }
    void AddBasicBlock(BasicBlock *bblock) {
        ASSERT((bblock)
               && (bblock->GetLoop() == nullptr)
               && (std::find(basicBlocks.begin(), basicBlocks.end(), bblock) == basicBlocks.end()));
        bblock->SetLoop(this);
        basicBlocks.push_back(bblock);
    }

    Loop *GetOuterLoop() {
        return outerLoop;
    }
    const Loop *GetOuterLoop() const {
        return outerLoop;
    }
    void SetOuterLoop(Loop *loop) {
        outerLoop = loop;
    }
    const std::pmr::vector<Loop *> &GetInnerLoops() const {
        return innerLoops;
    }
    void AddInnerLoop(Loop *loop) {
        ASSERT(std::find(innerLoops.begin(), innerLoops.end(), loop) == innerLoops.end());
        innerLoops.push_back(loop);
    }

    void SetIrreducibility(bool isIrr) {
        isIrreducible = isIrr;
    }
    bool IsIrreducible() const {
        return isIrreducible;
    }

    bool IsRoot() const {
        return isRoot;
    }
    bool IsIn(const Loop *other) const {
        ASSERT(other);
        auto *loop = GetOuterLoop();
        while (loop != nullptr) {
            if (loop == other) {
                return true;
            }
            loop = loop->GetOuterLoop();
        }
        return false;
    }

    NO_NEW_DELETE;

private:
    size_t id;

    BasicBlock *header;
    // TODO: may replace with set
    std::pmr::vector<BasicBlock *> backEdges;

    std::pmr::vector<BasicBlock *> basicBlocks;

    Loop *outerLoop;
    std::pmr::vector<Loop *> innerLoops;

    bool isIrreducible;

    // TODO: can replace with `outerLoop == nullptr`
    bool isRoot;
};
}   // namespace ir

#endif  // JIT_AOT_COMPILERS_COURSE_LOOP_H_
