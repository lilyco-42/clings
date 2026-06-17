## Lesson 22 Guess Number in My Hand 猜数游戏

### 代码

    /* 生成4位不重复随机数 */
    static void generate_secret(int secret[4])
    {
        int used[10] = {0};
        for (int i = 0; i < 4; i++) {
            int d;
            do { d = rand() % 10; } while (used[d]);
            secret[i] = d;
            used[d] = 1;
        }
    }

    /* 比较猜测与答案, 返回 ?A?B */
    static void check(const int secret[4], const int guess[4], int *a, int *b)
    {
        *a = *b = 0;
        for (int i = 0; i < 4; i++) {
            if (guess[i] == secret[i]) (*a)++;
            else for (int j = 0; j < 4; j++)
                if (guess[i] == secret[j]) { (*b)++; break; }
        }
    }

### 游戏规则

    电脑随机产生4位数(各位不重复)，用户输入4位数猜测:
    - A = 位置和数字都对
    - B = 数字对但位置不对
    - 4A0B = 猜对了!

### 知识点

- 随机数生成
  - `srand(time(NULL))` 播种，`rand() % 10` 生成 0-9
  - "不重复"约束：用 `used[10]` 数组标记已用数字
- 数组与循环的配合
  - `secret[4]` 和 `guess[4]` 存储各位数字
  - 双重循环检查 B (位置不对但数字对)
- 输入处理
  - `fgets` + `parse_guess`: 字符串转数字数组
  - 输入验证：长度检查，范围检查

### 课堂讨论

- 最少需要多少次猜测可以保证猜对？(信息论角度：10^4 = 10000 种可能)
- 如果允许重复数字，check 函数需要怎么修改？
- 如何实现一个"自动猜测"的 AI 玩家？

### 课后练习

- 添加猜测次数限制：超过 10 次则游戏失败
- 添加提示：在 3A0B 时提示"还差一步"
- 挑战：实现 Mastermind 变体 — 用颜色代替数字
