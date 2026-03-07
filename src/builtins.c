#include "builtins.h"
#include "evaluator.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 辅助函数：创建数字节点
ASTNode* make_number(int n) {
    char buffer[20];
    sprintf(buffer, "%d", n);
    ASTNode* node = create_atom_node(buffer);
    set_atom_type(node, ATOM_NUMBER);
    return node;
}

// 辅助函数：创建符号节点
ASTNode* make_symbol(const char* s) {
    ASTNode* node = create_atom_node(s);
    set_atom_type(node, ATOM_SYMBOL);
    return node;
}

// 加法操作
ASTNode* op_add(ASTNode* args,Env* env) {
    if (args == NULL) return make_number(0);
    
    int sum = 0;
    ASTNode* current = args;
    
    while (current != NULL) {
        ASTNode* evaluated = Eval(current, env);
        if (evaluated != NULL && evaluated->type == ATOM && 
            evaluated->atom.type == ATOM_NUMBER) {
            sum += atoi(evaluated->atom.value);
        }
        current = current->next;
    }
    
    return make_number(sum);
}

// 减法操作
ASTNode* op_sub(ASTNode* args, Env* env) {
    if (args == NULL) return make_number(0);
    
    ASTNode* first_eval = Eval(args, env);
    if (first_eval == NULL || first_eval->type != ATOM || 
        first_eval->atom.type != ATOM_NUMBER) {
        return make_number(0);
    }
    
    int result = atoi(first_eval->atom.value);
    ASTNode* current = args->next;
    
    while (current != NULL) {
        ASTNode* evaluated = Eval(current, env);
        if (evaluated != NULL && evaluated->type == ATOM &&
            evaluated->atom.type == ATOM_NUMBER) {
            result -= atoi(evaluated->atom.value);
        }
        current = current->next;
    }
    
    return make_number(result);
}

// 乘法操作
ASTNode* op_mul(ASTNode* args, Env* env) {
    if (args == NULL) return make_number(1);
    
    int result = 1;
    ASTNode* current = args;
    
    while (current != NULL) {
        ASTNode* evaluated = Eval(current, env);
        if (evaluated != NULL && evaluated->type == ATOM &&
            evaluated->atom.type == ATOM_NUMBER) {
            result *= atoi(evaluated->atom.value);
        }
        current = current->next;
    }
    
    return make_number(result);
}

// 除法操作
ASTNode* op_div(ASTNode* args, Env* env) {
    if (args == NULL) return make_number(0);
    
    ASTNode* first_eval = Eval(args, env);
    if (first_eval == NULL || first_eval->type != ATOM ||
        first_eval->atom.type != ATOM_NUMBER) {
        return make_number(0);
    }
    
    int result = atoi(first_eval->atom.value);
    ASTNode* current = args->next;
    
    while (current != NULL) {
        ASTNode* evaluated = Eval(current, env);
        if (evaluated != NULL && evaluated->type == ATOM &&
            evaluated->atom.type == ATOM_NUMBER) {
            int divisor = atoi(evaluated->atom.value);
            if (divisor != 0) {
                result /= divisor;
            }
        }
        current = current->next;
    }
    
    return make_number(result);
}

// quote操作 - 特殊形式，不对参数求值
ASTNode* op_quote(ASTNode* args, Env* env) {
    if (args == NULL) return make_symbol("nil");
    return args;  // 返回第一个参数（不求值）
}

// car操作 - 返回列表的第一个元素
ASTNode* op_car(ASTNode* args, Env* env) {
    if (args == NULL) return make_symbol("nil");
    
    ASTNode* list = Eval(args, global_env);
    if (list == NULL || list->type != LIST) {
        return make_symbol("nil");
    }
    
    ASTNode* first = list->list;
    if (first == NULL) return make_symbol("nil");
    
    return first;  // 返回第一个元素
}

// cdr操作 - 返回列表的剩余部分
ASTNode* op_cdr(ASTNode* args, Env* env) {
    if (args == NULL) return make_symbol("nil");
    
    ASTNode* list = Eval(args, env);
    if (list == NULL || list->type != LIST) {
        return make_symbol("nil");
    }
    
    ASTNode* first = list->list;
    if (first == NULL) return make_symbol("nil");
    
    ASTNode* rest = first->next;
    if (rest == NULL) return make_symbol("nil");
    
    return create_list_node(rest);
}

// cons操作 - 将元素添加到列表头部
ASTNode* op_cons(ASTNode* args, Env* env) {
    if (args == NULL) return make_symbol("nil");
    
    // 第一个参数是新元素
    ASTNode* first_arg = args;
    ASTNode* new_elem = Eval(first_arg, env);
    if (new_elem == NULL) return make_symbol("nil");
    
    // 第二个参数是原列表
    ASTNode* second_arg = args->next;
    if (second_arg == NULL) {
        // 只有一个参数，返回 (new_elem)
        return create_list_node(new_elem);
    }
    
    ASTNode* old_list = Eval(second_arg, env);
    
    // 如果第二个参数不是列表，当作空列表处理
    if (old_list == NULL || old_list->type != LIST) {
        return create_list_node(new_elem);
    }
    
    // 复制 new_elem
    ASTNode* new_elem_copy;
    if (new_elem->type == ATOM) {
        new_elem_copy = create_atom_node(new_elem->atom.value);
        set_atom_type(new_elem_copy, new_elem->atom.type);
    } else {
        new_elem_copy = new_elem;  // 简化处理
    }
    
    // new_elem_copy 的 next 指向原列表的第一个元素
    new_elem_copy->next = old_list->list;
    
    return create_list_node(new_elem_copy);
}

// 判断列表是否为空
ASTNode* op_null(ASTNode* args, Env* env) {
    if (args == NULL) return make_symbol("true");
    
    ASTNode* list = Eval(args, env);
    if (list == NULL || list->type != LIST) {
        return make_symbol("true");
    }
    
    if (list->list == NULL) {
        return make_symbol("true");
    }
    
    return make_symbol("false");
}

// 获取列表长度
ASTNode* op_length(ASTNode* args, Env* env) {
    if (args == NULL) return make_number(0);
    
    ASTNode* list = Eval(args, env);
    if (list == NULL || list->type != LIST) {
        return make_number(0);
    }
    
    int len = 0;
    ASTNode* current = list->list;
    while (current != NULL) {
        len++;
        current = current->next;
    }
    
    return make_number(len);
}

// 创建列表
ASTNode* op_list(ASTNode* args, Env* env) {
    if (args == NULL) return create_list_node(NULL);
    
    ASTNode* first_eval = NULL;
    ASTNode* last_eval = NULL;
    ASTNode* current = args;
    
    while (current != NULL) {
        ASTNode* evaluated = Eval(current, env);
        if (evaluated == NULL) {
            current = current->next;
            continue;
        }
        
        // 复制节点
        ASTNode* copy;
        if (evaluated->type == ATOM) {
            copy = create_atom_node(evaluated->atom.value);
            set_atom_type(copy, evaluated->atom.type);
        } else {
            copy = evaluated;  // 简化处理
        }
        
        if (first_eval == NULL) {
            first_eval = copy;
            last_eval = copy;
        } else {
            last_eval->next = copy;
            last_eval = copy;
        }
        
        current = current->next;
    }
    
    return create_list_node(first_eval);
}

// 相等比较（符号和数字）
ASTNode* op_eq(ASTNode* args, Env* env) {
    if (args == NULL || args->next == NULL) {
        return make_symbol("false");
    }
    
    ASTNode* first = Eval(args, env);
    ASTNode* second = Eval(args->next, env);
    
    if (first->type == ATOM && second->type == ATOM) {
        if (strcmp(first->atom.value, second->atom.value) == 0) {
            return make_symbol("true");
        }
    }
    
    return make_symbol("false");
}

// 小于比较（只用于数字）
ASTNode* op_lt(ASTNode* args, Env* env) {
    if (args == NULL || args->next == NULL) {
        return make_symbol("false");
    }
    
    ASTNode* first = Eval(args, env);
    ASTNode* second = Eval(args->next, env);
    
    if (first->type == ATOM && first->atom.type == ATOM_NUMBER &&
        second->type == ATOM && second->atom.type == ATOM_NUMBER) {
        if (atoi(first->atom.value) < atoi(second->atom.value)) {
            return make_symbol("true");
        }
    }
    
    return make_symbol("false");
}

// 大于比较（只用于数字）
ASTNode* op_gt(ASTNode* args, Env* env) {
    if (args == NULL || args->next == NULL) {
        return make_symbol("false");
    }
    
    ASTNode* first = Eval(args, env);
    ASTNode* second = Eval(args->next, env);
    
    if (first->type == ATOM && first->atom.type == ATOM_NUMBER &&
        second->type == ATOM && second->atom.type == ATOM_NUMBER) {
        if (atoi(first->atom.value) > atoi(second->atom.value)) {
            return make_symbol("true");
        }
    }
    
    return make_symbol("false");
}

// 字符串长度
ASTNode* op_str_len(ASTNode* args, Env* env) {
    if (args == NULL) return make_number(0);
    
    ASTNode* str = Eval(args, env);
    if (str == NULL || str->type != ATOM || str->atom.type != ATOM_STRING) {
        return make_number(0);
    }
    
    int len = strlen(str->atom.value);
    return make_number(len);
}

// 字符串拼接
ASTNode* op_str_cat(ASTNode* args, Env* env) {
    if (args == NULL) return make_symbol("nil");
    
    // 计算总长度
    int total_len = 0;
    ASTNode* current = args;
    while (current != NULL) {
        ASTNode* arg = Eval(current, env);
        if (arg != NULL && arg->type == ATOM && arg->atom.type == ATOM_STRING) {
            total_len += strlen(arg->atom.value);
        }
        current = current->next;
    }
    
    // 拼接
    char* result = (char*)malloc(total_len + 1);
    result[0] = '\0';
    
    current = args;
    while (current != NULL) {
        ASTNode* arg = Eval(current, env);
        if (arg != NULL && arg->type == ATOM && arg->atom.type == ATOM_STRING) {
            strcat(result, arg->atom.value);
        }
        current = current->next;
    }
    
    ASTNode* node = create_atom_node(result);
    set_atom_type(node, ATOM_STRING);
    free(result);
    return node;
}


// if 条件判断
// 语法: [if condition then-expr else-expr]
// 示例: [if (> x 5) q"big" q"small"]
ASTNode* op_if(ASTNode* args, Env* env) {
    // printf("op_if called\n");
    
    // 检查参数个数（至少需要 condition 和 then-expr）
    if (args == NULL) {
        // printf("Error: if missing condition\n");
        return make_symbol("nil");
    }
    
    // 第一个参数：条件
    ASTNode* cond_node = args;
    ASTNode* cond = Eval(cond_node, env);
    
    // 第二个参数：then 分支
    ASTNode* then_node = args->next;
    
    // 第三个参数：else 分支（可选）
    ASTNode* else_node = args->next ? args->next->next : NULL;
    
    // printf("Condition evaluated to: ");
    // if (cond) print_ast_tree(cond, 0, 1);
    // else printf("NULL\n");
    
    // 判断条件是否为真
    // 在 Lisp 中，只有 false/nil 是假，其他都是真
    int is_true = 1;  // 默认真
    
    if (cond == NULL) {
        is_true = 0;
    } else if (cond->type == ATOM) {
        // 检查是否是 false/nil（作为符号）
        if (cond->atom.type == ATOM_SYMBOL) {
            if (strcmp(cond->atom.value, "false") == 0 ||
                strcmp(cond->atom.value, "nil") == 0) {
                is_true = 0;
            }
        }
        // 数字：0 是假？还是所有非 nil 都真？
        // 传统 Lisp 中，只有 nil 是假，0 是真
        // 这里我们采用传统 Lisp 规则
    }
    
    // printf("Condition is %s\n", is_true ? "true" : "false");
    
    if (is_true) {
        // 条件为真，执行 then 分支
        if (then_node == NULL) return make_symbol("nil");
        return Eval(then_node, global_env);
    } else {
        // 条件为假，执行 else 分支（如果有）
        if (else_node == NULL) return make_symbol("nil");
        return Eval(else_node, global_env);
    }
}

// def 定义变量
ASTNode* op_def(ASTNode* args, Env* env) {
    // [def x 10]
    if (args == NULL || args->next == NULL) {
        return make_symbol("error");
    }
    
    // 第一个参数是变量名（必须是符号）
    ASTNode* name_node = args;
    if (name_node->type != ATOM || name_node->atom.type != ATOM_SYMBOL) {
        return make_symbol("error");
    }
    
    // 第二个参数是值（需要求值）
    ASTNode* value_node = args->next;
    ASTNode* value = Eval(value_node, env);
    
    // 存入环境
    env_define(env, name_node->atom.value, value);
    
    return value;
}

// set 修改变量
ASTNode* op_set(ASTNode* args, Env* env) {
    // [set x 20]
    if (args == NULL || args->next == NULL) {
        return make_symbol("error");
    }
    
    ASTNode* name_node = args;
    if (name_node->type != ATOM || name_node->atom.type != ATOM_SYMBOL) {
        return make_symbol("error");
    }
    
    ASTNode* value_node = args->next;
    ASTNode* value = Eval(value_node, env);
    
    // 修改环境中的变量
    env_set(env, name_node->atom.value, value);
    
    return value;
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
    hash_table_put(env, "null?", op_null);
    hash_table_put(env, "length", op_length);
    hash_table_put(env, "list", op_list);
    
    // 比较操作
    hash_table_put(env, "eq?", op_eq);
    hash_table_put(env, "<", op_lt);
    hash_table_put(env, ">", op_gt);
    
    // 字符串操作
    hash_table_put(env, "str-len", op_str_len);
    hash_table_put(env, "str-cat", op_str_cat);

    hash_table_put(env, "if", op_if);
    hash_table_put(env, "def", op_def);
    hash_table_put(env, "set", op_set);
    
    printf("内置函数注册完成，共注册 %d 个函数\n", env->count);
}