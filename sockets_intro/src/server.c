#include "../libs/netlib.h"

///
#define PORT  7890  //listen port
#define BUF_SIZE 1024

void running(char *msg)
{
    printf("%s", msg);
}

int main(void)
{
    running("Simple_server starting up ...\n");
    int sockfd;         //file descriptor returned by socket, used for listen socket
    int new_sockfd;     //new connection socket
    
    //This structure has been updated, read Beej's where it shows how to use getaddrinfo() to fill it out
    struct sockaddr_in host_addr, client_addr;      //address information
    socklen_t sin_size;
    long int recv_len = 1;
    int yes = 1;
    char buffer[BUF_SIZE];

    //IPv4 stream socket. 0 is default protocol
    //returns -1 on error
    /*
        We're setting up a TCP/IP socket using the PF_INET (IPv4 protocol family)
        and the socket type is SOCK_STREAM.
        Protocol is 0 since there is only one protocol supported in the PF_INET protocol
        family.
    */
    if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1)
        fatal("in socket");
    
    /*
        This function sets the socket option. We'll come back to this later.
        The provided option allows it to reuse a given address for binding.
    */
    if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
        fatal("setting socket option SO_REUSEADDR");

    
    //Setup the host_addr structure for use in the bind call.
    //Host Byte order and Network Byte Order handling
    host_addr.sin_family = AF_INET;                 //IPv4 Address Family for Internet. Host Byte Order -- Little-Endian
    host_addr.sin_port = htons(PORT);               //Short, network byte order -- Big-Endian
    host_addr.sin_addr.s_addr = 0;                  //Automatically fill with my IP.
    memset(&(host_addr.sin_zero), '\0', 8);         //Zero the rest of the struct.

    //This call will bind the socket to the current IP address on the given port.
    if(bind(sockfd, (struct  sockaddr *)&host_addr, sizeof(struct sockaddr)) == -1)
        fatal("binding to socket");

    //Tells the socket to listen for incoming connections.
    if (listen(sockfd, 5) == -1)
        fatal("listening on socket");

    //Accept loop.
    char *send_msg = "Hello, world\n";
    printf("send_msg len == %ld\n", strlen(send_msg));

    while(1)
    {
        sin_size = sizeof(struct sockaddr_in);
        new_sockfd = accept(sockfd, (struct sockaddr *)&client_addr, &sin_size);
        if(new_sockfd == -1)
            fatal("accepting connection");
        
        printf("server: got connection from %s port %d\n",
                inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        send(new_sockfd, send_msg, strlen(send_msg), 0);
        
        recv_len = recv(new_sockfd, &buffer, BUF_SIZE, 0);
        while(recv_len > 0) //while there is more data to receive, keep processing
        {
            printf("RECV: %ld bytes\n", recv_len);
            dump(buffer, recv_len);
            recv_len = recv(new_sockfd, &buffer, BUF_SIZE, 0);
        }

        //close new socket
        close(new_sockfd);

    }//end receive loop

    //user can run netstat -ant and search for port 7890 for listening socket.


    //shutdown
    running("Simple_server shutting down...\n");
    return 0;
}