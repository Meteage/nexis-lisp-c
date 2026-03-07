// builtins.h
#ifndef __BUILTINS_H__
#define __BUILTINS_H__

#include "hashtable.h"
#include "astnode.h"

// 判断字符串是否为数字（已在evaluator.h中声明，这里也可以声明）
int is_number(const char* str);

// 注册所有内置函数到指定的环境
void register_builtins(HashTable* env);

// 各个内置函数的声明
ASTNode* op_add(ASTNode* args);
ASTNode* op_sub(ASTNode* args);
ASTNode* op_mul(ASTNode* args);
ASTNode* op_div(ASTNode* args);
ASTNode* op_quote(ASTNode* args);
ASTNode* op_car(ASTNode* args);
ASTNode* op_cdr(ASTNode* args);
ASTNode* op_cons(ASTNode* args);
ASTNode* op_eq(ASTNode* args);
ASTNode* op_lt(ASTNode* args);
ASTNode* op_gt(ASTNode* args);

#endif