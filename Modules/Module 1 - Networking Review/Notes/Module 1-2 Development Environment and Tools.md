Module 1.2 – Development Environment Setup and Tools.
In this module we’ll walk through setting up our Linux VM and installing the necessary tools and dependencies. 
You should know how to install  the required software or Google the necessary steps if required. 
Deploying the VM.
It is recommended you work on a virtual machine; I’ve used Kali since it comes pre-packaged with all the necessary networking tools.
You can use either VMWare Workstation Player (free version)
https://www.vmware.com/products/workstation-player/workstation-player-evaluation.html
or VirtualBox
https://www.virtualbox.org/wiki/Downloads
Kali VM (pick the image matching your Hypervisor/virtualization program)
https://www.kali.org/get-kali/#kali-virtual-machines
Kali.org has documentation on how to Insall the VM image on different Virtual Machine solutions: https://www.kali.org/docs/virtualization/

Setting up the C compiler on Linux.
Read Appendix C- Setting Up your C compiler on Linux. https://learning.oreilly.com/library/view/hands-on-network-programming/9781789349863/221eebc0-0bb1-4661-a5aa-eafed9fcba7e.xhtml
Lewis Van Winkle, “Hands-On Network Programming with C". Packt Publishing. May 2019. ISBN: 9781789349863. https://learning.oreilly.com/library/view/hands-on-network-programming/9781789349863/
Wireshark (Installed by default on the Kali VM).
https://www.wireshark.org/download.html
We'll be using Wireshark to analyze the networked applications we write.
Wireshark is a network packet analyzer. A network packet analyzer presents captured packet data in as much detail as possible. You could think of a network packet analyzer as a measuring device for examining what’s happening inside a network cable, just like an electrician uses a voltmeter for examining what’s happening inside an electric cable (but at a higher level, of course).
        1.1.1. Some intended purposes
        Here are some reasons people use Wireshark:
        * Network administrators use it to troubleshoot network problems
        * Network security engineers use it to examine security problems
        * QA engineers use it to verify network applications
        * Developers use it to debug protocol implementations
        * People use it to learn network protocol internals
        * Wireshark can also be helpful in many other situations.


        * Download Page: https://www.wireshark.org/download.html
        * User guide: https://www.wireshark.org/docs/wsug_html/
        * Introduction: https://www.wireshark.org/docs/wsug_html_chunked/ChapterIntroduction.html#ChIntroWhatIs
Netcat
We'll use netcat to act as a simple client or server (TCP or UDP) for various examples. It's normally installed by default on Linux; if not already installed, you can install it with the following command (requires root privileges)
 # apt-get install ncat
Other tools:
Traceroute, ss, etc.
