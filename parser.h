#ifndef _PARSER_H_
#define _PARSER_H_

#include "token.h"

typedef struct ASTNode
{
	enum {ATOM,LIST} type;
	union 
	{
		char * atom;
		struct ASTNode ** list;
	};
} ASTNode;

typedef ASTNode ** AST;

AST parser(Token * tokenSteam);

void test_parser();

#endif