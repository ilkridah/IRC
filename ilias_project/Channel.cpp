#include "Channel.hpp"


ChannelHandler::ChannelHandler() : _user_channels(), _channel_users(){};

std::map<std::string, std::vector<std::string> > const&
ChannelHandler::get_users() {
    return _user_channels;
}

std::map<std::string, std::vector<std::string> > const&
ChannelHandler::get_channels() {
    return _channel_users;
}

Channel& ChannelHandler::get_channel(std::string const& channel_name) {
    std::map<std::string, Channel>::iterator it = _channels.find(channel_name);
    if (it == _channels.end())
        throw IRCException::ERR_NOSUCHCHANNEL(channel_name);
    return it->second;  // what should we do?
}

std::pair<bool, std::vector<std::string> > ChannelHandler::get_users(
    std::string const& channel_name) {
    std::map<std::string, std::vector<std::string> >::iterator users_iter =
        this->_channel_users.find(channel_name);
    if (users_iter == this->_channel_users.end()) {
        return std::make_pair(false, std::vector<std::string>());
    }
    return std::make_pair(true, users_iter->second);
};

std::pair<bool, std::vector<std::string> > ChannelHandler::get_channels(
    std::string const& user_nickname) {
    std::map<std::string, std::vector<std::string> >::iterator channels_iter =
        this->_user_channels.find(user_nickname);

    if (channels_iter == this->_user_channels.end()) {
        return std::make_pair(false, std::vector<std::string>());
    }

    return std::make_pair(true, channels_iter->second);
};

bool ChannelHandler::add_user(std::string const& channel_name,
                              std::string const& user_name,
                              std::string const& password) {
    if (!does_channel_exist(channel_name)) {
        _channels.insert(
            std::make_pair(channel_name, Channel(channel_name, password)));
        _channel_users[channel_name].push_back(user_name);
        _user_channels[user_name].push_back(channel_name);
        _is_admin[std::make_pair(channel_name, user_name)] = true;
        return true;
    }

    std::map<std::string, Channel>::iterator it = _channels.find(channel_name);
    Channel& channel = it->second;

    if (channel.password != password) {
        std::cout << "----" << channel.password << "----" << std::endl;
        std::cout << "----" << password << "----" << std::endl;
        throw IRCException::ERR_BADCHANNELKEY(channel_name);
    }

    if (is_member(channel_name, user_name))
        throw IRCException::ERR_USERONCHANNEL(user_name, channel_name);

    if (channel.InviteOnly) {
        if (!is_invited(channel_name, user_name))
            throw IRCException::ERR_INVITEONLYCHAN(channel_name);
        if (channel.limit != 0 &&
            _channel_users[channel_name].size() + 1 > channel.limit)
            throw IRCException::ERR_CHANNELISFULL(channel_name);
    } else if (channel.limit != 0 &&
               _channel_users[channel_name].size() + 1 > channel.limit) {
        throw IRCException::ERR_CHANNELISFULL(channel_name);
    }

    // Add the user to the channel
    _channel_users[channel_name].push_back(user_name);
    _user_channels[user_name].push_back(channel_name);
    _is_admin[std::make_pair(channel_name, user_name)] = false;

    return true;
}

void ChannelHandler::remove_user(std::string const& channel,
                                 std::string const& user) {
    std::vector<std::string>& users = this->_channel_users[channel];
    std::vector<std::string>::iterator found =
        std::find(users.begin(), users.end(), user);
    if (found == users.end())
        return;
    users.erase(found);
};

void ChannelHandler::remove_user(std::string const& user) {
    for (std::map<std::string, std::vector<std::string> >::iterator it =
             this->_channel_users.begin();
         it != this->_channel_users.end(); ++it) {
        this->remove_user(it->first, user);
    };
    this->_user_channels[user].clear();
};

void ChannelHandler::remove_channel(std::string const& channel) {
    this->_channel_users.erase(channel);
};

bool ChannelHandler::is_admin(std::string channel, std::string user) {
    return _is_admin[std::make_pair(channel, user)];
}

bool ChannelHandler::set_is_admin(std::string channel,
                                  std::string user,
                                  bool admin) {
    if (!is_member(channel, user))
        throw IRCException::ERR_USERNOTINCHANNEL(user, channel);
    return _is_admin[std::make_pair(channel, user)] = admin;
}

bool ChannelHandler::is_invited(std::string channel, std::string user) {
    return _is_invited[std::make_pair(channel, user)];
}

bool ChannelHandler::set_is_invited(std::string channel,
                                    std::string user,
                                    bool invited) {
    return _is_invited[std::make_pair(channel, user)] = invited;
}

std::vector<std::string> ChannelHandler::list_users() {
    std::vector<std::string> users;
    for (std::map<std::string, std::vector<std::string> >::iterator it =
             _user_channels.begin();
         it != _user_channels.end(); ++it) {
        users.push_back(it->first);
    }
    return users;
};

std::vector<std::string> ChannelHandler::list_channels() {
    std::vector<std::string> channels;
    for (std::map<std::string, std::vector<std::string> >::iterator it =
             _channel_users.begin();
         it != _channel_users.end(); ++it) {
        channels.push_back(it->first);
    }
    return channels;
};

bool ChannelHandler::does_channel_exist(std::string const& channel_name) const {
    std::map<std::string, std::vector<std::string> >::const_iterator it =
        _channel_users.find(channel_name);

    if (it != _channel_users.end())
        return true;
    return false;
};

bool ChannelHandler::does_user_exist(std::string const& channel_name) const {
    std::map<std::string, std::vector<std::string> >::const_iterator it =
        _user_channels.find(channel_name);

    if (it != _user_channels.end())
        return true;
    return false;
};

bool ChannelHandler::is_member(const std::string& channel,
                               const std::string& user) {
    std::map<std::string, std::vector<std::string> >::iterator res =
        _channel_users.find(channel);
    if (res == _channel_users.end()) {
        throw IRCException::ERR_USERSDONTMATCH();
    };
    std::vector<std::string> users = res->second;
    for (std::vector<std::string>::iterator it = users.begin();
         it != users.end(); ++it) {
        if (*it == user)
            return true;
    }
    return false;
}

void ChannelHandler::set_key(std::string const& channel_name,
                             std::string const& key) {
    std::map<std::string, Channel>::iterator it = _channels.find(channel_name);
    if (it != _channels.end())
        it->second.password = key;
}
void ChannelHandler::unset_key(std::string& channel_name) {
    std::map<std::string, Channel>::iterator it = _channels.find(channel_name);
    if (it != _channels.end()) {
        it->second.password.erase();
    }
}

void ChannelHandler::set_invite(std::string const& channel_name) {
    if (!does_channel_exist(channel_name))
        throw IRCException::ERR_NOSUCHCHANNEL(channel_name);
    if (is_admin(channel_name, _user_channels[channel_name][0]))
        throw IRCException::ERR_CHANOPRIVSNEEDED(channel_name);
    std::map<std::string, Channel>::iterator it = _channels.find(channel_name);
    if (it != _channels.end())
        it->second.InviteOnly = true;
}
void ChannelHandler::unset_invite(std::string const& channel_name) {
    std::map<std::string, Channel>::iterator it = _channels.find(channel_name);
    if (it != _channels.end())
        it->second.InviteOnly = false;
}
bool ChannelHandler::set_limit(std::string const& channel_name,
                               std::string const& limit) {
    std::map<std::string, Channel>::iterator it = _channels.find(channel_name);
    if (it != _channels.end()) {
        if (limit.find_first_not_of("0123456789") == std::string::npos) {
            std::stringstream ss;
            ss << limit;
            ss >> it->second.limit;
            return true;
        }
    }
    return false;
}

void ChannelHandler::unset_limit(std::string const& channel_name) {
    std::map<std::string, Channel>::iterator it = _channels.find(channel_name);
    if (it != _channels.end())
        it->second.limit = 0;
    // should we throw an exception if channel didnt exist?
}

bool ChannelHandler::set_res_topic(std::string const& channel_name) {
    std::map<std::string, Channel>::iterator it = _channels.find(channel_name);
    if (it != _channels.end()) {
        it->second.rest_topic = true;
        return true;
    }
    return false;
}
void ChannelHandler::unset_res_topic(std::string const& channel_name) {
    std::map<std::string, Channel>::iterator it = _channels.find(channel_name);
    if (it != _channels.end())
        it->second.rest_topic = false;
}

std::string ChannelHandler::gimmi_topic(std::string const& channel_name) {
    std::map<std::string, Channel>::iterator it = _channels.find(channel_name);
    if (it != _channels.end())
        return it->second.topic;
    return "";
}

void ChannelHandler::set_topic(std::string const& channel_name,
                               std::string const& topic) {
    std::map<std::string, Channel>::iterator it = _channels.find(channel_name);
    if (it != _channels.end()) {
        it->second.topic = topic;
    } else
        throw IRCException::ERR_NOSUCHCHANNEL(channel_name);
}

void ChannelHandler::unset_topic(std::string const& channel_name) {
    std::map<std::string, Channel>::iterator it = _channels.find(channel_name);
    if (it != _channels.end())
        it->second.topic = "";
    else
        throw IRCException::ERR_NOSUCHCHANNEL(channel_name);
}