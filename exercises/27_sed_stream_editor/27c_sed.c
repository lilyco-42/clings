/*
 * Lesson 27c: 完整 sed 流处理器
 *
 * 知识点：strtok 解析 s/pattern/replace/flags 格式, 多行处理, ^ 行首正则
 *
 * 任务：
 *   实现简化版 sed 命令，支持 s 替换命令：
 *
 *   用法: ./27c_sed 's/pattern/replace/flags'
 *
 *   功能要求：
 *   1. 解析 s/regexp/replacement/flags 格式
 *   2. 从 stdin 逐行读取（while fgets），对每行执行替换
 *   3. 支持 flags: 无(替换第1个), g(全部), N(第N个)
 *   4. 支持 ^ 行首锚定: ^pattern 只匹配行首
 *
 *   示例：
 *     echo "unix is great. unix is free." | ./27c_sed 's/unix/linux/'
 *     → "linux is great. unix is free.\n"
 *
 *     echo "unix is great. unix is free." | ./27c_sed 's/unix/linux/g'
 *     → "linux is great. linux is free.\n"
 *
 *     echo "unix is great." | ./27c_sed 's/^unix/linux/'
 *     → "linux is great.\n"
 */

#include <stdio.h>
#include <string.h>

#define SIZE 512

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s 's/pattern/replace/[flags]'\n", argv[0]);
        return 1;
    }

#error TODO: Fix this exercise. Run "clings hint" for help.
    /* 实现步骤：
     * 1. 用 strtok 解析 argv[1]: cmd / regexp / replace / flags
     * 2. 检查 regexp 是否以 '^' 开头（行首锚定）
     * 3. while (fgets(buf, SIZE, stdin) != NULL) 逐行处理
     * 4. 对每行执行替换逻辑（复用 27b 的算法） */

    return 0;
}
