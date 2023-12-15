set(SOURCES
    BranchElimination.cpp
    CheckElimination.cpp
    ConstantFolding.cpp
    DCE.cpp
    EmptyBlocksRemoval.cpp
    Inlining.cpp
    Peephole.cpp
    )

add_library(optimization STATIC ${SOURCES})

target_sources(optimization PUBLIC
    BranchElimination.h
    CheckElimination.h
    ConstantFolding.h
    DCE.h
    EmptyBlocksRemoval.h
    Inlining.h
    Peephole.h
    )

target_include_directories(optimization PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/analysis
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/ir
    ${CMAKE_SOURCE_DIR}/utils
    )

add_dependencies(optimization analysis)
