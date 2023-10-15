#include "../../netlib.h"


/*
    A tcp client that connects to a TCP server listening on a given port and receives data sent to it.
    Only receives data sent in one stream, doesn't implemente a recv()/send() loop.
*/

//helper functions
// get sockaddr, IPv4 or IPv6:
// void *get_in_addr(struct sockaddr *sa)
// {
//     if (sa->sa_family == AF_INET)
//     {
//         return &(((struct sockaddr_in*) sa)->sin_addr);
//     }

//     return &(((struct sockaddr_in6*)sa)->sin6_addr);

// }

//#define PORT 3490

#define MAXDATASIZE 100

int main(int argc, char *argv[])
{
    
    if(argc != 3)
    {
        fprintf(stderr, "usage: client IP |client hostname port\n");
        exit(1);
    }
    client_start();
/*
    //1. getaddrinfo()
    //2. socket()
    //3. connect()
    //4. send()
    //5. recv()
    //6. close()    
*/

    //1. getaddrinfo()
    /*
        Need a struct addrinfo structure for getaddrinfo()
        requires we clear out the structure, with memset, prior to use.

        The hints argument points to an addrinfo structure that specifies criteria for selecting the socket address
        structures returned in the list pointed to by res. If hints is not NULL it points to an addrinfo structure 
        whose ai_family, ai_socktype, and ai_protocol specify criteria that limit the set of socket addresses returned by getaddrinfo(), as follows:

    */

    struct addrinfo hints, *servinfo, *p;

    //clear out memory
    printf("Configuring remote address ...\n");
    fprintf(stderr, "calling memset...\n");
    memset(&hints, 0, sizeof(hints));
    fprintf(stderr, "memset succ()\n");

    //set relevant fields.
    //https://linux.die.net/man/3/getaddrinfo
    
    //ai_family: This field specifies the desired address family for the returned addresses. 
    //  Valid values for this field include AF_INET and AF_INET6. The value AF_UNSPEC indicates 
    //  that getaddrinfo() should return socket addresses for any address family 
    //  (either IPv4 or IPv6, for example) that can be used with node and service.
    hints.ai_family = AF_UNSPEC;            //Let the system decide

    //This field specifies the preferred socket type, for example SOCK_STREAM or SOCK_DGRAM. 
    //Specifying 0 in this field indicates that socket addresses of any type can be returned by getaddrinfo().
    hints.ai_socktype = SOCK_STREAM;        //we want TCP


    //call getaddrinfo()
    int result=0;
    const char *port = argv[2];
    const char *node = argv[1];
    //node and service need to be const char*
    //servinfo will be returned with the relevant data for the remote host.
    //getaddrinfo() makes a call and reaches out 
    if( (result = getaddrinfo(node, port, &hints, &servinfo)) != 0 )
    {
        //gai_strerror() is used to get errors returned by getaddrinfo() and freeaddrinfo()
        //for socket related functions use perror() or strerror().
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(result));
        return -1;
    }
    printf("getaddrinfo() success\n");

    //Let's print out the address

    printf("Remote address is: ");
    
    /*
        
        https://linux.die.net/man/3/getnameinfo

        The getnameinfo() function is the inverse of getaddrinfo(3): it converts a socket address
        to a corresponding host and service, in a protocol-independent manner. It combines the 
        functionality of gethostbyaddr(3) and getservbyport(3), but unlike those functions, 
        getnameinfo() is reentrant and allows programs to eliminate IPv4-versus-IPv6 dependencies.


        int getnameinfo
        (
            const struct sockaddr *sa,      //pointer to a generic socket address structure (sockaddr_in or sockaddr_in)
            socklen_t salen,                //size of the struct
            char *host,                     //pointer to a buffer where getnameinfo will write a null-terminated string containing hostname
            size_t hostlen,                 //size of host buffer
            char *serv,                     //pointer to a buffer where getnameinfo will write a null-terminated string containing hostname
            size_t servlen,                 //size of serv buffer
            int flags                       //flags to tell getnameinfo() how to present the data.
        );


    */
    char address_buffer[100];
    char service_buffer[100];
    result = getnameinfo(servinfo->ai_addr, servinfo->ai_addrlen,
                        address_buffer, sizeof(address_buffer),
                        service_buffer, sizeof(service_buffer),
                        NI_NUMERICHOST);

    printf("%s %s\n", address_buffer, service_buffer);

    //we are now ready to create our socket
    //2. socket()
    //recall, we set hints.ai_family = AF_UNSPEC;            //Let the system decide
    //So the system could very well return IPv6 or IPv4 as well as multiple addresses.
    //we need to loop through the addrinfo structure and pick one.
    int remoteSockfd;
    //doesn't handle the case when a server isn't listening
    for(p = servinfo; p != NULL; p = p->ai_next)
    {
        if( (remoteSockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1 )
        {
            perror("client: socket()");
            continue;
        }

        //3. connect()
        result = connect(remoteSockfd, p->ai_addr, p->ai_addrlen);
        if ( result == -1)
        {
            close(remoteSockfd);
            
            //fprintf(stderr, "connect(): %s\n", gai_strerror(result));
            perror("client: connect().");
            if (remoteSockfd == ECONNREFUSED)
            {
                fprintf(stderr, "Remote address is not listening. Exiting ...\n");
                exit(1);
            }
            continue;;
        }
        //if we get here we found a valid address to use.
        break;
    }

    if(p == NULL)
    {
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }
    char s[INET6_ADDRSTRLEN];
    //convert address to human readable?
    inet_ntop( p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
                            s, sizeof(s));
    printf("client: connecting to: %s\n", s);
    //https://linux.die.net/man/3/freeaddrinfo
    //The freeaddrinfo() function frees the memory that was allocated for the dynamically allocated linked list res.
    //frees structures allocated by getaddrinfo().
    freeaddrinfo(servinfo);

    int numbytes = 0;
    char buf[MAXDATASIZE];

    if((numbytes = recv(remoteSockfd, buf, MAXDATASIZE-1, 0)) == -1)
    {
        perror("recv()");
        exit(1);
    }
    buf[numbytes] = '\0';       //set null terminator character at the end of buffer.
    printf("Client: received '%s' \n", buf);
    close(remoteSockfd);
    client_stop();
    return 0;
}