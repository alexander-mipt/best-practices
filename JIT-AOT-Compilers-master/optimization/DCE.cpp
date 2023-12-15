#include "DCE.h"
#include "Traversals.h"


namespace ir {
bool DCEPass::Run() {
    aliveMarker = graph->GetNewMarker();

    PassManager::Run<RPO>(graph);
    auto rpoTraversal = graph->GetRPO();
    for (auto &bblock : rpoTraversal) {
        for (auto *instr : *bblock) {
            if (instr->HasSideEffects()) {
                markAlive(instr);
            }
        }
    }

    for (auto &bblock : rpoTraversal) {
        for (auto *instr : *bblock) {
            if (!instr->IsMarkerSet(aliveMarker)) {
                markDead(instr);
            }
        }
    }

    bool foundDead = !deadInstrs.empty();
    removeDead();
    return foundDead;
}

void DCEPass::markAlive(InstructionBase *instr) {
    ASSERT(instr);
    GetLogger(utils::LogPriority::DEBUG)
        << "Marking live instruction " << instr->GetId() << ' ' << instr->GetOpcodeName();
    auto wasSet = instr->SetMarker(aliveMarker);
    if (instr->HasInputs() && wasSet) {
        auto *inputInstr = instr->AsInputsInstruction();
        for (size_t i = 0, end = inputInstr->GetInputsCount(); i < end; ++i) {
            markAlive(inputInstr->GetInput(i).GetInstruction());
        }
    }
}

void DCEPass::markDead(InstructionBase *instr) {
    ASSERT(instr);
    GetLogger(utils::LogPriority::INFO)
        << "Removing dead instruction " << instr->GetId() << ' ' << instr->GetOpcodeName();
    deadInstrs.push_back(instr);
    // TODO: handle PHI case?
}

void DCEPass::removeDead() {
    for (auto *instr : deadInstrs) {
        instr->GetBasicBlock()->UnlinkInstruction(instr);
    }
    deadInstrs.clear();
}
}   // namespace ir
