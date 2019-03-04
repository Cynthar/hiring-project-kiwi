CFLAGS=-std=c99 -pedantic -Wall -Werror -iquote includes -c -o /dev/null

all: gcc clang

gcc:
	gcc $(CFLAGS) src/sensor.c
	gcc $(CFLAGS) src/gateway.c

clang:
	clang $(CFLAGS) src/sensor.c
	clang $(CFLAGS) src/gateway.c

Weverything:
	clang $(CFLAGS) -Weverything -Wno-error src/sensor.c
	clang $(CFLAGS) -Weverything -Wno-error src/gateway.c

.PHONY: all gcc clang Weverything
