#include "../../netlib.h"


/*
    A tcp client that connects to a TCP server listening on a given port.

*/
int main(int argc, char *argv[])
{
    client_start();
    if(argc != 3)
{
        fprintf(stderr, "usage: client hostname port\n");
        return 1;
    }

    int status;
    struct addrinfo hints;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    //get ready to connect
    struct addrinfo *servinfo;
    if((status = getaddrinfo(argv[1], argv[2], &hints, &servinfo)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        return 2;
    }
    
    //parse info

    struct addrinfo *p;
    
    freeaddrinfo(servinfo);
    client_stop();
    return 0;
}