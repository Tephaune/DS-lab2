#include "expression.h"
#include "stack.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static ExprResult result(ExprStatus status, long value, size_t position, const char *message) {
    ExprResult r = {.status = status, .value = value, .position = position, .message = ""};
    if (message != NULL) {
        (void)snprintf(r.message, sizeof r.message, "%s", message);
    }
    return r;
}

static bool is_opener(char c) { return c == '(' || c == '[' || c == '{'; }
static bool is_closer(char c) { return c == ')' || c == ']' || c == '}'; }

static char expected_opener(char closer) {
    switch (closer) {
    case ')': return '(';
    case ']': return '[';
    case '}': return '{';
    default: return '\0';
    }
}

static bool is_operator_token(const char *token) {
    return token[0] != '\0' && token[1] == '\0' && strchr("+-*/%", token[0]) != NULL;
}

static bool parse_long_token(const char *token, long *out) {
    if (token == NULL || *token == '\0') return false;
    char *end = NULL;
    long value = strtol(token, &end, 10);
    if (*end != '\0') return false;
    *out = value;
    return true;
}

static bool apply_operator(char op, long left, long right, long *out) {
    switch (op) {
    case '+': *out = left + right; return true;
    case '-': *out = left - right; return true;
    case '*': *out = left * right; return true;
    case '/':
        if (right == 0) return false;
        *out = left / right;
        return true;
    case '%':
        if (right == 0) return false;
        *out = left % right;
        return true;
    default: return false;
    }
}

const char *expr_status_name(ExprStatus status) {
    switch (status) {
    case EXPR_OK: return "OK";
    case EXPR_UNMATCHED_CLOSER: return "UNMATCHED_CLOSER";
    case EXPR_UNCLOSED_OPENER: return "UNCLOSED_OPENER";
    case EXPR_MISMATCHED_DELIMITER: return "MISMATCHED_DELIMITER";
    case EXPR_INVALID_TOKEN: return "INVALID_TOKEN";
    case EXPR_TOO_FEW_OPERANDS: return "TOO_FEW_OPERANDS";
    case EXPR_TOO_MANY_OPERANDS: return "TOO_MANY_OPERANDS";
    case EXPR_DIVIDE_BY_ZERO: return "DIVIDE_BY_ZERO";
    case EXPR_OUT_OF_MEMORY: return "OUT_OF_MEMORY";
    }
    return "UNKNOWN";
}

ExprResult check_delimiters(const char *text) {
    (void)expected_opener; // referenced by TODO 5b once you implement it
    Stack stack = stack_create();

    // CORE IDEA:
    // Opening delimiters are unfinished work, so PUSH them.
    // A closing delimiter must match the MOST RECENT unfinished opener, so POP.
    for (size_t i = 0; text[i] != '\0'; i++) {
        char c = text[i];
        if (is_opener(c)) {
            // TODO 5a: push this opener. Store the char as a long.
            // If stack_push fails: destroy the stack and return EXPR_OUT_OF_MEMORY.
        } else if (is_closer(c)) {
            // TODO 5b:
            // 1. pop the most recent opener into a long variable
            // 2. if pop fails, this closer has no opener -> EXPR_UNMATCHED_CLOSER
            // 3. compare the popped opener with expected_opener(c)
            // 4. if different -> EXPR_MISMATCHED_DELIMITER
            // IMPORTANT: destroy the stack before every early return.
        }
    }

    // TODO 5c: if the stack is not empty, at least one opener was never closed.
    // Return EXPR_UNCLOSED_OPENER. Otherwise return EXPR_OK.
    stack_destroy(&stack);
    return result(EXPR_OK, 0, 0, "balanced");
}

ExprResult eval_postfix(const char *expression, bool trace) {
    (void)apply_operator; // referenced by TODO 6b once you implement it
    Stack operands = stack_create();

    // GIVEN TOKENIZATION: strtok splits the copy at whitespace. This keeps the
    // lab focused on the stack algorithm rather than on writing a lexer.
    char *copy = malloc(strlen(expression) + 1);
    if (copy == NULL) return result(EXPR_OUT_OF_MEMORY, 0, 0, "could not copy expression");
    strcpy(copy, expression);

    size_t token_number = 0;
    for (char *token = strtok(copy, " \t\r\n"); token != NULL; token = strtok(NULL, " \t\r\n")) {
        token_number++;
        long number = 0;

        if (parse_long_token(token, &number)) {
            // TODO 6a: numbers are operands waiting to be used -> PUSH number.
            // Handle allocation failure by freeing copy, destroying the stack,
            // and returning EXPR_OUT_OF_MEMORY.
        } else if (is_operator_token(token)) {
            // TODO 6b: an operator consumes the TWO most recent operands.
            // Pop RIGHT first, then LEFT. This order matters for '-' '/' '%'.
            long right = 0;
            long left = 0;
            long computed = 0;

            // If either pop fails, return EXPR_TOO_FEW_OPERANDS after cleanup.
            // Then call apply_operator(token[0], left, right, &computed).
            // If it returns false, this lab treats that as divide/modulo by zero.
            // Finally PUSH computed back: it may be needed by a later operator.
            (void)right;
            (void)left;
            (void)computed;
        } else {
            char msg[160];
            (void)snprintf(msg, sizeof msg, "'%s' is not an integer or supported operator", token);
            free(copy);
            stack_destroy(&operands);
            return result(EXPR_INVALID_TOKEN, 0, token_number, msg);
        }

        if (trace) {
            printf("%-10s ", token);
            stack_print(&operands, stdout);
            putchar('\n');
        }
    }

    free(copy);

    // TODO 6c: a valid postfix expression must finish with EXACTLY one value.
    // * 0 values => too few operands / empty expression
    // * more than 1 => too many operands
    // * exactly 1 => pop it as the final answer, destroy stack, return EXPR_OK

    stack_destroy(&operands);
    return result(EXPR_TOO_FEW_OPERANDS, 0, token_number, "expression did not produce one result");
}
