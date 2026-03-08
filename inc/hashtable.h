// hashtable.h
#ifndef HASHTABLE_H
#define HASHTABLE_H

#include "astnode.h"

#define DEBUG_HASH 0  // 定义调试宏

// 前向声明 Env 结构
typedef struct Env Env;

// 函数指针类型 - 现在可以接受环境参数
typedef ASTNode* (*LispFunc)(ASTNode* args, Env* env);

// 哈希表节点
typedef struct HashNode {
    char* key;
    LispFunc value;         // 函数指针
    struct HashNode* next;
} HashNode;

// 哈希表
typedef struct {
    HashNode** buckets;
    int size;
    int count;
} HashTable;

// 函数声明
HashTable* create_hash_table(int size);
unsigned int hash_string(const char* str, int table_size);
void hash_table_put(HashTable* table, const char* key, LispFunc value);
LispFunc hash_table_get(HashTable* table, const char* key);
void hash_table_remove(HashTable* table, const char* key);
void free_hash_table(HashTable* table);
void print_hash_table(HashTable* table);

#endif