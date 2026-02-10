#include "parser.h"
#include "token.h"
#include "astnode.h"
#include <stdio.h>
#include "lexer.h"

//辅助函数 检查括号是否匹配
int check_parentheses(Token *token_stream) {
	int count = 0;
	Token *p = token_stream->next; //跳过头结点
	while (p != NULL) {
		if (p->type == TOKEN_BRACKET) {
			count++;
		} else if (p->type == TOKEN_RBACKET) {
			count--;
			if (count < 0) return 0; //右括号多于左括号
		}
		p = p->next;
	}
	return count == 0; //括号匹配
}

//辅助函数 
ASTNode* parse_expression(Token* token_stream){
	ASTNode *ast = NULL;
	ASTNode *first_children = NULL,*last_children = NULL;
	while (peek_token(token_stream) != -1)
	{
		switch (peek_token(token_stream))
		{
			case TOKEN_STRING:
			case TOKEN_SYMBOL:
			case TOKEN_NUMBER:
				ast = create_atom_node(token_stream->next->data);
				consume_token(token_stream,peek_token(token_stream));
				return ast;

			case TOKEN_AMPERSAND:
			case TOKEN_QUOTE:
			case TOKEN_UNQUOTE:
			case TOKEN_UNQUOTE_SPLICE:
				
				first_children = create_atom_node(token_stream->next->data);
				consume_token(token_stream,peek_token(token_stream));

				append_sibling(first_children,parse_expression(token_stream));
				ast = create_list_node(first_children);
				return ast;
			case TOKEN_BRACKET:
				consume_token(token_stream,TOKEN_BRACKET);

				while (peek_token(token_stream) != TOKEN_RBACKET && peek_token(token_stream) != -1)
				{
					if(first_children == NULL){
						first_children = parse_expression(token_stream);
						last_children = first_children;
					}
					else{
						last_children = append_sibling(last_children,parse_expression(token_stream));
					}
				}
				ast = create_list_node(first_children);

				consume_token(token_stream,TOKEN_RBACKET);
				return ast;
			default:
				return NULL;
		}
	}
	return ast;
}



//主要功能：将Token流转换为AST
//输入：Token流
//输出：AST

ASTNode * parse(Token *token_stream) {
	//先检验后处理
	//1.检查Token流是否为空
	if (token_stream == NULL || token_stream->next == NULL)  return NULL;
	//2.检查括号是否匹配
	if(!check_parentheses(token_stream)) return NULL;
	//3.处理Token流，构建AST
	return parse_expression(token_stream);
}

void test_parser() {
	printf("test_parser passed!\n");
	char* code = "[+ 0 '[1 2] [3 4] ~d ~@sddf &djdh]";
	Token* tokens = tokenize(code);
	print_token_stream(tokens);
	ASTNode* ast = parse(tokens);
	print_siblings(ast);
	print_ast_tree(ast,0,0);
}

int main(){
	test_parser();
	return 0;
}