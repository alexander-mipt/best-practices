set(SOURCES
    default/DefaultArch.cpp
    LinearScanRegAlloc.cpp
    RegAllocChecker.cpp
    )

add_library(codegen STATIC ${SOURCES})

target_sources(codegen PUBLIC
    default/DefaultArch.h
    LinearScanRegAlloc.h
    RegAllocChecker.h
    RegMap.h
    )

target_include_directories(codegen PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/analysis
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/ir
    ${CMAKE_SOURCE_DIR}/utils
    )

add_dependencies(codegen analysis)
