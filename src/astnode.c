#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "astnode.h"

// Helper function: Create atom node 
ASTNode* create_atom_node(const char* value) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (!node) return NULL;
    
    node->type = ATOM;
    node->next = NULL;
    
    // 复制字符串到 atom.value
    node->atom.value = (char*)malloc(strlen(value) + 1);
    if (!node->atom.value) {
        free(node);
        return NULL;
    }
    strcpy(node->atom.value, value);
    
    // 默认类型设为 symbol，由调用者显式设置
    node->atom.type = ATOM_SYMBOL;
    
    return node;
}

// 新增：设置原子类型的函数
void set_atom_type(ASTNode* node, AtomType type) {
    if (node && node->type == ATOM) {
        node->atom.type = type;
    }
}

// 新增：创建指定类型的原子节点
ASTNode* create_typed_atom(const char* value, AtomType type) {
    ASTNode* node = create_atom_node(value);
    if (node) {
        node->atom.type = type;
    }
    return node;
}

// Helper function: Create list node
ASTNode* create_list_node(ASTNode* children) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (!node) return NULL;
    
    node->type = LIST;
    node->next = NULL;
    node->list = children;

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
ASTNode* copy_ast_node(ASTNode* node) {
    if (node == NULL) return NULL;
    
    if (node->type == ATOM) {
        ASTNode* copy = create_atom_node(node->atom.value);
        set_atom_type(copy, node->atom.type);
        return copy;
    } else if (node->type == LIST) {
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
    
    return NULL;
}


void free_ast(ASTNode* node) {
    if (!node) return;
    
    // 先释放同级节点
    if (node->next) {
        free_ast(node->next);
    }
    
    if (node->type == ATOM) {
        // 释放 atom.value
        if (node->atom.value) {
            free(node->atom.value);
        }
    } 
    else if (node->type == LIST) {
        // 递归释放子节点
        if (node->list) {
            free_ast(node->list);
        }
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
    
    if (node->type == ATOM) {
        printf("> ATOM: \"%s\"", node->atom.value);
        // 显示类型
        switch (node->atom.type) {
            case ATOM_NUMBER: printf(" (number)"); break;
            case ATOM_STRING: printf(" (string)"); break;
            case ATOM_SYMBOL: printf(" (symbol)"); break;
        }
        printf("\n");
    } else if (node->type == LIST) {
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
    }
}

// 辅助函数：打印同级节点（用于调试）
void print_siblings(ASTNode* head) {
    printf("Sibling chain: ");
    ASTNode* current = head;
    while (current) {
        if (current->type == ATOM) {
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