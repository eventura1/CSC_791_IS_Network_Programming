#include "../../netlib.h"


/*
    showip.c -- show IP addresses for a host given on the command line.
*/
int main(int argc, char *argv[])
{
    client_start("client mode");
    
    struct  addrinfo hints, *res;
    int status;
    char ipstr[INET6_ADDRSTRLEN];

    if (argc != 2)
    {
        fprintf(stderr, "usage: showip hostname\n");
        return 1;
    }
    //https://cplusplus.com/reference/cstring/memset/
    //Sets the first num bytes of the block of memory pointed by ptr to the specified value (interpreted as an unsigned char).

    //the hints param points to a struct addfino that has already been filled out with relevant information. We do this so getaddrinfo
    //can get us the desired information.
    //getaddrinfo will return a struct addrinfo *res (results) with the requested data.
    //https://man7.org/linux/man-pages/man3/getaddrinfo.3.html
    /*
        The value AF_UNSPEC indicates that getaddrinfo() should return socket addresses for any 
        address family (either IPv4 or IPv6, for example) that can be used with node and service.
    */

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;            //let the system provide available addresses
    hints.ai_socktype = SOCK_STREAM;        //TCP stream socket
    
    //https://man7.org/linux/man-pages/man3/getaddrinfo.3.html

    if((status = getaddrinfo(argv[1], NULL, &hints, &res)) != 0)
    {
        //handle errors
        //gai_strerror might be similar to GetLastError() in Windows
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        return 2;
    }

    printf("IP Addresses for :%s\n\n", argv[1]);

    struct addrinfo *p;
    for(p = res; p != NULL; p = p->ai_next)
    {
        void *addr = NULL;
        char *ipver = NULL;
        
        //get the pointer to the address itself
        if(p->ai_family == AF_INET)
        {//Handle IPv4
            //why are we casting?
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
            addr = &(ipv4->sin_addr);
            ipver = "IPv4";
        }
        else
        {   //handle IPv6
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in *)p->ai_addr;
            addr = &(ipv6->sin6_addr);
            ipver = "IPv6";
        }

        //convert the IP to a string and print it.
        inet_ntop(p->ai_family, addr, ipstr, sizeof(ipstr));
        printf(" %s: %s\n", ipver, ipstr);

    }

    //free linked list
    freeaddrinfo(res);
    ////
    client_stop();
    return 0;
}