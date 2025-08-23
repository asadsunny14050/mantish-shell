CC = gcc 
CFLAGS = -Wall -Wextra -g

SRCS = src/main.c src/shell.c src/parse.c src/utils.c src/operators.c src/built_in.c src/queue.c src/debug.c 

TARGET = mantish

all: $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET)


clean: 
	rm -f $(TARGET)


