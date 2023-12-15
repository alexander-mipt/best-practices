set(SOURCES
    DomTree.cpp
    DSU.cpp
    GraphChecker.cpp
    LinearOrdering.cpp
    LivenessAnalyzer.cpp
    LoopAnalyzer.cpp
    Traversals.cpp
    )

add_library(analysis STATIC ${SOURCES})

target_sources(analysis PUBLIC
    DomTree.h
    DSU.h
    GraphChecker.h
    LinearOrdering.h
    LivenessAnalyzer.h
    LoopAnalyzer.h
    Traversals.h
    )

target_include_directories(analysis PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/ir
    ${CMAKE_SOURCE_DIR}/utils
    )

add_dependencies(analysis ir)
