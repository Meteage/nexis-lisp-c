#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "astnode.h"

// 创建原子节点
ASTNode* create_atom_node(const char* value) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (!node) return NULL;
    
    node->type = NODE_ATOM;
    node->next = NULL;
    node->atom.value = (char*)malloc(strlen(value) + 1);
    strcpy(node->atom.value, value);
    node->atom.atom_type = ATOM_SYMBOL;  // 默认符号类型
    
    return node;
}

// 创建列表节点
ASTNode* create_list_node(ASTNode* children) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (!node) return NULL;
    
    node->type = NODE_LIST;
    node->next = NULL;
    node->list = children;
    
    return node;
}

// 创建函数节点
// astnode.c
ASTNode* create_function_node(ASTNode* params, ASTNode* body, Env* env) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (!node) return NULL;
    
    node->type = NODE_FUNCTION;
    node->next = NULL;
    
    // 深拷贝参数列表（因为输入的 ast 会被释放）
    node->func.params = copy_ast_node(params);
    // 深拷贝函数体（因为输入的 ast 会被释放）
    node->func.body = copy_ast_node(body);
    // 环境是引用，不需要拷贝
    node->func.env = env;
    
    return node;
}

// 新增：设置原子类型的函数
void set_atom_type(ASTNode* node, AtomType type) {
    if (node && node->type == NODE_ATOM) {
        node->atom.atom_type = type;
    }
}

// 新增：创建指定类型的原子节点
ASTNode* create_typed_atom(const char* value, AtomType type) {
    ASTNode* node = create_atom_node(value);
    if (node) {
        node->atom.atom_type = type;
    }
    return node;
}



// 添加同级节点（返回新的尾节点）
ASTNode* append_sibling(ASTNode* tail, ASTNode* node) {
    if (!tail || !node) return node;
    tail->next = node;
    return node;  // 返回新的尾节点
}

// 另一种更常用的版本：添加到链表末尾，返回头节点
ASTNode* append_sibling_to_list(ASTNode* head, ASTNode* node) {
    if (!head) return node;
    if (!node) return head;
    
    ASTNode* current = head;
    while (current->next) {
        current = current->next;
    }
    current->next = node;
    return head;
}

// 复制 ASTNode（深拷贝）
// astnode.c
ASTNode* copy_ast_node(ASTNode* node) {
    if (node == NULL) return NULL;
    
    if (node->type == NODE_ATOM) {
        ASTNode* copy = create_atom_node(node->atom.value);
        set_atom_type(copy, node->atom.atom_type);
        return copy;
    } 
    else if (node->type == NODE_LIST) {
        // 复制列表（需要递归复制每个元素）
        ASTNode* copy_list = NULL;
        ASTNode* last = NULL;
        ASTNode* current = node->list;
        
        while (current != NULL) {
            ASTNode* elem_copy = copy_ast_node(current);
            if (copy_list == NULL) {
                copy_list = elem_copy;
                last = elem_copy;
            } else {
                last->next = elem_copy;
                last = elem_copy;
            }
            current = current->next;
        }
        
        return create_list_node(copy_list);
    }
    else if (node->type == NODE_FUNCTION) {
       // 函数节点已经在 create_function_node 中复制过了
        // 这里只需要浅拷贝（因为内容是独立的）
        ASTNode* copy = (ASTNode*)malloc(sizeof(ASTNode));
        if (!copy) return NULL;
        
        copy->type = NODE_FUNCTION;
        copy->next = NULL;
        copy->func.params = node->func.params;  // 已经是副本，可以共享
        copy->func.body = node->func.body;      // 已经是副本，可以共享
        copy->func.env = node->func.env;
        
        return copy;
    }
    
    printf("copy_ast_node: unknown node type %d\n", node->type);
    return NULL;
}

// 释放AST内存
void free_ast(ASTNode* node) {
    if (!node) return;
    
    // 先释放同级节点
    if (node->next) {
        free_ast(node->next);
    }
    
    // 根据类型释放
    switch (node->type) {
        case NODE_ATOM:
            if (node->atom.value) {
                free(node->atom.value);
            }
            break;
            
        case NODE_LIST:
            if (node->list) {
                free_ast(node->list);
            }
            break;
            
        case NODE_FUNCTION:
            // 函数节点：参数和body是引用，不释放
            // 环境由环境系统管理
            break;
    }
    
    free(node);
}

// Helper function: Print AST in tree format
void print_ast_tree(ASTNode* node, int depth, int is_last) {
    if (!node) return;
    
    // 打印缩进
    for (int i = 0; i < depth; i++) {
        printf("  ");
    }
    
    // 打印连接线
    if (depth > 0) {
        printf(is_last ? "\\-" : "|-");
    }
    
    if (node->type == NODE_ATOM) {
        printf("> ATOM: \"%s\"", node->atom.value);
        // 显示类型
        switch (node->atom.atom_type) {
            case ATOM_NUMBER: printf(" (number)"); break;
            case ATOM_STRING: printf(" (string)"); break;
            case ATOM_SYMBOL: printf(" (symbol)"); break;
        }
        printf("\n");
    } else if (node->type == NODE_LIST) {
        printf("> LIST\n");
        if (node->list) {
            ASTNode* child = node->list;
            int child_index = 0;
            
            // 计算子节点数量
            int child_count = 0;
            ASTNode* temp = child;
            while (temp) {
                child_count++;
                temp = temp->next;
            }
            
            // 打印每个子节点
            temp = child;
            while (temp) {
                int is_last_child = (child_index == child_count - 1);
                print_ast_tree(temp, depth + 1, is_last_child);
                temp = temp->next;
                child_index++;
            }
        } else {
            for (int i = 0; i < depth + 1; i++) {
                printf("  ");
            }
            printf("(empty list)\n");
        }
    }else if(node->type == NODE_FUNCTION){
        printf("FUNCTION (closure)\n");
        // 可以选择打印更多信息
        for (int i = 0; i < depth + 1; i++) printf("  ");
        printf("params: ");
        print_ast_tree(node->func.params, 0, 1);
        for (int i = 0; i < depth + 1; i++) printf("  ");
        printf("body: ");
        print_ast_tree(node->func.body, 0, 1);
    }
}

// 辅助函数：打印同级节点（用于调试）
void print_siblings(ASTNode* head) {
    printf("Sibling chain: ");
    ASTNode* current = head;
    while (current) {
        if (current->type == NODE_ATOM) {
            printf("\"%s\"", current->atom.value);
        } else {
            printf("[LIST]");
        }
        if (current->next) {
            printf(" -> ");
        }
        current = current->next;
    }
    printf("\n");
}

void test_ast_structure() {
    printf("=== Testing AST Structure ===\n");
    
    // 创建原子节点 - 默认都是 symbol
    ASTNode* atom1 = create_atom_node("+");      // symbol
    ASTNode* atom2 = create_atom_node("4");      // symbol（不是number！）
    ASTNode* atom3 = create_atom_node("6");      // symbol
    ASTNode* atom4 = create_atom_node("2");      // symbol
    
    // 如果需要 number 类型，要显式设置
    // set_atom_type(atom2, number);
    
    // 创建子节点链表：+ -> 4 -> 6 -> 2
    ASTNode* children = atom1;
    append_sibling(atom1, atom2);
    append_sibling(atom2, atom3);  
    append_sibling(atom3, atom4);
    
    // 打印子节点链表
    printf("Children list: ");
    print_siblings(children);
    
    // 创建列表节点
    ASTNode* ast = create_list_node(children);
    
    // 打印AST树
    printf("\nAST Tree:\n");
    print_ast_tree(ast, 0, 1);
    
    // 清理内存
    free_ast(ast);
    printf("=== Test Complete ===\n");
}

// 测试显式类型设置
void test_explicit_types() {
    printf("\n=== Testing Explicit Types ===\n");
    
    // 创建各种类型的节点
    ASTNode* num = create_typed_atom("42", ATOM_NUMBER);
    ASTNode* str = create_typed_atom("hello world",ATOM_STRING);
    ASTNode* bool_true = create_typed_atom("true", ATOM_NUMBER);
    ASTNode* sym = create_atom_node("x");  // 默认 symbol
    
    printf("Explicitly typed nodes:\n");
    print_ast_tree(num, 0, 1);
    print_ast_tree(str, 0, 1);
    print_ast_tree(bool_true, 0, 1);
    print_ast_tree(sym, 0, 1);
    
    free_ast(num);
    free_ast(str);
    free_ast(bool_true);
    free_ast(sym);
    
    printf("=== Test Complete ===\n");
}

// 测试字符串节点 - 需要在解析时设置类型
void test_string_node() {
    printf("\n=== Testing String Node ===\n");
    
    // 假设解析器遇到 q"hello world" 时调用
    ASTNode* str_node = create_atom_node("hello world");
    set_atom_type(str_node, ATOM_STRING);  // 显式设置为 string 类型
    
    printf("String node: ");
    print_ast_tree(str_node, 0, 1);
    
    free_ast(str_node);
    printf("=== Test Complete ===\n");
}

/*
int main() {
    test_ast_structure();
    test_explicit_types();
    test_string_node();
    return 0;
}
*/