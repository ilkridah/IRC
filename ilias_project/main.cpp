#include <iostream>
#include "Channel.hpp"
#include "IRC.hpp"

int main(int ac, char** av) {
    if (ac != 3) {
        std::cout << "Usage: ./IRCserv <port> <password>" << std::endl;
        return 1;
    }
    int port = atoi(av[1]);

    Server server(port);
    Poll poll;
    poll.add(server.get_fd());
    IRC(poll, server, av[2]).start_loop();
}
