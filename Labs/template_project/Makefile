CC = gcc
CFLAGS = -Wall -Wextra
SRC_DIR = src

#change the binaries' name(s) ass needed
all: server client

server: $(SRC_DIR)/server.c
	$(CC) $(CFLAGS) -o bin/server $(SRC_DIR)/server.c

client: $(SRC_DIR)/client.c
	$(CC) $(CFLAGS) -o bin/client $(SRC_DIR)/client.c

clean:
	rm -f bin/server bin/client
	
