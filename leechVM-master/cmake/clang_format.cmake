if(NOT SKIP_CLANG_FORMAT)
  find_program(CLANG_FORMAT clang-format HINTS ${CLANG_FORMAT_PATH})

  if (NOT CLANG_FORMAT)
     message(STATUS "Probably need to specify CLANG_FORMAT_PATH")
     message(FATAL_ERROR "Clang-format not found")
  endif()
endif()

function(format_target TARGET PREFIX SOURCE_LIST)
  if(SKIP_CLANG_FORMAT)
    return()
  endif()

  set(CFNAME "clangformat_${TARGET}")
  set(SRCS "")
  foreach(SOURCE_FILE ${SOURCE_LIST})
    list(APPEND SRCS "${PREFIX}/${SOURCE_FILE}")
  endforeach()

  set(INCDIR ${CMAKE_SOURCE_DIR}/include/${TARGET})
  file(GLOB HEADERSRCS ${INCDIR}/*)
  foreach(SOURCE_FILE ${HEADERSRCS})
    list(APPEND SRCS "${SOURCE_FILE}")
  endforeach()

  add_custom_target(
    ${CFNAME}
    COMMAND ${CLANG_FORMAT}
    -i
    ${SRCS}
  )
  add_dependencies(${TARGET} ${CFNAME})
endfunction()