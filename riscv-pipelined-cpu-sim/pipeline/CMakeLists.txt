set(SOURCES pipeline.cpp)

add_library(pipeline STATIC ${SOURCES})

target_sources(pipeline PUBLIC
        control_unit.h
        hazard_unit.h
        instruction.h
        pipeline.h
        pipeline.cpp
        )

target_include_directories(pipeline PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
