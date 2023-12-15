#include "Graph.h"
#include "LiveAnalysisStructs.h"


namespace ir {
LiveInterval::LiveInterval(std::initializer_list<LiveRange> init, InstructionBase *in)
    : ranges(init, in->GetBasicBlock()->GetGraph()->GetMemoryResource()),
      liveNumber(init.begin()->GetBegin()),
      instr(in)
{}

void LiveInterval::SetBegin(LiveRange::RangeType begin) {
    if (ranges.empty()) {
        ranges.emplace_back(begin, begin + LIVE_RANGE_STEP);
    } else {
        ASSERT(ranges.back().GetEnd() >= begin);
        ranges.back().SetBegin(begin);
    }
}

void LiveInterval::AddRange(const LiveRange &rng) {
    if (ranges.empty()) {
        ranges.push_back(rng);
    } else {
        auto &earliest = ranges.back();
        if (rng.LeftAdjacent(earliest)) {
            earliest.SetBegin(rng.GetBegin());
        } else if (rng < earliest) {
            ranges.push_back(rng);
        } else {
            ASSERT(earliest.Includes(rng));
        }
    }
}

LiveInterval *LiveIntervals::AddLiveInterval(LiveRange::RangeType liveNum, InstructionBase *instr) {
    ASSERT(instr);
    instr->SetLinearNumber(liveIntervals.size());
    auto *info = utils::template New<LiveInterval>(
        instr->GetBasicBlock()->GetGraph()->GetMemoryResource(), liveNum, instr);
    liveIntervals.push_back(info);
    return info;
}
}   // namespace ir
