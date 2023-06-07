#ifndef IRCREPLIES_HPP
#define IRCREPLIES_HPP

#include <stdexcept>
#include <string>
#include <vector>
#include "Channel.hpp"
#include "IRC.hpp"
#include "socket/Client.hpp"

namespace IRCReplay {
void RPL_TOPIC(Client& client, Channel const& channel) {
    ft_send(client, ":" + client.get_nick() + "!" + client.get_user() + "@" +
                        client.get_local_host() + " TOPIC " + channel.name +
                        " :" + channel.topic + "\r\n");
}

void RPL_NOTOPIC(Client& client, const Channel channel) {
    ft_send(client, channel.name +
                        ":"
                        " No topic is set\r\n");
}

void RPL_NAMREPLY(Client& client,
                  std::string const& channelname,
                  ChannelHandler& channels) {
    std::pair<bool, std::vector<std::string> const&> userlist_res =
        channels.get_users(channelname);
    if (!userlist_res.first)
        throw std::runtime_error("Channel not found");
    std::vector<std::string> const& userlist = userlist_res.second;

    ft_send(client, +":" + client.get_nick() + "!" + client.get_user() + "@" +
                        client.get_local_host() + " JOIN :" + channelname +
                        "\r\n");
    ft_send(client, ":irc.1337.com 353 " + client.get_nick() + " = " +
                        channelname + " :");
    for (size_t i = 0; i < userlist.size(); i++) {
        if (channels.is_admin(channelname, userlist[i]))
            ft_send(client, "@");
        // else
        //     ft_send(client, "+");
        ft_send(client, userlist[i] + " ");
    }
    ft_send(client, "\r\n");
    ft_send(client, ":irc.example.com 366 " + client.get_nick() + " " +
                        channelname + " :End of /NAMES list.\r\n");
}

inline std::string RPL_mode(std::string mode, std::string channelname) {
    return ("mode " + mode + " excuted @ " + channelname + "\r\n");
};

inline void RPL_INVITING(Client& client,
                         std::string channelname,
                         std::string guest) {
    ft_send(client, ":" + client.get_nick() + "!" + client.get_user() + "@" +
                        client.get_local_host() + " " + channelname + " " +
                        guest + "\r\n");
};

inline void RPL_WELCOME(Client& client, std::string nick) {
    ft_send(client,
            ":irc.1337.com 001 " + client.get_nick() +
                " :Welcome to Our IRC Server!\r\n"
                "If you need any help, just ask.\r\n Have a great time! /~ " +
                nick + " ~/" + "\r\n");
};
inline void RPL_PASSNOTICE(Client& client) {
    ft_send(client,
            "Please enter your password to connect to the server !!\r\n");
}

};  // namespace IRCReplay
#endif