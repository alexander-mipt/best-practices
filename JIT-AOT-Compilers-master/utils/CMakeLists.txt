set(SOURCES
    debug.cpp
    logger.cpp)

add_library(utils STATIC ${SOURCES})

target_sources(utils PUBLIC
    AllocatorUtils.h
    debug.h
    helpers.h
    logger.h
    macros.h
    )

include_directories(${log4cplus_INCLUDE_DIR})

target_include_directories(utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(utils PUBLIC ${log4cplus_LIB})
