
    *Provide a brief review of the OSI and TCP/IP Layer.
        Provide links to more in-depth documents.
        Provide slides and excerpts from HAO or other books.
    
    * Overview IPv4 and IPv6 and ports.

    *Introduction to what socket programming is, provide an overview of what we aim to accomplish, etc.

    *Code layout for the semester.

    *Environment setup.

Introduction:
    In this section we go over what is network programming and sockets are.

Goals for this Module:
    Learn what socket programming and sockets are.

Required reading material:
    * If you have a solid foundation of the OSI layer, IP addresses, and port numbers you could skim through the reading materials.

        Read chapter 2 (it's about 4-5 pages, short and to the point)
        Read chapter 3, sections 3.1, Pages: 10-12.
            "Beej's Guide to Network Programming: Using Internet Sockets", by Brian "Beej Jorgensen" Hall, independently published April 2023.
            https://beej.us/guide/bgnet/
        
        Read chapter 0x04, pages: 195 - 198. OSI Model and Sockets.
            "Hacking The Art of Exploitation, 2nd Edition", by Jon Erickson. No Starch press, 2008.
                https://learning.oreilly.com/library/view/hacking-the-art/9781593271442/

        Read chapter 1, pages: 8 -31 and chapter 2, pages: 41 -45:
            "Hands-On Network Programming with C" Lewis Van Winkle. Packt Publishing, 2019.
                https://learning.oreilly.com/library/view/hands-on-network-programming/9781789349863/

    Additional reading:
        This book provides a more in-depth/technical explanation for the the topics covered in this module:
            "UNIX Network Programming: The sockets networking API, 3/e", by W. Richard Stevens, Bill Fenner, and Andrew M. Rudoff. ISBN-10: 0131411551. ISBN-13: 978-0131411555. Addison-Wesley.
            https://learning.oreilly.com/library/view/the-sockets-networking/0131411551/

            Read chapter 1, sections: 1.1, 1.7
            Read chapter 1, sections: 2.1 - 2.4


What is network programming?

    Network programming is the act of using computer code to write programs or processes that can communicate with other programs or processes across a network. Programmers use various programming languages, code libraries, and protocols to do the work.
    https://www.cisco.com/c/en/us/solutions/enterprise-networks/what-is-network-programming.html


What is a socket?
    A socket is a communications connection point (endpoint) that you can name and address in a network. Socket programming shows how to use socket APIs to establish communication links between remote and local processes.

    The processes that use a socket can reside on the same system or different systems on different networks. Sockets are useful for both stand-alone and network applications. Sockets allow you to exchange information between processes on the same machine or across a network, distribute work to the most efficient machine, and they easily allow access to centralized data. Socket application program interfaces (APIs) are the network standard for TCP/IP. A wide range of operating systems support socket APIs. i5/OS sockets support multiple transport and networking protocols. Socket system functions and the socket network functions are threadsafe.
    
    https://www.ibm.com/docs/en/i/7.1?topic=communications-socket-programming


[1] Chapter 4, pg: 198-199.

    In simple terms, a socket is one endpoint of a communication link between systems. Your application sends and receives all of its network data through a socket.

    A socket is a standard way to perform network communication through the Operating System. A socket can be thought of as an endpoint to a connection, similar to the telephone system.
    
    Sockets are programmer's abstractions that take care of all the low-level (think OSI or TCP/IP) protocol / layer details. A programmer uses a socket to send or receive data over a network and this data is then transmitted through the OSI layer following a similar path of: session -> transport -> application -> network -> data link, and finally -> Physical layer. All of these steps are abstracted and transparent to a programmer through those APIs.

    Although there are multiple types of sockets, the two most commons are stream and datagram sockets, corresponding to TCP and UDP respectively. There are also Raw sockets, while not common in standard applications, these are used in applications such as Scapy, Wireshark and similar applications. Raw sockets allow a programmer to send, receive, and inspect network traffic. <EXPAND ON THIS MORE>

Socket Types:

    Stream Sockets: 
    [1] Chapter 4, pg: 198-199.

        "Stream sockets provide reliable two-way communication similar to when you call someone on the phone." 

        Use IP (Internet Protocol) for Routing.

        Stream sockets provide reliabled two-way communication where one side initiates the connection to the other, and after the connection is established, either side can communicate to the other. In addition, there is immediate confirmation that what you transmitted actually reached its destination.

        Implements the known 3-way handshake

        Stream sockets are implemented in the Transmission Control Protocol (TCP), which maps to layer 4 (Transport Layer) of the OSI model.
            Connection-oriented, Reliability, error-free, and all other benefits of stream sockets are provided by the Transmission Control Protocol (TCP).

        Stream socket protocols:
            HTTP/S, SSH, Telnet, and so on. If it requires reliability, error-free, etc. it likely uses stream sockets.

    Datagram Sockets: 
    [1] Chapter 4, pg: 198-199.

        "Communicating with a datagram socket is more like mailing a letter than making a phone call."

        Use IP (Internet Protocol) for Routing.
        
        Commonly referred to as "connectionless" sockets because they do not maintain an open connection like stream sockets do. They are more of the fire and forget variety, allowing us to build a packet and send out to its destination. As such,
            * Less overhead involved compared to stream sockets.
            * Better suited for gaming, video, and voice applications where loss of small amount of packets is not important.
        
        Stream sockets are implemented in the User Datagram Protocol (TCP), which maps to layer 4 (Transport Layer) of the OSI model.


        There are a few different socket application programming interfaces (APIs). The first were Berkeley sockets, which were released in 1983 with 4.3BSD Unix. The Berkeley socket API was widely successful and quickly evolved into a de facto standard. From there, it was adopted as a POSIX standard with little modification. The terms Berkeley sockets, BSD sockets, Unix sockets, and Portable Operating System Interface (POSIX) sockets are often used interchangeably.

        Windows' socket API is called Winsock. It was created to be largely compatible with  Berkeley sockets. In this book, we strive to create cross-platform code that is valid for both Berkeley sockets and Winsock.
        
        [2] Chapter 2. Pg: 41 

Sources:

[1] "Hacking The Art of Exploitation, 2nd Edition", by Jon Erickson. No Starch press, 2008.
        https://learning.oreilly.com/library/view/hacking-the-art/9781593271442/

[2] "Hands-On Network Programming with C" Lewis Van Winkle. Packt Publishing, 2019.
        https://learning.oreilly.com/library/view/hands-on-network-programming/9781789349863/

[3] "UNIX Network Programming: The sockets networking API, 3/e", by W. Richard Stevens, Bill Fenner, and Andrew M. Rudoff. ISBN-10: 0131411551. ISBN-13: 978-0131411555. Addison-Wesley.
        https://learning.oreilly.com/library/view/the-sockets-networking/0131411551/

[4] "Beej's Guide to Network Programming: Using Internet Sockets", by Brian "Beej Jorgensen" Hall, independently published April 2023.
        https://beej.us/guide/bgnet/