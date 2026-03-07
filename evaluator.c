#include "evaluator.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "parser.h"
#include "lexer.h"

/**
 * 判断一个字符串是否是数字
 * @param str 输入的字符串
 * @return 如果是数字返回1，否则返回0
 */
int is_number(const char * str){
    if(str == NULL || *str == '\0') return 0; //空字符串不是数字
    char *endstr;
    strtol(str, &endstr, 10); //尝试将字符串转换为长整数
    return *endstr == '\0'; //如果转换成功，endstr应该指向字符串的末尾
}

// 计算列表中的参数个数
int count_args(ASTNode* args) {
    int count = 0;
    ASTNode* current = args;
    while (current != NULL) {
        count++;
        current = current->next;
    }
    return count;
}

// 如果是原子
    // 判断是否是符号
        // 不是符号
            // 返回本身
        // 是符号
            // 执行相应的逻辑
// 如果是列表
    // 嵌套调用自己
ASTNode* eval(ASTNode * ast){
    if (ast == NULL) return NULL;
    
    if(ast->type == ATOM){
        // 判断是否是符号
        // 整个哈希表来查找
        printf("evaluating atom: %s\n", ast->atom);
        // 简单符号处理
        if(strcmp(ast->atom,"nil") == 0){
            return ast; //返回nil本身
        }
        if(strcmp(ast->atom,"true") == 0){
            return ast; //返回true本身
        }
        if(strcmp(ast->atom,"false") == 0){
            return ast; //返回false本身
        }
        // 数字直接返回本身
        if(is_number(ast->atom)){
            return ast; //返回数字本身
        }
        // 其他符号暂时不处理，直接返回本身
        return ast;
    }
    else if(ast->type == LIST){
        // 嵌套调用自己
        printf("evaluating list...\n");
        
        // 空列表返回nil
        if(ast->list == NULL) {
            return create_atom_node("nil");
        }
        
        // 获取第一个元素作为操作符
        ASTNode *first = ast->list;
        ASTNode *args = first->next; //剩余的元素作为参数

        // quote - 特殊形式，不对参数求值
        if(strcmp(first->atom,"'") == 0 || strcmp(first->atom,"quote") == 0){
            // quote 直接返回后面的内容（不求值）
            if (args == NULL) return create_atom_node("nil");
            return args;  // 返回第一个参数（不求值）
        }

        // 加减乘除 - 这些是函数，需要对参数求值
        if(strcmp(first->atom,"+") == 0){
            // +符号处理逻辑 [+ 1 2 3] => 6
            if (args == NULL) return create_atom_node("0");
            
            int sum = 0;
            ASTNode* current = args;
            
            // 遍历所有参数并求和
            while (current != NULL) {
                ASTNode* evaluated = eval(current);
                if (evaluated != NULL && evaluated->type == ATOM) {
                    sum += atoi(evaluated->atom);
                }
                current = current->next;
            }

            // 创建结果节点
            char buffer[20];
            sprintf(buffer, "%d", sum);
            return create_atom_node(buffer);
        }
        else if(strcmp(first->atom,"-") == 0){
            // -符号处理逻辑 [- 10 3 2] => 5
            if (args == NULL) return create_atom_node("0");
            
            // 先求第一个参数的值
            ASTNode* first_eval = eval(args);
            if (first_eval == NULL || first_eval->type != ATOM) {
                return create_atom_node("0");
            }
            
            int result = atoi(first_eval->atom);
            ASTNode* current = args->next;
            
            // 减去剩余的参数
            while (current != NULL) {
                ASTNode* evaluated = eval(current);
                if (evaluated != NULL && evaluated->type == ATOM) {
                    result -= atoi(evaluated->atom);
                }
                current = current->next;
            }
            
            char buffer[20];
            sprintf(buffer, "%d", result);
            return create_atom_node(buffer);
        }
        else if(strcmp(first->atom,"*") == 0){
            // *符号处理逻辑 [* 2 3 4] => 24
            if (args == NULL) return create_atom_node("1");
            
            int result = 1;
            ASTNode* current = args;
            
            while (current != NULL) {
                ASTNode* evaluated = eval(current);
                if (evaluated != NULL && evaluated->type == ATOM) {
                    result *= atoi(evaluated->atom);
                }
                current = current->next;
            }
            
            char buffer[20];
            sprintf(buffer, "%d", result);
            return create_atom_node(buffer);
        }
        else if(strcmp(first->atom,"/") == 0){
            // /符号处理逻辑 [/ 100 5 2] => 10
            if (args == NULL) return create_atom_node("0");
            
            // 先求第一个参数的值
            ASTNode* first_eval = eval(args);
            if (first_eval == NULL || first_eval->type != ATOM) {
                return create_atom_node("0");
            }
            
            int result = atoi(first_eval->atom);
            ASTNode* current = args->next;
            
            // 依次除以剩余的参数
            while (current != NULL) {
                ASTNode* evaluated = eval(current);
                if (evaluated != NULL && evaluated->type == ATOM) {
                    int divisor = atoi(evaluated->atom);
                    if (divisor != 0) {
                        result /= divisor;
                    }
                }
                current = current->next;
            }
            
            char buffer[20];
            sprintf(buffer, "%d", result);
            return create_atom_node(buffer);
        }
        
        // 如果不是特殊形式，默认返回第一个元素的求值结果
        return eval(first);
    }
    
    // 默认返回值（不应该执行到这里）
    return create_atom_node("nil");
}


// 调试
void test_eval(){
    printf("========== 开始测试 ==========\n");
    
    // 测试1: 算术运算
    printf("\n--- 测试1: 算术运算 ---\n");
    Token *token_stream1 = tokenize("[+ 1 2 3]");
    printf("输入: [+ 1 2 3]\n");
    ASTNode* ast1 = parse(token_stream1);
    printf("AST结构:\n");
    print_ast_tree(ast1, 0, 1);
    ASTNode* result1 = eval(ast1);
    printf("计算结果: ");
    print_ast_tree(result1, 0, 1);
    
    // 测试2: quote
    printf("\n--- 测试2: quote ---\n");
    Token *token_stream2 = tokenize("[' [1 2 3]]");
    printf("输入: [' [1 2 3]]\n");
    ASTNode* ast2 = parse(token_stream2);
    printf("AST结构:\n");
    print_ast_tree(ast2, 0, 1);
    ASTNode* result2 = eval(ast2);
    printf("计算结果: ");
    print_ast_tree(result2, 0, 1);
    
    // 测试3: 复杂表达式
    printf("\n--- 测试3: 复杂表达式 ---\n");
    Token *token_stream3 = tokenize("[+ 2 [* 2 3] [/ 8 2]]");
    printf("输入: [+ 2 [* 2 3] [/ 8 2]]\n");
    ASTNode* ast3 = parse(token_stream3);
    printf("AST结构:\n");
    print_ast_tree(ast3, 0, 1);
    ASTNode* result3 = eval(ast3);
    printf("计算结果: ");
    print_ast_tree(result3, 0, 1);
    
    printf("\n========== 测试完成 ==========\n");
}

int main(){
    test_eval();
    return 0;
}