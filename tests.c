#include "expression.h"
#include "stack.h"

#include <assert.h>
#include <stdio.h>

static void test_stack(void) {
    Stack s = stack_create();
    assert(stack_is_empty(&s));
    assert(stack_size(&s) == 0);
    assert(stack_check_invariant(&s));

    assert(stack_push(&s, 10));
    assert(stack_push(&s, 20));
    assert(stack_push(&s, 30));
    assert(stack_size(&s) == 3);
    assert(stack_check_invariant(&s));

    long value = 0;
    assert(stack_peek(&s, &value) && value == 30);
    assert(stack_pop(&s, &value) && value == 30);
    assert(stack_pop(&s, &value) && value == 20);
    assert(stack_pop(&s, &value) && value == 10);
    assert(!stack_pop(&s, &value));
    assert(stack_is_empty(&s));
    assert(stack_check_invariant(&s));

    stack_destroy(&s);
}

static void test_delimiters(void) {
    assert(check_delimiters("(a + b) * [c - {d / e}]").status == EXPR_OK);
    assert(check_delimiters("([{}])").status == EXPR_OK);
    assert(check_delimiters("([)]").status == EXPR_MISMATCHED_DELIMITER);
    assert(check_delimiters("a + b)").status == EXPR_UNMATCHED_CLOSER);
    assert(check_delimiters("((a + b)").status == EXPR_UNCLOSED_OPENER);
}

static void test_postfix(void) {
    ExprResult r = eval_postfix("8 3 2 * + 5 -", false);
    assert(r.status == EXPR_OK && r.value == 9);

    r = eval_postfix("20 5 / 3 +", false);
    assert(r.status == EXPR_OK && r.value == 7);

    r = eval_postfix("10 4 -", false);
    assert(r.status == EXPR_OK && r.value == 6); // catches operand-order bug

    assert(eval_postfix("2 +", false).status == EXPR_TOO_FEW_OPERANDS);
    assert(eval_postfix("2 3", false).status == EXPR_TOO_MANY_OPERANDS);
    assert(eval_postfix("4 0 /", false).status == EXPR_DIVIDE_BY_ZERO);
    assert(eval_postfix("2 hello +", false).status == EXPR_INVALID_TOKEN);
}

int main(void) {
    test_stack();
    test_delimiters();
    test_postfix();
    puts("All tests passed.");
    return 0;
}
