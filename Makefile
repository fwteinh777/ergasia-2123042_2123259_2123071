CC = gcc
CFLAGS = -Wall -Wextra -g
OBJ = main.o

program: $(OBJ)
	$(CC) $(CFLAGS) -o program $(OBJ)

main.o: main.c catalog.h
	$(CC) $(CFLAGS) -c main.c

clean:
	rm -f *.o program