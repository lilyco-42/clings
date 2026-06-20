// 35c_postfix_eval.c — 后缀表达式（逆波兰）求值
//
// 任务: 使用栈计算后缀表达式的值
//       支持单位数操作数和 +、-、* 运算符
//
// 知识点: 栈应用——表达式求值、后缀表达式
//
// 用法: ./35c_postfix_eval "12+3*4+"   → 13
//       ./35c_postfix_eval "53+82-*"   → 48
//
// 算法:
//   遍历表达式字符串:
//   - 数字: push(ch - '0')
//   - 运算符: pop 两个操作数，计算结果并 push
//   最后栈顶就是结果

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX_SIZE 256

int stack[MAX_SIZE];
int top = -1;

void push(int val) { stack[++top] = val; }
int pop(void) { return stack[top--]; }

// TODO: 实现 eval_postfix 函数
// 功能: 遍历表达式字符串 expr:
//   - 如果是数字字符 (isdigit)，push(ch - '0')
//   - 如果是运算符 (+, -, *)，pop 两个值 b 和 a（注意顺序！先 pop 的是 b）
//     计算 a op b，将结果 push 回去
//   遍历结束后返回 pop()（栈顶即结果）
//
// 注意: 减法和除法要注意操作数顺序: a = pop第二次, b = pop第一次, 结果 = a op b
// int eval_postfix(const char *expr) { ... }

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("usage: %s <postfix-expression>\n", argv[0]);
        return 1;
    }

    // TODO: 调用 eval_postfix(argv[1]) 并打印结果
    // printf("%d\n", eval_postfix(argv[1]));

    return 0;
}
