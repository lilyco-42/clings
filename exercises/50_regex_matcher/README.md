## Lesson 50 Regular Expression Matcher 正则表达式匹配器

### 代码
	/* Rob Pike 风格的极简正则匹配 */
	int match(const char *re, const char *text)
	{
	    if (re[0] == '^')
	        return matchhere(re + 1, text);
	    do {
	        if (matchhere(re, text)) return 1;
	    } while (*text++ != '\0');
	    return 0;
	}
	
	static int matchhere(const char *re, const char *text)
	{
	    if (re[0] == '\0') return 1;
	    if (re[1] == '*') return matchstar(re[0], re + 2, text);
	    if (re[0] == '$' && re[1] == '\0') return *text == '\0';
	    if (*text && (re[0] == '.' || re[0] == *text))
	        return matchhere(re + 1, text + 1);
	    return 0;
	}

### 知识点
* Rob Pike 极简正则实现
	- 仅 ~30 行代码覆盖: 字面量、`.`、`*`、`^`、`$`
	- 递归下降结构: `match` → `matchhere` → `matchstar`
	- 优雅的 `do-while` 逐位尝试匹配
* `*` 的贪婪匹配与回溯
	- `matchstar`: 先尽可能匹配(贪婪), 然后逐步回退
	- 回溯本质: 用 `do-while` 从最长到最短逐一尝试
	- 这就是正则引擎中**回溯法**的最简实现
* 字符类 `[abc]` 和 `[a-z]` 的扩展
	- 解析 `[`...`]` 内的字符范围
	- `[^abc]` 取反: `negate` 标志翻转匹配结果
* 编译器关联
	- 词法分析器本质上是正则匹配器 (每个 Token 类型对应一个正则模式)
	- `matchhere` 的递归结构与递归下降解析器同源

### 课堂讨论
* 贪婪匹配 `a.*b` 对 "aXbYb" 会匹配到哪里？为什么？
* 如果要支持 `+` (一次或多次), 应该怎么修改？
* NFA vs DFA: 为什么 Rob Pike 版本是 NFA 模式？

### 课后练习
* 添加 `+` 支持 (提示: `c+` 等价于 `cc*`)
* 添加 `?` 支持 (零次或一次)
* 挑战: 实现 `grep` 命令 — 从标准输入逐行匹配并输出

### 参考资料
* Rob Pike, "A Regular Expression Matcher" — 《Beautiful Code》第1章
* Russ Cox, "Regular Expression Matching Can Be Simple And Fast" https://swtch.com/~rsc/regexp/regexp1.html
