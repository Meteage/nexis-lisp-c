// repl.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "repl.h"
#include "evaluator.h"
#include "parser.h"
#include "lexer.h"
#include "builtins.h"

// 历史记录
#define MAX_HISTORY 100
#define MAX_LINE_LENGTH 1024

static char* history[MAX_HISTORY];
static int history_count = 0;

// 颜色定义（可选）
#define COLOR_RESET   "\033[0m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_CYAN    "\033[36m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_RED     "\033[31m"

static int use_color = 0;

// 检查是否支持颜色
static int support_color() {
#ifdef _WIN32
    return 0;  // Windows 简单起见，先不支持颜色
#else
    return 1;
#endif
}

// 添加到历史记录
static void add_to_history(const char* line) {
    if (strlen(line) == 0 || strcmp(line, "\n") == 0) return;
    
    if (history_count >= MAX_HISTORY) {
        free(history[0]);
        for (int i = 1; i < MAX_HISTORY; i++) {
            history[i-1] = history[i];
        }
        history_count--;
    }
    
    history[history_count] = strdup(line);
    history_count++;
}

// 打印提示符
static void print_prompt(int continued) {
    if (use_color) {
        if (continued) {
            printf(COLOR_CYAN "... " COLOR_RESET);
        } else {
            printf(COLOR_GREEN "lisp> " COLOR_RESET);
        }
    } else {
        if (continued) {
            printf("... ");
        } else {
            printf("lisp> ");
        }
    }
    fflush(stdout);
}

// 打印欢迎信息
static void print_welcome() {
    printf("\n");
    printf("========================================\n");
    printf("   Nexis Lisp v0.1.0\n");
    printf("========================================\n");
    printf("Commands:\n");
    printf("  (exit) or (quit)  - Exit REPL\n");
    printf("  (help)            - Show this help\n");
    printf("\n");
    printf("Supported:\n");
    printf("  Arithmetic : + - * /\n");
    printf("  List ops   : car cdr cons null? length list\n");
    printf("  String ops : str-len str-cat\n");
    printf("  Comparisons: eq? < >\n");
    printf("  Special    : quote ' if\n");
    printf("========================================\n\n");
}

// 检查括号是否匹配
static int check_brackets(const char* input) {
    int balance = 0;
    for (const char* p = input; *p; p++) {
        if (*p == '[') balance++;
        if (*p == ']') balance--;
    }
    return balance;
}

// 读取多行输入
static char* read_multiline() {
    static char buffer[4096];
    buffer[0] = '\0';
    int continued = 0;
    
    while (1) {
        print_prompt(continued);
        
        char line[MAX_LINE_LENGTH];
        if (fgets(line, sizeof(line), stdin) == NULL) {
            return NULL;
        }
        
        // 检查退出命令
        if (strcmp(line, "(exit)\n") == 0 || strcmp(line, "(quit)\n") == 0 ||
            strcmp(line, "exit\n") == 0 || strcmp(line, "quit\n") == 0) {
            return strdup("(exit)");
        }
        
        // 检查帮助命令
        if (strcmp(line, "(help)\n") == 0 || strcmp(line, "help\n") == 0) {
            print_welcome();
            continued = 0;
            buffer[0] = '\0';
            continue;
        }
        
        // 添加到历史
        add_to_history(line);
        
        // 拼接到缓冲区
        strcat(buffer, line);
        
        // 检查括号是否匹配
        if (check_brackets(buffer) <= 0) {
            break;
        }
        continued = 1;
    }
    
    return strdup(buffer);
}

// 打印结果
static void print_result(ASTNode* result) {
    if (result == NULL) {
        printf("nil\n");
        return;
    }
    
    if (use_color) {
        printf(COLOR_YELLOW);
        print_ast_tree(result, 0, 1);
        printf(COLOR_RESET);
    } else {
        print_ast_tree(result, 0, 1);
    }
}

// 打印错误
static void print_error(const char* msg) {
    if (use_color) {
        printf(COLOR_RED "Error: %s\n" COLOR_RESET, msg);
    } else {
        printf("Error: %s\n", msg);
    }
}

// 执行单行表达式
void repl_eval_line(const char* input) {
    if (input == NULL || strlen(input) == 0) return;
    
    // Tokenize
    Token* tokens = tokenize((char*)input);
    if (tokens == NULL) {
        print_error("Failed to tokenize");
        return;
    }
    
    // Parse
    ASTNode* ast = parse(tokens);
    if (ast == NULL) {
        print_error("Failed to parse");
        free_token_stream(tokens);
        return;
    }
    
    // Eval
    ASTNode* result = Eval(ast, global_env);
    
    // Print
    print_result(result);
    printf("\n");
    
    // Cleanup
    free_ast(ast);
    free_token_stream(tokens);
}

// REPL 主循环
void repl_start() {
    use_color = support_color();
    print_welcome();
    
    while (1) {
        char* input = read_multiline();
        if (input == NULL) break;
        
        if (strcmp(input, "(exit)") == 0 || strcmp(input, "exit") == 0) {
            free(input);
            printf("Goodbye!\n");
            break;
        }
        
        if (strlen(input) > 0 && strcmp(input, "\n") != 0) {
            repl_eval_line(input);
        }
        
        free(input);
    }
}

// 运行文件
int repl_run_file(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Error: Cannot open file %s\n", filename);
        return 1;
    }
    
    char buffer[4096];
    int line_num = 0;
    
    while (fgets(buffer, sizeof(buffer), file)) {
        line_num++;
        // 跳过空行和注释
        if (buffer[0] == '\n' || buffer[0] == ';' || buffer[0] == '#') {
            continue;
        }
        
        printf("[%d] %s", line_num, buffer);
        repl_eval_line(buffer);
    }
    
    fclose(file);
    return 0;
}