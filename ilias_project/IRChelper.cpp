#include <sys/_types/_size_t.h>
#include <sys/qos.h>
#include <sys/socket.h>
#include <climits>
#include <cstdlib>
#include <exception>
#include <stdexcept>
#include <string>
#include "Channel.hpp"
#include "IRC.hpp"
#include "IRCExecptions.hpp"
#include "IRCReplies.hpp"
#include "Parser.hpp"
#include "socket/Client.hpp"
#include "socket/Poll.hpp"

void IRC::mode(std::vector<std::string> args, Client& client) {
    std::string admin = client.get_nick();
    if (channels.is_admin(args[0], admin)){
    if (args[1] == "+o" && args.size() == 3)
        channels.set_is_admin(args[0], args[2], true);
    else if (args[1] == "-o" && args.size() == 3)
        channels.set_is_admin(args[0], args[2],false);
    else if (args[1] == "+k" && args.size() == 3) {
        channels.set_key(args[0], args[2]);
    } else if (args[1] == "-k" && args.size() == 2)
        channels.set_key(args[0],"");
    else if (args[1] == "+i" && args.size() == 2)
        channels.set_invite(args[0]);
    else if (args[1] == "-i" && args.size() == 2)
        channels.unset_invite(args[0]);
    else if (args[1] == "+l" && args.size() == 3){
        if (!channels.set_limit(args[0], args[2]))
            throw::IRCException::ERR_UNKNOWNMODE(args[1]);
    }
    else if (args[1] == "-l" && args.size() == 2)
        channels.unset_limit(args[0]);
    else if (args[1] == "+t" && args.size() == 2)
        channels.set_res_topic(args[0]);
    else if (args[1] == "-t" && args.size() == 2)
        channels.unset_res_topic(args[0]);
    else
        throw IRCException::ERR_UNKNOWNMODE(args[1]);}
    else 
        throw IRCException::ERR_CHANOPRIVSNEEDED(args[0]);
    ft_send(client, IRCReplay::RPL_mode(args[1], args[0]));
}

void IRC::join(Client& client, const Parser::Command& cmd) {
    if (cmd.args[0] == "0") {
        channels.remove_user(client.get_nick());
        std::vector<std::string> mychannels = client.get_myChannsList();
        for (size_t i = 0; i < mychannels.size(); i++)
            part(mychannels, client);
        return;
    }
    for (size_t i = 0; i < cmd.chan_key.size(); i++) {
        channels.add_user(cmd.args[0], client.get_nick(), cmd.chan_key[i].second) ;
            if (!channels.get_topic(cmd.chan_key[i].first).empty())
                IRCReplay::RPL_TOPIC(
                    client, channels.get_channel(cmd.chan_key[i].first));
            IRCReplay::RPL_NAMREPLY(client, cmd.chan_key[i].first, channels);
            client.add_myChannsList(cmd.chan_key[i].first);
        }
    }
}

void IRC::privMessage(Client& client, const Parser::Command& cmd) {
    std::string msg = cmd.args.back();
    if (cmd.args.size() > 2)
        throw IRCException::ERR_TOOMANYTARGETS("targets");
    if (cmd.args[0][0] == '#') {
        std::pair<bool, std::vector<std::string> const&> res =
            channels.get_users(cmd.args[0]);
        if (res.first) {
            std::vector<std::string> const& users = res.second;
            broadcastMessage(cmd.args[0],
                             ":" + client.get_nick() + "!~" +
                                 client.get_user() + "@" +
                                 client.get_local_host() + " PRIVMSG ",
                             msg, users);
        };
    } else {
        std::map<std::string, Client*>::iterator it =
            _nickname_pool.find(cmd.args[0]);
        if (it != _nickname_pool.end()) {
            Client& target = *it->second;
            target.send(":" + client.get_nick() + "!~" + client.get_user() +
                        "@" + client.get_local_host() + " PRIVMSG " +
                        cmd.args[0] + " :" + msg + "\r\n");
        } else
            throw IRCException::ERR_NOSUCHNICK(cmd.args[0]);
    }
}

void IRC::quit(Client& client) {
    _poll.remove(client.get_fd());
    _clients.erase(client.get_fd());
    if (client.is_nick_set())
        _nickname_pool.erase(client.get_nick());
    if (client.is_user_set())
        _username_pool.erase(client.get_user());
    channels.remove_user(client.get_nick());
    delete &client;
}

// void IRC::topic(Client& client, const Parser::Command& cmd) {
//     if (!channels.is_channel_existing(cmd.args[0]))
//         throw IRCException::ERR_NOTONCHANNEL(cmd.args[0]);
//     if (cmd.args.size() > 1)
//         channels.set_topic(cmd.args[1], cmd.args[0], client.get_nick());
//     if (channels.get_topic(cmd.args[0]).empty())
//         IRCReplay::RPL_NOTOPIC(client, channels.get_Channel(cmd.args[0]));
//     else
//         IRCReplay::RPL_TOPIC(client, channels.get_Channel(cmd.args[0]));
// }

// void IRC::part(std::vector<std::string> mychannel, Client& client) {
//     std::vector<std::string>::iterator it = mychannel.begin();
//     while (it != mychannel.end()) {
//         channels.Leaving_Channel(*it, client.get_nick());
//         client.remove_channel(*it);
//         it++;
//     }
// }

void IRC::invite(const Parser::Command& cmd, Client& client) {
    // channels.is_admin(cmd.args[1], client.get_nick());
    
    std::map<std::string, Client*>::iterator it =
        _nickname_pool.find(cmd.args[0]);
    if (it != _nickname_pool.end()) {
        _nickname_pool[cmd.args[0]]->send(
            ":" + client.get_nick() + "!" + client.get_user() + "@" +
            client.get_local_host() + " INVITE " + cmd.args[0] + " " +
            cmd.args[1] + "\r\n");
        IRCReplay::RPL_INVITING(client, cmd.args[1], cmd.args[0]);
        channels.add_channel_guest(cmd.args[1], cmd.args[0]);
    } else
        throw IRCException::ERR_NOSUCHNICK(cmd.args[0]);
}

void IRC::kick(const Parser::Command& cmd, Client& client) {
    std::string reason = client.get_nick();
    if (cmd.args.size() == 3)
        reason = cmd.args[2];
    for (size_t i = 0; i < cmd.chan_key.size(); i++) {
        channels.check_admin(cmd.chan_key[i].first, client.get_nick());
        channels.Leaving_Channel(
            cmd.chan_key[i].first,
            _nickname_pool[cmd.chan_key[i].second]->get_nick());
        _nickname_pool[cmd.chan_key[i].second]->remove_channel(
            cmd.chan_key[i].first);
        ft_send(*_nickname_pool[cmd.chan_key[i].second],
                ":" + client.get_nick() + "!" + client.get_user() + "@" +
                    client.get_local_host() + " KICK " + cmd.chan_key[i].first +
                    " " + cmd.chan_key[i].second + " :" + reason + "\r\n");
        std::vector<std::string> userList =
            this->channels.get_Users(cmd.chan_key[i].first);
        for (size_t j = 0; j < userList.size(); j++)
            ft_send(*_nickname_pool[userList[j]],
                    ":" + client.get_nick() + "!" + client.get_user() + "@" +
                        client.get_local_host() + " KICK " +
                        cmd.chan_key[i].first + " " + cmd.chan_key[i].second +
                        " :" + reason + "\r\n");
    }
}

void ft_send(Client& client, std::vector<std::string> msg) {
    for (size_t i = 0; i < msg.size(); i++)
        client.send(msg[i] + " ");
    client.send("\n");
}
