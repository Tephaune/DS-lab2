#include "expression.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void usage(const char *prog) {
    printf("usage:\n");
    printf("  %s check \"EXPRESSION\"\n", prog);
    printf("  %s eval  \"POSTFIX EXPRESSION\"\n", prog);
    printf("  %s trace \"POSTFIX EXPRESSION\"\n", prog);
    printf("\nexamples:\n");
    printf("  %s check \"(a + b) * [c - {d / e}]\"\n", prog);
    printf("  %s trace \"8 3 2 * + 5 -\"\n", prog);
}

static int print_result(ExprResult r, bool show_value) {
    if (r.status == EXPR_OK) {
        if (show_value) {
            printf("Result: %ld\n", r.value);
        } else {
            printf("Valid: delimiters are balanced.\n");
        }
        return EXIT_SUCCESS;
    }

    fprintf(stderr, "Error [%s] at position %zu: %s\n", expr_status_name(r.status), r.position,
            r.message);
    return EXIT_FAILURE;
}

int main(int argc, char **argv) {
    if (argc != 3) {
        usage(argv[0]);
        return EXIT_FAILURE;
    }

    if (strcmp(argv[1], "check") == 0) {
        return print_result(check_delimiters(argv[2]), false);
    }
    if (strcmp(argv[1], "eval") == 0) {
        return print_result(eval_postfix(argv[2], false), true);
    }
    if (strcmp(argv[1], "trace") == 0) {
        return print_result(eval_postfix(argv[2], true), true);
    }

    usage(argv[0]);
    return EXIT_FAILURE;
}
