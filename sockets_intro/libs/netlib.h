#ifndef NET_LIB_H
#define NET_LIB_H

//Use this header file to import all necessary libraries (similar among clients and servers) to avoid 
//code duplication

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>




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
    exit(-1);
}

#endif //NET_LIB_H