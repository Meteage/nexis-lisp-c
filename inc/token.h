#ifndef _TOKEN_H_
#define _TOKEN_H_

typedef enum {
	TOKEN_WHITESPACE,//空白
	TOKEN_COMMENT,//注释
	TOKEN_BRACKET,//[左括号
	TOKEN_RBACKET,//]右括号
	TOKEN_QUOTE,//'引号
	TOKEN_UNQUOTE,//~反引号
	TOKEN_UNQUOTE_SPLICE,//~@反引号拼接
	TOKEN_AMPERSAND,//&符号
	TOKEN_NUMBER,//数字
	TOKEN_STRING,//字符串
	TOKEN_SYMBOL,//符号
} TokenType; 

typedef struct token { 
	TokenType type;
	char* data;
	struct token *next;
} Token;

// 创建和管理
Token* init_token_stream(void);                      
Token* append_token(Token* head, TokenType type, char* data);  

TokenType peek_token(const Token* head);

// 消费/处理
int consume_token(Token* head, TokenType type);       

void free_token_stream(Token* head);

// 输出/调试
void print_token_stream(const Token* head);          

// 测试
void test_token_stream(void);                         

#endif
