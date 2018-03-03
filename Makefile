# don't modify this file.
# if you want your warn/errors to be less verbose,
# remove -Wextra (and maybe -Wall) but note that we'll
# compile with all CFLAGS listed here.

CC=g++
DEBUG=-g
CFLAGS=-Wall -O3 -Wextra -std=c++11
EXEC=imdb

build:
	$(CC) $(CFLAGS) -g *.cpp -o $(EXEC)

run:
	./$(EXEC)

clean:
	rm $(EXEC)

.PHONY: build run clean
