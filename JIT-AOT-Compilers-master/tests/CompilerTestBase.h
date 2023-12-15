#ifndef JIT_AOT_COMPILERS_COURSE_COMPILER_TEST_BASE_H_
#define JIT_AOT_COMPILERS_COURSE_COMPILER_TEST_BASE_H_

#include "default/DefaultArch.h"
#include "Compiler.h"
#include "GraphChecker.h"
#include "gtest/gtest.h"
#include "InstructionBuilder.h"
#include "PassBase.h"


namespace ir::tests {
class CompilerTestBase : public ::testing::Test {
public:
    CompilerTestBase() : compiler(codegen::DefaultArch::GetInstance()) {}

    void SetUp() override {
        graph = compiler.CreateNewGraph();
    }

    void TearDown() override {
        ASSERT_TRUE(compiler.DeleteFunctionGraph(graph->GetId()));
        graph = nullptr;
    }

    Graph *GetGraph() {
        return graph;
    }
    InstructionBuilder *GetInstructionBuilder(Graph *targetGraph = nullptr) {
        if (targetGraph == nullptr) {
            targetGraph = GetGraph();
        }
        return targetGraph->GetInstructionBuilder();
    }

    template <typename InstructionsT>
    static inline BasicBlock *FillFirstBlock(Graph *g, InstructionsT &&instrs)
    requires std::is_same_v<typename InstructionsT::value_type, InstructionBase *>
    {
        ASSERT(g != nullptr);
        auto *firstBlock = g->CreateEmptyBasicBlock();
        g->GetInstructionBuilder()->PushBackInstruction(firstBlock, std::move(instrs));
        g->SetFirstBasicBlock(firstBlock);
        return firstBlock;
    }
    template <typename... ArgsT>
    static inline BasicBlock *FillFirstBlock(Graph *g, ArgsT... instrs)
    requires ((std::is_same_v<ArgsT, InputArgumentInstruction *>
               || std::is_same_v<ArgsT, ConstantInstruction *>) && ...)
    {
        ASSERT(g != nullptr);
        auto *firstBlock = g->CreateEmptyBasicBlock();
        g->GetInstructionBuilder()->PushBackInstruction(firstBlock, instrs...);
        g->SetFirstBasicBlock(firstBlock);
        ASSERT(firstBlock->GetSize() == sizeof...(ArgsT));
        return firstBlock;
    }

    template <typename AllocatorT = std::allocator<InstructionBase *>>
    static void compareInstructions(std::vector<InstructionBase *, AllocatorT> expected,
                                    BasicBlock *bblock)
    {
        ASSERT_EQ(bblock->GetSize(), expected.size());
        size_t i = 0;
        for (auto *instr : *bblock) {
            ASSERT_EQ(instr, expected[i++]);
        }
    }

    static void VerifyControlAndDataFlowGraphs(Graph *graph) {
        PassManager::Run<GraphChecker>(graph);
    }
    static void VerifyControlAndDataFlowGraphs(const BasicBlock *bblock) {
        GraphChecker::VerifyControlAndDataFlowGraphs(bblock);
    }

public:
    static constexpr TypeId::TypeIdType MAGIC_TYPE_ID = 42;

    Compiler compiler;

protected:
    Graph *graph = nullptr;
};
}   // namespace ir::tests

#endif  // JIT_AOT_COMPILERS_COURSE_COMPILER_TEST_BASE_H_
