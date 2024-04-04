CC = gcc
CFLAGS = -fsanitize=address,undefined,leak -std=c99 -Og -Wall -g
DBFLAGS = -DDEBUG
mysh: mysh.o parser.o
	$(CC) -o $@ $^ $(CFLAGS)
myshdb: mysh.c parser.c
	$(CC) -o $@ $^ $(CFLAGS) $(DBFLAGS)
mysh.o: mysh.c
	$(CC) -c $^ $(CFLAGS)
parser.o: parser.c
	$(CC) -c parser.c $(CFLAGS)
clean: mysh myshdb
	rm -f *.o $^ 

#lets see if kareem made a functioning makefile
