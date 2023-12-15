#ifndef JIT_AOT_COMPILERS_COURSE_BASIC_BLOCK_H_
#define JIT_AOT_COMPILERS_COURSE_BASIC_BLOCK_H_

#include "AllocatorUtils.h"
#include "instructions/Instruction.h"
#include "macros.h"
#include <vector>


namespace ir {
class Graph;
class GraphTranslationHelper;
class Loop;

class BasicBlock : public Markable {
public:
    using IdType = size_t;

    BasicBlock(Graph *graph);
    NO_COPY_SEMANTIC(BasicBlock);
    NO_MOVE_SEMANTIC(BasicBlock);
    virtual DEFAULT_DTOR(BasicBlock);

    IdType GetId() const {
        return id;
    }
    IdType GetSize() const {
        return instrsCount;
    }
    bool IsEmpty() const {
        return GetSize() == 0;
    }

    bool IsFirstInGraph() const;
    bool IsLastInGraph() const;

    CompareInstruction *EndsWithConditionalJump();
    const CompareInstruction *EndsWithConditionalJump() const;
    void InverseConditionalBranch();

    size_t GetPredecessorsCount() const {
        return preds.size();
    }
    size_t GetSuccessorsCount() const {
        return succs.size();
    }
    bool HasNoPredecessors() const {
        return preds.empty();
    }
    bool HasNoSuccessors() const {
        return succs.empty();
    }
    bool HasPredecessor(const BasicBlock *bblock) const {
        return std::find(preds.begin(), preds.end(), bblock) != preds.end();
    }

    std::pmr::vector<BasicBlock *> &GetPredecessors() {
        return preds;
    }
    const std::pmr::vector<BasicBlock *> &GetPredecessors() const {
        return preds;
    }
    std::pmr::vector<BasicBlock *> &GetSuccessors() {
        return succs;
    }
    const std::pmr::vector<BasicBlock *> &GetSuccessors() const {
        return succs;
    }
    InstructionBase *GetFirstInstruction() {
        return firstInst;
    }
    const InstructionBase *GetFirstInstruction() const {
        return firstInst;
    }
    InstructionBase *GetLastInstruction() {
        return lastInst;
    }
    const InstructionBase *GetLastInstruction() const {
        return lastInst;
    }
    PhiInstruction *GetFirstPhiInstruction() {
        return firstPhi;
    }
    const PhiInstruction *GetFirstPhiInstruction() const {
        return firstPhi;
    }
    PhiInstruction *GetLastPhiInstruction() {
        return lastPhi;
    }
    const PhiInstruction *GetLastPhiInstruction() const {
        return lastPhi;
    }

    BasicBlock *GetDominator() {
        return dominator;
    }
    const BasicBlock *GetDominator() const {
        return dominator;
    }
    std::pmr::vector<BasicBlock *> &GetDominatedBlocks() {
        return dominated;
    }
    const std::pmr::vector<BasicBlock *> &GetDominatedBlocks() const {
        return dominated;
    }
    // Indicated whether this block dominates over the other.
    bool Dominates(const BasicBlock *other) const;

    Loop *GetLoop() {
        return loop;
    }
    const Loop *GetLoop() const {
        return loop;
    }
    bool IsLoopHeader() const;

    Graph *GetGraph() {
        return graph;
    }
    const Graph *GetGraph() const {
        return graph;
    }

    void SetId(IdType newId) {
        id = newId;
    }
    void AddPredecessor(BasicBlock *bblock);
    void AddSuccessor(BasicBlock *bblock);
    void RemovePredecessor(BasicBlock *bblock);
    void RemoveSuccessor(BasicBlock *bblock);
    void ReplaceSuccessor(BasicBlock *prevSucc, BasicBlock *newSucc);
    void ReplacePredecessor(BasicBlock *prevPred, BasicBlock *newPred);

    void SetDominator(BasicBlock *newIDom) {
        dominator = newIDom;
    }
    void AddDominatedBlock(BasicBlock *bblock) {
        ASSERT(bblock);
        dominated.push_back(bblock);
    }
    void RemoveDominatedBlock(BasicBlock *bblock);
    void ClearDominatedBlocks() {
        dominated.clear();
    }

    void SetLoop(Loop *newLoop) {
        loop = newLoop;
    }

    void SetGraph(Graph *g) {
        graph = g;
    }

    // Moves instruction into the end of this block
    // (which must be the first in its graph)
    // while ignoring the initial block, assuming it will be deleted anyway.
    void MoveConstantUnsafe(InstructionBase *instr);

    void PushForwardInstruction(InstructionBase *instr);
    void PushBackInstruction(InstructionBase *instr);
    void InsertBefore(InstructionBase *before, InstructionBase *target);
    void InsertAfter(InstructionBase *after, InstructionBase *target);
    void UnlinkInstruction(InstructionBase *target);
    void ReplaceInstruction(InstructionBase *prevInstr, InstructionBase *newInstr);

    // implemented as BasicBlock's method to be able to directly access internal fields
    BasicBlock *SplitAfterInstruction(InstructionBase *instr, bool connectAfterSplit);

    BasicBlock *Copy(Graph *targetGraph, GraphTranslationHelper &translationHelper) const;

    NO_NEW_DELETE;

public:
    // STL compatible iterator
    template <InstructionPointerType T, bool OnlyPhi = false>
    class Iterator {
    public:
        // iterator traits
        using difference_type = std::ptrdiff_t;
        using value_type = T;
        using pointer = T*;
        using reference = T&;
        using iterator_category = std::bidirectional_iterator_tag;

        Iterator() : curr(nullptr) {}
        explicit Iterator(T instr) : curr(instr) {}
        Iterator &operator++() {
            if constexpr (OnlyPhi) {
                auto *tmp = curr->GetNextInstruction();
                if (tmp == nullptr || !tmp->IsPhi()) {
                    curr = nullptr;
                } else {
                    curr = tmp->AsPhi();
                }
            } else {
                curr = curr->GetNextInstruction();
            }
            return *this;
        }
        Iterator operator++(int) {
            auto retval = *this;
            ++(*this);
            return retval;
        }
        Iterator &operator--() {
            if constexpr (OnlyPhi) {
                curr = curr->GetPrevInstruction()->AsPhi();
            } else {
                curr = curr->GetPrevInstruction();
            }
            return *this;
        }
        Iterator operator--(int) {
            auto retval = *this;
            --(*this);
            return retval;
        }
        bool operator==(const Iterator &other) const {
            return curr == other.curr;
        }
        value_type operator*() {
            return curr;
        }

    private:
        T curr;
    };

    auto begin() {
        InstructionBase *instr = GetFirstPhiInstruction();
        return Iterator{instr != nullptr ? instr : GetFirstInstruction()};
    }
    auto begin() const {
        const InstructionBase *instr = GetFirstPhiInstruction();
        return Iterator{instr != nullptr ? instr : GetFirstInstruction()};
    }
    auto end() {
        return Iterator<decltype(GetLastInstruction())>{nullptr};
    }
    auto end() const {
        return Iterator<decltype(GetLastInstruction())>{nullptr};
    }
    auto size() const {
        return GetSize();
    }

    template <typename T>
    requires std::is_same_v<std::remove_cv_t<T>, BasicBlock>
    class PhiIterationFacade {
    public:
        PhiIterationFacade(T &bblock) : bblock(bblock) {}

        auto begin() {
            return Iterator<decltype(bblock.GetFirstPhiInstruction()), true>{
                bblock.GetFirstPhiInstruction()};
        }
        auto begin() const {
            return Iterator<decltype(bblock.GetFirstPhiInstruction()), true>{
                bblock.GetFirstPhiInstruction()};
        }
        auto end() {
            return Iterator<decltype(bblock.GetLastPhiInstruction()), true>{nullptr};
        }
        auto end() const {
            return Iterator<decltype(bblock.GetLastPhiInstruction()), true>{nullptr};
        }
    private:
        T &bblock;
    };

    auto IteratePhi() & {
        return PhiIterationFacade(*this);
    }
    auto IteratePhi() const & {
        return PhiIterationFacade(*this);
    }

    template <typename T>
    requires std::is_same_v<std::remove_cv_t<T>, BasicBlock>
    class NonPhiIterationFacade {
    public:
        NonPhiIterationFacade(T &bblock) : bblock(bblock) {}

        auto begin() {
            return Iterator{bblock.GetFirstInstruction()};
        }
        auto begin() const {
            return Iterator{bblock.GetFirstInstruction()};
        }
        auto end() {
            return Iterator<decltype(bblock.GetLastInstruction())>{nullptr};
        }
        auto end() const {
            return Iterator<decltype(bblock.GetLastInstruction())>{nullptr};
        }
    private:
        T &bblock;
    };

    auto IterateNonPhi() & {
        return NonPhiIterationFacade(*this);
    }
    auto IterateNonPhi() const & {
        return NonPhiIterationFacade(*this);
    }

public:
    static constexpr IdType INVALID_ID = static_cast<IdType>(-1);

private:
    template <bool PushBack>
    void pushInstruction(InstructionBase *instr);

    void pushPhi(InstructionBase *instr);

    void replaceInControlFlow(InstructionBase *prevInstr, InstructionBase *newInstr);

private:
    IdType id = INVALID_ID;

    std::pmr::vector<BasicBlock *> preds;
    // for conditional branches the first successor must correspond to true branch
    std::pmr::vector<BasicBlock *> succs;

    size_t instrsCount = 0;

    PhiInstruction *firstPhi = nullptr;
    PhiInstruction *lastPhi = nullptr;
    InstructionBase *firstInst = nullptr;
    InstructionBase *lastInst = nullptr;

    BasicBlock *dominator = nullptr;
    std::pmr::vector<BasicBlock *> dominated;

    Loop *loop = nullptr;

    Graph *graph = nullptr;
};

static_assert(std::input_or_output_iterator<BasicBlock::Iterator<InstructionBase*>>);
static_assert(std::ranges::range<BasicBlock>);
}   // namespace ir

#endif  // JIT_AOT_COMPILERS_COURSE_BASIC_BLOCK_H_
