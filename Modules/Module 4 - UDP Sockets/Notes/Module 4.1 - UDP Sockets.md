Date range: 9/11/2023 - 9/15/2023


    * UDP and TCP sockets are very similar, having learned about TCP sockets we should be able to write a UDP application after covering a few fundamental differences.
    * UDP is a connectionless, unreliable, datagram protocol
    * While TCP is a connection-oriented, reliable byte stream.

UDP sockets Notes:

    * There is no 3-way handshake with UDP.
    * There is no connect + send, instead we get sendto().  Although we can use connect() + send() it is important to note they behave differently when using UDP sockets.
    * All connect() does with a UDP socket is associate a remote address. There is no handshake or session establishment.

![Alt text](image-2.png)

###
UDP socket functions:

UDP uses recvfrom() and sendto(), these two functions are similar to send() and recv() with the addition of three additional arguments (covered below).

The recvfrom() function:

    The recvfrom() call is used to receive messages from a socket, and may be used to receive data on a socket whether or not it is connection-oriented.
    
    ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t  *addrlen);
    
    Paremeters:
        sockfd: file descriptor returned from socket().
        
        *buf: A pointer to the buffer where the received data will be stored

        len: The maximum length of the data that can be received and stored in buf.

        flags: Optional flags that can modify the behavior of the recvfrom function. 

        *src_addr: A pointer to a struct sockaddr that will be filled with the source address of the sender. In connectionless protocols like UDP, this is important for knowing where the data came from. 
            In short, this structure containes the protocol address (IP address and port number) of the system connecting/sending the data to this socket. This structure tells who sent the datagram or who initiated the connection.

        *addrlen: A pointer to a variable that specifies the size of the src_addr structure. Upon return, it contains the actual size of the source address.
    
    Returns:
        returns the length of the message on successful completion. If a message is too long to fit in the supplied buffer, excess bytes may be discarded depending on the type of socket the message is received from.

        The number of bytes received, or -1 if an error occurred. If the socket is in non-blocking mode and no data is available, recvfrom will return -1 with errno set to EAGAIN or EWOULDBLOCK.

        Writing a datagram of length 0 is acceptable. This means that a return value of 0 from recvfrom() is acceptable and does not mean the peer has closed the connection, as does the return value of 0 from  recv(). [source: Unix Network Programming + man pages]

    Notes: (man recvfrom)
       recvfrom() places the received message into the buffer buf.  The caller must specify the size of the buffer in len. If src_addr is not NULL, and the underlying protocol provides the source address of the message,  that  source  address is placed in the buffer pointed to by src_addr.  In this case, addrlen is a value‐result argument. Before the call, it should be initialized to the size of the buffer associated with src_addr. Upon return, addrlen is updated to contain the actual size of the source address. The returned address is truncated if the buffer provided is too small; in this  case, addrlen will return a value greater than was supplied to the call.

       If the caller is not interested in the source address, src_addr and addrlen should be specified as NULL.

##
The sendto() function:
    The sendto() function sends data on the socket with descriptor socket. The sendto() call applies to either connected or unconnected sockets.

    ssize_t sendto(int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen)

    Parameters:
        sockfd: file descriptor returned from socket().

        *buf: The pointer to the buffer containing the message to transmit.

        len: The length of the message in the buffer pointed to by the buf parameter.

        flags: Optional flags that can modify the behavior of the sendto function. 

        *dest_addr: A socket structure containing the protocol address of where the data is to be sent. 
            This structured call filled in by a call to recvfrom(), manually by the user or by a call to getaddrinfo().

        addrlen: The size of the dest_addr structure.

    Returns:
        On success, these calls return the number of characters sent. On error, -1 is returned, and errno is set appropriately.
        
        A value of 0 or greater indicates the number of bytes sent, however, this does not assure that data delivery was complete. A connection can be dropped by a peer socket and a SIGPIPE signal generated at a later time if data delivery is not complete.

        No indication of failure to deliver is implied in the return value of this call when used with datagram sockets.

    Notes:
        * With a UDP socket, the first time the process class sendto, if the socket has not yet had a local port bound to it, that is when an ephemeral port is chosen by the kernel for the socket. [Unix network programming, chapter 8.6, pg: 294]


###
UDP Client:

Two different ways to structure a UDP client.
    1. Using connect(), send(), recv()
       1. In this mode, the client only receives data from the per having the IP address and the port that is given to connect.
       2. This can be useful if we want to limit the hosts we want to communicate with.
    2. using sendto() and recvfrom()
       1. In this mode, recvfrom() returns data from any peer that addresses us.
       2. Can also use bind() to lock down address.

Both methods require the application calls the socket() function.

The client does not establish a connection with the server, instead the client just sends a datagram to the server using the sendto function, which requires the address of the destination (server) as a parameter. UDP makes no effort to make there are no errors or that the data makes it to its destination.

UDP Client Example (only the three socket functions mentioned above):

    int socket_a;
    socket_a = socket(family, sock_type, protocol)
    sendto(socket_a, data, data_length, flag, peer_address, peer_address_length);
    #
    recvfrom(socket_a, data, data_length, flag, peer_address, peer_address_length);


Difference between a TCP client and a UDP Client

![Alt text](image.png)


UDP Server:

UDP Server methods:
    1. UDP servers only need one socket to communicate with any number of peers.
    2. UDP servers don't require listen() and bind() to wait for and establish new connections. These functions are not used.
    3. Use bind() to bind the socket to a local address.
    4. Use select() to check/wait for data but only need to monitor one socket instead of multiple sockets like in TCP.

![Alt text](image-1.png)     


UDP Server Example (socket, bind, sendto, recvfrom)
    int socket_a;
    socket_a = socket(family, sock_type, protocol)
    bind(socket_a, bind_address, bind_address_len);
    
    #
    sendto(socket_a, data, data_length, flag, peer_address, peer_address_length);
    #
    recvfrom(socket_a, data, data_length, flag, peer_address, peer_address_length);

That covers the main differences between a TCP and UDP server program. 

How are sockets, addresses + ports bound in UDP?
    sendto() will ask the kernel for this information the first time this function is called.
    Can use connect() or bind() for this.
    Once a port is chosen by the kernel for a given host using sendto(), that port never changes for the duration of that call. That is, as long as we're communicating with that host using that given socket file descriptor.

Lost Datagrams:
    Recall, UDP is not reliable and there is no guarantee that a client or server will send or receive all the data. A typical way to prevent recvfrom() from not receiving all its intended data is to place a timeout on the client's call to recvfrom(). Though this is not an absolute solution. 

![Alt text](image-3.png)

17