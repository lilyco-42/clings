// 35d_infix_to_postfix.c — 中缀表达式转后缀（Shunting Yard 算法）
//
// 任务: 使用栈实现 Shunting Yard 算法
//       将中缀表达式转换为后缀表达式（逆波兰表示法）
//
// 知识点: Shunting Yard 算法、运算符优先级、栈应用
//
// 用法: ./35d_infix_to_postfix "1+2"       → 12+
//       ./35d_infix_to_postfix "2+3*4"     → 234*+
//       ./35d_infix_to_postfix "(1+2)*3"   → 12+3*
//
// 规则:
//   - 操作数（单位数字）直接输出
//   - '(' 入栈
//   - ')' 弹出栈直到遇到 '('
//   - 运算符: 弹出栈顶优先级 >= 当前运算符的，再入栈
//   - 最后弹出栈中剩余运算符

#include <stdio.h>
#include <ctype.h>

#define MAX_SIZE 256

char stack[MAX_SIZE];
int top = -1;

void push(char c) { stack[++top] = c; }
char pop(void) { return stack[top--]; }
char peek(void) { return stack[top]; }
int is_empty(void) { return top == -1; }

// TODO: 实现 priority 函数
// 功能: 返回运算符的优先级
//       '+','-' → 1;  '*','/' → 2;  其他 → 0
// int priority(char op) { ... }

// TODO: 实现 infix_to_postfix 函数
// 功能: 将中缀表达式 expr 转换为后缀表达式存入 output
//   遍历 expr 的每个字符:
//   - 数字: 直接存入 output
//   - '(': 入栈
//   - ')': 弹出栈直到遇到 '('，弹出 '(' 但不输出
//   - 运算符: 弹出栈顶优先级 >= 当前的运算符存入 output，再将当前运算符入栈
//   遍历结束后弹出栈中所有剩余运算符存入 output
//   在 output 末尾加 '\0'
// void infix_to_postfix(const char *expr, char *output) { ... }

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("usage: %s <infix-expression>\n", argv[0]);
        return 1;
    }

    // TODO: 调用 infix_to_postfix 转换并输出结果
    // char output[MAX_SIZE];
    // infix_to_postfix(argv[1], output);
    // printf("%s\n", output);

    return 0;
}
