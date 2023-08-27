CC = gcc
CFLAGS = -Wall -Wextra
SRC_DIR = src

#change the binaries' name(s) ass needed
all: server client showip

server: $(SRC_DIR)/server.c
	$(CC) $(CFLAGS) -o bin/server $(SRC_DIR)/server.c

client: $(SRC_DIR)/client.c
	$(CC) $(CFLAGS) -o bin/client $(SRC_DIR)/client.c

showip: $(SRC_DIR)/showip.c
	$(CC) $(CFLAGS) -o bin/showip $(SRC_DIR)/showip.c


clean:
	rm -f bin/server bin/client bin/showip
	
