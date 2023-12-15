#ifndef JIT_AOT_COMPILERS_COURSE_PASS_BASE_H_
#define JIT_AOT_COMPILERS_COURSE_PASS_BASE_H_

#include "Graph.h"
#include <log4cpp/Category.hh>


namespace ir {
class PassBase;

class PassManager final {
public:
    PassManager() = delete;
    NO_COPY_SEMANTIC(PassManager);
    NO_MOVE_SEMANTIC(PassManager);
    DEFAULT_DTOR(PassManager);

    template <typename PassT, typename... ArgsT>
    static bool Run(Graph *graph, ArgsT... args)
    requires std::is_base_of_v<PassBase, PassT>
    {
        if constexpr (utils::has_set_flag_v<PassT>) {
            static_assert(std::is_same_v<std::remove_cv_t<decltype(PassT::SET_FLAG)>, AnalysisFlag>);
            if (graph->IsAnalysisValid(PassT::SET_FLAG)) {
                return true;
            }
            auto res = PassT(graph, args...).Run();
            graph->SetAnalysisValid<PassT::SET_FLAG>(true);
            return res;
        }
        return PassT(graph, args...).Run();
    }

    template <AnalysisFlag... Flags>
    static void SetInvalid(Graph *graph) {
        utils::expand_t{(graph->SetAnalysisValid<Flags>(false), void(), 0)...};
    }
};

class PassBase {
public:
    PassBase(Graph *graph) : graph(graph) {
        ASSERT(graph);
    }
    NO_COPY_SEMANTIC(PassBase);
    NO_MOVE_SEMANTIC(PassBase);
    virtual DEFAULT_DTOR(PassBase);

    virtual bool Run() = 0;

protected:
    Graph *graph;
};
}   // namespace ir

#endif  // JIT_AOT_COMPILERS_COURSE_PASS_BASE_H_
