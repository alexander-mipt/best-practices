#ifndef JIT_AOT_COMPILERS_COURSE_DSU_H_
#define JIT_AOT_COMPILERS_COURSE_DSU_H_

#include "AllocatorUtils.h"
#include "BasicBlock.h"
#include "macros.h"
#include <numeric>
#include <stdexcept>
#include <vector>


namespace ir {
class DSU final {
public:
    DSU() = delete;
    DSU(std::pmr::vector<BasicBlock *> &labels,
        const std::pmr::vector<size_t> &sdoms,
        std::pmr::memory_resource *memResource)
        : universum(labels.size(), nullptr, memResource),
          labels(labels),
          sdoms(sdoms)
    {}
    DEFAULT_COPY_SEMANTIC(DSU);
    DEFAULT_MOVE_SEMANTIC(DSU);
    DEFAULT_DTOR(DSU);

    BasicBlock *Find(BasicBlock *bblock);

    size_t GetSize() const {
        return universum.size();
    }

    void Unite(BasicBlock *target, BasicBlock *parent) {
        setUniversum(target->GetId(), parent);
    }

    void Dump() const;

private:
    BasicBlock *getUniversum(size_t id) {
        ASSERT(id < universum.size());
        return universum[id];
    }
    const BasicBlock *getUniversum(size_t id) const {
        ASSERT(id < universum.size());
        return universum[id];
    }
    size_t getSemiDominator(BasicBlock *bblock) const {
        ASSERT((bblock) && bblock->GetId() < sdoms.size());
        return sdoms[bblock->GetId()];
    }
    BasicBlock *getLabel(BasicBlock *bblock) {
        ASSERT((bblock) && bblock->GetId() < labels.size());
        return labels[bblock->GetId()];
    }
    const BasicBlock *getLabel(BasicBlock *bblock) const {
        ASSERT((bblock) && bblock->GetId() < labels.size());
        return labels[bblock->GetId()];
    }
    void setLabel(BasicBlock *bblock, BasicBlock *lbl) {
        ASSERT((lbl) && (bblock) && bblock->GetId() < labels.size());
        labels[bblock->GetId()] = lbl;
    }
    void setUniversum(size_t id, BasicBlock *bblock) {
        ASSERT(id < universum.size());
        universum[id] = bblock;
    }

    void compressUniversum(BasicBlock *bblock);

private:
    std::pmr::vector<BasicBlock *> universum;
    std::pmr::vector<BasicBlock *> &labels;
    const std::pmr::vector<size_t> &sdoms;
};
}   // namespace ir

#endif  // JIT_AOT_COMPILERS_COURSE_DSU_H_
