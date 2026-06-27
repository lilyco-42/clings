---
name: clings-exercise
description: |
  This skill should be used when creating or modifying C programming exercises
  for the clings project (Unit 2 "C Essentials" and beyond). It provides the
  standardized exercise template, file structure conventions, and verification
  workflow. Use this skill whenever the user asks to write a new exercise,
  create exercise files, or follow the clings exercise pattern.
---

# Clings Exercise Development

## Overview

Create C programming exercises for the clings project following a consistent
pattern: a single `.c` template with `#error` blocking, stdin-based I/O, `/* */`
comments, and `exercises.toml`-based test configuration.

## Workflow

### Step 1: Determine Exercise Metadata

For each new exercise, establish:
- **Lesson number** (`NN`): sequential from the unit plan
- **Directory name**: `exercises/<NN>_<snake_name>/`
- **Source file**: `<NN>_<snake_name>.c`
- **order**: sequential across the unit (starts at 73 for unit2 lesson 25)

### Step 2: Create Exercise Files

Create four files per exercise:

1. **`exercises/<NN>_<name>/<NN>_<name>.c`** — Student template
   - Copy from `assets/template.c` and fill placeholders
   - Must contain `#error TODO: ...` inside the function body
   - `main()` handles all I/O (student only fills the core function)
   - Use `fgets` + strip-newline pattern for stdin input
   - Use `/* */` comments exclusively

2. **`exercises/<NN>_<name>/exercises.toml`** — Test configuration
   - Copy from `assets/exercises.toml` and fill placeholders
   - At least 3 test cases covering normal + edge + special
   - Mode is always `"stdout"` for unit2 exercises
   - Hint must include complete algorithm pseudocode and key pitfalls

3. **`exercises/<NN>_<name>/Makefile`** — Build system
   - Copy from `assets/Makefile` and fill placeholders

4. **`exercises/<NN>_<name>/README.md`** — Lesson documentation
   - Title, task, algorithm steps, examples, knowledge points
   - Discussion questions and references

### Step 3: Create Solution Files

Create the solution directory with compiled answer:

1. **`solutions/<NN>_<name>/<NN>_<name>.c`** — Complete working solution
   - Remove `#error`, implement the function
   - Keep all I/O scaffolding identical to the template

2. **`solutions/<NN>_<name>/Makefile`** — Same as exercise Makefile

### Step 4: Verify

After creating files, always verify:

```bash
# 1. Template must fail to compile
cd exercises/<NN>_<name> && gcc -Wall -o /tmp/test <NN>_<name>.c 2>&1
# Expected: #error TODO: ...

# 2. Solution must compile cleanly and pass all tests
cd solutions/<NN>_<name> && gcc -Wall -o <NN>_<name> <NN>_<name>.c
printf "test_input1\n" | ./<NN>_<name>   # Match expected output
printf "test_input2\n" | ./<NN>_<name>   # Match expected output
```

## Template Asset Files

The `assets/` directory contains template files with `<PLACEHOLDERS>` that must
be replaced:

| Asset | Purpose | Key Placeholders |
|-------|---------|-----------------|
| `assets/template.c` | C source template | `<NN>`, `<NAME>`, `<CHINESE_TITLE>`, `<FUNCTION_NAME>`, `<RETURN_TYPE>`, `<PARAMETERS>` |
| `assets/exercises.toml` | Test config | `<NN>`, `<name>`, `<ORDER>`, test cases |
| `assets/Makefile` | Build | `<NN>`, `<name>` |

## Detailed Conventions

For comprehensive reference on all conventions (comments, I/O pattern, test
cases, difficulty ratings, README structure), load `references/exercise-pattern.md`.

Key rules at a glance:
- All comments use `/* */`, never `//`
- Input via `fgets` + manual `\n` stripping
- Output format must match exercises.toml `stdout` exactly (including trailing `\n`)
- `#error TODO: ...` goes at the top of the function body the student must write
- Student writes only the core function; `main()` is always provided
