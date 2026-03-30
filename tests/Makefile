CC      = gcc
CFLAGS  = -Wall -Wextra -O2 -Isrc -lm
SRC     = src/main.c
TEST    = tests/test_all.c

.PHONY: all run test clean

all: raytracer

raytracer: $(SRC)
	$(CC) $(CFLAGS) -o raytracer $(SRC) -lm

run: raytracer
	./raytracer

test:
	$(CC) $(CFLAGS) -o test_runner $(TEST) -lm
	./test_runner

clean:
	rm -f raytracer test_runner output/*.ppm
