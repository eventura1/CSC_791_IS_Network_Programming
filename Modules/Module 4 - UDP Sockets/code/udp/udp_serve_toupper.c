#include "../../../netlib.h"
#include <ctype.h>

int main()
{
    server_start("8080");

    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;

    struct addrinfo *bind_address;
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

    if(!udp_select_loop(socket_listen))
        fprintf(stderr,"udp_select_loop error.\n");


    printf("closing socket ...\n");
    result = close(socket_listen);
    handle_socket_err("close", result, errno);
    
    server_stop();
    return 0;
}