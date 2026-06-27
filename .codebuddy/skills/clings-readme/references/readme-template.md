# Clings Exercise README Template & Conventions

## Length Standard

**Minimum 250 lines**, no upper limit. Write as much as needed to cover every knowledge point thoroughly. The goal is complete understanding — every concept must be explained in enough depth for a student to fully grasp it. Do not pad with filler, but do not cut corners either. If a complex topic (like memmove overlapping, BST deletion, or quicksort partitioning) needs 350+ lines to explain properly, write 350+ lines.

## Mandatory Sections (in order)

### 1. Title & Course Task (课程任务)
- One-sentence title
- Bullet list of sub-tasks
- Function signatures if applicable
- Verification input/output examples

### 2. Prerequisite Knowledge (前置知识)
- Explain foundational concepts students must understand
- Use ASCII diagrams for data structures (memory layout, struct fields, pointer relationships)
- Define terminology (e.g. "LIFO", "FIFO", "self-loop")
- Contrast with related concepts (e.g. "circular vs linear queue")

### 3. Algorithm Deep Dive (算法详解)
- **MUST include ASCII art diagrams** showing step-by-step process
- Use labeled pointers/anchor markers (e.g. `↑ prev`, `↑ curr`, `↑ next`)
- Show initial state → intermediate steps → final state
- Explain WHY each step works, not just WHAT it does
- Use tables for parallel comparisons (e.g. head insert vs tail insert)

### 4. Complete Step-by-Step Trace (逐步跟踪)
- Walk through at least one complete example from start to finish
- Use a table format with columns: Step | Operation | State | Notes
- Show data structure state changes at each step
- Highlight edge cases encountered along the way

### 5. Edge Case Analysis Table
- Table with columns: Scenario | Expected Behavior | Why
- Cover: empty/null input, single element, maximum values, boundary conditions

### 6. Comparison Tables
- Compare the current topic with related concepts
- Examples: "memcpy vs memmove", "DFS vs BFS", "head insert vs tail insert"
- Columns: Dimension | Option A | Option B

### 7. Common Mistakes Table (常见错误)
- Table with columns: Mistake | Consequence | Correct Approach
- Minimum 4 entries, ideally 5-6
- Include real error code snippets in the Mistake column
- Consequences should explain the actual failure mode (segfault, memory leak, wrong output)

### 8. Important Knowledge Points (重要知识点)
- 5-8 concise bullet points
- Each point is a standalone insight, not obvious from code alone
- Focus on "why" and "when", not "what"

### 9. Classroom Discussion (课堂讨论)
- 5-6 numbered questions with detailed multi-sentence answers
- Questions should be thought-provoking, not trivial
- Mix of: design tradeoffs, alternative approaches, real-world applications, edge case exploration
- Each answer should be 2-4 sentences with concrete examples

### 10. Follow-up Connections (后续衔接)
- Link to prerequisite lessons
- Link to next/related lessons
- Brief explanation of how this lesson's concept is reused

### 11. References (参考资料)
- 2-4 references
- Mix of: man pages, standard textbooks, Wikipedia, real-world implementations

## Prohibited Content

- **NO complete answer code** — maximum is 1-2 line hints or pseudocode fragments
- **NO `// TODO:` or `#error` directives** — these belong in .c templates, not READMEs
- **NO single-line sections** — every section must have substantive content

## Style Guidelines

- Use `/* */` style for all C code examples in READMEs (matching project convention)
- Use ASCII art with box-drawing characters: `┌─┐│└┘├┤┬┴┼`
- Use arrow characters for pointers: `→ ← ↑ ↓`
- Tables use pipe syntax: `| Col1 | Col2 |`
- Code blocks use triple backticks with `c` language tag
- Chinese mixed with English technical terms (e.g. "LIFO 特性")
