#pragma once
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <poll.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <vector>

class Server {
   protected:
    int _fd;
    int _port;

   public:
    Server(int port);
    int accept() const;
    int get_fd() const;
    int get_port() const;
    ~Server();
};