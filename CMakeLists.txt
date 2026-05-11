cmake_minimum_required(VERSION 3.6)

project(qammodel VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED 20)

file(GLOB_RECURSE QAM_SRC src/*)

add_executable(${PROJECT_NAME} ${QAM_SRC})

target_include_directories(${PROJECT_NAME} PUBLIC src/main)
target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_20)

# enable_testing()
# add_subdirectory(tests)