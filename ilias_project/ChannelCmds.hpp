#include "Channel.hpp"
#include "IRCExecptions.hpp"

using std::string;

class ChannelCmds
{
    ChannelHandler &_channel_handler;

public:
    ChannelCmds(ChannelHandler &handler) : _channel_handler(handler){};
    void Joining_Channel(const string &Channelname, const string &user, const string &key);

    void Leaving_Channel(const string &Channelname, const string &user);

    void check_channel(string Channelname, string user);

    void check_admin(string Channelname, string admin);

    void set_restrictedTopic(string Channelname);

    void remove_restrictedTopic(string Channelname);

    bool is_channel_admin(string Channelname, string user);

    void add_channel_admin(string Channelname, string user);

    void add_channel_guest(std::string Channelname, std::string user);

    void remove_channel_admin(string Channelname,
                              string user);
};