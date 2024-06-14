CC = gcc
CFLAGS = -Wall -pthread -ljson-c
OBJ = src/main.o src/users/users.o src/operations/operations.o

all: main

main: $(OBJ)
	$(CC) -o $@ $(OBJ) $(CFLAGS)

src/main.o: src/main.c src/users/users.h src/operations/operations.h
	$(CC) -c -o $@ $< $(CFLAGS)

src/users/users.o: src/users/users.c src/users/users.h
	$(CC) -c -o $@ $< $(CFLAGS)

src/operations/operations.o: src/operations/operations.c src/operations/operations.h src/users/users.h
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	rm -f src/*.o src/users/*.o src/operations/*.o main
