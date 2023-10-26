# Simple Distributed Cache System

This is a simple distributed cache system for UESTC distributed system course homework.

I finished it in my spare time in one week so it's very simple and just supports some very simple functions. It supports simple key-value access and can be runned on distributed system with several servers.

I used `HTTP` and `RPC` protocol to implement it, and found two libraries in github, they are **cpp-httplib** and **gRPC**. Their address is here:\
<https://github.com/yhirose/cpp-httplib>\
<https://github.com/grpc/grpc>

If you want to build it from source code, your system must be `ubuntu20.04` and two libraries mentioned above must be installed on your system first. You can find some help from their github page about how to install them, and this page about **gRPC** may be helpful:\
<https://grpc.io/docs/languages/cpp/quickstart>

In the code I wrote, I assumed that the entire distributed system had only three servers with IP addresses of `172.18.0.2`, `172.18.0.3` and `172.18.0.4` , so you must modify the code in the file named `main.cpp` to fit your own configuration before you build it. You have to modify a global variable called `NUM_OF_SERVER` which represents the number of servers, and another global hash structure variable, `IP_OF_SERVERS`, which stores the mapping of server names to IP addresses. Finally, the `HTTP` service listens on port 2000 and the `RPC` service listens on port 3000 in my code, again you can modify them.

I also offer the binary files and packages in the  **release** page if you don't want to build it by yourself. But you must ensure that your server configuration is the same as mine, otherwise it may not run correctly. I used `CMake` to build the project so you can see `CMakeLists.txt` in the directory at each level.

If you have any questions or suggestions, feel free to contact me.
