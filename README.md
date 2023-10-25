# Simple Distributed Cache System

This is a simple distributed cache system for UESTC distributed system course homework.

I finished it in my spare time in one week so it's very simple and just supports some very simple functions. It supports simple key-value access, you can run it on distributed system with several server.

I used `http` and `rpc` protocol to implement it, and found two libraries in github, they are **cpp-httplib** and **gRPC**. Their address is here:
<https://github.com/yhirose/cpp-httplib>\
<https://github.com/grpc/grpc>

If you want to run it, your system must be `ubuntu20.04` and two libraries mentioned above must be installed on your system first. You can find some help from their github page about how to install them, and this page about gRPC may be helpful:\
<https://grpc.io/docs/languages/cpp/quickstart>

I used `CMake` to build the project so you can see `CMakeLists.txt` in the directory at each level. If you don't want to build it by yourself, I also offer binary files and packages in the **release** directory.

If you have any questions or suggestions, feel free to contact me.
