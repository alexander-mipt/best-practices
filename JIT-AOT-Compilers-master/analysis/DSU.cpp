#include "DSU.h"


namespace ir {
BasicBlock *DSU::Find(BasicBlock *bblock) {
    ASSERT((bblock) && (bblock->GetId() < GetSize()));
    if (getUniversum(bblock->GetId()) == nullptr) {    // return if it's the root
        return bblock;
    }
    compressUniversum(bblock);
    // return node with min semi-dominator in ancestors chain
    return getLabel(bblock);
}

void DSU::compressUniversum(BasicBlock *bblock) {
    auto anc = getUniversum(bblock->GetId());
    ASSERT(anc != nullptr);

    if (getUniversum(anc->GetId()) == nullptr) {    // return if root
        return;
    }

    compressUniversum(anc);

    auto minForBBlock = getLabel(bblock);
    auto minForAnc = getLabel(anc);
    if (getSemiDominator(minForAnc) < getSemiDominator(minForBBlock)) {
        setLabel(bblock, minForAnc);      // update min semi-dominator
    }
    setUniversum(bblock->GetId(), getUniversum(anc->GetId()));  // link to the root
}

void DSU::Dump() const {
    std::cout << "===== DSU =====\n";
    for (size_t i = 0; i < GetSize(); ++i) {
        if (getUniversum(i) != nullptr) {
            std::cout << "= BBlock #" << i << ": " << getUniversum(i)->GetId() << std::endl;
        } else {
            std::cout << "= BBlock #" << i << " is root\n";
        }
    }
    std::cout << "===============" << std::endl;
}
}   // namespace ir
