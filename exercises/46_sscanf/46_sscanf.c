/* 46_sscanf.c — sscanf 高级格式化解析
 *
 * 任务: 1. 实现 parse_addr(): 用 sscanf 一行代码解析 "host:port"
 *       2. 编写 main(): 读输入，成功打印 host/port，失败打印 parse error
 *
 * 核心技巧: %[^:] — 扫描集取反，"匹配除冒号外的所有字符"
 *   对于 "192.168.1.100:8080"，%[^:] 匹配 "192.168.1.100"
 *   然后字面匹配 ':'，%d 匹配端口号
 *
 * 知识点: %[^:] 高级格式控制符、sscanf 返回值判断
 *
 * 验证：
 *   stdin: "192.168.1.100:8080\n" → host: 192.168.1.100  port: 8080
 *   stdin: "localhost:3000\n"     → host: localhost      port: 3000
 *   stdin: "invalid\n"            → parse error
 */
#include <stdio.h>
#include <string.h>

/* 用 sscanf 解析 host:port 格式
 * 成功返回 1（host 和 port 通过参数传出），失败返回 0 */
int parse_addr(const char *input, char *host, int *port) {
#error TODO: Finish this exercise. Run "clings hint" for help.
    /* 关键: sscanf 的返回值 = 成功匹配并赋值的字段数
     * 如果成功匹配 2 个字段（host 和 port），返回 2
     * 格式: "%[^:]:%d" — host 读至冒号，跳过冒号，port 读整数 */

    /* return (成功匹配 2 个字段) */
}

int main(void) {
    char line[256];
    fgets(line, sizeof(line), stdin);
    int len = strlen(line);
    if (len > 0 && line[len - 1] == '\n') line[len - 1] = '\0';

    char host[128];
    int port;
#error TODO: Finish this exercise. Run "clings hint" for help.
    /* 调用 parse_addr，根据返回值输出 host 和 port 或 "parse error" */
    return 0;
}
