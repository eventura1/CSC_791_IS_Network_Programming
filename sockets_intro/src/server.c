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
    
    //This structure has been updated, read Beej's
    struct sockaddr_in host_addr, client_addr;      //address information
    socklen_t sin_size;
    int recv_len = 1;
    int yes = 1;
    char buffer[BUF_SIZE];

    //IPv4 stream socket. 0 is default protocol
    //returns -1 on error
    //if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1)

    running("Simple_server shutting down...\n");
    return 0;
}