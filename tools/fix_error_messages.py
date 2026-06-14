#!/usr/bin/env python3
"""Replace Chinese #error TODO messages with English for GCC compatibility."""
import re
from pathlib import Path

EXERCISES_DIR = Path(__file__).resolve().parent.parent / "exercises"

# Map: old Chinese #error line content → new English version
# Only Unit 1 exercises need fixing (Unit 2-4 already English)
REPLACEMENTS = {
    # 01
    '#error TODO: 写一个完整的 main 函数，返回 0。需要: 返回类型 int, 函数名 main, 参数 void, 函数体 { return 0; }':
        '#error TODO: Write a complete main function that returns 0. Run "clings hint" for help.',
    '#error TODO: 定义全局变量 global=2, 局部变量 local=1，return global + local':
        '#error TODO: Define global var (=2) and local var (=1), return their sum. Run "clings hint" for help.',
    # 02
    '#error TODO: 使用 printf 输出 "hello, world.\\n"':
        '#error TODO: Use printf to print "hello, world.\\n". Run "clings hint" for help.',
    # 03
    '#error TODO: 用 while 循环，从 counter=1 数到 10，每次打印 "counter = %d\\n"':
        '#error TODO: Use a while loop to count from 1 to 10, printing each value. Run "clings hint" for help.',
    '#error TODO: 用 for 循环从 10 倒数到 1，打印每个值，同时累加 sum':
        '#error TODO: Use a for loop counting down from 10 to 1, printing and summing. Run "clings hint" for help.',
    # 04
    '#error TODO: 用 if/else 判断 num % 2 == 0，打印 "num %d is even\\n" 或 "num %d is odd\\n"':
        '#error TODO: Use if/else to check num%2==0, print "num %d is even/odd\\n". Run "clings hint" for help.',
    # 05
    '#error TODO: 用 for 循环从 1 加到 100，打印 "sum = %d\\n"':
        '#error TODO: Sum 1 to 100 using a loop, print "sum = %d\\n". Run "clings hint" for help.',
    '#error TODO: 在循环中用 continue 跳过奇数，只累加偶数':
        '#error TODO: Use continue to skip odd numbers, sum only even ones. Run "clings hint" for help.',
    # 06
    '#error TODO: 写 for-for 嵌套循环，格式 printf("%d*%d=%d\\t", j, i, i*j)，每行末尾换行':
        '#error TODO: Nested for loops for multiplication table. Run "clings hint" for help.',
    # 07
    '#error TODO: 实现 is_prime 函数，从 100 往回找第一个素数':
        '#error TODO: Implement is_prime(), find the largest prime <= 100. Run "clings hint" for help.',
    # 08
    '#error TODO: 实现 find(num, digit) — 用 do-while 逐位提取，统计 digit 出现次数':
        '#error TODO: Implement find(num, digit) counting digit occurrences. Run "clings hint" for help.',
    # 09
    '#error TODO: 实现 itoa_hex — do-while 取 num%16 查表, 逆序输出':
        '#error TODO: Implement itoa_hex using do-while + hex lookup table. Run "clings hint" for help.',
    "#error TODO: do-while { buf[i]=num%10+'0'; i++; num/=10; } while(num!=0); buf[i]='\\0'; 然后 for 逆序":
        '#error TODO: Implement itoa: extract digits with do-while, then reverse. Run "clings hint" for help.',
    # 10
    '#error TODO: init_ring: next[i] = (i+1) % n; print_ring: 从 0 开始沿 next[] 遍历打印':
        '#error TODO: init_ring: next[i]=(i+1)%n; print_ring: traverse and print. Run "clings hint" for help.',
    '#error TODO: 初始化 left=ALL, counter=0, i=0, prev=ALL-1; 然后 while(left>0) 循环实现出队逻辑':
        '#error TODO: Implement Josephus ring elimination loop. Run "clings hint" for help.',
    # 11
    '#error TODO: 定义 struct point { float x; float y; }，typedef 为 point_t':
        '#error TODO: Define struct point {float x; float y;} with typedef. Run "clings hint" for help.',
    '#error TODO: 实现 float calculate(point_t p1, point_t p2)':
        '#error TODO: Implement float calculate(point_t p1, point_t p2) for distance. Run "clings hint" for help.',
    # 12
    '#error TODO: 用 union 或指针检测字节序，小端返回 1，大端返回 0':
        '#error TODO: Detect byte order using union or pointer cast. Run "clings hint" for help.',
    # 13
    '#error TODO: while(str[i]) 遍历，记录每个字符，循环结束后 return 最后记录的字符':
        '#error TODO: Traverse string to find and return the last character. Run "clings hint" for help.',
    '#error TODO: switch(tail_num) 分组 + 三元表达式 (today==限行日)?1:0':
        '#error TODO: Use switch on tail_num and ternary expr for restriction check. Run "clings hint" for help.',
    '#error TODO: 用 m[12] 累加天数, switch((alldays+origin_day)%7) 返回 enum day':
        '#error TODO: Compute weekday from date using cumulative days + modulo. Run "clings hint" for help.',
    '#error TODO: scanf 读车牌号和年月日, 调用 get_last_char + get_week_day + is_restricted, 输出结果':
        '#error TODO: Read input, call get_last_char + get_week_day + is_restricted. Run "clings hint" for help.',
    # 14
    '#error TODO: 二重 for 循环, chessboard[i][j] = my_rand() % 2':
        '#error TODO: Fill chessboard[i][j] = my_rand() % 2 with nested loops. Run "clings hint" for help.',
    '#error TODO: 二重 for 循环打印, 元素间空格 (if j>0 printf(" ")), printf("%d"), 每行末 "\\n"':
        '#error TODO: Print board with nested loops, spaces between elements. Run "clings hint" for help.',
    '#error TODO: nr = row + dir.dr, nc = col + dir.dc; is_valid(nr, nc) && chessboard[nr][nc] == 0 则返回 1':
        '#error TODO: Compute new pos, check is_valid && chessboard==0, return 1 if ok. Run "clings hint" for help.',
    # 15
    '#error TODO: for(i=0;i<32;i++) if(num & (1<<i)) sum++; 逐位检测':
        '#error TODO: Count set bits by checking each of 32 bit positions. Run "clings hint" for help.',
    '#error TODO: while(num) { num &= (num-1); sum++; } 每次清除最低位的 1':
        '#error TODO: Count bits using n&(n-1) trick to clear lowest set bit. Run "clings hint" for help.',
    '#error TODO: 5 层分治 — sum = (sum & M1) + ((sum >> 1) & M1); 依次 M2>>2, M3>>4, M4>>8, M5>>16':
        '#error TODO: Implement 5-layer divide-and-conquer popcount with masks M1-M5. Run "clings hint" for help.',
    # 16
    '#error TODO: 计算 delta = dst - src, 然后 while(*src) { src[delta] = *src; src++; } src[delta]=0':
        '#error TODO: Compute delta offset, copy using single pointer traversal. Run "clings hint" for help.',
    "#error TODO: char *p = dest; while((*dest++ = *src++) != '\\0'); return p;":
        '#error TODO: Implement mystrcpy using pointer idiom. Run "clings hint" for help.',
    # 17
    '#error TODO: while(1) 状态机循环 — 4 分支 (参考上方任务说明), \'\\0\' 时 break':
        '#error TODO: Implement state machine loop with 4 branches for word counting. Run "clings hint" for help.',
    # 18
    "#error TODO: do-while 取余查表 hex[], buf[i++]=hex[rest], num/=base, 最后逆序 + '\\0'":
        '#error TODO: Implement itoa with base conversion using hex lookup table. Run "clings hint" for help.',
    '#error TODO: va_list ap; 遍历 format, %d/%x 用 itoa+myputs, %s 用 myputs, %c 用 putchar (参考 18b)':
        '#error TODO: Implement myprintf with va_list, handling %d/%x/%s/%c. Run "clings hint" for help.',
    "#error TODO: do-while 取余入 buf，buf[i]='\\0'，然后 for 循环逆序 (参考 18a)":
        '#error TODO: Implement itoa (same as 18a). Run "clings hint" for help.',
    '#error TODO: va_list ap; 遍历 format, %d/%x 用 itoa+myputs, %s 用 myputs, %c 用 putchar (参考 18b)':
        '#error TODO: Implement myprintf with va_list (same as 18b). Run "clings hint" for help.',
    # 19
    "#error TODO: 状态机切分 — 空格时 state=0 并截断('\\0'), 非空格且 state==0 时记录 argv[argc++]":
        '#error TODO: Implement shell_parse: split input into argc/argv using state machine. Run "clings hint" for help.',
    '#error TODO: strcmp 匹配 argv[0]，赋值全局 pf 和 opchar，调用 math_main':
        '#error TODO: Match argv[0] with strcmp, assign function pointer, call it. Run "clings hint" for help.',
    '#error TODO: 定义 struct operation { name, pf, opchar } 和 op[] 数组 (add/sub/mul/div/power)':
        '#error TODO: Define struct operation and op[] array with 5 operations. Run "clings hint" for help.',
    '#error TODO: 遍历 op[], strcmp 匹配 argv[0] 后设置全局 pf/opchar, 调用 math_main':
        '#error TODO: Loop through op[] table, match command, call function. Run "clings hint" for help.',
    '#error TODO: 实现 shell_parse (状态机切分 argv) 和 command_do (遍历命令表匹配调用)':
        '#error TODO: Implement both shell_parse and command_do. Run "clings hint" for help.',
    # 20
    '#error TODO: 5 状态 FSM — 逐字符 getchar, 根据 state 决定 putchar 或跳过':
        '#error TODO: Implement 5-state FSM to strip C comments. Run "clings hint" for help.',
    "#error TODO: getchar 循环 — EOF 返回空串, 非字母返回单字符, 字母则 do-while 读完整单词 + ungetc":
        '#error TODO: Implement getword: read chars, return word or single char. Run "clings hint" for help.',
    '#error TODO: 遍历 line，把匹配 macros[i].name 的单词替换为 macros[i].value，然后输出':
        '#error TODO: Replace macro names with values in the input line. Run "clings hint" for help.',
    # 21
    '#error TODO: while(1) { getchar, buf累积, yy_nxt查表, state<0时处理token(yy_accept+打印+ungetc) }':
        '#error TODO: Implement DFA loop: accumulate chars, lookup yy_nxt, emit tokens. Run "clings hint" for help.',
    # 22
    '#error TODO: 用 used[10] + do { d = my_rand() % 10; } while(used[d]) 生成 4 个不重复的 0-9 随机数':
        '#error TODO: Generate 4 unique digits using used[] + do-while + my_rand(). Run "clings hint" for help.',
    '#error TODO: 外层 i=0..3: guess[i]==secret[i]→A++, 否则内层 j 找相同数字→B++':
        '#error TODO: Count A (exact match) and B (wrong position) for guess vs secret. Run "clings hint" for help.',
    '#error TODO: 检查 strlen(s)>=4, 循环 s[i]-\'0\' 转为 guess[i], 检查 0-9 范围, 失败返回 -1':
        '#error TODO: Parse guess string: check length, convert chars to digits. Run "clings hint" for help.',
    '#error TODO: generate_secret → while(1) { fgets → parse_guess → check → printf %dA%dB → if(a==4) Congratulations+break } → 打印 secret':
        '#error TODO: Implement game loop: generate, read guesses, check, print results. Run "clings hint" for help.',
    # 23
    '#error TODO: memset(board, \'.\', sizeof(board));':
        '#error TODO: Initialize board with memset. Run "clings hint" for help.',
    '#error TODO: for k=0..4 检查 (r+k*dr, c+k*dc) 是否越界且等于 player，count>=5 返回 1':
        '#error TODO: Check 5 consecutive cells in direction (dr,dc) for same player. Run "clings hint" for help.',
    '#error TODO: 二重循环遍历棋盘，4 个方向调用 check_dir，有一个返回 1 则整体返回 1':
        '#error TODO: Scan board, check 4 directions at each position for a win. Run "clings hint" for help.',
    '#error TODO: while(1) 游戏循环: turn%2 选玩家, scanf 落子, 验证合法性, check_win 判赢, board_full 判平, turn++':
        '#error TODO: Implement game loop: alternate players, place, check win/draw. Run "clings hint" for help.',
    # 24
    '#error TODO: while(*text) 逐位置用 tolower 比较，匹配则 return text，否则 text++':
        '#error TODO: Case-insensitive string search using tolower comparison. Run "clings hint" for help.',
    '#error TODO: while find_istr(p,"href=") → 跳过href= → 检测引号 → 提取URL → printf("[%d] %s\\n")':
        '#error TODO: Extract URLs from href= attributes in HTML. Run "clings hint" for help.',
    '#error TODO: read_all(stdin) → extract_links → printf Total → free':
        '#error TODO: Read all input, extract links, print total, free memory. Run "clings hint" for help.',
    '#error TODO: while find_istr(p,"href=") → p+=5 → 跳空格 → 检测引号 → 提取URL → printf("[%d] %s\\n")':
        '#error TODO: Extract URLs from href= attributes in HTML. Run "clings hint" for help.',
    '#error TODO: while(*text) { 内层 for tolower 比较 → match 则 return text; 否则 text++; } return NULL;':
        '#error TODO: Case-insensitive string search using tolower comparison. Run "clings hint" for help.',
}


def main():
    count = 0
    for c_file in sorted(EXERCISES_DIR.rglob("*.c")):
        text = c_file.read_text(encoding="utf-8")
        modified = False
        for old, new in REPLACEMENTS.items():
            if old in text:
                text = text.replace(old, new)
                modified = True
                count += 1
        if modified:
            c_file.write_text(text, encoding="utf-8")
            print(f"  [OK] {c_file.relative_to(EXERCISES_DIR.parent)}")

    print(f"\nReplaced {count} #error messages")


if __name__ == "__main__":
    main()
