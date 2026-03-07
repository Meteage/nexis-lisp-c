#include "hashtable.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 创建一个新的哈希表
HashTable* create_hash_table(int size) {
    HashTable* table = (HashTable*)malloc(sizeof(HashTable));
    table->size = size;
    table->count = 0;
    
    // 分配桶数组
    table->buckets = (HashNode**)calloc(size, sizeof(HashNode*));
    
    return table;
}

// 哈希函数：BKDR哈希算法（字符串哈希的经典算法）
unsigned int hash_string(const char* str, int table_size) {
    unsigned int seed = 131;  // 31, 131, 1313, 13131, 131313 等质数
    unsigned int hash = 0;
    
    while (*str) {
        hash = hash * seed + (*str++);
    }
    
    return hash % table_size;
}

// 插入键值对
void hash_table_put(HashTable* table, const char* key, LispFunc value) {
    if (table == NULL || key == NULL) return;
    
    // 计算哈希值
    unsigned int index = hash_string(key, table->size);
    
    // 检查key是否已存在
    HashNode* current = table->buckets[index];
    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            // 更新现有节点的值
            current->value = value;
            return;
        }
        current = current->next;
    }
    
    // 创建新节点
    HashNode* new_node = (HashNode*)malloc(sizeof(HashNode));
    new_node->key = (char*)malloc(strlen(key) + 1);
    strcpy(new_node->key, key);
    new_node->value = value;
    
    // 插入到链表头部
    new_node->next = table->buckets[index];
    table->buckets[index] = new_node;
    table->count++;
}

// 查找
LispFunc hash_table_get(HashTable* table, const char* key) {
    if (table == NULL || key == NULL) return NULL;
    
    unsigned int index = hash_string(key, table->size);
    
    HashNode* current = table->buckets[index];
    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            return current->value;
        }
        current = current->next;
    }
    
    return NULL;  // 没找到
}

// 删除
void hash_table_remove(HashTable* table, const char* key) {
    if (table == NULL || key == NULL) return;
    
    unsigned int index = hash_string(key, table->size);
    
    HashNode* current = table->buckets[index];
    HashNode* prev = NULL;
    
    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            if (prev == NULL) {
                // 删除头节点
                table->buckets[index] = current->next;
            } else {
                // 删除中间节点
                prev->next = current->next;
            }
            
            // 释放内存
            free(current->key);
            free(current);
            table->count--;
            return;
        }
        prev = current;
        current = current->next;
    }
}

// 释放哈希表
void free_hash_table(HashTable* table) {
    if (table == NULL) return;
    
    // 释放所有桶中的链表
    for (int i = 0; i < table->size; i++) {
        HashNode* current = table->buckets[i];
        while (current != NULL) {
            HashNode* next = current->next;
            free(current->key);
            free(current);
            current = next;
        }
    }
    
    // 释放桶数组
    free(table->buckets);
    
    // 释放表结构
    free(table);
}

// 打印哈希表（调试用）
void print_hash_table(HashTable* table) {
    if (table == NULL) {
        printf("HashTable is NULL\n");
        return;
    }
    
    printf("HashTable (size=%d, count=%d):\n", table->size, table->count);
    
    for (int i = 0; i < table->size; i++) {
        if (table->buckets[i] != NULL) {
            printf("  bucket[%d]: ", i);
            HashNode* current = table->buckets[i];
            while (current != NULL) {
                printf("-> [%s] ", current->key);
                current = current->next;
            }
            printf("\n");
        }
    }
}
