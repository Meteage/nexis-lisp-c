#include <stdio.h>
#include "token.h"
#include "lexer.h"
#include "parser.h"
#include "evaluator.h"

int main(const int argc,const char *argv){
	printf("Hello, nexis!\n");
	test_token_stream();
	test_tokenize();
	test_parser();
	test_eval();
    
    // 释放环境
    if (global_env != NULL) {
        free_hash_table(global_env);
        global_env = NULL;
    }
	return 0;
}