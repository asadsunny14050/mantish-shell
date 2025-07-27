CC = gcc 
CFLAGS = -Wall -Wextra -g

SRCS = src/main.c

TARGET = mantish

all: $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET)


clean: 
	rm -f $(TARGET)


