#include <sys/_types/_size_t.h>
#include <sys/qos.h>
#include <sys/socket.h>
#include <climits>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <stdexcept>
#include <string>
#include <vector>
#include "Channel.hpp"
#include "IRC.hpp"
#include "IRCExecptions.hpp"
#include "IRCReplies.hpp"
#include "Parser.hpp"
#include "socket/Client.hpp"
#include "socket/Poll.hpp"

void IRC::mode(std::vector<std::string> args, Client& client) {
    std::string admin = client.get_nick();
    if (channels.is_admin(args[0], admin)) {
        if (args[1] == "+o" && args.size() == 3)
            channels.set_is_admin(args[0], args[2], true);
        else if (args[1] == "-o" && args.size() == 3)
            channels.set_is_admin(args[0], args[2], false);
        else if (args[1] == "+k" && args.size() == 3) {
            channels.set_key(args[0], args[2]);
        } else if (args[1] == "-k" && args.size() == 2)
            channels.set_key(args[0], "");
        else if (args[1] == "+i" && args.size() == 2)
            channels.set_invite(args[0]);
        else if (args[1] == "-i" && args.size() == 2)
            channels.unset_invite(args[0]);
        else if (args[1] == "+l" && args.size() == 3) {
            if (!channels.set_limit(args[0], args[2]))
                throw ::IRCException::ERR_UNKNOWNMODE(args[1]);
        } else if (args[1] == "-l" && args.size() == 2)
            channels.unset_limit(args[0]);
        else if (args[1] == "+t" && args.size() == 2)
            channels.set_res_topic(args[0]);
        else if (args[1] == "-t" && args.size() == 2)
            channels.unset_res_topic(args[0]);
        else
            throw IRCException::ERR_UNKNOWNMODE(args[1]);
    } else
        throw IRCException::ERR_CHANOPRIVSNEEDED(args[0]);
    client.send(IRCReplay::RPL_mode(args[1], args[0]));
}

void IRC::join(Client& client, const Parser::Command& cmd) {
    std::string pass;
    if (cmd.args[0] == "0") {
        channels.remove_user(client.get_nick());
    }
    for (size_t i = 0; i < cmd.chan_key.size(); i++) {
        if (cmd.args.size() < 2)
            pass = "";
        else
            pass = cmd.args[1];
        channels.add_user(cmd.chan_key[i].first, client.get_nick(), pass);
        if (!channels.gimmi_topic(cmd.chan_key[i].first).empty())
            IRCReplay::RPL_TOPIC(client,
                                 channels.get_channel(cmd.chan_key[i].first));
        IRCReplay::RPL_NAMREPLY(client, cmd.chan_key[i].first, channels);
    }
}

void IRC::privMessage(Client& client, const Parser::Command& cmd) {
    std::string msg = cmd.args.back();
    if (cmd.args.size() > 2)
        throw IRCException::ERR_TOOMANYTARGETS("targets");
    if (cmd.args[0][0] == '#') {
        std::pair<bool, std::vector<std::string> > res =
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

void IRC::topic(Client& client, const Parser::Command& cmd) {
    if (channels.does_channel_exist(cmd.args[0]) == false) {
        throw IRCException::ERR_NOTONCHANNEL(cmd.args[0]);
    }
    if (cmd.args.size() > 1) {
        if (channels.get_channel(cmd.args[0]).rest_topic == true)
            if (channels.is_admin(cmd.args[0], client.get_nick()) == true)
                channels.set_topic(cmd.args[0], cmd.args[1]);
            else
                throw IRCException::ERR_CHANOPRIVSNEEDED(cmd.args[0]);
        else
            channels.set_topic(cmd.args[0], cmd.args[1]);
    }
    if (channels.gimmi_topic(cmd.args[0]).empty())
        IRCReplay::RPL_NOTOPIC(client, channels.get_channel(cmd.args[0]));
    else
        IRCReplay::RPL_TOPIC(client, channels.get_channel(cmd.args[0]));
}

void IRC::part(std::string const& mychannel, Client& client) {
    std::string reason = "Leaving";
    if (channels.does_channel_exist(mychannel) == true) {
        std::vector<std::string> chans = spliter(mychannel, ',');
        std::vector<std::string>::iterator it = chans.begin();
        for (; it != chans.end(); it++) {
            channels.remove_user(*it, client.get_nick());
            client.send(":" + client.get_nick() + "!" + client.get_user() +
                        "@" + client.get_local_host() + " PART " + *it + " :" +
                        reason + "\r\n");
        }
    } else
        throw IRCException::ERR_NOSUCHCHANNEL(mychannel);
}

void IRC::invite(const Parser::Command& cmd, Client& client) {
    std::map<std::string, Client*>::iterator it =
        _nickname_pool.find(cmd.args[0]);
    if (it != _nickname_pool.end()) {
        _nickname_pool[cmd.args[0]]->send(
            ":" + client.get_nick() + "!" + client.get_user() + "@" +
            client.get_local_host() + " INVITE " + cmd.args[0] + " " +
            cmd.args[1] + "\r\n");
        IRCReplay::RPL_INVITING(client, cmd.args[1], cmd.args[0]);
        channels.set_is_invited(cmd.args[1], cmd.args[0], true);
    } else
        throw IRCException::ERR_NOSUCHNICK(cmd.args[0]);
}

void IRC::kick(const Parser::Command& cmd, Client& client) {
    std::string reason = client.get_nick();
    if (cmd.args.size() == 3)
        reason = cmd.args[2];
    for (size_t i = 0; i < cmd.chan_key.size(); i++) {
        if (channels.is_admin(cmd.chan_key[i].first, client.get_nick()) == 1) {
            channels.remove_user(
                cmd.chan_key[i].first,
                _nickname_pool[cmd.chan_key[i].second]->get_nick());
            channels.remove_user(cmd.chan_key[i].first, cmd.chan_key[i].second);
            _nickname_pool[cmd.chan_key[i].second]->send(
                ":" + client.get_nick() + "!" + client.get_user() + "@" +
                client.get_local_host() + " KICK " + cmd.chan_key[i].first +
                " " + cmd.chan_key[i].second + " :" + reason + "\r\n");

            std::pair<bool, std::vector<std::string> > usersMap =
                channels.get_users(cmd.chan_key[i].first);

            for (size_t x = 0; x < usersMap.second.size(); x++) {
                _nickname_pool[usersMap.second[x]]->send(
                    ":" + client.get_nick() + "!" + client.get_user() + "@" +
                    client.get_local_host() + " KICK " + cmd.chan_key[i].first +
                    " " + cmd.chan_key[i].second + " :" + reason + "\r\n");
            }
        } else {
            throw IRCException::ERR_CHANOPRIVSNEEDED(cmd.chan_key[i].first);
        }
    }
}

void IRC::names(std::string const& mychannel, Client& client) {
    if (mychannel[0] == '#') {
        std::pair<bool, std::vector<std::string> > usersMap;
        std::vector<std::string> chans = spliter(mychannel, ',');
        for (size_t j = 0; j < chans.size(); j++) {
            usersMap = channels.get_users(chans[j]);
            if (usersMap.first) {
                std::vector<std::string> const& users = usersMap.second;
                std::string msg = ":irc.1337.com 353 " + client.get_nick() +
                                  " = " + mychannel + " :";
                for (size_t i = 0; i < users.size(); i++) {
                    if (channels.is_admin(mychannel, users[i]) == 1)
                        msg += "@";
                    msg += users[i];
                    if (i != users.size() - 1)
                        msg += " ";
                }
                msg += "\r\n";
                client.send(msg);
                client.send(":irc.1337.com 366 " +
                            channels.list_users().back() + " " + mychannel +
                            " :End of NAMES list\r\n");
            } else
                throw IRCException::ERR_NOSUCHNICK(mychannel);
        }
    } else
        throw IRCException::ERR_NOSUCHCHANNEL(mychannel);
}
//     if(channels.does_user_exist(mychannel) == true)
//     {
//         std::pair<bool, std::vector<std::string> > chanMap =
//             channels.get_channels(client.get_nick());
//         std::pair<bool, std::vector<std::string> > usersMap =
//         channels.get_users(mychannel);
//         for (size_t i=0;i<chanMap.second.size();i++)
//             std::cout << chanMap.second[i] << " ";
//         std::cout << std::endl;
//         if (chanMap.first && usersMap.first) {
//             std::vector<std::string> const& chans = chanMap.second;
//             std::string msg = ":irc.1337.com 353 " + client.get_nick() + " =
//             " + " :"; for (size_t i = 0; i < chans.size(); i++) {
//                 msg += chans[i];
//                 if (i != chans.size() - 1)
//                     msg += " ";
//             }
//             client.send(msg + "\r\n");
//             client.send(":irc.1337.com 366 " +
//             channels.list_channels().back() + " " + mychannel + " :End of
//             NAMES list\r\n");
//         } else
//             throw IRCException::ERR_NOSUCHNICK(mychannel);
//     }
// }

void IRC::names(Client& client) {
    std::pair<bool, std::vector<std::string> > tmp;
    tmp = channels.get_channels(client.get_nick());
    if (tmp.first) {
        std::string msg;
        for (size_t i = 0; i < tmp.second.size(); i++) {
            msg += ":irc.1337.com 353 " + client.get_nick() + " = " +
                   tmp.second[i] + " :";
            std::pair<bool, std::vector<std::string> > tmp1 =
                channels.get_users(tmp.second[i]);
            if (tmp1.first) {
                for (size_t j = 0; j < tmp1.second.size(); j++) {
                    if (channels.is_admin(tmp.second[i],tmp1.second[j]))
                        msg += "@" ;
                    msg += tmp1.second[j] + " ";
                }
            }
            msg += "\r\n";
            client.send(msg);
            msg.erase();
        }
    }
    client.send(":irc.1337.com 366 " + client.get_nick() +
                " :End of NAMES list\r\n");
}
