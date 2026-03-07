// evaluator.h
#ifndef __EVALUATOR_H__
#define __EVALUATOR_H__

#include "astnode.h"
#include "hashtable.h"

// 全局环境（外部可访问）
extern HashTable* global_env;

// 判断字符串是否为数字
int is_number(const char* str);

// 初始化全局环境
void init_global_env();

// 主求值函数
ASTNode* Eval(ASTNode* ast, HashTable* env);

// 辅助函数
int count_args(ASTNode* args);

// 测试函数
void test_eval();

#endif