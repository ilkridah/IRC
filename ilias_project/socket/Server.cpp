// #pragma once
// #include <string>

// class Server
// {
// private:
//     int _fd;
//     int _port;
//     int _pass
// public:
//     Server(int port);
//     int accept() const;
//     int get_fd() const;
//     int get_port() const;
// };

#include "Server.hpp"

Server::Server(int port) : _port(port) {
    // Create a socket and bind it to a local address
    _fd = socket(AF_INET, SOCK_STREAM, 0);
    if (_fd < 0) {
        std::cerr << "Error creating socket" << std::endl;
        exit(1);
    }
    int yes = 1;
    setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(port);
    if (bind(_fd, (struct sockaddr*)&server_address, sizeof(server_address)) <
        0) {
        std::cerr << "Error binding socket" << std::endl;
        exit(1);
    }
    // Listen for incoming connections
    listen(_fd, 5);
    std::cout << "Server was binded with port : " << _port << " and fd " << _fd
              << std::endl;
}

int Server::accept() const {
    // Accept a new connection
    struct sockaddr_in client_address;
    socklen_t client_address_size = sizeof(client_address);
    memset(&client_address, 0, sizeof(client_address));
    int client_socket =
        ::accept(_fd, (struct sockaddr*)&client_address, &client_address_size);
    if (client_socket < 0) {
        std::cerr << "Error accepting connection" << std::endl;
        exit(1);
    }
    std::cout << "Accepted connection from "
              << inet_ntoa(client_address.sin_addr) << std::endl;
    return client_socket;
}

int Server::get_fd() const { 
    return _fd;
}
int Server::get_port() const {
    return _port;
}

Server::~Server() {
    close(_fd);
}