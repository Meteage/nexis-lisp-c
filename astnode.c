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
    node->atom = strdup(value);
    if (!node->atom) {
        free(node);
        return NULL;
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
ASTNode* append_sibling(ASTNode* tail, ASTNode* node){
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

void free_ast(ASTNode* node) {
    if (!node) return;
    
    // 先释放同级节点
    if (node->next) {
        free_ast(node->next);
    }
    
    if (node->type == ATOM) {
        free(node->atom);
    } 
    else if (node->type == LIST) {
        // 递归释放子节点
        if (node->list) {
            free_ast(node->list);
        }
    }
    free(node);
}

// Helper function: Print AST in tree format（修正版）
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
        printf("> ATOM: \"%s\"\n", node->atom);
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
            printf("\"%s\"", current->atom);
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

void test_ast_structure(){
    printf("=== Testing AST Structure ===\n");
    
    // 创建原子节点
    ASTNode* atom1 = create_atom_node("+");
    ASTNode* atom2 = create_atom_node("4");
    ASTNode* atom3 = create_atom_node("6");
    ASTNode* atom4 = create_atom_node("2");
    
    // 创建子节点链表：+ -> 4 -> 6 -> 2
    ASTNode* children = atom1;
    append_sibling(atom1, atom2);  // atom1->next = atom2
    append_sibling(atom2, atom3);  // atom2->next = atom3  
    append_sibling(atom3, atom4);  // atom3->next = atom4
    
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

// 测试嵌套列表
void test_nested_ast() {
    printf("\n=== Testing Nested AST ===\n");
    
    // 创建表达式: (+ (* 3 4) 5)
    
    // 内部列表：(* 3 4)
    ASTNode* multiply = create_atom_node("*");
    ASTNode* num3 = create_atom_node("3");
    ASTNode* num4 = create_atom_node("4");
    multiply->next = num3;
    num3->next = num4;
    
    ASTNode* multiply_list = create_list_node(multiply);
    
    // 外部列表：(+ (* 3 4) 5)
    ASTNode* plus = create_atom_node("+");
    ASTNode* num5 = create_atom_node("5");
    
    // 连接：+ -> (* 3 4) -> 5
    plus->next = multiply_list;
    multiply_list->next = num5;
    
    ASTNode* addition_list = create_list_node(plus);
    
    printf("AST for (+ (* 3 4) 5):\n");
    print_ast_tree(addition_list, 0, 1);
    
    free_ast(addition_list);
    printf("=== Test Complete ===\n");
}

/*
int main() {
    test_ast_structure();
    test_nested_ast();
    return 0;
}
*/