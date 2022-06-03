SRC := src/lexer.c src/main.c src/string.c src/token.c src/vector.c

.PHONY: all
all:
	gcc -o crow -O3 $(SRC)

.PHONY: debug
debug:
	gcc -o crow -g $(SRC)

.PHONY: test
test:
	gcc -o temp test/string.c src/string.c && ./temp && rm temp
