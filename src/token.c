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
Token * append_token(Token*head,TokenType type,char* data){
	Token * tail = (Token *)malloc(sizeof(Token));
	if(tail == NULL) return NULL;
	tail->type = type;
	tail->data = data;
	tail->next = NULL;
	head->next = tail;
	return tail;
}

TokenType peek_token(const Token* head){
	if(head->next == NULL) return -1;
	return head->next->type;
}

//消耗token
int consume_token(Token *head,TokenType type){
	if(head->next->type != type) return 0;
	Token* p = head->next;
	head->next = p->next;
	free(p);
	return 1;
}

// 释放整个token流
void free_token_stream(Token* head) {
    if (head == NULL) return;
    
    Token* current = head;
    while (current != NULL) {
        Token* next = current->next;
        if (current->data != NULL) {
            free(current->data);
        }
        free(current);
        current = next;
    }
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
	t = append_token(t,TOKEN_BRACKET,"[");
	t = append_token(t,TOKEN_SYMBOL,"+");
	t = append_token(t,TOKEN_NUMBER,"1");
	t = append_token(t,TOKEN_NUMBER,"2");
	t = append_token(t,TOKEN_NUMBER,"3");
	t = append_token(t,TOKEN_RBACKET,"]");
	print_token_stream(h);
	consume_token(h,TOKEN_BRACKET);
	print_token_stream(h);
	printf("test_token_stream passed!\n");
	
}