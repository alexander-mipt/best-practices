#ifndef JIT_AOT_COMPILERS_COURSE_LIVE_ANALYSIS_STRUCTS_H_
#define JIT_AOT_COMPILERS_COURSE_LIVE_ANALYSIS_STRUCTS_H_

#include <algorithm>
#include "instructions/InstructionBase.h"
#include <limits>
#include <unordered_set>
#include "ValueLocation.h"


namespace ir {
// Live range of the instruction represented as half-opened interval [begin, end)
class LiveRange final {
public:
    using RangeType = std::size_t;

    LiveRange(RangeType b, RangeType e) : begin(b), end(e) {
        ASSERT(begin <= end);
    }
    LiveRange() = delete;
    DEFAULT_COPY_SEMANTIC(LiveRange);
    DEFAULT_MOVE_SEMANTIC(LiveRange);
    DEFAULT_DTOR(LiveRange);

    void SetBegin(RangeType b) {
        ASSERT(b < end);
        begin = b;
    }
    constexpr RangeType GetBegin() const {
        return begin;
    }
    constexpr RangeType GetEnd() const {
        return end;
    }

    bool Includes(const LiveRange &other) const {
        return GetBegin() <= other.GetBegin() && GetEnd() >= other.GetEnd();
    }
    bool Intersects(const LiveRange &other) const {
        return (GetBegin() >= other.GetBegin() && GetBegin() < other.GetEnd())
            || (other.GetBegin() >= GetBegin() && other.GetBegin() < GetEnd());
    }
    LiveRange Union(const LiveRange &other) const {
        ASSERT(Intersects(other));
        return {std::min(GetBegin(), other.GetBegin()), std::max(end, other.GetEnd())};
    }

    constexpr bool LeftAdjacent(const LiveRange &other) const {
        return GetEnd() == other.GetBegin();
    }
    constexpr bool operator<(const LiveRange &other) const {
        return GetEnd() <= other.GetBegin();
    }
    constexpr bool operator<=(const LiveRange &other) const {
        return GetEnd() <= other.GetEnd();
    }
    constexpr bool operator==(const LiveRange &other) const {
        return GetBegin() == other.GetBegin() && GetEnd() == other.GetEnd();
    }

public:
    static constexpr RangeType INVALID_RANGE = std::numeric_limits<RangeType>::max();

private:
    RangeType begin;
    RangeType end;
};

inline std::ostream &operator<<(std::ostream &os, const LiveRange &rng) {
    os << '<' << rng.GetBegin() << ',' << rng.GetEnd() << '>';
    return os;
}

class LiveInterval {
public:
    using allocator_type = std::pmr::polymorphic_allocator<std::byte>;

    explicit LiveInterval(const allocator_type &a) : ranges(a), liveNumber(0), instr(nullptr) {}
    LiveInterval(LiveRange::RangeType liveNum, InstructionBase *in, std::pmr::memory_resource *memResource)
        : ranges(memResource), liveNumber(liveNum), instr(in) {}
    LiveInterval(LiveRange::RangeType liveNum, InstructionBase *in, const allocator_type &a)
        : ranges(a), liveNumber(liveNum), instr(in) {}

    LiveInterval(std::initializer_list<LiveRange> init, InstructionBase *in);
    LiveInterval(std::initializer_list<LiveRange> init, InstructionBase *in, const allocator_type &a)
        : ranges(init, a), liveNumber(init.begin()->GetBegin()), instr(in) {}
    LiveInterval() = delete;

    NO_COPY_SEMANTIC(LiveInterval);
    LiveInterval(const LiveInterval &other, const allocator_type &a)
        : ranges(other.ranges, a),
          liveNumber(other.liveNumber),
          instr(other.instr)
    {}

    NO_MOVE_SEMANTIC(LiveInterval);
    LiveInterval(LiveInterval &&other, const allocator_type &a)
        : ranges(std::move(other.ranges), a),
          liveNumber(other.liveNumber),
          instr(other.instr)
    {}

    virtual DEFAULT_DTOR(LiveInterval);

    LiveRange::RangeType GetLiveNumber() const {
        return liveNumber;
    }

    InstructionBase *GetInstruction() {
        return instr;
    }
    const InstructionBase *GetInstruction() const {
        return instr;
    }

    const codegen::ValueLocation &GetLocation() const {
        return loc;
    }
    void SetLocation(codegen::ValueLocation newLoc) {
        loc = newLoc;
    }

    LiveRange GetUpperRange() const {
        if (ranges.empty()) {
            return {0, 0};
        }
        return {ranges.back().GetBegin(), ranges.front().GetEnd()};
    }
    LiveRange::RangeType GetBegin() const {
        return GetUpperRange().GetBegin();
    }
    LiveRange::RangeType GetEnd() const {
        return GetUpperRange().GetEnd();
    }

    bool operator==(const LiveInterval &other) const {
        if (ranges.size() != other.ranges.size()) {
            return false;
        }
        for (size_t i = 0, end = ranges.size(); i < end; ++i) {
            if (ranges[i] != other.ranges[i]) {
                return false;
            }
        }
        return true;
    }

    void SetBegin(LiveRange::RangeType begin);

    void AddRange(LiveRange::RangeType begin, LiveRange::RangeType end) {
        AddRange({begin, end});
    }

    void AddRange(const LiveRange &rng);

    auto begin() {
        return ranges.rbegin();
    }
    auto end() {
        return ranges.rend();
    }
    auto begin() const {
        return ranges.rbegin();
    }
    auto end() const {
        return ranges.rend();
    }

public:
    static constexpr size_t LIVE_RANGE_STEP = 2;

private:
    // live ranges sorted in descending order
    std::pmr::vector<LiveRange> ranges;
    LiveRange::RangeType liveNumber;
    InstructionBase *instr;
    codegen::ValueLocation loc;
};

class LiveIntervals {
public:
    LiveIntervals(std::pmr::memory_resource *memResource) : liveIntervals(memResource) {}

    LiveIntervals() = delete;
    NO_COPY_SEMANTIC(LiveIntervals);
    NO_MOVE_SEMANTIC(LiveIntervals);
    virtual DEFAULT_DTOR(LiveIntervals);

    LiveInterval *AddLiveInterval(LiveRange::RangeType liveNum, InstructionBase *instr);

    LiveInterval *GetLiveIntervals(const InstructionBase *instr) {
        ASSERT((instr) && instr->GetLinearNumber() < Size());
        auto *res = liveIntervals[instr->GetLinearNumber()];
        ASSERT(res->GetInstruction() == instr);
        return res;
    }
    const LiveInterval *GetLiveIntervals(const InstructionBase *instr) const {
        ASSERT((instr) && instr->GetLinearNumber() < Size());
        const auto *res = liveIntervals[instr->GetLinearNumber()];
        ASSERT(res->GetInstruction() == instr);
        return res;
    }

    size_t Size() const {
        return liveIntervals.size();
    }

    void Clear() {
        liveIntervals.clear();
    }

public:
    LiveInterval *operator[](int idx) {
        return liveIntervals[idx];
    }
    const LiveInterval *operator[](int idx) const {
        return liveIntervals[idx];
    }

    auto begin() {
        return liveIntervals.rbegin();
    }
    auto end() {
        return liveIntervals.rend();
    }
    auto begin() const {
        return liveIntervals.rbegin();
    }
    auto end() const {
        return liveIntervals.rend();
    }

private:
    std::pmr::vector<LiveInterval *> liveIntervals;
};

inline std::ostream &operator<<(std::ostream &os, const LiveInterval &intervals) {
    ASSERT(intervals.GetInstruction());
    os << '#' << intervals.GetInstruction()->GetId() << '\t';
    for (const auto &iter : intervals) {
        os << iter << ' ';
    }
    return os;
}

class LiveSet {
public:
    using allocator_type = std::pmr::polymorphic_allocator<std::byte>;

    explicit LiveSet(const allocator_type &a) : instructions(a) {}
    explicit LiveSet(std::pmr::memory_resource *memResource) : instructions(memResource) {}

    NO_COPY_SEMANTIC(LiveSet);
    LiveSet(const LiveSet &other, const allocator_type &a) : instructions(other.instructions, a) {}

    NO_MOVE_SEMANTIC(LiveSet);
    LiveSet(LiveSet &&other, const allocator_type &a) : instructions(std::move(other.instructions), a) {}

    virtual DEFAULT_DTOR(LiveSet);

    void Add(InstructionBase *instr) {
        ASSERT(instr);
        instructions.emplace(instr);
    }
    bool Remove(InstructionBase *instr) {
        ASSERT(instr);
        return instructions.erase(instr);
    }

    LiveSet &Union(LiveSet &&other) {
        instructions.merge(std::move(other.instructions));
        return *this;
    }
    LiveSet &Union(const LiveSet &other) {
        instructions.insert(other.instructions.begin(), other.instructions.end());
        return *this;
    }

    auto begin() {
        return instructions.begin();
    }
    auto end() {
        return instructions.end();
    }
    auto begin() const {
        return instructions.begin();
    }
    auto end() const {
        return instructions.end();
    }

    allocator_type get_allocator() const noexcept {
        return instructions.get_allocator();
    }

private:
    std::pmr::unordered_set<InstructionBase *> instructions;
};

class BlockInfo {
public:
    using allocator_type = LiveSet::allocator_type;

    explicit BlockInfo(const allocator_type &a) : liveRange(0, 0), liveIn(a) {}
    BlockInfo(LiveRange range, const allocator_type &a) : liveRange(range), liveIn(a) {}

    DEFAULT_COPY_SEMANTIC(BlockInfo);
    BlockInfo(const BlockInfo &other, const allocator_type &a)
        : liveRange(other.liveRange),
          liveIn(other.liveIn, a)
    {}

    DEFAULT_MOVE_SEMANTIC(BlockInfo);
    BlockInfo(BlockInfo &&other, const allocator_type &a)
        : liveRange(other.liveRange),
          liveIn(std::move(other.liveIn), a)
    {}

    DEFAULT_DTOR(BlockInfo);

    LiveRange GetRange() {
        return liveRange;
    }
    const LiveRange &GetRange() const {
        return liveRange;
    }
    void SetRange(LiveRange range) {
        liveRange = range;
    }

    LiveSet &GetLiveIn() {
        return liveIn;
    }
    const LiveSet &GetLiveIn() const {
        return liveIn;
    }

    allocator_type get_allocator() const noexcept {
        return liveIn.get_allocator();
    }

private:
    LiveRange liveRange;
    LiveSet liveIn;
};
}   // namespace ir

#endif  // JIT_AOT_COMPILERS_COURSE_LIVE_ANALYSIS_STRUCTS_H_
