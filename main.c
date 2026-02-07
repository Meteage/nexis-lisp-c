#include <stdio.h>
#include "token.h"
#include "lexer.h"

int main(const int argc,const char *argv){
	printf("Hello, nexis!\n");

	test_tokenStream();
	printf("test_tokenStream passed!\n");
	test_tokenize();
	printf("test_tokenize passed!\n");
	return 0;
}