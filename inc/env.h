// env.h
#ifndef ENV_H
#define ENV_H

#include "astnode.h"
#include "hashtable.h"

#define DEBUG 1

// 环境结构（支持嵌套作用域）
typedef struct Env {
    HashTable* bindings;    // 变量绑定表
    struct Env* outer;       // 外层环境（用于闭包）
} Env;

// 创建新环境
Env* env_create(Env* outer);

// 在环境中定义变量
void env_define(Env* env, const char* name, ASTNode* value);

// 查找变量
ASTNode* env_lookup(Env* env, const char* name);

// 修改变量
void env_set(Env* env, const char* name, ASTNode* value);

// 检查变量是否存在
int env_exists(Env* env, const char* name);

// 打印环境（调试用）
void env_print(Env* env);

// 释放环境
void env_free(Env* env);

#endif