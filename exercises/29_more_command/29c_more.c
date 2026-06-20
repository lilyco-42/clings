// 29c_more.c — 完整 more 命令（Linux-only）
//
// 任务: 实现完整的 more 分页浏览器，支持 j/k/空格/b/q 快捷键。
//       需要 termios 控制终端进入 raw 模式。
//
// 知识点: termios（raw mode）、ioctl（TIOCGWINSZ）、终端恢复
//
// 编译: gcc -Wall -Wextra -std=c11 -g 29c_more.c -o 29c_more
// 用法: ./29c_more <filename>
// 注意: 此练习需要 Linux 环境（Docker 或远程），Windows 不支持 termios

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define MAX_LINES 65536

static struct termios orig_ts;
static char *pv[MAX_LINES];
static int lines = 0;

// TODO: 实现 get_term_size(int *w, int *h)
//       使用 ioctl(STDIN_FILENO, TIOCGWINSZ, &ws) 获取终端尺寸
//       注意: struct winsize 用栈变量，不要 malloc

// TODO: 实现 set_raw_mode()
//       1. tcgetattr 保存原始属性到 orig_ts
//       2. 复制一份，关闭 ECHO | ICANON
//       3. 设置 VMIN=1, VTIME=0
//       4. tcsetattr 应用新属性

// TODO: 实现 restore_term()
//       tcsetattr 恢复 orig_ts（在退出前必须调用！）

// TODO: 实现 free_lines() 释放所有 malloc 的行

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    // TODO: 打开文件，fgets + malloc 读取所有行
    // TODO: 关闭文件
    // TODO: 调用 get_term_size 获取终端高度 height
    // TODO: 调用 set_raw_mode 进入 raw 模式
    // TODO: 实现分页循环:
    //   - system("clear") 清屏
    //   - 显示当前页（从 top 开始，显示 dropdown 行）
    //   - 打印 ":" 提示符
    //   - getc(stdin) 读取按键（用 int 接收！）
    //   - 根据按键调整 top:
    //     'q': 退出
    //     'j'/Enter: 下移一行
    //     'k': 上移一行
    //     ' '/'f': 下翻一页
    //     'b': 上翻一页
    // TODO: 退出前调用 restore_term() 恢复终端！
    // TODO: 调用 free_lines() 释放内存

    return 0;
}
