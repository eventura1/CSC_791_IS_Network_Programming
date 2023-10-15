#include "../../netlib.h"


int main(int argc, char *argv[])
{

    if(argc !=3)
    {
        fprintf(stderr, "Usage: %s <hostname|ip addr> port\n", argv[0]);
        return 1;
    }
    //on a future version, add ability to specify source address + port using bind().
    client_start();
    //After this module, we'll turn these steps into a library.
    printf("Configuring remote address ...\n");
    struct addrinfo hints, *remote_host;
    memset(&hints, 0, sizeof(hints));
    
    //let getaddrinfo() decide which is the best protocol based on what the 
    //client offers.
    //hints.ai_family = AF_UNSPEC;  

    hints.ai_socktype = SOCK_STREAM;
    // char *dst_host = argv[1];
    // char *dst_port = argv[2];

    int result = 0;
    if( getaddrinfo(argv[1], argv[2], &hints, &remote_host))
    {
        fprintf(stderr, "getaddrinfo() failed. (%s)\n", gai_strerror(result));
        return 1;
    }
    // char address_buffer[100];
    // char service_buffer[100];
    
    // printf("Calling getnameinfo()\n");
    // result = getnameinfo
    // (
    //     remote_host->ai_addr,
    //     remote_host->ai_addrlen,
    //     address_buffer,
    //     sizeof(address_buffer),
    //     service_buffer,
    //     sizeof(service_buffer),
    //     NI_NUMERICHOST|NI_NUMERICSERV
    // );

    // printf("Connecting to %s: %s\n", address_buffer, service_buffer);
    print_remote_addr_info(remote_host);
    
    //error handling
    //https://stackoverflow.com/questions/12102332/when-should-i-use-perror-and-fprintfstderr
    //1. socket()

    int socket_peer = socket(remote_host->ai_family, remote_host->ai_socktype, remote_host->ai_protocol);
    if(socket_peer == -1)
    {
        perror("socket() error. ");
        return -1;
    }
    printf("socket() successful\n");

    //2. connect()
    //establish connectin with host
    //we can observe the 3-way handshake in Wireshark or TCP dump.
    result = connect(socket_peer, remote_host->ai_addr, remote_host->ai_addrlen );
    if (result == -1)
    {
        perror("connect() error. ");
        return -1;
    }
    printf("connect() success\n");
    freeaddrinfo(remote_host);
    //could check for failure.

    //ready to send data.
    printf("Connected.\n");

    //To explore how to send data w/o select()
    //our send_data functionality will be implemented by two different functions
    //so we don't have to write another client.


    // printf("To send data, enter text followed by enter.\n");
    
    /*
        With this implementation //approach
        send()/recv() block.
        So we have to wait for send() to send data out
        and for recv() to receive data before our program can continue.

    */
    // int  input_size = 1024;
    // char input[input_size];
    // unsigned char out_buffer[input_size];
    // while(1)
    // {
    //     // printf("input buff: %s\n", input);
    //     // printf("clearing memory\n");
    //     // memset(input, '\0', sizeof(input));
    //     // printf("cleared memory. buff: %s\n", input);
    //     printf("input buff: %s\n", input);
    //     printf("To send data, enter text followed by enter.\n");
        
    //     if(fgets(input, sizeof(input), stdin) == NULL)
    //     {
    //         perror("fgets");
    //         break;
    //     }
        
    //     printf("input buffer: %s\n", input);
    //     size_t slen = strlen(input);
    //     if (slen > 0)
    //     {
    //         printf("stripping new line.\n");
    //         if (input[slen-1] == '\n')
    //             input[slen-1] ='\0';
    //     }
    //     if(strcmp(input, "qq") == 0)
    //     {
    //         printf("[*] Disconnect signal received.\n");
    //         break;
    //     }

    //     printf("[*] Processing input\n");
    //     if(send_data(socket_peer, input) == 0)
    //         break;
        
    //     printf("[*] Receving data\n");
    //     if(recv_data(socket_peer, out_buffer) == 0)
    //         break;
    // }

    // if(!send_recv_loop(socket_peer))
    //     printf("send/recv function failure.\n");


    //implementation using select().
    select_loop(socket_peer);
    //after running select_loop() we can interact with our program without 
    //having to wait for either input to be read in or data to be received.

    //implementation using poll().
    //tear down connection.
    //we can observe the 4-way tear down process in Wireshark or TCP dump.
    printf("Closing socket.\n");
    if(close(socket_peer))
    {
        //inform the user but don't do anything.
        perror("close() error: ");
    }
    
    client_stop();
    return 0;
}