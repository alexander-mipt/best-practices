cmake_minimum_required(VERSION 3.17)
project(RISC_V_Simulator)

set(CMAKE_CXX_STANDARD 17)

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

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

string(TOLOWER ${CMAKE_BUILD_TYPE} LOWER_BUILD_TYPE)
if(${LOWER_BUILD_TYPE} MATCHES "debug")
    add_definitions(-DDEBUG)
endif()


set(CMAKE_CXX_FLAGS "-Wall -Wextra -Werror")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

add_executable(cpu main.cpp)

target_include_directories(cpu PUBLIC module)

add_subdirectory(pipeline)

if(${LOWER_BUILD_TYPE} MATCHES "debug")
    add_git_submodule(third_party/googletest)

    add_subdirectory(tests)
endif()

target_link_libraries(cpu PUBLIC
        pipeline
        )
