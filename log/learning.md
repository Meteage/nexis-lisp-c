# 哈希表与CMake实战总结

## 一、哈希表核心代码

### 1. 哈希表数据结构
```c
// 函数指针类型
typedef ASTNode* (*LispFunc)(ASTNode* args);

// 哈希表节点（链地址法）
typedef struct HashNode {
    char* key;              // 函数名如 "+"
    LispFunc value;         // 函数指针 op_add
    struct HashNode* next;  // 冲突链表
} HashNode;

// 哈希表
typedef struct {
    HashNode** buckets;     // 桶数组
    int size;               // 桶数量
    int count;              // 键值对数量
} HashTable;
```

### 2. 哈希函数（BKDR算法）
```c
unsigned int hash_string(const char* str, int table_size) {
    unsigned int seed = 131;  // 质数
    unsigned int hash = 0;
    while (*str) {
        hash = hash * seed + (*str++);
    }
    return hash % table_size;
}
```

### 3. 核心操作
```c
// 插入
void hash_table_put(HashTable* table, const char* key, LispFunc value) {
    int index = hash_string(key, table->size);
    
    // 头插法
    HashNode* node = malloc(sizeof(HashNode));
    node->key = strdup(key);
    node->value = value;
    node->next = table->buckets[index];
    table->buckets[index] = node;
    table->count++;
}

// 查找
LispFunc hash_table_get(HashTable* table, const char* key) {
    int index = hash_string(key, table->size);
    for (HashNode* p = table->buckets[index]; p; p = p->next) {
        if (strcmp(p->key, key) == 0) return p->value;
    }
    return NULL;
}
```

### 4. 实际应用：函数注册与调用
```c
// 注册内置函数
void register_builtins(HashTable* env) {
    hash_table_put(env, "+", op_add);
    hash_table_put(env, "-", op_sub);
    hash_table_put(env, "*", op_mul);
    hash_table_put(env, "quote", op_quote);
}

// eval 中使用
ASTNode* Eval(ASTNode* ast, HashTable* env) {
    if (ast->type == LIST) {
        char* op = ast->list->atom;  // 操作符
        LispFunc func = hash_table_get(env, op);
        if (func) return func(ast->list->next);  // 调用
    }
    return ast;
}
```

---

## 二、CMake 最小配置

### 1. 基础 CMakeLists.txt
```cmake
cmake_minimum_required(VERSION 3.10)
project(lisp C)

set(CMAKE_C_STANDARD 99)

# 头文件目录
include_directories(inc)

# 源文件
add_executable(lisp
    src/evaluator.c
    src/builtins.c
    src/hashtable.c
)

# 编译选项
target_compile_options(lisp PRIVATE -Wall -g)
```

### 2. 常用命令速查
```bash
# 配置（MinGW）
cmake -B build -G "MinGW Makefiles"

# 编译
cmake --build build

# 运行
./build/lisp.exe

# Debug模式
cmake -B build -DCMAKE_BUILD_TYPE=Debug

# 清理
rm -rf build
```

### 3. 实用变量
```cmake
${CMAKE_SOURCE_DIR}      # 源码根目录
${CMAKE_BINARY_DIR}      # 构建目录
${CMAKE_CURRENT_SOURCE_DIR} # 当前CMakeLists.txt目录
${PROJECT_NAME}          # 项目名
${CMAKE_C_COMPILER}      # C编译器
```

---

## 三、项目结构
```
.
├── inc/
│   ├── hashtable.h
│   ├── evaluator.h
│   └── builtins.h
├── src/
│   ├── hashtable.c
│   ├── evaluator.c
│   ├── builtins.c
│   └── main.c
├── CMakeLists.txt
└── build/
```

---

## 四、踩坑记录

### 1. 链接错误
```
undefined reference to `op_add'
```
**解决**：确保函数在 `.c` 中实现，头文件正确声明

### 2. 头文件找不到
```
fatal error: hashtable.h: No such file
```
**解决**：`include_directories(inc)`

### 3. 哈希冲突
**解决**：链地址法，每个桶维护链表

### 4. CMake生成器错误
```
CMake Error: Generator 'MinGW Makefiles' not found
```
**解决**：安装MinGW并添加到PATH

---

## 五、核心收益

### 哈希表
- ✅ 从 `if-else` 硬编码改为动态查找
- ✅ O(1) 时间复杂度查找函数
- ✅ 新增函数只需注册，不改eval逻辑

### CMake
- ✅ 一套配置，Windows/Linux通用
- ✅ 自动处理依赖关系
- ✅ 源码与构建产物分离

### 代码量对比
```
旧方式：30+行 if-else 判断
新方式：3行哈希表查找 + 注册函数
```

---

## 六、最小测试用例

```c
// 测试哈希表
HashTable* env = create_hash_table(101);
register_builtins(env);

LispFunc f = hash_table_get(env, "+");
assert(f == op_add);  // 查找成功

// 测试CMake编译
// build/ 下执行：
// cmake .. -G "MinGW Makefiles" && cmake --build .
// ./bin/lisp_interpreter.exe
```

---

## 一句话总结

**哈希表**：用空间换时间，O(1)查找函数指针  
**CMake**：自动化构建，告别手写gcc命令