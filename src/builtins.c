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
        if (evaluated != NULL && evaluated->type == NODE_ATOM && 
            evaluated->atom.atom_type == ATOM_NUMBER) {
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
    if (first_eval == NULL || first_eval->type != NODE_ATOM || 
        first_eval->atom.atom_type != ATOM_NUMBER) {
        return make_number(0);
    }
    
    int result = atoi(first_eval->atom.value);
    ASTNode* current = args->next;
    
    while (current != NULL) {
        ASTNode* evaluated = Eval(current, env);
        if (evaluated != NULL && evaluated->type == NODE_ATOM &&
            evaluated->atom.atom_type == ATOM_NUMBER) {
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
        if (evaluated != NULL && evaluated->type == NODE_ATOM &&
            evaluated->atom.atom_type == ATOM_NUMBER) {
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
    if (first_eval == NULL || first_eval->type != NODE_ATOM ||
        first_eval->atom.atom_type != ATOM_NUMBER) {
        return make_number(0);
    }
    
    int result = atoi(first_eval->atom.value);
    ASTNode* current = args->next;
    
    while (current != NULL) {
        ASTNode* evaluated = Eval(current, env);
        if (evaluated != NULL && evaluated->type == NODE_ATOM &&
            evaluated->atom.atom_type == ATOM_NUMBER) {
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
    
    ASTNode* list = Eval(args, env);
    if (list == NULL || list->type != NODE_LIST) {
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
    if (list == NULL || list->type != NODE_LIST) {
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
    if (old_list == NULL || old_list->type != NODE_LIST) {
        return create_list_node(new_elem);
    }
    
    // 复制 new_elem
    ASTNode* new_elem_copy;
    if (new_elem->type == NODE_ATOM) {
        new_elem_copy = create_atom_node(new_elem->atom.value);
        set_atom_type(new_elem_copy, new_elem->atom.atom_type);
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
    if (list == NULL || list->type != NODE_LIST) {
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
    if (list == NULL || list->type != NODE_LIST) {
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
        if (evaluated->type == NODE_ATOM) {
            copy = create_atom_node(evaluated->atom.value);
            set_atom_type(copy, evaluated->atom.atom_type);
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
    
    if (first->type == NODE_ATOM && second->type == NODE_ATOM) {
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
    
    if (first->type == NODE_ATOM && first->atom.atom_type == ATOM_NUMBER &&
        second->type == NODE_ATOM && second->atom.atom_type == ATOM_NUMBER) {
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
    
    if (first->type == NODE_ATOM && first->atom.atom_type == ATOM_NUMBER &&
        second->type == NODE_ATOM && second->atom.atom_type == ATOM_NUMBER) {
        if (atoi(first->atom.value) > atoi(second->atom.value)) {
            return make_symbol("true");
        }
    }
    
    return make_symbol("false");
}


// 多参数版本 - 小于等于
ASTNode* op_le_multi(ASTNode* args, Env* env) {
    if (args == NULL || args->next == NULL) {
        return make_symbol("true");  // 单个参数总是 true
    }
    
    ASTNode* prev = Eval(args, env);
    ASTNode* current = args->next;
    
    while (current != NULL) {
        ASTNode* curr_val = Eval(current, env);
        
        // 类型检查
        if (prev->type != NODE_ATOM || curr_val->type != NODE_ATOM) {
            return make_symbol("false");
        }
        
        // 数字比较
        if (prev->atom.atom_type == ATOM_NUMBER && 
            curr_val->atom.atom_type == ATOM_NUMBER) {
            int a = atoi(prev->atom.value);
            int b = atoi(curr_val->atom.value);
            if (a > b) return make_symbol("false");
        }
        // 字符串比较
        else if (prev->atom.atom_type == ATOM_STRING && 
                 curr_val->atom.atom_type == ATOM_STRING) {
            if (strcmp(prev->atom.value, curr_val->atom.value) > 0) {
                return make_symbol("false");
            }
        }
        else {
            return make_symbol("false");  // 类型不匹配
        }
        
        prev = curr_val;
        current = current->next;
    }
    
    return make_symbol("true");
}

// 多参数版本 - 大于等于
ASTNode* op_ge_multi(ASTNode* args, Env* env) {
    if (args == NULL || args->next == NULL) {
        return make_symbol("true");
    }
    
    ASTNode* prev = Eval(args, env);
    ASTNode* current = args->next;
    
    while (current != NULL) {
        ASTNode* curr_val = Eval(current, env);
        
        if (prev->type != NODE_ATOM || curr_val->type != NODE_ATOM) {
            return make_symbol("false");
        }
        
        if (prev->atom.atom_type == ATOM_NUMBER && 
            curr_val->atom.atom_type == ATOM_NUMBER) {
            int a = atoi(prev->atom.value);
            int b = atoi(curr_val->atom.value);
            if (a < b) return make_symbol("false");
        }
        else if (prev->atom.atom_type == ATOM_STRING && 
                 curr_val->atom.atom_type == ATOM_STRING) {
            if (strcmp(prev->atom.value, curr_val->atom.value) < 0) {
                return make_symbol("false");
            }
        }
        else {
            return make_symbol("false");
        }
        
        prev = curr_val;
        current = current->next;
    }
    
    return make_symbol("true");
}

// 字符串长度
ASTNode* op_str_len(ASTNode* args, Env* env) {
    if (args == NULL) return make_number(0);
    
    ASTNode* str = Eval(args, env);
    if (str == NULL || str->type != NODE_ATOM || str->atom.atom_type != ATOM_STRING) {
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
        if (arg != NULL && arg->type == NODE_ATOM && arg->atom.atom_type == ATOM_STRING) {
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
        if (arg != NULL && arg->type == NODE_ATOM && arg->atom.atom_type == ATOM_STRING) {
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
    } else if (cond->type == NODE_ATOM && cond->atom.atom_type == ATOM_SYMBOL) {
        // 检查是否是 false/nil（作为符号）
        if (cond->atom.atom_type == ATOM_SYMBOL) {
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
        return Eval(then_node, env);
    } else {
        // 条件为假，执行 else 分支（如果有）
        if (else_node == NULL) return make_symbol("nil");
        return Eval(else_node, env);
    }
}

// def 定义变量
ASTNode* op_def(ASTNode* args, Env* env) {
    if (args == NULL || args->next == NULL) {
        return make_symbol("error");
    }
    
    ASTNode* name_node = args;
    if (name_node->type != NODE_ATOM || name_node->atom.atom_type != ATOM_SYMBOL) {
        return make_symbol("error");
    }
    
    ASTNode* value_node = args->next;
    ASTNode* value = Eval(value_node, env);
    
    // 存入变量表
    env_define_var(env, name_node->atom.value, value);
    
    return value;
}

// set 修改变量
ASTNode* op_set(ASTNode* args, Env* env) {
    // [set x 20]
    if (args == NULL || args->next == NULL) {
        return make_symbol("error");
    }
    
    ASTNode* name_node = args;
    if (name_node->type != NODE_ATOM || name_node->atom.atom_type != ATOM_SYMBOL) {
        return make_symbol("error");
    }
    
    ASTNode* value_node = args->next;
    ASTNode* value = Eval(value_node, env);
    
    // 修改环境中的变量
    env_set_var(env, name_node->atom.value, value);
    
    return value;
}

/**
 * let 特殊形式 - 创建局部变量绑定
 * 
 * 语法: [let [[x 10] [y 20]] [* x y]]
 * 
 * @param args 参数列表，包含:
 *             - 第一个参数: 绑定列表 [[x 10] [y 20]]
 *             - 第二个参数: 函数体 [* x y]
 * @param env 当前环境
 * @return 函数体求值结果
 */
ASTNode* op_let(ASTNode* args, Env* env) {
    
    #if DEBUG_LET
    printf("\n=== op_let start ===\n");
    printf("Args: ");
    print_ast_tree(args, 0, 1);
    #endif
    
    // ---------- 参数检查 ----------
    // let 需要至少两个参数：绑定列表和函数体
    if (args == NULL || args->next == NULL) {
        #if DEBUG_LET
        printf("Error: let requires bindings and body\n");
        #endif
        return make_symbol("error");
    }
    
    // ---------- 解析参数 ----------
    // 第一个参数：绑定列表，格式如 [[x 10] [y 20]]
    ASTNode* bindings = args;
    
    // 第二个参数：函数体，格式如 [* x y]
    ASTNode* body = args->next;
    
    #if DEBUG_LET
    printf("Bindings: ");
    print_ast_tree(bindings, 0, 1);
    printf("Body: ");
    print_ast_tree(body, 0, 1);
    #endif
    
    // ---------- 创建新环境 ----------
    // 创建词法作用域，外层是当前环境
    Env* new_env = env_create(env);
    
    #if DEBUG_LET
    printf("Old environment: %p\n", (void*)env);
    printf("New environment created: %p\n", (void*)new_env);
    #endif
    
    // ---------- 处理变量绑定 ----------
    // bindings 应该是列表，其每个元素是 [var val] 形式的列表
    ASTNode* current = bindings->list;  // 获取绑定列表的第一个元素
    
    while (current != NULL) {
        // 每个 binding 是 [x 10] 这样的列表
        ASTNode* binding = current->list;
        
        if (binding && binding->next) {
            #if DEBUG_LET
            printf("\nProcessing binding: ");
            print_ast_tree(binding, 0, 1);
            #endif
            
            // 变量名（必须是符号）
            ASTNode* name = binding;
            
            // 值表达式
            ASTNode* val_expr = binding->next;
            
            #if DEBUG_LET
            printf("  Variable name: %s\n", name->atom.value);
            printf("  Value expression: ");
            print_ast_tree(val_expr, 0, 1);
            #endif
            
            // 在当前环境对值表达式求值
            // 注意：使用 env 而不是 new_env，这是并行绑定语义
            ASTNode* val = Eval(val_expr, env);
            
            #if DEBUG_LET
            printf("  Evaluated value: ");
            print_ast_tree(val, 0, 1);
            #endif
            
            // 在新环境中定义变量
            env_define_var(new_env, name->atom.value, val);
            
            #if DEBUG_LET
            // 验证变量是否成功定义
            ASTNode* check = env_lookup_var(new_env, name->atom.value);
            printf("  Verification: ");
            if (check) {
                print_ast_tree(check, 0, 1);
            } else {
                printf("FAILED - variable not found in new_env!\n");
            }
            #endif
        }
        
        current = current->next;
    }
    
    #if DEBUG_LET
    printf("\nAll bindings processed, evaluating body in new environment\n");
    #endif
    
    // ---------- 在新环境中求值函数体 ----------
    ASTNode* result = Eval(body, new_env);

    env_free(new_env);  // 释放新环境
    
    return result;
    
    #undef LET_DEBUG  // 清理宏定义
}



ASTNode* op_lambda(ASTNode* args, Env* env) {
    #if DEBUG_LAMBDA
    printf("\n=== op_lambda start ===\n");
    #endif
    
    if (args == NULL || args->next == NULL) {
        return make_symbol("error");
    }
    
    ASTNode* params = args;
    ASTNode* body = args->next;
    
    #if DEBUG_LAMBDA
    printf("Params: ");
    print_ast_tree(params, 0, 1);
    printf("Body: ");
    print_ast_tree(body, 0, 1);
    #endif
    
    // 创建函数节点（捕获当前环境）
    ASTNode* func = create_function_node(params, body, env);
    
    #if DEBUG_LAMBDA
    printf("Function created at %p\n", (void*)func);
    printf("=== op_lambda end ===\n");
    #endif
    
    return func;
}

void register_builtins(Env* env) {
    if (env == NULL) return;
    
    // 算术运算
    env_define_func(env, "+", op_add);
    env_define_func(env, "-", op_sub);
    env_define_func(env, "*", op_mul);
    env_define_func(env, "/", op_div);
    
    // 特殊形式
    env_define_func(env, "quote", op_quote);
    env_define_func(env, "'", op_quote);
    
    // 列表操作
    env_define_func(env, "car", op_car);
    env_define_func(env, "cdr", op_cdr);
    env_define_func(env, "cons", op_cons);
    env_define_func(env, "null?", op_null);
    env_define_func(env, "length", op_length);
    env_define_func(env, "list", op_list);
    
    // 比较操作
    env_define_func(env, "eq?", op_eq);
    env_define_func(env, "<", op_lt);
    env_define_func(env, ">", op_gt);
    env_define_func(env, "<=", op_le_multi);
    env_define_func(env, ">=", op_ge_multi);
    
    // 字符串操作
    env_define_func(env, "str-len", op_str_len);
    env_define_func(env, "str-cat", op_str_cat);

    // 特殊形式
    env_define_func(env, "if", op_if);
    env_define_func(env, "def", op_def);
    env_define_func(env, "set", op_set);
    env_define_func(env, "let", op_let);
    env_define_func(env, "lambda", op_lambda);
    env_define_func(env, "fn", op_lambda);
    printf("Built-in functions registered: %d\n", env->functions->count);
}