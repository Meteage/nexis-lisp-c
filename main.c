#include <stdio.h>
#include "token.h"
#include "lexer.h"

int main(const int argc,const char *argv){
	printf("Hello, nexis!\n");
	test_tokenStream();
	test_tokenize();
	return 0;
}