// env.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "env.h"

// 创建新环境
Env* env_create(Env* outer) {
    Env* env = (Env*)malloc(sizeof(Env));
    if (!env) return NULL;
    
    env->functions = create_hash_table(101);  // 存函数
    env->variables = create_hash_table(101);  // 存变量
    env->outer = outer;
    
    return env;
}

// 定义变量（存入 variables）
void env_define_var(Env* env, const char* name, ASTNode* value) {
    if (!env || !name) return;

    #if DEBUG_ENV
    printf("env_define_var: env=%p, name=%s, value type=%d\n", 
           (void*)env, name, value->type);
    #endif

    // 创建副本存储
    ASTNode* value_copy = copy_ast_node(value);

    #if DEBUG_ENV
    printf("env_define_var: copy created at %p\n", (void*)value_copy);
    #endif
    
    // 存入变量哈希表
    hash_table_put(env->variables, name, (LispFunc)value_copy);

    #if DEBUG_ENV
    printf("env_define_var: after hash_table_put\n");
    #endif
    
    #if DEBUG_ENV
    // 验证是否存入成功
    LispFunc check = hash_table_get(env->variables, name);
    if (check != NULL) {
        printf("env_define_var: VERIFICATION SUCCESS - found '%s'\n", name);
    } else {
        printf("env_define_var: VERIFICATION FAILED - '%s' not found!\n", name);
    }
    #endif
}

// 定义函数（存入 functions）
void env_define_func(Env* env, const char* name, LispFunc func) {
    if (!env || !name) return;
    
    // 直接存入函数哈希表
    hash_table_put(env->functions, name, func);
    
    #if DEBUG_ENV
    printf("Defined function: %s\n", name);
    #endif
}

// 查找变量
ASTNode* env_lookup_var(Env* env, const char* name) {
    if (!env || !name) return NULL;
    
    #if DEBUG_ENV
    printf("env_lookup_var: searching for '%s' in env %p\n", name, (void*)env);
    #endif
    
    // 在当前环境查找变量
    LispFunc val = hash_table_get(env->variables, name);
    if (val != NULL) {
        #if DEBUG_ENV
        printf("env_lookup_var: FOUND '%s' in variables\n", name);
        #endif
        // 返回副本
        return copy_ast_node((ASTNode*)val);
    }
    
    // 在外层环境查找
    if (env->outer != NULL) {
        #if DEBUG_ENV
        printf("env_lookup_var: checking outer env\n");
        #endif
        return env_lookup_var(env->outer, name);
    }
    
    #if DEBUG_ENV
    printf("env_lookup_var: '%s' not found\n", name);
    #endif
    return NULL;
}

// 查找函数
LispFunc env_lookup_func(Env* env, const char* name) {
    if (!env || !name) return NULL;
    
    #if DEBUG_ENV
    printf("env_lookup_func: searching for '%s' in env %p\n", name, (void*)env);
    #endif
    
    // 在当前环境查找函数
    LispFunc func = hash_table_get(env->functions, name);
    if (func != NULL) {
        #ifdef DEBUG
        printf("env_lookup_func: FOUND '%s' in functions\n", name);
        #endif
        return func;
    }
    
    // 在外层环境查找
    if (env->outer != NULL) {
        #ifdef DEBUG_ENV
        printf("env_lookup_func: checking outer env\n");
        #endif
        return env_lookup_func(env->outer, name);
    }
    
    #ifdef DEBUG_ENV
    printf("env_lookup_func: '%s' not found\n", name);
    #endif
    return NULL;
}

// 修改变量
void env_set_var(Env* env, const char* name, ASTNode* value) {
    if (!env || !name) return;
    
    // 先检查当前环境是否有这个变量
    if (hash_table_get(env->variables, name) != NULL) {
        // 创建副本
        ASTNode* value_copy = copy_ast_node(value);
        // 在当前环境修改
        hash_table_put(env->variables, name, (LispFunc)value_copy);
        return;
    }
    
    // 如果当前环境没有，且存在外层环境，在外层修改
    if (env->outer != NULL) {
        env_set_var(env->outer, name, value);
        return;
    }
    
    // 变量不存在，可以选择报错或自动定义
    #if DEBUG_ENV
    printf("Warning: variable '%s' not found, defining in current environment\n", name);
    #endif
    env_define_var(env, name, value);
}

// 检查变量是否存在
int env_var_exists(Env* env, const char* name) {
    return (env_lookup_var(env, name) != NULL);
}

// 打印环境（调试用）
void env_print(Env* env) {
    if (!env) return;
    
    printf("Environment [%p]:\n", (void*)env);
    printf("  functions count: %d\n", env->functions->count);
    printf("  variables count: %d\n", env->variables->count);
    
    if (env->outer) {
        printf("  outer: %p\n", (void*)env->outer);
    }
}

// 释放环境
void env_free(Env* env) {
    if (!env) return;
    
    if (env->functions) {
        free_hash_table(env->functions);
    }
    if (env->variables) {
        free_hash_table(env->variables);
    }
    free(env);
}