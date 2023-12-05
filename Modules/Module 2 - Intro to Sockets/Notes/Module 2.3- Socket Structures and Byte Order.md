
Goals for this Module (Week 2):

    Write a simple TCP server and explain each piece. We want to disect each socket function.

    Explain each socket function.
    Explain Socket structures.
    Data representation for network programming:
        *Byte order
    * Basic TCP client and Server.
    
    * Overview IPv4 and IPv6 and ports.

    *Introduction to what socket programming is, provide an overview of what we aim to accomplish, etc.

    *Code layout for the semester.

    *Environment setup.
        Code layout
        

Required Reading (see the Books and resources section for links):
    
    [1] "Hacking The Art of Exploitation
        Chapter 4, section 0x420. pages 198 - 203.

    [2] "Hands-On Network Programming with C"
        Chapter 2. Pages: 40- 52

    [3] https://beej.us/guide/bgnet/ 
        Chapter 3 and 4.
    
    If you want a more in-depth explanation, read:
        [4] "UNIX Network Programming: The sockets networking API, 3/e"
        This book is a bit dated but it does provide a more in-depth explanation for some of the functions and structures. Not required to read.

Introduction:
In this section we cover the following topics:
    * Sockets
    * Socket functions
      * Socket functions and 3-way handshake mapping / OSI layer
        * Unix Network Programming  2.6
    * Socket structures
    * Byte order: How data is represented across a network of similar and different architectures
    * Client-Server communication model
      * * Unix Network Programming  2.9



***********************************************************************************************************************************

Chapter 5 - DNS from Handson programming, covers these in great detail

1. Write a small TCP client / server (might require two programs).
2. We'll dissect each as a study case of how the API structures work
	
Structures:
	sockaddr_in
		
	addrinfo

socket API and addrinfo
	
	showip program is a good intro to the structures: sockaddr_in, addrinfo
	
	1. Write the program
	2. explain each structure.
	
//Beej's	
Let's examine showip.c

	//https://man7.org/linux/man-pages/man3/getaddrinfo.3.html
    
	/*

       int getaddrinfo(
        const char *restrict node,                  // remote address. e.g. "google.com" or 1.1.1.1
        const char *restrict service,               // remote port or service name : 80 or http.
        const struct addrinfo *restrict hints,      //  A structure of addrinfo type filled out with relevant info.
                                                    //  That is, the type of data we wish for getaddrinfo() to return.
                                                    // We fill this structure with the data we are requesting prior to our call to 
                                                    // getaddrinfo().

        struct addrinfo **restrict res);            //A struct addrinfo that will hold the results from getaddrinfo().
    */
	
Before we can understand what these structures are and how they are used it helps to understand what this function is doing.
    getaddrinfo():
        This function will return information on a particular host name t(such as is IP address) and load up a struct sockaddr for you, taking care of the gritty details ( whether it is IPv4 or IPv6). It replaces the old functions gethostbyname() and getservbyname().

        In other words, the purpose of this function is to populate an addrinfo structure with the requested information. It knows what information to request from a remote system by using a structure of type addrinfo as its hint structure (which we populate). All that means is that we are pre-poluating a template structure with the type of data we want, getaddrinfo() uses our template structure and requests that data from the remote host.

        We'll revist this function more in detail later but I've commented each parameter with enough details to help us understand how it's used.
        
        //https://man7.org/linux/man-pages/man3/getaddrinfo.3.html
        
        /*
        int getaddrinfo(
            const char *restrict node,                  // remote address. e.g. "google.com" or 1.1.1.1
            const char *restrict service,               // remote port or service name : 80 or http.
            
            const struct addrinfo *restrict hints,      //  A structure of addrinfo type filled in with the relevant info.
                                                        //  That is, the type of data we wish for getaddrinfo() to return.
                                                        // We fill this structure with the data we are requesting prior to our call to 
                                                        // getaddrinfo().

            struct addrinfo **restrict res);            // A struct addrinfo (a pointer to a pointer) that will hold the results from getaddrinfo().
                                                        // this structure is empty and will be used by the function to copy the values returned by
                                                        // the remote system.
        */
        
   * In legacy code, getaddrinfo() was not available and you will see programmers manually initialize addrinfo.


With that in mind, let's explore our first structure, addrinfo.
Chapter 3.2 from Unix Network Programming has a great explanation for each of these structures.
Chapter 3.3 From Beej's guide goes over each of these structures.


    struct addrinfo:
        * This structure is a more recent invention, and is used to prep the socket address structures for subsequent use. It’s also used in host name lookups, and service name lookups: getaddrinfo() and getnameinfo().
	
	
 We can inspect the structure's definition by browsing to /usr/include/netdb.h on a modern Linux system.

    /* Structure to contain information about address of a service provider.  */

        struct addrinfo
        {
            int ai_flags;			    /* Input flags.  */
            int ai_family;		        /* Protocol family for socket.  */ (IPv4 or IPv6 or AF_UNSPEC)
            int ai_socktype;		    /* Socket type.  */
            int ai_protocol;		    /* Protocol for socket.  */
            socklen_t ai_addrlen;		/* Length of socket address.  */
            struct sockaddr *ai_addr;	/* Socket address for socket.  */
            char *ai_canonname;		    /* Canonical name for service location.  */
            struct addrinfo *ai_next;	/* Pointer to next in list.  */ 

                    //linked list pointing to next element, that is a call to getaddrinfo can return a list
                    //of multiple addresses, each referenced by a separate addrinfo struct
        };

    The following pages contain all the technical details for the structure and each field.
    https://linux.die.net/man/3/getaddrinfo
    https://man7.org/linux/man-pages/man3/getaddrinfo.3.html

	
###
        struct addrinfo 
        {
            int ai_flags;               // AI_PASSIVE, AI_CANONNAME, etc.
            int ai_family;              // AF_INET, AF_INET6, AF_UNSPEC
            int ai_socktype;            // SOCK_STREAM, SOCK_DGRAM
            int ai_protocol;            // use 0 for "any"
            size_t ai_addrlen;          // size of ai_addr in bytes
            struct sockaddr *ai_addr;   // struct sockaddr_in or _in6
            char *ai_canonname;         // full canonical hostname
            struct addrinfo *ai_next;   // linked list, next node
        };

            
            * AI_FLAGS:    

            * AI_FAMILY: The internet_address protocol: IPv4 or IPv6 or AF_UNSPEC

            * ai_next: A linked list that points to the next element (that is if the returned 
                structure from getaddrinfo contains more than one server info structure. 
                This is common if the server is listening on multiple interfaces.)

            * sockaddr ai_addr
              * struct sockaddr holds socket address information for many types of sockets.
###

A socket address structure is always passed by reference when passed asn an argument to any socket function.
Any socket function taking a pointer to these structures must deal with socket address structures from any of the supported protocol families.


struct sockaddr{ ... }

        * This structure describes a generic socket address and holds socket address information for many types of sockets.
  

    sockaddr definition:
        struct sockaddr 
        {
            unsigned short sa_family; // address family, AF_xxx
            char sa_data[14]; // 14 bytes of protocol address
        };

            * sa_family is usually AF_INET (IPv4) or AF_INET6 (IPv6).
            * sa contains destination address and port for the socket.
    
As you have realized by now, manually packing this structure can be tedious and error-prone. Programmers created a parallel data structure called struct sockaddr_in("in" stands for internet) to be used with IPv4. 

This is an IPv4 socket address structure, commmolly called an "Internet socket address structure".

        struct sockaddr_in 
        {
            short int sin_family;           // Address family, AF_INET
            unsigned short int sin_port;    // Port number
            struct in_addr sin_addr;        // Internet address
            unsigned char sin_zero[8];      // Same size as struct sockaddr
        }; 

            * As you can easily observe, sockaddr_in makes it easy to reference elements of the socket address.
            * It is important to note that sockaddr_in and sockaddr can be casted between one type or the other.
            * This allows you to use them interchangably when using the connect function, as long as they're properly casted.
            * sin_zero: It's used for padding to maintain size compability with sockaddr and should be set to zero.
            
        // (IPv4 only--see struct in6_addr for IPv6)
        // Internet address (a structure for historical reasons)
        

        struct in_addr
        {
            uint32_t s_addr; // that's a 32-bit int (4 bytes)
        };

        //IPv6 structures

        // (IPv6 only--see struct sockaddr_in and struct in_addr for IPv4)
        struct sockaddr_in6 
        {
            u_int16_t sin6_family; // address family, AF_INET6
            u_int16_t sin6_port; // port number, Network Byte Order
            u_int32_t sin6_flowinfo; // IPv6 flow information
            struct in6_addr sin6_addr; // IPv6 address
            u_int32_t sin6_scope_id; // Scope ID
        };
        
        struct in6_addr 
        {
            unsigned char s6_addr[16]; // IPv6 address
        };
    
Lastly, we cover 
    struct sockaddr_storage:
        * It is designed to be large enough to hold both IPv4 and IPv6 structures.
        * For some calls, sometimes you don’t know in advance if it’s going to fill out your struct sockaddr with an IPv4 or IPv6 address. So you pass in this parallel structure very similar to struct sockaddr except larger, and then cast it to the type you need:
       
       struct sockaddr_storage
       {
           sa_family_t ss_family;           // address family.
        
            // all this is padding, implementation specific, ignore it.
           char __ss_pad1[_SS_PAD1SIZE];
           int64_t __ss_align;
           char __ss_pad2[_SS_PAD2SIZE];
       };
