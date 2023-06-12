#pragma once
#include <i386/types.h>
#include <sys/_types/_size_t.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cassert>
#include <iostream>
#include <ostream>
#include <string>
#include <vector>
#include "../Channel.hpp"
#include "../IRCExecptions.hpp"
#include "Client.hpp"
#include "Poll.hpp"

#define MAX_CHANNEL 20

class Client {
    int _fd;
    std::string _buffer;
    std::string _nick_name;
    std::string _user_name;
    std::string _user_host;
    std::string _local_host;
    std::string _user_serv;
    std::string _user_real;
    bool _is_pass_valid;
    bool _is_registered;
    std::vector<std::string> myChannels;
    // int _max_channel;

   public:
    Client(int fd);
    int read_cmd();
    bool cmd_is_ready() const;
    std::string get_buffer() const;
    void empty_buffer();
    int get_fd() const;
    bool is_pass_valid() const { return _is_pass_valid; }
    void set_pass_valid() { _is_pass_valid = true; }
    void set_registered() { _is_registered = true; }
    bool is_nick_set() const { return !_nick_name.empty(); }
    bool is_registered() const { return _is_registered; }
    void set_nick(std::string nick) { _nick_name = nick; }
    std::string get_nick() const { return _nick_name; }

    bool is_user_set() const { return !_user_name.empty(); }
    void set_user(std::string user,
                  std::string hostname,
                  std::string servername,
                  std::string realname) {
        _user_name = user;
        _user_host = hostname;
        _user_serv = servername;
        _user_real = realname;
    }
    void set_local_host(std::string local_host) {
        _local_host = local_host;
    }
    size_t send(std::string buffer) {
        int _send = ::send(_fd, buffer.c_str(), buffer.size(), 0);
        if(_send < 0){
            std::cerr << "Error to send" << std::endl; exit(1);}
        else 
            return _send;
    }
    std::string get_user() const { return _user_name; }
    std::string get_local_host() const { return _local_host; }
    std::string get_host() const { return _user_host; }

    void remove_channel(std::string const& Channelname);
    std::vector<std::string> get_myChannels() const { return myChannels; }
    std::string get_local_host();
    ~Client();
};