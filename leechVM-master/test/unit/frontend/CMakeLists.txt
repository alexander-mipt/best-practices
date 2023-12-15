set(TESTS_DIR ${PROJECT_SOURCE_DIR}/test/frontend/)
configure_file(config.in config.hh @ONLY)

add_executable(frontend_test frontend.cc)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

upd_tar_list(frontend_test TESTLIST)
