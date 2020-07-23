cmake_minimum_required(VERSION 3.5)
project(tensorRT-tolov4)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

set(CMAKE_BUILD_TYPE Relaese)
set(CMAKE_CXX_STANDARD  11)
set(GPU_ARCHS 62)
add_subdirectory(onnx-tensorrt)
add_subdirectory(source)
add_subdirectory(example)
