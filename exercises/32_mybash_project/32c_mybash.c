// 32c_mybash.c — 整合所有命令的 Bash Clone（Linux-only）
//
// 任务: 实现一个简单的 shell，支持交互式输入和内置命令分发。
//       整合 Lesson 25-31 的所有命令（mycp/readelf/sed/math/more/sort/ll）。
//
// 知识点: 综合项目——命令行解析 + 函数指针分发 + 多模块链接
//
// 编译: 需要链接 25-31 的 .c 文件（参见 Makefile）
// 用法: ./32c_mybash（进入交互模式，输入 exit 或 Ctrl+D 退出）

#include <stdio.h>
#include <string.h>

// TODO: 声明外部函数（或 #include 对应头文件）:
//   int mycp_main(int, char **);
//   int readelf_main(int, char **);
//   int sed_main(int, char **);
//   int math_main(int, char **);
//   int more_main(int, char **);
//   int sort_main(int, char **);
//   int ll_main(int, char **);

// TODO: 定义分发表 struct operation + op[] 数组

// TODO: 实现 command_do(int argc, char *argv[])

// TODO: 实现 shell_parse(char *buf)

int main(void)
{
    char buf[256];

    // TODO: 实现 REPL 循环:
    //   while (1) {
    //       printf("NCCL# ");
    //       if (fgets(buf, sizeof(buf), stdin) == NULL) break;  // Ctrl+D
    //       去掉末尾换行
    //       if (strcmp(buf, "exit") == 0 || strcmp(buf, "quit") == 0) break;
    //       shell_parse(buf);
    //   }
    //   printf("\n");

    return 0;
}
