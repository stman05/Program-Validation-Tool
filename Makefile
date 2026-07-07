#Author: Stefanos Manos
CC = gcc
CFLAGS = -Wall -g
.PHONY : clean

hw4 : hw4.c p4diff
	$(CC) $(CFLAGS) $< -o $@

p4diff : p4diff.c
	$(CC) $(CFLAGS) $< -o $@

clean :
	rm hw4 p4diff
