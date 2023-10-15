/*
    tcp_serve_toupper.c
    As a motivating example, we are going to build a TCP server that converts strings into
    uppercase. If a client connects and sends Hello, then our program will send HELLO back.

    This program uses select() to handle multiple incoming connections.

*/

#include "../../netlib.h"

void usage(char *msg)
{
    printf("Usage: %s port\n", msg);
    exit(1);
}

int main()
{
    char *port = "8080";
    server_start(port);
    printf("Configuring local address ...\n");
    
    struct addrinfo hints; 
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    
    struct addrinfo *bind_addr;
    getaddrinfo(0, port, &hints, &bind_addr);

    int error_num = 0;
    int result = 0;
    
    //1.socket()
    printf("Creating socket ...\n");
    int socket_listen = socket(bind_addr->ai_family, bind_addr->ai_socktype, bind_addr->ai_protocol);
    error_num = errno;
    handle_socket_err("socket", socket_listen, error_num);

    
    //2. bind()
    printf("Binding...\n");
    result = bind(socket_listen, bind_addr->ai_addr, bind_addr->ai_addrlen);
    error_num = errno;
    handle_socket_err("bind", result, error_num);
    freeaddrinfo(bind_addr);
    
    //3. listen()
    int BACKLOG = 10;
    result = listen(socket_listen, BACKLOG);
    error_num = errno;
    handle_socket_err("listen", result, error_num);

    printf("FD: %X\n", socket_listen);

    //up to this point our program has been the same as other servers.
    //we will now implement our server using the select() function.

    fd_set master;
    
    FD_ZERO(&master);
    FD_SET(socket_listen, &master);
    int max_socket = socket_listen;
    printf("Waiting for connections ...\n");

    while(1)
    {
        fd_set reads;
        
        //make copy. our fd_set will be modified when select() returns
        reads = master;
        result = select(max_socket + 1, &reads, 0, 0, 0);
        error_num = errno;
        if(!handle_socket_err("select", result, errno))
            return -1;

        
        for(int i = 1; i <= max_socket; i)
        {
            if (FD_ISSET(i, &reads))
            {
                //handle socket
            }
        }
    }


    result = close(socket_listen);
    server_stop();
    return 0;
}