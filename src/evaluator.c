// evaluator.c
#include "evaluator.h"
#include "builtins.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "parser.h"
#include "lexer.h"

// 全局环境
Env* global_env = NULL;

void init_global_env() {
    if (global_env == NULL) {
        // 创建全局环境（无外层环境）
        global_env = env_create(NULL);
        
        // 注册内置函数（存入 global_env->bindings）
        register_builtins(global_env->bindings);
        
        printf("Global environment initialized with %d bindings\n", 
               global_env->bindings->count);
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
ASTNode* Eval(ASTNode* ast, Env* env) {
    if (ast == NULL) return NULL;
    
    if (ast->type == ATOM) {
        printf("evaluating atom: %s\n", ast->atom.value);
        
        switch(ast->atom.type){
            case ATOM_NUMBER:
            case ATOM_STRING:
                return ast;
            case ATOM_SYMBOL:
                //如果是符号就搜索变量库
                ASTNode* val = env_lookup(env, ast->atom.value);
                if (val != NULL) {
                    // printf("found variable: %s\n", ast->atom.value);
                    //printf("value: %s\n", val->atom.value);
                    return val;
                }
            default:
                printf("warning: undefined symbol '%s'\n", ast->atom.value);
            return ast;
        }
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
        if (first->type == ATOM && first->atom.type == ATOM_SYMBOL) {
            // 从环境中查找函数
            LispFunc func = hash_table_get(global_env->bindings, first->atom.value);
            if (func != NULL) {
                // 调用注册的函数
                return func(args,global_env);
            }
        }
        
        // 如果不是函数调用，返回第一个元素的求值结果
        return Eval(first, env);
    }
    
    return create_atom_node("nil");
}
