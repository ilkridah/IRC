#pragma once

#include <string>
#include "./socket/Client.hpp"

namespace IRCReplay {
    void RPL_TOPIC(Client& client, Channel const& channel);
    void RPL_NOTOPIC(Client& client, const Channel channel);
    void RPL_NAMREPLY(Client& client,
                      std::string const& channelname,
                      ChannelHandler& channels);
    std::string RPL_mode(std::string mode, std::string channelname);
    void RPL_INVITING(Client& client,
                             std::string channelname,
                             std::string guest);
    void RPL_WELCOME(Client& client, std::string nick);
    void RPL_PASSNOTICE(Client& client);
}