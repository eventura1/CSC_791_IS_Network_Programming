/*
    This program demonstrates the use of getaddrinfo() and getnameinfo().

    This program takes a name or IP address for its only argument. It then uses
    getaddrinfo() to resolve that name or IP address into an address structure,
    and the program prints that IP address using getnameinfo() for the text conversion.
*/

#include "../../netlib.h"

int main(int argc, char *argv[])
{
    client_start();
    if(argc !=2)
    {
        printf("Usage: %s hostname|IP address\n", argv[0]);
        return 1;
    }

    printf("Resolving hostname '%s'\n", argv[1]);
    struct addrinfo hints, *peer_address;
    memset(&hints, 0, sizeof(hints));
    hints.ai_flags = AI_ALL;            //request both IPv4 and IPv6 addresses.

    if(getaddrinfo(argv[1], 0, &hints, &peer_address))  //returns 0 for failure.
    {
        fprintf(stderr, "getaddrinfo() failed. Error#: (%d) [Error msg: (%s)]\n",errno ,strerror(errno));
        return 1;
    }
    printf("Remote address is:\n");
    int buffer_size = 100;
    struct addrinfo *address = peer_address;
    do
    {
        char address_buffer[buffer_size];
        getnameinfo(address->ai_addr, address->ai_addrlen,
                     address_buffer, sizeof(address_buffer),
                     0, 0,
                     NI_NUMERICHOST);
                
        printf("\t%s\n",address_buffer);

    } while ((address = address->ai_next));
    freeaddrinfo(peer_address);
    address = NULL;
    client_stop();
    return 0;
}