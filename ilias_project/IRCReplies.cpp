#include "IRCReplies.hpp"

namespace IRCReplies {

void RPL_TOPIC(Client& client, Channel const& channel) {
    client.send(":" + client.get_nick() + "!" + client.get_user() + "@" +
                client.get_local_host() + " TOPIC " + channel.name + " :" +
                channel.topic + "\r\n");
}

void RPL_NOTOPIC(Client& client, const Channel channel) {
    client.send(channel.name + ": No topic is set\r\n");
}

void RPL_NAMREPLY(Client& client,
                  std::string const& channelname,
                  ChannelHandler& channels) {
    std::pair<bool, std::vector<std::string> > userlist_res =
        channels.get_users(channelname);

    if (!userlist_res.first) {
        throw std::runtime_error("Channel not found");
    } else {
        std::vector<std::string> const& userlist = userlist_res.second;
        client.send(":" + client.get_nick() + "!" + client.get_user() + "@" +
                    client.get_local_host() + " JOIN :" + channelname + "\r\n");
        client.send(":irc.1337.com 353 " + client.get_nick() + " = " +
                    channelname + " :");

        for (size_t i = 0; i < userlist.size(); i++) {
            if (channels.is_admin(channelname, userlist[i])) {
                client.send("@");
            }
            client.send(userlist[i] + " ");
        }
    }

    client.send("\r\n");
    client.send(":irc.example.com 366 " + client.get_nick() + " " +
                channelname + " :End of /NAMES list.\r\n");
}

std::string RPL_mode(std::string mode, std::string channelname) {
    return ("mode " + mode + " executed @ " + channelname + "\r\n");
}

void RPL_INVITING(Client& client, std::string channelname, std::string guest) {
    client.send(":" + client.get_nick() + "!" + client.get_user() + "@" +
                client.get_local_host() + " " + channelname + " " + guest +
                "\r\n");
}

void RPL_WELCOME(Client& client, std::string nick) {
    client.send(":irc.1337.com 001 " + client.get_nick() +
                " :Welcome to Our IRC Server!\r\n"
                "If you need any help, just ask.\r\n Have a great time! /~ " +
                nick + " ~/\r\n");
}

void RPL_PASSNOTICE(Client& client) {
    client.send("Please enter your password to connect to the server!!\r\n");
}


}  // namespace IRCReplies
