## Lesson 49 Delete Comments in C Program C程序去注释

### 代码

    /* strip_comments.c - 用 FSM 去除 C 代码中的注释 */
    typedef enum {
        S_NORMAL, S_SLASH, S_LINE_CMT, S_BLOCK_CMT,
        S_BLOCK_STAR, S_STRING, S_STRING_ESC, S_CHAR, S_CHAR_ESC
    } State;

    void strip_comments(FILE *in, FILE *out)
    {
        State state = S_NORMAL;
        int c;
        while ((c = fgetc(in)) != EOF) {
            switch (state) {
            case S_NORMAL:
                if (c == '/')       state = S_SLASH;
                else if (c == '"')  { state = S_STRING; fputc(c, out); }
                else                fputc(c, out);
                break;
            case S_SLASH:
                if (c == '/')       state = S_LINE_CMT;
                else if (c == '*')  state = S_BLOCK_CMT;
                else { fputc('/', out); fputc(c, out); state = S_NORMAL; }
                break;
            /* ... 完整代码见 strip_comments.c */
            }
        }
    }

### 知识点

- 有限状态机 (Finite State Machine)
  - 9 个状态覆盖: 正常代码、`//`行注释、`/* */`块注释、字符串/字符字面量
  - 状态转移由当前字符驱动，`switch-case` 实现清晰高效
  - 核心难点: 字符串内的 `//` 和 `/* */` **不是注释**
- 字符串/字符字面量的转义保护
  - `S_STRING_ESC` 状态确保 `\"` 不会提前结束字符串
  - 同理 `S_CHAR_ESC` 保护 `'\''`
- 行号保留策略
  - 注释中的换行符仍然输出，保持行号对齐
  - 块注释替换为空格（避免相邻 token 粘连）

### 课堂讨论

- 为什么需要 `S_SLASH` 这个中间状态？能否省略？
- 如果块注释没有闭合（`/* ...` 到文件末尾），应该如何处理？
- 此 FSM 能否处理 C++ 的 `R"delimiter(raw string)delimiter"` 语法？

### 课后练习

- 测试: `char *s = "not /* comment */";` 应保留字符串内容不变
- 扩展: 增加统计功能 — 输出去除了多少行注释、多少块注释
- 挑战: 用函数指针数组 `handler[S_COUNT]` 替代 `switch`，实现表驱动 FSM

### 参考资料

- K&R《C程序设计语言》习题 1-23: 去除C程序中所有注释
- Rob Pike, "Lexical Scanning in Go" — 状态机词法扫描思想
