#ifndef _PARSER_H_
#define _PARSER_H_
#include "token.h"
#include "astnode.h"

ASTNode * parse(Token *token_stream);
void test_parser();
#endif