// evaluator.c
#include "evaluator.h"
#include "builtins.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "parser.h"
#include "lexer.h"

// 全局环境
HashTable* global_env = NULL;

/**
 * 判断一个字符串是否是数字
 * @param str 输入的字符串
 * @return 如果是数字返回1，否则返回0
 */
int is_number(const char * str){
    if(str == NULL || *str == '\0') return 0; //空字符串不是数字
    char *endstr;
    strtol(str, &endstr, 10); //尝试将字符串转换为长整数
    return *endstr == '\0'; //如果转换成功，endstr应该指向字符串的末尾
}

// 初始化全局环境
void init_global_env() {
    if (global_env == NULL) {
        global_env = create_hash_table(101);  // 使用质数大小的哈希表
        register_builtins(global_env);  // 调用builtins.c中的注册函数
    }
}

// 计算列表中的参数个数
int count_args(ASTNode* args) {
    int count = 0;
    ASTNode* current = args;
    while (current != NULL) {
        count++;
        current = current->next;
    }
    return count;
}

// 主求值函数
ASTNode* Eval(ASTNode* ast, HashTable* env) {
    if (ast == NULL) return NULL;
    
    if (ast->type == ATOM) {
        printf("evaluating atom: %s\n", ast->atom);
        
        // 自求值表达式
        if (strcmp(ast->atom, "nil") == 0 ||
            strcmp(ast->atom, "true") == 0 ||
            strcmp(ast->atom, "false") == 0 ||
            is_number(ast->atom)) {
            return ast;
        }
        
        // 变量查找（可以后续扩展）
        // 目前直接返回本身
        return ast;
    }
    else if (ast->type == LIST) {
        printf("evaluating list...\n");
        
        // 空列表返回nil
        if (ast->list == NULL) {
            return create_atom_node("nil");
        }
        
        // 获取第一个元素作为操作符
        ASTNode* first = ast->list;
        ASTNode* args = first->next;
        
        // 如果是原子类型，尝试查找函数
        if (first->type == ATOM) {
            // 从环境中查找函数
            LispFunc func = hash_table_get(env, first->atom);
            if (func != NULL) {
                // 调用注册的函数
                return func(args);
            }
        }
        
        // 如果不是函数调用，返回第一个元素的求值结果
        return Eval(first, env);
    }
    
    return create_atom_node("nil");
}

// 调试函数
void test_eval() {
    printf("========== 开始测试 ==========\n");
    
    // 初始化环境
    init_global_env();
    
    // 测试1: 算术运算
    printf("\n--- 测试1: 算术运算 ---\n");
    Token *token_stream1 = tokenize("[+ 1 2 3]");
    printf("输入: [+ 1 2 3]\n");
    ASTNode* ast1 = parse(token_stream1);
    printf("AST结构:\n");
    print_ast_tree(ast1, 0, 1);
    ASTNode* result1 = Eval(ast1, global_env);
    printf("计算结果: ");
    print_ast_tree(result1, 0, 1);
    
    // 测试2: quote
    printf("\n--- 测试2: quote ---\n");
    Token *token_stream2 = tokenize("[' [1 2 3]]");
    printf("输入: [' [1 2 3]]\n");
    ASTNode* ast2 = parse(token_stream2);
    printf("AST结构:\n");
    print_ast_tree(ast2, 0, 1);
    ASTNode* result2 = Eval(ast2, global_env);
    printf("计算结果: ");
    print_ast_tree(result2, 0, 1);
    
    // 测试3: 复杂表达式
    printf("\n--- 测试3: 复杂表达式 ---\n");
    Token *token_stream3 = tokenize("[+ 2 [* 2 3] [/ 8 2]]");
    printf("输入: [+ 2 [* 2 3] [/ 8 2]]\n");
    ASTNode* ast3 = parse(token_stream3);
    printf("AST结构:\n");
    print_ast_tree(ast3, 0, 1);
    ASTNode* result3 = Eval(ast3, global_env);
    printf("计算结果: ");
    print_ast_tree(result3, 0, 1);
    
    // 测试4: 比较操作
    printf("\n--- 测试4: 比较操作 ---\n");
    Token *token_stream4 = tokenize("[eq? [+ 1 2] 3]");
    printf("输入: [eq? [+ 1 2] 3]\n");
    ASTNode* ast4 = parse(token_stream4);
    printf("AST结构:\n");
    print_ast_tree(ast4, 0, 1);
    ASTNode* result4 = Eval(ast4, global_env);
    printf("计算结果: ");
    print_ast_tree(result4, 0, 1);

    printf("\n-------test5:------\n");
    Token *token_stream5 = tokenize("'[eq? [+ 1 2] 3]");
    ASTNode* ast5 = parse(token_stream5);
    print_ast_tree(ast5, 0, 1);
    ASTNode* result5 = Eval(ast5, global_env);
    printf("result: ");
    print_ast_tree(result5, 0, 1);
    
    printf("\n========== 测试完成 ==========\n");
}

/*
int main() {
    test_eval();
    
    // 释放环境
    if (global_env != NULL) {
        free_hash_table(global_env);
        global_env = NULL;
    }
    
    return 0;
}
*/