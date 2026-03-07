// env.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "env.h"


// 创建新环境
Env* env_create(Env* outer) {
    Env* env = (Env*)malloc(sizeof(Env));
    if (!env) return NULL;
    
    env->bindings = create_hash_table(101);  // 哈希表大小 101
    env->outer = outer;
    
    return env;
}

// 在环境中定义变量
void env_define(Env* env, const char* name, ASTNode* value) {
    if (!env || !name) return;
    
    // 创建副本存储
    ASTNode* value_copy = copy_ast_node(value);
    
    // 存入哈希表
    hash_table_put(env->bindings, name, (LispFunc)value_copy);
    
    #ifdef DEBUG
    printf("Defined: %s = ", name);
    print_ast_tree(value_copy, 0, 1);
    #endif
}

// 修改 env_lookup，返回副本
ASTNode* env_lookup(Env* env, const char* name) {
    if (!env || !name) return NULL;
    
    LispFunc val = hash_table_get(env->bindings, name);
    if (val != NULL) {
        // 返回副本，避免原节点被修改
        return copy_ast_node((ASTNode*)val);
    }
    
    if (env->outer != NULL) {
        return env_lookup(env->outer, name);
    }
    
    return NULL;
}

// 修改变量
void env_set(Env* env, const char* name, ASTNode* value) {
    if (!env || !name) return;
    
    // 先检查当前环境是否有这个变量
    if (hash_table_get(env->bindings, name) != NULL) {
        // 在当前环境修改
        hash_table_put(env->bindings, name, (LispFunc)value);
        return;
    }
    
    // 如果当前环境没有，且存在外层环境，在外层修改
    if (env->outer != NULL) {
        env_set(env->outer, name, value);
        return;
    }
    
    // 变量不存在，可以选择报错或自动定义
    #ifdef DEBUG
    printf("Warning: variable '%s' not found, defining in current environment\n", name);
    #endif
    env_define(env, name, value);
}

// 检查变量是否存在
int env_exists(Env* env, const char* name) {
    return (env_lookup(env, name) != NULL);
}

// 打印环境（调试用）
void env_print(Env* env) {
    if (!env) return;
    
    printf("Environment [%p]:\n", (void*)env);
    
    // 打印哈希表内容
    // 需要 hashtable.h 提供遍历功能
    // 这里简化处理，只打印数量
    printf("  bindings count: %d\n", env->bindings->count);
    
    if (env->outer) {
        printf("  outer: %p\n", (void*)env->outer);
    }
}

// 释放环境
void env_free(Env* env) {
    if (!env) return;
    
    // 注意：只释放当前环境，不释放外层环境
    // 外层环境由调用者决定何时释放
    if (env->bindings) {
        free_hash_table(env->bindings);
    }
    free(env);
}