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


ASTNode* apply_function(ASTNode* func, ASTNode* args, Env* env) {
    if (func->type != NODE_FUNCTION) {
        return create_atom_node("error");
    }
    
    // 获取函数信息
    ASTNode* params = func->func.params;
    ASTNode* body = func->func.body;
    Env* capture_env = func->func.env;  // 捕获的环境
    
    #ifdef DEBUG
    printf("Applying function, captured env: %p\n", (void*)capture_env);
    #endif
    
    // 创建新环境（外层是捕获的环境）
    Env* new_env = env_create(capture_env);
    
    // 绑定参数
    ASTNode* p = params->list;  // 参数列表
    ASTNode* a = args;           // 实际参数
    
    while (p != NULL && a != NULL) {
        // 对参数求值
        ASTNode* arg_val = Eval(a, env);
        
        // 复制值（避免共享）
        ASTNode* val_copy;
        if (arg_val->type == NODE_ATOM) {
            val_copy = create_atom_node(arg_val->atom.value);
            set_atom_type(val_copy, arg_val->atom.atom_type);
        } else {
            val_copy = arg_val;  // 简化
        }
        
        // 绑定参数
        env_define(new_env, p->atom.value, val_copy);
        
        p = p->next;
        a = a->next;
    }
    
    // 求值函数体
    ASTNode* result = Eval(body, new_env);
    
    return result;
}

// 主求值函数
ASTNode* Eval(ASTNode* ast, Env* env) {
    if (ast == NULL) return NULL;
    printf("Env pointer: %p\n", (void*)env);
    if (ast->type == NODE_ATOM) {
        printf("evaluating atom: %s\n", ast->atom.value);
        
        switch(ast->atom.atom_type){
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
    else if (ast->type == NODE_LIST) {
        printf("evaluating list...\n");
        
        // 空列表返回nil
        if (ast->list == NULL) {
            return create_atom_node("nil");
        }
        print_ast_tree(ast, 0, 1);
        
        // 获取第一个元素作为操作符
        ASTNode* first = ast->list;
        ASTNode* args  = first->next;

        
        
        if(first->type == NODE_LIST){
            first = Eval(first, env);
        }

        if (first->type == NODE_ATOM && first->atom.atom_type == ATOM_SYMBOL) {
            char* op = first->atom.value;
            
            // 1. 内置函数（从全局环境查找）
            LispFunc func = hash_table_get(global_env->bindings, op);
            if (func != NULL) {
                return func(args, env);
            }
            
            // 3. 用户定义的函数（从当前环境查找变量）
            ASTNode* func_node = env_lookup(env, op);
            if (func_node != NULL && func_node->type == NODE_FUNCTION) {
                return apply_function(func_node, args, env);
            }
        }
   
        // 如果第一个元素是函数对象（如 lambda 表达式的结果）
        if (first->type == NODE_FUNCTION) {
            return apply_function(first, args, env);
        }

        // 如果不是函数调用，返回第一个元素的求值结果
        return Eval(first, env);
    }
    
    return create_atom_node("nil");
}
