/*
    tcp_serve_toupper.c
    As a motivating example, we are going to build a TCP server that converts strings into
    uppercase. If a client connects and sends Hello, then our program will send HELLO back.

    This program uses select() to handle multiple incoming connections.

*/

#include "../../../netlib.h"
#include <ctype.h>

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
    if(!handle_socket_err("socket", socket_listen, error_num))
        return 1;

    
    //2. bind()
    printf("Binding...\n");
    result = bind(socket_listen, bind_addr->ai_addr, bind_addr->ai_addrlen);
    error_num = errno;
    if(!handle_socket_err("bind", result, error_num))
        return 1;
    freeaddrinfo(bind_addr);
    
    //3. listen()
    int BACKLOG = 10;
    result = listen(socket_listen, BACKLOG);
    error_num = errno;
    if(!handle_socket_err("listen", result, error_num))
        return 1;

    printf("FD: %X\n", socket_listen);

    //up to this point our program has been the same as other servers.
    //we will now implement our server using the select() function.

    fd_set master;
    //clear the FD set
    FD_ZERO(&master);
    //add file descriptor for select to monitor
    FD_SET(socket_listen, &master);
    int max_socket = socket_listen;
    printf("Waiting for connections ...\n");

    int idx = 0;
    int READ_SIZE = 1024;
    while(1)
    {
        fd_set reads;
        
        //make copy. our fd_set will be modified when select() returns
        reads = master;
        
        //monitor FD sets for I/O operations.
        result = select(max_socket + 1, &reads, 0, 0, 0);
        error_num = errno;
        if(!handle_socket_err("select", result, error_num))
            return -1;

        //Loop through all posible sockets and see whether it was flaged by select()
        //as being ready.
        for(int i = 1; i <= max_socket; i++)
        {
            //In this case, FD_ISSET() is only true for sockets that are ready to be read.
            //In this case, we're looking for socket_listen, which means we're interested in
            //sockets that are ready to establish a connection with accept().
            if(FD_ISSET(i, &reads))
            {
                //Is the listening socket ready to accept()? 
                if(i == socket_listen)
                {
                    struct sockaddr_storage client_address;
                    socklen_t client_len = sizeof(client_address);
                    int socket_client = accept(socket_listen, (struct sockaddr*) &client_address,  &client_len);
                    error_num = errno;
                    if(!handle_socket_err("accept", socket_client, error_num))
                        return 1;
                    
                    //Add new connection to our master set so we can check it.
                    FD_SET(socket_client, &master);
                    if (socket_client > max_socket)
                        max_socket = socket_client;
                    
                    print_remote_address(client_address);
                }
                else    //must be a socket ready to receive data.
                {
                    char read[READ_SIZE];
                    int bytes_received = recv(i, read, READ_SIZE,0);
                    //handle disconnects or errors and remove the socket from the set and close socket.
                    if(bytes_received < 1)
                    {
                        FD_CLR(i, &master); //remove from set
                        close(i);
                        continue;
                    }
                    //handle converting data to upper.
                    int j;
                    for(j = 0; j < bytes_received; ++j)
                        read[j] = toupper(read[j]);
                    
                    send(i, read, bytes_received, 0);
                }//end else
            
            }//end if FD_SET
        
        }//end for i to max_socket
        idx++;
    } //end while(1)


    printf("closing listening socket...\n");
    result = close(socket_listen);
    error_num = errno;
    handle_socket_err("close", result, error_num);
    
    server_stop();
    return 0;
}