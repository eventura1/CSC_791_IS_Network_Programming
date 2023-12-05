#include "../../../netlib.h"

int main()
{

    server_start("8080");

    printf("Configuring local address ...\n");
    struct addrinfo hints, *bind_address;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;
    getaddrinfo(0, "8080", &hints, &bind_address);
    
    printf("Creating socket ...\n");
    int socket_listen = socket(bind_address->ai_family, bind_address->ai_socktype, bind_address->ai_protocol);
    if(!handle_socket_err("socket", socket_listen, errno))
        return 1;
    
    printf("Binding socket to local address ...\n");
    int result = bind(socket_listen, bind_address->ai_addr, bind_address->ai_addrlen);
    if(!handle_socket_err("bind", result, errno))
        return 1;
    freeaddrinfo(bind_address);

    //We now start exploring the difference between TCP and UDP.
    //Once the local address is bound, we can simply start sending/receiving data.
    //no need to call listen() or accept().
    
    //recall, sockaddr_storage is big enough for IPv4 or IPv6, this makes our code protocol independent.
    struct sockaddr_storage client_address;    
    socklen_t client_len = sizeof(client_address);
    int READ_SIZE = 1024;
    char read[READ_SIZE];
    int bytes_received = recvfrom(socket_listen, read, READ_SIZE, 0, (struct sockaddr *) &client_address, &client_len);
    //On return, we use client_address to send data to the remote end
    //printf("Remote address is: ");
    print_remote_address(client_address);
    printf("Received:(%d bytes): %.*s\n", bytes_received, bytes_received, read);


    printf("Closing socket ...\n");
    close(socket_listen);
    server_stop();
    return 0;
}