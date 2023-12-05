
In this module we'll write a simple HTTP client in C. 

Require reading material:
•	[1] Lewis Van Winkle, “Hands-On Network Programming with C". Packt Publishing. May 2019. ISBN: 9781789349863. https://learning.oreilly.com/library/view/hands-on-network-programming/9781789349863/
    o	Read chapter 6.

HTTP Clients

As previously noted in module 6.1, whenever a client makes a request to a webserver it must parse the URL in order to identify the host it needs to connect to, the protocol or service, and the resource it wishes to request. 


For this module we will re-write the web_get.c program from the text book to work on Linux (remove cross-platform support) and we'll go through the HTTP portions of the code as we do this. Read tahe textbook!!

If you'd like to follow along my version, go to the code directory in the module's folder and open web_get.c

*The file netlib.h contains utility functions I use to avoid re-writing unecessary code as well as helper functions for all the different applications. 


Step 1. Setting up structures and connecting to the server.

I previously mentioned that a lot of the applications we'll write moving forward will make use of the socket functions we learned in modules 2-4, we can see that in the function below.

Our client will connect to the server using the same methods we've used before: socket() and connect()., since HTTP is built on top of TCP.


``` C 
int connect_to_host(char *hostname, char *port)
{
    
    //TRACE_ENTER();    //For debugging purposes

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
    
    //TRACE_EXIT();     //For debugging purposes
    return server;
}

```


Step 2. Craft and send HTTP requests.

This is the part of the program where we implement new logic. As per the HTTP protocol, a client can make a request to the server for any given resource hosted by the server (as long as the client is allowed to access the resource).



``` C
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

    // As covered in module 6.1 - HTTP protocol, we need to craft the HTTP request
    // in the format specified by the protocol (assuming our server adheres to the standard)
    // The first part of the request is the method, followed by the protocol and the new line characters
    sprintf(buffer, "GET /%s HTTP/1.1\r\n", path);

    //The next step is to set the "Host:" field, this will tell the remote end which website to route our request to.
    sprintf(buffer + strlen(buffer), "Host: %s:%s\r\n", hostname, port);

    
    //https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Connection
    //Connection: close Indicates that either the client or the server would like to close the connection after the
    //request has been served.
    sprintf(buffer + strlen(buffer), "Connection: close\r\n");
    
    //Set the user agent for the client
    sprintf(buffer + strlen(buffer), "User-Agent: honpwc webget 1.0\r\n");
    
    //\r\n signifies the end of the request.
    sprintf(buffer + strlen(buffer), "\r\n");

    // Send the request using the send() function.
    send(s, buffer, strlen(buffer), 0);
    printf("Sent Headers:\n%s", buffer);
}

```


Step 3. Handling the Response.

The next step is to handle the response from the server. You'll notice we're relying on standard programming techniques such as parsing text and using the normal Socket API functions previously covered, select(), etc. The bulk of the work here is understanding how the protocol behaves, what the response should look like and parsing that data according.

``` C

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

```


Step 4. Close the connection
Finally, we close the connection using the close() function.

``` C 
printf("Closing socket\n");
close(server);
server_stop();
return 0;
```

