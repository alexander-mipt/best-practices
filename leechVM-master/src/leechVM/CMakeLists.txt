add_library(leechVM leechVM.cc)
target_include_directories(leechVM
    SYSTEM PRIVATE
    ${CMAKE_BINARY_DIR}/src/frontend
)
target_link_libraries(leechVM PRIVATE executor frontend)
