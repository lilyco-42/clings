## Lesson 54 JSON Parser JSON 解析器

### 代码
	/* 递归下降解析 JSON 值 */
	static JsonValue *parse_value(void)
	{
	    skip_ws();
	    switch (*cur) {
	    case '"': return parse_string();
	    case '[': return parse_array();
	    case '{': return parse_object();
	    case 't': cur += 4; return new_bool(1);  /* true */
	    case 'f': cur += 5; return new_bool(0);  /* false */
	    case 'n': cur += 4; return new_null();    /* null */
	    default:
	        if (*cur == '-' || isdigit(*cur))
	            return parse_number();
	        error("unexpected character");
	    }
	}

### 知识点
* 递归下降解析法 (Recursive Descent)
	- 每个语法规则对应一个解析函数
	- JSON: `value → string | number | array | object | true | false | null`
	- `parse_array` 调用 `parse_value` 形成递归
	- **这与 Unit 4 编译器 parser 的结构完全一致**
* JSON 值类型与 C 表示
	- `enum JsonType`: NULL/BOOL/NUMBER/STRING/ARRAY/OBJECT
	- `union` 存储不同类型的值 (tagged union)
	- OBJECT 用链表 `JsonPair` 存储键值对
* 内存管理
	- `calloc` 分配节点, `free_json` 递归释放
	- 字符串需要额外 `malloc` + `free`

### 课堂讨论
* JSON 解析器和 C 编译器的 parser 有什么结构相似之处？
* 如果 JSON 嵌套很深 (如 10000 层), 会有什么问题？
* 为什么用 `tagged union` 而不是每种类型一个 struct？

### 课后练习
* 添加 JSON 路径查询: `json_get(root, "compiler.target[0]")` → "riscv"
* 添加 JSON 输出: `json_to_string(root)` 生成 JSON 文本
* 挑战: 从文件读取 JSON, 实现 `jq` 风格的命令行查询工具

### 参考资料
* RFC 8259 - The JavaScript Object Notation (JSON) Data Interchange Format
* Douglas Crockford, "Introducing JSON" https://json.org
