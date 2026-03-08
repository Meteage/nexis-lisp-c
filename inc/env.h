// env.h
#ifndef ENV_H
#define ENV_H

#include "astnode.h"
#include "hashtable.h"

#define DEBUG_ENV 0  // 定义调试宏

// 环境结构（支持嵌套作用域）
typedef struct Env {
    HashTable* functions;   // 存内置函数 (LispFunc)
    HashTable* variables;   // 存变量 (ASTNode*)
    struct Env* outer;      // 外层环境
} Env;

// 创建新环境
Env* env_create(Env* outer);

// 定义变量（存入 variables）
void env_define_var(Env* env, const char* name, ASTNode* value);

// 定义函数（存入 functions）
void env_define_func(Env* env, const char* name, LispFunc func);

// 查找变量
ASTNode* env_lookup_var(Env* env, const char* name);

// 查找函数
LispFunc env_lookup_func(Env* env, const char* name);

// 修改变量
void env_set_var(Env* env, const char* name, ASTNode* value);

// 检查变量是否存在
int env_var_exists(Env* env, const char* name);

// 打印环境（调试用）
void env_print(Env* env);

// 释放环境
void env_free(Env* env);

#endif