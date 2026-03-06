#include "evaluator.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "parser.h"
#include "lexer.h"

//如果是原子
	//判断是否是符号
		//不是符号
			//返回本身
		//是符号
			//执行相应的逻辑
//如果是列表
	//嵌套调用自己
ASTNode* Eval(ASTNode * ast){
	if(ast->type == ATOM){
		//判断是否是符号
		//整个哈希表来查找
		printf("Evaluating atom: %s\n", ast->atom);
		//简单符号处理
		if(strcmp(ast->atom,"nil") == 0){
			return ast; //返回nil本身
		}
		if(strcmp(ast->atom,"true") == 0){
			return ast; //返回true本身
		}
		if(strcmp(ast->atom,"false") == 0){
			return ast; //返回false本身
		}
		//数字直接返回本身
		if(atoi(ast->atom) != 0 || strcmp(ast->atom,"0") == 0){
			return ast; //返回数字本身
		}


		if(strcmp(ast->atom,"'") == 0 || strcmp(ast->atom,"q") == 0 ){
			//quote 直接返回后面的内容
			return ast->next;
		}
		//加减乘除
		if(strcmp(ast->atom,"+") == 0){
			//+符号处理逻辑
			ASTNode * p = ast->next;

			while (p != NULL && p->next != NULL)
			{
				int a = atoi(Eval(p)->atom);
				int b = atoi(Eval(p->next)->atom);
				//将结果转换为字符串
				char str[20] = {0};
				sprintf(str, "%d", a+b);
				//创建一个新的AST节点来存储结果
				ASTNode* node = create_atom_node(str);

				//连接+ 1 2 3-> 3 3
				ast->next = node;
				node->next = p->next->next;

				//释放之前的AST节点
				free(p->next->next);
				free(p->next);

				p = node;
			}
			
			return p;
		}
		else if(strcmp(ast->atom,"-") == 0){
		}
		else if(strcmp(ast->atom,"*") == 0){
		}
		else if(strcmp(ast->atom,"/") == 0){
		}
	}
	else if(ast->type == LIST){
		//嵌套调用自己
		printf("Evaluating list...\n");
		return Eval(ast->list);
	}
}

//调试
void test_eval(){
	printf("test_eval passed!\n");
	Token *token_stream = tokenize("ture [+ 1 2 3] ['[1 2 3 4 5] \"Hello World!\" 'djjh]");
	print_token_stream(token_stream);
	ASTNode* ast = parse(token_stream);
	print_ast_tree(ast,0,1);
	ASTNode* result = Eval(ast);
	printf("Evaluation result:\n");
	print_ast_tree(result,0,1);
}

int main(){
	test_eval();
	return 0;
}