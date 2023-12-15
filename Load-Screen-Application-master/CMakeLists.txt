message(STATUS "CLANG_TIDY = ${CLANG_TIDY}")
cmake_minimum_required(VERSION 3.16)
project(LLVM_Course C)

set(CMAKE_C_STANDARD 11)

if(${CLANG_TIDY})
    set(CMAKE_C_CLANG_TIDY "clang-tidy;-header-filter=.;-checks=*")
endif()

include_directories(
        ${PROJECT_SOURCE_DIR})

execute_process(COMMAND python3 "${CMAKE_CURRENT_SOURCE_DIR}/get_dimensions.py" WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                OUTPUT_VARIABLE DEFINES)
message(STATUS "${DEFINES}")
separate_arguments(DEFINES)
list(GET DEFINES 0 WIDTH)
list(GET DEFINES 1 HEIGHT)


add_executable(load_screen
        main.c
        screen.h
        screen.c
        spiral.h
        spiral.c
        point.h
        controller.c
        controller.h
        defines.h)

# target_compile_definitions(load_screen PRIVATE ${FIRST} ${SECOND})
target_compile_definitions(load_screen PRIVATE SCREEN_WIDTH=${WIDTH} SCREEN_HEIGHT=${HEIGHT})

target_link_libraries(
        load_screen
        m)
