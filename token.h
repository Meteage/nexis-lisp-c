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

Token * initTokenSteam();
Token * add_Token(Token*List,TokenType type,char *data);
int consume_token(Token *List,TokenType type);
void printTokenSteam(Token * Head);
void test_tokenStream();

#endif
