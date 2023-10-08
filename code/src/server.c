#include "../../netlib.h"

#define PORT "3490"     //listen port


int main()
{
    server_start(PORT);
    
    //free linked list
    freeaddrinfo(servinfo);

    /////////
    server_stop();
    return 0;
}