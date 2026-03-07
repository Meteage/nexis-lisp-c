#ifndef LEXER_H_
#define LEXER_H_

#include "token.h"

#define MAX_STRING_LENGTH 64
#define MAX_NUMBER_LENGTH 64
void test_tokenize();
Token * tokenize(const char *input);
#endif // LEXER_H_