CC = gcc
CFLAGS = -std=gnu99 -Wall -Wextra -Werror -pedantic -pthread

all: clean proj2

proj2:
	$(CC) $(CFLAGS) proj2.c -o proj2

clean:
	rm -rf proj2 proj2.out proj2.zip

test: clean
	$(CC) $(CFLAGS) -DNDEBUG proj2.c -o proj2
