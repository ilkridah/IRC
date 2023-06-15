#include "Poll.hpp"


Poll::Poll() : _poll_fds() {}

void Poll::add(int socket) {
    pollfd socket_poll_fd;
    bzero(&socket_poll_fd, sizeof(pollfd));
    socket_poll_fd.fd = socket;
    socket_poll_fd.events = POLLIN;
    _poll_fds.push_back(socket_poll_fd);
}

void Poll::remove(int socket) {
    for (size_t i = 0; i < _poll_fds.size(); i++) {
        if (_poll_fds[i].fd == socket) {
            _poll_fds.erase(_poll_fds.begin() + i);
            return;
        }
    }
    std::cout << "failed to remove from Poll()" << std::endl;
    exit(1);
};

int Poll::get_event() {
    int num_fds_ready = poll(_poll_fds.data(), _poll_fds.size(), -1);

    if (num_fds_ready <= 0) {
        std::cerr << "Error in poll()" << std::endl;
        exit(1);
    }

    for (size_t i = 0; i < _poll_fds.size(); i++) {
        if (_poll_fds[i].revents & POLLIN) {
            return _poll_fds[i].fd;
        }
    }
    std::cout << "No event after poll()" << std::endl;
    exit(1);
}

std::vector<int> Poll::get_events() {
    std::vector<int> events;
    int num_fds_ready = poll(_poll_fds.data(), _poll_fds.size(), -1);

    if (num_fds_ready <= 0) {
        std::cerr << "Error in poll()" << std::endl;
        exit(1);
    }

    for (size_t i = 0; i < _poll_fds.size(); i++) {
        if (_poll_fds[i].revents & POLLIN) {
            events.push_back(_poll_fds[i].fd);
        }
    }

    return events;
};
