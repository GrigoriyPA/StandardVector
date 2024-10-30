cmake_minimum_required(VERSION 3.10)

set(PROJECT_NAME cpp_proy)
set(CMAKE_CXX_STANDARD 23)

project(${PROJECT_NAME})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/build)

add_executable(${PROJECT_NAME} vector/vector_ut.cpp)
