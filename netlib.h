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

//utility function to display error messages
void fatal (char *message)
{
    char error_message[100];
    strcpy(error_message, "[!!] Fatal Error ");
    strncat(error_message, message, 83);
    perror(error_message);
    printf("Exiting...");
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


void print_remote_address(struct sockaddr_storage addr)
{
    printf("In print_remote_address\n");
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
        printf("Connection from %s: %s\n", address_buffer, service_buffer);

}



#endif //NET_LIB_H