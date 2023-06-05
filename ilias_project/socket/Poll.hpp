#pragma once
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <poll.h>
#include <stdlib.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#include <map>
#include <queue>
#include <string>
#include <vector>
#include "Client.hpp"
#include "Server.hpp"

class Poll {
   private:
    std::vector<pollfd> _poll_fds;

   public:
    Poll();
    void add(int socket);
    void remove(int socket);
    int get_event();
    std::vector<int> get_events();
};