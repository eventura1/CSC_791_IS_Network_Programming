/*
    web_get is a simple HTTP web client that connects to a remote host and performs 
    a GET request for the given resource.


*/
#include "../../../netlib.h"

#define TIMEOUT 5.0         //handles a response taking too long.
#define BUFFER_SIZE 2048


/*
    @Brief: Builds and sends an HTTP request.

    Args:
        int s: An active/open socket to send the data over.
        hostname: name of the server.
        port: port to connect to.
        path: requested resource.

    Returns: N/A.
*/

void send_request(int s, char *hostname, char *port, char *path)
{
    char buffer[BUFFER_SIZE];
    
    sprintf(buffer, "GET /%s HTTP/1.1\r\n", path);
    sprintf(buffer + strlen(buffer), "Host: %s:%s\r\n", hostname, port);
    sprintf(buffer + strlen(buffer), "Connection: close\r\n");
    sprintf(buffer + strlen(buffer), "User-Agent: honpwc webget 1.0\r\n");
    
    //\r\n signifies the end of the request.
    sprintf(buffer + strlen(buffer), "\r\n");

    send(s, buffer, strlen(buffer), 0);
    printf("Sent Headers:\n%s", buffer);
}

/*
    @Brief: Helper function that takes in a hostname and a port number and attempts to establish
    a new TCP socket connection to it.

    @Args:
        hostname: remote server to connect to.
        port: port to connect to.
    
    @Returns:
        An open socket used to connect to send data over.

*/
int connect_to_host(char *hostname, char *port)
{
    TRACE_ENTER();
    printf("Configuring remote address ...\n");
    struct addrinfo hints, *peer_address;
    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;
    if (getaddrinfo(hostname, port, &hints, &peer_address))
    {
        fprintf(stderr, "getaddrinfo() failed. (%d)\n", errno);
    }

    print_remote_addr_info(peer_address);

    int error_num = 0;
    int result = 0;
    printf("Creating socket ...\n");
    int server = socket(peer_address->ai_family, peer_address->ai_socktype, peer_address->ai_protocol);
    error_num = errno;
    if(!handle_socket_err("socket", server, error_num))
        return 1;

    printf("Connecting ...\n");
    result = connect(server, peer_address->ai_addr, peer_address->ai_addrlen);
    error_num = errno;
    if(!handle_socket_err("socket", result, error_num))
        return 1;

    freeaddrinfo(peer_address);
    printf("Connected.\n\n");
    TRACE_EXIT();
    return server;
}


int main(int argc, char *argv[])
{
    
    if (argc < 2) {
        fprintf(stderr, "usage: web_get url\n");
        return 1;
    }
    server_start("8080");
    char *url = argv[1];

    char *hostname, *port, *path;
    parse_url(url, &hostname, &port, &path);
    int server = connect_to_host(hostname, port);
    send_request(server, hostname, port, path);

    //Time request's time
    const clock_t start_time = clock();

    //Logic to parse an HTTP response.
    
    #define RESPONSE_SIZE 8192
    char response[RESPONSE_SIZE + 1];
    char *p = response, *q;
    char *end = response + RESPONSE_SIZE;
    char *body = 0;

    enum { length, chunked, connection};
    int encoding = 0;
    int remaining = 0;
    

    //receive and process HTTP response.
    while(1)
    {
        if( (clock() - start_time) / CLOCKS_PER_SEC > TIMEOUT )
        {
            fprintf(stderr, "timeout after %.2f seconds\n", TIMEOUT);
            return 1;
        }

        if(p == end)
        {
            fprintf(stderr, "out of buffer space\n");
            return 1;
        }
        fd_set reads;       //create set
        FD_ZERO(&reads);    //clear set
        FD_SET(server, &reads);  //add socket to set
        struct timeval timeout; 
        timeout.tv_sec = 0;
        timeout.tv_usec = 200000;
        
        if(select(server + 1, &reads, 0, 0, &timeout) < 0)
        {
            fprintf(stderr, "select() failed. (%d)\n", errno);
            return 1;
        }

        //check for our FD_SET being set
        if (FD_ISSET(server, &reads))
        {
            int bytes_received = recv(server, p, end - p, 0);
            if (bytes_received < 1)
            {
                if(encoding == connection && body)
                {
                    printf("[*] Econding: Connection[*]\n");
                    printf("%.*s", (int) (end - body), body);
                }

                printf("\nConnection closed by peer.\n");
                break;
            }

            printf("[*]Received (%d bytes): '%.*s'", bytes_received, bytes_received, p);

            p += bytes_received;
            *p = 0;
            //search for the end of the HTTP header (\r\n\r\n)
            if(!body && (body = strstr(response, "\r\n\r\n")))
            {
                *body = 0;
                body += 4;
                printf("[*]Received Headers:\n%s\n", response);

                q = strstr(response, "\nContent-Length: ");
                if(q)
                {
                    encoding = length;
                    q = strchr(q, ' ');
                    q += 1;
                    remaining = strtol(q, 0, 10);
                }
                else
                {
                    q = strstr(response, "\nTransfer-Conedoing: chunked");
                    if(q)
                    {
                        encoding = chunked;
                        remaining = 0;
                    }
                    else
                    {
                        encoding = connection;
                    }
                }
                printf("\nReceived Body:\n");
            }

            if (body)
            {
                if(encoding == length)
                {
                    if(p - body >= remaining) 
                    {
                        printf("%.*s", remaining, body);
                        break;
                    }
                }
                else if (encoding == chunked)
                {
                    do
                    {
                        if (remaining == 0)
                        {
                            if( (q = strstr(body, "\r\n")))
                            {
                                remaining = strtol(body, 0, 16);
                                if (!remaining)
                                    goto finish;
                                body = q + 2;
                            }
                            else
                            {
                                break;
                            }
                                
                        }
                        if (remaining && p - body >= remaining)
                        {
                            printf("%.*s", remaining, body);
                            body += remaining + 2;
                            remaining = 0;
                        }
                    } while (!remaining);
                    
                }
            }

        }
    }
finish:
    printf("Closing socket\n");
    close(server);
    server_stop();
    return 0;

}
