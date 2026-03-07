cmake_minimum_required(VERSION 3.10)
project(NexisLisp VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-g -Wall -Wextra)
else()
    add_compile_options(-O2)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc)

# 源文件列表
set(SOURCES
    src/test.c           # 新增
    src/main.c
    src/repl.c           # REPL 模块
    src/env.c
    src/evaluator.c
    src/builtins.c
    src/hashtable.c
    src/parser.c
    src/lexer.c
    src/token.c
    src/astnode.c
)

# 头文件列表
set(
    inc/test.h           # 新增
    inc/repl.h           # 新增
    inc/env.h
    inc/evaluator.h
    inc/builtins.h
    inc/hashtable.h
    inc/parser.h
    inc/lexer.h
    inc/astnode.h
    inc/token.h
)

add_executable(nexis-lisp ${SOURCES} ${HEADERS})

set_target_properties(nexis-lisp PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

message(STATUS "Project: ${PROJECT_NAME}")
message(STATUS "Version: ${PROJECT_VERSION}")
message(STATUS "Source files: ${SOURCES}")