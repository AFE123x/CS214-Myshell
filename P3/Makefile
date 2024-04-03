CC = gcc
CFLAGS = -fsanitize=address,undefined,leak -std=c99 -Og -Wall -g

mysh: mysh.o parser.o
	$(CC) -o $@ $^ $(CFLAGS)
mysh.o: mysh.c
	$(CC) -c $^ $(CFLAGS)
parser.o: parser.c
	$(CC) -c parser.c $(CFLAGS)
clean:
	rm *.o $^

#lets see if kareem made a functioning makefile