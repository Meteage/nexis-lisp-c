#include "evaluator.h"
#include "lexer.h"
#include "parser.h"
#include "builtins.h"
#include "token.h"
#include <stdio.h>
#include <stdlib.h>



// 调试函数 - 全英文版本
void test_eval() {
    printf("========== TESTING EVALUATOR ==========\n");
    
    // 初始化环境
    init_global_env();
    printf("Global environment initialized with %d built-in functions\n", global_env->functions->count);
    
    // ===== TEST 1: Basic Arithmetic =====
    printf("\n----- Test 1: Basic Arithmetic -----\n");
    Token *t1 = tokenize("[+ 1 2 3]");
    printf("Input  : [+ 1 2 3]\n");
    ASTNode* a1 = parse(t1);
    printf("AST    :\n");
    print_ast_tree(a1, 0, 1);
    ASTNode* r1 = Eval(a1, global_env);
    printf("Result : ");
    print_ast_tree(r1, 0, 1);
    printf("Expected: 6\n");
    
    // ===== TEST 2: Multiple Arithmetic Operations =====
    printf("\n----- Test 2: Multiple Operations -----\n");
    Token *t2 = tokenize("[* [+ 2 3] [- 10 4]]");
    printf("Input  : [* [+ 2 3] [- 10 4]]\n");
    ASTNode* a2 = parse(t2);
    printf("AST    :\n");
    print_ast_tree(a2, 0, 1);
    ASTNode* r2 = Eval(a2, global_env);
    printf("Result : ");
    print_ast_tree(r2, 0, 1);
    printf("Expected: 30  (5 * 6)\n");
    
    // ===== TEST 3: Quote Special Form =====
    printf("\n----- Test 3: Quote -----\n");
    Token *t3 = tokenize("[' [1 2 3]]");
    printf("Input  : [' [1 2 3]]\n");
    ASTNode* a3 = parse(t3);
    printf("AST    :\n");
    print_ast_tree(a3, 0, 1);
    ASTNode* r3 = Eval(a3, global_env);
    printf("Result : ");
    print_ast_tree(r3, 0, 1);
    printf("Expected: [1 2 3] (unevaluated)\n");
    
    // ===== TEST 4: Quote Without Evaluation =====
    printf("\n----- Test 4: Quote Prevents Evaluation -----\n");
    Token *t4 = tokenize("'[+ 1 2 3]");
    printf("Input  : '[+ 1 2 3]\n");
    ASTNode* a4 = parse(t4);
    printf("AST    :\n");
    print_ast_tree(a4, 0, 1);
    ASTNode* r4 = Eval(a4, global_env);
    printf("Result : ");
    print_ast_tree(r4, 0, 1);
    printf("Expected: [+ 1 2 3] (not 6)\n");
    
    // ===== TEST 5: Car Operation =====
    printf("\n----- Test 5: Car (first element) -----\n");
    Token *t5 = tokenize("[car '[10 20 30]]");
    printf("Input  : [car '[10 20 30]]\n");
    ASTNode* a5 = parse(t5);
    printf("AST    :\n");
    print_ast_tree(a5, 0, 1);
    ASTNode* r5 = Eval(a5, global_env);
    printf("Result : ");
    print_ast_tree(r5, 0, 1);
    printf("Expected: 10\n");
    
    // ===== TEST 6: Cdr Operation =====
    printf("\n----- Test 6: Cdr (rest elements) -----\n");
    Token *t6 = tokenize("[cdr '[10 20 30]]");
    printf("Input  : [cdr '[10 20 30]]\n");
    ASTNode* a6 = parse(t6);
    printf("AST    :\n");
    print_ast_tree(a6, 0, 1);
    ASTNode* r6 = Eval(a6, global_env);
    printf("Result : ");
    print_ast_tree(r6, 0, 1);
    printf("Expected: [20 30]\n");
    
    // ===== TEST 7: Cons Operation =====
    printf("\n----- Test 7: Cons (construct list) -----\n");
    Token *t7 = tokenize("[cons 1 '[2 3 4]]");
    printf("Input  : [cons 1 '[2 3 4]]\n");
    ASTNode* a7 = parse(t7);
    printf("AST    :\n");
    print_ast_tree(a7, 0, 1);
    ASTNode* r7 = Eval(a7, global_env);
    printf("Result : ");
    print_ast_tree(r7, 0, 1);
    printf("Expected: [1 2 3 4]\n");
    
    // ===== TEST 8: Nested Cons =====
    printf("\n----- Test 8: Nested Cons -----\n");
    Token *t8 = tokenize("[cons [car '[1 2]] [cdr '[3 4]]]");
    printf("Input  : [cons [car '[1 2]] [cdr '[3 4]]]\n");
    ASTNode* a8 = parse(t8);
    printf("AST    :\n");
    print_ast_tree(a8, 0, 1);
    ASTNode* r8 = Eval(a8, global_env);
    printf("Result : ");
    print_ast_tree(r8, 0, 1);
    printf("Expected: [1 [4]]\n");
    
    // ===== TEST 9: List Length =====
    printf("\n----- Test 9: List Length -----\n");
    Token *t9 = tokenize("[length '[5 6 7 8 9]]");
    printf("Input  : [length '[5 6 7 8 9]]\n");
    ASTNode* a9 = parse(t9);
    printf("AST    :\n");
    print_ast_tree(a9, 0, 1);
    ASTNode* r9 = Eval(a9, global_env);
    printf("Result : ");
    print_ast_tree(r9, 0, 1);
    printf("Expected: 5\n");
    
    // ===== TEST 10: Null Check =====
    printf("\n----- Test 10: Null? Check -----\n");
    Token *t10a = tokenize("[null? '[]]");
    printf("Input 1: [null? '[]]\n");
    ASTNode* a10a = parse(t10a);
    ASTNode* r10a = Eval(a10a, global_env);
    printf("Result 1: ");
    print_ast_tree(r10a, 0, 1);
    printf("Expected: true\n");
    
    Token *t10b = tokenize("[null? '[1 2]]");
    printf("Input 2: [null? '[1 2]]\n");
    ASTNode* a10b = parse(t10b);
    ASTNode* r10b = Eval(a10b, global_env);
    printf("Result 2: ");
    print_ast_tree(r10b, 0, 1);
    printf("Expected: false\n");
    
    // ===== TEST 11: List Creation =====
    printf("\n----- Test 11: List Creation -----\n");
    Token *t11 = tokenize("[list 1 2 3]");
    printf("Input  : [list 1 2 3]\n");
    ASTNode* a11 = parse(t11);
    printf("AST    :\n");
    print_ast_tree(a11, 0, 1);
    ASTNode* r11 = Eval(a11, global_env);
    printf("Result : ");
    print_ast_tree(r11, 0, 1);
    printf("Expected: [1 2 3]\n");
    
    // ===== TEST 12: String Operations =====
    printf("\n----- Test 12: String Operations -----\n");
    Token *t12a = tokenize("[str-len \"hello\"]");
    printf("Input 1: [str-len \"hello\"]\n");
    ASTNode* a12a = parse(t12a);
    printf("AST    :\n");
    print_ast_tree(a12a, 0, 1);
    ASTNode* r12a = Eval(a12a, global_env);
    printf("Result 1: ");
    print_ast_tree(r12a, 0, 1);
    printf("Expected: 5\n");
    
    Token *t12b = tokenize("[str-cat \"hello \" \"world\"]");
    printf("Input 2: [str-cat \"hello \" \"world\"]\n");
    ASTNode* a12b = parse(t12b);
    ASTNode* r12b = Eval(a12b, global_env);
    printf("Result 2: ");
    print_ast_tree(r12b, 0, 1);
    printf("Expected: \"hello world\"\n");
    
    // ===== TEST 13: Comparison Operations =====
    printf("\n----- Test 13: Comparisons -----\n");
    
    Token *t13a = tokenize("[eq? 5 5]");
    printf("Input 1: [eq? 5 5]\n");
    ASTNode* a13a = parse(t13a);
    ASTNode* r13a = Eval(a13a, global_env);
    printf("Result 1: ");
    print_ast_tree(r13a, 0, 1);
    printf("Expected: true\n");
    
    Token *t13b = tokenize("[eq? 5 6]");
    printf("Input 2: [eq? 5 6]\n");
    ASTNode* a13b = parse(t13b);
    ASTNode* r13b = Eval(a13b, global_env);
    printf("Result 2: ");
    print_ast_tree(r13b, 0, 1);
    printf("Expected: false\n");
    
    Token *t13c = tokenize("[< 3 5]");
    printf("Input 3: [< 3 5]\n");
    ASTNode* a13c = parse(t13c);
    ASTNode* r13c = Eval(a13c, global_env);
    printf("Result 3: ");
    print_ast_tree(r13c, 0, 1);
    printf("Expected: true\n");
    
    Token *t13d = tokenize("[> 10 5]");
    printf("Input 4: [> 10 5]\n");
    ASTNode* a13d = parse(t13d);
    ASTNode* r13d = Eval(a13d, global_env);
    printf("Result 4: ");
    print_ast_tree(r13d, 0, 1);
    printf("Expected: true\n");
    
    // ===== TEST 14: Complex Expressions =====
    printf("\n----- Test 14: Complex List Operations -----\n");
    Token *t14 = tokenize("[car [cdr [cons 1 '[2 3 4]]]]");
    printf("Input  : [car [cdr [cons 1 '[2 3 4]]]]\n");
    ASTNode* a14 = parse(t14);
    printf("AST    :\n");
    print_ast_tree(a14, 0, 1);
    ASTNode* r14 = Eval(a14, global_env);
    printf("Result : ");
    print_ast_tree(r14, 0, 1);
    printf("Expected: 2\n");
    
    // ===== TEST 15: Mixed Types =====
    printf("\n----- Test 15: Mixed Types -----\n");
    Token *t15 = tokenize("[list 1 \"two\" 3]");
    printf("Input  : [list 1 \"two\" 3]\n");
    ASTNode* a15 = parse(t15);
    printf("AST    :\n");
    print_ast_tree(a15, 0, 1);
    ASTNode* r15 = Eval(a15, global_env);
    printf("Result : ");
    print_ast_tree(r15, 0, 1);
    printf("Expected: [1 \"two\" 3]\n");

    // ===== TEST 16: If Condition =====
    printf("\n----- Test 16: If Condition -----\n");

    Token *t16a = tokenize("[if true \"yes\" \"no\"]");
    printf("Input 1: [if true \"yes\" \"no\"]\n");
    ASTNode* a16a = parse(t16a);
    ASTNode* r16a = Eval(a16a, global_env);
    printf("Result 1: ");
    print_ast_tree(r16a, 0, 1);
    printf("Expected: \"yes\"\n");

    Token *t16b = tokenize("[if false \"yes\" \"no\"]");
    printf("Input 2: [if false \"yes\" \"no\"]\n");
    ASTNode* a16b = parse(t16b);
    ASTNode* r16b = Eval(a16b, global_env);
    printf("Result 2: ");
    print_ast_tree(r16b, 0, 1);
    printf("Expected: \"no\"\n");
    
    printf("\n========== TESTING COMPLETE ==========\n");
    printf("Total tests run: 16\n");
}
