#ifndef NET_LIB_H
#define NET_LIB_H

//Use this header file to import all necessary libraries (similar among clients and servers) to avoid 
//code duplication

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

//Socket related headers
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>


//Utility functions
#ifdef DEBUG
#define TRACE_ENTER() printf("[*]ENTER: %s()\n", __func__);
#define TRACE_EXIT() printf("[*]EXIT: %s()\n", __func__);
#else
#define TRACE_ENTER() do {} while (0)
#define TRACE_EXIT() do {} while (0);
#endif

void dump(const unsigned char *data_buffer, const unsigned int length)
{
    unsigned char byte;
    unsigned int i, j;
    for(i=0; i < length; i++) 
    {
        byte = data_buffer[i];
        printf("%02x ", data_buffer[i]); // Display byte in hex.
        if(((i%16)==15) || (i==length-1)) 
        {
            for(j=0; j < 15-(i%16); j++)
            printf(" ");
            printf("| ");
            for(j=(i-(i%16)); j <= i; j++) 
            { // Display printable bytes from line.
                byte = data_buffer[j];
                if((byte > 31) && (byte < 127)) // Outside printable char range
                printf("%c", byte);
                else
                printf(".");
            }
        printf("\n"); // End of the dump line (each line is 16 bytes)
        } // End if
    } // End for
}

int handle_socket_err(char *func, int result, int error)
{
    TRACE_ENTER();
    if(result < 0)
        fprintf(stderr, "%s() error: %s\n", func, strerror(error));
    
    TRACE_EXIT();
    return 1;
    
}

//utility function to display error messages
void fatal (char *message)
{
    char error_message[100];
    strcpy(error_message, "[!!] Fatal Error ");
    strncat(error_message, message, 83);
    perror(error_message);
    //printf("Exiting...");
    exit(-1);
}

void server_start(char *msg)
{
    printf("[*] Ready to use socket API.\n");
    printf("[*] Server starting on port: %s\n", msg);
}

void server_stop()
{
    printf("[*] Server shutting down.\n");
}

void client_start()
{
    printf("[*] Ready to use socket API.\n");
    printf("[*] Client starting\n");
}

void client_stop()
{
    printf("[*] Client Stopping\n");
}

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
    {
        return &(((struct sockaddr_in*) sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);

}

void print_remote_addr_info(struct addrinfo *addr)
{
    TRACE_ENTER();

    char address_buffer[100];
    char service_buffer[100];
    int result = getnameinfo
    (
        addr->ai_addr,
        addr->ai_addrlen,
        address_buffer,
        sizeof(address_buffer),
        service_buffer,
        sizeof(service_buffer),
        NI_NUMERICHOST|NI_NUMERICSERV
    );
    if (result != 0)
    {
        fprintf(stderr,"getnameinfo() failed. Error: (%s)\n", gai_strerror(result));

    }
    else
        printf("Connecting to %s:%s\n", address_buffer, service_buffer);

    TRACE_EXIT();
}

void print_remote_address(struct sockaddr_storage addr)
{
    TRACE_ENTER();
    //printf("In print_remote_address\n");
    char address_buffer[100];
    char service_buffer[100];
    socklen_t addr_len = sizeof(addr);
    
    int result = getnameinfo
    (
        (struct sockaddr*) &addr,
        addr_len,
        address_buffer,
        sizeof(address_buffer),
        service_buffer,
        sizeof(service_buffer),
        NI_NUMERICHOST
    );
    if (result != 0)
    {
        fprintf(stderr,"getnameinfo() failed. Error: (%s)\n", gai_strerror(result));

    }
    else
        printf("Connection from %s:%s\n", address_buffer, service_buffer);

    TRACE_EXIT();
}

/*
    Adapted from HAO, 0x400. pg: 209-210

    This function accepts a socket FD and a ptr to the null terminated string to send. 
    The function will make sure all the bytes of the string are sent.
    
    Args:
        int sockfd: a file descriptor returned by the function socket(), used to send data to a remote host.
        unsigned char* data: a null terminated string to send to the remote host.

    Returns:
        1 on success, 0 on failure.printf("[*]EXIT: %s\n", __func__);
*/
int send_data(int sockfd, const char* data)
{
    //printf("[*]Enter send_data()\n");
    TRACE_ENTER();
    int sent_bytes, bytes_to_send;

    bytes_to_send = strlen(data);
    int ret_val = 1;
    //send data
    while(bytes_to_send > 0)
    {
        //int send(int sockfd, const void *msg, int len, int flags);
        printf("Sending: %s\n", data);
        sent_bytes = send(sockfd, data, bytes_to_send, 0);
        
        //On  success, these calls return the number of bytes sent. 
        //On error, -1 is returned, and errno is  set  to  indicate  the error.
        
        //check for error
        if(sent_bytes == -1)
        {
            perror("send() error. ");
            fprintf(stderr, "send_data error on send(). Error: %s\n", strerror(errno));
            //return ret_val;
            ret_val = 0;
            break;
        }
        
        //how much data do we have left to send?
        bytes_to_send -= sent_bytes;
        data += sent_bytes;
    }

    //printf("[*]Exit send_data()\n");
    TRACE_EXIT();
    return ret_val;   //success
}

/*
*  Adapted from HAO, 0x400. pg: 209-210
*  This function accepts a socket FD and a ptr to a destination
* buffer. It will receive from the socket until the EOL byte
* sequence in seen. The EOL bytes are read from the socket, but
* the destination buffer is terminated before these bytes.
* Returns the size of the read line (without EOL bytes).
*/

int recv_data(int sockfd, unsigned char* data)
{
    //printf("[*]Enter recv_data()\n");
    TRACE_ENTER();
    int data_size = 1024;
    int ret_val = 0;    //return 0 on failure, 1 on success.
    //Returns:
    //   * The number of bytes received.
    //   * 0 if the remote side has closed the connection.
    //   * Or -1 on error, sets errno.
    
    //recieve data until one of the return values occurs.
    
    //we'll loop while receiving data_size chunks at a time
    int bytes_recv = 0;
    while(1) //read a single byte
    {
        printf("Processing in recv_data()...\n");
        bytes_recv = recv(sockfd, data, data_size-1, 0);
        
        if(bytes_recv == 0) //remote side closed connection.
        {
            perror("recv() error");
            fprintf(stderr, "Server disconnected.\n. error: %s\n", strerror(errno));
            break;
        }
        else if(bytes_recv == -1)
        {
            perror("recv() error");
            fprintf(stderr, "recv_data() error on recv(). Error: %s\n", strerror(errno));
            break;
        }
        else
        {
            //we got all the data, add null terminator.
            printf("Got all the data. done\n");
            data[bytes_recv] = '\0';
            ret_val = 1;
            break;
        }
    }

    fprintf(stderr, "Ret: %d\n", ret_val);
    printf("Received: %.*s\n", bytes_recv, data);
    //printf("[*]Exit recv_data()\n");
    TRACE_EXIT();
    return ret_val;

}

//might not be necessary for this example
void add_new_line(char *input)
{
    TRACE_ENTER();
    size_t slen = strlen(input);
    if (slen > 0)
    {
        // printf("len before adding: %ld\n", strlen(input));
        // printf("Adding new line.\n");
        input[slen] ='\n';
        // printf("len before after: %ld\n", strlen(input));
    }
    
    TRACE_EXIT();
}

void strip_new_line(char *input)
{
    TRACE_ENTER();
    size_t slen = strlen(input);
    if (slen > 0)
    {
        //printf("stripping new line.\n");
        if (input[slen-1] == '\n')
            input[slen-1] ='\0';
    }
    
    TRACE_EXIT();
}

//returns 1 on success, 0 on failure.
int send_recv_loop(int socket_peer)
{
    TRACE_ENTER();
    /*
        With this implementation //approach
        send()/recv() block.
        So we have to wait for send() to send data out
        and for recv() to receive data before our program can continue.
        
    */
    int  input_size = 1024;
    char input[input_size];
    unsigned char out_buffer[input_size];
    int ret_val = 1;
    while(1)
    {
        // printf("input buff: %s\n", input);
        // printf("clearing memory\n");
        // memset(input, '\0', sizeof(input));
        // printf("cleared memory. buff: %s\n", input);
        printf("input buff: %s\n", input);
        printf("To send data, enter text followed by enter.\n");
        
        if(fgets(input, sizeof(input), stdin) == NULL)
        {
            perror("fgets");
            ret_val = 0;
            break;
        }
        
        printf("input buffer: %s\n", input);
        // size_t slen = strlen(input);
        // if (slen > 0)
        // {
        //     printf("stripping new line.\n");
        //     if (input[slen-1] == '\n')
        //         input[slen-1] ='\0';
        // }
        strip_new_line(input);
        printf("buffer after call to strip_new_line(). %s\n", input);
        if(strcmp(input, "qq") == 0)
        {
            printf("[*] Disconnect signal received.\n");
            break;
        }

        printf("[*] Processing input\n");
        if(send_data(socket_peer, input) == 0)
        {
            ret_val = 0;
            break;
        }
            
        
        printf("[*] Receving data\n");
        if(recv_data(socket_peer, out_buffer) == 0)
        {
            ret_val = 0;
            break;
        }
    }
    printf("Ret_val: %d\n", ret_val);
    //printf("[*]EXIT send_recv_loop()\n");
    TRACE_EXIT();
    return ret_val;
}


int select_loop(int sockfd)
{
    TRACE_ENTER();
    
    int ret_val = 0;
    int read_size = 4096;
    printf("To send data, enter text followed by enter.\n");
    while(1)
    {
        // //Macros - functions to manipulate fd_set structs.

        //     Function                        Description
        //     FD_SET(int fd, fd_set *set);    Add fd to the set.
        //     FD_CLR(int fd, fd_set *set);    Remove fd from the set.
        //     FD_ISSET(int fd, fd_set *set);  Return true if fd is in the set.
        //     FD_ZERO(fd_set *set);           Clear all entries from the set.
        //we only want to monitor for reads ()
        fd_set read_fdset;
        
        //Zero out fd_set before use.
        FD_ZERO(&read_fdset);
        

        //Add our socket to the fd_set
        FD_SET(sockfd, &read_fdset);
        FD_SET(0, &read_fdset);
        
        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 100000;
        // RETURN VALUE
        //  On success, select() and pselect() return the number of file descriptors contained in the three returned
        //  descriptor  sets  (that is, the total number of bits that are set in readfds, writefds, exceptfds).  The
        //  return value may be zero if the timeout expired before any file descriptors became ready.

        //  On error, -1 is returned, and errno is set to indicate the error; the file descriptor sets  are  unmodi‐
        //  fied, and timeout becomes undefined.
        
        if(select(sockfd + 1, &read_fdset, 0, 0, &timeout) < 0)
        //int ret = select(sockfd + 1, &read_fdset, 0, 0, &timeout);
        //if(ret < 0)
        {
            perror("select()");
            fprintf(stderr, "error on select(). Error: %s\n", strerror(errno));
            ret_val = 0;
            break;
        }

        //select() has returned, we check for whether our socket is set in reads
        
        if (FD_ISSET(sockfd, &read_fdset))
        {
            printf("Checking reads_fd\n");
            char read[read_size];
            //Returns:
            //   * The number of bytes received.
            //   * 0 if the remote side has closed the connection.
            //   * Or -1 on error, sets errno.
            
            int bytes_received = recv(sockfd, read, read_size, 0);
            if(bytes_received == 0)
            {
                perror("recv()");
                fprintf(stderr, "Connection closed by host. Error: %s", strerror(errno));
                ret_val = 0;
                break;
            }
            if(bytes_received < 0)
            {
                perror("recv()");
                fprintf(stderr, "recv() error from %s(). Error: %s", __func__, strerror(errno));
                ret_val = 0;
                break;
            }
            printf("Received: (%d) bytes): %.*s",
                    bytes_received, bytes_received, read);
            
        }
        //check for STDIN
        if(FD_ISSET(0, &read_fdset))
        {
            printf("Checking for stdin\n");
            char read[read_size];
            if(!fgets(read, read_size, stdin))
                break;
            
            strip_new_line(read);
            if(strcmp(read, "qq") == 0)
            {
                printf("[*] Disconnect signal received.\n");
                break;
            }
            add_new_line(read);
            printf("Sending: %s", read);
            int bytes_sent = send(sockfd, read, strlen(read), 0);
            printf("Sent %d bytes.\n", bytes_sent);
        }
    }

    TRACE_EXIT();
    return ret_val;
}


void config_addr_info_struct(struct addrinfo *addr_struct)



#endif //NET_LIB_Hdisplay how to send 