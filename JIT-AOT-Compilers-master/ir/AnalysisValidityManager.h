#ifndef JIT_AOT_COMPILERS_COURSE_ANALYSIS_VALIDITY_MANAGER_H_
#define JIT_AOT_COMPILERS_COURSE_ANALYSIS_VALIDITY_MANAGER_H_

#include <bitset>
#include "helpers.h"
#include "macros.h"


namespace ir {
enum class AnalysisFlag : uint8_t {
    DOM_TREE = 0,
    LOOP_ANALYSIS,
    RPO,
    LINEAR_ORDERING,
    INVALID,
    ANALYSIS_COUNT = INVALID,
};

class AnalysisValidityManager {
public:
    AnalysisValidityManager() = default;
    DEFAULT_COPY_SEMANTIC(AnalysisValidityManager);
    DEFAULT_MOVE_SEMANTIC(AnalysisValidityManager);
    virtual DEFAULT_DTOR(AnalysisValidityManager);

    bool IsAnalysisValid(AnalysisFlag a) const {
        return mask[utils::to_underlying(a)];
    }
    template <AnalysisFlag AFlag>
    void SetAnalysisValid(bool isValid) {
        mask[utils::to_underlying(AFlag)] = isValid;
    }

private:
    std::bitset<utils::to_underlying(AnalysisFlag::ANALYSIS_COUNT)> mask{false};
};
}   // namespace ir

#endif  // JIT_AOT_COMPILERS_COURSE_ANALYSIS_VALIDITY_MANAGER_H_
