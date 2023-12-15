cmake_minimum_required(VERSION 3.22)
project(JIT_AOT)

set(CMAKE_CXX_STANDARD 20)

function(add_git_submodule dir)
    # add a Git submodule directory to CMake, assuming the
    # Git submodule directory is a CMake project.
    #
    # Usage: in CMakeLists.txt
    #
    # include(AddGitSubmodule.cmake)
    # add_git_submodule(mysubmod_dir)

    find_package(Git REQUIRED)

    if(NOT EXISTS ${dir}/CMakeLists.txt)
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive -- ${dir}
                WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
    endif()

    add_subdirectory(${dir})

endfunction(add_git_submodule)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/")

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()
if(NOT ${CMAKE_BUILD_TYPE} MATCHES "Debug")
    add_definitions(-DNDEBUG)
endif()

add_link_options("-fuse-ld=lld")

set(CMAKE_CXX_FLAGS "-Wall -Wextra -Werror -fconcepts-diagnostics-depth=3")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

find_package(log4cplus REQUIRED)
include_directories(${log4cplus_INCLUDE_DIR})

add_executable(fact main.cpp)
add_subdirectory(utils)
add_subdirectory(ir)
add_subdirectory(analysis)
add_subdirectory(optimization)
add_subdirectory(codegen)

if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
    add_git_submodule(third_party/googletest)
    add_subdirectory(tests)
endif()

target_link_libraries(fact PUBLIC
    ${log4cplus_LIB}
    analysis
    codegen
    ir
    optimization
    utils
    )
