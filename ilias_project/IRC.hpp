#pragma once
#include <cassert>
#include <set>
#include <string>
#include <vector>
#include "Channel.hpp"
#include "IRCExecptions.hpp"
#include "IRCReplies.hpp"
#include "Parser.hpp"
#include "socket/Client.hpp"
#include "socket/Poll.hpp"
#include "socket/Server.hpp"

#include <sstream>

class IRC {
   private:
    ChannelHandler channels;
    const std::string _password;
    Parser _parser;
    std::map<int, Client*> _clients;
    Poll _poll;
    Server _server;
    std::set<std::string> _username_pool;
    std::map<std::string, Client*> _nickname_pool;

   public:
    IRC(Poll& poll, Server& server, std::string password);
    void handle_new_connection();
    void handle_client(int fd);
    void exec_command(Client& client, const Parser::Command& command);
    void start_loop();
    void broadcastMessage(const Client& client,
                          const std::string& channelName,
                          const std::string& sender,
                          const std::string& message,
                          const std::vector<std::string>& users);

    void authentification(Client& client, const Parser::Command& cmd);
    void quit(Client& client);
    void mode(std::vector<std::string> args, Client& client);
    void join(Client& client, const Parser::Command& cmd);
    void privMessage(Client& client, const Parser::Command& cmd);
    void topic(Client& client, const Parser::Command& cmd);
    void part(const Parser::Command& cmd, Client& client);
    void invite(const Parser::Command& cmd, Client& client);
    void kick(const Parser::Command& cmd, Client& client);
    void names(std::string const& mychannel, Client& client);
    void names(Client& client);
};

std::string api_res(std::string str);
