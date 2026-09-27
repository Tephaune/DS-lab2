CC ?= gcc
CFLAGS := -std=c23 -Wall -Wextra -Wpedantic -Wconversion -Werror -g
SRC := stack.c expression.c

.PHONY: all run test sanitize valgrind format format-check tidy clean

all: exprlab

exprlab: main.c $(SRC) stack.h expression.h
	$(CC) $(CFLAGS) main.c $(SRC) -o $@

tests: tests.c $(SRC) stack.h expression.h
	$(CC) $(CFLAGS) tests.c $(SRC) -o $@

run: exprlab
	./exprlab trace "8 3 2 * + 5 -"

test: tests
	./tests

sanitize:
	$(CC) $(CFLAGS) -fsanitize=address,undefined -fno-omit-frame-pointer tests.c $(SRC) -o tests-san
	./tests-san

valgrind: tests
	valgrind --leak-check=full --show-leak-kinds=all --errors-for-leak-kinds=all --error-exitcode=1 ./tests

format:
	clang-format -i *.c *.h

format-check:
	clang-format --dry-run --Werror *.c *.h

tidy:
	clang-tidy main.c tests.c stack.c expression.c -- $(CFLAGS)

clean:
	rm -f exprlab tests tests-san
