#include "Client.hpp"
#include <sys/_types/_size_t.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cassert>
#include <iostream>
#include "Poll.hpp"

Client::Client(int fd)
    : _fd(fd), _buffer(), _nick_name(), _is_pass_valid(){};

int Client::read_cmd() {
    char buff[512] = {0};
    int size = 0;
    if ((size = ::recv(_fd, buff, 512, 0)) == 0) {
        shutdown(_fd, 0);
        std::cout << "Client disconnected" << std::endl;
        return 0;
    }
    buff[size] = '\0';
    _buffer.append(std::string(buff));
    return 1;
};

bool Client::cmd_is_ready() const {
    return _buffer.find("\r\n") != std::string::npos || _buffer.size() >= 512;
};

std::string Client::get_buffer() const {
    size_t found = _buffer.find("\r\n");
    if (found != std::string::npos) {
        return _buffer.substr(0, std::max(static_cast<size_t>(512), found));
    }
    if (_buffer.size() >= 512)
        return _buffer.substr(0, 512);
    std::cout << ("get_buffer when not ready !") << std::endl;
    exit(1);
    return _buffer;
};

void Client::empty_buffer() {
    _buffer = "";
};

Client::~Client() {
    shutdown(_fd, 0);
    close(_fd);
}

int Client::get_fd() const {
    return _fd;
}


// std::vector<std::string> Client::get_myChannsList() {
//     return myChannels;
// }

void Client::remove_channel(std::string const& Channelname)
{
   std::vector<std::string>::iterator it = find(myChannels.begin(), myChannels.end(), Channelname);
   if (it != myChannels.end())
        myChannels.erase(it);
    else
        throw IRCException::ERR_NOTONCHANNEL(Channelname);
}

std::string Client::get_local_host() {
    char hostname[256];
    if (gethostname(hostname, sizeof(hostname)) == 0)
        return std::string(hostname);
    else 
        return std::string(" Error getting hostname.");
}