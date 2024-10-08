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
    bool _is_invisible;
    std::vector<std::string> myChannels;

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
    void set_is_invisible() { _is_invisible = true; }
    void unset_is_invisible() { _is_invisible = false; }
    bool get_is_invisible() const { return _is_invisible; }
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
        size_t _send;
        if((_send = ::send(_fd, buffer.c_str(), buffer.size(),0) <= 0)){
            std::cerr << "Error to send" << std::endl;
            exit(1);}
        else 
            return _send;
    }
    std::string get_user() const { return _user_name; }
    std::string get_local_host() const { return _local_host; }
    std::string get_host() const { return _user_host; }


    std::string get_local_host();
    ~Client();
};