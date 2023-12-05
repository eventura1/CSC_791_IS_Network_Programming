//#include "../../../netlib.h"
#include "web_server.h"

//Because of this declaration, the code is not re-entrant.
//static struct client_info *clients = 0;

int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        printf("Usage: %s <port>\n", argv[0]);
        return 1;
    }

    char *port = argv[1];
    server_start(port);
    
    int server = create_server_socket(0, port);
    int error_number = 0;
    while(1)
    {

        fd_set reads;
        reads = wait_on_client(server);
        
        if(FD_ISSET(server, &reads))
        {
            struct client_info *client = get_client(-1);

            client->socket = accept(server, (struct sockaddr *) &(client->address),
            &(client->address_length));

            error_number = errno;
            if(!handle_socket_err("accept", client->socket, error_number))
                return 1;
            
            printf("New connection from: %s.\n", get_client_address(client));
        }

        struct client_info *client = clients;
        while(client)
        {
            struct client_info *next = client->next;

            if(FD_ISSET(client->socket, &reads))
            {
                if(MAX_REQUEST_SIZE == client->received)
                send_400(client);
                client = next;
                continue;
            }

            int r = recv(client->socket, client->request + client->received, 
                        MAX_REQUEST_SIZE - client->received, 0);
            
            if(r < 1)
            {
                printf("Unexpected disconnect from %s.\n", get_client_address(client));
                drop_client(client);
            }
            else {
                    printf("IN ELSE\n");
                    client->received += r;
                    client->request[client->received] = 0;

                    char *q = strstr(client->request, "\r\n\r\n");
                    if (q) {
                        *q = 0;

                        if (strncmp("GET /", client->request, 5)) {
                            send_400(client);
                        } else {
                            char *path = client->request + 4;
                            char *end_path = strstr(path, " ");
                            if (!end_path) {
                                send_400(client);
                            } else {
                                *end_path = 0;
                                serve_resource(client, path);
                            }
                        }
                    } //if (q)
                }
            client = next;
        }//end while(client)

    }   //end while(1)

    printf("Closing socket ...\n");
    close(server);
    server_stop();
    return 0;
}