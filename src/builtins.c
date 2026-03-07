// builtins.c
#include "builtins.h"
#include "evaluator.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 加法操作
ASTNode* op_add(ASTNode* args) {
    if (args == NULL) return create_atom_node("0");
    
    int sum = 0;
    ASTNode* current = args;
    
    while (current != NULL) {
        ASTNode* evaluated = Eval(current, global_env);
        if (evaluated != NULL && evaluated->type == ATOM) {
            sum += atoi(evaluated->atom);
        }
        current = current->next;
    }
    
    char buffer[20];
    sprintf(buffer, "%d", sum);
    return create_atom_node(buffer);
}

// 减法操作
ASTNode* op_sub(ASTNode* args) {
    if (args == NULL) return create_atom_node("0");
    
    ASTNode* first_eval = Eval(args, global_env);
    if (first_eval == NULL || first_eval->type != ATOM) {
        return create_atom_node("0");
    }
    
    int result = atoi(first_eval->atom);
    ASTNode* current = args->next;
    
    while (current != NULL) {
        ASTNode* evaluated = Eval(current, global_env);
        if (evaluated != NULL && evaluated->type == ATOM) {
            result -= atoi(evaluated->atom);
        }
        current = current->next;
    }
    
    char buffer[20];
    sprintf(buffer, "%d", result);
    return create_atom_node(buffer);
}

// 乘法操作
ASTNode* op_mul(ASTNode* args) {
    if (args == NULL) return create_atom_node("1");
    
    int result = 1;
    ASTNode* current = args;
    
    while (current != NULL) {
        ASTNode* evaluated = Eval(current, global_env);
        if (evaluated != NULL && evaluated->type == ATOM) {
            result *= atoi(evaluated->atom);
        }
        current = current->next;
    }
    
    char buffer[20];
    sprintf(buffer, "%d", result);
    return create_atom_node(buffer);
}

// 除法操作
ASTNode* op_div(ASTNode* args) {
    if (args == NULL) return create_atom_node("0");
    
    ASTNode* first_eval = Eval(args, global_env);
    if (first_eval == NULL || first_eval->type != ATOM) {
        return create_atom_node("0");
    }
    
    int result = atoi(first_eval->atom);
    ASTNode* current = args->next;
    
    while (current != NULL) {
        ASTNode* evaluated = Eval(current, global_env);
        if (evaluated != NULL && evaluated->type == ATOM) {
            int divisor = atoi(evaluated->atom);
            if (divisor != 0) {
                result /= divisor;
            }
        }
        current = current->next;
    }
    
    char buffer[20];
    sprintf(buffer, "%d", result);
    return create_atom_node(buffer);
}

// quote操作 - 特殊形式，不对参数求值
ASTNode* op_quote(ASTNode* args) {
    if (args == NULL) return create_atom_node("nil");
    return args;  // 返回第一个参数（不求值）
}

// car操作 - 返回列表的第一个元素
ASTNode* op_car(ASTNode* args) {
    if (args == NULL || args->type != LIST) {
        return create_atom_node("nil");
    }
    
    ASTNode* list = Eval(args, global_env);
    if (list == NULL || list->type != LIST || list->list == NULL) {
        return create_atom_node("nil");
    }
    
    return list->list;  // 返回第一个元素
}

// cdr操作 - 返回列表的剩余部分
ASTNode* op_cdr(ASTNode* args) {
    if (args == NULL || args->type != LIST) {
        return create_atom_node("nil");
    }
    
    ASTNode* list = Eval(args, global_env);
    if (list == NULL || list->type != LIST) {
        return create_atom_node("nil");
    }
    
    // 创建新列表，包含原列表的剩余元素
    if (list->list == NULL || list->list->next == NULL) {
        return create_atom_node("nil");
    }
    
    return list->list->next;  // 返回第二个及之后的元素
}

// cons操作 - 将元素添加到列表头部
ASTNode* op_cons(ASTNode* args) {
    if (args == NULL) return create_atom_node("nil");
    
    ASTNode* first = Eval(args, global_env);
    ASTNode* second = (args->next != NULL) ? Eval(args->next, global_env) : create_atom_node("nil");
    
    // 创建新节点
    ASTNode* new_node = (ASTNode*)malloc(sizeof(ASTNode));
    new_node->type = LIST;
    new_node->list = first;
    
    // 将second作为剩余部分
    if (second->type == LIST) {
        first->next = second->list;
    } else {
        first->next = second;
    }
    
    return new_node;
}

// 相等比较
ASTNode* op_eq(ASTNode* args) {
    if (args == NULL || args->next == NULL) {
        return create_atom_node("false");
    }
    
    ASTNode* first = Eval(args, global_env);
    ASTNode* second = Eval(args->next, global_env);
    
    if (first->type == ATOM && second->type == ATOM) {
        if (strcmp(first->atom, second->atom) == 0) {
            return create_atom_node("true");
        }
    }
    
    return create_atom_node("false");
}

// 小于比较
ASTNode* op_lt(ASTNode* args) {
    if (args == NULL || args->next == NULL) {
        return create_atom_node("false");
    }
    
    ASTNode* first = Eval(args, global_env);
    ASTNode* second = Eval(args->next, global_env);
    
    if (first->type == ATOM && second->type == ATOM && 
        is_number(first->atom) && is_number(second->atom)) {
        if (atoi(first->atom) < atoi(second->atom)) {
            return create_atom_node("true");
        }
    }
    
    return create_atom_node("false");
}

// 大于比较
ASTNode* op_gt(ASTNode* args) {
    if (args == NULL || args->next == NULL) {
        return create_atom_node("false");
    }
    
    ASTNode* first = Eval(args, global_env);
    ASTNode* second = Eval(args->next, global_env);
    
    if (first->type == ATOM && second->type == ATOM && 
        is_number(first->atom) && is_number(second->atom)) {
        if (atoi(first->atom) > atoi(second->atom)) {
            return create_atom_node("true");
        }
    }
    
    return create_atom_node("false");
}

// 注册所有内置函数
void register_builtins(HashTable* env) {
    if (env == NULL) return;
    
    // 算术运算
    hash_table_put(env, "+", op_add);
    hash_table_put(env, "-", op_sub);
    hash_table_put(env, "*", op_mul);
    hash_table_put(env, "/", op_div);
    
    // 特殊形式
    hash_table_put(env, "quote", op_quote);
    hash_table_put(env, "'", op_quote);
    
    // 列表操作
    hash_table_put(env, "car", op_car);
    hash_table_put(env, "cdr", op_cdr);
    hash_table_put(env, "cons", op_cons);
    
    // 比较操作
    hash_table_put(env, "eq?", op_eq);
    hash_table_put(env, "<", op_lt);
    hash_table_put(env, ">", op_gt);
    
    printf("内置函数注册完成，共注册 %d 个函数\n", env->count);
}