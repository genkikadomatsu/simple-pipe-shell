CC = gcc
LD = $(CC)
CPPFLAGS = -g -std=gnu11 -Wpedantic -Wall
CFLAGS = -I.
LDFLAGS =
LDLIBS =
PROGRAM = shell
SRCS = shell.c tokenizer.c  pipeline.c
OBJS = $(SRCS:.c=.o)

all: $(PROGRAM)

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

$(PROGRAM): $(OBJS)
	$(LD) $(LDFLAGS) $(LDLIBS) -o $@ $(OBJS)

.PHONY: clean

clean:
	rm -f $(OBJS) $(PROGRAM)

