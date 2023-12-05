#
Chapter 3 Lewis Van Winkle + other resources as needed.

    Chapter 3 will provide a deeper technical understanding (through the material + additional research). This in turn will allow me to go back to weeks 1-2 to write the walk-through with a better understanding of the basic topics. 
        OSI & TCP/IP Layer
        Intro to sockets
            basics of UDP and TCP sockets
            implementing a basic TCP server + client.
            implementing a basic UDP server + client.
The following topics are covered in this chapter:
    Configuring a remote address with getaddrinfo()
    Initiating a TCP connection with connect()
    Detecting terminal input in a non-blocking manner
    Multiplexing with fork()
    Multiplexing with select()
    Detecting peer disconnects
    Implementing a very basic microservice
    The stream-like nature of TCP
    The blocking behavior of send()


Blocking APIs/Functions:

In computer programming, the term "blocking" and "API functions" can be related but refer to different concepts. Let me explain each concept separately:

Blocking:

    Blocking refers to a situation in which a program or a specific operation halts or pauses the execution of the program until a certain condition is met or a task is completed. During this time, the program may appear unresponsive because it's waiting (sleeping) for something to happen.

    Blocking can occur in various scenarios, such as:

    Blocking I/O: When a program performs input/output operations (like reading from a file or waiting for user input), it may block the execution until the operation is completed.
    
    Blocking Synchronization: When multiple threads or processes need to coordinate their work, they may block each other while waiting for certain resources to become available.
    
    Blocking in Network Communication: In networking, when a program sends or receives data over the network, it may block until the data transfer is finished or a timeout occurs.
    
    Blocking can be problematic in certain situations, especially in applications that require responsiveness. In these cases, developers often use techniques like asynchronous programming or multithreading to avoid blocking and keep the application responsive.

Explain why polling in code is not efficient>


Multiplexing TCP connection: We cover different ways to multiplex socket connections.

    The socket APIs block by default.
        Accept() blocks while waiting for incoming connection / until a new connection is available.
        Recv() blocks while waiting for new data to become available for reading.

        Blocking can be problematic in certain situations, especially in applications that require responsiveness.
            Our server would only be able to serve one client / connection at a time.
            Servers (TCP/UDP, HTTP) serving more than one client cannot block while only one client is connecting/sending/receiving data.
            Applications usually need to server multiple clients at a time/manage several connections simultaneously.

            The same applies to client applications. A browser, for example, needs to be able to handle multiple connections (multiple tabs) to different websites, or a single web browser needs to be able to perform multiple actions when connecting to a server (downloading files, loading images, scripts, etc.)


    Polling non-blocking sockets:
        //There is more data about this on BG's
        Polling in computer programming refers to a technique where a program or process periodically checks the status or condition of a resource, data, or event to determine whether it has changed or meets a certain condition. This technique is often used when a program needs to monitor external events or resources asynchronously. 

        We can configure sockets to use non-blocking operations.
            Linux: 
                fcntl() with the O_NONBLOCK flag.

            Windows:
                ioctlsocket() with the FIONOBIO flag.
        
        //How is poll() different than polling?
        Polling can be a waste of computer resources since most of the time there will be no data to read and also adds to the program's complexity.

            poll() takes an array of struct pollfds with information about which socket descriptors we want to monitor for.
            The OS blocks on poll() until one of those events occurs


    The select() function:
        Supportd by both Berkely and Windows sockets.

        select() gives you the power to monitor several sockets at the same time.
            it notifies the application which sockets are ready for reading, writing, and those which have raised exceptions.
            select() is slow but portable. Slow when it comes to giant numbers of connections.
                look into libevent* 

        Given a set of sockets, it can be used to block until any of the sockets in that set is ready to be read from (written to?)

        Can be configured to return after a "time-out" period if none of the monitored events take place. (see below)

        The select() function definition
        int select(int numfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);

            This function monitors "sets" of file descriptors; in particular: readfs, writefds, exceptfds. For read, write, and exceptions, respectively.

            numfds: should be set to the values of the highest file descriptor plus one.(Why?)
                The reason for specifying nfds as the highest file descriptor plus one is related to the way the select() function's implementation handles bitsets (fd_set structures) internally.

                Bitsets: Internally, select() uses bitsets to represent file descriptors. Each bit corresponds to a file descriptor, and the select() function checks the bits to determine which file descriptors have events pending. By specifying nfds as one greater than the highest file descriptor, it ensures that the bitset is large enough to represent all file descriptors you intend to monitor.


            when select returns, readfds will be modified to reflect which of the file descriptors you select is ready for reading.
                can use FD_ISSET() to test them.
                After select() returns, the values in the sets will be changed to show which are ready for reading or writing, and which have exceptions.
                
                On success, select() itself returns the number of socket descriptors contained in the (up to) three descriptor sets it monitored. The return value is zero if it timed out before any sockets were readable/writeable/excepted. select() returns -1 to indicate an error.


            Each of these sets is of type fd_set.
            Any or all of these parameters can be NULL if you’re not interested in those types of events.
            
            The first parameter, numfds is the highest-numbered socket descriptor (they’re just ints, remember?) plus one.
                s1 = socket(...);
                s2 = socket(...);
                FD_SET(s1, &readfds);
                FD_SET(s2, &readfds);

                // since we got s2 second, it's the "greater", so we use that to indicate the numfds param in select().
                n = s2+1
                select( n, ...);

            //Macros - functions to manipulate fd_set structs.

            Function                        Description
            FD_SET(int fd, fd_set *set);    Add fd to the set.
            FD_CLR(int fd, fd_set *set);    Remove fd from the set.
            FD_ISSET(int fd, fd_set *set);  Return true if fd is in the set.
            FD_ZERO(fd_set *set);           Clear all entries from the set.

            struct timeval:
                A time structure that allows specifying a time-out period for select/fd_sets.
                Select returns if time-out period exceeds before any file descriptor is ready.

                struct timeval {
                    int tv_sec;         // seconds
                    int tv_usec;        // microseconds
                    };
                _micro_seconds, not milliseconds. There are 1,000 microseconds in a millisecond, and 1,000 milliseconds in a second. Thus, there are 1,000,000 microseconds in a second.
                    //have select wait 1.5 seconds:
                        tv_sec = 1;
                        tv_usec = 500000;
                
                Other things of interest: If you set the fields in your struct timeval to 0, select() will timeout immediately, effectively polling all the file descriptors in your sets.

                If you set the parameter timeout to NULL, it will never timeout, and will wait until the first file descriptor is ready.
                
                Finally, if you don’t care about waiting for a certain set, you can just set it to NULL in the call to select().

        One more note of interest about select(): if you have a socket that is listen()ing, you can check to see if there is a new connection by putting that socket’s file descriptor in the readfds set.
        
        RETURN VALUE
            On success, select() and pselect() return the number of file descriptors contained in the three returned
            descriptor  sets  (that is, the total number of bits that are set in readfds, writefds, exceptfds).  The
            return value may be zero if the timeout expired before any file descriptors became ready.

            On error, -1 is returned, and errno is set to indicate the error; the file descriptor sets  are  unmodi‐
            fied, and timeout becomes undefined.

        Using the select() function:
        1. Must first add our sockets into an fd_set.
            SOCKET socket_listen, socket_a, socket_b;
            
            //declare an fd_set variable
            fd_set our_sockets;
            FD_ZERO(&our_sockets);                          //it's important to zero-out the fd_set

            //Note: *sockets are added to a fd_set*
            FD_SET(socket_listen, &our_sockets);            //add socket_listen to the our_socket set.
            FD_SET(socket_a, &our_sockets);
            FD_SET(socket_b, &our_sockets);

            //select() also requires that we pass a number that's larger than the largest socket descriptor we are going to monitor.
            SOCKET max_socket;
            max_socket = socket_listen;
            if (socket_a > max_socket) max_socket = socket_a;
            if (socket_b > max_socket) max_socket = socket_b;

            When we call select(), it modified our fd_set of sockets to indicate which sockets are ready.

            It is recommended to make a copy of our socket set before calling select().
            fd_set copy;
            copy = our_sockets;         //do they point to the same or are they different objects?
            
            //the select call blocks until at least one of the sockets is ready to be read from.
            //Only copy is modified when select() returns and only contains sockets that are ready to be read from.
            //the argument position for the given FD determines what select will monitor the sockets for :read, write, or exceptions.
            select(max_socoket+1, &copy, 0, 0, 0);

            //checks if socket_listen is in the set of ready FDs copy.
            if (FD_ISSET(socket_listen, &copy))
                //perform action.
            
        Iterating through an FD_SET:
            Can use a simple for loop but must start at 1 not 0.
            For each possible socket descriptor, we simply use FD_ISSET() to check if the socket descriptor is in the set.

            SOCKET i;
            for (i = 1; <= max_socket;i++)
            {
                //if (FD_ISSET(socket_descriptor_id, FD_SET))
                if (FD_ISSET(i, &master))
                {
                    //perform action
                }
            }


The poll() function:
Synchronous I/O Multiplexing

    
Forking and multi-threading.
    Do more research on the fork and threading models.


select(), poll(), epoll()

select is preferred over epoll and poll (older implementations of them anyways) because they were not portable between Berkeley sockets and Winsock (Unix / Windows)