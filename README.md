# CMPS2131 — Lab 2: Stacks as an Expression Engine

This lab is about **learning why a stack is useful**, not fighting with parsing code.
You are given the command-line program, tokenization code, error types, tests, printing helpers,
and most of the application structure. Your job is to implement the small pieces where **LIFO
reasoning** matters.

You will build two applications with the same linked stack:

1. a **delimiter checker** for `()`, `[]`, and `{}`; and
2. a **postfix calculator** for integer arithmetic.

The important question throughout the lab is:

> **What unfinished work must I remember, and why must I retrieve the most recent item first?**

---

## C language standard for this lab

This course uses **C23**. All code in this lab should be compiled as C23, not as C17 or the older provisional `c2x` mode. The supplied `Makefile` already enforces:

```sh
-std=c23
```

The code intentionally uses **conservative modern C23**. The goal of this lab is to learn stacks, not to collect language features. You will see useful modern practices such as:

- `[[nodiscard]]` on functions whose return value should be checked;
- `bool` for true/false results;
- `size_t` for sizes and indexes;
- designated initializers where they make initialization clearer;
- `const` where a function should not modify an object;
- enums for named status/error values; and
- explicit ownership of dynamically allocated nodes.

Do not replace these with older C idioms. At the same time, do not add advanced C23 features merely for novelty. **C23 is our normal programming environment; the stack is the topic.**

To verify that your compiler is using the expected standard, run:

```sh
make clean
make
```

If compilation succeeds, the project has been built with the course's required C23 flags.

---

## 1. Learning goals

By the end of the lab you should be able to:

- explain Last-In, First-Out (LIFO) in terms of an actual algorithm;
- implement `push`, `pop`, and `peek` on a linked stack;
- trace the `top` pointer as nodes are added and removed;
- state and check a stack representation invariant;
- use a stack to match nested delimiters;
- use a stack to evaluate postfix expressions;
- explain why operand order matters for subtraction and division;
- verify your C code with tests, sanitizers, and Valgrind.

---

## 2. What is already done for you

This is **not** a blank-page lab.

| File | Edit? | Purpose |
|---|---|---|
| `stack.h` | No | Public stack contract and invariant |
| `stack.c` | **Yes** | Four short stack TODOs |
| `expression.h` | No | Application result/error types |
| `expression.c` | **Yes** | Delimiter + postfix algorithm TODOs; parsing helpers are given |
| `main.c` | No | Finished command-line interface |
| `tests.c` | No | Tests that tell you when each stage works |
| `Makefile` | No | Build/test/debug workflow |

You are **not** expected to write a lexer or parser. `expression.c` already splits the postfix
input into whitespace-separated tokens and already contains helpers for recognizing numbers and
operators.

---

## 3. First, picture the linked stack

If we push `10`, then `20`, then `30`:

```text
                 top
                  |
                  v
              +------+------+
              |  30  | next | ----+
              +------+------+     |
                                    v
                              +------+------+
                              |  20  | next | ----+
                              +------+------+     |
                                                    v
                                              +------+------+
                                              |  10  | NULL |
                                              +------+------+

size = 3
```

`30` must be removed first because it was added last.

### Push algorithm

To push `value`:

```text
1. allocate a new node
2. store value in it
3. new node points to the old top
4. top moves to the new node
5. size increases
```

The crucial pointer update is:

```text
new_node->next = stack->top;
stack->top = new_node;
```

Do it in that order. If you overwrite `top` first, you lose the rest of the stack.

### Pop algorithm

To pop:

```text
1. reject an empty stack
2. save the current top node in temp
3. copy temp's value to the caller
4. move top to temp->next
5. free temp
6. decrease size
```

The temporary pointer matters because after `top` moves, you still need an address for the node
that must be freed.

---

## 4. Part A — implement the Stack ADT

Open `stack.c`. Complete **TODO 1–4** in this order:

1. `stack_push`
2. `stack_pop`
3. `stack_peek`
4. `stack_check_invariant`

Then run:

```sh
make test
```

The expression tests will still fail, but the first stack test should pass once this section is
correct.

### The invariant

A valid stack must always satisfy both rules:

```text
number of nodes reachable from top == stack->size

and

top == NULL  exactly when  size == 0
```

Think of an invariant as a promise that every public operation must preserve.

---

## 5. Part B — delimiter checking

Consider:

```text
(a + b) * [c - {d / e}]
```

When you see an opening delimiter, you do **not** yet know where its matching closer is. It is
unfinished work, so remember it on the stack.

```text
character     action                  stack
------------------------------------------------
(             push '('                [(]
[             push '['                [(, []
{             push '{'                [(, [, {]
}             pop '{'                 [(, []
]             pop '['                 [(]
)             pop '('                 []
```

Why a stack? Because nested delimiters close in the **reverse order** in which they opened.

### Delimiter algorithm

```text
create empty stack

for each character c:
    if c is an opener:
        push c

    if c is a closer:
        if stack is empty:
            error: unmatched closer

        opener = pop

        if opener is not the matching opener for c:
            error: mismatched delimiter

when input ends:
    if stack is not empty:
        error: unclosed opener

otherwise:
    valid
```

The helper functions `is_opener`, `is_closer`, and `expected_opener` are already written for you.
Complete **TODO 5a–5c** only.

Try these manually:

```sh
./exprlab check "(a + b) * [c - {d / e}]"
./exprlab check "([)]"
./exprlab check "a + b)"
./exprlab check "((a + b)"
```

Before running them, predict which one should produce each error.

---

## 6. Part C — postfix calculator

Most arithmetic notation is **infix**:

```text
8 + 3
```

The operator is between its operands.

In **postfix** notation, the operator comes after its operands:

```text
8 3 +
```

This is useful for learning stacks because no precedence rules or parentheses are needed.

### Example

Evaluate:

```text
8 3 2 * + 5 -
```

Trace it by hand:

```text
token       action                             stack
-------------------------------------------------------
8           push 8                             [8]
3           push 3                             [8, 3]
2           push 2                             [8, 3, 2]
*           pop 2, pop 3, push 3*2             [8, 6]
+           pop 6, pop 8, push 8+6             [14]
5           push 5                             [14, 5]
-           pop 5, pop 14, push 14-5           [9]
```

The final answer is `9`.

### Postfix algorithm

```text
create empty operand stack

for each token:
    if token is an integer:
        push it

    otherwise if token is an operator:
        right = pop
        left  = pop

        if either pop fails:
            error: too few operands

        result = left operator right
        push result

    otherwise:
        error: invalid token

at the end:
    if stack has exactly one value:
        that value is the answer
    if stack has zero values:
        error: no result
    if stack has more than one value:
        error: unused operands remain
```

### The operand-order trap

For:

```text
10 4 -
```

`4` is on top, so it is popped first. But the answer is:

```text
10 - 4 = 6
```

Therefore:

```c
right = pop();
left  = pop();
result = left - right;
```

Do **not** write `right - left`.

Complete **TODO 6a–6c** in `expression.c`.

---

## 7. Use trace mode while learning

The program can print the operand stack after every token:

```sh
make
./exprlab trace "8 3 2 * + 5 -"
```

Expected shape:

```text
8          [8]
3          [8, 3]
2          [8, 3, 2]
*          [8, 6]
+          [14]
5          [14, 5]
-          [9]
Result: 9
```

If your answer is wrong, do not immediately change code. Compare the **first row where your
stack differs** from the expected trace. That is usually where the bug is.

---

## 8. Errors your program must detect

Your completed code should distinguish these cases:

```text
([)]          mismatched delimiter
abc)          unmatched closing delimiter
((abc)        unclosed opening delimiter

2 +           too few operands
2 3           too many operands
4 0 /         divide by zero
2 hello +     invalid token
```

Error handling is part of the algorithm, not an optional extra.

---

## 9. Workflow

Build:

```sh
make
```

Run the demonstration:

```sh
make run
```

Run all tests:

```sh
make test
```

Compile with Clang too:

```sh
make clean
make CC=clang test
```

Check memory safety:

```sh
make sanitize
make valgrind
```

Format and check style:

```sh
make format
make format-check
make tidy
```

A correct submission must not leak stack nodes, including on **error paths**. For example, if
postfix evaluation discovers an invalid token after pushing five values, those five nodes still
have to be released before the function returns.

---

## 10. Suggested work plan

Do not try to finish everything at once.

### Checkpoint 1 — 20–30 minutes

Implement `push`, `pop`, and `peek`. Draw the pointer changes before coding them.

### Checkpoint 2 — 15 minutes

Implement the invariant and use the tests to verify the core stack.

### Checkpoint 3 — 25–35 minutes

Implement the delimiter checker. Test valid nesting, wrong nesting, extra closers, and missing
closers.

### Checkpoint 4 — 35–45 minutes

Implement postfix evaluation. Start with only addition:

```text
2 3 +
```

Then try subtraction to verify operand order:

```text
10 4 -
```

Then try a longer expression:

```text
8 3 2 * + 5 -
```

### Checkpoint 5 — 20 minutes

Run tests, sanitizer, Valgrind, and formatting checks. Fix one failure at a time.

---

## 11. What to submit

Submit:

1. your completed `stack.c`;
2. your completed `expression.c`; and
3. a short reflection (about half a page) answering:

> In the delimiter checker and postfix evaluator, what does each stack element represent? Why
> does each algorithm need the **most recently pushed** item rather than the oldest one? Use one
> concrete example from each application.

Do not merely define LIFO. Explain why LIFO is required by the two algorithms.

---

## 12. Definition of done

- [ ] `make test` prints `All tests passed.`
- [ ] `make CC=clang test` also passes with zero warnings.
- [ ] `make sanitize` reports no errors.
- [ ] `make valgrind` reports no leaks or invalid accesses.
- [ ] `make format-check` passes.
- [ ] `./exprlab check "(a + b) * [c - {d / e}]"` reports valid.
- [ ] `./exprlab trace "8 3 2 * + 5 -"` finishes with `Result: 9`.
- [ ] Reflection completed in your own words.

---

## 13. Challenge extensions — optional

Only after the required lab works:

- Add exponentiation `^`.
- Report the exact opener that was left unclosed.
- Add a `--trace-check` mode for delimiter checking.
- Convert a small infix expression to postfix. This is a substantially harder algorithm and is
  **not** required for this lab.
