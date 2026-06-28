---
name: clings-readme
description: |
  This skill should be used when writing or rewriting README.md files for clings
  programming exercises. It provides the standardized section structure, quality
  standards (minimum 250 lines, no upper limit, ASCII diagrams, tables), content guidelines, and
  anti-patterns to avoid. Use this skill whenever the user asks to write a new
  README, expand an existing README, or ensure README quality.
---

# Clings README Development

## Overview

Write educational README.md files for clings C programming exercises. Every README
must be at least 250 lines (no upper limit), packed with ASCII diagrams, comparison
tables, step-by-step traces, and discussion questions. The goal is complete knowledge
coverage — every concept explained in depth so students can fully understand the topic.
No answer code allowed.

## Workflow

### Step 1: Read Context Files

Before writing, read:

- The exercise's `.c` template — to understand the task and given helpers
- The exercise's `exercises.toml` — for test cases and expected behavior
- The gold standard reference: `exercises/27_my_memmove/README.md` (301 lines) — for structure and depth
- Another detailed example: `exercises/29_eight_queens/README.md` — for diagrams and tables

### Step 2: Write Following the Mandatory Structure

Every README must include ALL 11 sections in this order:

1. **Title & Course Task** — what the student implements, with verification examples
2. **Prerequisite Knowledge** — foundational concepts with ASCII diagrams
3. **Algorithm Deep Dive** — step-by-step with ASCII art and labeled pointer diagrams
4. **Complete Step-by-Step Trace** — table format showing state at each step
5. **Edge Case Analysis** — table of scenarios and expected behaviors
6. **Comparison Tables** — related concepts side by side
7. **Common Mistakes** — table with mistake / consequence / correct approach (5+ entries)
8. **Important Knowledge Points** — 5-8 bullet insights
9. **Classroom Discussion** — 5-6 numbered questions with multi-sentence answers
10. **Follow-up Connections** — prerequisite and next lessons
11. **References** — man pages, textbooks, Wikipedia

### Step 3: Apply Quality Standards

- **Minimum 250 lines** (no upper limit — write as much as needed to cover all knowledge points)
- ASCII diagrams use: `┌─┐│└┘├┤┬┴┼` and `→ ← ↑ ↓`
- Tables use pipe syntax with header separator
- Every table has at least 3 rows of data
- Code examples are in ```c blocks with `/* */` comments
- Chinese with English technical terms inline
- Cover ALL knowledge points — do not skip any concept related to the topic
- Each concept should be explained in enough detail for a student to fully understand it

### Step 4: Verify

After writing, check:

- [ ] All 11 sections present and non-empty
- [ ] At least 1 ASCII diagram with labeled pointers
- [ ] At least 2 comparison/analysis tables
- [ ] Common mistakes table has 5+ entries
- [ ] Discussion section has 5+ questions with detailed answers
- [ ] No complete answer code (hint-style only)
- [ ] Total lines ≥ 250

## Detailed Conventions

For the full section-by-section specification, content rules, prohibited patterns,
and style guidelines, load `references/readme-template.md`.

## Anti-Patterns to Avoid

- Do NOT include complete function implementations
- Do NOT use `//` comments (use `/* */`)
- Do NOT write single-line sections
- Do NOT skip the edge case analysis table
- Do NOT write discussion questions without answers
