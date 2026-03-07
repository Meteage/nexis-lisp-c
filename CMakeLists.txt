cmake_minimum_required(VERSION 3.10)

project(LispInterpreter VERSION 1.0.0 LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-g -Wall -Wextra)
else()
    add_compile_options(-O2)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# 自动查找所有源文件和头文件
file(GLOB SOURCES "src/*.c")
file(GLOB HEADERS "inc/*.h")

# 添加头文件目录
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc)

add_executable(lisp_interpreter ${SOURCES} ${HEADERS})

set_target_properties(lisp_interpreter PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

message(STATUS "Project: ${PROJECT_NAME}")
message(STATUS "Found ${CMAKE_MATCH_COUNT} source files in src/")
message(STATUS "Found ${CMAKE_MATCH_COUNT} header files in inc/")