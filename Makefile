PRG := crow

WRN := -Wall -Wextra -Wpedantic
SRC := src/lexer.c src/main.c src/string.c src/token.c src/vector.c

.PHONY: all
all:
	gcc -o $(PRG) -O3 $(WRN) $(SRC)

.PHONY: debug
debug:
	gcc -o $(PROG) -g $(WRN) $(SRC)

.PHONY: test
test:
	gcc -o temp test/string.c src/string.c && ./temp && rm temp
