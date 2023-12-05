#ifndef WEB_SERVER_H
#define WEB_SERVER_H

/*
    This header file contains all the helper functions for the
    web_server.c application.
    
    Requires "../../../netlib.h"
*/

#include "../../../netlib.h"

#define MAX_REQUEST_SIZE 2047

/*
    This struct allows us to store information about each connected client.
*/
struct client_info
{
    socklen_t address_length;               //accept() requires the address lenght as input
    struct sockaddr_storage address;         //actual address structure
    int socket;                             //client's socket.
    char request[MAX_REQUEST_SIZE + 1];     //buffer to store client's data. +1 to account for null bytes
    int received;                           //trackes number of bytes received.
    struct client_info *next;               //pointer to the next client_info node.
};

//Because of this declaration, the code is not re-entrant.
static struct client_info *clients = 0;

const char *get_content_type(const char* path)
{
    TRACE_ENTER();
    const char *last_dot = strrchr(path, '.');
    if (last_dot) {
        if (strcmp(last_dot, ".css") == 0) return "text/css";
        if (strcmp(last_dot, ".csv") == 0) return "text/csv";
        if (strcmp(last_dot, ".gif") == 0) return "image/gif";
        if (strcmp(last_dot, ".htm") == 0) return "text/html";
        if (strcmp(last_dot, ".html") == 0) return "text/html";
        if (strcmp(last_dot, ".ico") == 0) return "image/x-icon";
        if (strcmp(last_dot, ".jpeg") == 0) return "image/jpeg";
        if (strcmp(last_dot, ".jpg") == 0) return "image/jpeg";
        if (strcmp(last_dot, ".js") == 0) return "application/javascript";
        if (strcmp(last_dot, ".json") == 0) return "application/json";
        if (strcmp(last_dot, ".png") == 0) return "image/png";
        if (strcmp(last_dot, ".pdf") == 0) return "application/pdf";
        if (strcmp(last_dot, ".svg") == 0) return "image/svg+xml";
        if (strcmp(last_dot, ".txt") == 0) return "text/plain";
    }

    TRACE_EXIT();

    return "application/octet-stream";
}


/*
    @Brief:
    Searches a linked list for the given socket, s, and returns
    the client_info structure for the given client/socket.
    It creates a new client_info structure if the socket is not found.

    @Args:
        int s: the socket to search for.

    @Return:
        returns the coresponding client_info data structure if socket
        s is found.
        Creates a new linked list if the socket is not found.

*/
struct client_info *get_client(int s)
{
    TRACE_ENTER();
    struct client_info *ci = clients;

    while(ci)
    {
        if (ci->socket == s)
            break;
        ci = ci->next;
    }
    if(ci)
        return ci;
    
    //create new client_info if no matching structure was found.
    struct client_info *n = 
        (struct client_info *) calloc(1, sizeof(struct client_info));
    
    if(!n)
    {
        fprintf(stderr, "Out of memory. calloc() failed, error: (%d)\n", errno);
        exit(1);    //terminate since we don't have enough memory.
    }

    n->address_length = sizeof(n->address);     //value for accept()
    n->next = clients;                          //set to the root of global linked-list
    clients = n;                        //add n to the beginning of the linked-list
    
    TRACE_EXIT();
    return n;
}

/*
    @Brief: Searches through a linked-list of clients and removes the 
    given client.

    @Args:
        *client: The client to search for.

    @Returns: N/A.
*/

void drop_client(struct client_info *client)
{
    TRACE_ENTER();
    close(client->socket);
    
    //pointer-to-pointer that points to the global linked-list of clients to search through.
    struct client_info **p = &clients;
    while(*p)
    {
        if(*p == client)
        {
            *p = client->next;  //remove client and set *p to point to the next node
            free(client);
            TRACE_EXIT();       
            return;
        }
        p = &(*p)->next;        //next node in the list
    }

    TRACE_EXIT();
    fprintf(stderr, "drop_client not found.\n");
    exit(1);
}

const char *get_client_address(struct client_info *ci)
{
    TRACE_ENTER();
    static char address_buffer[100];
    getnameinfo((struct sockaddr*) &ci->address,
        ci->address_length,
        address_buffer,
        sizeof(address_buffer),
        0 , 0, NI_NUMERICHOST
        );
    TRACE_EXIT();
    return address_buffer;
}

/*
    @Brief: Uses the select() function to wait until either a client has data
    available or a new client is attempting to connect.

    @Args:
        server:
    
    @Returns:

*/

fd_set wait_on_client(int server)
{
    TRACE_ENTER();
    fd_set reads;
    FD_ZERO(&reads);
    FD_SET(server, &reads);
    int max_socket = server;

    struct client_info *ci = clients;

    //add client's socket to set
    while(ci)
    {
        FD_SET(ci->socket, &reads);
        if (ci->socket > max_socket)
            max_socket = ci->socket;
        ci = ci->next;
    }

    if(select(max_socket + 1, &reads, 0, 0, 0) < 0)
    {
        fprintf(stderr, "select() failed. (%d)\n", errno);
        exit(1);
    }
    TRACE_EXIT();
    return reads;
}


/*
    @Brief: Sends 400 error for the server for a given client.
    It drops(closes socket) the client.

    @Args:
        client: connected client to send the error to.
    
    @Return:
        N/A.

*/
void send_400(struct client_info *client)
{
    TRACE_ENTER();
    const char *c400 = "HTTP/1.1 400 Bad Request\r\n"
    "Connection: close\r\n"
    "Content-Length: 11\r\n\r\nBad Request";

    send(client->socket, c400, strlen(c400), 0);
    drop_client(client);
    TRACE_EXIT();

}

void send_404(struct client_info *client)
{
    TRACE_ENTER();
    const char *c404 = "HTTP/1.1 404 Not Found\r\n"
        "Connection: close\r\n"
        "Content-Length: 9\r\n\r\nNot Found";

    send(client->socket, c404, strlen(c404), 0);
    drop_client(client);
    TRACE_EXIT();
}

/*
    @brief:
        Serve requested resource to the given client.
        
*/


void serve_resource(struct client_info *client, const char *path)
{
    TRACE_ENTER();

    printf("serve_resource %s %s\n", get_client_address(client), path);


    if (strcmp(path, "/") == 0) path = "/index.html";

    if (strlen(path) > 100) {
        send_400(client);
        return;
    }

    if (strstr(path, "..")) {
        send_404(client);
        return;
    }

    char full_path[128];

    printf("doing the path thing\n");
    sprintf(full_path, "public%s", path);
    printf("Full path: %s\n", full_path);
    FILE *fp = fopen(full_path, "rb");
    if(!fp)
    {
        send_404(client);
        return;
    }

    fseek(fp, 0L, SEEK_END);
    size_t cl = ftell(fp);
    rewind(fp);

    const char *ct = get_content_type(full_path);


#define BSIZE 1024
    char buffer[BSIZE];

    sprintf(buffer, "HTTP/1.1 200 OK\r\n");
    send(client->socket, buffer, strlen(buffer), 0);

    sprintf(buffer, "Connection: close\r\n");
    send(client->socket, buffer, strlen(buffer), 0);

    sprintf(buffer, "Content-Length: %lu\r\n", cl);
    send(client->socket, buffer, strlen(buffer), 0);

    sprintf(buffer, "Content-Type: %s\r\n", ct);
    send(client->socket, buffer, strlen(buffer), 0);

    sprintf(buffer, "\r\n");
    send(client->socket, buffer, strlen(buffer), 0);

    int r = fread(buffer, 1, BSIZE, fp);
    while (r) {
        send(client->socket, buffer, r, 0);
        r = fread(buffer, 1, BSIZE, fp);
    }

    fclose(fp);
    drop_client(client);
    TRACE_EXIT();
}



#endif