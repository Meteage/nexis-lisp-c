// builtins.h
#ifndef BUILTINS_H
#define BUILTINS_H

#include "hashtable.h"
#include "astnode.h"
#include "env.h"

// 所有内置函数现在都接受 (ASTNode*, Env*)
ASTNode* op_add(ASTNode* args, Env* env);
ASTNode* op_sub(ASTNode* args, Env* env);
ASTNode* op_mul(ASTNode* args, Env* env);
ASTNode* op_div(ASTNode* args, Env* env);
ASTNode* op_quote(ASTNode* args, Env* env);
ASTNode* op_car(ASTNode* args, Env* env);
ASTNode* op_cdr(ASTNode* args, Env* env);
ASTNode* op_cons(ASTNode* args, Env* env);
ASTNode* op_null(ASTNode* args, Env* env);
ASTNode* op_length(ASTNode* args, Env* env);
ASTNode* op_list(ASTNode* args, Env* env);
ASTNode* op_eq(ASTNode* args, Env* env);
ASTNode* op_lt(ASTNode* args, Env* env);
ASTNode* op_gt(ASTNode* args, Env* env);
ASTNode* op_str_len(ASTNode* args, Env* env);
ASTNode* op_str_cat(ASTNode* args, Env* env);
ASTNode* op_if(ASTNode* args, Env* env);
ASTNode* op_def(ASTNode* args, Env* env);
ASTNode* op_set(ASTNode* args, Env* env);

// 注册函数
void register_builtins(HashTable* env);

#endif