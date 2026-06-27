# Clings Exercise Development Reference

## Conventions

### Naming
- Exercise directory: `exercises/<NN>_<snake_name>/` where `NN` is the lesson number
- Source file: `<NN>_<snake_name>.c` (single file per exercise, no sub-exercises)
- Solution directory: `solutions/<NN>_<snake_name>/`
- Solution file: same name as source

### Comment Style
Always use `/* */` block comments, never `//`. This is C-traditional and works with all standards.

### Template Structure
Every `.c` template must:
1. Start with a block comment: exercise title, task description, knowledge points
2. Include verification examples (stdin → expected output)
3. Have `#include` directives needed
4. Contain `#error TODO: ...` inside the function body that blocks compilation
5. Provide a complete `main()` that handles I/O — the student only fills in the core function

### Input/Output Pattern (Primary: stdin)
```c
char haystack[256];
char needle[256];
fgets(haystack, sizeof(haystack), stdin);
fgets(needle, sizeof(needle), stdin);

/* Strip trailing newline */
int i = 0;
while (haystack[i] && haystack[i] != '\n') i++;
haystack[i] = '\0';
i = 0;
while (needle[i] && needle[i] != '\n') i++;
needle[i] = '\0';
```

Use `stdin = "line1\nline2\n"` in exercises.toml test cases.

### exercises.toml Pattern
```toml
[[exercises]]
name = "<NN>_<name>"
title = "<Chinese title>"
unit = "unit2"
lesson = <NN>
order = <order>
mode = "stdout"
source = "<NN>_<name>.c"
lesson_url = "https://cnb.cool/..."
hint = '''
Algorithm pseudocode with key pitfalls.
'''

[[exercises.cases]]
stdin = "input1\ninput2\n"
stdout = "expected output\n"
```

### Test Case Requirements
- At least 3 cases covering: normal, edge, and special
- One case for "not found" / "not found" scenarios
- One case for empty/null input where applicable

### Makefile Pattern
```makefile
.PHONY: all clean

all: <NN>_<name>

<NN>_<name>: <NN>_<name>.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f <NN>_<name>
```

### README.md Structure
1. Lesson title and task description
2. Algorithm outline (step-by-step)
3. Examples with expected output
4. Key knowledge points
5. Discussion questions
6. Connection to later lessons (if applicable)
7. References

### Difficulty Rating
| Level | When to use |
|-------|-------------|
| ★★☆☆ | Simple algorithm, ~10 lines student code |
| ★★★☆ | Moderate complexity, ~15-20 lines |
| ★★★★ | Multiple sub-steps or tricky edge cases |
| ★★★★★ | Capstone projects (GPS, mybash-level) |

## Verification Checklist
After creating an exercise, verify:
- [ ] Template fails to compile (`#error` triggers)
- [ ] Solution compiles with `gcc -Wall` and no warnings
- [ ] All test cases pass with exact stdout match
- [ ] exercises.toml `name` matches source filename
- [ ] `/* */` comments throughout, no `//`
- [ ] README.md references correct lesson number
