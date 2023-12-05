#include "../../../netlib.h"

#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

//#include <openssl/ssl.h>

/******
 * A simple time server that displays the time to an HTTPS client.
 * This program is an adaptation of time_server.c from Chapter 2, Getting to Grips with
 * Socket APIs, which served the time over plain HTTP.
*/


void get_ssl_version()
{
    printf("[*] OpenSSL version: %s\n", OpenSSL_version(SSLEAY_VERSION));
}


int main()
{
    server_start("8443");
    get_ssl_version();

    //Step 1: Initialize SSL
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    

    //Step 2: Create SSL context
    SSL_CTX *ctx = SSL_CTX_new(TLS_server_method());
    if(!ctx)
    {
        fprintf(stderr, "SSL_CTX_new() failed.\n");
        return 1;
    }

    //Step 3: Specify certificates and keys.
    if(!SSL_CTX_use_certificate_file(ctx, "cert.pem", SSL_FILETYPE_PEM) ||
        !SSL_CTX_use_PrivateKey_file(ctx, "key.pem", SSL_FILETYPE_PEM))
    {
        fprintf(stderr, "SSL_CTX_use_certificate_file() failed.\n");
        ERR_print_errors_fp(stderr);
        return 1;
    }
    
    //Step 4: Make TCP connection

    printf("Configuring local address...\n");
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    struct addrinfo *bind_address;
    getaddrinfo(0, "8443", &hints, &bind_address);


    printf("Creating socket...\n");
    int socket_listen;
    socket_listen = socket(bind_address->ai_family,
            bind_address->ai_socktype, bind_address->ai_protocol);
    if (socket_listen == -1) {
        fprintf(stderr, "socket() failed. (%d)\n", errno);
        return 1;
    }


    printf("Binding socket to local address...\n");
    if (bind(socket_listen,
                bind_address->ai_addr, bind_address->ai_addrlen)) {
        fprintf(stderr, "bind() failed. (%d)\n", errno);
        return 1;
    }
    freeaddrinfo(bind_address);


    printf("Listening...\n");
    if (listen(socket_listen, 10) < 0) {
        fprintf(stderr, "listen() failed. (%d)\n", errno);
        return 1;
    }

    while(1)
    {
        printf("Waiting for connection ...\n");
        struct sockaddr_storage client_address;
        socklen_t client_len = sizeof(client_address);

        int socket_client = accept(socket_listen,
                     (struct sockaddr*)&client_address, &client_len);

        if(socket_client == -1)
        {
            fprintf(stderr, "accept() failed. (%d)\n", errno);
            return 1;
        }
        print_remote_address(client_address);

        //Step  5 - Initialize TLS connection
        SSL *ssl = SSL_new(ctx);
        if(!ctx)
        {
            fprintf(stderr, "SSL_new() failed.\n");
            return 1;
        }

        SSL_set_fd(ssl, socket_client);
        if(SSL_accept(ssl) <= 0)
        {
            fprintf(stderr, "SSL_accept() failed.\n");
            ERR_print_errors_fp(stderr);

            SSL_shutdown(ssl);
            close(socket_client);
            SSL_free(ssl);
            
            continue;
        }
        printf("SSL connection using %s\n", SSL_get_cipher(ssl));

        printf("Reading request ...\n");
        char request[1024];
        int bytes_received = SSL_read(ssl, request, 1024);
        printf("Received: %d bytes\n", bytes_received);

        printf("Sending response ...\n");
         const char *response =
            "HTTP/1.1 200 OK\r\n"
            "Connection: close\r\n"
            "Content-Type: text/plain\r\n\r\n"
            "Local time is: ";
        
        int bytes_sent = SSL_write(ssl, response, strlen(response));
        printf("Sent: %d of %d bytes.\n", bytes_sent, (int)strlen(response));
        time_t timer;
        time(&timer);
        char *time_msg = ctime(&timer);
        bytes_sent = SSL_write(ssl, time_msg, strlen(time_msg));
        printf("Sent %d of %d bytes.\n", bytes_sent, (int)strlen(time_msg));

        printf("Closing connection...\n");
        SSL_shutdown(ssl);
        close(socket_client);
        SSL_free(ssl);

    }


    //****CLEAN UP*****//
    printf("Closing resources ...\n");
    close(socket_listen);
    SSL_CTX_free(ctx);
    server_stop();
    return 0;
}