#include <stdio.h>
#include "token.h"
#include "lexer.h"
#include "parser.h"

int main(const int argc,const char *argv){
	printf("Hello, nexis!\n");
	test_token_stream();
	test_tokenize();
	test_parser();
	return 0;
}