#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <stdbool.h>
#include <stddef.h>

typedef enum ExprStatus {
    EXPR_OK = 0,
    EXPR_UNMATCHED_CLOSER,
    EXPR_UNCLOSED_OPENER,
    EXPR_MISMATCHED_DELIMITER,
    EXPR_INVALID_TOKEN,
    EXPR_TOO_FEW_OPERANDS,
    EXPR_TOO_MANY_OPERANDS,
    EXPR_DIVIDE_BY_ZERO,
    EXPR_OUT_OF_MEMORY
} ExprStatus;

typedef struct ExprResult {
    ExprStatus status;
    long value;           // meaningful for successful postfix evaluation
    size_t position;      // character position for delimiter/token errors
    char message[160];    // short human-readable explanation
} ExprResult;

// Check (), [], and {} in any ordinary text/expression.
[[nodiscard]] ExprResult check_delimiters(const char *text);

// Evaluate a SPACE-SEPARATED postfix expression containing integer literals
// and + - * / % operators. Example: "8 3 2 * + 5 -" => 9.
[[nodiscard]] ExprResult eval_postfix(const char *expression, bool trace);

const char *expr_status_name(ExprStatus status);

#endif
