#ifndef _ASTNODE_H_
#define _ASTNODE_H_

typedef struct ASTNode
{
	enum {ATOM,LIST} type;//类型
	struct ASTNode * next;//同级节点
	union 
	{
		char * atom;
		struct ASTNode * list;//子级节点
	};
} ASTNode;


ASTNode* create_atom_node(const char* value);
ASTNode* create_list_node(ASTNode* children);
ASTNode* append_sibling(ASTNode* tail, ASTNode* node);
ASTNode* append_sibling_to_list(ASTNode* head, ASTNode* node);
void free_ast(ASTNode* node);
void print_siblings(ASTNode* head);
void print_ast_tree(ASTNode* node, int depth, int is_last);

#endif