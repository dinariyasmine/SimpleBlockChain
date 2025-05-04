CC = gcc
CFLAGS = -Wall -Wextra -g -lssl -lcrypto -lpthread

SOURCES = main.c blockchain.c block.c transaction.c merkle.c utils.c tests.c ui.c
OBJECTS = $(SOURCES:.c=.o)
EXECUTABLE = blockchain_app

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(CFLAGS)

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)
