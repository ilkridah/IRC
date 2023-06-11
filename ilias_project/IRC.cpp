#include "IRC.hpp"
#include <sys/_types/_size_t.h>
#include <sys/qos.h>
#include <sys/socket.h>
#include <climits>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include "IRCReplies.hpp"
#include "Parser.hpp"
#include "socket/Client.hpp"
#include "socket/Poll.hpp"

IRC::IRC(Poll& poll, Server& server, std::string password)
    : _password(password),
      _parser(),
      _clients(),
      _poll(poll),
      _server(server){};

void IRC::handle_new_connection() {
    std::cout << "New client connected" << std::endl;
    Client* client = new Client(_server.accept());
    _poll.add(client->get_fd());
    _clients[client->get_fd()] = client;
};

std::string remove_crlf(const std::string& str) {
    size_t pos = 0;
    if ((pos = str.find("\r\n")) != std::string::npos) {
        return str.substr(0, pos);
    }
    return str;
}

void IRC::handle_client(int fd) {
    Client& client = *_clients[fd];
    if (client.read_cmd() == 0) {
        _poll.remove(fd);
        _nickname_pool.erase(client.get_nick());
        _username_pool.erase(client.get_user());
        _clients.erase(client.get_fd());
        delete &client;
    } else if (client.cmd_is_ready()) {
        std::string buffer = client.get_buffer();
        client.empty_buffer();
        Parser::Command cmd = _parser(remove_crlf(buffer));
        if (cmd.command == Parser::NOT_FOUND &&
            cmd.ath == Parser::NOT_REGISTRED)
            return;
        authentification(client, cmd);
        if (!client.is_registered() && client.is_pass_valid() &&
            client.is_nick_set() && client.is_user_set()) {
            client.set_registered();
            IRCReplay::RPL_WELCOME(client, client.get_local_host());
        }
        if (cmd.ath == Parser::NOT_REGISTRED)
            exec_command(client, cmd);
    }
};

void IRC::start_loop() {
    while (1337) {
        int event = _poll.get_event();
        if (event == _server.get_fd()) {
            handle_new_connection();
        } else {
            try {
                handle_client(event);
            } catch (std::exception& e) {
                _clients[event]->send(e.what());
            }
        }
    }
};
 


void IRC::exec_command(Client& client, const Parser::Command& cmd) {
    if (!client.is_registered() && cmd.command != Parser::QUIT)
        throw IRCException::ERR_NOTREGISTERED();
    switch (cmd.command) {
        case Parser::QUIT:
            quit(client);
            break;
        case Parser::PRIVMSG:
            privMessage(client ,cmd);
            break;
        case Parser::JOIN:
            join(client, cmd);
            break;
        case Parser::MODE:
            mode(cmd.args, client);
            break;
        case Parser::TOPIC:
            topic(client, cmd);
            break;
        case Parser::PART:
            part(cmd.args[0], client);
            break;
        case Parser::KICK:
            kick(cmd ,client);
            break;
        case Parser::INVITE:
            invite(cmd, client);
            break;
        case Parser::NAMES:
            names(cmd.args[0], client);
            break;
        case Parser::WEATHER:
            client.send(api_res(cmd.args[0]));
            break;
        case Parser::NOT_FOUND:
            throw IRCException::ERR_UNKNOWNCOMMAND("");
            break;
    }
}

void IRC::broadcastMessage(const std::string& channelName,  const std::string& sender,
                           const std::string& message, const std::vector<std::string> & users) {
    for (size_t i = 0; i < users.size(); i++) {
        if(_nickname_pool[sender] != _nickname_pool[users[i]])
            _nickname_pool[users[i]]->send(sender + channelName + " :" + message + "\r\n");
    }
}
