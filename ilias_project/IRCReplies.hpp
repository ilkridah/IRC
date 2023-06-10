#ifndef IRCREPLIES_HPP
#define IRCREPLIES_HPP

#include <cstdio>
#include <cstdlib>
#include <stdexcept>
#include <string>
#include <vector>
#include "Channel.hpp"
#include "IRC.hpp"
#include "socket/Client.hpp"

namespace IRCReplay {
inline void RPL_TOPIC(Client& client, Channel const& channel) {
    client.send( ":" + client.get_nick() + "!" + client.get_user() + "@" +
                        client.get_local_host() + " TOPIC " + channel.name +
                        " :" + channel.topic + "\r\n");
}

inline void RPL_NOTOPIC(Client& client, const Channel channel) {
    client.send( channel.name +
                        ":"
                        " No topic is set\r\n");
}

inline void RPL_NAMREPLY(Client& client,
                  std::string const& channelname,
                  ChannelHandler& channels) {
    std::pair<bool, std::vector<std::string> > userlist_res =
        channels.get_users(channelname);

    // std::cout << "/ " << userlist_res.second[0] << "/" << std::endl;
    if (!userlist_res.first)
        throw std::runtime_error("Channel not found");
    else {
        std::vector<std::string> const& userlist = userlist_res.second;
        // std::cout << "**" << userlist_res.second[0] << "**" << std::endl
        client.send(":" + client.get_nick() + "!" + client.get_user() + "@" +
                            client.get_local_host() + " JOIN :" + channelname +
                            "\r\n");
        client.send( ":irc.1337.com 353 " + client.get_nick() + " = " +
                            channelname + " :");
        for (size_t i = 0; i < userlist.size(); i++) {
            std::cout << userlist_res.second[i] << channels.is_admin(channelname, userlist[i]) << std::endl;
        //     exit(0);
            std::cout << "["<<(channels.is_admin(channelname, userlist[i])) << "]" << std::endl;
            if (channels.is_admin(channelname, userlist[i])){
                puts("is it admin?");
                client.send( "@");
            }
            // else
            //     client.send( "+");

            client.send(userlist[i] + " ");
        }
    }
    client.send( "\r\n");
    client.send( ":irc.example.com 366 " + client.get_nick() + " " +
                        channelname + " :End of /NAMES list.\r\n");
}

inline std::string RPL_mode(std::string mode, std::string channelname) {
    return ("mode " + mode + " excuted @ " + channelname + "\r\n");
};

inline void RPL_INVITING(Client& client,
                         std::string channelname,
                         std::string guest) {
    client.send( ":" + client.get_nick() + "!" + client.get_user() + "@" +
                        client.get_local_host() + " " + channelname + " " +
                        guest + "\r\n");
};

inline void RPL_WELCOME(Client& client, std::string nick) {
    client.send(
            ":irc.1337.com 001 " + client.get_nick() +
                " :Welcome to Our IRC Server!\r\n"
                "If you need any help, just ask.\r\n Have a great time! /~ " +
                nick + " ~/" + "\r\n");
};
inline void RPL_PASSNOTICE(Client& client) {
    client.send(
            "Please enter your password to connect to the server !!\r\n");
}

};  // namespace IRCReplay
#endif