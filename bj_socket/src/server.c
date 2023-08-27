#include "../../netlib.h"

#define PORT "3490"     //listen port


int main()
{
    server_start(PORT);
    int status;
    struct addrinfo hints;
    struct addrinfo *servinfo;

    memset(&hints, 0, sizeof(hints));       //empty structure
    hints.ai_family = AF_UNSPEC;            //let the system determine the protocol for the address
    hints.ai_socktype = SOCK_STREAM;        //TCP socket
    hints.ai_flags = AI_PASSIVE;            //fill in my IP for me

    //AI_PASSIVE tells getaddrinfo to assign the address of the local host to the socket (avoids hardcoding it)
    // as such, need to set node param in getaddrinfo to NULL. Can use a specific address there instead of NULL and setting AI_PASSIVE
    if ((status = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0)
    {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        fatal("getaddrinfo");
        exit(1);
    }


    while(1);
    
    //free linked list
    freeaddrinfo(servinfo);

    /////////
    server_stop();
    return 0;
}