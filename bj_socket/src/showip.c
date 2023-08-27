#include "../../netlib.h"


/*
    showip.c -- show IP addresses for a host given on the command line.
*/
int main(int argc, char *argv[])
{
    client_start("client mode");
    
    

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
    //code will differ from the book a bit, as far as where variables are declared. Using best practices of
    //declaring variables as close to where they are used as possible.

    struct addrinfo hints, *res;
    char ipstr[INET6_ADDRSTRLEN];       //length of an IPv6 address.
    int status;                         //return value for getaddrinfo()

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;            //let the system provide available addresses
    hints.ai_socktype = SOCK_STREAM;        //TCP stream socket
    
    //https://man7.org/linux/man-pages/man3/getaddrinfo.3.html
    /*

       int getaddrinfo(
        const char *restrict node,                  /address. e.g. "google.com" or 1.1.1.1
        const char *restrict service,               //port or service name : 80 or http.
        const struct addrinfo *restrict hints,      //A struct addrinfo filled out with relevant info.
                                                    //That is, the type of data we wish for getaddrinfo() to return.
        struct addrinfo **restrict res);            //A struct addrinfo that will hold the results from getaddrinfo().
    */
   
    if((status = getaddrinfo(argv[1], NULL, &hints, &res)) != 0)
    {    //handle errors
    
        //gai_strerror might be similar to GetLastError() in Windows
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        return 2;
    }
    printf("IP Addresses for :%s\n\n", argv[1]);

    struct addrinfo *p;
    //from struct addrinfo definition
    //struct addrinfo *ai_next;	/* Pointer to next in list.  */ 
    //walk the linked list for all avaible addresses for the query (that is IPv4 and IPv6)
    for(p = res; p != NULL; p = p->ai_next)
    {
        void *addr = NULL;
        char *ipver = NULL;
        
        //get the pointer to the address itself
        //AF_INET == IPv4 and AF_INET6 == IPv6.
        //remember, there are only two AF families that are used.
        if(p->ai_family == AF_INET)
        {//Handle IPv4
            //one of the members of addrinfo is a sockaddr struct
            //struct sockaddr *ai_addr;	/* Socket address for socket.
            //This structure is made up as such:
            // Structure describing an Internet socket address.
            
            // struct sockaddr_in {
            // short int sin_family; // Address family, AF_INET
            // unsigned short int sin_port; // Port number
            // struct in_addr sin_addr; // Internet address
            // unsigned char sin_zero[8]; // Same size as struct sockaddr
            // };

            //We're accessing that structure's sin_addr member to retrieve the IP address
            //from our results.

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