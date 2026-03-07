// evaluator.h
#ifndef __EVALUATOR_H__
#define __EVALUATOR_H__

#include "astnode.h"
#include "hashtable.h"

#include "env.h"  // 新增

// 全局环境（现在用 Env 结构）
extern Env* global_env;

// 初始化全局环境
void init_global_env();

// 主求值函数
ASTNode* Eval(ASTNode* ast, Env* env);

// 辅助函数
int count_args(ASTNode* args);


#endif