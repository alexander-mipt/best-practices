cmake_minimum_required(VERSION 3.16)

if("${CMAKE_BINARY_DIR}" STREQUAL "${CMAKE_SOURCE_DIR}")
  message(
    FATAL_ERROR
      "In-source builds are disabled.
  Please create a subfolder and use `cmake ..` inside it.
  NOTE: cmake will now create CMakeCache.txt and CMakeFiles/*.
  You must delete them, or cmake will refuse to work.")
endif()

project(leech)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
  string(APPEND CMAKE_CXX_FLAGS_RELEASE " -O2")
endif()

macro(check_compiler_version COMPILER VER)
  if(CMAKE_CXX_COMPILER_ID STREQUAL ${COMPILER})
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS ${VER})
      message(
        FATAL_ERROR
          "Insufficient ${COMPILER} version. Needed: ${VER}. Your: ${CMAKE_CXX_COMPILER_VERSION}"
      )
    endif()
  endif()
endmacro()

check_compiler_version("GNU" "9.3.0")
check_compiler_version("Clang" "10.0.0")

include(cmake/list_dirs.cmake)
include(cmake/upd_tar_list.cmake)
include(cmake/comp_flags.cmake)
include(cmake/clang_format.cmake)

# indicate the docs build
option(BUILD_DOC "Build docs" OFF)
# indicate the tests build
option(BUILD_TESTS "Build tests" OFF)
# add -Werror option
option(ENABLE_WERROR "Enable -Werror option (CI)" OFF)

# Test running stuff
if(BUILD_TESTS)
  enable_testing()
endif()

find_package(BISON 3.5 REQUIRED)
find_package(FLEX REQUIRED)

add_subdirectory(docs)
add_subdirectory(src)
add_subdirectory(tools)

add_subdirectory(test)
add_subdirectory(thirdparty)

message("Collected libs: ${LIBLIST}")
message("Collected tools: ${TOOLLIST}")

set(TARGETS)
list(APPEND TARGETS ${LIBLIST} ${TOOLLIST})

if(DEBUG_PRINT)
  message("Debug print enabled!")
endif()

foreach(TARGET ${TARGETS})
  target_include_directories(${TARGET} PRIVATE ${CMAKE_SOURCE_DIR}/include)
  target_compile_features(${TARGET} PRIVATE cxx_std_20)
  apply_compiler_flags(${TARGET} PUBLIC)
  if(ENABLE_WERROR)
    target_compile_options(${TARGET} PRIVATE -Werror)
  endif()
  if(DEBUG_PRINT)
    target_compile_definitions(${TARGET} PRIVATE DEBUG_PRINT=1)
  endif()
endforeach()

foreach(TOOL ${TOOLLIST})
  target_link_libraries(${TOOL} PRIVATE ${LIBLIST})
endforeach()
