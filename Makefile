CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS = -lm

SRCDIR = src
SRCS = $(SRCDIR)/main.c dist/argtable3.c
OBJS = $(SRCS:.c=.o)
EXEC = $(SRCDIR)/justify

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(EXEC) $(LDFLAGS)
	rm -f $(SRCDIR)/main.o
	@echo "I WARNINGS VISUALIZZATI NON IMPATTANO SUL COMPORTAMENTO DEL PROGRAMMA!"

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(EXEC) $(SRCDIR)/main.o
