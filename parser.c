#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"

// Helper function: Create atom node
ASTNode* create_atom_node(const char* value) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (!node) return NULL;
    
    node->type = ATOM;
    node->atom = strdup(value);
    if (!node->atom) {
        free(node);
        return NULL;
    }
    return node;
}

// Helper function: Create list node
ASTNode* create_list_node(ASTNode** children, int count) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (!node) return NULL;
    
    node->type = LIST;
    node->list = (ASTNode**)malloc(sizeof(ASTNode*) * (count + 1));
    if (!node->list) {
        free(node);
        return NULL;
    }
    
    for (int i = 0; i < count; i++) {
        node->list[i] = children[i];
    }
    node->list[count] = NULL; // NULL terminated
    
    return node;
}

// Helper function: Free AST
void free_ast(ASTNode* node) {
    if (!node) return;
    
    if (node->type == ATOM) {
        free(node->atom);
    } 
    else if (node->type == LIST) {
        if (node->list) {
            for (int i = 0; node->list[i] != NULL; i++) {
                free_ast(node->list[i]);
            }
            free(node->list);
        }
    }
    free(node);
}

// Helper function: Print AST in tree format
void print_ast_tree(ASTNode* node, int depth, int is_last) {
    if (!node) return;
    
    // Print indentation
    for (int i = 0; i < depth - 1; i++) {
        printf("│   ");
    }
    if (depth > 0) {
        printf(is_last ? "└── " : "├── ");
    }
    
    if (node->type == ATOM) {
        printf("ATOM: \"%s\"\n", node->atom);
    } else if (node->type == LIST) {
        printf("LIST\n");
        if (node->list) {
            for (int i = 0; node->list[i] != NULL; i++) {
                int last_child = (node->list[i + 1] == NULL);
                print_ast_tree(node->list[i], depth + 1, last_child);
            }
        } else {
            for (int i = 0; i < depth; i++) {
                printf("    ");
            }
            printf("    (empty list)\n");
        }
    }
}

void test_ast_structure(){
	ASTNode* list[4];
	ASTNode* atom1 = create_atom_node("+");
	ASTNode* atom2 = create_atom_node("4");
	ASTNode* atom3 = create_atom_node("6");
	ASTNode* atom4 = create_atom_node("2");
	list[0] = atom1;
	list[1] = atom2;
	list[2] = atom3;
	list[3] = atom4;

	ASTNode* ast = create_list_node(list,4);
	print_ast_tree(ast,0,1);

}


int main() {
    test_ast_structure();
    return 0;
}