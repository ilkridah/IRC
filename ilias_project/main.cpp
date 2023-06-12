#include <iostream>
#include "Channel.hpp"
#include "IRC.hpp"

int main(int ac, char** av) {
    if (ac != 3) {
        std::cout << "Usage: ./IRCserv <port> <password>" << std::endl;
        return 1;
    }
    std::stringstream ss;
    ss << av[1];
    if (ss.str().find_first_not_of("0123456789") != std::string::npos)
    {
        std::cout << "Please enter a valid port number1" << std::endl;
        return 1;
    } 
    else {
        int port;
        ss >> port;
        if (port < 5000) {
            std::cout << "Please enter a valid port number2" << std::endl;
            return 1;
        }
        Server server(port);
        Poll poll;
        poll.add(server.get_fd());
        IRC(poll, server, av[2]).start_loop();
    }
}
