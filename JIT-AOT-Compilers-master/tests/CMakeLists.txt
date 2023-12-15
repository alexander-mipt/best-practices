set(BINARY tests)

set(SOURCES
    BasicBlockTest.cpp
    BranchEliminationTest.cpp
    CheckEliminationTest.cpp
    CompilerTestBase.h
    DCETest.cpp
    DomTreeTest.cpp
    EmptyBlocksRemovalTest.cpp
    GraphTest.cpp
    InliningTest.cpp
    InstructionsTest.cpp
    LinearOrderingTest.cpp
    LinearScanRegAllocTest.cpp
    LivenessAnalysisTest.cpp
    LoopAnalysisTest.cpp
    main.cpp
    PeepholesTest.cpp
    TestGraphSamples.h
    TestGraphSamples.cpp
    TraversalsTest.cpp
    )

add_executable(${BINARY} ${SOURCES})

add_test(NAME ${BINARY} COMMAND ${BINARY})

target_link_libraries(${BINARY} PUBLIC analysis ir gtest optimization codegen utils)
