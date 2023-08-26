CC = gcc
CFLAGS = -Wall -Wextra
SRC_DIR = src

all: server client

server: $(SRC_DIR)/server.c
	$(CC) $(CFLAGS) -o server $(SRC_DIR)/server.c

client: $(SRC_DIR)/client.c
	$(CC) $(CFLAGS) -o client $(SRC_DIR)/client.c

clean:
	rm -f server client
