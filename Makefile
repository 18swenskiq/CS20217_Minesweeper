
CC = gcc
CFLAGS = -I. -I..
CLIBS = -lncurses
TARGET = libcs20217.a
OBJS = cs20217.o minesweeper.o

$(TARGET): $(OBJS)
	ar rcs $(TARGET) $^

clean:
	rm -f $(TARGET) $(OBJS)
