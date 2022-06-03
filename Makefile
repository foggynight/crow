PRG := crow

WRN := -Wall -Wextra -Wpedantic
SRC := src/*

.PHONY: all
all:
	gcc -o $(PRG) -O3 $(WRN) $(SRC)

.PHONY: debug
debug:
	gcc -o $(PROG) -g $(WRN) $(SRC)

.PHONY: test
test:
	gcc -o temp test/string.c src/string.c && ./temp && rm temp
