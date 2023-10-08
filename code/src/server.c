/*
    This is a simple TCP server that listens on a given port and returns the time to the connecting host.
    It only handles 

*/
#include "../../netlib.h"
#include <signal.h>
#include <sys/wait.h>
#include <time.h>
#define PORT "3490"     //listen port
#define BACKLOG 10      //how many pending connections queue will hold


// void sigchld_handler(int s)
// {
//     int saved_errno = errno;

//     while(waitpid (-1, NULL, WNOHANG) > 0)
//         ;
//     errno = saved_errno;
    
// }


void returnTime()
{

}
int main()
{
    server_start(PORT);
    
    /*
        //1. getaddrinfo()
        //2. socket()
        //3. bind()
        //4. listen()
        //5. send()/recv()
        //6. close()    
    */

    //1. getaddrinfo()
    printf("Configuring local address ...\n");
    struct addrinfo hints, *bind_address;

    //zero out structure
    memset(&hints, 0, sizeof(hints));
    
    //This is the protocol family we want our server to listen on (IPv4 or IPv6 or both)
    //AF_INET = ipv4
    //AF_INET6 = IPV6
    //AF_UNSPEC = getaddrinfo() should return socket addresses for any address family. 
    hints.ai_family = AF_INET6;      //let's do IPv4

    //socket type: TCP or UDP 
    hints.ai_socktype = SOCK_STREAM;

    //flags
    /*
        If the AI_PASSIVE flag is specified in hints.ai_flags, and node is NULL, then the returned socket 
        addresses will be suitable for bind(2)ing a socket that will accept(2) connections.
        The returned socket address will contain the "wildcard address" (INADDR_ANY for IPv4 addresses, IN6ADDR_ANY_INIT for IPv6 address). 
        The wildcard address is used by applications (typically servers) that intend to accept connections on any of the hosts's network addresses.
        If node is not NULL, then the AI_PASSIVE flag is ignored.
    */
    //by calling getaddrinfo() before the other socket functions
    //we avoid having to pass the socket family, type, and protocol multiple times.
    //getaddrinfo() allows us to pass the structured with its values instead. this 
    //makes our code more robust and easier to change to another family type.
    hints.ai_flags = AI_PASSIVE;
    printf("getaddrinfo() running\n");
    int result = getaddrinfo(NULL, PORT, &hints, &bind_address);
    if(result != 0)
    {
        fprintf(stderr, "getaddrinfo(): %s\n", gai_strerror(result));
        return 1;
    }

    printf("getaddrinfo() success\n");

    
    //2. socket()

    printf("Creating socket...\n");
    //We're requsting an IPv4 socket, SOCK_STREAM type, and IPPROTO_TCP. 
    int listen_sockfd = socket(bind_address->ai_family, bind_address->ai_socktype, bind_address->ai_protocol);
    //check for error
    if(listen_sockfd == -1)
    {
        fprintf(stderr, "socket() error. (%s)\n", gai_strerror(result));
        return 1;
    }

    //3. bind()
    printf("Binding socket ...\n");
    result = bind(listen_sockfd, bind_address->ai_addr, bind_address->ai_addrlen);
    
    if(result == -1)
    {
        fprintf(stderr, "bind() error. (%s)\n", gai_strerror(result));
        close(listen_sockfd);
        return 1;
    }
    printf("Socket bound()\n");
    freeaddrinfo(bind_address);
    //we can now listen
    //4. listen()
    //prior to this call listen_sockfd was an active socket.
    //listen() will convert it to a passive socket (accepts connections)
    result = listen(listen_sockfd, BACKLOG);
    if (result == -1)
    {
        fprintf(stderr, "listen() error. (%s)\n", gai_strerror(result));
        close(listen_sockfd);
        return 1;
    }
    
    //We're now ready to accept connections
    //5. accept()
    struct sockaddr_storage client_address;
    socklen_t client_len = sizeof(client_address);
    int client_sockfd = accept(listen_sockfd, (struct sockaddr *) &client_address, &client_len);
    printf("Accepting()...\n");
    if(client_sockfd == -1)
    {
        fprintf(stderr, "accept() error. (%s)\n", gai_strerror(result));
        close(listen_sockfd);
        close(client_sockfd);
        return 1;
    }
    printf("Client connected.\n");
    //accepts() blocks, so the server will sleep until a connection is made.
    //it will shutdown after a connection is completed.
    //why doesn't the client (ncat) die?
    print_remote_address(client_address);
    
    printf("Reading request ...\n");
    char request[1024];
    //the application blocks again.
    int bytes_recv = recv(client_sockfd, request, sizeof(request), 0);
    printf("Received: %d\n", bytes_recv);
    //this won't always work because there is no guarantee that the received data is null terminated.
    //printf("Data: %s\n", request);

    //but this will work. "%.*s" tells printf() that we want to print a specific number of characters
    //bytes_recv
    printf("Data: %.*s\n", bytes_recv, request);
    

    printf("Sending response ...\n");
    const char *response = 
    "HTTP/1.1 200 OK\r\n"
    "Connection: close\r\n"
    "Content-Type: text/plain\r\n\r\n"
    "Local time is: ";
    int bytes_sent = send(client_sockfd, response, strlen(response), 0);
    printf("Sent %d of %d bytes.\n", bytes_sent, (int)strlen(response));

    time_t timer;
    time(&timer);
    char *time_msg = ctime(&timer);
    bytes_sent = send(client_sockfd, time_msg, strlen(time_msg), 0);
    printf("Sent %d of %d bytes.\n", bytes_sent, (int)strlen(time_msg));

    printf("Closing connectiong ...\n");

    /////////
    close(client_sockfd);
    close(listen_sockfd);
    server_stop();
    return 0;
}