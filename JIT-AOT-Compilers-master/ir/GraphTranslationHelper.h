#ifndef JIT_AOT_COMPILERS_COURSE_GRAPH_TRANSLATION_HELPER_H_
#define JIT_AOT_COMPILERS_COURSE_GRAPH_TRANSLATION_HELPER_H_

#include "BasicBlock.h"
#include <unordered_map>


namespace ir {
struct GraphTranslationHelper final {
    // mapping from Instructions' original IDs into corresponding copied instructions
    std::pmr::unordered_map<InstructionBase::IdType, InstructionBase *> origToCopy;
    std::pmr::unordered_map<InstructionBase::IdType, const InstructionBase *> copyToOrig;
    std::pmr::unordered_map<BasicBlock::IdType, BasicBlock *> blocksToCopy;

    explicit GraphTranslationHelper(std::pmr::memory_resource *memResource)
        : origToCopy(memResource), copyToOrig(memResource), blocksToCopy(memResource) {}

    bool Verify(size_t instructionsCount) const {
        return instructionsCount == origToCopy.size() && instructionsCount == copyToOrig.size();
    }

    void InsertInstructionsPair(const InstructionBase *origInstr, InstructionBase *copyInstr) {
        ASSERT((origInstr) && (copyInstr));
        ASSERT(!origToCopy.contains(origInstr->GetId()));
        ASSERT(!copyToOrig.contains(copyInstr->GetId()));
        origToCopy.insert({origInstr->GetId(), copyInstr});
        copyToOrig.insert({copyInstr->GetId(), origInstr});
    }

    InstructionBase *ToCopy(const InstructionBase *origInstr) const {
        ASSERT(origInstr);
        return translateImpl(origToCopy, origInstr->GetId());
    }
    InstructionBase *ToCopy(const Input &origInstr) const {
        ASSERT(origInstr.GetInstruction());
        return translateImpl(origToCopy, origInstr->GetId());
    }

    const InstructionBase *ToOrig(const InstructionBase *copyInstr) const {
        ASSERT(copyInstr);
        return translateImpl(copyToOrig, copyInstr->GetId());
    }
    const InstructionBase *ToOrig(const Input &copyInstr) const {
        ASSERT(copyInstr.GetInstruction());
        return translateImpl(copyToOrig, copyInstr->GetId());
    }

    BasicBlock *ToCopy(const BasicBlock *origBlock) const {
        ASSERT(origBlock);
        auto copyBlock = blocksToCopy.find(origBlock->GetId());
        ASSERT(copyBlock != blocksToCopy.end());
        return copyBlock->second;
    }

private:
    template <typename ValueT>
    static inline ValueT translateImpl(
        const std::pmr::unordered_map<InstructionBase::IdType, ValueT> &mapping,
        InstructionBase::IdType id)
    {
        auto copyInstr = mapping.find(id);
        ASSERT(copyInstr != mapping.end());
        return copyInstr->second;
    }
};
}   // namespace ir

#endif  // JIT_AOT_COMPILERS_COURSE_GRAPH_TRANSLATION_HELPER_H_
