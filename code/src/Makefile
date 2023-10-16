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

tcpclient: $(SRC_DIR)/tcpclient.c
	$(CC) $(CFLAGS) -o $(BIN_DIR)/tcpclient $(SRC_DIR)/tcpclient.c

tcpclient_debug: $(SRC_DIR)/tcpclient.c
	$(CC) $(CFLAGS) -g -DDEBUG -o $(BIN_DIR)/tcpclient $(SRC_DIR)/tcpclient.c

showip: $(SRC_DIR)/showip.c
	$(CC) $(CFLAGS) -o $(BIN_DIR)/showip $(SRC_DIR)/showip.c

tcpserver_multiplex: $(SRC_DIR)/tcpserver_multiplex.c
	$(CC) $(CFLAGS) -o $(BIN_DIR)/tcpserver_multiplex $(SRC_DIR)/tcpserver_multiplex.c

tcpserver_multiplex_debug: $(SRC_DIR)/tcpserver_multiplex.c
	$(CC) $(CFLAGS) -g -DDEBUG -o $(BIN_DIR)/tcpserver_multiplex_debug $(SRC_DIR)/tcpserver_multiplex.c

udpclient1: $(SRC_DIR)/udpclient1.c
	$(CC) $(CFLAGS) -o $(BIN_DIR)/udpclient1 $(SRC_DIR)/udpclient1.c

udp_recvfrom: $(SRC_DIR)/udp_recvfrom.c
	$(CC) $(CFLAGS) -o $(BIN_DIR)/udp_recvfrom $(SRC_DIR)/udp_recvfrom.c

udp_sendto: $(SRC_DIR)/udp_sendto.c
	$(CC) $(CFLAGS) -o $(BIN_DIR)/udp_sendto $(SRC_DIR)/udp_sendto.c	

udp_serve_toupper: $(SRC_DIR)/udp_serve_toupper.c
	$(CC) $(CFLAGS) -o $(BIN_DIR)/udp_serve_toupper $(SRC_DIR)/udp_serve_toupper.c	


clean:
	rm -f $(BIN_DIR)/*
	
