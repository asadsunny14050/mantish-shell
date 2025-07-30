CC = gcc 
CFLAGS = -Wall -Wextra -g

SRCS = src/main.c src/debug_functions.c src/shell.c src/parse.c

TARGET = mantish

all: $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET)


clean: 
	rm -f $(TARGET)


