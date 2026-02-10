#include "parser.h"
#include "token.h"
#include "astnode.h"
#include <stdio.h>


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

//主要功能：将Token流转换为AST
//输入：Token流
//输出：AST

ASTNode * parse(Token *token_stream) {
	//先检验后处理
	//1.检查Token流是否为空
	if (token_stream == NULL || token_stream->next == NULL)  return NULL;
	//2.检查括号是否匹配
	if(!check_parentheses(token_stream)) return NULL;
	//3.递归构建AST
	ASTNode *ast = NULL;
	Token *p = token_stream->next;//跳过头结点
	while(p != NULL){
		switch (p->type)
		{
			case TOKEN_BRACKET:
				consume_token(p,TOKEN_BRACKET);
				ast = create_list_node(parse(p));
				consume_token(p,TOKEN_RBACKET);
				break;
		
			default:
				break;
		}
		p = p->next;
	}

	return ast;
}

void test_parser() {
	printf("test_parser passed!\n");
}