CC = gcc
CFLAGS = -Wall -Wextra -pedantic -O3

BIN = vigenere

%.o: %.c
	$(CC) $(CFLAGS) -c $^

$(BIN): *.o
	$(CC) $(CFLAGS) -o $@ $^

.PHONY: clean
clean:
	rm *.o
	rm $(BIN)
