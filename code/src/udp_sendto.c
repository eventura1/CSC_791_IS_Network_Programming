#include "../../netlib.h"

int main(int argc, char *argv[])
{
    client_start();

    if (argc !=3 )
    {
        printf("Usage: %s ip port\n", argv[0]);
        return 1;
    }

    struct addrinfo hints, *peer_address;
    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_DGRAM;
    if(getaddrinfo(argv[1], argv[2], &hints, &peer_address))
    {
        fprintf(stderr, "getaddrinfo() failed. (%s)\n", strerror(errno));
        return 1;
    }
    print_remote_addr_info(peer_address);
    int socket_peer = socket(peer_address->ai_family, peer_address->ai_socktype, peer_address->ai_protocol);
    if(!handle_socket_err("socket", socket_peer, errno))
        return 1;
    int result = 0;
    const char *message = "Hello World";
    int bytes_sent = sendto(socket_peer, message, strlen(message), 0, peer_address->ai_addr, peer_address->ai_addrlen);
    //send to doesn't return an error message, it simply tries to send the data.
    printf("Sent: %d bytes.\n", bytes_sent);
    
    

    freeaddrinfo(peer_address);
    
    printf("Closing socket ...\n"); 
    result = close(socket_peer);
    handle_socket_err("close", result, errno);
    client_stop();
    return 0;
}