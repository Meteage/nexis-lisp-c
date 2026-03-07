#ifndef HASHTABLE_H
#define HASHTABLE_H

#include "astnode.h"  // 包含ASTNode定义

// 函数指针类型 - 所有Lisp操作函数的统一签名
typedef ASTNode* (*LispFunc)(ASTNode* args);

// 哈希表节点（链地址法解决冲突）
typedef struct HashNode {
    char* key;              // 符号名
    LispFunc value;         // 对应的函数指针
    struct HashNode* next;  // 链表下一个节点
} HashNode;

// 哈希表结构
typedef struct {
    HashNode** buckets;     // 桶数组
    int size;               // 桶的数量
    int count;              // 当前存储的键值对数量
} HashTable;

// 创建哈希表
HashTable* create_hash_table(int size);

// 哈希函数（使用字符串哈希）
unsigned int hash_string(const char* str, int table_size);

// 插入键值对
void hash_table_put(HashTable* table, const char* key, LispFunc value);

// 查找
LispFunc hash_table_get(HashTable* table, const char* key);

// 删除
void hash_table_remove(HashTable* table, const char* key);

// 释放哈希表
void free_hash_table(HashTable* table);

// 打印哈希表（调试用）
void print_hash_table(HashTable* table);

#endif