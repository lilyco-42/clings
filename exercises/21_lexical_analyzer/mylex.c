/*
 * Lesson 21: 词法分析器 — DFA 表驱动
 *
 * 知识点：状态机，DFA 表驱动，机制与策略的分离
 * 参考原课 mylex.c — 用 yy_nxt[][] 转移表 + yy_accept[] 动作表驱动
 *
 * 核心思想："机制与策略分离"
 *   - 机制 (mechanism): main 循环中的 state = yy_nxt[state][c] 状态转移
 *   - 策略 (policy): yy_nxt/yy_accept 表的具体数值 (由工具生成)
 *
 * 任务：实现 DFA 主循环:
 *       1. while (1) 循环，getchar 读字符
 *       2. buf[i++] = c 积累当前 token
 *       3. state = yy_nxt[state][c] 查表转移
 *       4. 当 state < 0 时：找到 token 边界
 *          - state = -state; act = yy_accept[state]
 *          - buf[i-1] = '\0' (去掉触发退出的字符)
 *          - 跳过空白 (act==21) 和换行 (act==1)，其余打印
 *          - ungetc(c, stdin) 回退触发字符
 *          - 重置 i=0, state=1
 *
 * 验证：输入 C 代码片段 → "\tPattern NN found: token\n" 格式
 */

#include <stdio.h>
#include "yy_nxt.c"
#include "yy_accept.c"

int main(void)
{
	int state = 1;
	char buf[64];
	int i = 0;

#error TODO: Implement DFA loop: accumulate chars, lookup yy_nxt, emit tokens. Run "clings hint" for help.

	return 0;
}
