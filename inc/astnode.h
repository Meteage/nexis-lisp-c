#ifndef _ASTNODE_H_
#define _ASTNODE_H_

// 前向声明 Env
typedef struct Env Env;

// 节点类型
typedef enum {
    NODE_ATOM,      // 原子
    NODE_LIST,      // 列表
    NODE_FUNCTION   // 函数
} NodeType;

// 原子类型
typedef enum {
    ATOM_NUMBER,
    ATOM_STRING,
    ATOM_SYMBOL
} AtomType;

// 函数结构
typedef struct Function {
    struct ASTNode* params;  // 参数列表
    struct ASTNode* body;    // 函数体
    Env* env;                // 闭包环境
} Function;

typedef struct ASTNode
{
    NodeType type;           // 节点类型
    struct ASTNode* next;    // 同级节点
    
    union {
        // 原子类型的数据
        struct {
            char* value;
            AtomType atom_type;
        } atom;
        
        // 列表类型的数据
        struct ASTNode* list;
        
        // 函数类型的数据
        Function func;
    };
} ASTNode;

// 函数声明
ASTNode* create_atom_node(const char* value);
ASTNode* create_list_node(ASTNode* children);
ASTNode* create_function_node(ASTNode* params, ASTNode* body, Env* env);
ASTNode* copy_ast_node(ASTNode* node);

ASTNode* append_sibling(ASTNode* tail, ASTNode* node);
ASTNode* append_sibling_to_list(ASTNode* head, ASTNode* node);
void set_atom_type(ASTNode* node, AtomType type);
void free_ast(ASTNode* node);
void print_siblings(ASTNode* head);
void print_ast_tree(ASTNode* node, int depth, int is_last);

#endif