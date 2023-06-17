#include "MainHeader.hpp"

int main(int ac, char** av) {
    if (ac != 3) {
        std::cout << "Usage: ./IRCserv <port> <password>" << std::endl;
        return 1;
    }

    std::stringstream ss(av[1]);
    int port;

    if (!(ss >> port) || ss.fail() || !(ss.eof()) || port < 5000 || port >= 65536) {
        std::cout << "Please enter a valid port number" << std::endl;
        return 1;
    }

    Server server(port);
    Poll poll;
    poll.add(server.get_fd());
    IRC(poll, server, av[2]).start_loop();
}

