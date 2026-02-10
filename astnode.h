#ifndef _ASTNODE_H_
#define _ASTNODE_H_

typedef struct ASTNode
{
	enum {ATOM,LIST} type;
	union 
	{
		char * atom;
		struct ASTNode ** list;
	};
} ASTNode;


ASTNode* create_atom_node(const char* value);
ASTNode* create_list_node(ASTNode** children, int count);
void free_ast(ASTNode* node);
void print_ast_tree(ASTNode* node, int depth, int is_last);

#endif