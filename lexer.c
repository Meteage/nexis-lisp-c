#include "lexer.h"
#include "stdlib.h"
#include "stdio.h"

Token * tokenize(const char *input){
	Token * head = init_token_stream();
	Token * tail = head;
	while (*input != '\0')
	{
		switch (*input)
		{
		//定义无效字符
		case '\n':
		case '\t':
		case ' ' :
			//跳过
			input++;
			break;
		case ';':
			//注释
			input++;
			while ((*input != '\n'))
			{
				input++;
			}
		break;
		case '&':
			tail = append_token(tail,TOKEN_AMPERSAND,"&");
			input++;
			break;
		case '\'':
			tail = append_token(tail,TOKEN_QUOTE,"\'");
			input++;
			break;
		case '~':
			input++;
			if(*input == '@'){
				tail = append_token(tail,TOKEN_UNQUOTE_SPLICE,"~@");
				//跳过@符号
				input++;
			}
			else{
				tail = append_token(tail,TOKEN_UNQUOTE,"~");
			}
		break;
		case '[':
			tail = append_token(tail,TOKEN_BRACKET,"[");
			input++;
			break;
		case ']':
			tail = append_token(tail,TOKEN_RBACKET,"]");
			input++;
			break;
		case '.':
		case '0'...'9':
		{	
			//提取数字
			char * num = (char*)malloc(sizeof(char)*MAX_NUMBER_LENGTH);
			int i = 0;
			while (( *input>='0'&&*input<='9' || *input == '.'))
			{
				num[i] = *input;
				input++;
				i++;
			}
			num[i] = '\0';
			tail = append_token(tail,TOKEN_NUMBER,num);
		}
		break;
		case '\"':
		{	
			//提取字符串
			char * str = (char*)malloc(sizeof(char)*MAX_STRING_LENGTH);
			int i = 0;
			input++;
			while (*input != '\"')
			{
				str[i] = *input;
				input++;
				i++;
			}
			str[i] = '\0';
			input++;
			tail = append_token(tail,TOKEN_STRING,str);
		}
		break;
		
		default:
		{	
			//提取符号
			char * sym = (char*)malloc(sizeof(char)*MAX_STRING_LENGTH);
			int i = 0;
			while (!(*input == '\t'||*input == '\n'||*input == ' '||*input=='['||*input==']'||*input=='\0'))
			{
				sym[i] = *input;
				input++;
				i++;
			}
			sym[i] = '\0';
			tail = append_token(tail,TOKEN_SYMBOL,sym);
		}
		break;
		}
	}
	return head;
}

//debug 测试tokenize函数
void test_tokenize(){
	const char *input = "[+ 0 '[1 2] [3 4] ~d ~@sddf &djdh]";
	Token * tokens = tokenize(input);
	print_token_stream(tokens);
	printf("test_tokenize passed!\n");

}