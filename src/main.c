// main.c
#include <stdio.h>
#include <string.h>
#include "token.h"
#include "lexer.h"
#include "parser.h"
#include "evaluator.h"
#include "repl.h"
#include "test.h"

void print_usage() {
    printf("Usage: nexis-lisp [options]\n");
    printf("Options:\n");
    printf("  -h, --help     Show this help\n");
    printf("  -t, --test     Run tests\n");
    printf("  -f, --file     Load and evaluate file\n");
    printf("  (no options)   Start REPL\n");
}

int main(int argc, char* argv[]) {
    // 初始化全局环境
    init_global_env();
    
    int result = 0;
    
    if (argc > 1) {
        if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
            print_usage();
        }
        else if (strcmp(argv[1], "-t") == 0 || strcmp(argv[1], "--test") == 0) {
            // 运行测试
            printf("Running tests...\n");
            test_token_stream();
            test_tokenize();
            test_parser();
            test_eval();
        }
        else if (strcmp(argv[1], "-f") == 0 || strcmp(argv[1], "--file") == 0) {
            if (argc > 2) {
                result = repl_run_file(argv[2]);
            } else {
                printf("Error: No file specified\n");
                result = 1;
            }
        }
        else {
            // 默认当作文件名
            result = repl_run_file(argv[1]);
        }
    } else {
        // 无参数，启动 REPL
        repl_start();
    }
    
    // 释放环境
    if (global_env != NULL) {
        free_hash_table(global_env->functions);
        global_env = NULL;
    }
    
    return result;
}