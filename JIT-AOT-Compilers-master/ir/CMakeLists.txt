set(SOURCES
    BasicBlock.cpp
    Compiler.cpp
    Graph.cpp
    GraphCopyHelper.cpp
    Instruction.cpp
    InstructionBase.cpp
    InstructionsCopy.cpp
    LiveAnalysisStructs.cpp
    )

add_library(ir STATIC ${SOURCES})

target_sources(ir PUBLIC
    AnalysisValidityManager.h
    BasicBlock.h
    Compiler.h
    Concepts.h
    Graph.h
    GraphCopyHelper.h
    GraphTranslationHelper.h
    InstructionBuilder.h
    LiveAnalysisStructs.h
    Loop.h
    PassBase.h
    marker/marker.h
    instructions/Input.h
    instructions/Instruction.h
    instructions/InstructionBase.h
    instructions/Types.h
    instructions/Users.h
    )

target_include_directories(ir PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/utils
    )

add_dependencies(ir utils)
