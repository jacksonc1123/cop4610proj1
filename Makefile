# Project 1 makefile

SHELL = /bin/sh
CC = gcc
CFLAGS = -g
ALL_CFLAGS = -std=c99 -Wall -Wextra -Werror

.SUFFIXES: .c .o

TARGETS=tinysh
OBJ=shell.o

tinysh: $(OBJ)
	$(CC) -o tinysh $(OBJ) $(CFLAGS) $(ALL_CFLAGS)

shell.o: shell.c

clean:
	rm tinysh $(OBJ)

run:
	./tinysh

runb:
	./tinysh batchfile.file
