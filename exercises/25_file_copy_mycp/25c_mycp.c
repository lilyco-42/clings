/*
 * Lesson 25c: mycp — 完整文件拷贝命令
 *
 * 知识点：综合运用 fopen/fread/fwrite/fclose, 错误处理, 资源释放
 *
 * 任务：
 *   实现 cp 命令: 将 argv[1] 的文件内容完整拷贝到 argv[2]
 *
 *   要求：
 *   1. argc < 3 时打印 Usage 到 stderr 并返回 1
 *   2. 源文件打开失败时用 perror 报错并返回 1
 *   3. 目标文件打开失败时用 perror 报错，关闭源文件后返回 1
 *   4. 循环 fread/fwrite 拷贝内容（不使用 feof 做循环条件）
 *   5. fwrite 写入量与 fread 读取量不一致时报错
 *   6. 读取结束后检查 ferror 确认无读错误
 *   7. 关闭两个文件后返回 0
 *
 * 验证：
 *   ./25c_mycp source.txt dest.txt && diff source.txt dest.txt
 */

#include <stdio.h>

#define BUF_SIZE 512

int main(int argc, char *argv[])
{
#error TODO: Fix this exercise. Run "clings hint" for help.
    /* 实现完整的文件拷贝逻辑 */
    return 0;
}
