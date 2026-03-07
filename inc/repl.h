// repl.h
#ifndef REPL_H
#define REPL_H

// REPL 主循环
void repl_start();

// 运行文件
int repl_run_file(const char* filename);

// 执行单行表达式
void repl_eval_line(const char* input);

#endif