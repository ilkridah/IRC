#include "IRC.hpp"
#include "IRCExecptions.hpp"

void IRC::mode(std::vector<std::string> args, Client& client) {
    std::string admin = client.get_nick();
    if (args[0][0] == '#') {
        if (channels.is_admin(args[0], admin)) {
            if (args[1] == "+o" && args.size() == 3)
                channels.set_is_admin(args[0], args[2], true);
            else if (args[1] == "-o" && args.size() == 3)
                channels.set_is_admin(args[0], args[2], false);
            else if (args[1] == "+k" && args.size() == 3) {
                channels.set_key(args[0], args[2]);
            } else if (args[1] == "-k" && args.size() == 2)
                channels.unset_key(args[0]);
            else if (args[1] == "+i" && args.size() == 2)
                channels.set_invite(args[0],client.get_nick());
            else if (args[1] == "-i" && args.size() == 2)
                channels.unset_invite(args[0],client.get_nick());
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
    } else {
        if (client.get_nick() == args[0]) {
            if (args[1] == "+i")
                client.set_is_invisible();
            else if (args[1] == "-i")
                client.unset_is_invisible();
            else
                throw IRCException::ERR_UMODEUNKNOWNFLAG();
        } else
            throw IRCException::ERR_NOSUCHNICK(args[0]);
    }
    client.send(IRCReplies::RPL_mode(args[1], args[0]));
}

void IRC::join(Client& client, const Parser::Command& cmd) {
    std::string pass;
    if (cmd.args[0] == "0") {
        channels.remove_user(client.get_nick());
    }
    for (size_t i = 0; i < cmd.chan_key.size(); i++) {
        channels.add_user(cmd.chan_key[i].first, client.get_nick(),
                          cmd.chan_key[i].second);
        if (!channels.gimmi_topic(cmd.chan_key[i].first).empty())
            IRCReplies::RPL_TOPIC(client,
                                  channels.get_channel(cmd.chan_key[i].first));
        IRCReplies::RPL_NAMREPLY(client, cmd.chan_key[i].first, channels);
    }
}

void IRC::privMessage(Client& client, const Parser::Command& cmd) {
    std::string msg = cmd.args.back();
    for (size_t i = 0; i < cmd.args.size() - 1; i++) {
        if (cmd.args[i][0] == '#') {
            std::pair<bool, std::vector<std::string> > res =
                channels.get_users(cmd.args[i]);
            if (res.first) {
                std::vector<std::string> const& users = res.second;
                broadcastMessage(client, cmd.args[i],
                                 ":" + client.get_nick() + "!~" +
                                     client.get_user() + "@" +
                                     client.get_local_host() + " PRIVMSG ",
                                 msg, users);
            } else
                throw IRCException::ERR_CANNOTSENDTOCHAN(cmd.args[0]);
        } else {
            std::map<std::string, Client*>::iterator it =
                _nickname_pool.find(cmd.args[i]);
            if (it != _nickname_pool.end()) {
                Client& target = *it->second;
                target.send(":" + client.get_nick() + "!~" + client.get_user() +
                            "@" + client.get_local_host() + " PRIVMSG " +
                            cmd.args[i] + " :" + msg + "\r\n");
            } else
                throw IRCException::ERR_NOSUCHNICK(cmd.args[i]);
        }
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
        IRCReplies::RPL_NOTOPIC(client, channels.get_channel(cmd.args[0]));
    else
        IRCReplies::RPL_TOPIC(client, channels.get_channel(cmd.args[0]));
}

void IRC::part(const Parser::Command& cmd, Client& client) {
    std::string reason = "Leaving";
    std::vector<std::string> tmp;
    tmp = cmd.args;
    std::vector<std::string>::iterator it = tmp.begin();
    for (; it != tmp.end(); it++) {
        if (channels.does_channel_exist(*it) == true) {
            std::pair<bool, std::vector<std::string> > Guser =
                channels.get_users(*it);
            if (!channels.is_member(*it, client.get_nick()))
                throw IRCException::ERR_NOSUCHNICK(client.get_nick());
            channels.remove_user(*it, client.get_nick());
            channels.remove_channel(*it, client.get_nick());

            for (size_t i = 0; i < Guser.second.size(); i++)
                _nickname_pool[Guser.second[i]]->send(
                    ":" + client.get_nick() + "!" + client.get_user() + "@" +
                    client.get_local_host() + " PART " + *it + " :" + reason +
                    "\r\n");
        } else
            throw IRCException::ERR_NOSUCHCHANNEL(*it);
    }
}

void IRC::invite(const Parser::Command& cmd, Client& client) {
    std::map<std::string, Client*>::iterator it =
        _nickname_pool.find(cmd.args[0]);
    if (!channels.does_channel_exist(cmd.args[1]))
        throw IRCException::ERR_NOSUCHCHANNEL(cmd.args[1]);
    if (channels.is_member(cmd.args[1], cmd.args[0]) == true)
        throw IRCException::ERR_USERONCHANNEL(cmd.args[0], cmd.args[1]);
    if (it != _nickname_pool.end()) {
        _nickname_pool[cmd.args[0]]->send(
            ":" + client.get_nick() + "!" + client.get_user() + "@" +
            client.get_local_host() + " INVITE " + cmd.args[0] + " " +
            cmd.args[1] + "\r\n");
        IRCReplies::RPL_INVITING(client, cmd.args[1], cmd.args[0]);
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
            if (!channels.is_member(cmd.chan_key[i].first,
                                    cmd.chan_key[i].second))
                throw IRCException::ERR_NOSUCHNICK(cmd.chan_key[i].second);
            if (_nickname_pool[cmd.chan_key[i].second] !=
                _nickname_pool[client.get_nick()])
                channels.remove_user(cmd.chan_key[i].first,
                                     cmd.chan_key[i].second);
            else
                throw IRCException::ERR_SELFKICK(client.get_nick());
            _nickname_pool[cmd.chan_key[i].second]->send(
                ":" + client.get_nick() + "!" + client.get_user() + "@" +
                client.get_local_host() + " KICK " + cmd.chan_key[i].first +
                " " + cmd.chan_key[i].second + " :" + reason + "\r\n");
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
                    if (!_nickname_pool[users[i]]->get_is_invisible()) {
                        if (channels.is_admin(mychannel, users[i]) == 1)
                            msg += "@";
                        msg += users[i];
                        if (i != users.size() - 1)
                            msg += " ";
                    }
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
                    if (!_nickname_pool[tmp1.second[j]]->get_is_invisible()) {
                        if (channels.is_admin(tmp.second[i], tmp1.second[j]))
                            msg += "@";
                        msg += tmp1.second[j] + " ";
                    }
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
