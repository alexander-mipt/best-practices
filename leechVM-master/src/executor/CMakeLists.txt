# add_library(callbacks callbacks.cc)
# target_include_directories(callbacks PRIVATE ${CMAKE_SOURCE_DIR}/include)

add_library(executor executor.cc callbacks.cc)

# target_link_libraries(executor PUBLIC callbacks)
