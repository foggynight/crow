SRC := src/main.c src/vector.c

.PHONY: all
all:
	gcc -o crow -O3 $(SRC)

.PHONY: debug
debug:
	gcc -o crow -g $(SRC)
