In this module we'll cover the HTTP server architecture.

HTTP Web Server features:
    *  Supports multiple simultaneous connections from multiple clients.
    *  Each received HTTP request needs to be parsed.
    *  Server needs to reply wiht the proper HTTP response; should include the file if possible.

Require reading material:
•	[1] Lewis Van Winkle, “Hands-On Network Programming with C". Packt Publishing. May 2019. ISBN: 9781789349863. https://learning.oreilly.com/library/view/hands-on-network-programming/9781789349863/
    o	Read chapter 7.


Similar to 6.2, Work on the example from the textbook, re-writing it to remove cross-platform support and learning about HTTP servers as we go through it. I recommend researching the HTTP server protocol portion of the RFC and other resources as you go through it. 

If you'd like to follow along my version, go to the code directory in the module's folder and open both web_server.c and web_server.h. I 
    web_server.h: contains the logic for the supported responses, content delivery, serving resources, and handling client connections.
    web_server.c: contains the main function and server loop. 

*The file netlib.h contains utility functions I use to avoid re-writing unecessary code as well as helper functions for all the different applications. 

