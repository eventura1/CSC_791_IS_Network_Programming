CC = gcc
CFLAGS = -Wall -Wextra
SRC_DIR = ../src
BIN_DIR = ../bin


all: server client showip

server: $(SRC_DIR)/server.c
	$(CC) $(CFLAGS) -o $(BIN_DIR)/server $(SRC_DIR)/server.c

server_loop: $(SRC_DIR)/server_loop.c
	$(CC) $(CFLAGS) -o $(BIN_DIR)/server_loop $(SRC_DIR)/server_loop.c

client: $(SRC_DIR)/client.c
	$(CC) $(CFLAGS) -o $(BIN_DIR)/client $(SRC_DIR)/client.c

showip: $(SRC_DIR)/showip.c
	$(CC) $(CFLAGS) -o $(BIN_DIR)/showip $(SRC_DIR)/showip.c

clean:
	rm -f $(BIN_DIR)/*
	
