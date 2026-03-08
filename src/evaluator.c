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
        
        // 注册内置函数（存入 functions 表）
        register_builtins(global_env);
        
        printf("Global environment initialized\n");
        printf("  functions: %d\n", global_env->functions->count);
        printf("  variables: %d\n", global_env->variables->count);
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
    Env* capture_env = func->func.env;  // 定义时的环境
    
    #ifdef DEBUG_EVAL
    printf("Applying function, captured env: %p\n", (void*)capture_env);
    #endif
    
    // 创建新环境（外层是捕获的环境）
    Env* new_env = env_create(capture_env);
    
    // 绑定参数
    ASTNode* p = params->list;  // 参数列表 [n]
    ASTNode* a = args;           // 实际参数 [4]
    
    while (p != NULL && a != NULL) {
        // 对参数求值（在调用者的环境中）
        ASTNode* arg_val = Eval(a, env);
        
        printf("Binding %s = ", p->atom.value);
        print_ast_tree(arg_val, 0, 1);
        
        // 复制值
        ASTNode* val_copy;
        if (arg_val->type == NODE_ATOM) {
            val_copy = create_atom_node(arg_val->atom.value);
            set_atom_type(val_copy, arg_val->atom.atom_type);
        } else {
            val_copy = copy_ast_node(arg_val);
        }
        
        // ⚠️ 关键：绑定到新环境
        env_define_var(new_env, p->atom.value, val_copy);
        
        // 验证
        ASTNode* check = env_lookup_var(new_env, p->atom.value);
        if (check) {
            printf("  ✓ %s bound in new_env %p\n", p->atom.value, (void*)new_env);
        } else {
            printf("  ✗ %s binding FAILED!\n", p->atom.value);
        }
        
        p = p->next;
        a = a->next;
    }
    
    // ⚠️ 关键：在 new_env 中求值函数体
    printf("Evaluating body in new_env %p\n", (void*)new_env);
    ASTNode* result = Eval(body, new_env);
    
    return result;
}

// 主求值函数
ASTNode* Eval(ASTNode* ast, Env* env) {
    if (ast == NULL) return NULL;
    
    #ifdef DEBUG_EVAL
    printf("Env pointer: %p\n", (void*)env);
    printf("Global env pointer: %p\n", (void*)global_env);
    #endif
    if (ast->type == NODE_ATOM) {

        printf("evaluating atom: %s\n", ast->atom.value);
        
        switch(ast->atom.atom_type){
            case ATOM_NUMBER:
            case ATOM_STRING:
                return ast;
            case ATOM_SYMBOL:{
                // 1. 先查找变量（用户定义的）
                ASTNode* var = env_lookup_var(env, ast->atom.value);
                if (var != NULL) {
                    printf("found variable: %s\n", ast->atom.value);
                    return var;
                }
                
                // 2. 再查找函数（内置的）
                LispFunc func = env_lookup_func(env, ast->atom.value);
                if (func != NULL) {
                    printf("found built-in function: %s\n", ast->atom.value);
                    // 返回符号本身，函数调用时再查找
                    return ast;
                }
                
                // 3. 未定义
                printf("warning: undefined symbol '%s'\n", ast->atom.value);
                return ast;
            }
            default:
                printf("warning: undefined symbol '%s'\n", ast->atom.value);
                return create_atom_node("error");
        }
    }
    else if (ast->type == NODE_LIST) {
        printf("evaluating list...\n");
        
        // 空列表返回nil
        if (ast->list == NULL) {
            return create_atom_node("nil");
        }
        
        #if DEBUG_EVAL
        print_ast_tree(ast, 0, 1);
        #endif
        
        // 获取第一个元素作为操作符
        ASTNode* first = Eval(ast->list, env);// 求值第一个元素，支持函数作为第一元素
        ASTNode* args  = ast->list->next;  // 参数列表

        if(first->type == NODE_ATOM && first->atom.atom_type == ATOM_SYMBOL){
            char* op = first->atom.value;
            // 1. 先查找用户定义的函数（在变量表中，类型为 NODE_FUNCTION）
            ASTNode* user_func = env_lookup_var(env, op);
            if (user_func != NULL && user_func->type == NODE_FUNCTION) {
                return apply_function(user_func, args, env);
            }
            
            // 2. 再查找内置函数（在函数表中）
            LispFunc builtin = env_lookup_func(env, op);
            if (builtin != NULL) {
                return builtin(args, env);
            }
        }

        if(first->type == NODE_FUNCTION){
            return apply_function(first, args, env);
        }

        return first;//默认返回第一个元素
    }
    
    return create_atom_node("nil");
}
