#include <stdlib.h>
#include <stdio.h>
#include "token.h"

//创建TokenSteam
Token * init_token_stream(){
	Token * head = (Token *)malloc(sizeof(Token));
	if(head == NULL) return NULL;
	head->type = TOKEN_WHITESPACE;
	head->data = NULL;
	head->next = NULL;
	return head;
}

//尾插法
Token * add_token(Token*head,TokenType type,char* data){
	Token * tail = (Token *)malloc(sizeof(Token));
	if(tail == NULL) return NULL;
	tail->type = type;
	tail->data = data;
	tail->next = NULL;
	head->next = tail;
	return tail;
}

//消耗token
int consume_token(Token *head,TokenType type){
	if(head->next->type != type) return 0;
	Token* p = head->next;
	head->next = p->next;
	free(p);
	return 1;
}

//debug 打印TokenSteam
void print_token_stream(const Token* head){
	Token *p = head->next;
	while(p!=NULL){
		printf("TokenType: %2d Token: %s \n",p->type,p->data);
		p = p->next;
	}
}
//debug 测试TokenSteam
void test_token_stream(){
	Token * h = init_token_stream();
	Token *t = h;
	t = add_token(t,TOKEN_BRACKET,"[");
	t = add_token(t,TOKEN_SYMBOL,"+");
	t = add_token(t,TOKEN_NUMBER,"1");
	t = add_token(t,TOKEN_NUMBER,"2");
	t = add_token(t,TOKEN_NUMBER,"3");
	t = add_token(t,TOKEN_RBACKET,"]");
	print_token_stream(h);
	consume_token(h,TOKEN_BRACKET);
	print_token_stream(h);
	printf("test_token_stream passed!\n");
	
}