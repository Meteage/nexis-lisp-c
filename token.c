#include <stdlib.h>
#include <stdio.h>
#include "token.h"

//创建TokenSteam
Token * initTokenSteam(){
	Token * head = (Token *)malloc(sizeof(Token));
	head->type = TOKEN_WHITESPACE;
	head->data = NULL;
	head->next = NULL;
	return head;
}

//尾插法
Token * add_Token(Token*List,TokenType type,char *data){
	Token * tail = (Token *)malloc(sizeof(Token));
	tail->type = type;
	tail->data = data;
	tail->next = NULL;
	List->next = tail;
	return tail;
}

//消耗token
int consume_token(Token *List,TokenType type){
	if(List->next->type != type) return 0;
	Token* p = List->next;
	List->next = p->next;
	free(p);
	return 1;
}

//debug 打印TokenSteam
void printTokenSteam(Token * Head){
	Token *p = Head->next;
	while(p!=NULL){
		printf("TokenType: %2d Token: %s \n",p->type,p->data);
		p = p->next;
	}
}
//debug 测试TokenSteam
void test_tokenStream(){
	Token * h = initTokenSteam();
	Token *t = h;
	t = add_Token(t,TOKEN_BRACKET,"[");
	t = add_Token(t,TOKEN_SYMBOL,"+");
	t = add_Token(t,TOKEN_NUMBER,"1");
	t = add_Token(t,TOKEN_NUMBER,"2");
	t = add_Token(t,TOKEN_NUMBER,"3");
	t = add_Token(t,TOKEN_RBACKET,"]");
	printTokenSteam(h);
	consume_token(h,TOKEN_BRACKET);
	printTokenSteam(h);
	printf("test_tokenStream passed!\n");
	
}