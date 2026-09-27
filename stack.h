#ifndef STACK_H
#define STACK_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

// A stack node. The application should NOT manipulate nodes directly.
typedef struct StackNode {
    long value;
    struct StackNode *next;
} StackNode;

// Stack header. Only the nodes are heap allocated.
typedef struct Stack {
    StackNode *top;
    size_t size;
} Stack;

// Create an empty stack.
[[nodiscard]] Stack stack_create(void);

// Free every remaining node and reset the stack to empty.
void stack_destroy(Stack *stack);

// Push one value onto the top. O(1).
// Returns false only if memory allocation fails.
[[nodiscard]] bool stack_push(Stack *stack, long value);

// Remove the top value. O(1).
// On success, stores the removed value in *out and returns true.
// Returns false if the stack is empty; *out is left unchanged.
[[nodiscard]] bool stack_pop(Stack *stack, long *out);

// Read, but do not remove, the top value. O(1).
// Returns false when the stack is empty.
[[nodiscard]] bool stack_peek(const Stack *stack, long *out);

[[nodiscard]] bool stack_is_empty(const Stack *stack);
[[nodiscard]] size_t stack_size(const Stack *stack);

// Helpful for learning/debugging. Prints bottom -> top.
void stack_print(const Stack *stack, FILE *out);

// Representation invariant:
// 1. size equals the number of reachable nodes.
// 2. top is NULL exactly when size is 0.
[[nodiscard]] bool stack_check_invariant(const Stack *stack);

#endif
