CC = gcc
CFLAGS = -Wall -Wextra 
SRC_DIR = ../src
BIN_DIR = ../bin
#convinience dirs
TCP_CLIENT = $(SRC_DIR)/tcp_client
TCP_SERVER = $(SRC_DIR)/tcp_server
WEB_CLIENT = $(SRC_DIR)/web_client
WEB_SERVER = $(SRC_DIR)/web_server
UDP = $(SRC_DIR)/udp
DNS_SRC = $(SRC_DIR)/dns

#all: server client showip

server: $(TCP_SERVER)/server.c
	$(CC) $(CFLAGS) -o $(BIN_DIR)/server $(TCP_SERVER)/server.c

server_loop: $(TCP_SERVER)/server_loop.c
	$(CC) $(CFLAGS) -o $(BIN_DIR)/server_loop $(TCP_SERVER)/server_loop.c

client: $(TCP_CLIENT)/client.c
	$(CC) $(CFLAGS) -o $(BIN_DIR)/client $(TCP_CLIENT)/client.c

tcpclient: $(TCP_CLIENT)/tcpclient.c
	$(CC) $(CFLAGS) -o $(BIN_DIR)/tcpclient $(TCP_CLIENT)/tcpclient.c

tcpclient_debug: $(TCP_CLIENT)/tcpclient.c
	$(CC) $(CFLAGS) -g -DDEBUG -o $(BIN_DIR)/tcpclient $(TCP_CLIENT)/tcpclient.c

tcpserver_multiplex: $(TCP_SERVER)/tcpserver_multiplex.c
	$(CC) $(CFLAGS) -o $(BIN_DIR)/tcpserver_multiplex $(TCP_SERVER)/tcpserver_multiplex.c

tcpserver_multiplex_debug: $(TCP_SERVER)/tcpserver_multiplex.c
	$(CC) $(CFLAGS) -g -DDEBUG -o $(BIN_DIR)/tcpserver_multiplex_debug $(TCP_SERVER)/tcpserver_multiplex.c

udpclient1: $(UDP)/udpclient1.c
	$(CC) $(CFLAGS) -o $(BIN_DIR)/udpclient1 $(UDP)/udpclient1.c

udp_recvfrom: $(UDP)/udp_recvfrom.c
	$(CC) $(CFLAGS) -o $(BIN_DIR)/udp_recvfrom $(UDP)/udp_recvfrom.c

udp_sendto: $(UDP)/udp_sendto.c
	$(CC) $(CFLAGS) -o $(BIN_DIR)/udp_sendto $(UDP)/udp_sendto.c	

udp_serve_toupper: $(UDP)/udp_serve_toupper.c
	$(CC) $(CFLAGS) -o $(BIN_DIR)/udp_serve_toupper $(UDP)udp_serve_toupper.c	

webget: $(WEB_CLIENT)/web_get.c
	$(CC) $(CFLAGS) -o $(BIN_DIR)/webget $(WEB_CLIENT)/web_get.c

webget_debug: $(WEB_CLIENT)/web_get.c
	$(CC) $(CFLAGS) -g -DDEBUG -o $(BIN_DIR)/webget_debug $(WEB_CLIENT)/web_get.c

showip: $(DNS_SRC)/showip.c
	$(CC) $(CFLAGS) -o $(BIN_DIR)/showip $(DNS_SRC)/showip.c

lookup: $(DNS_SRC)/lookup.c
	$(CC) $(CFLAGS) -o $(BIN_DIR)/lookup $(DNS_SRC)/lookup.c


webserver_debug: $(WEB_SERVER)/web_server.c
	$(CC) $(CFLAGS) -g -DDEBUG -o $(BIN_DIR)/web_server_debug $(WEB_SERVER)/web_server.c

webserver: $(WEB_SERVER)/web_server.c
	$(CC) $(CFLAGS) -o $(BIN_DIR)/webserver $(WEB_SERVER)/web_server.c


clean:
	rm -f $(BIN_DIR)/*
	
