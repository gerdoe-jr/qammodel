cmake_minimum_required(VERSION 3.10)

project(qammodel VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED 23)

file(GLOB_RECURSE QAM_SRC src/*)

add_executable(${PROJECT_NAME} ${QAM_SRC})

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_23)
