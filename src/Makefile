.PHONY: build clean

CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -MMD -MP $(shell pkg-config --cflags cairo)
LDLIBS = $(shell pkg-config --libs cairo) -lm

exec = kover
modules = scene antenna construction parsing validation draw
objects = $(exec).o $(patsubst %,%.o,$(modules))

build: $(exec)

$(exec): $(objects)
	$(CC) $^ -o $@ $(LDLIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(objects) $(objects:.o=.d) $(exec)

-include $(objects:.o=.d)
