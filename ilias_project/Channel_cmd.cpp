#include <string>
#include "./ChannelCmds.hpp"

using std::string;

void ChannelCmds::Joining_Channel(const string& Channelname, const string& user, const string& key)
{
    // std::string userkey = k;
    Channel& channel = _channel_handler.get_channel(Channelname);
    if (channel.InviteOnly)
    {
        if (_channel_handler.is_invited(Channelname, user) == false)
            throw IRCException::ERR_INVITEONLYCHAN(Channelname);
    }
    if (_channel_handler.is_member(Channelname, user))
        throw IRCException::ERR_USERONCHANNEL(Channelname, user);
    if (channel.limit != 0) {
        if (_channel_handler.get_users(Channelname).second.size() == channel.limit) {
            throw IRCException::ERR_CHANNELISFULL(Channelname);
        }
    }
    if (_channel_handler.add_user(Channelname, user, key) == false) {
        throw IRCException::ERR_BADCHANNELKEY(Channelname);
    } 
}

void ChannelCmds::Leaving_Channel(const string& Channelname, const string& user)
{
    // check_channel(Channelname, user);
   if( _channel_handler.does_channel_exist(Channelname))
        throw IRCException::ERR_NOSUCHCHANNEL(Channelname);
    // std::vector<string>::iterator it = std::find(Chlist[Channelname]->users.begin(), Chlist[Channelname]->users.end(), user);
    if (_channel_handler.is_member(Channelname, user))
        throw IRCException::ERR_NOTONCHANNEL(Channelname);
    else
        _channel_handler.remove_user(Channelname, user);
    // if (is_channel_admin(Channelname, user))
    // {
    //     std::set<string>::iterator iter = Chlist[Channelname]->admins.find(user);
    //     if (iter != Chlist[Channelname]->admins.end())
    //         Chlist[Channelname]->admins.erase(iter);
    //     if (Chlist[Channelname]->admins.size()== 0 && Chlist[Channelname]->users.size() != 0)
    //          Chlist[Channelname]->admins.insert(Chlist[Channelname]->users[0]);
    // }
}

// void ChannelCmds::check_channel(string Channelname, string user)
// {
//     if (is_channel_existing(Channelname) == 0)
//         throw IRCException::ERR_NOTONCHANNEL(Channelname);
//     if (is_channel_member(Channelname, user) == 0)
//         throw IRCException::ERR_USERSDONTMATCH();
// }

// void ChannelCmds::check_admin(string Channelname, string admin)
// {
//     if (is_channel_existing(Channelname) == 0)
//         throw IRCException::ERR_NOTONCHANNEL(Channelname);
//     if (is_channel_member(Channelname, admin) == 0)
//         throw IRCException::ERR_USERSDONTMATCH();
//     if (is_channel_admin(Channelname, admin) == 0)
//         throw IRCException::ERR_USERSDONTMATCH();
// }

// void ChannelCmds::set_restrictedTopic(string Channelname) 
// {
//     Chlist[Channelname]->restrictedTopic = true;
// }

// void ChannelCmds::remove_restrictedTopic(string Channelname)
// {
//     Chlist[Channelname]->restrictedTopic = false;
// }

// bool ChannelCmds::is_channel_admin(string Channelname, string user) {  // chan exist
//     std::set<string>::iterator it = Chlist[Channelname]->admins.find(user);
//     if (it != Chlist[Channelname]->admins.end())
//         return 1;
//     return 0;
// }

// void ChannelCmds::add_channel_admin(string Channelname, string user) {
//     Chlist[Channelname]->admins.insert(user);
// }

// void ChannelCmds::add_channel_guest(std::string Channelname, std::string user)
// {
//     Chlist[Channelname]->guest.insert(user);
// }

// void ChannelCmds::remove_channel_admin(string Channelname,
//                                            string user) {
//     std::set<string>::iterator it = Chlist[Channelname]->admins.find(user);
//     if (it != Chlist[Channelname]->admins.end())
//         Chlist[Channelname]->admins.erase(it);
// }

bool isStringDigits(const string& str) {
    for (int i = 0; str[i]; i++) {
        if (!std::isdigit(str[i])) {
            return false;
        }
    }
    return true;
}
