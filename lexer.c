#include "lexer.h"
#include "stdlib.h"

Token * tokenize(const char *input){
	Token * head = initTokenSteam();
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
			tail = add_Token(tail,TOKEN_AMPERSAND,"\'");
			input++;
			break;
		case '\'':
			tail = add_Token(tail,TOKEN_QUOTE,"\'");
			input++;
			break;
		case '~':
			input++;
			if(*input == '@'){
				tail = add_Token(tail,TOKEN_UNQUOTE_SPLICE,"~@");
			}
			else{
				tail = add_Token(tail,TOKEN_UNQUOTE,"~");
			}
		break;
		case '[':
			tail = add_Token(tail,TOKEN_BRACKET,"[");
			input++;
			break;
		case ']':
			tail = add_Token(tail,TOKEN_RBACKET,"]");
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
			tail = add_Token(tail,TOKEN_NUMBER,num);
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
			tail = add_Token(tail,TOKEN_STRING,str);
		}
		break;
		
		default:
		{	
			//提取符号
			char * sym = (char*)malloc(sizeof(char)*MAX_STRING_LENGTH);
			int i = 0;
			while (!(*input == '\t'||*input == '\n'||*input == ' '))
			{
				sym[i] = *input;
				input++;
				i++;
			}
			sym[i] = '\0';
			tail = add_Token(tail,TOKEN_SYMBOL,sym);
		}
		break;
		}
	}
	return head;
}

//debug 测试tokenize函数
void test_tokenize(){
	const char *input = "[+ 1 2 3] ;这是一个注释\n[1 2 3] \"Hello, world!\" ~@foo &bar 'qux 1.23 0.45";
	Token * tokens = tokenize(input);
	printTokenSteam(tokens);
}