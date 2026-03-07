#ifndef BUILTINS_H
#define BUILTINS_H

#include "hashtable.h"
#include "astnode.h"

// 辅助函数
ASTNode* make_number(int n);
ASTNode* make_symbol(const char* s);

// 算术运算
ASTNode* op_add(ASTNode* args);
ASTNode* op_sub(ASTNode* args);
ASTNode* op_mul(ASTNode* args);
ASTNode* op_div(ASTNode* args);

// 特殊形式
ASTNode* op_quote(ASTNode* args);

// 列表操作
ASTNode* op_car(ASTNode* args);
ASTNode* op_cdr(ASTNode* args);
ASTNode* op_cons(ASTNode* args);
ASTNode* op_null(ASTNode* args);
ASTNode* op_length(ASTNode* args);
ASTNode* op_list(ASTNode* args);

// 比较操作
ASTNode* op_eq(ASTNode* args);
ASTNode* op_lt(ASTNode* args);
ASTNode* op_gt(ASTNode* args);

// 字符串操作
ASTNode* op_str_len(ASTNode* args);
ASTNode* op_str_cat(ASTNode* args);

ASTNode* op_if(ASTNode* args);

// 注册函数
void register_builtins(HashTable* env);

#endif