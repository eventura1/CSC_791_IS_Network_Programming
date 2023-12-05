#include "../../../netlib.h"

#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

//#include <openssl/ssl.h>

void get_ssl_version()
{
    printf("OpenSSL version: %s\n", OpenSSL_version(SSLEAY_VERSION));
}


int print_server_certificate(SSL *ssl)
{
    TRACE_ENTER();
    int ret = 0;

    X509 *cert = SSL_get_peer_certificate(ssl);
    if(!cert)
    {
        fprintf(stderr, "SSL_get_peer_certificate() failed.\n");
        return ret;
    }

    char *tmp;
    if((tmp = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0)))
    {
        printf("subject: %s\n", tmp);
        OPENSSL_free(tmp);
        ret = 1;
    }
    
    if((tmp = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0)))
    {
        printf("issuer: %s\n", tmp);
        OPENSSL_free(tmp);
        ret = 1;
    }
    
    X509_free(cert);
    TRACE_EXIT();
    return ret;
}

#define BUFFER_SIZE 2048
typedef struct TLS_SERVER_OBJECT
{
    char *hostname;
    char *port;
    int server_socket;
    char data[BUFFER_SIZE + 1];
    SSL *ssl;
} tls_obj, *tls_obj_ptr;

tls_obj_ptr make_tls_info_objc(const char *hostname, const char *port, SSL *ssl, int server_socket)
{
    TRACE_ENTER();
    tls_obj_ptr server_obj = (tls_obj_ptr)calloc(1, sizeof(tls_obj));
    if (!server_obj)
    {
        fprintf(stderr, "Out of memory.\n");
        exit(1);
    }

    //struct tls_obj *obj = (struct tls_obj*) calloc(1, sizeof(struct TLS_SERVER_OBJECT));

    printf("Setting hostname ...\n");
    server_obj->hostname = strdup(hostname);

    printf("Setting port ...\n");
    server_obj->port = strdup(port);

    printf("Setting socket ...\n");
    server_obj->server_socket = server_socket;

    printf("Setting ssl ...\n");
    server_obj->ssl = ssl;

    memset(server_obj->data, 0, sizeof(server_obj->data));  // Clear the data buffer

    TRACE_EXIT();
    return server_obj;
}

// Function to free the memory allocated for the struct
void freeTLSObject(tls_obj_ptr server_obj)
{
    TRACE_ENTER();
    if (!server_obj)
    {
        return;
    }

    free(server_obj->hostname);
    free(server_obj->port);
    free(server_obj);
    TRACE_EXIT();
}

void print_tsl_info_obj(tls_obj_ptr server_obj)
{
    TRACE_ENTER();
    printf("Hostname: %s\n", server_obj->hostname);
    printf("Port: %s\n", server_obj->port);
    printf("Socket_FD: %d\n", server_obj->server_socket);
    printf("SSL object: 0x%p\n", server_obj->ssl);
    printf("Object address: 0x%p\n", server_obj);
    TRACE_EXIT();
}

int send_HTTPS_request( tls_obj_ptr server_object)
{
    TRACE_ENTER();
    //server_object->data
    char buffer[BUFFER_SIZE];
    sprintf(buffer, "GET / HTTP/1.1\r\n");
    sprintf(buffer + strlen(buffer), "Host: %s:%s\r\n", server_object->hostname, server_object->port);
    sprintf(buffer + strlen(buffer), "Connection: close\r\n");
    sprintf(buffer + strlen(buffer), "User-Agent: https_simple\r\n");
    sprintf(buffer + strlen(buffer), "\r\n");

    int ret_val = SSL_write(server_object->ssl, buffer, strlen(buffer));
    if(ret_val <= 0)
    {
        fprintf(stderr, "SSL_write() failed. Error: %d", SSL_get_error(server_object->ssl, ret_val));
        return ret_val;
    }
    printf("Sent Headers:\n%s\nsize of headers: %ld\n", buffer, strlen(buffer));

    TRACE_EXIT();
    return ret_val;

}

int main(int argc, char *argv[])
{
    
    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s hostname port\n", argv[0]);
        return 1;
    }

    client_start();
    
    get_ssl_version();

    //Step 1: Initialize OpenSSL library
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();


    //Step 2: Create SSL context
    //An SSL context is an object holding various SSL configurations.

    SSL_CTX *ctx = SSL_CTX_new(TLS_client_method());
    if(!ctx)
    {
        fprintf(stderr, "SSL_CTX_new() failed.\n");
        return 1;
    }

    //Step 3: create a TCP connection.

    char *hostname = argv[1];
    char *port = argv[2];
    
    printf("Configuring remote address ...\n");
    struct addrinfo hints, *peer_address;
    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;
    if (getaddrinfo(hostname, port, &hints, &peer_address))
    {
        fprintf(stderr, "getaddrinfo() failed. Error: %d\n", errno);
        exit(1);
    }
    print_remote_addr_info(peer_address);
    
    printf("Creating socket ...\n");
    int server = socket(peer_address->ai_family, peer_address->ai_socktype, peer_address->ai_protocol);
    if(!server)
    {
        fprintf(stderr, "socket() failed. Error: %d\n", errno);
        exit(1);
    }

    printf("Connecting ...\n");
    int result = connect(server, peer_address->ai_addr, peer_address->ai_addrlen);
    int errornumber = errno;
    if(!handle_socket_err("connect", result, errornumber))
        return 1;
    
    freeaddrinfo(peer_address);
    //printf("Connected to remote server.\n");

    //Step 4: Initialize TLS connection.
    //At this point, our TCP connection has been established and we're ready to start negotiating
    //the TLS connection.

    //Create a new SSL object used to track the new SSL/TLS connection.
    SSL *ssl = SSL_new(ctx);
    if(!ctx)
    {
        fprintf(stderr, "SSL_new() failed.\n");
        return 1;
    }

    //set the server name (SNI field)
    if(!SSL_set_tlsext_host_name(ssl, hostname))
    {
        fprintf(stderr, "SSL_set_tlsext_host_name() failed.\n");
        return 1;
    }
    
    //set the file descriptor for the TLS side of the encrypted channel
    SSL_set_fd(ssl, server);

    //Initiate the TLS switch-over/connection.
    //this call initiates the SSL handshake.
    if(SSL_connect(ssl) == -1)
    {
        fprintf(stderr, "SSL_connect() failed.\n");
        ERR_print_errors_fp(stderr);
        return 1;
    }

    //print cipher suite used by the connection
    printf("SSL/TLS using %s\n", SSL_get_cipher(ssl));

    //Display the server's certificate
    if(!print_server_certificate(ssl))
    {
        fprintf(stderr, "print_server_certificate() failed.\n");
        return 1;
    }
    
    printf("making tls_object ...\n");
    tls_obj_ptr my_ssl = make_tls_info_objc(hostname, port, ssl, server);
    print_tsl_info_obj(my_ssl);

    result = send_HTTPS_request(my_ssl);
    if(!result)
        fprintf(stderr, " send_HTTPS_request() failed.\n"); //alert user only

    while(1)
    {
        int bytes_received = SSL_read(my_ssl->ssl, my_ssl->data, sizeof(my_ssl->data));
        if(bytes_received < 1)
        {
            printf("\nConnection closed by peer.\n");
            break;
        }
        printf("Received (%d bytes): \n'%.*s'\n",
                bytes_received, bytes_received, my_ssl->data);
    }

    //******CLEAN-UP********/
    freeTLSObject(my_ssl);
    printf("Closing socket ...\n");
    SSL_shutdown(ssl);
    close(server);
    SSL_free(ssl);
    SSL_CTX_free(ctx);
    client_stop();
    return 0;
}