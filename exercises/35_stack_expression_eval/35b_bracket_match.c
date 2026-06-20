// 35b_bracket_match.c — 栈实现括号匹配
//
// 任务: 使用栈检查输入字符串的括号是否匹配
//       支持 ()、[]、{} 三种括号
//
// 知识点: 栈应用——括号匹配、字符比较
//
// 用法: ./35b_bracket_match "(())"    → yes
//       ./35b_bracket_match "(()"     → no
//       ./35b_bracket_match "()[]{}"  → yes
//
// 预期: 匹配输出 "yes"，不匹配输出 "no"

#include <stdio.h>
#include <string.h>

#define MAX_SIZE 256

char stack[MAX_SIZE];
int top = -1;

void push(char c) { stack[++top] = c; }
char pop(void) { return stack[top--]; }
int is_empty(void) { return top == -1; }

// TODO: 实现 is_match 函数
// 功能: 判断左括号 open 和右括号 close 是否匹配
//       '(' 匹配 ')'，'[' 匹配 ']'，'{' 匹配 '}'
// int is_match(char open, char close) { ... }

// TODO: 实现 check_brackets 函数
// 功能: 遍历字符串 s 的每个字符:
//       - 遇到左括号 '('/'['/{' 则 push
//       - 遇到右括号 ')'/']/'}':
//         如果栈空或栈顶不匹配，返回 0
//         否则 pop
//       遍历结束后，栈空则返回 1（匹配），否则返回 0
// int check_brackets(const char *s) { ... }

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("usage: %s <expression>\n", argv[0]);
        return 1;
    }

    // TODO: 调用 check_brackets(argv[1])
    // 根据返回值打印 "yes\n" 或 "no\n"

    return 0;
}
