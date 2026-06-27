## Lesson 31 — 单链表查找与删除

### 课程任务

在 Lesson 30 构建链表的基础上，实现单链表的三种核心操作：

1. **`find_by_value(head, val)`** — 按值查找：遍历链表，返回第一个 `data == val` 的节点指针，未找到返回 `NULL`
2. **`find_by_index(head, idx)`** — 按索引查找（0-based，从 0 开始计数）：返回第 `idx` 个节点的指针，索引越界返回 `NULL`
3. **`delete_node(head, val)`** — 按值删除：删除第一个 `data == val` 的节点，返回（可能已被更新的）head，正确处理**删头节点、删中间节点、删尾节点、删唯一节点、未找到**五种情况

`make_node`、`build_list`、`list_print`、`list_free` 已由模板提供。你的任务是实现三个查找/删除函数。

### 前置知识：为什么链表删除比插入难？

Lesson 30 的插入操作只需要知道"在哪里插入"——头插不需要遍历（O(1)），尾插只需找到最后一个节点然后挂上去（O(n)）。

但**删除**有一层额外的复杂性：单链表的节点只有 `next` 指针指向后面，**无法"回头看"前一个节点**。要删除节点 `cur`，必须修改它**前一个节点**的 `next` 指针来"跳过" `cur`：

```
删除前: ... → [prev] → [cur] → [next] → ...
                   ↑ prev->next 指向 cur

删除后: ... → [prev] → [next] → ...
                   ↑ prev->next 改为跳过 cur
                   free(cur) 释放被删除的节点
```

### 链表遍历的基本模式

#### 模式 A：只读遍历（查找用）

```c
struct node *cur = head;
while (cur != NULL) {
    // 处理 cur->data
    cur = cur->next;
}
```

#### 模式 B：带前驱指针遍历（删除需要！）

```c
struct node *prev = NULL;
struct node *cur = head;
while (cur != NULL) {
    if (/* 找到目标 */) {
        // 用 prev 和 cur 执行删除
    }
    prev = cur;        // ← 关键！prev 落后 cur 一步
    cur = cur->next;
}
```

**`prev` 始终指向 `cur` 的前一个节点。** 这个"落后一步"的关系是删除操作正确性的基石。

### 按值查找 `find_by_value(head, val)`

#### 核心思想

从头到尾遍历链表，逐一比较每个节点的 `data` 字段，找到第一个匹配值即返回该节点指针。如果遍历完整条链表都没找到，返回 `NULL`。

#### 逐步跟踪

```
链表: [1]→[3]→[5]→[7]→NULL
       查找 val=5

cur=1: 1 ≠ 5 → cur = cur->next
cur=3: 3 ≠ 5 → cur = cur->next
cur=5: 5 == 5 → 返回指向此节点的指针 ✓

查找 val=9:
cur=1→3→5→7→NULL (遍历结束)
未找到 → 返回 NULL ✓
```

#### 复杂度与注意事项

| 指标 | 值 |
|------|-----|
| 时间复杂度 | O(n)，最坏遍历到尾 |
| 空间复杂度 | O(1) |
| 返回值 | 节点指针或 NULL |

**注意**：按值查找只返回第一个匹配的节点。如果链表中有多个节点的值相同（如 `3→3→5`），查找 `3` 只返回第一个 `3` 所在的节点。

### 按索引查找 `find_by_index(head, idx)`

#### 核心思想

遍历链表的同时维护一个计数器 `i`（从 0 开始）。当 `i == idx` 时返回当前节点。如果遍历结束都没到达 `idx`，说明越界。

#### 图解

```
链表: [A]→[B]→[C]→[D]→NULL
       查找 idx=2

i=0, cur=A: 0≠2 → i=1, cur=B
i=1, cur=B: 1≠2 → i=2, cur=C
i=2, cur=C: 2==2 → 返回 C ✓

查找 idx=5:
i=0→...→i=3, cur=D→NULL (遍历结束)
5 > 最大索引 3 → 返回 NULL (越界) ✓

查找 idx=0:
i=0, cur=A: 0==0 → 返回 A (头节点) ✓
```

#### 边界情况

| 输入 | 预期 | 原因 |
|------|------|------|
| 空链表, idx=0 | NULL | head==NULL，无节点 |
| 空链表, idx>0 | NULL | 同上 |
| head→X, idx=0 | 返回 head | 第一个节点 |
| head→...→X, idx=n-1 | 返回尾节点 | 最后一个节点 |
| head→...→X, idx=n | NULL | 只有 n 个节点，最大索引 n-1 |
| idx < 0 | NULL 或未定义 | 题目假设 idx ≥ 0 |

### 删除操作 `delete_node(head, val)` — 五种边界

删除是链表操作中最容易出错的部分。必须区分五种情况：

| # | 情况 | `prev` 状态 | 操作 | `head` 变化 |
|---|------|-----------|------|------------|
| 1 | 删头节点 | `prev == NULL` | `head = cur->next; free(cur);` | ✅ 更新为新 head |
| 2 | 删中间节点 | `prev != NULL` | `prev->next = cur->next; free(cur);` | ❌ 不变 |
| 3 | 删尾节点 | `prev != NULL`, `cur->next == NULL` | `prev->next = NULL; free(cur);` | ❌ 不变 |
| 4 | 删唯一节点 | `prev == NULL`, `cur->next == NULL` | `free(cur);` | ✅ 变为 NULL |
| 5 | 未找到 | — | 不操作 | ❌ 不变 |

#### 图解：删除头节点

```
链表: head→[1]→[3]→[5]→NULL, 删除 val=1

cur=1: 匹配！
prev==NULL → 这是头节点
head = cur->next (即节点3)
free(cur)

结果: head→[3]→[5]→NULL ✓
```

#### 图解：删除中间节点

```
链表: head→[1]→[3]→[5]→NULL, 删除 val=3

遍历: prev=NULL cur=1 → 1≠3 → prev=1 cur=3
cur=3: 匹配！prev==1 (非NULL)
prev->next = cur->next (即节点5)
free(cur)

结果: head→[1]→[5]→NULL ✓ (1的next现在指向5)
```

#### 图解：删除尾节点

```
链表: head→[1]→[3]→[5]→NULL, 删除 val=5

遍历: prev=1 cur=3 → 3≠5 → prev=3 cur=5
cur=5: 匹配！prev==3 (非NULL), cur->next==NULL (尾节点)
prev->next = NULL
free(cur)

结果: head→[1]→[3]→NULL ✓
```

### 常见错误与陷阱

| 错误代码/思路 | 后果 | 正确做法 |
|-------------|------|---------|
| 忘写 `free(cur)` | 内存泄漏。节点被移出链表但内存未归还 | 修改 `prev->next` 后立即 `free(cur)` |
| 删头节点不更新 `head` | `head` 成为悬空指针，后续访问段错误 | `head = cur->next; free(cur); return head;` |
| `prev = cur` 放在 `if(找到)` 里面 | 删头节点时 prev 仍为 NULL，后续迭代逻辑错误 | `prev = cur` 放在循环末尾，条件判断之外 |
| `free(cur)` 在 `prev->next = cur->next` 之前 | `cur->next` 已释放，无法读取 | 先保存 `cur->next` 或先调整指针再 free |
| 空链表调用 `delete_node` 时访问 `cur->data` | 段错误 | 先判断 `head == NULL` 直接返回 |
| 忘记 `return head` | 调用者拿不到可能更新的 head | 删除后必须返回（可能更新的）head |

### 重要知识点

- 前驱指针 `prev` 是单链表删除操作的**必需品**——因为没有 `prev` 就无法"回头看"
- 五种删除边界的区分标志：`prev == NULL`（是否删头）+ `cur->next == NULL`（是否删尾）
- `free` 的时机：**先调整完指针，再释放内存**。顺序不能反
- 删除唯一节点后 head 变成 NULL——调用者必须用返回值更新自己的 head
- 查找/删除的返回值约定：找到返回节点指针，未找到返回 NULL

### 课堂讨论

1. **如果链表是双向链表（有 `prev` 指针），删除会简化吗？** — 会。双向链表的每个节点知道自己前一个是谁，不需要遍历找前驱，任何位置的删除都是 O(1)。

2. **如何删除所有值为 val 的节点（而不是只删第一个）？** — 遍历整个链表，每找到一个就删除。关键是删除后 `cur` 已失效，需要用临时变量保存 `next` 再继续。

3. **查找和删除哪个更易在面试中写出 bug？** — 删除。大部分候选人能写出查找，但删除的边界处理（头节点、尾节点、唯一节点）是扣分重灾区。

4. **为什么 `delete_node` 要返回 `struct node *`？** — C 语言的值传递意味着函数内修改 `head` 形参不会影响调用者的实参。通过返回新 head，调用者用 `head = delete_node(head, val)` 更新。

### 后续衔接

- Lesson 30: 链表构建（前置）
- Lesson 32: 链表反转与环检测（指针操作的高阶应用）
- Lesson 33: 循环链表（删除逻辑在循环链表中更复杂）

### 参考资料

- K&R《C 程序设计语言》§6.7 — 链表实现
- 《数据结构与算法分析 — C 语言描述》§3.2 — 链表 ADT
- `man free` — 内存释放
