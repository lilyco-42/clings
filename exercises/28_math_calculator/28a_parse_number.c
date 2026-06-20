/*
 * Lesson 28a: 解析数字
 *
 * 知识点：strtod 字符串转浮点, 指针前进, 跳过空白
 *
 * 任务：
 *   实现一个简单的数字解析器：
 *   1. 从 argv[1] 读取表达式字符串
 *   2. 跳过前导空白
 *   3. 用 strtod 解析一个数字
 *   4. 如果是整数则用 %d 打印，否则用 %.6g 打印
 *
 *   示例：
 *     ./28a_parse_number "42"      → "42\n"
 *     ./28a_parse_number "3.14"    → "3.14\n"
 *     ./28a_parse_number "  -5.0"  → "-5\n"
 */

#include <stdio.h>
#include <stdlib.h>

static const char *pos;

static void skip_spaces(void)
{
    while (*pos == ' ' || *pos == '\t')
        pos++;
}

static double parse_number(void)
{
    skip_spaces();
    char *end;
    double val = strtod(pos, &end);
    if (end == pos) {
        fprintf(stderr, "Error: expected number at '%s'\n", pos);
        exit(1);
    }
    pos = end;
    return val;
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s expression\n", argv[0]);
        return 1;
    }

#error TODO: Fix this exercise. Run "clings hint" for help.
    /* 1. pos = argv[1]
     * 2. 调用 parse_number() 获取值
     * 3. 如果 val == (long)val 则打印 %ld，否则 %.6g */

    return 0;
}
