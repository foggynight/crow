PRG := crow
WRN := -Wall -Wextra -Wpedantic
SRC := src/*.c
INSTALL := /usr/local/bin

.PHONY: all
all:
	gcc -o $(PRG) -O3 $(WRN) $(SRC)

.PHONY: debug
debug:
	gcc -o $(PRG) -g $(WRN) $(SRC)

.PHONY: install
install:
	cp $(PRG) $(INSTALL)

.PHONY: uninstall
uninstall:
	rm $(INSTALL)/$(PRG)

.PHONY: test
test:
	gcc -o temp test/string.c src/string.c && ./temp && rm temp
